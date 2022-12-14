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

#ifndef _CAMERA_RT106F_ELOCK_3D_H_
#define _CAMERA_RT106F_ELOCK_3D_H_

#if RT106F_ELOCK_BOARD && CAMERA_SUPPORT_3D

#ifdef __cplusplus
extern "C" {
#endif


/* Camera definition. */
#define APP_CAMERA_CONTROL_FLAGS (kCAMERA_VsyncActiveLow | kCAMERA_HrefActiveLow | kCAMERA_DataLatchOnRisingEdge)

#define APP_CAMERA_HIMAX 0

#define APP_CAMERA_TYPE  APP_CAMERA_HIMAX

/* Frame buffer data alignment. */
#define FRAME_BUFFER_ALIGN 64

#define APP_FRAME_BUFFER_COUNT 4

/* Pixel format RGB565, bytesPerPixel is 2. */
#define APP_BPP 2

#define CAMERA_MSG_Q_COUNT 10

#if (APP_CAMERA_TYPE != APP_CAMERA_OV7725)
void BOARD_I2C_ReleaseBus(void);
#endif

void BOARD_InitCameraResource(void);

void Camera_ELCDIF_Init(void);

void Camera_InitPxp(void);

int Camera_Start();
int Camera_SendQMsg(void* msg);
int Camera_QMsgSetPWM(uint8_t led, uint8_t pulse_width);
void Camera_GetPWM(uint8_t led, uint8_t* pulse_width);
int Camera_SetMonoMode(uint8_t enable );
uint8_t Camera_GetDispMode(void);
int Camera_SetDispMode(uint8_t displayMode );
int Camera_SelectLED(uint8_t led );
uint8_t Camera_GetExposureMode(void);
int Camera_QMsgSetExposureMode(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif
#endif /* _CAMERA_RT106F_ELOCK_3D_H_ */
