/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_LPM_H_
#define _SLN_LPM_H_

#include "fsl_common.h"
#include "lpm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef enum _app_wakeup_source
{
    kAPP_WakeupSourceGPT, /*!< Wakeup by PIT.        */
    kAPP_WakeupSourcePin, /*!< Wakeup by external pin. */
} app_wakeup_source_t;

int SLN_LPM_Init(void);
int SLN_PowerModeSwitch(lpm_power_mode_t targetPowerMode);

#ifdef __cplusplus
}
#endif

#endif /* _SLN_LPM_H_ */
