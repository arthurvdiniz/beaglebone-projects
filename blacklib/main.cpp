/**
 * Autor: Tiago Fernandes de Miranda
 * Universidade Federal do Rio Grande do Norte
 */
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "BlackGPIO/BlackGPIO.h"
#include <unistd.h>
#include <vector>
#include <array>

// Inclua as classes que achar necessario
using namespace BlackLib;

int main(int argc, char * argv[]) {
    clock_t start, intermediate, end;
    bool statusBotao = false;
    bool nextLevel, debounceStart;
    double tempo_jogo = 5;
    int lastRand = 9999, currentRand;

    BlackGPIO botaoLed_1(GPIO_48, input);
    BlackGPIO botaoLed_2(GPIO_49, input);
    BlackGPIO botaoLed_3(GPIO_15, input);
    BlackGPIO botaoLed_4(GPIO_20, input);
    BlackGPIO botaoStart(GPIO_14, input);

    BlackGPIO Led_1(GPIO_44, output);
    BlackGPIO Led_2(GPIO_26, output);
    BlackGPIO Led_3(GPIO_67, output);
    BlackGPIO Led_4(GPIO_68, output);
    BlackGPIO Led_OP(GPIO_66, output);
    BlackGPIO Led_Lose(GPIO_65, output);

    std::vector<int> sequencias_jogo;
    std::vector<int> sequencias_jogador;

    Led_1.setValue(low);
    Led_2.setValue(low);
    Led_3.setValue(low);
    Led_4.setValue(low);
    Led_OP.setValue(low);
    Led_Lose.setValue(low);

    srand(time(NULL));

    while (true) {
        std::cout << botaoStart.getValue() << std::endl;
        if (botaoStart.getValue() == "1") {
            Led_OP.setValue(high);
            Led_Lose.setValue(low);
            do {
                Led_1.setValue(low);
                Led_2.setValue(low);
                Led_3.setValue(low);
                Led_4.setValue(low);

                sleep(1);

                nextLevel = true;
                currentRand = rand() % 4 + 1;
                while (currentRand == lastRand) {
                    currentRand = rand() % 4 + 1;
                }
                sequencias_jogo.push_back(currentRand);
                lastRand = currentRand;

                for (int i = 0; i < sequencias_jogo.size(); i++) {
                    switch (sequencias_jogo[i]) {
                        case 1:
                            Led_1.setValue(high);
                            sleep(1);
                            Led_1.setValue(low);
                            break;
                        case 2:
                            Led_2.setValue(high);
                            sleep(1);
                            Led_2.setValue(low);
                            break;
                        case 3:
                            Led_3.setValue(high);
                            sleep(1);
                            Led_3.setValue(low);
                            break;
                        case 4:
                            Led_4.setValue(high);
                            sleep(1);
                            Led_4.setValue(low);
                            break;
                    }
                }

                while (!statusBotao) {
                    if (botaoStart.getValue() == "1") {
                        statusBotao = true;
                        debounceStart = false;
                    }
                }
                
                //sleep(1.5);

                // start = clock();

                while (statusBotao) {
                    if (botaoStart.getValue() == "1" && debounceStart) statusBotao = false;
                    if (sequencias_jogador.size() == 0) {
                        if (botaoLed_1.getValue() == "1") {
                            debounceStart = true;
                            Led_2.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(low);
                            Led_1.setValue(high);
                            sequencias_jogador.push_back(1);
                        }
                        else if (botaoLed_2.getValue() == "1") {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(low);
                            Led_2.setValue(high);
                            sequencias_jogador.push_back(2);
                        }
                        else if (botaoLed_3.getValue() == "1") {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_2.setValue(low);
                            Led_4.setValue(low);
                            Led_3.setValue(high);
                            sequencias_jogador.push_back(3);
                        }
                        else if (botaoLed_4.getValue() == "1") {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_2.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(high);
                            sequencias_jogador.push_back(4);
                        }
                    } else {
                        if (botaoLed_1.getValue() == "1" && sequencias_jogador.back() != 1) {
                            debounceStart = true;
                            Led_2.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(low);
                            Led_1.setValue(high);
                            sequencias_jogador.push_back(1);
                        }
                        else if (botaoLed_2.getValue() == "1" && sequencias_jogador.back() != 2) {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(low);
                            Led_2.setValue(high);
                            sequencias_jogador.push_back(2);
                        }
                        else if (botaoLed_3.getValue() == "1" && sequencias_jogador.back() != 3) {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_2.setValue(low);
                            Led_4.setValue(low);
                            Led_3.setValue(high);
                            sequencias_jogador.push_back(3);
                        }
                        else if (botaoLed_4.getValue() == "1" && sequencias_jogador.back() != 4) {
                            debounceStart = true;
                            Led_1.setValue(low);
                            Led_2.setValue(low);
                            Led_3.setValue(low);
                            Led_4.setValue(high);
                            sequencias_jogador.push_back(4);
                        }
                    }
                }
                
                // end = clock();

                if (sequencias_jogador.size() > sequencias_jogo.size() || sequencias_jogador.size() < sequencias_jogo.size()) {
                    Led_Lose.setValue(high);
                    nextLevel = false;
                } else {
                    for (int i = 0; i < sequencias_jogo.size(); i++)  {
                        if (sequencias_jogo[i] != sequencias_jogador[i]) {
                            Led_Lose.setValue(high);
                            nextLevel = false;
                        }
                    }
                }

                sequencias_jogador.erase(sequencias_jogador.begin(), sequencias_jogador.begin() + sequencias_jogador.size());


                // intermediate = (double) (end-start)/CLOCKS_PER_SEC;

                // if (tempo_jogo - intermediate > 0){
                //     tempo_jogo = (tempo_jogo - intermediate) + 5;
                // } else {
                //     //Led
                //     break;
                // }

            } while (nextLevel); 
            break;
        }
    }

    Led_OP.setValue(low);
    Led_1.setValue(low);
    Led_2.setValue(low);
    Led_3.setValue(low);
    Led_4.setValue(low);

    sleep(10);
    return 0;
}
