/**
 * Autor: Tiago Fernandes de Miranda
 * Universidade Federal do Rio Grande do Norte
 */
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <pthread.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <unistd.h>

// Inclua as classes que achar necessario

using namespace BlackLib;

int main(int argc, char * argv[])
{
    clock_t start, intermediate, end;
    bool statusBotao = false;
    bool nextLevel;
    double tempo_jogo = 5;

    BlackGPIO botaoLed_1(GPIO_14, input);
    BlackGPIO botaoLed_2(GPIO_15, input);
    BlackGPIO botaoLed_3(GPIO_16, input);
    BlackGPIO botaoLed_4(GPIO_17, input);
    BlackGPIO botaoStart(GPIO_18, input);

    BlackGPIO Led_1(GPIO_10, output);
    BlackGPIO Led_2(GPIO_11, output);
    BlackGPIO Led_3(GPIO_12, output);
    BlackGPIO Led_4(GPIO_13, output);
    BlackGPIO Led_OP(GPIO_13, output);
    BlackGPIO Led_notwin(GPIO_13, output);

    int sequencias_jogo[50];
    int sequencias_jogador[50];
    if (botaoStart.getValue()){
        do {
            nextLevel = true;
            sequencias_jogo.push(rand()% 4 + 1);
            for(int i = 0; i <= sizeof(sequencias_jogo); i++) {
                switch (sequencias_jogo[i]) {
                    case 1:
                        Led_1.setValue(high);
                        sleep(0.5);
                        Led_1.setValue(low);
                        continue;
                    case 2:
                        Led_2.setValue(high);
                        sleep(0.5);
                        Led_2.setValue(low);
                        continue;
                    case 3:
                        Led_3.setValue(high);
                        sleep(0.5);
                        Led_3.setValue(low);
                        continue;
                    case 4:
                        Led_4.setValue(high);
                        sleep(0.5);
                        Led_4.setValue(low);
                        continue;
                }
            }
            while (!statusBotao){
                if (botaoStart.getValue()) statusBotao = true;
            }
            start = clock();

            while (statusBotao){
                if (botaoStart.getValue()) statusBotao = false;
                if(botaoLed_1.getValue()) sequencias_jogador.push_back(1);
                if(botaoLed_2.getValue()) sequencias_jogador.push_back(2);
                if(botaoLed_3.getValue()) sequencias_jogador.push_back(3);
                if(botaoLed_4.getValue()) sequencias_jogador.push_back(4);
            }
            end = clock();
            for(int i = 0; i<= sizeof(sequencias_jogo);i++){
                if(sequencias_jogo[i] != sequencias_jogador[i]){
                    //Led
                    nextLevel = false;

                }
            }


            intermediate = (double) (end-start)/CLOCKS_PER_SEC;

            if(tempo_jogo-intermediate>0){
                tempo_jogo = (tempo_jogo-intermediate) + 5;
            }
            else{
                //Led
                break;
            }

        } while(nextLevel == true && tempo_jogo-intermediate>0); // nextLevel == true; 
    }
    return 0;
}
