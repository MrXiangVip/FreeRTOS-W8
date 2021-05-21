/*
 * Copyright 2019-2020 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "database.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "generic_list.h"
#include "sln_vizn_api.h"
#include "sln_vizn_api_internal.h"
#include "sln_dev_cfg.h"
#include "sln_timers.h"
#include "commondef.h"
#include <assert.h>

#include "fsl_log.h"


#if FOXLINK_CAMERA || DUAL_CAMERA
#include "fsl_pcal.h"
#endif
#include "camera.h"
#include "oasis.h"

/*******************************************************************************
 * Definitions
 *******************************************************************************/

typedef struct _VIZN_api_context_handle
{
    list_t clientListHead;

} VIZN_api_context_handle_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static int VIZN_API_Enrolment(VIZN_api_client_t *pClient, face_info_t face_info);
static int VIZN_API_Recognize(VIZN_api_client_t *pClient, face_info_t face_info);
static int VIZN_API_Detect(VIZN_api_client_t *pClient, int dt);

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static QMsg sFaceRecEnrolmentMode;
static uint8_t sApiHandleBuffer[VIZN_API_HANDLE_SIZE];

static bool s_face_detect              = false;
VIZN_api_handle_t gApiHandle;
static SemaphoreHandle_t s_cfg_lock;
static client_operations_t api_ops = {
    .detect    = VIZN_API_Detect,
    .recognize = VIZN_API_Recognize,
    .enrolment = VIZN_API_Enrolment,
};
VIZN_API_CLIENT_DEFINE(VIZN_API, (char *)"VIZN_API", &api_ops);


/*******************************************************************************
 * Code
 ******************************************************************************/
static inline char IsValidCharacter(char c)
{
    if (isalnum(c) || (c == '-') || (c == '_'))
    {
        return 1;
    }
    return 0;
}

static int IsValidUserName(char *name)
{
    int i;
    for (i=0; name[i]!=0; i++)
    {
        if (!IsValidCharacter(name[i]))
        {
            return 0;
        }
    }
    return 1;
}

static vizn_api_status_t Cfg_Lock()
{
    if (s_cfg_lock == NULL)
    {
        return kStatus_API_Sem_NoLock;
    }

    if (pdTRUE != xSemaphoreTake(s_cfg_lock, portMAX_DELAY))
    {
        return kStatus_API_Sem_ERETRY;
    }
    return kStatus_API_Layer_Success;
}

static vizn_api_status_t Cfg_Unlock()
{
    xSemaphoreGive(s_cfg_lock);
    return kStatus_API_Layer_Success;
}


static void VIZN_TimerCallback(TimerHandle_t xTimer)
{
    QCmdMsg cmd;
    uint8_t id = *(uint8_t *)pvTimerGetTimerID(xTimer);
    switch (id)
    {
        case TIMER_REG_NO_FACE:
            if(!s_face_detect)
            {
                if(Oasis_GetState() == OASIS_STATE_FACE_ADD_STARTED)
                {
                    cmd.id = QCMD_ADD_FACE_CANCEL;
                    Oasis_SendCmd(cmd);
                    StopRegistrationTimers();
                }
            }
            break;
        case TIMER_REG_HAS_FACE:
            if(Oasis_GetState() == OASIS_STATE_FACE_ADD_STARTED)
            {
                cmd.id = QCMD_ADD_FACE_CANCEL;
                Oasis_SendCmd(cmd);
            }
            break;
        case TIMER_SYSTEM_LOCKED:
        case TIMER_DET_NO_FACE:
        case TIMER_REC_NO_FACE:
            {
                Oasis_TimerProcess(id);
            }
            break;
    }
}

static int VIZN_API_Detect(VIZN_api_client_t *pClient, int dt)
{
    assert(pClient == &VIZN_API_CLIENT(VIZN_API));
    if(dt < 0)
    {
        s_face_detect = false;
    } else {
        s_face_detect = true;
        ResetDetNoFaceTimers();
        ResetRegNoFaceTimers();
    }
    return 1;
}

static int VIZN_API_Recognize(VIZN_api_client_t *pClient, face_info_t face_info)
{
    assert(pClient == &VIZN_API_CLIENT(VIZN_API));
    if (face_info.recognize)
    {
        ResetRecNoFaceTimers();
    }
    else
    {
        // recognised failed
    }
    return 1;
}

static int VIZN_API_Enrolment(VIZN_api_client_t *pClient, face_info_t face_info)
{
    assert(pClient == &VIZN_API_CLIENT(VIZN_API));
    StopRegistrationTimers();
    return 1;
}

/*******************************************************************************
 * Code API
 ******************************************************************************/

vizn_api_status_t VIZN_API_Init(VIZN_api_handle_t *apiHandle)
{
    *apiHandle = &sApiHandleBuffer[0];
    sln_cfg_data_t cfg;
    VIZN_api_context_handle_t *apiContextHandle = (VIZN_api_context_handle_t *)(*apiHandle);
    //The API is always a client of the engine.
    apiContextHandle->clientListHead.max = MAX_VIZN_CLIENTS + 1;

    Cfg_AppDataRead(&cfg);
    sFaceRecEnrolmentMode.id = QMSG_FACEREC_ENROLMENTMODE;
    sFaceRecEnrolmentMode.msg.cmd.data.enrolment_mode = cfg.enrolment_mode;

    s_cfg_lock = xSemaphoreCreateMutex();
    if (s_cfg_lock == NULL )
    {
        return kStatus_API_Layer_Error;
    }

    if (SLN_Init_Timers(VIZN_TimerCallback) != 0)
        return kStatus_API_Layer_Error;
    VIZN_RegisterClient(gApiHandle, &VIZN_API_CLIENT(VIZN_API));
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_RegisterClient(VIZN_api_handle_t apiHandle, VIZN_api_client_t *clientHandle)
{
    VIZN_api_context_handle_t *apiContextHandle = (VIZN_api_context_handle_t *)apiHandle;
    list_status_t status;
    assert(clientHandle);
    assert(apiHandle);
    /* memory set for shellHandle */
    (void)memset(&clientHandle->link, 0, sizeof(clientHandle->link));

    status = LIST_AddTail(&apiContextHandle->clientListHead, &clientHandle->link);
    if (status != kLIST_Ok)
    {
        if (status == kLIST_Full)
            return kStatus_API_RegisterClient_LimitReached;
        else
            return kStatus_API_Layer_Error;
    }
    else
    {
        return kStatus_API_Layer_Success;
    }
}

vizn_api_status_t VIZN_UnregisterClient(VIZN_api_handle_t apiHandle, VIZN_api_client_t *clientHandle)
{
    list_status_t status;
    assert(clientHandle);
    status = LIST_RemoveElement(&clientHandle->link);

    /* memory set for shellHandle */
    (void)memset(&clientHandle->link, 0, sizeof(clientHandle->link));

    if (status != kLIST_Ok)
        return kStatus_API_Layer_Error;
    else
        return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_GetRegisteredUsers(VIZN_api_client_t *clientHandle, std::vector<std::string> *nameList)
{
    int status = DB_GetNames(nameList);
    if (status < 0)
        return kStatus_API_Layer_Error;
    else
        return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_SetRegisteredMode(VIZN_api_client_t *clientHandle, cfg_enrolment_t mode)
{
    uint32_t status;
    //Set mode
    QMsg* pQMsg;
    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);

    sFaceRecEnrolmentMode.msg.cmd.data.enrolment_mode = mode;
    pQMsg = &sFaceRecEnrolmentMode;
    if (Oasis_SendQMsg((void*)&pQMsg) != 0)
    {
        Cfg_Unlock();
        return kStatus_API_Layer_SetRegisteredMode_SaveFailed;
    }

    cfg.enrolment_mode = (uint8_t)mode;
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();

    if (SLN_FLASH_MGMT_OK != status)
    {
        return kStatus_API_Layer_SetRegisteredMode_SaveFailed;
    }
    else
    {
        return kStatus_API_Layer_Success;
    }

}
vizn_api_status_t VIZN_AddUser (VIZN_api_client_t *clientHandle)
{
    QCmdMsg cmd;
    if((Oasis_GetState() == OASIS_STATE_FACE_ADD_START)
        ||(Oasis_GetState() == OASIS_STATE_FACE_ADD_STARTED)
        ||(Oasis_GetState() == OASIS_STATE_FACE_ADD_STOP))
    {
        return kStatus_API_Layer_AddUser_MultipleAddCommand;
    }

    cmd.id = QCMD_ADD_FACE;
    cmd.data.name = NULL;
    Oasis_SendCmd(cmd);
    s_face_detect  = false;
    //StartRegistrationTimers();
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_AddUser (VIZN_api_client_t *clientHandle, char *userName)
{
    QCmdMsg cmd;
    if (!strcmp(userName, "-s"))
    {
        if((Oasis_GetState() != OASIS_STATE_FACE_ADD_START)
            &&(Oasis_GetState() != OASIS_STATE_FACE_ADD_STARTED)
            &&(Oasis_GetState() != OASIS_STATE_FACE_ADD_STOP))
        {
            return kStatus_API_Layer_AddUser_NoAddCommand;
        }
        cmd.id = QCMD_ADD_FACE_CANCEL;
        Oasis_SendCmd(cmd);
        //StopRegistrationTimers();
        return kStatus_API_Layer_AddUser_AddCommandStopped;
    }

    if (!IsValidUserName(userName))
    {
        return kStatus_API_Layer_AddUser_InvalidUserName;
    }

    if((Oasis_GetState() == OASIS_STATE_FACE_ADD_START)
        ||(Oasis_GetState() == OASIS_STATE_FACE_ADD_STARTED)
        ||(Oasis_GetState() == OASIS_STATE_FACE_ADD_STOP))
    {
        return kStatus_API_Layer_AddUser_MultipleAddCommand;
    }

    cmd.id = QCMD_ADD_FACE;
    cmd.data.name = userName;
    Oasis_SendCmd(cmd);
    s_face_detect  = false;
    //StartRegistrationTimers();

    return kStatus_API_Layer_Success;

}

vizn_api_status_t VIZN_DelUser (VIZN_api_client_t *clientHandle, char *userName)
{

    if (!IsValidUserName(userName))
    {
        return kStatus_API_Layer_DelUser_InvalidUserName;
    }

    if (DB_Del(std::string(userName)) !=0)
    {
        return kStatus_API_Layer_DelUser_FailedDelete;
    }
    else
    {
        return kStatus_API_Layer_Success;
    }
}

vizn_api_status_t VIZN_DelCurrentUser (VIZN_api_client_t *clientHandle)
{
    QCmdMsg cmd;
    if ((Oasis_GetState() == OASIS_STATE_FACE_DEL_START)
        ||(Oasis_GetState() == OASIS_STATE_FACE_DEL_STARTED))
    {
        cmd.id = QCMD_DEL_FACE_CANCEL;
        Oasis_SendCmd(cmd);
        return kStatus_API_Layer_DelUser_DelCommandStopped;
    }
    cmd.id = QCMD_DEL_FACE;
    Oasis_SendCmd(cmd);
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_DelUser (VIZN_api_client_t *clientHandle)
{

    if (0 != DB_DelAll())
        return kStatus_API_Layer_DelUser_FailedDeleteAll;
    else
        return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_RenameUser (VIZN_api_client_t *clientHandle, char *oldUserName , char *newUserName )
{
    int32_t status;

	if (!IsValidUserName(oldUserName) || !IsValidUserName(newUserName))
    {
        return kStatus_API_Layer_RenameUser_InvalidUserName;
    }

	status = DB_Ren(std::string(oldUserName), std::string(newUserName));
    if (status == 0)
    {
        return kStatus_API_Layer_Success;
    }
    else if (status == -2)
    {
        return kStatus_API_Layer_RenameUser_AlreadyExists;
    }
    else
    {
        return kStatus_API_Layer_RenameUser_FailedRename;
    }
}

vizn_api_status_t VIZN_SetVerboseMode(VIZN_api_client_t *clientHandle, cfg_verbose_t mode)
{

    uint32_t status;
    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);
    cfg.usb_shell_verbose = (uint8_t)mode;
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();
    if (SLN_FLASH_MGMT_OK != status)
    {
        return kStatus_API_Layer_SetVerboseMode_SaveFailed;
    }

    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_SaveDB (VIZN_api_client_t *clientHandle, int count)
{
    int status = DB_Save(count);
    if (status < 0)
        return kStatus_API_Layer_Error;
    else
        return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_SystemReset (VIZN_api_client_t *clientHandle)
{
    NVIC_SystemReset();
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_SetDetResolutionMode(VIZN_api_client_t *clientHandle, cfg_detresolution_t mode)
{
    uint32_t status;
	bool already = false;

    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);

    if(cfg.detect_resolution_mode == (uint8_t)mode)
		already = true;
	else {
        cfg.detect_resolution_mode = (uint8_t)mode;
        status = Cfg_AppDataSave(&cfg);
	}
    Cfg_Unlock();

    if(already)
	{
		return kStatus_API_Layer_SetDetResolutionMode_Same;
    }
    if (SLN_FLASH_MGMT_OK != status)
    {
        return kStatus_API_Layer_SetDetResolutionMode_SaveFailed;
    }
    else
    {
        return kStatus_API_Layer_Success;
    }
}

vizn_api_status_t VIZN_EnrolmentEvent(VIZN_api_handle_t apiHandle, face_info_t faceInfo)
{

    VIZN_api_context_handle_t *apiContextHandle = (VIZN_api_context_handle_t *)apiHandle;
    assert(apiHandle);
    list_element_handle_t p = apiContextHandle->clientListHead.head;
    while (p != NULL)
    {
        VIZN_api_client_t *client = VIZN_CLIENT_POINTER(p);
        if(client->ops->enrolment)
            client->ops->enrolment(client, faceInfo);
        p = LIST_GetNext(p);
    }
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_DetectEvent(VIZN_api_handle_t apiHandle, int dt)
{

    VIZN_api_context_handle_t *apiContextHandle = (VIZN_api_context_handle_t *)apiHandle;
    assert(apiHandle);
    list_element_handle_t p = apiContextHandle->clientListHead.head;

    while (p != NULL){
        VIZN_api_client_t *client = VIZN_CLIENT_POINTER(p);
        if(client->ops->detect)
            client->ops->detect(client, dt);
        p = LIST_GetNext(p);
    }
    return kStatus_API_Layer_Success;
};

vizn_api_status_t VIZN_RecognizeEvent(VIZN_api_handle_t apiHandle,face_info_t faceInfo)
{

    VIZN_api_context_handle_t *apiContextHandle = (VIZN_api_context_handle_t *)apiHandle;
    assert(apiHandle);
    list_element_handle_t p = apiContextHandle->clientListHead.head;

    while (p != NULL)
    {
        VIZN_api_client_t *client = VIZN_CLIENT_POINTER(p);
        if(client->ops->recognize)
            client->ops->recognize(client, faceInfo);
        p = LIST_GetNext(p);
    }
    return kStatus_API_Layer_Success;
};

vizn_api_status_t VIZN_GetIRFilter (VIZN_api_client_t *clientHandle, cfg_irfilter_t *mode)
{
#if !FOXLINK_CAMERA
    return kStatus_API_Layer_SetCameraIRFilterMode_NotSupported;
#else
    uint8_t IRFilterMode;
    IRFilterMode = Cfg_AppDataGetCameraIRFilterMode();
    *mode =(cfg_irfilter_t) IRFilterMode;
    return kStatus_API_Layer_Success ;
#endif
}

vizn_api_status_t VIZN_SetIRFilter (VIZN_api_client_t *clientHandle, cfg_irfilter_t mode)
{
#if !FOXLINK_CAMERA
    return kStatus_API_Layer_SetCameraIRFilterMode_NotSupported;
#else
    uint32_t status;
    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);
    cfg.camera_irfilter_mode = (int)mode;
    SetIRFilterMotor(mode);
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();
    if (SLN_FLASH_MGMT_OK != status)
    {
        return kStatus_API_Layer_SetCameraIRFilterMode_SaveFailed;
    }
    return kStatus_API_Layer_Success;
#endif
}

vizn_api_status_t VIZN_SetPulseWidth (VIZN_api_client_t *clientHandle, cfg_led_t led, uint8_t pulse_width)
{
#if !(FOXLINK_CAMERA || DUAL_CAMERA)
    return kStatus_API_Layer_SetCameraPulseWidth_NotSupported;
#else
    uint32_t status;
    sln_cfg_data_t cfg;
#if !DUAL_CAMERA
    if(led == LED_WHITE)
        return kStatus_API_Layer_SetCameraPulseWidth_NotSupported;
#endif
    if (pulse_width > 100)
        return kStatus_API_Layer_SetCameraPulseWidth_WrongValue;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);
    if( led == LED_WHITE)
        cfg.camera_white_pulse_width = pulse_width;
    else
        cfg.camera_ir_pulse_width = pulse_width;
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();
    Camera_QMsgSetPWM(led,pulse_width);
    if (status != SLN_FLASH_MGMT_OK)
    {
        return kStatus_API_Layer_SetCameraPulseWidth_SaveFailed;
    }
    return  kStatus_API_Layer_Success;
#endif
}

vizn_api_status_t VIZN_GetPulseWidth (VIZN_api_client_t *clientHandle, cfg_led_t led, uint8_t *pulse_width)
{
#if !(FOXLINK_CAMERA || DUAL_CAMERA)
    return kStatus_API_Layer_SetCameraPulseWidth_NotSupported;
#else
#if !DUAL_CAMERA
    if(led == LED_WHITE)
        return kStatus_API_Layer_SetCameraPulseWidth_NotSupported;
#endif
    if(led == LED_WHITE)
        *pulse_width = Cfg_AppDataGetCameraWhitePulseWidth();
    else
        *pulse_width = Cfg_AppDataGetCameraIRPulseWidth();
#endif
    return kStatus_API_Layer_Success ;
}

vizn_api_status_t VIZN_SetEmotionTypes (VIZN_api_client_t *clientHandle, uint8_t types)
{
    uint32_t status;

    // 0 : disable the emotion recognition
    // 2 : 2 types emotion recognition
    // 4 : 4 types emotion recognition
    // 7 : 7 types emotion recognition
    if ((types == 2) || (types == 4) ||
        (types == 7) || (types == 0))
    {
        sln_cfg_data_t cfg;
        Cfg_Lock();
        Cfg_AppDataRead(&cfg);

        cfg.emotion_types = (uint8_t)types;
        status = Cfg_AppDataSave(&cfg);
        Cfg_Unlock();

        if (SLN_FLASH_MGMT_OK != status)
        {
            return kStatus_API_Layer_SetEmotionTypes_SaveFailed;
        }
        else
        {
            if (types == 0)
            {
                return kStatus_API_Layer_SetEmotionTypes_Disabled;
            }
            else
            {
                return kStatus_API_Layer_Success;
            }
        }
    }
    else
    {
        return kStatus_API_Layer_SetEmotionTypes_InvalidTypes;
    }
}

vizn_api_status_t VIZN_GetEmotionTypes (VIZN_api_client_t *clientHandle, uint8_t *types)
{
    *types = Cfg_AppDataGetEmotionRecTypes();
    return kStatus_API_Layer_Success ;
}

vizn_api_status_t VIZN_SetLivenessMode (VIZN_api_client_t *clientHandle, cfg_liveness_t mode)
{
    uint32_t status;
    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);
    cfg.liveness_mode = (uint8_t)mode;
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();
    Camera_SetMonoMode(cfg.liveness_mode);
    if (SLN_FLASH_MGMT_OK != status)
    {
        return kStatus_API_Layer_SetLivenessMode_SaveFailed;
    }

    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_GetLivenessMode (VIZN_api_client_t *clientHandle, cfg_liveness_t *mode)
{
    *mode = (cfg_liveness_t)Cfg_AppDataGetLivenessMode();
    return kStatus_API_Layer_Success;
}

vizn_api_status_t VIZN_SetDispMode (VIZN_api_client_t *clientHandle, cfg_displaymode_t mode)
{
#if !DUAL_CAMERA
    return kStatus_API_Layer_SetDispMode_NotSupported;
#else
    Camera_SetDispMode((uint8_t)mode);
    return kStatus_API_Layer_Success;
#endif
}

vizn_api_status_t VIZN_SetAppType (VIZN_api_client_t *clientHandle, cfg_apptype_t type)
{
#if !DUAL_CAMERA
    return kStatus_API_Layer_SetAppType_NotSupported;
#else
    uint32_t status;
    sln_cfg_data_t cfg;
    Cfg_Lock();
    Cfg_AppDataRead(&cfg);
    cfg.app_type = type;
    status = Cfg_AppDataSave(&cfg);
    Cfg_Unlock();
    return kStatus_API_Layer_Success;
#endif
}

vizn_api_status_t VIZN_GetAppType (VIZN_api_client_t *clientHandle, cfg_apptype_t *type)
{
    *type = (cfg_apptype_t)Cfg_AppDataGetApplicationType();
    return kStatus_API_Layer_Success;
}
