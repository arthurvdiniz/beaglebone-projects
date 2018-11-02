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

using namespace BlackLib;

void handleCreationThread (int res) {
    if (res) {
        printf("Error on creating thread - %d.\n", res);
        exit(-1);
    }
}

void handleJoinThread (int res) {
    if (res) {
        printf("Error on joining thread - %d.\n", res);
        exit(-1);
    }
}

void *readEntries(void *arg);
void *setPriorities(void *arg);

void *handleTrain1(void *arg);
void *handleTrain2(void *arg);
void *handleTrain3(void *arg);
void *handleTrain4(void *arg);

float valueEntryTrain1;
float valueEntryTrain2;
float valueEntryTrain3;
float valueEntryTrain4;

int main(int argc, char * argv[]) {
    int res;
    void *resStatus;

    pthread_t thrReadEntries, thrSetPriorities, thrTrain1, thrTrain2, thrTrain3, thrTrain4;


    handleCreationThread(pthread_create(&thrReadEntries, NULL, readEntries, (void *) 1));
    handleCreationThread(pthread_create(&thrSetPriorities, NULL, setPriorities, (void *) 2));
    handleCreationThread(pthread_create(&thrTrain1, NULL, handleTrain1, (void *) 3));
    handleCreationThread(pthread_create(&thrTrain2, NULL, handleTrain2, (void *) 4));
    handleCreationThread(pthread_create(&thrTrain3, NULL, handleTrain3, (void *) 5));
    handleCreationThread(pthread_create(&thrTrain4, NULL, handleTrain4, (void *) 6));

    handleJoinThread(pthread_join(thrReadEntries, &resStatus));
    handleJoinThread(pthread_join(thrSetPriorities, &resStatus));
    handleJoinThread(pthread_join(thrTrain1, &resStatus));
    handleJoinThread(pthread_join(thrTrain2, &resStatus));
    handleJoinThread(pthread_join(thrTrain3, &resStatus));
    handleJoinThread(pthread_join(thrTrain4, &resStatus));

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

// void *readEntries(void *arg) {
//     ADC en1(AIN0);
//     ADC en2(AIN1);

//     int ret;

//     pthread_t this_thread = pthread_self();

//     struct sched_param params;
//     params.sched_priority = sched_get_priority_max(SCHED_FIFO);

//     ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

//     if (ret != 0) {
//         std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
//         exit(-1);
//     }

//     while (true) {
//         valueEntry1 = en1.getFloatValue();
//         valueEntry2 = en2.getFloatValue();

//         sleep(0.5);
//     }

// }

// void *setPriorityEntry1(void *arg) {
//     BlackGPIO led1(GPIO_44, output);

//     int ret;
//     bool on = false;

//     pthread_t this_thread = pthread_self();

//     struct sched_param params;

//     while (true) {
//         if (valueEntry1 > valueEntry2) {
//             params.sched_priority = sched_get_priority_max(SCHED_FIFO) - sched_get_priority_min(SCHED_FIFO);
//         } else {
//             params.sched_priority = sched_get_priority_min(SCHED_FIFO);
//         }

//         ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

//         if (ret != 0) {
//             std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
//             exit(-1);
//         }

//         if (on) {
//             led1.setValue(high);
//             carga(1000);
//             on = false;
//         } else {
//             led1.setValue(low);
//             carga(1000);
//             on = true;
//         }
//     }

// }
// void *setPriorityEntry2(void *arg) {

//     BlackGPIO led2(GPIO_26, output);

//     int ret;
//     bool on = false;

//     pthread_t this_thread = pthread_self();

//     struct sched_param params;

//     while (true) {
//         if (valueEntry2 > valueEntry1) {
//             params.sched_priority = sched_get_priority_max(SCHED_FIFO) - sched_get_priority_min(SCHED_FIFO);
//         } else {
//             params.sched_priority = sched_get_priority_min(SCHED_FIFO);
//         }

//         ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

//         if (ret != 0) {
//             std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
//             exit(-1);
//         }
//         if (on) {
//             led2.setValue(high);
//             carga(1000);
//             on = false;
//         } else {
//             led2.setValue(low);
//             carga(1000);
//             on = true;
//         }
//     }
// }

// http://www.yonch.com/tech/82-linux-thread-priority