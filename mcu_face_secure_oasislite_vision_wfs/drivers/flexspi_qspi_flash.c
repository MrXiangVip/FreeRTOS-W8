/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if RT106F_ELOCK_BOARD

#include "fsl_debug_console.h"
#include "fsl_flexspi.h"

#include "board.h"
#include "clock_config.h"
#include "flexspi_qspi_flash.h"
#include "fsl_common.h"
#include "pin_mux.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
const uint32_t customLUT[NORFLASH_LUT_LENGTH] = {
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadData,
                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD,NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadData,
                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD,NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08,
                        kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadQuad,
                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD,NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x08,
                        kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteEnable,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgramQuad,
                        kFLEXSPI_Command_RADDR_SDR,kFLEXSPI_1PAD, NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    [4 * NOR_CMD_LUT_SEQ_IDX_READJEDECID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ManufactDeviceID,
                        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_READJEDECID + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg1,
                        kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg2,
                        kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteStatusReg2,
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ChipErase,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_SectorErase,
                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD,NORFLASH_ADDR_LENGTH),

    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgram,
                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, NORFLASH_ADDR_LENGTH),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04,
                        kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0)
};

/*******************************************************************************
 * Code
 ******************************************************************************/
__attribute__((section(".data.$SRAM_DTC")))
static status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write neable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

__attribute__((section(".data.$SRAM_DTC")))
static status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base)
{
    /* Wait status ready. */
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }
        if (FLASH_BUSY_STATUS_POL)
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }

    } while (isBusy);

    return status;
}

__attribute__((section(".data.$SRAM_DTC")))
status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base)
{
    flexspi_transfer_t flashXfer;
    status_t status;
    uint32_t writeValue = 0x40;

    /* Write neable */
    status = flexspi_nor_write_enable(base, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2;
    flashXfer.data          = &writeValue;
    flashXfer.dataSize      = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

__attribute__((section(".data.$SRAM_DTC")))
status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

__attribute__((section(".data.$SRAM_DTC")))
status_t flexspi_nor_flash_page_program_with_buffer(FLEXSPI_Type *base, uint32_t address, const uint32_t *src)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write neable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Prepare page program command */
    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data          = (uint32_t *)src;
    flashXfer.dataSize      = FLASH_PAGE_SIZE;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

__attribute__((section(".data.$SRAM_DTC")))
status_t flexspi_nor_qspi_flash_id(FLEXSPI_Type *base, uint16_t pid)
{
    uint16_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READJEDECID;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 2;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (status != kStatus_Success)
    {
       return status;
    }

    if(readValue == pid)
        return kStatus_Success;
    else
        return kStatus_Fail;
}

#endif
