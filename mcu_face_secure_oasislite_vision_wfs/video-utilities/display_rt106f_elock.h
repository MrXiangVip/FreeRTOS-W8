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

#ifndef _DISPLAY_RT106F_ELOCK_H_
#define _DISPLAY_RT106F_ELOCK_H_

#if RT106F_ELOCK_BOARD

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_BUFFERS 2
int Display_SendQMsg(void* msg);
int Display_Start();
void Display_Init_Task(void* param);
int Display_Sync();
int Display_Update(uint32_t backBuffer, uint8_t p_isLiveView);
void BOARD_TurnOnLcd();
void BOARD_TurnOffLcd();

#ifdef __cplusplus
}
#endif

#endif
#endif /* _DISPLAY_RT106F_ELOCK_H_ */
