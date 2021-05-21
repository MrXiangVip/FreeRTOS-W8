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
#include "fsl_qspi_nor_disk.h"
#include "sln_flash.h"
#include "flexspi_qspi_flash.h"
#include "fsl_debug_console.h"

static BYTE fatfs_cache[FATFS_BLOCK_SIZE_BYTES];

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

    SLN_Flash_Init();

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

uint32_t is_filed_used(uint32_t addr_beigin, uint32_t addr_end)
{
    char* addr;
    for(addr = (char*)addr_beigin; addr <= (char*)addr_end; addr++) {
        if(*(addr+FlexSPI_AMBA_BASE) != 0xFF)
            return 1;
    }
    return 0;
}

static void ram_memcpy(void *dst, void *src, size_t len)
{
    uint8_t *ptrDst = (uint8_t *)dst;
    uint8_t *ptrSrc = (uint8_t *)src;

    while (len)
    {
        *ptrDst++ = *ptrSrc++;
        len--;
    }
}

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

    //write size.
    uint32_t write_size = count * FATFS_SECTOR_SIZE;

    uint32_t sector_addr_begin = (addr_begin / FATFS_BLOCK_SIZE_BYTES) * FATFS_BLOCK_SIZE_BYTES;

    if ((addr_begin >= sector_addr_begin) &&
            (addr_begin + write_size - 1) < (sector_addr_begin + FATFS_BLOCK_SIZE_BYTES)) // one sector operate
    {
        uint32_t sector_offset = addr_begin - sector_addr_begin;
        if( !is_filed_used(addr_begin, addr_begin + write_size -1) ) //not need erase
        {
            for(int i = 0; i < write_size; i += FLASH_PAGE_SIZE)
            {
                if( i + FLASH_PAGE_SIZE < write_size)
                {
                    SLN_Write_Flash_Page(addr_begin + i, (uint8_t *)buff + i, FLASH_PAGE_SIZE);
                }
                else
                {
                    SLN_Write_Flash_Page(addr_begin + i, (uint8_t *)buff + i, write_size - i);
                }
            }
        }
        else//need erase
        {
            SLN_Read_Flash_At_Address(sector_addr_begin, fatfs_cache, FATFS_BLOCK_SIZE_BYTES);
            ram_memcpy(fatfs_cache + sector_offset, (uint8_t *)buff, write_size);
            SLN_Erase_Sector(sector_addr_begin);
            for(int i = 0; i < FATFS_BLOCK_SIZE_BYTES; i+=FLASH_PAGE_SIZE)
            {
                SLN_Write_Flash_Page(sector_addr_begin + i, fatfs_cache + i, FLASH_PAGE_SIZE);
            }
        }
    }
    else// multiple write
    {
        assert(0);
        PRINTF("multiple write.\r\n");
    }

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

