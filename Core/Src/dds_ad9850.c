#include "dds_ad9850.h"

extern SPI_HandleTypeDef hspi2;

void DDS_Reset(void)
{
    HAL_GPIO_WritePin(DDS_RST_GPIO_Port, DDS_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(DDS_RST_GPIO_Port, DDS_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // Puls na W_CLK (PB13)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

    // Puls na FU_UD
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_RESET);
}

void DDS_Init(void)
{
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    DDS_Reset();
}

void DDS_SetFrequency(float freq_hz, uint8_t prostokat)
{
    uint32_t freq_word = (uint32_t)(freq_hz * 4294967296.0f / DDS_REF_CLK);

    uint8_t dane[5];
    dane[0] = (freq_word)       & 0xFF;
    dane[1] = (freq_word >> 8)  & 0xFF;
    dane[2] = (freq_word >> 16) & 0xFF;
    dane[3] = (freq_word >> 24) & 0xFF;
    dane[4] = prostokat ? 0x02 : 0x00;

    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, dane, 5, 100);
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DDS_CS_GPIO_Port, DDS_CS_Pin, GPIO_PIN_RESET);
}
