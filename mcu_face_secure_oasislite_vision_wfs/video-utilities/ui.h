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

#ifndef _UI_H_
#define _UI_H_

#if RT106F_ELOCK_BOARD

#ifdef __cplusplus
extern "C" {
#endif

#define UI_MSG_Q_COUNT 4

void UI_Start(void);
void UI_UpdateFrame(void);
void UI_UpdateFace(unsigned char* pFace);
void UI_AddName(const char* newName);
void UI_RefreshNames(void);
void UI_DelSelName(void);
void UI_ShowFoundName(const char* name, float similar);
void UI_ResetDialog(void);
void UI_DelExistName(const char* oldName);

int UI_SendQMsg(void* msg);

#ifdef __cplusplus
}
#endif

#endif
#endif /* _UI_H_ */
