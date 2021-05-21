/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if RTVISION_BOARD
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_FLEXSPI FLEXSPI
#define FLASH_SIZE 0x2000000
#define EXAMPLE_FLEXSPI_AMBA_BASE FlexSPI_AMBA_BASE
#define FLASH_PAGE_SIZE 512
#define EXAMPLE_SECTOR 2
#define SECTOR_SIZE 0x40000
#define EXAMPLE_FLEXSPI_CLOCK kCLOCK_FlexSpi
#define HYPERFLASH_CMD_LUT_SEQ_IDX_READDATA 0
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITEDATA 1
#define HYPERFLASH_CMD_LUT_SEQ_IDX_READSTATUS 2
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITEENABLE 4
#define HYPERFLASH_CMD_LUT_SEQ_IDX_ERASESECTOR 6
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITE_BUF_SEQ2 10
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITE_BUF_SEQ3 11
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITE_BUF_SEQ4 12
#define HYPERFLASH_CMD_LUT_SEQ_IDX_WRITE_BUF_SEQ5 13
#define HYPERFLASH_CMD_LUT_SEQ_IDX_READVCR 14

#if 0 // Currently unavailable
#define HYPERFLASH_CMD_LUT_SEQ_IDX_PROGVCR //TBD
#endif

#define CUSTOM_LUT_LENGTH 64

extern const uint32_t customLUT[CUSTOM_LUT_LENGTH];

status_t flexspi_nor_hyperbus_read(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes);

status_t flexspi_nor_hyperbus_write(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes);

status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr);

status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base);

status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);

status_t flexspi_nor_flash_page_program_with_buffer(FLEXSPI_Type *base, uint32_t address, const uint32_t *src);

status_t flexspi_nor_read_vcr(FLEXSPI_Type *base, uint32_t *vcr);

status_t flexspi_nor_write_vcr(FLEXSPI_Type *base, uint32_t *vcr);

status_t flexspi_nor_hyperflash_cfi(FLEXSPI_Type *base);

status_t flexspi_nor_hyperflash_id(FLEXSPI_Type *base, uint8_t *pid);

#endif
