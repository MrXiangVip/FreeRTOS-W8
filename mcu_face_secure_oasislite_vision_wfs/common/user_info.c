/**************************************************************************
 * 	FileName:	 	user_info.h
 *	Description:	This file is including the function interface which is
 					used to store cfg in flash
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-11-17
 *	Updated:		
 *					
**************************************************************************/
 
#include "user_info.h"

#define DataCheckFailed 1
#define DataCheckOK     0
/** A structure that contains the user application config data */
static user_info_t user_data;

static int32_t UserInfo_SanityCheck(user_info_t *cfg)
{
    int32_t status = DataCheckOK;
    /*if (!(cfg->usb_shell_verbose >= VERBOSE_MODE_OFF && cfg->usb_shell_verbose <= VERBOSE_MODE_L3))
    {
        cfg->usb_shell_verbose = VERBOSE_MODE_OFF;
        status = DataCheckFailed;
    }*/

    return status;
}

int32_t UserInfo_FlashRead(user_info_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;
    uint32_t len = sizeof(user_info_t);
    status = SLN_FLASH_MGMT_Read(USER_INFO_FILE_NAME, (uint8_t *)cfg, &len);

    return status;
}

int32_t UserInfo_FlashWrite(user_info_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;

    status = SLN_FLASH_MGMT_Save(USER_INFO_FILE_NAME, (uint8_t *)cfg, sizeof(user_info_t));

    return status;
}

void UserInfo_GetDefault(user_info_t *cfg)
{
    user_info_t default_app_data = {0, 0};
    memcpy(cfg,&default_app_data,sizeof(user_info_t));
}

void UserInfo_AppDataInit()
{
    int32_t status;

    /* At first boot SLN_FLASH_MGMT_ENOENTRY2 is returned because there are no data saved */
    status = CfgData_FlashRead(&user_data);
    if (SLN_FLASH_MGMT_OK != status)
    {
        CfgData_GetDefault(&user_data);
        if (SLN_FLASH_MGMT_ENOENTRY2 == status)
        {
            CfgData_FlashWrite(&user_data);
        }
    }
    status = CfgData_SanityCheck(&user_data);
    if (status == DataCheckFailed)
    {
        CfgData_FlashWrite(&user_data);        
    }
}

void UserInfo_AppDataRead(user_info_t *cfg)
{
    memcpy(cfg,&user_data,sizeof(user_info_t));
}

int32_t UserInfo_AppDataSave(user_info_t *cfg)
{
    int32_t status = SLN_FLASH_MGMT_OK;

    status = SLN_FLASH_MGMT_Save(USER_INFO_FILE_NAME, (uint8_t *)cfg, sizeof(user_info_t));
    if (SLN_FLASH_MGMT_OK == status)
    {
        memcpy(&user_data,cfg,sizeof(user_info_t));
    }

    return status;
}

