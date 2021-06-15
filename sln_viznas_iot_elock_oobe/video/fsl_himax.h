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

#ifndef _FSL_HIMAX_H_
#define _FSL_HIMAX_H_

#include "fsl_common.h"
#include "commondef.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CFG_CAM_WIDTH                             2560
#define CFG_CAM_HEIGHT                            800

#define SH430MH_NIR_CAM_I2C_SLAVE_ADDR_7BIT 0x37

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Himax frame sequence mode selection ID */
typedef enum hx_frame_mode_select {
	HX_FRAME_MODE_SELECT_ONLY_NIR,
	HX_FRAME_MODE_SELECT_ONLY_DOT,
	HX_FRAME_MODE_SELECT_ONLY_DEPTH,
	HX_FRAME_MODE_SELECT_ALT_NIR_DEPTH,
	HX_FRAME_MODE_SELECT_ALT_NIR_DEPTH_DOT,
	HX_FRAME_MODE_SELECT_UNKNOWN = 0xFF,
} hx_frame_mode_select_t;

/* Himax frame sequence mode selection setting structure */
typedef struct hx_frame_mode_select_setting {
	uint8_t mode_id;
	uint16_t cmd_len;
	uint8_t cmd[6];
} hx_frame_mode_select_setting_t;

enum {
	FRAME_TYPE_IR = 0,
	FRAME_TYPE_DEPTH = 1,
	FRAME_TYPE_ANY = 0xFF
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif
status_t hx_Init(void);
status_t hx_Deinit(void);
uint8_t hx_cam_ctrl_get_frame_type(const uint8_t *in_data, uint16_t width, uint16_t height);
#if defined(__cplusplus)
}
#endif

#endif /* _FSL_HIMAX_H_ */
