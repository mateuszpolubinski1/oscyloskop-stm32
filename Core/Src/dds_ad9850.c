#include "dds_ad9850.h"

// Pomocnicza funkcja - puls na pinie
static void pulseHigh(GPIO_TypeDef* port, uint16_t pin)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

// Wysyła bajt bit po bicie - LSB first
static void tfr_byte(uint8_t data)
{
    for (int i = 0; i < 8; i++)
    {
        // Ustaw bit DATA na MOSI (PB15)
        if (data & 0x01)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

        // Puls na W_CLK (PB13)
        pulseHigh(GPIOB, GPIO_PIN_13);

        data >>= 1;  // przesuń do następnego bitu
    }
}

void DDS_Reset(void)
{
    // Ustaw wszystko na LOW
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  // W_CLK
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);  // DATA
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_RESET);  // FU_UD
    HAL_GPIO_WritePin(DDS_RST_GPIO_Port, DDS_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // Puls reset
    pulseHigh(DDS_RST_GPIO_Port, DDS_RST_Pin);

    // Puls W_CLK
    pulseHigh(GPIOB, GPIO_PIN_13);

    // Puls FU_UD - aktywuje tryb szeregowy
    pulseHigh(DDS_CS_GPIO_Port, DDS_CS_Pin);
}

void DDS_Init(void)
{
    HAL_Delay(10);
    DDS_Reset();
}

void DDS_SetFrequency(float freq_hz, uint8_t prostokat)
{
    uint32_t freq_word = (uint32_t)(freq_hz * 4294967296.0f / DDS_REF_CLK);

    // Wyślij 4 bajty frequency word - LSB first
    tfr_byte(freq_word & 0xFF);
    tfr_byte((freq_word >> 8) & 0xFF);
    tfr_byte((freq_word >> 16) & 0xFF);
    tfr_byte((freq_word >> 24) & 0xFF);

    // Control byte - 0x00 = sinus, 0x02 = prostokąt (tylko niektóre moduły)
    tfr_byte(prostokat ? 0x02 : 0x00);

    // Aktywuj nową częstotliwość - puls FU_UD
    pulseHigh(DDS_CS_GPIO_Port, DDS_CS_Pin);
}
