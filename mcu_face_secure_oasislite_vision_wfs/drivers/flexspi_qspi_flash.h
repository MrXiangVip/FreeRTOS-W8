/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if RT106F_ELOCK_BOARD

#ifndef _FLEXSPI_QSPI_FLASH_H_
#define _FLEXSPI_QSPI_FLASH_H_

#define EXAMPLE_FLEXSPI FLEXSPI
#define EXAMPLE_FLEXSPI_CLOCK kCLOCK_FlexSpi
#define EXAMPLE_FLEXSPI_AMBA_BASE FlexSPI_AMBA_BASE

#define FLASH_SIZE 0x800000U//0x2000 /* 64Mb/KByte */
#define FLASH_PAGE_SIZE 256
#define SECTOR_SIZE 0x1000 /* 4K */

#define CUSTOM_LUT_LENGTH 60
#define FLASH_QUAD_ENABLE 0x40
#define FLASH_BUSY_STATUS_POL 1
#define FLASH_BUSY_STATUS_OFFSET 0

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL                 0
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST                   1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD              2
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE                 3
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD            4
#define NOR_CMD_LUT_SEQ_IDX_READJEDECID                 5
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1              6
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2              7
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2             8
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP                   9
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR                 10
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM                 11

#define NORFLASH_SIZE                   8192

#define NORFLASH_LUT_LENGTH             48
#define NORFLASH_ADDR_LENGTH            24

#define W25Q_WriteEnable                0x06
#define W25Q_WriteDisable               0x04
#define W25Q_ReadStatusReg1             0x05
#define W25Q_ReadStatusReg2             0x35
#define W25Q_WriteStatusReg1            0x01
#define W25Q_WriteStatusReg2            0x31
#define W25Q_ReadData                   0x03
#define W25Q_FastReadData               0x0B
#define W25Q_FastReadDual               0x3B
#define W25Q_FastReadQuad               0x6B
#define W25Q_PageProgram                0x02
#define W25Q_PageProgramQuad            0x32
#define W25Q_BlockErase                 0xD8
#define W25Q_SectorErase                0x20
#define W25Q_ChipErase                  0xC7
#define W25Q_PowerDown                  0xB9
#define W25Q_ReleasePowerDown           0xAB
#define W25Q_DeviceID                   0xAB
#define W25Q_ManufactDeviceID           0x90
#define W25Q_JedecDeviceID              0x9F

status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);

status_t flexspi_nor_flash_page_program_with_buffer(FLEXSPI_Type *base, uint32_t address, const uint32_t *src);

status_t flexspi_nor_qspi_flash_id(FLEXSPI_Type *base, uint16_t pid);

#endif
#endif
