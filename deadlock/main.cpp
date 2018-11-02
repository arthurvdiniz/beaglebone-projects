/**
 * Autores: Arthur Diniz, Daniel Menescal e Elvis Ferreira
 * Universidade Federal do Rio Grande do Norte
 */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"

using namespace BlackLib;

void *readEntries(void *arg);
void *setPriorityEntry1(void *arg);
void *setPriorityEntry2(void *arg);

float valueEntry1;
float valueEntry2;

int main(int argc, char * argv[]) {
    int res1, res2, res3;
    void *resf1, *resf2, *resf3;

    pthread_t thread1, thread2, thread3;

    res1 = pthread_create(&thread1, NULL, readEntries, (void *) 1);
    res2 = pthread_create(&thread2, NULL, setPriorityEntry1, (void *) 2);
    res3 = pthread_create(&thread3, NULL, setPriorityEntry2, (void *) 3);

    if (res1 || res2 || res3) {
        std::cout << "Erro ao criar a thread" << std::endl;
        exit(-1);
    }

    res1 = pthread_join(thread1, &resf1);
    res2 = pthread_join(thread2, &resf2);
    res3 = pthread_join(thread3, &resf3);

    if (res1 || res2 || res3) {
        std::cout << "Erro ao fazer o join" << std::endl;
        exit(-1);
    }

    pthread_exit(NULL);

    return 0;
}

void carga(int k){
	float f = 0.999999;
	for (int i=0; i<k; i++) {
		f = f*f*f*f*f;
		f = 1.56;
		for(int j=0; j<k; j++){
			f = sin(f)*sin(f)*f*f*f;
		}
	}
}

void *readEntries(void *arg) {
    ADC en1(AIN0);
    ADC en2(AIN1);

    int ret;

    pthread_t this_thread = pthread_self();

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);

    ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

    if (ret != 0) {
        std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
        exit(-1);    
    }

    while (true) {
        valueEntry1 = en1.getFloatValue();
        valueEntry2 = en2.getFloatValue();

        sleep(0.5);
    }

}

void *setPriorityEntry1(void *arg) {
    BlackGPIO led1(GPIO_44, output);

    int ret;
    bool on = false;

    pthread_t this_thread = pthread_self();
    
    struct sched_param params;

    while (true) {
        if (valueEntry1 > valueEntry2) {
            params.sched_priority = sched_get_priority_max(SCHED_FIFO) - sched_get_priority_min(SCHED_FIFO);
        } else {
            params.sched_priority = sched_get_priority_min(SCHED_FIFO);
        }

        ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

        if (ret != 0) {
            std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
            exit(-1);   
        }
        
        if (on) {
            led1.setValue(high);
            carga(1000);
            on = false;
        } else {
            led1.setValue(low);
            carga(1000);
            on = true;
        }
    }

}
void *setPriorityEntry2(void *arg) {

    BlackGPIO led2(GPIO_26, output);

    int ret;
    bool on = false;

    pthread_t this_thread = pthread_self();
    
    struct sched_param params;

    while (true) {
        if (valueEntry2 > valueEntry1) {
            params.sched_priority = sched_get_priority_max(SCHED_FIFO) - sched_get_priority_min(SCHED_FIFO);
        } else {
            params.sched_priority = sched_get_priority_min(SCHED_FIFO);
        }

        ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

        if (ret != 0) {
            std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
            exit(-1);     
        }
        if (on) {
            led2.setValue(high);
            carga(1000);
            on = false;
        } else {
            led2.setValue(low);
            carga(1000);
            on = true;
        }
    }
}

// http://www.yonch.com/tech/82-linux-thread-priority