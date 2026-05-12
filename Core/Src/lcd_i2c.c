/*
 * lcd_i2c.c
 *
 *  Created on: 12 maj 2026
 *      Author: mateu
 */
#include "lcd_i2c.h"

extern I2C_HandleTypeDef hi2c1;

static void lcd_send_cmd(char cmd)
{
    char data_u = (cmd & 0xF0);
    char data_l = ((cmd << 4) & 0xF0);
    uint8_t data_t[4];
    data_t[0] = data_u | 0x0C;   // EN=1, RS=0
    data_t[1] = data_u | 0x08;   // EN=0, RS=0
    data_t[2] = data_l | 0x0C;   // EN=1, RS=0
    data_t[3] = data_l | 0x08;   // EN=0, RS=0
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, data_t, 4, 100);
    HAL_Delay(1);
}

static void lcd_send_data(char data)
{
    char data_u = (data & 0xF0);
    char data_l = ((data << 4) & 0xF0);
    uint8_t data_t[4];
    data_t[0] = data_u | 0x0D;   // EN=1, RS=1
    data_t[1] = data_u | 0x09;   // EN=0, RS=1
    data_t[2] = data_l | 0x0D;   // EN=1, RS=1
    data_t[3] = data_l | 0x09;   // EN=0, RS=1
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, data_t, 4, 100);
    HAL_Delay(1);
}

void LCD_Init(void)
{
    HAL_Delay(50);
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20);  // 4-bit mode

    lcd_send_cmd(0x28);  // 4-bit, 2 linie
    HAL_Delay(1);
    lcd_send_cmd(0x08);  // display off
    HAL_Delay(1);
    lcd_send_cmd(0x01);  // clear
    HAL_Delay(2);
    lcd_send_cmd(0x06);  // entry mode
    HAL_Delay(1);
    lcd_send_cmd(0x0C);  // display on
}

void LCD_Clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t adres = (row == 0) ? (0x80 | col) : (0xC0 | col);
    lcd_send_cmd(adres);
}

void LCD_Print(char *text)
{
    while (*text)
    {
        lcd_send_data(*text);
        text++;
    }
}

