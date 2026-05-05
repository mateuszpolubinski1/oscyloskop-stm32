/*
 * trigger.c
 *
 *  Created on: 5 maj 2026
 *      Author: mateu
 */

#include "trigger.h"

void Trigger_Init(Trigger_t *trig, TriggerMode_t mode, uint16_t poziom)
{
    trig->mode = mode;
    trig->poziom = poziom;
}

int16_t Trigger_Szukaj(Trigger_t *trig, uint16_t *bufor, uint16_t rozmiar)
{
    if (trig->mode == TRIG_NONE)
        return 0;

    for (int i = 0; i < rozmiar - 1; i++)
    {
        if (trig->mode == TRIG_RISING)
        {
            if (bufor[i] < trig->poziom && bufor[i+1] >= trig->poziom)
                return i;
        }
        else if (trig->mode == TRIG_FALLING)
        {
            if (bufor[i] > trig->poziom && bufor[i+1] <= trig->poziom)
                return i;
        }
    }

    return -1;
}
