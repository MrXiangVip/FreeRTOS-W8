/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "fsl_debug_console.h"
#include "ff.h"
#include "diskio.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* buffer size (in byte) for read/write operations */
#define BUFFER_SIZE (100U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
uint8_t g_bufferWrite[BUFFER_SIZE] = "xiangchaosheng";
uint8_t g_bufferRead[BUFFER_SIZE];
/*!
 * @brief Main function
 */
int fatfs_test(void)
{
    FRESULT error;
    DIR directory; /* Directory object */
    FILINFO fileInformation;
    UINT bytesWritten;
    UINT bytesRead;
    const TCHAR driverNumberBuffer[3U] = {NORDISK + '0', ':', '/'};
    BYTE work[FF_MAX_SS];
    BYTE isNeedMkfs = 0;

    BOARD_ConfigMPU();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    if (f_mount(&g_fileSystem, driverNumberBuffer, 1U) != FR_OK)
    {
        isNeedMkfs = 1;
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        PRINTF("Change drive failed.\r\n");
    }
#endif

#if FF_USE_MKFS
    if(isNeedMkfs)
    {
        PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
        if (f_mkfs(driverNumberBuffer, FM_ANY, 0U, work, sizeof work))
        {
            PRINTF("Make file system failed.\r\n");
            return -1;
        }
    }
#endif /* FF_USE_MKFS */

    PRINTF("\r\nCreate a file in %s.\r\n", _T("/cs.dat"));
    error = f_open(&g_fileObject, _T("/cs.dat"), (FA_OPEN_ALWAYS));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("File exists.\r\n");
        }
        else
        {
            PRINTF("Open file failed.\r\n");
            return -1;
        }
    }
    if (f_close(&g_fileObject))
    {
        PRINTF("\r\nClose file failed.\r\n");
        return -1;
    }


    PRINTF("Read from %s\r\n", _T("/cs.dat"));
    error = f_open(&g_fileObject, _T("/cs.dat"), (FA_READ));
    if (error)
    {
       if (error == FR_EXIST)
       {
           PRINTF("File exists.\r\n");
       }
       else
       {
           PRINTF("Open file failed.\r\n");
           return -1;
       }
    }
    memset(g_bufferRead, 0U, sizeof(g_bufferRead));
    error = f_read(&g_fileObject, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead)))
    {
        PRINTF("Read file failed. \r\n");
    }
    PRINTF("Content of %s: %s.\r\n", _T("/cs.dat"), g_bufferRead);
    if (f_close(&g_fileObject))
    {
        PRINTF("\r\nClose file failed.\r\n");
        return -1;
    }


    error = f_open(&g_fileObject, _T("/cs.dat"), (FA_WRITE));
    if (error)
    {
       if (error == FR_EXIST)
       {
           PRINTF("File exists.\r\n");
       }
       else
       {
           PRINTF("Open file failed.\r\n");
           return -1;
       }
    }
    PRINTF("\r\nWrite to above created file.\r\n");
    error = f_puts(g_bufferWrite, &g_fileObject);
    if ((error == -1))
    {
        PRINTF("Write file failed. \r\n");
    }
    if (f_close(&g_fileObject))
    {
        PRINTF("\r\nClose file failed.\r\n");
        return -1;
    }


    PRINTF("Read from %s\r\n", _T("/cs.dat"));
    error = f_open(&g_fileObject, _T("/cs.dat"), (FA_READ));
    if (error)
    {
       if (error == FR_EXIST)
       {
           PRINTF("File exists.\r\n");
       }
       else
       {
           PRINTF("Open file failed.\r\n");
           return -1;
       }
    }
    memset(g_bufferRead, 0U, sizeof(g_bufferRead));
    error = f_read(&g_fileObject, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead)))
    {
        PRINTF("Read file failed. \r\n");
    }
    PRINTF("Content of %s: %s.\r\n", _T("/cs.dat"), g_bufferRead);
    if (f_close(&g_fileObject))
    {
        PRINTF("\r\nClose file failed.\r\n");
        return -1;
    }


    while (true)
    {
    }
}
