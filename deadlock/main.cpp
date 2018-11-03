/**
 * Autores: Arthur Diniz, Daniel Menescal e Elvis Ferreira
 * Universidade Federal do Rio Grande do Norte
 */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"

#define NUM_TRAINS 3
#define MAX_VALUE 1.8

using namespace BlackLib;


void *readEntries(void *arg);
void *setPriorities(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);

float valueEntries[NUM_TRAINS];

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

int main(int argc, char * argv[]) {
    void *resStatus;

    pthread_t thrReadEntries, thrSetPriorities;


    handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));
    handleCreationThread(pthread_create(&thrSetPriorities, NULL, setPriorities, (void *) 2));

    handleJoinThread(pthread_join(thrReadEntries, &resStatus));
    handleJoinThread(pthread_join(thrSetPriorities, &resStatus));

    pthread_exit(NULL);

    return 0;
}

// void carga(int k){
// 	float f = 0.999999;
// 	for (int i=0; i<k; i++) {
// 		f = f*f*f*f*f;
// 		f = 1.56;
// 		for(int j=0; j<k; j++){
// 			f = sin(f)*sin(f)*f*f*f;
// 		}
// 	}
// }

void *readEntries(void *arg) {
    ADC train1(AIN0);
    ADC train2(AIN1);
    ADC train3(AIN2);
    ADC train4(AIN3);

    int ret;

    pthread_t this_thread = pthread_self();

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_RR);

    ret = pthread_setschedparam(this_thread, SCHED_RR, &params);

    if (ret != 0) {
        std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
        exit(-1);
    }

    while (true) {
        valueEntries[0] = train1.getFloatValue();
        valueEntries[1] = train2.getFloatValue();
        valueEntries[2] = train3.getFloatValue();
        valueEntries[3] = train4.getFloatValue();

        sleep(0.5);
    }
}

void *setPriorities(void *arg) {
    int ret;
    struct sched_param params;

    void *resStatus;

    pthread_t thr[NUM_TRAINS];

    handleCreationThread(pthread_create(&thr[0], NULL, handleTrain1, (void *) 3));
    handleCreationThread(pthread_create(&thr[1], NULL, handleTrain2, (void *) 4));
    handleCreationThread(pthread_create(&thr[2], NULL, handleTrain3, (void *) 5));
    handleCreationThread(pthread_create(&thr[3], NULL, handleTrain4, (void *) 6));

    while (true) {

        for (int i = 0; i < NUM_TRAINS; i++) {
            params.sched_priority = (valueEntries[i] / MAX_VALUE) * (sched_get_priority_max(SCHED_RR) - 1);

            ret = pthread_setschedparam(thr[i], SCHED_RR, &params);

            if (ret) {
                printf("Error in setting thread prio - %d.\n", ret);
                exit(-1);
            }
        }

        sleep(0.5);
    }

}

void *handleTrain1() {

}

void *handleTrain2() {
    
}

void *handleTrain3() {
    
}

void *handleTrain4() {
    
}

// http://www.yonch.com/tech/82-linux-thread-priority