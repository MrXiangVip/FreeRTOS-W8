/*
 * Copyright 2020 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#ifndef _FSL_DVP_MIPI_H_
#define _FSL_DVP_MIPI_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ********************************************"**********************************/
#define CAM_BRIDGE_CHIP  "TC358746AXBG"
#define CAM_BRIDGE_I2C_SLAVE_ADDR 0x0E
#define CAM_BRIDGE_CHIP_ID 0x4401
#define CAM_BRIDGE_REG_CHIP_ID 0x0000
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif
status_t DVPMIPI_Init(void);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_DVP_MIPI_H_ */
