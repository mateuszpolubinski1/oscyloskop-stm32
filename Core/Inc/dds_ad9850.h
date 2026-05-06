/*
 * dds_ad9850.h
 *
 *  Created on: 6 maj 2026
 *      Author: mateu
 */

#ifndef INC_DDS_AD9850_H_
#define INC_DDS_AD9850_H_

#include "main.h"

// Częstotliwość zegara referencyjnego AD9850
#define DDS_REF_CLK     125000000.0f  // 125 MHz

// Funkcje
void DDS_Init(void);
void DDS_SetFrequency(float freq_hz, uint8_t prostokat);
void DDS_Reset(void);

#endif /* INC_DDS_AD9850_H_ */
