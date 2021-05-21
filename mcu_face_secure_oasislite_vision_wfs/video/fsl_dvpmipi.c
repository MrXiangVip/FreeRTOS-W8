#include "board.h"
#include "fsl_dvpmipi.h"

#include <stdio.h>

#include "fsl_log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _dvpmipi_reg {
	uint16_t reg_addr;
	uint16_t addr_len;
	uint16_t val;
	uint16_t val_len;
	uint32_t delay_ms;
} dvpmipi_reg_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const dvpmipi_reg_t dvpmipi_settings[] = {
	{0x0002, 2, 0x0001, 2, 1},
	{0x0002, 2, 0x0000, 2, 0},
	{0x0016, 2, 0x3063, 2, 0},
	{0x0018, 2, 0x0403, 2, 1},
	{0x0018, 2, 0x0413, 2, 0},
	{0x0020, 2, 0x0011, 2, 0},
	{0x0060, 2, 0x800A, 2, 0},
	{0x0006, 2, 0x00C8, 2, 0},
	{0x0008, 2, 0x0001, 2, 0},
	{0x0004, 2, 0x8174, 2, 0},
};

/*******************************************************************************
 * Code
 ******************************************************************************/

void DVPMIPI_DelayMs(uint32_t ms)
{
#if defined(FSL_RTOS_FREE_RTOS)
    TickType_t tick;

    tick = ms * configTICK_RATE_HZ / 1000;

    tick = (0 == tick) ? 1 : tick;

    vTaskDelay(tick);
#else
    uint32_t i;
    uint32_t loopPerMs = SystemCoreClock / 3000;

    while (ms--)
    {
        i = loopPerMs;
        while (i--)
        {
            __NOP();
        }
    }
#endif
}

static status_t DVPMIPI_Reset(void)
{
	/* Hard reset */
    GPIO_PinWrite(BOARD_3DCAMERA_DVPMIPI_RST_GPIO, BOARD_3DCAMERA_DVPMIPI_RST_GPIO_PIN, 0);
    DVPMIPI_DelayMs(1);
	GPIO_PinWrite(BOARD_3DCAMERA_DVPMIPI_RST_GPIO, BOARD_3DCAMERA_DVPMIPI_RST_GPIO_PIN, 0);
	DVPMIPI_DelayMs(50);
	GPIO_PinWrite(BOARD_3DCAMERA_DVPMIPI_RST_GPIO, BOARD_3DCAMERA_DVPMIPI_RST_GPIO_PIN, 1);
	DVPMIPI_DelayMs(45);
	return kStatus_Success;
}

status_t DVPMIPI_I2C_WriteReg(uint8_t addrType,
                            uint32_t reg,
                            uint8_t regWidth,
                            uint32_t value)
{
    uint8_t data[4];
    uint8_t i;
    uint32_t regTmp;

    if (2 == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    i = regWidth;
    while (i--)
    {
        data[i] = (uint8_t)value;
        value >>= 8;
    }

    return BOARD_3DCamera_I2C_Send(CAM_BRIDGE_I2C_SLAVE_ADDR, reg, addrType, data, regWidth);
}


static status_t DVPMIPI_I2C_ReadReg(uint8_t addrType,
                                uint32_t reg,
                                uint8_t regWidth,
                                void *value)
{
    uint8_t data[4];
    uint8_t i = 0;
    uint32_t regTmp;
    status_t status;

    if (2 == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    status = BOARD_3DCamera_I2C_Receive(CAM_BRIDGE_I2C_SLAVE_ADDR, reg, addrType, data, regWidth);

    if (kStatus_Success == status)
    {
        while (regWidth--)
        {
            ((uint8_t *)value)[i++] = data[regWidth];
        }
    }

    return status;
}


static status_t DVPMIPI_MultiWrite(const dvpmipi_reg_t regs[], uint32_t num)
{
    status_t status = kStatus_Success;

    for (uint32_t i = 0; i < num; i++)
    {
        status = DVPMIPI_I2C_WriteReg(regs[i].addr_len,regs[i].reg_addr, regs[i].val_len, regs[i].val);
        DVPMIPI_DelayMs(regs[i].delay_ms);
        if (kStatus_Success != status)
        {
            break;
        }
    }
    return status;
}

status_t DVPMIPI_Init(void)
{
	status_t status;
	uint16_t chip_id;

    DVPMIPI_Reset();
	/* Identify the device. */
	status = DVPMIPI_I2C_ReadReg(2, CAM_BRIDGE_REG_CHIP_ID, 2, &chip_id);

	if (kStatus_Success != status)
    {
        return status;
    }

	if (CAM_BRIDGE_CHIP_ID != chip_id)
    {
        return kStatus_Fail;
    }

    status = DVPMIPI_MultiWrite(dvpmipi_settings, ARRAY_SIZE(dvpmipi_settings));
	
	if (kStatus_Success != status) 
    {
        return status;
    }

    return status;
}
