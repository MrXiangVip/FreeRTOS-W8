/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */
#ifndef FATFS_DRIVER_FSL_NOR_DISK_H_
#define FATFS_DRIVER_FSL_NOR_DISK_H_

#include "integer.h"
#include "diskio.h"


#define FATFS_FLASH_REGION_OFFSET       0x820000//0x60820000 -> 0x60a20000
#define FATFS_FLASH_REGION_SIZE_BYTES   0x200000//2Mbytes

#define FATFS_SECTOR_SIZE  (FLASH_PAGE_SIZE * 2)
#define FATFS_SECTOR_COUNT (FATFS_FLASH_REGION_SIZE_BYTES/FATFS_SECTOR_SIZE)
#define FATFS_BLOCK_SIZE   (SECTOR_SIZE/FATFS_SECTOR_SIZE)


#define FATFS_BLOCK_SIZE_BYTES (FATFS_BLOCK_SIZE*FATFS_SECTOR_SIZE)

#define FATFS_SECTOR_OFFSET (FATFS_FLASH_REGION_OFFSET/FATFS_SECTOR_SIZE)

DRESULT nor_disk_status(BYTE pdrv);


DRESULT nor_disk_initialize(BYTE pdrv);


DRESULT nor_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);


DRESULT nor_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);


DRESULT nor_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);


#endif /* FATFS_DRIVER_FSL_NOR_DISK_H_ */
