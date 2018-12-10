/**
 * Autores: Arthur Diniz, Daniel Menescal e Elvis Ferreira
 * Universidade Federal do Rio Grande do Norte
 */
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <string>

#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"

#define MULTICAST_ADDR "225.0.0.37"
#define PORT_SCKT 1802

// defines for trains
#define NUM_TRAINS 4
#define MAX_VALUE 1.8
#define SEM_NORMAL 5
#define SEM_DANGER 2
#define MAX_SLEEP_TIME 2

using namespace BlackLib;

void *readEntries(void *arg);
void *setSpeed(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);

pthread_t thr[NUM_TRAINS];

sem_t semNormal[SEM_NORMAL], semDanger[SEM_DANGER], semAux;

float sleep_time[NUM_TRAINS];

void handleCreationThread(int res) {
    if (res) {
        printf("Error on creating thread - %d.\n", res);
        exit(-1);
    }
}

void handleJoinThread(int res) {
    if (res) {
        printf("Error on joining thread - %d.\n", res);
        exit(-1);
    }
}

void handlePriority(int res) {
    if (res) {
        printf("Error in setting thread prio - %d.\n", res);
        exit(-1);
    }
}

void handleInitSemaphore(int res) {
    if (res) {
        printf("Error on creating semaphore - %d.\n", res);
        exit(-1);
    }
}

void handleDestroySemaphore(int res) {
    if (res) {
        printf("Error on destroying semaphore - %d.\n", res);
        exit(-1);
    }
}

int main(int argc, char * argv[]) {
    pthread_t thrReadEntries, thrSetSpeed;

    // for (int i = 0; i < SEM_NORMAL; i++) {
    //     handleInitSemaphore(sem_init(&semNormal[i], 0, 1));
    // }

    // for (int i = 0; i < SEM_DANGER; i++) {
    //     handleInitSemaphore(sem_init(&semDanger[i], 0, 2));
    // }

    // handleInitSemaphore(sem_init(&semAux, 0, 3));

    handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));
    handleCreationThread(pthread_create(&thrSetSpeed, NULL, setSpeed, NULL));
    
    // handleCreationThread(pthread_create(&thr[0], NULL, handleTrain1, NULL));
    // handleCreationThread(pthread_create(&thr[1], NULL, handleTrain2, NULL));
    // handleCreationThread(pthread_create(&thr[2], NULL, handleTrain3, NULL));
    // handleCreationThread(pthread_create(&thr[3], NULL, handleTrain4, NULL));

    handleJoinThread(pthread_join(thrReadEntries, NULL));
    handleJoinThread(pthread_join(thrSetSpeed, NULL));

    // handleJoinThread(pthread_join(thr[0], NULL));
    // handleJoinThread(pthread_join(thr[1], NULL));
    // handleJoinThread(pthread_join(thr[2], NULL));
    // handleJoinThread(pthread_join(thr[3], NULL));


    // for (int i = 0; i < SEM_NORMAL; i++) {
    //     handleDestroySemaphore(sem_destroy(&semNormal[i]));
    // }

    // for (int i = 0; i < SEM_DANGER; i++) {
    //     handleDestroySemaphore(sem_destroy(&semDanger[i]));
    // }
    
    // handleDestroySemaphore(sem_destroy(&semAux));
    pthread_exit(NULL);

    return 0;
}

void *readEntries(void *arg) {
	int values[NUM_TRAINS];

	// BEAGLEBONE VARIABLES
	ADC train1(AIN0);
	ADC train2(AIN1);
	ADC train3(AIN2);
	ADC train4(AIN3);

	// THREAD SCHEDULING
	pthread_t this_thread = pthread_self();

	struct sched_param params;
	params.sched_priority = sched_get_priority_max(SCHED_RR);

	handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

	// SOCKET
	int sockfd;
	int len;
	socklen_t len_recv;
	struct sockaddr_in address;
	int result;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
	address.sin_port = htons(PORT_SCKT);
	len = sizeof(address);

	while (true) {
		values[0] = train1.getIntValue();
		values[1] = train2.getIntValue();
		values[2] = train3.getIntValue();
		values[3] = train4.getIntValue();

		sendto(sockfd, values, sizeof(values), 0, (struct sockaddr *) &address, len);

		printf("Client sent: (Train 1, Train 2, Train 3, Train 4) = (%d, %d, %d, %d)\n", values[0], values[1], values[2], values[3]);

		sleep(0.5);
	}

	close(sockfd);
}


void *setSpeed(void *arg) {
	int valueEntries[NUM_TRAINS];

	// THREAD SCHEDULING
	struct sched_param params;

	pthread_t this_thread = pthread_self();
	params.sched_priority = sched_get_priority_max(SCHED_RR) - 1;

	handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

	// SOCKET
	int server_sockfd, client_sockfd;
	size_t server_len;
	socklen_t client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	struct ip_mreq mreq;

	if ( (server_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("An error has occurred on opening socket");
		exit(1);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT_SCKT);

	server_len = sizeof(server_address);

	if ( bind(server_sockfd, (struct sockaddr *) &server_address, server_len) < 0 ) {
		perror("An error has occurend on binding");
		exit(1);
	}

	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if ( setsockopt(server_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq) ) < 0) {
		perror("setsockopt");
		exit(1);
	}

	while (true) {
		printf("Server waiting...\n");

		client_len = sizeof(client_address);

		client_len = sizeof(client_address);
		if ( recvfrom(server_sockfd, &valueEntries, sizeof(valueEntries), 0, (struct sockaddr *) &client_address, &client_len) < 0 ) {
			perror("error on RECVFROM()");
			exit(1);
		}

		printf("Server received: (Train 1, Train 2, Train 3, Train 4) = (%d, %d, %d, %d)\n", valueEntries[0], valueEntries[1], valueEntries[2], valueEntries[3]);

		for (int i = 0; i < NUM_TRAINS; i++) {
			sleep_time[i] = MAX_SLEEP_TIME / valueEntries[i] + 0.1;
		}

		sleep(0.5);
	}
}

// void *handleTrain1(void *arg) {
//     int sleepTime = MAX_SLEEP_TIME;
//     bool first = false;
    
//     BlackGPIO ledFree(GPIO_44, output);
//     BlackGPIO ledLock1(GPIO_44, output);
//     BlackGPIO ledLock2(GPIO_44, output);
//     BlackGPIO ledLock3(GPIO_44, output);

//     while (true) {
//         sleepTime = sleep_time[0];

//         if (!first) {
//             printf("Trem 1 -> cima\n");
//             ledFree.setValue(high);
//             sleep(sleepTime*2);
//         }

//         first = true; 

//         sem_wait(&semAux);
//         sem_wait(&semDanger[0]);
//         sem_wait(&semNormal[0]);
//         printf("Trem 1 -> critico 0\n");
//         ledFree.setValue(low);

//         ledLock1.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semDanger[1]);
//         sem_wait(&semNormal[1]);
//         printf("Trem 1 -> critico 1\n");
//         sem_post(&semAux);
//         sem_post(&semDanger[0]);
//         sem_post(&semNormal[0]);
//         ledLock1.setValue(low);

//         ledLock2.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semNormal[2]);
//         printf("Trem 1 -> critico 2\n");
//         sem_post(&semDanger[1]);
//         sem_post(&semNormal[1]);
//         ledLock2.setValue(low);

//         ledLock3.setValue(high);
//         sleep(sleepTime);
//         sem_post(&semNormal[2]);
//         ledLock3.setValue(low);

//         printf("Trem 1 -> cima\n");
//         ledFree.setValue(high);
//         sleep(sleepTime*3);
//     }
// }

// void *handleTrain2(void *arg) {
//     int sleepTime = MAX_SLEEP_TIME;
//     bool first = false;

//     BlackGPIO ledFree(GPIO_44, output);
//     BlackGPIO ledLock1(GPIO_44, output);
//     BlackGPIO ledLock2(GPIO_44, output);

//     while (true) {
//         sleepTime = sleep_time[1];

//         if (!first) {
//             printf("Trem 2 -> baixo\n");
//             ledFree.setValue(high);
//             sleep(sleepTime);
//         }

//         first = true;
        
//         sem_wait(&semAux);
//         sem_wait(&semDanger[0]);
//         sem_wait(&semNormal[3]);
//         printf("Trem 2 -> critico 3\n");
//         ledFree.setValue(low);

//         ledLock1.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semNormal[0]);
//         printf("Trem 2 -> critico 0\n");
//         sem_post(&semAux);
//         sem_post(&semDanger[0]);
//         sem_post(&semNormal[3]);
//         ledLock1.setValue(low);

//         ledLock2.setValue(high);
//         sleep(sleepTime);
//         sem_post(&semNormal[0]);
//         ledLock2.setValue(low);

//         printf("Trem 2 -> baixo\n");
//         ledFree.setValue(high);
//         sleep(sleepTime*2);
//     }
    
// }

// void *handleTrain3(void *arg) {
//     int sleepTime = MAX_SLEEP_TIME;

//     BlackGPIO ledFree(GPIO_44, output);
//     BlackGPIO ledLock1(GPIO_44, output);
//     BlackGPIO ledLock2(GPIO_44, output);
//     BlackGPIO ledLock3(GPIO_44, output);
    
//     while (true) {
//         sleepTime = sleep_time[2];

//         printf("Trem 3 -> baixo\n");
//         ledFree.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semAux);
//         sem_wait(&semDanger[1]);
//         sem_wait(&semNormal[4]);
//         printf("Trem 3 -> critico 4\n");
//         ledFree.setValue(low);    

//         ledLock1.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semDanger[0]);
//         sem_wait(&semNormal[1]);
//         printf("Trem 3 -> critico 1\n");
//         sem_post(&semAux);
//         sem_post(&semDanger[1]);
//         sem_post(&semNormal[4]);
//         ledLock1.setValue(low);

//         ledLock2.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semNormal[3]);
//         printf("Trem 3 -> critico 3\n");
//         sem_post(&semDanger[0]);
//         sem_post(&semNormal[1]);
//         ledLock2.setValue(low);

//         ledLock3.setValue(high);
//         sleep(sleepTime);
//         sem_post(&semNormal[3]);
//         ledLock3.setValue(low);
//     }
// }

// void *handleTrain4(void *arg) {
//     int sleepTime = MAX_SLEEP_TIME;
//     int first = false;

//     BlackGPIO ledFree(GPIO_44, output);
//     BlackGPIO ledLock1(GPIO_44, output);
//     BlackGPIO ledLock2(GPIO_44, output);

//     while (true) {
//         sleepTime = sleep_time[3];


//         if (!first) {
//             sem_wait(&semAux);
//             sem_wait(&semDanger[1]);
//             sem_wait(&semNormal[2]);
//             printf("Trem 4 -> critico 2\n");
//         }

//         first = true;
        
//         ledLock1.setValue(high);
//         sleep(sleepTime);
//         sem_wait(&semNormal[4]);
//         printf("Trem 4 -> critico 4\n");
//         sem_post(&semAux);
//         sem_post(&semDanger[1]);
//         sem_post(&semNormal[2]);
//         ledLock1.setValue(low);

//         ledLock2.setValue(high);
//         sleep(sleepTime);
//         sem_post(&semNormal[4]);
//         ledLock2.setValue(low);

//         printf("Trem 4 -> baixo\n");
//         ledFree.setValue(high);
//         sleep(sleepTime*2);
//         sem_wait(&semAux);
//         sem_wait(&semDanger[1]);
//         sem_wait(&semNormal[2]);
//         printf("Trem 4 -> critico 2\n");
//         ledFree.setValue(low);

//     }
    
// }