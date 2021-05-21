/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_DEV_CFG_H_
#define _SLN_DEV_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sln_flash.h"
#include "sln_flash_mgmt.h"

/** The flash file where the configuration data will be stored */
#define CONFIG_DATA_FILE_NAME     "dev_cfg.dat"
#define PULSE_WIDTH_NOTSUPPORTED 101

/**
 * A structure to represent the configuration data
 */
typedef struct _sln_cfg_data
{
    uint8_t usb_shell_verbose;  /**< Enable/Disable usb shell verbosity */
    uint8_t enrolment_mode;     /**< The enrolment mode */
    uint8_t camera_irfilter_mode;
    uint8_t camera_ir_pulse_width;  /** pwm for ir led*/
    uint8_t camera_white_pulse_width; /** pwm for white led with dual camera*/
    /* NO_EMOTION_RECOGNITION = 0, EMOTION_2_TYPES = 2,  EOTION_4_TYPES = 4, MOTION_7_TYPES = 7,*/
    uint8_t emotion_types;
    uint8_t liveness_mode;  /**< Enable/Disable IR liveness feature */
    uint8_t detect_resolution_mode;  /**< qvga/vga detection resolution mode */
    uint8_t app_type;       /* ELOCK or DOOR_ACCESS application type*/
    uint8_t display_mode;   /**  Display RGB/IR camera frame */

} sln_cfg_data_t;

typedef enum _cfg_enrolment{
	ENROLMENT_MODE_MANUAL   =   0,
	ENROLMENT_MODE_AUTO    =   1
}cfg_enrolment_t;


typedef enum _cfg_verbose{
	VERBOSE_MODE_OFF    =   0,  /**< Verbosity off */
	VERBOSE_MODE_L1     =   1,  /**< Verbosity low (show critical messages) */
	VERBOSE_MODE_L2     =   2,  /**< Verbosity medium (show detailed messages) */
	VERBOSE_MODE_L3     =   3   /**< Verbosity high (show all messages) */
}cfg_verbose_t;

typedef enum _cfg_irfilter{
	IRFILTER_MODE_OFF   =   0,
	IRFILTER_MODE_ON    =   1,
	IRFILTER_MODE_NOTSUPPORTED    =  2,
}cfg_irfilter_t;

typedef enum _cfg_liveness{
	LIVENESS_MODE_OFF   =   0,
	LIVENESS_MODE_ON    =   1
}cfg_liveness_t;

typedef enum _cfg_detresolution{
	DETECT_RESOLUTION_QVGA   =   0,
	DETECT_RESOLUTION_VGA    =   1
}cfg_detresolution_t;

typedef enum _cfg_displaymode{
	DISPALY_MODE_RGB   =   0,
	DISPALY_MODE_IR    =   1
}cfg_displaymode_t;

typedef enum _cfg_led{
	LED_WHITE   =   0,
	LED_IR    =   1,
	LED_NUM,
}cfg_led_t;

typedef enum _cfg_apptype{
    APP_TYPE_ELOCK_LIGHT   =   0,
    APP_TYPE_ELOCK_HEAVY =   1,
    APP_TYPE_DOOR_ACCESS_LIGHT   =   2,
    APP_TYPE_DOOR_ACCESS_HEAVY = 3
}cfg_apptype_t;


/**
 * @brief Reads config data from flash memory
 *
 * @param cfg[out]     Pointer to a pre-allocated sln_cfg_data_t structure where the data will be stored
 * @return             Status of read
 */
int32_t CfgData_FlashRead(sln_cfg_data_t *cfg);

/**
 * @brief Writes config data in flash memory
 *
 * @param cfg[in]      Pointer to a sln_cfg_data_t structure containing the data that will be written
 * @return             Status of write
 */
int32_t CfgData_FlashWrite(sln_cfg_data_t *cfg);

/**
 * @brief Initializes config data with default values
 *
 * @param cfg[out]     Pointer to a pre-allocated sln_cfg_data_t structure where the data will be stored
 */
void CfgData_GetDefault(sln_cfg_data_t *cfg);

/**
 * @brief Initializes the user application configuration data
 */
void Cfg_AppDataInit();

/**
 * @brief Reads application config data
 *
 * @param cfg[out]     Pointer to a pre-allocated sln_cfg_data_t structure where the data will be stored
 * @return             Status of read
 */
void Cfg_AppDataRead(sln_cfg_data_t *cfg);

/**
 * @brief Saves application config data
 *
 * @param cfg[in]      Pointer to a sln_cfg_data_t structure containing the data that will be saved
 * @return             Status of save
 */
int32_t Cfg_AppDataSave(sln_cfg_data_t *cfg);

/**
 * @brief Reads application config usb_shell_verbose

 * @return             config value
 */
uint8_t Cfg_AppDataGetVerbosity();

/**
 * @brief Reads application config auto_add_faces

 * @return             config value
 */
uint8_t Cfg_AppDataGetEnrolmentMode();

/**
 * @brief Reads application config camera_irfilter_mode

 * @return             config value
 */
uint8_t Cfg_AppDataGetCameraIRFilterMode();

/**
 * @brief Reads application config camera_ir_pulse_width

 * @return             config value
 */
uint8_t Cfg_AppDataGetCameraIRPulseWidth();

/**
 * @brief Reads application config camera_white_pulse_width

 * @return             config value
 */
uint8_t Cfg_AppDataGetCameraWhitePulseWidth();

/**
 * @brief Reads application config emotion_rec_types

 * @return             config value
 */
uint8_t Cfg_AppDataGetEmotionRecTypes();

/**
 * @brief Reads application config t liveness_mode

 * @return             config value
 */
uint8_t Cfg_AppDataGetLivenessMode();

/**
 * @brief Reads application config t display_mode

 * @return             config value
 */
uint8_t Cfg_AppDataGetDisplayMode();

/**
 * @brief Reads application config t detect_resolution_mode

 * @return             config value
 */
uint8_t Cfg_AppDataGetDetectResolutionMode();

/**
 * @brief Reads application config t app_type

 * @return             config value
 */
uint8_t Cfg_AppDataGetApplicationType();

#ifdef __cplusplus
}
#endif

#endif /* _SLN_DEV_CFG_H_ */
