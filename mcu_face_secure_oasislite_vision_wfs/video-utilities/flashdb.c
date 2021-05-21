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
#include "flashdb.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "commondef.h"
#include "sln_flash.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PAGE_ADDR(x) ((x) * FACEREC_FS_FLASH_PAGE_SIZE)
#define SECTOR_ADDR(x) ((x) * FLASH_SECTOR_SIZE)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static uint8_t data_cache[FLASH_SECTOR_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t Flash_FacerecFsEraseMapBlock(void)
{
    status_t status = SLN_Erase_Sector(FACEREC_FS_MAP_ADDR);
    if (status != FLASH_OK) {
        return FLASH_ERR;
    }

    return FLASH_OK;
}

uint32_t Flash_FacerecFsEraseItemBlock(void)
{
    status_t status;

    for (int i = 0; i < FACEREC_FS_SECTORS; i++) {
        status = SLN_Erase_Sector(FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(i));
        if (status != FLASH_OK) {
            return FLASH_ERR;
        }
    }

    return FLASH_OK;
}

uint32_t Flash_FacerecFsEraseAllBlock(void)
{
    status_t status;
    status = Flash_FacerecFsEraseMapBlock();
    if (status != FLASH_OK) {
        return FLASH_ERR;
    }
    status = Flash_FacerecFsEraseItemBlock();

    if (status != FLASH_OK) {
        return FLASH_ERR;
    }

    return FLASH_OK;
}

uint32_t Flash_FacerecFsReadMapMagic(FeatureMap *pMap)
{
    status_t status;

    int size_page = (sizeof(FeatureMap) % FACEREC_FS_FLASH_PAGE_SIZE) ?
                    (sizeof(FeatureMap) / FACEREC_FS_FLASH_PAGE_SIZE + 1) :
                    (sizeof(FeatureMap) / FACEREC_FS_FLASH_PAGE_SIZE);

    status = SLN_Read_Flash_At_Address(FACEREC_FS_MAP_ADDR, (uint8_t *) pMap,
            size_page * FACEREC_FS_FLASH_PAGE_SIZE);
    if (status != FLASH_OK) {
        return FLASH_ERR;
    }

    return FLASH_OK;
}

uint8_t Flash_FacerecFsReadMagicValue(int index)
{
    status_t status;
    uint8_t magic_temp[4];

    status = SLN_Read_Flash_At_Address(FACEREC_FS_MAP_ADDR + (index & ~0x03), magic_temp, 4);
    if (status != FLASH_OK) {
        return -1;
    }

    return magic_temp[index % 4];
}

uint32_t Flash_FacerecFsReadItem(int index, FeatureItem * pItem)
{
    status_t status;

    int size_page = (sizeof(FeatureItem) % FACEREC_FS_FLASH_PAGE_SIZE) ?
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE + 1) :
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE);
    int offset_page = index * size_page;

    status = SLN_Read_Flash_At_Address(FACEREC_FS_ITEM_ADDR + PAGE_ADDR(offset_page),
            (uint8_t *) pItem, size_page * FACEREC_FS_FLASH_PAGE_SIZE);
    if (status != FLASH_OK) {
        return FLASH_ERR;
    }

    return FLASH_OK;
}

uint8_t Flash_FacerecFsReadItemMagicValue(int index)
{
    status_t status;
    uint8_t magic_temp[4];

    int size_page = (sizeof(FeatureItem) % FACEREC_FS_FLASH_PAGE_SIZE) ?
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE + 1) :
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE);
    int offset_page = index * size_page;

    status = SLN_Read_Flash_At_Address(FACEREC_FS_ITEM_ADDR + PAGE_ADDR(offset_page), magic_temp, 4);
    if (status != FLASH_OK) {
        return -1;
    }

    return magic_temp[0];

}

uint32_t Flash_FacerecFsUpdateMapMagic(int index, FeatureMap *pMap)
{
    status_t status;

    uint8_t magic_value = Flash_FacerecFsReadMagicValue(index);

    /* checkout if need change magic */
    if (magic_value == pMap->magic[index]) {
        return FLASH_OK;
    } else if (!(~magic_value &  pMap->magic[index])) {
        status = SLN_Write_Flash_Page(FACEREC_FS_MAP_ADDR, (uint8_t *) pMap,
                FACEREC_FS_FLASH_PAGE_SIZE);
        if (status != FLASH_OK) {
            return FLASH_ERR;
        }
    } else {
        SLN_Erase_Sector(FACEREC_FS_MAP_ADDR);
        status = SLN_Write_Flash_Page(FACEREC_FS_MAP_ADDR, (uint8_t *) pMap,
                FACEREC_FS_FLASH_PAGE_SIZE);
        if (status != FLASH_OK) {
            return FLASH_ERR;
        }
    }

    return FLASH_OK;
}

uint32_t Flash_FacerecFsUpdateItemMagic(int index, uint8_t flag)
{
    status_t status;
    uint8_t magic_value;

    int size_page = (sizeof(FeatureItem) % FACEREC_FS_FLASH_PAGE_SIZE) ?
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE + 1) :
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE);
    int offset_page = index * size_page;
    int offset_sector = offset_page / FLASH_NPAGE_PER_SECTOR;

    magic_value = Flash_FacerecFsReadItemMagicValue(index);

    /* checkout if need change magic */
    if (magic_value == flag) {
        return FLASH_OK;
    } else if (!(~magic_value & flag)) {
        SLN_Read_Flash_At_Address(FACEREC_FS_ITEM_ADDR + PAGE_ADDR(offset_page), data_cache, FACEREC_FS_FLASH_PAGE_SIZE);

        data_cache[0] = flag;

        status = SLN_Write_Flash_Page(FACEREC_FS_ITEM_ADDR + PAGE_ADDR(offset_page),
                (uint8_t *) data_cache,
                FACEREC_FS_FLASH_PAGE_SIZE);
        if (status != FLASH_OK) {
            return FLASH_ERR;
        }
    } else {
        int offset_in_sector = (offset_page - offset_sector * FLASH_NPAGE_PER_SECTOR) * FACEREC_FS_FLASH_PAGE_SIZE;

        SLN_Read_Flash_At_Address(FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector), data_cache, FLASH_SECTOR_SIZE);

        data_cache[offset_in_sector] = flag;

        SLN_Erase_Sector(FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector));

        for (int i = 0; i < FLASH_SECTOR_SIZE / FACEREC_FS_FLASH_PAGE_SIZE; i++) {
            status = SLN_Write_Flash_Page(
                    FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector) + PAGE_ADDR(i),
                    (uint8_t *) data_cache + i * FACEREC_FS_FLASH_PAGE_SIZE,
                    FACEREC_FS_FLASH_PAGE_SIZE);
            if (status != FLASH_OK) {
                return FLASH_ERR;
            }
        }
    }

    return FLASH_OK;

}

uint32_t Flash_FacerecFsUpdateItem(int index, FeatureItem *pItem)
{
    status_t status;
    uint8_t magic_value;

    int size_page = (sizeof(FeatureItem) % FACEREC_FS_FLASH_PAGE_SIZE) ?
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE + 1) :
                    (sizeof(FeatureItem) / FACEREC_FS_FLASH_PAGE_SIZE);
    int offset_page = index * size_page;
    int offset_sector = offset_page / FLASH_NPAGE_PER_SECTOR;

    magic_value = Flash_FacerecFsReadItemMagicValue(index);

    if (magic_value == FEATUREDATA_MAGIC_UNUSE) {
        for (int i = 0; i < size_page; i++) {
            status = SLN_Write_Flash_Page(FACEREC_FS_ITEM_ADDR + PAGE_ADDR(offset_page + i),
                    (uint8_t *) pItem + i * FACEREC_FS_FLASH_PAGE_SIZE,
                    FACEREC_FS_FLASH_PAGE_SIZE);
            if (status != FLASH_OK) {
                return FLASH_ERR;
            }
        }
    } else {
        int offset_in_sector = (offset_page - offset_sector * FLASH_NPAGE_PER_SECTOR) * FACEREC_FS_FLASH_PAGE_SIZE;

        SLN_Read_Flash_At_Address(FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector), data_cache, FLASH_SECTOR_SIZE);

        memcpy(&data_cache[offset_in_sector], pItem, sizeof(FeatureItem));

        SLN_Erase_Sector(FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector));

        for (int i = 0; i < FLASH_SECTOR_SIZE / FACEREC_FS_FLASH_PAGE_SIZE; i++) {
            status = SLN_Write_Flash_Page(
                    FACEREC_FS_ITEM_ADDR + SECTOR_ADDR(offset_sector) + PAGE_ADDR(i),
                    (uint8_t *) data_cache + i * FACEREC_FS_FLASH_PAGE_SIZE,
                    FACEREC_FS_FLASH_PAGE_SIZE);
            if (status != FLASH_OK) {
                return FLASH_ERR;
            }
        }

    }

    return FLASH_OK;
}

#if 0
#include "test_db_bin.h"

FeatureData featureData_dump;

int main(void)
{
    FeatureData featureData_t;

    // copy the map
    memcpy(&featureData_t.map, test_db_bin, sizeof(featureData_t.map));

    // copy the items
    memcpy(&featureData_t.item, test_db_bin + 0x1000, sizeof(featureData_t.item));

    BOARD_ConfigMPU();

    BOARD_InitDEBUG_UARTPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

#if RTVISION_BOARD
#ifndef FLASH_XIP_SIMULTANEOUS_FS
    BOARD_InitHyperflashPins();
#endif
#endif

    /* flash init */
    Flash_FacerecFsInit();

    /* erase all bank */
    Flash_FacerecFsEraseAlllBlock();

    for (int i = 0; i < 5; i++) {
        Flash_FacerecFsUpdateMapMagic(i, &featureData_t);
        Flash_FacerecFsUpdateItem(i, &featureData_t);
    }

    for (int i = 0; i < 5; i++) {
        Flash_FacerecFsReadMapMagic(&featureData_dump.map);
        Flash_FacerecFsReadItem(i, &featureData_dump.item[i]);
    }

    while(1);

    return 0;
}
#endif

