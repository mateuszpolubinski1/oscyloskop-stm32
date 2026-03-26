/*
 * adc_dma.h
 *
 *  Created on: 26 mar 2026
 *      Author: mateu
 */

#ifndef INC_ADC_DMA_H_
#define INC_ADC_DMA_H_

#include "main.h"
extern ADC_HandleTypeDef hadc1;

// Rozmiar bufora — 1024 próbki (512 na połowę)
#define ADC_BUFOR_ROZMIAR 1024

// Bufor DMA — wypełniany automatycznie przez sprzęt
extern uint16_t bufor_adc[ADC_BUFOR_ROZMIAR];

// Flagi ustawiane w callbackach DMA
extern volatile uint8_t flaga_polowa;
extern volatile uint8_t flaga_pelny;

// Funkcje
void ADC_DMA_Start(void);
void ADC_DMA_Stop(void);
void ADC_DMA_Przetworz_Polowe(void);
void ADC_DMA_Przetworz_Pelny(void);

#endif /* INC_ADC_DMA_H_ */
