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
#define MAX_VALUE 1.8
#define SEM_NORMAL 5
#define SEM_DANGER 2
#define MAX_SLEEP_TIME 3

using namespace BlackLib;

void *readEntries(void *arg);
void *setPriorities(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);


pthread_t thr[NUM_TRAINS];

pthread_mutex_t mutex[SEM_NORMAL];
sem_t semNormal[SEM_NORMAL], semDanger[SEM_DANGER], semAux;

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

void handleInitSemaphore(int res) {
    if (res) {
        printf("Error on creating semaphore or mutex - %d.\n", res);
        exit(-1);
    }
}

void handleDestroySemaphore(int res) {
    if (res) {
        printf("Error on destroying semaphore or mutex - %d.\n", res);
        exit(-1);
    }
}

int main(int argc, char * argv[]) {
    
    void *resStatus;

    pthread_t thrReadEntries, thrSetPriorities;

    for (int i = 0; i < SEM_NORMAL; i++) {
        handleInitSemaphore(pthread_mutex_init(&mutex[i], NULL));
        // handleInitSemaphore(sem_init(&semNormal[i], 0, 1));
    }

    for (int i = 0; i < SEM_DANGER; i++) {
        handleInitSemaphore(sem_init(&semDanger[i], 0, 2));
    }

    handleInitSemaphore(sem_init(&semAux, 0, 3));

    // handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));

    // handleCreationThread(pthread_create(&thrSetPriorities, NULL, setPriorities, (void *) 2));
    
    handleCreationThread(pthread_create(&thr[0], NULL, handleTrain1, NULL));
    handleCreationThread(pthread_create(&thr[1], NULL, handleTrain2, NULL));
    handleCreationThread(pthread_create(&thr[2], NULL, handleTrain3, NULL));
    handleCreationThread(pthread_create(&thr[3], NULL, handleTrain4, NULL));

    // handleJoinThread(pthread_join(thrReadEntries, &resStatus));

    // handleJoinThread(pthread_join(thrSetPriorities, &resStatus));

    handleJoinThread(pthread_join(thr[0], NULL));
    handleJoinThread(pthread_join(thr[1], NULL));
    handleJoinThread(pthread_join(thr[2], NULL));
    handleJoinThread(pthread_join(thr[3], NULL));

    pthread_exit(NULL);

    for (int i = 0; i < SEM_NORMAL; i++) {
        // handleDestroySemaphore(sem_destroy(&semNormal[i]));
        handleDestroySemaphore(sem_destroy(&semDanger[i]));
    }

    for (int i = 0; i < SEM_DANGER; i++) {
        handleDestroySemaphore(pthread_mutex_destroy(&mutex[i]));
    }
    
    handleDestroySemaphore(sem_destroy(&semAux));

    return 0;
}

// void *readEntries(void *arg) {
//     printf("oi");
//     ADC train1(AIN0);
//     ADC train2(AIN1);
//     ADC train3(AIN4);
//     ADC train4(AIN3);
//     printf("oi2");
//     pthread_t this_thread = pthread_self();

//     struct sched_param params;
//     params.sched_priority = sched_get_priority_max(SCHED_FIFO);

//     handlePriority(pthread_setschedparam(this_thread, SCHED_FIFO, &params));

//     while (true) {
//         printf("oi3");
//         valueEntries[0] = train1.getFloatValue();
//         valueEntries[1] = train2.getFloatValue();
//         valueEntries[2] = train3.getFloatValue();
//         valueEntries[3] = train4.getFloatValue();

//         printf("1: %f\n", valueEntries[0]);
//         printf("2: %f\n", valueEntries[1]);
//         printf("3: %f\n", valueEntries[2]);
//         printf("4: %f\n", valueEntries[3]);

//         sleep(0.5);
//     }
// }

// void *setPriorities(void *arg) {
//     struct sched_param params;

//     pthread_t this_thread = pthread_self();
//     params.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;

//     handlePriority(pthread_setschedparam(this_thread, SCHED_FIFO, &params));

//     while (true) {
//         for (int i = 0; i < NUM_TRAINS; i++) {
//             params.sched_priority = (int) ((valueEntries[i]/ MAX_VALUE) * (sched_get_priority_max(SCHED_FIFO) - 2));
//             if (params.sched_priority == 0) params.sched_priority = 1;
//             handlePriority(pthread_setschedparam(thr[i], SCHED_FIFO, &params));
//         }

//         sleep(0.5);
//     }
// }

void *handleTrain1(void *arg) {
    int sleepTime = MAX_SLEEP_TIME;
    bool first = false;
    
    BlackGPIO ledFree(GPIO_74, output);
    BlackGPIO ledLock1(GPIO_39, output);
    BlackGPIO ledLock2(GPIO_34, output);
    BlackGPIO ledLock3(GPIO_35, output);

    while (true) {
        if (!first) {
            printf("Trem 1 -> cima\n");
            ledFree.setValue(high);
            sleep(sleepTime*2);
        }

        first = true; 

        // sem_wait(&semNormal[0]);
        pthread_mutex_lock(&mutex[0]);
        sem_wait(&semDanger[0]);
        sem_wait(&semAux);

        printf("Trem 1 -> critico 0\n");
        ledFree.setValue(low);
        ledLock1.setValue(high);
        sleep(sleepTime);
        // sem_wait(&semNormal[1]);
        pthread_mutex_lock(&mutex[1]);
        sem_wait(&semDanger[1]);

        printf("Trem 1 -> critico 1\n");
        // sem_post(&semNormal[0]);
        pthread_mutex_unlock(&mutex[0]);
        sem_post(&semDanger[0]);
        sem_post(&semAux);
        ledLock1.setValue(low);
        ledLock2.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[2]);
        // sem_wait(&semNormal[2]);

        printf("Trem 1 -> critico 2\n");
        // sem_post(&semNormal[1]);
        pthread_mutex_unlock(&mutex[1]);
        sem_post(&semDanger[1]);
        ledLock2.setValue(low);
        ledLock3.setValue(high);
        sleep(sleepTime);
        pthread_mutex_unlock(&mutex[2]);
        // sem_post(&semNormal[2]);
        ledLock3.setValue(low);

        printf("Trem 1 -> cima\n");
        ledFree.setValue(high);
        sleep(sleepTime*3);
    }
}

void *handleTrain2(void *arg) {
    int sleepTime = MAX_SLEEP_TIME;
    bool first = false;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);

    while (true) {
        if (!first) {
            printf("Trem 2 -> baixo\n");
            ledFree.setValue(high);
            sleep(sleepTime);
        }

        first = true;
        
        // sem_wait(&semNormal[3]);
        pthread_mutex_lock(&mutex[3]);
        sem_wait(&semDanger[0]);
        sem_wait(&semAux);

        printf("Trem 2 -> critico 3\n");
        ledFree.setValue(low);
        ledLock1.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[0]);
        // sem_wait(&semNormal[0]);

        printf("Trem 2 -> critico 0\n");
        // sem_post(&semNormal[3]);
        pthread_mutex_unlock(&mutex[3]);
        sem_post(&semDanger[0]);
        sem_post(&semAux);
        ledLock1.setValue(low);        
        ledLock2.setValue(high);
        sleep(sleepTime);
        pthread_mutex_unlock(&mutex[0]);
        // sem_post(&semNormal[0]);
        ledLock2.setValue(low);

        printf("Trem 2 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime*2);
    }
    
}

void *handleTrain3(void *arg) {
    int sleepTime = MAX_SLEEP_TIME;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);
    BlackGPIO ledLock3(GPIO_44, output);
    
    while (true) {
        printf("Trem 3 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[4]);
        // sem_wait(&semNormal[4]);
        sem_wait(&semDanger[1]);
        sem_wait(&semAux);

        printf("Trem 3 -> critico 4\n");
        ledFree.setValue(low);        
        ledLock1.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[1]);
        // sem_wait(&semNormal[1]);
        sem_wait(&semDanger[0]);

        printf("Trem 3 -> critico 1\n");
        pthread_mutex_unlock(&mutex[4]);
        // sem_post(&semNormal[4]);
        sem_post(&semDanger[1]);
        sem_post(&semAux);
        ledLock1.setValue(low);
        ledLock2.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[3]);
        // sem_wait(&semNormal[3]);

        printf("Trem 3 -> critico 3\n");
        pthread_mutex_unlock(&mutex[1]);
        // sem_post(&semNormal[1]);
        sem_post(&semDanger[0]);
        ledLock2.setValue(low);
        ledLock3.setValue(high);
        sleep(sleepTime);
        pthread_mutex_unlock(&mutex[3]);
        // sem_post(&semNormal[3]);
        ledLock3.setValue(low);
    }
}

void *handleTrain4(void *arg) {
    int sleepTime = MAX_SLEEP_TIME;
    int first = false;

    BlackGPIO ledFree(GPIO_44, output);
    BlackGPIO ledLock1(GPIO_44, output);
    BlackGPIO ledLock2(GPIO_44, output);

    while (true) {
        // sleepTime = sleep_time[3];

        if (!first) {
            pthread_mutex_lock(&mutex[2]);
            // sem_wait(&semNormal[2]);
            sem_wait(&semDanger[1]);
            sem_wait(&semAux);
            printf("Trem 4 -> critico 2\n");
        }

        first = true;
        
        ledLock1.setValue(high);
        sleep(sleepTime);
        pthread_mutex_lock(&mutex[4]);
        // sem_wait(&semNormal[4]);

        printf("Trem 4 -> critico 4\n");
        pthread_mutex_unlock(&mutex[2]);
        // sem_post(&semNormal[2]);
        sem_post(&semDanger[1]);
        sem_post(&semAux);
        ledLock1.setValue(low);        
        ledLock2.setValue(high);
        sleep(sleepTime);
        pthread_mutex_unlock(&mutex[4]);
        // sem_post(&semNormal[4]);
        ledLock2.setValue(low);

        printf("Trem 4 -> baixo\n");
        ledFree.setValue(high);
        sleep(sleepTime*2);
        pthread_mutex_lock(&mutex[2]);
        // sem_wait(&semNormal[2]);
        sem_wait(&semDanger[1]);
        sem_wait(&semAux);

        printf("Trem 4 -> critico 2\n");
        ledFree.setValue(low);

    }
    
}