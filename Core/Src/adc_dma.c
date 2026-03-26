/*
 * adc_dma.c
 *
 *  Created on: 26 mar 2026
 *      Author: mateu
 */

#include "adc_dma.h"
#include "usbd_cdc_if.h"

// Definicja bufora — tutaj jest faktyczna pamięć
uint16_t bufor_adc[ADC_BUFOR_ROZMIAR];

// Definicja flag
volatile uint8_t flaga_polowa = 0;
volatile uint8_t flaga_pelny = 0;

void ADC_DMA_Start(void)
{
    flaga_polowa = 0;
    flaga_pelny = 0;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)bufor_adc, ADC_BUFOR_ROZMIAR);
}

void ADC_DMA_Stop(void)
{
    HAL_ADC_Stop_DMA(&hadc1);
    flaga_polowa = 0;
    flaga_pelny = 0;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        flaga_polowa = 1;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        flaga_pelny = 1;
    }
}

void ADC_DMA_Przetworz_Polowe(void)
{
    flaga_polowa = 0;
    while(CDC_Transmit_FS((uint8_t*)&bufor_adc[0],
          ADC_BUFOR_ROZMIAR/2) == USBD_BUSY)
    {
        HAL_Delay(1);
    }
}

void ADC_DMA_Przetworz_Pelny(void)
{
    flaga_pelny = 0;
    while(CDC_Transmit_FS((uint8_t*)&bufor_adc[ADC_BUFOR_ROZMIAR/2],
          ADC_BUFOR_ROZMIAR/2) == USBD_BUSY)
    {
        HAL_Delay(1);
    }
}
