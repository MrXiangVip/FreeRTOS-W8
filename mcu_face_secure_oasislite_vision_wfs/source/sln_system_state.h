/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SYSTEM_STATE_H_
#define _SYSTEM_STATE_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * GLobal  Declarations
 ******************************************************************************/
typedef enum _system_states
{
    sysStateNull = 0,
    sysStateIdle,
    sysStateDetectedNoUser,
    sysStateDetectedUnknownUser,
    sysStateDetectedKnownUser
} system_states_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void SysState_Init();
void SysState_Set(system_states_t state);

#if defined(__cplusplus)
}
#endif

#endif /* _SYSTEM_STATE_H_ */
