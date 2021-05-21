/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */
#include "sln_dev_cfg.h"

#define SanityCheckFailed 1
#define SanityCheckOK     0
/** A structure that contains the user application config data */
static sln_cfg_data_t app_data;

static int32_t CfgData_SanityCheck(sln_cfg_data_t *cfg)
{
    int32_t status = SanityCheckOK;
    if (!(cfg->usb_shell_verbose >= VERBOSE_MODE_OFF && cfg->usb_shell_verbose <= VERBOSE_MODE_L3))
    {
        cfg->usb_shell_verbose = VERBOSE_MODE_OFF;
        status = SanityCheckFailed;
    }
    if(cfg->enrolment_mode != ENROLMENT_MODE_MANUAL && cfg->enrolment_mode != ENROLMENT_MODE_AUTO)
    {
        cfg->enrolment_mode = ENROLMENT_MODE_MANUAL;
        status = SanityCheckFailed;
    }
    if(cfg->camera_irfilter_mode != IRFILTER_MODE_OFF && cfg->camera_irfilter_mode != IRFILTER_MODE_ON)
    {
        cfg->camera_irfilter_mode = IRFILTER_MODE_OFF;
        status = SanityCheckFailed;
    }
    if(cfg->camera_ir_pulse_width < 0 || cfg->camera_ir_pulse_width > 100)
    {
        cfg->camera_ir_pulse_width = 0;
        status = SanityCheckFailed;
    }
    if(cfg->camera_white_pulse_width < 0 || cfg->camera_white_pulse_width > 100)
    {
        cfg->camera_white_pulse_width = 0;
        status = SanityCheckFailed;
    }
    if(cfg->emotion_types !=0 && cfg->emotion_types !=2 && cfg->emotion_types !=4 && cfg->emotion_types !=7)
    {
        cfg->emotion_types = 0;
        status = SanityCheckFailed;
    }
    if(cfg->liveness_mode != LIVENESS_MODE_OFF && cfg->liveness_mode != LIVENESS_MODE_ON)
    {
        cfg->liveness_mode = LIVENESS_MODE_OFF;
        status = SanityCheckFailed;
    }
    if(cfg->detect_resolution_mode != DETECT_RESOLUTION_QVGA && cfg->detect_resolution_mode != DETECT_RESOLUTION_VGA)
    {
        cfg->detect_resolution_mode = DETECT_RESOLUTION_QVGA;
        status = SanityCheckFailed;
    }
    if(cfg->app_type != APP_TYPE_ELOCK_LIGHT && cfg->app_type != APP_TYPE_ELOCK_HEAVY
        && cfg->app_type != APP_TYPE_DOOR_ACCESS_LIGHT && cfg->app_type != APP_TYPE_DOOR_ACCESS_HEAVY)
    {
        cfg->app_type = APP_TYPE_ELOCK_LIGHT;
        status = SanityCheckFailed;
    }
    return status;
}

int32_t CfgData_FlashRead(sln_cfg_data_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;
    uint32_t len = sizeof(sln_cfg_data_t);
    status = SLN_FLASH_MGMT_Read(CONFIG_DATA_FILE_NAME, (uint8_t *)cfg, &len);

    return status;
}

int32_t CfgData_FlashWrite(sln_cfg_data_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;

    status = SLN_FLASH_MGMT_Save(CONFIG_DATA_FILE_NAME, (uint8_t *)cfg, sizeof(sln_cfg_data_t));

    return status;
}

void CfgData_GetDefault(sln_cfg_data_t *cfg)
{
#if DUAL_CAMERA
    sln_cfg_data_t default_app_data = {0, 0 , 0, 20, 0, 0, 1, 1, 0, 0};
#else
    sln_cfg_data_t default_app_data = {0, 0 , 0, 0, 0, 0, 0, 1, 0, 0};
#endif
    memcpy(cfg,&default_app_data,sizeof(sln_cfg_data_t));
}

void Cfg_AppDataInit()
{
    int32_t status;

    /* At first boot SLN_FLASH_MGMT_ENOENTRY2 is returned because there are no data saved */
    status = CfgData_FlashRead(&app_data);
    if (SLN_FLASH_MGMT_OK != status)
    {
        CfgData_GetDefault(&app_data);
        if (SLN_FLASH_MGMT_ENOENTRY2 == status)
        {
            CfgData_FlashWrite(&app_data);
        }
    }
    status = CfgData_SanityCheck(&app_data);
    if (status == SanityCheckFailed)
    {
        CfgData_FlashWrite(&app_data);        
    }
}

void Cfg_AppDataRead(sln_cfg_data_t *cfg)
{
    memcpy(cfg,&app_data,sizeof(sln_cfg_data_t));
}

int32_t Cfg_AppDataSave(sln_cfg_data_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;

    status = SLN_FLASH_MGMT_Save(CONFIG_DATA_FILE_NAME, (uint8_t *)cfg, sizeof(sln_cfg_data_t));
    if (SLN_FLASH_MGMT_OK == status)
    {
        memcpy(&app_data,cfg,sizeof(sln_cfg_data_t));
    }

    return status;
}

uint8_t Cfg_AppDataGetVerbosity()
{
    return app_data.usb_shell_verbose;
}

uint8_t Cfg_AppDataGetEnrolmentMode()
{
    return app_data.enrolment_mode;
}

uint8_t Cfg_AppDataGetCameraIRFilterMode()
{
#if !FOXLINK_CAMERA
	return IRFILTER_MODE_NOTSUPPORTED;
#else
	return app_data.camera_irfilter_mode;
#endif
}

uint8_t Cfg_AppDataGetCameraIRPulseWidth()
{
#if !(FOXLINK_CAMERA || DUAL_CAMERA)
    return PULSE_WIDTH_NOTSUPPORTED;
#else
    return app_data.camera_ir_pulse_width;
#endif
}

uint8_t Cfg_AppDataGetCameraWhitePulseWidth()
{
#if !DUAL_CAMERA
    return PULSE_WIDTH_NOTSUPPORTED;
#else
    return app_data.camera_white_pulse_width;
#endif
}

uint8_t Cfg_AppDataGetEmotionRecTypes()
{
    return app_data.emotion_types;
}

uint8_t Cfg_AppDataGetLivenessMode()
{
    return app_data.liveness_mode;
}

uint8_t Cfg_AppDataGetDisplayMode()
{
    return app_data.display_mode;
}

uint8_t Cfg_AppDataGetDetectResolutionMode()
{
    return app_data.detect_resolution_mode;
}

uint8_t Cfg_AppDataGetApplicationType()
{
    return app_data.app_type;
}

