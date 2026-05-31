/*
 * user_diskio_spi.h
 *
 *  Created on: 31 maj 2026
 *      Author: mateu
 */
#ifndef USER_DISKIO_SPI_H
#define USER_DISKIO_SPI_H

#include "diskio.h"

DSTATUS USER_SPI_initialize(BYTE pdrv);
DSTATUS USER_SPI_status(BYTE pdrv);
DRESULT USER_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
DRESULT USER_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#endif
