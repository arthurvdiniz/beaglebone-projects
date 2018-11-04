/**
 * Autores: Arthur Diniz, Daniel Menescal e Elvis Ferreira
 * Universidade Federal do Rio Grande do Norte
 */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>

#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"

#define NUM_TRAINS 4
// #define MAX_VALUE 3
#define SEM_NORMAL 5
//#define SEM_DANGER 2
#define SLEEP_TIME 1

using namespace BlackLib;


// void *readEntries(void *arg);
// void *setPriorities(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);

pthread_t thr[NUM_TRAINS];

// float valueEntries[NUM_TRAINS];

sem_t semNormal[SEM_NORMAL], semDanger;

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

// void handlePriority(int res) {
//     if (res) {
//         printf("Error in setting thread prio - %d.\n", res);
//         exit(-1);
//     }
// }

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
    void *resStatus;

    // pthread_t thrReadEntries, thrSetPriorities;

    for (int i = 0; i < SEM_NORMAL; i++) {
        handleInitSemaphore(sem_init(&semNormal[i], 0, 1));
    }

    handleInitSemaphore(sem_init(&semDanger, 0, 3));

    // handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));
    // handleCreationThread(pthread_create(&thrSetPriorities, NULL, setPriorities, (void *) 2));

    handleCreationThread(pthread_create(&thr[0], NULL, handleTrain1, (void *) 3));
    handleCreationThread(pthread_create(&thr[1], NULL, handleTrain2, (void *) 4));
    handleCreationThread(pthread_create(&thr[2], NULL, handleTrain3, (void *) 5));
    handleCreationThread(pthread_create(&thr[3], NULL, handleTrain4, (void *) 6));

    // handleJoinThread(pthread_join(thrReadEntries, &resStatus));
    // handleJoinThread(pthread_join(thrSetPriorities, &resStatus));

    handleJoinThread(pthread_join(thr[0], &resStatus));
    handleJoinThread(pthread_join(thr[1], &resStatus));
    handleJoinThread(pthread_join(thr[2], &resStatus));
    handleJoinThread(pthread_join(thr[3], &resStatus));

    pthread_exit(NULL);

    for (int i = 0; i < SEM_NORMAL; i++) {
        handleDestroySemaphore(sem_destroy(&semNormal[i]));
    }

    handleDestroySemaphore(sem_destroy(&semDanger));

    return 0;
}

// void *readEntries(void *arg) {
//     ADC train1(AIN0);
//     ADC train2(AIN1);
//     ADC train3(AIN2);
//     ADC train4(AIN3);

//     pthread_t this_thread = pthread_self();

//     struct sched_param params;
//     params.sched_priority = sched_get_priority_max(SCHED_RR);

//     handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

//     while (true) {
//         valueEntries[0] = train1.getFloatValue();
//         valueEntries[1] = train2.getFloatValue();
//         valueEntries[2] = train3.getFloatValue();
//         valueEntries[3] = train4.getFloatValue();

//         sleep(0.5);
//     }
// }

// void *setPriorities(void *arg) {
//     struct sched_param params;

//     pthread_t this_thread = pthread_self();
//     params.sched_priority = sched_get_priority_max(SCHED_RR) - 1;

//     handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

//     while (true) {
//         for (int i = 0; i < NUM_TRAINS; i++) {
//             if (i == 0) { 
//                 params.sched_priority = (int) ((valueEntries[i] / 1.8) * (sched_get_priority_max(SCHED_RR) - 2));
//             } else {
//                 params.sched_priority = (int) ((valueEntries[i] / MAX_VALUE) * (sched_get_priority_max(SCHED_RR) - 2));
//             }
//             handlePriority(pthread_setschedparam(thr[i], SCHED_RR, &params));
//         }

//         sleep(0.5);
//     }
// }

void *handleTrain1(void *arg) {
    int sleepTime = SLEEP_TIME;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);
    BlackGPIO ledLock3(GPIO_44, output);

    while (true) {
        printf("Trem 1 -> cima\n");
        ledFree.setValue(high);
        sleep(sleepTime);
        sleep(sleepTime);
        sem_wait(&semNormal[0]);
        sem_wait(&semDanger);
        ledFree.setValue(low);


        printf("Trem 1 -> critico 0\n");
        ledLock1.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[1]);
        sem_wait(&semDanger);
        sem_post(&semNormal[0]);
        sem_post(&semDanger);
        ledLock1.setValue(low);


        printf("Trem 1 -> critico 1\n");
        ledLock2.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[2]);
        sem_post(&semNormal[1]);
        sem_post(&semDanger);
        ledLock2.setValue(low);


        printf("Trem 1 -> critico 2\n");
        ledLock3.setValue(high);
        sleep(sleepTime);
        sem_post(&semNormal[2]);
        ledLock3.setValue(low);

        ledFree.setValue(high);
        sleep(sleepTime);
    }
}

void *handleTrain2(void *arg) {
    int sleepTime = SLEEP_TIME;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);

    while (true) {
        printf("Trem 2 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[3]);
        sem_wait(&semDanger);
        ledFree.setValue(low);


        printf("Trem 2 -> critico 3\n");
        ledLock1.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[0]);
        sem_post(&semNormal[3]);
        sem_post(&semDanger);
        ledLock1.setValue(low);


        printf("Trem 2 -> critico 0\n");
        ledLock2.setValue(high);
        sleep(sleepTime);
        sem_post(&semNormal[0]);
        ledLock2.setValue(low);

        ledFree.setValue(high);
        sleep(sleepTime);

    }
    
}

void *handleTrain3(void *arg) {
    int sleepTime = SLEEP_TIME;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);
    BlackGPIO ledLock3(GPIO_44, output);
    
    while (true) {
        printf("Trem 3 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[4]);
        sem_wait(&semDanger);
        ledFree.setValue(low);


        printf("Trem 3 -> critico 4\n");
        ledLock1.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[1]);
        sem_wait(&semDanger);
        sem_post(&semNormal[4]);
        sem_post(&semDanger);
        ledLock1.setValue(low);


        printf("Trem 3 -> critico 1\n");
        ledLock2.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[3]);
        sem_post(&semNormal[1]);
        sem_post(&semDanger);
        ledLock2.setValue(low);


        printf("Trem 3 -> critico 3\n");
        ledLock3.setValue(high);
        sleep(sleepTime);
        sem_post(&semNormal[3]);
        ledLock3.setValue(low);

    }
}

void *handleTrain4(void *arg) {
    int sleepTime = SLEEP_TIME;
    int first = false;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);

    while (true) {

        printf("Trem 4 -> critico 2\n");

        if (!first) {
            sem_wait(&semNormal[2]);
            sem_wait(&semDanger);
        }

        first = true;

        ledLock1.setValue(high);
        sleep(sleepTime);
        sem_wait(&semNormal[4]);
        sem_post(&semNormal[2]);
        sem_post(&semDanger);
        ledLock1.setValue(low);


        printf("Trem 4 -> critico 4\n");
        ledLock2.setValue(high);
        sleep(sleepTime);
        sem_post(&semNormal[4]);
        ledLock2.setValue(low);


        printf("Trem 4 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime);
        sleep(sleepTime);
        sem_wait(&semNormal[2]);
        sem_wait(&semDanger);
        ledFree.setValue(low);

    }
    
}