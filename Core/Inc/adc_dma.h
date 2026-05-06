/*
 * adc_dma.h
 *
 *  Created on: 26 mar 2026
 *      Author: mateu
 */

#ifndef INC_ADC_DMA_H_
#define INC_ADC_DMA_H_

#include "main.h"
#include "trigger.h"

#define ADC_BUFOR_ROZMIAR 1024

extern uint16_t bufor_adc[ADC_BUFOR_ROZMIAR];
extern volatile uint8_t flaga_polowa;
extern volatile uint8_t flaga_pelny;
extern Trigger_t trigger;
extern ADC_HandleTypeDef hadc1;

void ADC_DMA_Start(void);
void ADC_DMA_Stop(void);
void ADC_DMA_Przetworz_Polowe(void);
void ADC_DMA_Przetworz_Pelny(void);

#endif /* INC_ADC_DMA_H_ */
