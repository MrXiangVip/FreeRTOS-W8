/*
 * Copyright 2018 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_FLASH_MGMT_
#define _SLN_FLASH_MGMT_

/*!
 * SLN Flash Management
 * 
 * Sector Size: 256 KBytes
 * Page Size:  512 Bytes
 * 
 * One Sector per File
 * 
 * Sector Layout:
 * 
 * Page #   | Usage
 * ---------|------------------
 *    0     | Sector Map
 *    1     | File {Header, Data}
 *    2     | File {Header, Data}
 *    ...   | ...
 *    255   | File {Header, Data}
 *
 *  New File entries are page aligned; some wasted space may occur
 *
 */

#include <stdint.h>

#if RTVISION_BOARD
#include "flexspi_hyper_flash.h"
#define SLN_FLASH_MGMT_BASE_ADDR        (0x1700000)
#define SLN_FLASH_MGMT_LAST_ADDR         (0x1F40000)
#elif RT106F_ELOCK_BOARD
#include "flexspi_qspi_flash.h"
#define SLN_FLASH_MGMT_BASE_ADDR        (0x800000) //(0x560000)
#define SLN_FLASH_MGMT_LAST_ADDR        (0x820000)//(0x580000)
#endif

#define SLN_FLASH_MGMT_SIZE_BYTES       (SLN_FLASH_MGMT_LAST_ADDR - SLN_FLASH_MGMT_BASE_ADDR)
#define SLN_FLASH_MGMT_SECTOR_SIZE		(SECTOR_SIZE)
#define SLN_FLASH_MGMT_FILE_ADDR(x)     (SLN_FLASH_MGMT_BASE_ADDR + (x * SLN_FLASH_MGMT_SECTOR_SIZE))

#define SLN_FLASH_MGMT_FILE_NAME_LEN    (30U)

#define SLN_FLASH_MGMT_MAP_OLD          (0x00)
#define SLN_FLASH_MGMT_MAP_CURRENT      (0xAA)
#define SLN_FLASH_MGMT_MAP_FREE         (0xFF)

/* Define error codes */
typedef enum _sln_flash_mgmt_status
{
    SLN_FLASH_MGMT_OK         =  0x00,
    SLN_FLASH_MGMT_ENOLOCK    = -0x01,
    SLN_FLASH_MGMT_ERETRY     = -0x10,
    SLN_FLASH_MGMT_ENOMEM     = -0x20,
    SLN_FLASH_MGMT_ENOMEM2    = -0x21,
    SLN_FLASH_MGMT_ENOMEM3    = -0x22,
    SLN_FLASH_MGMT_ENOMEM4    = -0x23,
    SLN_FLASH_MGMT_EIO        = -0x30,
    SLN_FLASH_MGMT_EINVAL     = -0x40,
    SLN_FLASH_MGMT_EINVAL2    = -0x41,
    SLN_FLASH_MGMT_EINVAL3    = -0x42,
    SLN_FLASH_MGMT_ENOENTRY   = -0x50,
    SLN_FLASH_MGMT_ENOENTRY2  = -0x51,
    SLN_FLASH_MGMT_ENOENTRY3  = -0x52,
    SLN_FLASH_MGMT_EOVERFLOW  = -0x60,
    SLN_FLASH_MGMT_EOVERFLOW2 = -0x61,
    SLN_FLASH_MGMT_EENCRYPT   = -0x70,
    SLN_FLASH_MGMT_EENCRYPT2  = -0x71,
} sln_flash_mgmt_status_t;

/* Use the flash API with or without encryption */
#define SLN_FLASH_PLAIN (false)
#define SLN_FLASH_ENCRYPTED (true)

typedef struct _sln_flash_entry
{
    char name[SLN_FLASH_MGMT_FILE_NAME_LEN + 1];
    uint32_t address;
    bool isEncrypted;
} sln_flash_entry_t;

typedef struct _sln_flash_map
{
    uint8_t map[FLASH_PAGE_SIZE - 1];
    uint8_t reserved;
} sln_flash_map_t;

typedef struct _sln_file_header
{
    uint32_t valid:1;       /*! Indicates this file is valid to read */
    uint32_t clean:1;       /*! Indicates this file has been updated in place, if 0 CRC is invalid in NVM */
    uint32_t sizeBytes:14;  /*! Limits to 16 KB file size */
    uint32_t reserved:16;   /*! Padding */
    uint32_t crc:32;        /*! 32 bits of CRC */
}sln_file_header_t;

#if defined(__cplusplus)
extern "C"
{
#endif

/*!
 * @brief Initialize flash management; initializes private memory and file lock
 *
 * @param *flashEntries Pointer to array of flash entries, terminated with empty entry
 * @param erase Flag to indicate whether to erase all entries during initilialization
 *
 * @returns Status of initialization
 *
 */
int32_t SLN_FLASH_MGMT_Init(sln_flash_entry_t *flashEntries, uint8_t erase);

/*!
 * @brief Save to a named entry
 *
 * @param name String name of entry/file to save data into
 * @param data Pointer to data to save to file
 * @param len Length in bytes to save
 *
 * @returns Status of save
 */
int32_t SLN_FLASH_MGMT_Save(const char *name, uint8_t *data, uint32_t len);

/*!
 * @brief Update a named entry [can only clear bits as per nature of Flash Memory] [can't increase length, will fail]
 *
 * @param name String name of entry/file to update data
 * @param data Pointer to data to update to file
 * @param len Pointer for length in bytes to update, output back to user to know files size on NVM
 *
 * @returns Status of save [will fail with SLN_FLASH_MGMT_ENOENTRY2 if no previous save]
 */
int32_t SLN_FLASH_MGMT_Update(const char *name, uint8_t *data, uint32_t *len);

/*!
 * @brief Read from a named entry
 *
 * Usage:
 * @code
 *
 *      int32_t ret = SLN_FLASH_MGMT_OK;
 *      uint8_t *data = NULL;
 *      uint32_t len = 0;
 * 
 *      ret = SLN_FLASH_MGMT_Read("file.dat", NULL, &len);
 * 
 *      if (SLN_FLASH_MGMT_OK == ret)
 *      {
 *          // len is set to size in bytes of file data in Flash
 *          data = (uint8_t *)pvPortMalloc(len);
 *          if (NULL == data)
 *          {
 *              // Handle per application spec
 *          }
 *      }
 *      else
 *      {
 *          // Handle per application spec
 *      }
 * 
 *      // Before calling len can be reduced to read less data, 
 *      // but cannot be increased beyond size in file header (function will reduce len param value)
 *      ret = SLN_FLASH_MGMT_Read("file.dat", data, &len);
 * 
 *      if (SLN_FLASH_MGMT_OK == ret)
 *      {
 *          // Data is read correctly
 *      }
 *      else
 *      {
 *          // Handle per application spec
 *      }
 *
 * @endcode
 * 
 * @param name String name of entry/file to read from
 * @param data Pointer to data to copy data into
 * @param len Pointer for length in bytes to read
 *
 * @returns Status of read
 */
int32_t SLN_FLASH_MGMT_Read(const char *name, uint8_t *data, uint32_t *len);

/*!
 * @brief Completely erase an entry [sector erase]
 *
 * @param name String name of entry/file to erase
 *
 * @retuns Status of the erase
 */
int32_t SLN_FLASH_MGMT_Erase(const char *name);

/*!
 * @brief De-initialize the file management
 *
 * @param *flashEntries Pointer to the list of entries to de-initialize
 * @param erase Flag to indicate whether on not to erase each entry/sector during de-init
 *
 * @returns Status of de-initialization
 */
int32_t SLN_FLASH_MGMT_Deinit(sln_flash_entry_t *flashEntries, uint8_t erase);

#if defined(__cplusplus)
}
#endif

#endif /* _SLN_FLASH_MGMT_ */
