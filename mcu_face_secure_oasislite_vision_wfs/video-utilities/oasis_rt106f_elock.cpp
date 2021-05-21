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

#if RT106F_ELOCK_BOARD

#include "oasislite_runtime.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "camera.h"
#include "oasis.h"
#include "stdio.h"
#include <vector>
#include <string>
#include "database.h"
#include "oasis.h"
#include "sln_vizn_api_internal.h"
#include "fsl_camera_device.h"
#include "sln_timers.h"
#include "capture.h"
#include "MCU_UART5.h"
#include "fsl_log.h"

extern VIZN_api_client_t VIZN_API_CLIENT(Buttons);

#define OASIS_DETECT_MIN_FACE 100

#define MS_TIMER_LOCK  1000
/* User can modify the pwm range according to the real hardware.
 * To obtain good face brightness in different light conditions.
 * Also balance power consumption and recognition distance.
 */
#define IR_PWM_MIN          20
#define IR_PWM_MAX          100
#define IR_PWM_INTERVAL     10

#define WHITE_PWM_MIN       0
#define WHITE_PWM_MAX       30
#define WHITE_PWM_INTERVAL  10

struct TimeStat{
    int det_start;
    int det_comp;
    int rec_start;
    int rec_comp;
    int emo_start;
    int emo_comp;

    int det_fps_start;
    int det_fps;

    int rec_fps_start;
    int rec_fps;

} gTimeStat;

const char* gEmoString[OASIS_EMO_ID_INVALID] = {
    "anger",//OASIS_EMO_ID_ANGER
    "disgust",
    "fear",
    "happy",
    "sad",
    "surprised",
    "neutral"
};

//__attribute__((section(".bss.$SRAM_OC")))
static FaceRecBuffer gFaceRecBuf;
static void clearFaceInfoMsg(QUIInfoMsg* info);
static void EvtHandler(ImageFrame_t** frames, OASISLTEvt_t evt, OASISLTCbPara_t* para, void* user_data);
static int GetRegisteredFacesHandler(int idx, uint16_t* face_ids, void* faces, unsigned int* size);
static int AddNewFaceHandler(uint16_t* face_id, void* face);
static int UpdateFaceHandler(uint16_t face_id, void* face);
static void AdjustBrightnessHandler(uint8_t frame_idx, uint8_t direction);
static int Oasis_Printf(const char* formatString);
static void Oasis_SetState(OasisState state);
static int Oasis_Exit();

volatile int g_AddNewFace         = 0;
volatile int g_RemoveExistingFace = 0;
std::string g_AddNewFaceName;
int g_IsAuthenticated = 0;
static OasisState sCurrentState = OASIS_STATE_FACE_REC_START;
static QMsg gFaceInfoMsg;
static QueueHandle_t gFaceDetMsgQ = NULL;
static QMsg gFaceDetReqMsg;
static QMsg sCmdMsg;

/*support VGA resolution and heavy model type*/
#define MEM_POOL4OASIS_LIB_SIZE (750*1024)
__attribute__((section(".bss.$SRAM_OC_CACHEABLE")))
static int gMemPool4OasisLib[MEM_POOL4OASIS_LIB_SIZE / sizeof(int)];

static int lockstatus = 1;
static OASISLTInitPara_t gInit_para;

#include "sln_shell.h"

void Oasis_TimerProcess(uint8_t timerId)
{
    switch(timerId)
    {
        case TIMER_SYSTEM_LOCKED:
        {
            QCmdMsg cmd;
            if(Oasis_GetState() == OASIS_STATE_FACE_ADD_STARTED)
            {
                cmd.id = QCMD_ADD_FACE_CANCEL;
                Oasis_SendCmd(cmd);
            }else{
                cmd.id = QCMD_REC_FACE;
                Oasis_SendCmd(cmd);
            }

            QMsg* pQMsg = &gFaceInfoMsg;
            clearFaceInfoMsg(&pQMsg->msg.info);
            Camera_SendQMsg((void*)&pQMsg);

            QMsg* sentQMsg = &gFaceDetReqMsg;
            Camera_SendQMsg((void*)&sentQMsg);
            lockstatus = 1;
        }
        break;

        case TIMER_DET_NO_FACE:
        case TIMER_REC_NO_FACE:
        {
            Camera_QMsgSetPWM(LED_WHITE, 0);
            StartDetNoFaceTimers();
            StartRecNoFaceTimers();
            Camera_SetRGBExposureMode(CAMERA_EXPOSURE_MODE_AUTO_LEVEL0);
        }
        break;

        default:
        break;
    }
}

static void EvtHandler(ImageFrame_t* frames[OASISLT_INT_FRAME_IDX_LAST], OASISLTEvt_t evt, OASISLTCbPara_t* para, void* user_data)
{
    if (frames == NULL || para == NULL) {
		LOGD("[OASIS]:return null\r\n");
        return;
    }

    QMsg* pQMsg = &gFaceInfoMsg;
    pQMsg->msg.info.regstatus = 0;
    face_info_t face_info;
    memset(&face_info, 0, sizeof(face_info_t));
    //UsbShell_Printf("[OASIS]:evt:%d\r\n",evt);
    //LOGD("[OASIS]:evt:%d\r\n",evt);
    struct TimeStat* timeState = (struct TimeStat*)user_data;

    switch (evt) {
    case OASISLT_EVT_DET_START: {
        timeState->det_start = Time_Now();
    }
    break;

    case OASISLT_EVT_DET_COMPLETE: {
        timeState->det_comp = Time_Now();
        pQMsg->msg.info.dt =  timeState->det_start - timeState->det_comp;

        if (para->faceBoxIR == NULL) {
            memset(pQMsg->msg.info.rect, -1, sizeof(para->faceBoxIR->rect));
            memset(pQMsg->msg.info.name, 0x0, sizeof(pQMsg->msg.info.name));
        } else {
            LOGD("[EVT]:IR detected:%d\r\n",para->faceBoxIR->rect[0]);
            memcpy(pQMsg->msg.info.rect, para->faceBoxIR->rect, sizeof(para->faceBoxIR->rect));
        }

#if DUAL_CAMERA

        if (para->faceBoxRGB == NULL) {
            memset(pQMsg->msg.info.rect2, -1, sizeof(para->faceBoxRGB->rect));
        } else {
            LOGD("[EVT]:RGB detected:%d\r\n",para->faceBoxRGB->rect[0]);
            memcpy(pQMsg->msg.info.rect2, para->faceBoxRGB->rect, sizeof(para->faceBoxRGB->rect));
        }

#endif


#ifdef SHOW_FPS
        /*pit timer unit is us*/
        timeState->det_fps++;
        int diff = abs(timeState->det_fps_start - timeState->det_comp);

        if (diff > 1000000 / PIT_TIMER_UNIT && diff < 5000000 / PIT_TIMER_UNIT) {
            //update fps
            pQMsg->msg.info.detect_fps = timeState->det_fps * 1000.0f / diff;
            timeState->det_fps = 0;
            timeState->det_fps_start = timeState->det_comp;

        }

#endif
    }
#if DUAL_CAMERA
    VIZN_DetectEvent(gApiHandle, ((para->faceBoxIR == NULL)|| (para->faceBoxRGB == NULL)) ? -1 : pQMsg->msg.info.dt );
#else
    VIZN_DetectEvent(gApiHandle, (para->faceBoxIR == NULL ) ? -1 : pQMsg->msg.info.dt );
#endif
    break;

    case OASISLT_EVT_QUALITY_CHK_START:
        break;

    case OASISLT_EVT_QUALITY_CHK_COMPLETE: {

        //capture_frame(frames[0]->data);
        //capture_frame(frames[1]->data);

        LOGD("[OASIS]:quality chk res:%d\r\n", para->qualityResult);

        pQMsg->msg.info.irLive = para->reserved[5];
        pQMsg->msg.info.front = para->reserved[1];
        pQMsg->msg.info.blur = para->reserved[3];
        pQMsg->msg.info.rgbLive = para->reserved[8];
        pQMsg->msg.info.irBrightness = para->reserved[11];
        pQMsg->msg.info.rgbBrightness = para->reserved[13];
        Camera_GetPWM(LED_IR,&pQMsg->msg.info.irPwm);
        Camera_GetPWM(LED_WHITE,&pQMsg->msg.info.rgbPwm);

        LOGD("[irBrightness]:%d\r\n",pQMsg->msg.info.irBrightness);
        LOGD("[rgbBrightness]:%d\r\n",pQMsg->msg.info.rgbBrightness);

        if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_OK) {
            LOGD("[EVT]:ok!\r\n");
        } else if (OASIS_QUALITY_RESULT_FACE_SIDE_FACE == para->qualityResult) {
            LOGD("[EVT]:side face!\r\n");
        } else if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_TOO_SMALL) {
            LOGD("[EVT]:Small Face!\r\n");
        } else if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_BLUR) {
            LOGD("[EVT]: Blurry Face!\r\n");
        } else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_LIVENESS_IR) {
            LOGD("[EVT]: IR Fake Face!\r\n");
        } else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_LIVENESS_RGB) {
            LOGD("[EVT]: RGB Fake Face!\r\n");
        }else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_DARK
                  || para->qualityResult == OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_OVEREXPOSURE) {
            LOGD("[EVT]: Face Brightness unfit!\r\n");
        }
    }
    break;

    case OASISLT_EVT_MASK_CHK_START:
    break;

    case OASISLT_EVT_MASK_CHK_COMPLETE: {
        if (para->maskResult == OASIS_MASK_CHECK_RESULT_FACE_WITH_MASK) {
            LOGD("[EVT]:Face with mask!\r\n");
        } else {
            LOGD("[EVT]:Face without mask!\r\n");
        }
    }
    break;

    case OASISLT_EVT_GLASSES_CHK_START:
    break;

    case OASISLT_EVT_GLASSES_CHK_COMPLETE: {
        if (para->glassesResult == OASIS_GLASSES_CHECK_RESULT_FACE_WITH_GLASSES) {
            LOGD("[EVT]:Face with glasses!\r\n");
        } else {
            LOGD("[EVT]:Face without glasses!\r\n");
        }
    }
    break;

    case OASISLT_EVT_REC_START: {
        timeState->rec_start = Time_Now();
    }
    break;

    case OASISLT_EVT_REC_COMPLETE: {
        timeState->rec_comp = Time_Now();
        pQMsg->msg.info.rt =  timeState->rec_start - timeState->rec_comp;
        face_info.rt = pQMsg->msg.info.rt;
#ifdef SHOW_FPS
        /*pit timer unit is us*/
        timeState->rec_fps++;
        int diff = abs(timeState->rec_fps_start - timeState->rec_comp);

        if (diff > 1000000 / PIT_TIMER_UNIT) {
            //update fps
            pQMsg->msg.info.recognize_fps = timeState->rec_fps * 1000.0f / diff;
            timeState->rec_fps = 0;
            timeState->rec_fps_start = timeState->rec_comp;

        }

#endif
        unsigned id = para->faceID;
        OASISLTRecognizeRes_t recResult = para->recResult;

        memset(pQMsg->msg.info.name, 0x0, sizeof(pQMsg->msg.info.name));

        pQMsg->msg.info.newface = 0;

        if (recResult == OASIS_REC_RESULT_KNOWN_FACE) {
            if (Oasis_GetState() == OASIS_STATE_FACE_REC_STARTED) {
                lockstatus = 0;
                StopDetNoFaceTimers();
                StopRecNoFaceTimers();
                Oasis_SetState(OASIS_STATE_FACE_REC_STOPPED);
            }

            LOGD("[OASIS]:face id:%d\r\n", id);
            std::string name;
            DB_GetName(id, name);
            memcpy(pQMsg->msg.info.name, name.c_str(), name.size());
            memcpy(face_info.name, name.c_str(), name.size());
            face_info.recognize = true;

            LOGD("[OASIS]:face id:%d name:%s\r\n", id, pQMsg->msg.info.name);

            //user request to remove current face
            if (Oasis_GetState() == OASIS_STATE_FACE_DEL_STARTED) {
                StopDeregistrationTimers();
                Oasis_SetState(OASIS_STATE_FACE_DEL_STOPPED);
                VIZN_DelUser(&VIZN_API_CLIENT(Buttons), (char*)pQMsg->msg.info.name);
                lockstatus = 0;
            }

        } 
		else {
            //face is not recognized, do nothing
            LOGD("[OASIS]:face unrecognized: %d\r\n",  id);
            face_info.recognize = false;
            LOGD("[EVT]: Unkown Face!\r\n");
            //if(ENROLMENT_MODE_AUTO == Cfg_AppDataGetEnrolmentMode())
                //VIZN_AddUser(NULL);
        }

        VIZN_RecognizeEvent(gApiHandle, face_info);

		//send qMsg to uart5 task when face recognize over 			
		LOGD("[OASIS]:face recognize over!\r\n");
		if(face_info.recognize)
		{
			LOGD("username<len:%d>:%s.\n", strlen(face_info.name), face_info.name); 
		}
		Set_curFaceInfo(face_info.name, face_info.recognize, face_info.enrolment, recResult);
		Uart5_GetFaceRecResult((uint8_t)face_info.recognize);
    }
    break;

    case OASISLT_EVT_EMO_REC_START: {

        timeState->emo_start = Time_Now();
    }
    break;

    case OASISLT_EVT_EMO_REC_COMPLETE: {
        timeState->emo_comp = Time_Now();
        LOGD("[OASIS]:Emo rec complete:%d.\r\n", para->emoID);
        //if (para->emo_conf > 50)
        {
            pQMsg->msg.info.emotion = para->emoID;
            //here we use name as emoID and similar as emotion recognition confidence level just for display purpose
            //memcpy(pQMsg->msg.info.name,gEmoString[para->emoID],strlen(gEmoString[para->emoID]) + 1);

        }
    }
    break;

    case OASISLT_EVT_REG_START: {
        pQMsg->msg.info.regstatus = 1;
    }
    break;

    case OASISLT_EVT_REG_IN_PROGRESS: {
        LOGD("[OASIS]:reg in process : %d\r\n");
    }
    break;

    case OASISLT_EVT_REG_COMPLETE: {
        uint16_t  id = para->faceID;
        OASISLTRegisterRes_t regResult = para->regResult;
        LOGD("[OASIS]:reg complete:%d:%d\r\n", id, regResult);
        memset(pQMsg->msg.info.name, 0x0, sizeof(pQMsg->msg.info.name));
        pQMsg->msg.info.newface = 0;
        pQMsg->msg.info.regstatus = 2;
        face_info.enrolment = false;
        Oasis_SetState(OASIS_STATE_FACE_ADD_STOPPED);
        StopRegistrationTimers();
        if (regResult == OASIS_REG_RESULT_OK) {
            lockstatus = 0;
            std::string name;
            DB_GetName(id, name);
            memcpy(pQMsg->msg.info.name, name.c_str(), name.size());
            memcpy(face_info.name, name.c_str(), name.size());
            face_info.enrolment = true;
            LOGD("[OASIS]:new face id:%d,name:%s\r\n", id, pQMsg->msg.info.name);

            int count;
            DB_Count(&count);
            //gFaceInfoMsg.msg.info.registeredFaces = featurenames.size();
            pQMsg->msg.info.registeredFaces = count;
            pQMsg->msg.info.newface = 1;
            //pQMsg->msg.info.updateFlag |= DISPLAY_INFO_UPDATE_NAME_SIM_RT|DISPLAY_INFO_UPDATE_NEW_REG_FACE;
        } else if (regResult == OASIS_REG_RESULT_DUP) {
            lockstatus = 0;
            std::string name;
            DB_GetName(id, name);
            memcpy(pQMsg->msg.info.name, name.c_str(), name.size());
            memcpy(face_info.name, name.c_str(), name.size());
            face_info.enrolment = true;

            LOGD("[OASIS]:dup face id:%d,name:%s\r\n", id, pQMsg->msg.info.name);
        } else {
            lockstatus = 0;
            LOGD("[OASIS]:registration failed(%d)!\r\n", regResult);
        }

        VIZN_EnrolmentEvent(gApiHandle, face_info);

		//send qMsg to uart5 task when face register over		
		LOGD("[OASIS]:face regiseter over!\r\n");
		if(face_info.enrolment)
		{
			LOGD("username<len:%d>:%s.\n", strlen(face_info.name), face_info.name); 			
		}
		Set_curFaceInfo(face_info.name, face_info.recognize, face_info.enrolment, regResult);
		Uart5_GetFaceRegResult((uint8_t)face_info.enrolment);		
	    }
    break;

    default:
        assert(0);
    }

    if (evt == OASISLT_EVT_DET_COMPLETE ||
        evt == OASISLT_EVT_REC_COMPLETE ||
        evt == OASISLT_EVT_EMO_REC_COMPLETE ||
        evt == OASISLT_EVT_QUALITY_CHK_COMPLETE ||
        evt == OASISLT_EVT_REG_COMPLETE) {
        pQMsg->msg.info.similar = para->reserved[0];
        Camera_SendQMsg((void*)&pQMsg);

        if (evt == OASISLT_EVT_REC_COMPLETE ||
            evt == OASISLT_EVT_REG_COMPLETE) {
            LOGD("[EVT]:sim:[%d]\r\n", para->reserved[0]);
            LOGD("[EVT]:FF[%d][%d]\r\n", para->reserved[1], para->reserved[2]);
            LOGD("[EVT]:Blur[%d][%d]\r\n", para->reserved[3], para->reserved[4]);
            LOGD("[EVT]:Liveness[%d][%d]\r\n", para->reserved[5], para->reserved[6]);
        }
    }
}


static int GetRegisteredFacesHandler(int idx, uint16_t* face_ids, void* faces, unsigned int* size)
{
    int face_item_size = OASISLT_getFaceItemSize();

    //UsbShell_Printf("[face_item_size]:%d\r\n",face_item_size);

    std::vector<uint16_t> allIDs;
    DB_GetIDs(allIDs);

    if (*size == 0) {
        *size = allIDs.size();
        return 0;
    }

    if (idx < 0 || (unsigned int)idx >= allIDs.size() ) {
        *size = 0;
        return 0;
    } else {
        if (allIDs.size() <= *size + idx) {
            *size = allIDs.size() - idx;

        }


        for (uint32_t i = 0; i < *size; i++) {
            // get idx+i ids form DB
            face_ids[i] = allIDs[idx + i];
            //get idx+i item from DB
            DB_GetFeature(allIDs[idx + i], (float*)((intptr_t)faces + i * face_item_size));
        }

        return 0;
    }

}


static int AddNewFaceHandler(uint16_t* face_id, void* face)
{
    int ret;
    float* feature_data = (float*)face;

    ret = DB_GenID(face_id);

    if (ret < 0) {
        return -1;
    }

    //g_AddNewFace can be triggered by two ways:
    //1. by CLI command, which provide a given user name
    //2. by a button click in manual/auto mode, no user name is provided
    if (g_AddNewFace && g_AddNewFaceName != "") {
        ret = DB_Add(*face_id, g_AddNewFaceName, feature_data);
    } else {
        ret = DB_Add(*face_id, feature_data);
    }

    if (ret >= 0) {
        return 0;
    } else {
        if (ret == DB_MGMT_NOSPACE) {
            UsbShell_Printf("Maximum number of users reached\r\n");
        }

        return -1;
    }


}

static int UpdateFaceHandler(uint16_t face_id, void* face)
{

    int ret;
    float* feature_data = (float*)face;

    ret = DB_Update(face_id, feature_data);

    if (ret >= 0) {
        return 0;
    } else {
        return -1;
    }
}

/* PWM control LED's light intensity to adjust face brightness.
 * led: which led needs to be adjusted?  LED_IR or LED_WHITE.
 * curPWM:  current pwm value.
 * direction: 1: up, need to increase brightness;  0: down, need to reduce brightness.
 */
static void Oasis_PWMControl(uint8_t led, uint8_t curPWM, uint8_t direction)
{
    uint8_t pwm;
    uint8_t pwm_min      = ((led == LED_IR) ? IR_PWM_MIN : WHITE_PWM_MIN);
    uint8_t pwm_max      = ((led == LED_IR) ? IR_PWM_MAX : WHITE_PWM_MAX);
    uint8_t pwm_interval = ((led == LED_IR) ? IR_PWM_INTERVAL : WHITE_PWM_INTERVAL);
    UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[LED:%d][curPWM:%d][dir:%d]\r\n", led, curPWM, direction);

    if (direction)
    {
        pwm = curPWM + pwm_interval;
        if (pwm >= pwm_max)
            pwm = pwm_max;
    }
    else
    {
        if (curPWM <= (pwm_interval + pwm_min))
            pwm = pwm_min;
        else
            pwm = curPWM - pwm_interval;

    }
    Camera_QMsgSetPWM(led, pwm);
}

//For GC0308, we can combine maunal exposure and pwm to adjust rgb face brightness.
//For MT9M114, only use pwm to adjust rgb face brightness.
//static void Oasis_RGBControl(uint8_t direction)
static void Oasis_LedControl(cfg_led_t ledID,uint8_t direction, uint8_t enableRGBModeCtrl)
{

    uint8_t pwm;
    Camera_GetPWM(ledID,&pwm);
    UsbShell_DbgPrintf(VERBOSE_MODE_L2,"Oasis_LedControl,led:%d dir:%d pwm:%d\r\n",ledID, direction, pwm);
    Oasis_PWMControl(ledID, pwm, direction);

    if (LED_WHITE == ledID && enableRGBModeCtrl)
    {
        uint8_t mode = Camera_GetRGBExposureMode();
        UsbShell_Printf("[OASIS]:Oasis_LedControl mode %d",mode);
		if (direction)
		{
			//Camera_QMsgSetPWM(LED_WHITE, pwm);
			mode = (mode < CAMERA_EXPOSURE_MODE_AUTO_LEVEL3)? (mode + 1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL3;
		}else
		{
			//Camera_QMsgSetPWM(LED_WHITE,0);
			mode = (mode > CAMERA_EXPOSURE_MODE_AUTO_LEVEL0)? (mode-1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL0;
		}
        UsbShell_Printf("----> %d\r\n",mode);
		Camera_SetRGBExposureMode(mode);
    }
}

/* Used to dynamically adjust face brightness, user can adjust brightness by modifing LED's light intensity or using manual exposure.
 *  frame_idx: which frame needs to be adjusted on, OASISLT_INT_FRAME_IDX_RGB or OASISLT_INT_FRAME_IDX_IR ?
 *  direction: 1: up, need to increase brightness;  0: down, need to reduce brightness.
 */
static void AdjustBrightnessHandler(uint8_t frame_idx, uint8_t direction)
{

    if (frame_idx == OASISLT_INT_FRAME_IDX_IR)
    {
        Oasis_LedControl(LED_IR,direction,0);
    }
    else
    {
    	Oasis_LedControl(LED_WHITE,direction,1);
    }
}

static int Oasis_Printf(const char* formatString)
{
    UsbShell_DbgPrintf(VERBOSE_MODE_L2, formatString);
    return 0;
}


static void Oasis_SetState(OasisState state)
{
    sCurrentState = state;
}

OasisState Oasis_GetState(void)
{
   return sCurrentState;
}

static void Oasis_ReceiveCmd(QCmdMsg cmd)
{
    QMsg* sentQMsg = &gFaceDetReqMsg;
    //exit lock timer when cmd is received.
    if (lockstatus == 0) {
        lockstatus = 1;
        StopLockTimers();
        Camera_SendQMsg((void*)&sentQMsg);
    }

    OasisState state = Oasis_GetState();
    //UsbShell_Printf("[Rev]:in_state:%d\r\n",state);
    switch (cmd.id) {
    case QCMD_REC_FACE:{
        g_AddNewFace = 0;
        g_RemoveExistingFace = 0;
        StopAllTimers();
        StartDetNoFaceTimers();
        StartRecNoFaceTimers();
        Oasis_SetState(OASIS_STATE_FACE_REC_START);
    }
    break;

    case QCMD_ADD_FACE: {
        g_AddNewFace = 1;
        g_RemoveExistingFace = 0;

        if (cmd.data.name == NULL) {
            g_AddNewFaceName = "";
        } else {
            g_AddNewFaceName = cmd.data.name;
        }
        StopAllTimers();
        StartRegistrationTimers();
        Oasis_SetState(OASIS_STATE_FACE_ADD_START);
    }
    break;

    case QCMD_ADD_FACE_CANCEL: {
        if((state == OASIS_STATE_FACE_ADD_START)
            ||(state == OASIS_STATE_FACE_ADD_STARTED))
        {
            g_AddNewFace = 0;
            g_AddNewFaceName = "";
            g_RemoveExistingFace = 0;
            StopRegistrationTimers();
            Oasis_SetState(OASIS_STATE_FACE_ADD_STOP);
        }
    }
    break;

    case QCMD_DEL_FACE: {
        g_AddNewFace = 0;
        g_RemoveExistingFace = 1;
        StopAllTimers();
        if(state == OASIS_STATE_FACE_ADD_STARTED){
            Oasis_SetState(OASIS_STATE_FACE_ADD_STOP);
            Oasis_SendCmd(cmd);
        }else if(state == OASIS_STATE_FACE_ADD_STOP){
            Oasis_SendCmd(cmd);
        }else{
            StartDeregistrationTimers();
            Oasis_SetState(OASIS_STATE_FACE_DEL_START);
        }
    }
    break;

    case QCMD_DEL_FACE_CANCEL: {
        if((state == OASIS_STATE_FACE_DEL_START)
            ||(state == OASIS_STATE_FACE_DEL_STARTED))
        {
            g_AddNewFace = 0;
            g_RemoveExistingFace = 0;
            StopDeregistrationTimers();
            Oasis_SetState(OASIS_STATE_FACE_REC_START);
        }
    }
    break;

    default:
        break;
    }

    QMsg* pQMsg = &gFaceInfoMsg;
    clearFaceInfoMsg(&pQMsg->msg.info);
    Camera_SendQMsg((void*)&pQMsg);

    Camera_SendQMsg((void*)&sentQMsg);
    //UsbShell_Printf("[Rev]:out_state:%d\r\n",Oasis_GetState());
}

volatile int g_OASISLT_heap_debug;
static void Oasis_Task(void* param)
{
    BaseType_t ret;
    QMsg* rxQMsg = NULL;
    QMsg* sentQMsg = NULL;
    QMsg* infoQMsg = NULL;
    OASISLTInitPara_t* init_p = (OASISLTInitPara_t*)param;
    ImageFrame_t frame;
    frame.height = init_p->height;
    frame.width = init_p->width;
    frame.data = NULL;
#if DUAL_CAMERA
    ImageFrame_t frame2;
    frame2.height = init_p->height;
    frame2.width = init_p->width;
    frame2.data = NULL;
#endif
    UsbShell_Printf("[OASIS DETECT]:running\r\n");
    LOGD("[OASIS DETECT]:running\r\n");

    //ask for the first frame
    gFaceInfoMsg.id = QMSG_FACEREC_INFO_UPDATE;
    infoQMsg = &gFaceInfoMsg;
    clearFaceInfoMsg(&infoQMsg->msg.info);
    Camera_SendQMsg((void*)&infoQMsg);
    //sentQMsg = (QMsg*)pvPortMalloc(sizeof(*sentQMsg));
    gFaceDetReqMsg.id           = QMSG_FACEREC_FRAME_REQ;
    gFaceDetReqMsg.msg.raw.data = gFaceRecBuf.data;
#if DUAL_CAMERA
    gFaceDetReqMsg.msg.raw.data2 = gFaceRecBuf.data2;
#endif
    sentQMsg = &gFaceDetReqMsg;
    Camera_SendQMsg((void*)&sentQMsg);


    //if configuration is manual mode, set reg mode only when "add button"(g_AddNewFace is set) is pressed
    uint8_t reg_mode = (Cfg_AppDataGetEnrolmentMode() == ENROLMENT_MODE_AUTO) ? OASIS_REG_MODE : 0;
    memset(&gTimeStat, 0, sizeof(gTimeStat));

    uint8_t run_flag = OASIS_DET_REC;

    if (init_p->enable_flags & OASIS_ENABLE_EMO) {
        run_flag = OASIS_DET_REC_EMO;
    }

    run_flag |= reg_mode;

    //initial state
    StartDetNoFaceTimers();
    StartRecNoFaceTimers();
    Oasis_SetState(OASIS_STATE_FACE_REC_START);

    while (1) {

#if AUTO_CALIBRATION
        //force to do liveness check in registration process
        g_AddNewFace = 1;
        lockstatus = 1;
#endif
        // pick up one response message.
        ret = xQueueReceive(gFaceDetMsgQ, (void*)&rxQMsg, portMAX_DELAY);

		//LOGD("[OASIS DETECT]:ret is %d, rxQMsg->id is %d\r\n", ret, rxQMsg->id);

        if (ret) {
            switch (rxQMsg->id) {
            case QMSG_FACEREC_FRAME_RES: {
                if (lockstatus == 1) {
                    OasisState state = Oasis_GetState();
                    //UsbShell_Printf("[OASIS]:Enter state:%d\r\n",state);
                    if (state == OASIS_STATE_FACE_REC_START){
                        Oasis_SetState(OASIS_STATE_FACE_REC_STARTED);
                    }else if (state == OASIS_STATE_FACE_ADD_START){
                        Oasis_SetState(OASIS_STATE_FACE_ADD_STARTED);
                    }else if (state == OASIS_STATE_FACE_DEL_START){
                        Oasis_SetState(OASIS_STATE_FACE_DEL_STARTED);
                    }

                    g_OASISLT_heap_debug = OASISHeapGetMinimumEverFreeHeapSize();
                    frame.data = gFaceRecBuf.data;
#if DUAL_CAMERA
                    frame2.data = gFaceRecBuf.data2;
                    //if user request to add new user, enable reg mode, it not, use default mode (get enrolment mode)
                    int ret = OASISLT_run2D(&frame, &frame2, run_flag | (g_AddNewFace ? OASIS_REG_MODE : 0), init_p->min_face, &gTimeStat);
#else
                    //if user request to add new user, enable reg mode, it not, use default mode (get enrolment mode)
                    int ret = OASISLT_run(&frame, run_flag | (g_AddNewFace ? OASIS_REG_MODE : 0), init_p->min_face, &gTimeStat);
#endif

                    if (ret) {
                        UsbShell_Printf("N:%d %d\r\n", ret, g_OASISLT_heap_debug);
                        while(1);
                    }
                    //UsbShell_Printf("[OASIS]:Exit state:%d\r\n",Oasis_GetState());

                    if (lockstatus == 0) {
                        StartLockTimers();
                        //set default pwm here
                        uint8_t pwm;
                        VIZN_GetPulseWidth(NULL, LED_IR, &pwm);
                        Camera_QMsgSetPWM(LED_IR,pwm);

#if BOARD_SUPPORT_PARALLEL_LCD
                        Camera_QMsgSetPWM(LED_WHITE, 0);
#endif
                        Camera_SetRGBExposureMode(CAMERA_EXPOSURE_MODE_AUTO_LEVEL0);

                    } else {
                        sentQMsg = &gFaceDetReqMsg;
                        Camera_SendQMsg((void*)&sentQMsg);
                    }
                }
            }
            break;

            case QMSG_FACEREC_ENROLMENTMODE: {
                run_flag &= ~(OASIS_REG_MODE);
                reg_mode = (rxQMsg->msg.cmd.data.enrolment_mode == ENROLMENT_MODE_AUTO) ? OASIS_REG_MODE : 0;
                run_flag |= reg_mode;
            }
            break;

            case QMSG_CMD: {
				LOGD("[OASIS DETECT]: rxQMsg->id QMSG_CMD\r\n");
                Oasis_ReceiveCmd(rxQMsg->msg.cmd);
            }
            break;

            default:
                assert(0);
            }

        }
    }
}


int Oasis_Start()
{
    gFaceRecBuf.data = (uint8_t*)pvPortMalloc(REC_RECT_WIDTH * REC_RECT_HEIGHT * 3);

	//LOGD("%s 1\r\n", __FUNCTION__);

    if (gFaceRecBuf.data == NULL) {
		LOGD("%s data return -1\r\n", __FUNCTION__);
        return -1;
    }

#if DUAL_CAMERA
    gFaceRecBuf.data2 = (uint8_t*)pvPortMalloc(REC_RECT_WIDTH * REC_RECT_HEIGHT * 3);

    if (gFaceRecBuf.data2 == NULL) {
		LOGD("%s data2 return -1\r\n", __FUNCTION__);
        return -1;
    }

#endif
	//LOGD("%s 2\r\n", __FUNCTION__);

    memset(&gInit_para, 0, sizeof(gInit_para));

    gInit_para.img_format = OASIS_IMG_FORMAT_BGR888;
#if DUAL_CAMERA
    if (Cfg_AppDataGetApplicationType() == APP_TYPE_ELOCK_LIGHT
        || Cfg_AppDataGetApplicationType() == APP_TYPE_ELOCK_HEAVY) {
        gInit_para.img_type = OASIS_IMG_TYPE_IR_RGB_DUAL;
    } else {
        gInit_para.img_type = OASIS_IMG_TYPE_RGB_IR_DUAL;
    }

#else

    if (LIVENESS_MODE_ON == Cfg_AppDataGetLivenessMode()) {
        gInit_para.img_type = OASIS_IMG_TYPE_IR_SINGLE;
    } else {
        gInit_para.img_type = OASIS_IMG_TYPE_RGB_SINGLE;
    }

#endif
    gInit_para.min_face = OASIS_DETECT_MIN_FACE;
	//LOGD("%s 3\r\n", __FUNCTION__);

#if DUAL_CAMERA
    gInit_para.cbs = {
        EvtHandler,
        GetRegisteredFacesHandler,
        AddNewFaceHandler,
        UpdateFaceHandler,
        AdjustBrightnessHandler,
        (void*)Oasis_Printf
    };
#else
    gInit_para.cbs = {
        EvtHandler,
        GetRegisteredFacesHandler,
        AddNewFaceHandler,
        UpdateFaceHandler,
        AdjustBrightnessHandler,
        (void*)Oasis_Printf
    };
#endif
    gInit_para.enable_flags = OASIS_ENABLE_DET | OASIS_ENABLE_REC | OASIS_ENABLE_MULTI_VIEW;
    //gInit_para.enable_flags |= OASIS_ENABLE_BRIGHTNESS_FAIL_CHECK;
    //gInit_para.enable_flags |= OASIS_ENABLE_MASK_DET_REC;

    gInit_para.emo_mode = OASIS_EMOTION_MODE_INVALID;
    if (Cfg_AppDataGetApplicationType() == APP_TYPE_ELOCK_LIGHT
        || Cfg_AppDataGetApplicationType() == APP_TYPE_DOOR_ACCESS_LIGHT){
        gInit_para.mod_class = OASISLT_MODEL_CLASS_LIGHT;
    }else{
        gInit_para.mod_class = OASISLT_MODEL_CLASS_HEAVY;
    }
    gInit_para.false_accept_rate = OASIS_FAR_1_1000000;


    gInit_para.enable_flags |= (Cfg_AppDataGetLivenessMode() == LIVENESS_MODE_ON) ? OASIS_ENABLE_LIVENESS : 0;
    uint8_t mode = Cfg_AppDataGetEmotionRecTypes();

    if (0 != mode) {
        gInit_para.emo_mode = (OASISLTEmoMode_t)mode;
        gInit_para.enable_flags |= OASIS_ENABLE_EMO;

    }
	//LOGD("%s 4\r\n", __FUNCTION__);

    if (Cfg_AppDataGetDetectResolutionMode() == DETECT_RESOLUTION_VGA) {
        gInit_para.height = REC_RECT_HEIGHT;
        gInit_para.width = REC_RECT_WIDTH;
    } else if (Cfg_AppDataGetDetectResolutionMode() == DETECT_RESOLUTION_QVGA) {
        gInit_para.height = REC_RECT_HEIGHT;
        gInit_para.width = REC_RECT_WIDTH;
    } else {
        return -4;
    }

	//LOGD("%s 5\r\n", __FUNCTION__);

    int ret = 0;
    ret = OASISLT_init(&gInit_para);

    if (ret > 0) {
        gInit_para.mem_pool = (char*)gMemPool4OasisLib;
        gInit_para.size = sizeof(gMemPool4OasisLib);
        ret = OASISLT_init(&gInit_para);
    }

    assert(ret == 0);

    /*get authentication result*/
    g_IsAuthenticated = gInit_para.auth;

	//LOGD("%s 6\r\n", __FUNCTION__);
    gFaceDetMsgQ = xQueueCreate(FACEREC_MSG_Q_COUNT, sizeof(QMsg*));

    if (gFaceDetMsgQ == NULL) {
        UsbShell_Printf("[ERROR]:xQueueCreate facedet queue\r\n");
        return -1;
    }
    sCmdMsg.id = QMSG_CMD;

    UsbShell_Printf("[OASIS]:start\r\n");


    if (xTaskCreate(Oasis_Task, "Oasis Task", OASISDETTASK_STACKSIZE,
                    &gInit_para, OASISDETTASK_PRIORITY, NULL) != pdPASS) {
        UsbShell_Printf("[ERROR]:oasis Task created failed\r\n");

        Oasis_Exit();

        while (1);
    }

    UsbShell_Printf("[OASIS]:starting\r\n");
    return ret;
}




static int Oasis_Exit()
{
    int ret = 0;
    ret     = OASISLT_uninit();

    vPortFree(gFaceRecBuf.data);
#if DUAL_CAMERA
    vPortFree(gFaceRecBuf.data2);
#endif
    return ret;
}

static void clearFaceInfoMsg(QUIInfoMsg* info)
{
    for (int i = 0; i < 4; i++) {
        info->rect[i] = -1;
    }

    memset(info->name, 0x0, 64);
    info->similar = 0.0f;
    info->dt      = 0;
    info->rt      = 0;
    std::vector<std::string> featurenames;
    DB_GetNames(&featurenames);
    info->registeredFaces = featurenames.size();

    info->newface = 0;
    info->emotion = E_INVALID;
    info->blur = 0xFF;
    info->rgbLive = 0xFF;
    info->front = 0xFF;
    info->irLive = 0xFF;
    info->irBrightness = 0xFF;
    info->irPwm = 0xFF;
    info->rgbBrightness = 0xFF;
    info->rgbPwm = 0xFF;
}

int Oasis_SendQMsg(void* msg)
{
    BaseType_t ret;
    ret = xQueueSend(gFaceDetMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS) {
        UsbShell_Printf("[ERROR]:FaceDet_SendQMsg failed\r\n");
        return -1;
    }

    return 0;
}

int Oasis_SendCmd(QCmdMsg cmd)
{
    int status = -1;
    QMsg* pQMsg;
    pQMsg = &sCmdMsg;
    pQMsg->msg.cmd.id = cmd.id;
    if(cmd.id == QCMD_ADD_FACE)
    {
        pQMsg->msg.cmd.data.name = cmd.data.name;
    }
    status = Oasis_SendQMsg((void*)&pQMsg);
    return status;
}

#endif
