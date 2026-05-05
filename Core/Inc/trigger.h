/*
 * trigger.h
 *
 *  Created on: 5 maj 2026
 *      Author: mateu
 */

#ifndef INC_TRIGGER_H_
#define INC_TRIGGER_H_

#include "main.h"

// Tryby triggera
typedef enum {
    TRIG_RISING,   // zbocze narastające
    TRIG_FALLING,  // zbocze opadające
    TRIG_NONE      // brak triggera
} TriggerMode_t;

// Konfiguracja triggera
typedef struct {
    TriggerMode_t mode;   // tryb
    uint16_t poziom;      // próg 0-4095
} Trigger_t;

// Funkcje
void Trigger_Init(Trigger_t *trig, TriggerMode_t mode, uint16_t poziom);
int16_t Trigger_Szukaj(Trigger_t *trig, uint16_t *bufor, uint16_t rozmiar);

#endif /* INC_TRIGGER_H_ */
