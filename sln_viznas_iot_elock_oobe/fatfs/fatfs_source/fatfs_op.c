/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */
#include "fatfs_op.h"
#include "ff.h"
#include "diskio.h"
#include "generic_list.h"

const TCHAR driverNumberBuffer[3U] = {NORDISK + '0', ':', '/'};

static FATFS g_fileSystem;
//static FIL &g_fileObject;   /* File object */
static DIR g_dir;          /* Directory object */
static FILINFO g_fileInfo; /* File information */

int fatfs_mount_with_mkfs(void)
{
    FRESULT error;
    BYTE isNeedMkfs = 0;
    BYTE* work = NULL;

#if 0//#ifdef FSL_RTOS_FREE_RTOS
    work = (BYTE*)pvPortMalloc(FF_MAX_SS);
#else
    work = (BYTE*)malloc(FF_MAX_SS);	/* Allocate a new memory block with POSIX API */
#endif


    if ((error = f_mount(&g_fileSystem, driverNumberBuffer, 1U)) != FR_OK)
    {
        isNeedMkfs = 1;
    }else
    {
        FATFS_PRINTF(("f_mount OK!!!.\r\n"));
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        FATFS_PRINTF(("Change drive failed.\r\n"));
    }
#endif

#if FF_USE_MKFS
    if (isNeedMkfs)
    {
        FATFS_PRINTF(("\r\nMake file system......The time may be long if the card capacity is big.\r\n"));
        if ((error = f_mkfs(driverNumberBuffer, NULL, work, FF_MAX_SS)) != FR_OK)
        {
            FATFS_PRINTF(("Make file system failed.\r\n"));
        }
    }
#endif /* FF_USE_MKFS */

#if 0 //#ifdef FSL_RTOS_FREE_RTOS
    vPortFree(work);
#else
    free(work);	/* Allocate a new memory block with POSIX API */
#endif

    return error;
}

int fatfs_unmount(void)
{
    return 0;
}

int fatfs_write(const char *file_name, const char *buf, int offset, int bytes)
{
    FRESULT error;
    FIL g_fileObject;   /* File object */

    if (bytes > 0 && offset >= 0)
    {
		error    = f_open(&g_fileObject, _T(file_name), (FA_WRITE | FA_OPEN_ALWAYS));
		if (error)
		{
			if (error == FR_EXIST)
			{
				FATFS_PRINTF(("%s File exists.\r\n", file_name));
			}
			else
			{
				FATFS_PRINTF(("Open %s file failed: %d.\r\n", file_name, error));
				return error;
			}
		}

		error = f_lseek(&g_fileObject, offset);
		if (error)
		{
			FATFS_PRINTF(("Move file point failed: %d.\r\n", error));

		}else
		{

			UINT num = 0;
			error = f_write(&g_fileObject, buf, bytes, &num);
			if (error || (bytes != num))
			{
				error = FR_DISK_ERR;
				FATFS_PRINTF(("Write file failed: %d, %d write number: %d.\r\n", error, bytes, num));

			}else
			{
				FATFS_PRINTF(("Write file:%s, %d.\r\n", file_name, bytes));

			}
		}

		if ((error = f_close(&g_fileObject)) != 0)
		{
			FATFS_PRINTF(("\r\nClose file failed.\r\n"));

		}

    }else
    {
    	FATFS_PRINTF(("Write file invalid length/offset:%d/%d.\r\n", bytes, offset));
    	error = FR_INVALID_PARAMETER;
    }

    return error;
}

#if 0
int fatfs_write_append(const char *file_name, const char *buf, int bytes)
{
    FRESULT error;
    UINT num = -1;
    FIL g_fileObject;   /* File object */

    error    = f_open(&g_fileObject, _T(file_name), (FA_OPEN_ALWAYS | FA_WRITE ));
    if (error)
    {
        if (error == FR_EXIST)
        {
            FATFS_PRINTF(("File exists.\r\n"));
        }
        else
        {
            FATFS_PRINTF(("Write file %s failed: %d.\r\n", file_name, error));
            return error;
        }
    }

    error = f_lseek(&g_fileObject, f_size(&g_fileObject));
    if (error)
    {
        FATFS_PRINTF(("Move file point failed: %d.\r\n", error));

    }else
    {

		error = f_write(&g_fileObject, buf, bytes, &num);
		if (error || (bytes != num))
		{
			FATFS_PRINTF(("Write file failed: %d, %d write number: %d.\r\n", error, bytes, num));

		}

    }

	if (f_close(&g_fileObject))
	{
		FATFS_PRINTF(("\r\nClose file failed.\r\n"));
        error = -2;
	}

    return error;
}
#endif

int fatfs_read(char *file_name, char *buf, int offset, int bytes)
{
    FRESULT error;
    FIL g_fileObject;   /* File object */

    if (bytes > 0 && offset >= 0)
    {
		error = f_open(&g_fileObject, _T(file_name), (FA_READ));
		if (error)
		{
			if (error == FR_EXIST)
			{
				FATFS_PRINTF(("File exists.\r\n"));
			}
			else
			{
				FATFS_PRINTF(("Open file %s failed: %d.\r\n", file_name, error));
				return error;
			}
		}

		FATFS_PRINTF(("Read from %s, %d\r\n", file_name, bytes));

		error = f_lseek(&g_fileObject, offset);
		if (error)
		{
			FATFS_PRINTF(("Move file point failed: %d.\r\n", error));

		}else
		{
			UINT num = 0;
			error = f_read(&g_fileObject, buf, bytes, &num);
			if (error || (num != bytes))
			{
				FATFS_PRINTF(("Read file failed: %d, read bytes: %d, read number: %d.\r\n", error, bytes, num));
			}

			//    FATFS_PRINTF(("Content of %s: %s.\r\n", _T(file_name), buf));
			if ((error = f_close(&g_fileObject)) != 0)
			{
				FATFS_PRINTF(("\r\nClose file failed.\r\n"));
			}
		}

    }
    else
    {
    	error = FR_INVALID_PARAMETER;
    	FATFS_PRINTF(("Read file invalid length/offset:%d/%d.\r\n", bytes, offset));
    }
    return error;
}

#if 0
int fatfs_opendir(const char *dir_name)
{
    if (f_opendir(&g_dir, dir_name))
    {
        FATFS_PRINTF(("Open directory failed.\r\n"));
        return -1;
    }

    return 0;
}

int fatfs_readdir(char *file_name)
{
    FRESULT error;

    error = f_readdir(&g_dir, &g_fileInfo);

    /* To the end. */
    if ((error != FR_OK) || (g_fileInfo.fname[0U] == 0U))
    {
        if (error != FR_OK)
            FATFS_PRINTF(("Read file %s failed: %d.\r\n", file_name, error));
        return 0;
    }

    if (g_fileInfo.fname[0] == '.')
    {
        return 1;
    }

    if (g_fileInfo.fattrib & AM_DIR)
    {
        //        FATFS_PRINTF(("Directory file : %s.\r\n", g_fileInfo.fname));
        return 2;
    }
    else
    {
        //        FATFS_PRINTF(("General file : %s.\r\n", g_fileInfo.fname));
        memcpy(file_name, g_fileInfo.fname, sizeof(g_fileInfo.fname));
        return 3;
    }

    //    return -1;
}

int fatfs_closedir(void)
{
    if (f_closedir(&g_dir))
    {
        FATFS_PRINTF(("Close directory failed.\r\n"));
        return -1;
    }

    return 0;
}
#endif

int fatfs_getsize(char *file_name)
{
    FRESULT error;
    UINT size = 0;
    FIL g_fileObject;   /* File object */
    error = f_open(&g_fileObject, _T(file_name), (FA_READ));
    if (error)
    {
        if (error == FR_EXIST)
        {
            FATFS_PRINTF(("File exists.\r\n"));
        }
        else
        {
            FATFS_PRINTF(("Open file  %s failed: %d.\r\n", file_name, error));
            return 0;
        }
    }

    size = f_size(&g_fileObject);

    if (f_close(&g_fileObject))
    {
        FATFS_PRINTF(("\r\nClose file failed.\r\n"));
    }

    return size;
}

int fatfs_delete(const char *file_name)
{
    FRESULT error;
    error = f_unlink(file_name);
    if (error)
    {
        FATFS_PRINTF(("Delete %s failed: %d.\r\n", file_name, error));
        return -1;
    }
    FATFS_PRINTF(("Delete %s succeed.\r\n", file_name));

    return 0;
}

#if 0
int fatfs_rename(const char *old_name, const char *new_name)
{
    FRESULT error;

    error = f_rename(old_name, new_name);
    if (error)
    {
        FATFS_PRINTF(("Rename %s -> %s failed: %d.\r\n", old_name, new_name, error));
        return -1;
    }

    return 0;
}

int fatfs_mkdir(const char *dir_name)
{
    FRESULT error;

    //    FATFS_PRINTF("\r\nCreate directory......\r\n");
    error = f_mkdir(_T(dir_name));
    if (error)
    {
        if (error == FR_EXIST)
        {
            FATFS_PRINTF(("Directory exists.\r\n"));
        }
        else
        {
            FATFS_PRINTF(("Make directory failed: %d.\r\n", error));
            return -1;
        }
    }

    return 0;
}
#endif
