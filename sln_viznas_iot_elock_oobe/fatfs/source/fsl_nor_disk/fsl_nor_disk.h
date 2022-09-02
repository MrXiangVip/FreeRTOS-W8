/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef FATFS_DRIVER_FSL_NOR_DISK_H_
#define FATFS_DRIVER_FSL_NOR_DISK_H_

#include "ff.h"
#include "diskio.h"
#include "flexspi_qspi_flash_ops.h"


//#define FATFS_FLASH_REGION_OFFSET       0xB20000//0x60B20000 -> 0x60F20000
//#define FATFS_FLASH_REGION_SIZE_BYTES   0x400000//4Mbytes

//#define FATFS_FLASH_REGION_OFFSET       0xB20000//0x60B20000 -> 0x60D20000
#define FATFS_FLASH_REGION_OFFSET       0x800000//0x60B20000 -> 0x60920000   xshx mod 20220831
#define FATFS_FLASH_REGION_SIZE_BYTES   0x200000//2Mbytes

#define FATFS_SECTOR_SIZE  (FLASH_PAGE_SIZE * 16)  //4K size
#define FATFS_SECTOR_COUNT (FATFS_FLASH_REGION_SIZE_BYTES/FATFS_SECTOR_SIZE)
#define FATFS_BLOCK_SIZE   (SECTOR_SIZE/FATFS_SECTOR_SIZE)


#define FATFS_BLOCK_SIZE_BYTES (FATFS_BLOCK_SIZE*FATFS_SECTOR_SIZE)

#define FATFS_SECTOR_OFFSET (FATFS_FLASH_REGION_OFFSET/FATFS_SECTOR_SIZE)

#if defined(__cplusplus)
extern "C" {
#endif

DRESULT nor_disk_status(BYTE pdrv);


DRESULT nor_disk_initialize(BYTE pdrv);


DRESULT nor_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);


DRESULT nor_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);


DRESULT nor_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);
#if defined(__cplusplus)
}
#endif


#endif /* FATFS_DRIVER_FSL_NOR_DISK_H_ */
