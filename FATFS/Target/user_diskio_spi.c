/*
 * user_diskio_spi.c
 *
 *  Created on: 31 maj 2026
 *      Author: mateu
 */

#include "user_diskio_spi.h"
#include "main.h"
#include "ff_gen_drv.h"

extern SPI_HandleTypeDef hspi1;

// Komendy SD
#define CMD0   (0)
#define CMD1   (1)
#define CMD8   (8)
#define CMD9   (9)
#define CMD10  (10)
#define CMD12  (12)
#define CMD16  (16)
#define CMD17  (17)
#define CMD18  (18)
#define CMD23  (23)
#define CMD24  (24)
#define CMD25  (25)
#define CMD41  (41)
#define CMD55  (55)
#define CMD58  (58)
#define ACMD41 (0x40 + 41)

#define CS_LOW()   HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET)

static volatile DSTATUS Stat = STA_NOINIT;
static BYTE CardType;

// Wymiana 1 bajtu przez SPI
static BYTE xchg_spi(BYTE dat)
{
    BYTE rx;
    HAL_SPI_TransmitReceive(&hspi1, &dat, &rx, 1, 100);
    return rx;
}

// Odbieranie bloku danych z SD
static int rcvr_datablock(BYTE *buff, UINT btr)
{
    BYTE token;
    uint32_t timer = HAL_GetTick() + 200;
    do {
        token = xchg_spi(0xFF);
    } while ((token == 0xFF) && (HAL_GetTick() < timer));

    if (token != 0xFE) return 0;

    do {
        *buff++ = xchg_spi(0xFF);
        *buff++ = xchg_spi(0xFF);
    } while (btr -= 2);

    xchg_spi(0xFF);  // CRC
    xchg_spi(0xFF);
    return 1;
}

// Wysłanie bloku danych
static int xmit_datablock(const BYTE *buff, BYTE token)
{
    BYTE resp;
    xchg_spi(token);
    if (token != 0xFD) {
        for (UINT bc = 0; bc < 512; bc++) xchg_spi(*buff++);
        xchg_spi(0xFF);
        xchg_spi(0xFF);
        resp = xchg_spi(0xFF);
        if ((resp & 0x1F) != 0x05) return 0;
    }
    return 1;
}

// Wysłanie komendy SD
static BYTE send_cmd(BYTE cmd, DWORD arg)
{
    BYTE n, res;

    if (cmd & 0x80) {
        cmd &= 0x7F;
        res = send_cmd(CMD55, 0);
        if (res > 1) return res;
    }

    // Czekaj aż karta będzie gotowa
    if (cmd != CMD12) {
        CS_HIGH();
        xchg_spi(0xFF);
        CS_LOW();
        uint32_t timer = HAL_GetTick() + 500;
        do {
            if (xchg_spi(0xFF) == 0xFF) break;
        } while (HAL_GetTick() < timer);
    }

    // Wyślij komendę
    xchg_spi(0x40 | cmd);
    xchg_spi((BYTE)(arg >> 24));
    xchg_spi((BYTE)(arg >> 16));
    xchg_spi((BYTE)(arg >> 8));
    xchg_spi((BYTE)arg);
    n = 0x01;
    if (cmd == CMD0) n = 0x95;
    if (cmd == CMD8) n = 0x87;
    xchg_spi(n);

    if (cmd == CMD12) xchg_spi(0xFF);

    // Odbierz odpowiedź
    UINT trials = 10;
    do {
        res = xchg_spi(0xFF);
    } while ((res & 0x80) && --trials);

    return res;
}

// === FUNKCJE FATFS ===

DSTATUS USER_SPI_initialize(BYTE pdrv)
{
    BYTE n, cmd, ty, ocr[4];

    if (pdrv) return STA_NOINIT;

    // Wszystkie diody zgaszone na start
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);  // CZERWONA = funkcja wywołana

    CS_HIGH();
    for (n = 0; n < 10; n++) xchg_spi(0xFF);

    ty = 0;
    CS_LOW();

    BYTE odp_cmd0 = send_cmd(CMD0, 0);
    if (odp_cmd0 == 1) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);  // POMARAŃCZOWA = CMD0 OK

        uint32_t timer = HAL_GetTick() + 3000;
        if (send_cmd(CMD8, 0x1AA) == 1) {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);  // NIEBIESKA = CMD8 OK
            for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
                BYTE odp_acmd41 = 1;
                while ((HAL_GetTick() < timer) && (odp_acmd41 = send_cmd(ACMD41, 1UL << 30)));

                if (odp_acmd41 == 0) {
                    // ACMD41 zwrócił 0 - karta gotowa - mignięcie niebieskiej
                    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
                    HAL_Delay(100);
                    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
                }

                if ((HAL_GetTick() < timer) && send_cmd(CMD58, 0) == 0) {
                    for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
                    ty = (ocr[0] & 0x40) ? 12 : 4;
                }
            }
        } else {
            if (send_cmd(ACMD41, 0) <= 1) {
                ty = 2; cmd = ACMD41;
            } else {
                ty = 1; cmd = CMD1;
            }
            while ((HAL_GetTick() < timer) && send_cmd(cmd, 0));
            if (!(HAL_GetTick() < timer) || send_cmd(CMD16, 512) != 0) ty = 0;
        }
    }
    CardType = ty;

    CS_HIGH();
    xchg_spi(0xFF);

    if (ty) {
        Stat &= ~STA_NOINIT;
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);  // ZIELONA = wszystko OK
    } else {
        Stat = STA_NOINIT;
    }
    return Stat;
}

DSTATUS USER_SPI_status(BYTE pdrv)
{
    if (pdrv) return STA_NOINIT;
    return Stat;
}

DRESULT USER_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & 8)) sector *= 512;  // konwersja na adres bajtowy

    CS_LOW();
    if (count == 1) {
        if ((send_cmd(CMD17, sector) == 0) && rcvr_datablock(buff, 512))
            count = 0;
    } else {
        if (send_cmd(CMD18, sector) == 0) {
            do {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);
        }
    }
    CS_HIGH();
    xchg_spi(0xFF);

    return count ? RES_ERROR : RES_OK;
}

DRESULT USER_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

    if (!(CardType & 8)) sector *= 512;

    CS_LOW();
    if (count == 1) {
        if ((send_cmd(CMD24, sector) == 0) && xmit_datablock(buff, 0xFE))
            count = 0;
    } else {
        if (CardType & 4) send_cmd(ACMD41, count);
        if (send_cmd(CMD25, sector) == 0) {
            do {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD)) count = 1;
        }
    }
    CS_HIGH();
    xchg_spi(0xFF);

    return count ? RES_ERROR : RES_OK;
}

DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    DRESULT res = RES_ERROR;

    CS_LOW();
    switch (cmd) {
        case CTRL_SYNC:
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            // Dla uproszczenia - nie potrzebne do zapisu
            *(DWORD*)buff = 0;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 128;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
    }
    CS_HIGH();
    xchg_spi(0xFF);

    return res;
}
