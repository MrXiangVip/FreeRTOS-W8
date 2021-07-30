/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_nor_disk.h"
#include "sln_flash.h"
#include "flexspi_qspi_flash_ops.h"
#include "fsl_debug_console.h"
#include "fatfs_op.h"

#include "FreeRTOS.h"
#include "task.h"

//static BYTE fatfs_cache[FATFS_BLOCK_SIZE_BYTES];

DRESULT nor_disk_status(BYTE pdrv)
{
     DSTATUS stat = RES_OK;

     if( pdrv != NORDISK)
     {
         return STA_NOINIT;
     }

     return stat;
}

DRESULT nor_disk_initialize(BYTE pdrv)
{
    DSTATUS stat = RES_OK;

    if (pdrv != NORDISK)
    {
        return STA_NOINIT;
    }

    //SLN_Flash_Init();

    return stat;
}

DRESULT nor_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DSTATUS stat = RES_OK;

    if (pdrv != NORDISK)
    {
        return RES_PARERR;
    }
    sector += FATFS_SECTOR_OFFSET;
    SLN_Read_Flash_At_Address(sector * FATFS_SECTOR_SIZE, buff, count * FATFS_SECTOR_SIZE);

    return stat;
}


//static void ram_memcpy(void *dst, void *src, size_t len)
//{
//    uint8_t *ptrDst = (uint8_t *)dst;
//    uint8_t *ptrSrc = (uint8_t *)src;
//
//    while (len)
//    {
//        *ptrDst++ = *ptrSrc++;
//        len--;
//    }
//}

DRESULT nor_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DSTATUS stat = RES_OK;

    if (pdrv != NORDISK)
    {
        return RES_PARERR;
    }

    sector += FATFS_SECTOR_OFFSET;

    //write start addr.
    uint32_t addr_begin = sector * FATFS_SECTOR_SIZE;

    for (UINT i = 0;i<count;i++)
    {
    	SLN_Write_Sector(addr_begin + i*FATFS_SECTOR_SIZE,(BYTE*)buff + i*FATFS_SECTOR_SIZE);

    }

   	//SDK_DelayAtLeastUs(20*1000,600*1000*1000UL);
    return stat;
}


DRESULT nor_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DSTATUS stat = RES_OK;

    if (pdrv != NORDISK)
    {
     return RES_PARERR;
    }

    switch (cmd)
    {
     case GET_SECTOR_COUNT:
         if (buff)
         {
             *(uint32_t *)buff = FATFS_SECTOR_COUNT;
         }
         else
         {
             stat = RES_PARERR;
         }
         break;
     case GET_SECTOR_SIZE:
         if (buff)
         {
             *(uint32_t *)buff = FATFS_SECTOR_SIZE;
         }
         else
         {
             stat = RES_PARERR;
         }
         break;
     case GET_BLOCK_SIZE:
         if (buff)
         {
             *(uint32_t *)buff = FATFS_BLOCK_SIZE;
         }
         else
         {
             stat = RES_PARERR;
         }
         break;
     case CTRL_SYNC:
         stat = RES_OK;
         break;
     default:
         stat = RES_PARERR;
         break;
    }
    return stat;
}


