/*
 * lcd_i2c.h
 *
 *  Created on: 12 maj 2026
 *      Author: mateu
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "main.h"

// Adres I2C wyświetlacza (najpopularniejsze: 0x27 lub 0x3F)
#define LCD_I2C_ADDR  (0x27 << 1)  // przesunięcie bo HAL używa 8-bit

// Funkcje
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *text);

#endif /* INC_LCD_I2C_H_ */
