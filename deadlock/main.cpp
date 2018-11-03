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

#define NUM_TRAINS 4
#define MAX_VALUE 1.8

using namespace BlackLib;


void *readEntries(void *arg);
void *setPriorities(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);

pthread_t thr[NUM_TRAINS];

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

void handlePriority(int res) {
    if (res) {
        printf("Error in setting thread prio - %d.\n", res);
        exit(-1);
    }
}

int main(int argc, char * argv[]) {
    void *resStatus;

    pthread_t thrReadEntries, thrSetPriorities;


    handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));
    handleCreationThread(pthread_create(&thrSetPriorities, NULL, setPriorities, (void *) 2));

    handleCreationThread(pthread_create(&thr[0], NULL, handleTrain1, (void *) 3));
    handleCreationThread(pthread_create(&thr[1], NULL, handleTrain2, (void *) 4));
    handleCreationThread(pthread_create(&thr[2], NULL, handleTrain3, (void *) 5));
    handleCreationThread(pthread_create(&thr[3], NULL, handleTrain4, (void *) 6));

    handleJoinThread(pthread_join(thrReadEntries, &resStatus));
    handleJoinThread(pthread_join(thrSetPriorities, &resStatus));

    handleJoinThread(pthread_join(thr[0], &resStatus));
    handleJoinThread(pthread_join(thr[1], &resStatus));
    handleJoinThread(pthread_join(thr[2], &resStatus));
    handleJoinThread(pthread_join(thr[3], &resStatus));

    pthread_exit(NULL);

    return 0;
}

void *readEntries(void *arg) {
    ADC train1(AIN0);
    ADC train2(AIN1);
    ADC train3(AIN2);
    ADC train4(AIN3);

    pthread_t this_thread = pthread_self();

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_RR);

    handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

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

    pthread_t this_thread = pthread_self();
    params.sched_priority = sched_get_priority_max(SCHED_RR) - 1;

    handlePriority(pthread_setschedparam(this_thread, SCHED_RR, &params));

    while (true) {
        for (int i = 0; i < NUM_TRAINS; i++) {
            params.sched_priority = (int) ((valueEntries[i] / MAX_VALUE) * (sched_get_priority_max(SCHED_RR) - 2));
            handlePriority(pthread_setschedparam(thr[i], SCHED_RR, &params));
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