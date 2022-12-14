/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "camera.h"
#include "fsl_camera_device.h"
#include "oasis.h"
#include "stdio.h"
#include <vector>
#include <string>
#include "database.h"
#include "oasis.h"
#include "sln_api_internal.h"
#include "sln_shell.h"
#include "MCU_UART5.h"

#include "toojpeg.h"
#include "fatfs_op.h"
#include "fsl_log.h"
#include "ff.h"

/*******************************************************************************
 * Definitions
 *******************************************************************************/
#define OASIS_DETECT_MIN_FACE 100
/* User can modify the pwm range according to the real hardware.
 * To obtain good face brightness in different light conditions.
 * Also balance power consumption and recognition distance.
 */
#define IR_PWM_MIN      0   //20
#define IR_PWM_MAX      50  //100
#define IR_PWM_INTERVAL 5   //10

#define WHITE_PWM_MIN      0
#define WHITE_PWM_MAX      40
#define WHITE_PWM_INTERVAL 10

struct TimeStat
{
    int det_start;
    int det_comp;
    int rec_start;
    int rec_comp;

    int det_fps_start;
    int det_fps;

    int rec_fps_start;
    int rec_fps;

    char new_name[31]; //used for add user

} gTimeStat;


/*******************************************************************************
 * Prototypes
 *******************************************************************************/

//__attribute__((section(".bss.$SRAM_OC")))

static void clearFaceInfo(face_info_t *face_info);
static void clearFaceInfoMsg(QUIInfoMsg *info);
static void EvtHandler(ImageFrame_t *frames[], OASISLTEvt_t evt, OASISLTCbPara_t *para, void *user_data);
// static void EvtHandler2(ImageFrame_t* frame,OASISLTEvt_t evt,OASISLTCbPara_t* para,void* user_data);
static int GetRegisteredFacesHandler(uint16_t *face_ids, void **faces, uint32_t *size, void* user_data);
static int AddNewFaceHandler(uint16_t *face_id, void *face, void* snapshot, int snapshot_len, void* user_data);
static int UpdateFaceHandler(uint16_t face_id, void *face, void* snapshot_data, int length, int offset, void* user_data);
static int Oasis_Printf(const char *formatString);
static int Oasis_Exit();
static void Oasis_Task(void *param);
static int Oasis_SetModelClass(OASISLTModelClass_t *model_class);
static int Oasis_SetImgType(OASISLTImageType_t *img_type);

/*******************************************************************************
 * Variables
 *******************************************************************************/
extern uint8_t g_RemoveExistingFace;
extern VIZN_api_client_t VIZN_API_CLIENT(Buttons);
//extern std::string g_AddNewFaceName;
static QUIInfoMsg gui_info;
static FaceRecBuffer s_FaceRecBuf = {NULL,NULL};
static QueueHandle_t gFaceDetMsgQ = NULL;
static OASISLTInitPara_t s_InitPara;
static uint8_t s_lockstatus       = 1;
static OasisState s_CurOasisState = OASIS_STATE_FACE_REC_START;
static uint8_t s_appType;
static char oasis_filename[64] = {0};
static int failedCount=0;

volatile int g_OASISLT_heap_debug;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t s_OasisTaskStack[OASISDETTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_OasisTaskTCB;
OCRAM_CACHED_BSS RAM_ADDRESS_ALIGNMENT(4) static uint8_t s_OasisMemPool[760 * 1024];
#endif

#ifdef __cplusplus
extern "C" {
#endif
int util_resize(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h, int c);
void util_crop(unsigned char* src, int srcw, int srch, unsigned char* dst, int dstw, int dsth, int top, int left, int elemsize);
#ifdef __cplusplus
}
#endif

#define OASIS_JPEG_IMG_WIDTH (90)		//50 60
#define OASIS_JPEG_IMG_HEIGHT (120)		//50 80
static uint8_t s_tmpBuffer4Jpeg[OASIS_JPEG_IMG_WIDTH*OASIS_JPEG_IMG_HEIGHT*3];
static uint32_t s_dataSizeInJpeg = 0;

static float s_OasisFeature[1024];

//#define OASIS_JPEG_IMG_BUFFER_SIZE (100*1024)
//#define OASIS_JPEG_IMG_NUM_MAX (30)
//typedef struct _OasisJpegImgCtx
//{
//	//total buffer used to save all jpeg images, images are put as
//	//  img1(x bytes), img2(y bytes), img3(z bytes)
//	uint8_t Buffers[OASIS_JPEG_IMG_BUFFER_SIZE];
//
//	//occupied size
//	int occupied_size;
//
//	//each of image's size in bytes
//	int JpegImgSizes[OASIS_JPEG_IMG_NUM_MAX];
//
//	int imgNum;
//
//}OasisJpegImgCtx;
//
//static OasisJpegImgCtx s_JpegImgContext = {0};
void Oasis_SetOasisFileName(const char *filename)
{
    LOGD("[OASIS]: %s %s\r\n",__func__, filename);
    memset(oasis_filename, 0, sizeof(oasis_filename));
    strcpy(oasis_filename, filename);
}

void Oasis_WriteJpeg() {

    if (strlen(oasis_filename) != 0) {
        int ret = fatfs_write(oasis_filename, (char *) s_tmpBuffer4Jpeg, 0, s_dataSizeInJpeg);
        if (ret == FR_OK) {
            LOGD("[OASIS]:%s  ???????????????????????? %d\r\n", oasis_filename, ret);
        } else {
            LOGD("[OASIS]:%s  ???????????????????????? %d\r\n", oasis_filename, ret);
        }
        memset(oasis_filename, 0, sizeof(oasis_filename));
    } else {
        LOGD("?????????????????????\r\n");
    }
    return;
}

char * getOasisBuffer() {
    return (char *)s_tmpBuffer4Jpeg;
}

int getOasisBufferSize() {
    return s_dataSizeInJpeg;
}
static void Oasis_WriteJpegBuffer(uint8_t byte)
{
	s_tmpBuffer4Jpeg[s_dataSizeInJpeg++] = byte;
}

float * getOasisFeature() {
	return (float *)s_OasisFeature;
}

int getOasisFeatureSize() {
	return OASISLT_getFaceItemSize();
}

/*******************************************************************************
 * Code
 *******************************************************************************/

static void clearFaceInfoMsg(QUIInfoMsg *info)
{
    std::vector<std::string> featurenames;
    for (int i = 0; i < 4; i++)
    {
        info->rect[i] = -1;
    }
    DB_GetNames(&featurenames);

    memset(info->name, 0x0, 64);
    info->similar         = 1.0f;
    info->dt              = 0;
    info->rt              = 0;
    info->registeredFaces = featurenames.size();
//    info->emotion         = 0;
    info->blur            = 0xFF;
    info->rgbLive         = 0xFF;
    info->front           = 0xFF;
    info->irLive          = 0xFF;
    info->irBrightness = 0xFF;
    info->irPwm = 0xFF;
    info->rgbBrightness = 0xFF;
    info->rgbPwm = 0xFF;
}

static void clearFaceInfo(face_info_t *face_info)
{
    face_info->dt               = 0;
    face_info->rt               = 0;
    face_info->enrolment        = false;
    face_info->recognize        = false;
    face_info->enrolment_mode   = 0;
    face_info->enrolment_result = 0;
    face_info->face_id          = 0;
    face_info->evt              =-1;// -1 ????????????????????????,????????????????????????????????????
}

static int Oasis_SendFaceInfoMsg(QUIInfoMsg info)
{
    QMsg *pFaceInfoMsg      = (QMsg*)pvPortMalloc(sizeof(QMsg));
    if (NULL == pFaceInfoMsg)
    {
        //LOGE("[ERROR]: pFaceInfoMsg pvPortMalloc failed\r\n");
        return -1;
    }
    pFaceInfoMsg->id        = QMSG_FACEREC_INFO_UPDATE;
    memcpy(&pFaceInfoMsg->msg.info, &info, sizeof(QUIInfoMsg));
    return Camera_SendQMsg((void *)&pFaceInfoMsg);
}

static int Oasis_SendFaceDetReqMsg(void *dataIR, void *dataRGB)
{
    QMsg *pFaceDetReqMsg          = (QMsg*)pvPortMalloc(sizeof(QMsg));
    if (NULL == pFaceDetReqMsg)
    {
        //LOGE("[ERROR]: pFaceDetReqMsg pvPortMalloc failed\r\n");
        return -1;
    }
    pFaceDetReqMsg->id            = QMSG_FACEREC_FRAME_REQ;
    pFaceDetReqMsg->msg.raw.IR_frame_data  = dataIR;
    pFaceDetReqMsg->msg.raw.RGB_frame_data = dataRGB;
    return Camera_SendQMsg((void *)&pFaceDetReqMsg);
}

static void EvtHandler(ImageFrame_t *frames[], OASISLTEvt_t evt, OASISLTCbPara_t *para, void *user_data)
{
    face_info_t face_info;
    struct TimeStat *timeState;


    /*The memset is corupting string pointers use clearFaceInfo function.
     The MCUX11 optimize copy trait. face_info.name = std::string(name) won't make a copy. Not seen with MCUX10.*/
    // memset(&face_info, 0, sizeof(face_info_t));
    clearFaceInfo(&face_info);
    //UsbShell_Printf("[OASIS]:evt:%d\r\n",evt);
    face_info.evt = evt;//????????????
    timeState = (struct TimeStat *)user_data;
//    LOGD("[EVT]: %d !\r\n", evt);
    switch (evt)
    {
        case OASISLT_EVT_DET_START:
        {
//            LOGD("[OASIS]:0.??????????????????!\r\n");
            timeState->det_start = Time_Now();
        }
        break;

        case OASISLT_EVT_DET_COMPLETE:
        {
//            LOGD("[OASIS]:1.????????????!\r\n");
            timeState->det_comp = Time_Now();
            gui_info.dt  = timeState->det_start - timeState->det_comp;
            if (para->faceBoxIR == NULL && para->faceBoxRGB == NULL)
            {
                memset(gui_info.name, 0x0, sizeof(gui_info.name));
            }

            if (para->faceBoxIR == NULL)
            {
                memset(gui_info.rect, -1, sizeof(para->faceBoxIR->rect));
            }
            else
            {
                memcpy(gui_info.rect, para->faceBoxIR->rect, sizeof(para->faceBoxIR->rect));
            }

            if (para->faceBoxRGB == NULL)
            {
                memset(gui_info.rect2, -1, sizeof(para->faceBoxRGB->rect));
            }
            else
            {
                // UsbShell_Printf("[EVT]:RGB detected:%d\r\n",para->faceBoxRGB->rect[0]);
                memcpy(gui_info.rect2, para->faceBoxRGB->rect, sizeof(para->faceBoxRGB->rect));
            }

            /*pit timer unit is us*/
            timeState->det_fps++;
            int diff = abs(timeState->det_fps_start - timeState->det_comp);
            if (diff > 1000000 / PIT_TIMER_UNIT && diff < 5000000 / PIT_TIMER_UNIT)
            {
                // update fps
                gui_info.detect_fps = timeState->det_fps * 1000.0f / diff;
                timeState->det_fps         = 0;
                timeState->det_fps_start   = timeState->det_comp;
            }

            VIZN_DetectEvent(gApiHandle,
                             (para->faceBoxIR == NULL && para->faceBoxRGB == NULL) ? -1 : gui_info.dt);
        }

        break;

        case OASISLT_EVT_QUALITY_CHK_START:
            break;
        case OASISLT_EVT_QUALITY_CHK_COMPLETE:
        {
            //UsbShell_Printf("[OASIS]:quality chk res:%d\r\n", para->qualityResult);
//            LOGD("[OASIS]:3.????????????!\r\n");

            gui_info.irLive  = para->reserved[5];
            gui_info.front   = para->reserved[1];
            gui_info.blur    = para->reserved[3];
            gui_info.rgbLive = para->reserved[7];
            gui_info.irBrightness = para->reserved[10];
            gui_info.rgbBrightness = para->reserved[12];

            Camera_GetPWM(LED_IR,&gui_info.irPwm);
            Camera_GetPWM(LED_WHITE,&gui_info.rgbPwm);

            //UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[irBrightness]:%d\r\n",gui_info.irBrightness);
            //LOGD("[irBrightness]:%d\r\n",gui_info.irBrightness);
            //UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[rgbBrightness]:%d\r\n",gui_info.rgbBrightness);
            //LOGD("[rgbBrightness]:%d\r\n",gui_info.rgbBrightness);
//            LOGD("[Brightness]:ir %d, rgb %d\r\n",gui_info.irBrightness, gui_info.rgbBrightness);

            if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_OK)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]:ok!\r\n");
                LOGD("[EVT]:face quality ok!\r\n");
            }
            else if (OASIS_QUALITY_RESULT_FACE_SIDE_FACE == para->qualityResult)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]:side face!\r\n");
                LOGD("[EVT]:side face!\r\n");
            }
            else if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_TOO_SMALL)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]:Small Face!\r\n");
                LOGD("[EVT]:Small Face!\r\n");
            }
            else if (para->qualityResult == OASIS_QUALITY_RESULT_FACE_BLUR)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]: Blurry Face!\r\n");
                LOGD("[EVT]: Blurry Face!\r\n");
            }
            else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_LIVENESS_IR)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]: IR Fake Face!\r\n");
                LOGD("[EVT]: IR Fake Face!\r\n");
            }
            else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_LIVENESS_RGB)
            {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]: RGB Fake Face!\r\n");
                LOGD("[EVT]: RGB Fake Face!\r\n");
            }else if (para->qualityResult == OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_DARK
                  || para->qualityResult == OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_OVEREXPOSURE) {
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[EVT]: Face Brightness unfit!\r\n");
                LOGD("[EVT]: Face Brightness unfit!\r\n");
            }else{
//                LOGD("[EVT]: face failed! invalid\r\n");
            }
        }
        break;

//        case OASISLT_EVT_MASK_CHK_START:
//        break;
//
//        case OASISLT_EVT_MASK_CHK_COMPLETE: {
//            if (para->maskResult == OASIS_MASK_CHECK_RESULT_FACE_WITH_MASK) {
//                UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[EVT]:Face with mask!\r\n");
//            } else {
//                UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[EVT]:Face without mask!\r\n");
//            }
//        }
//        break;
//
//        case OASISLT_EVT_GLASSES_CHK_START:
//        break;
//
//        case OASISLT_EVT_GLASSES_CHK_COMPLETE: {
//            if (para->glassesResult == OASIS_GLASSES_CHECK_RESULT_FACE_WITH_GLASSES) {
//                UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[EVT]:Face with glasses!\r\n");
//            } else {
//                UsbShell_DbgPrintf(VERBOSE_MODE_L2,"[EVT]:Face without glasses!\r\n");
//            }
//        }
//        break;

        case OASISLT_EVT_REC_START:
        {
            timeState->rec_start = Time_Now();
        }
        break;
        case OASISLT_EVT_REC_COMPLETE:
        {
//            LOGD("[OASIS]:5.????????????!\r\n");
            int diff;
            unsigned id                     = para->faceID;
            OASISLTRecognizeRes_t recResult = para->recResult;

            timeState->rec_comp = Time_Now();

            gui_info.rt  = timeState->rec_start - timeState->rec_comp;
            face_info.rt        = gui_info.rt;

            /*pit timer unit is us*/
            timeState->rec_fps++;
            diff = abs(timeState->rec_fps_start - timeState->rec_comp);
            if (diff > 1000000 / PIT_TIMER_UNIT)
            {
                // update fps
                gui_info.recognize_fps = timeState->rec_fps * 1000.0f / diff;
                timeState->rec_fps            = 0;
                timeState->rec_fps_start      = timeState->rec_comp;
            }
            memset(gui_info.name, 0x0, sizeof(gui_info.name));

//            recResult = OASIS_REC_RESULT_KNOWN_FACE;
            if (recResult == OASIS_REC_RESULT_KNOWN_FACE)
            {
                std::string name;
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:face id:%d\r\n", id);
                LOGD("[OASIS]:faceID:%d\r\n", id);
                DB_GetName(id, name);
                memcpy(gui_info.name, name.c_str(), name.size());
                face_info.recognize = true;
                face_info.name      = std::string(name);
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:face id:%d name:%s\r\n", id, gui_info.name);
                LOGD("[OASIS]:faceID:%d name:%s\r\n", id, gui_info.name);
                UserExtendManager::getInstance()->setCurrentUser( gui_info.name );
//              ??????????????? ???????????????  xshx add 20220825
                int ret = UserExtendManager::getInstance()->checkUUIDUserModePermission( (char *)name.c_str() );
                if( ret > R60_REC_ATTFAIL_ACCFAIL_FACE ){
                    LOGD("[OASIS]: ???????????? ?????? \r\n");
                    recResult = (OASISLTRecognizeRes_t)ret;
                }else{
                    LOGD("[OASIS]: ?????? ????????? %d \r\n",ret);
                    recResult = R60_REC_ATTFAIL_ACCFAIL_FACE;
                }
            }
            else
            {
                // face is not recognized, do nothing
                //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:face unrecognized\r\n");
//                LOGD("[OASIS]:face unrecognized\r\n");
                face_info.recognize = false;
            }

//            VIZN_RecognizeEvent(gApiHandle, face_info);
            //save face into jpeg
            //void util_crop(unsigned char* src, int srcw, int srch, unsigned char* dst, int dstw, int dsth, int top, int left, int elemsize);
//          ?????????????????????
            if (  recResult != OASIS_REC_RESULT_UNKNOWN_FACE && recResult != OASIS_REC_RESULT_INVALID )
            {
//            	int w = para->faceBoxRGB->rect[2] - para->faceBoxRGB->rect[0] + 1;
//            	int h = para->faceBoxRGB->rect[3] - para->faceBoxRGB->rect[1] + 1;
                int w,h,top,left;
                if (para->faceBoxRGB != NULL)
                {
                    w = para->faceBoxRGB->rect[2] - para->faceBoxRGB->rect[0] + 1;
                    h = para->faceBoxRGB->rect[3] - para->faceBoxRGB->rect[1] + 1;
                    top = para->faceBoxRGB->rect[1];
                    left = para->faceBoxRGB->rect[0];
                }else
                {
                    w = para->faceBoxIR->rect[2] - para->faceBoxIR->rect[0] + 1;
                    h = para->faceBoxIR->rect[3] - para->faceBoxIR->rect[1] + 1;
                    top = para->faceBoxIR->rect[1];
                    left = para->faceBoxIR->rect[0];

                }
            	uint8_t* croped = (uint8_t*)pvPortMalloc(w*h*3);
            	assert(croped != NULL);
            	util_crop(frames[OASISLT_INT_FRAME_IDX_RGB]->data,
            			frames[OASISLT_INT_FRAME_IDX_RGB]->width,
						frames[OASISLT_INT_FRAME_IDX_RGB]->height,
						croped,
						w,
						h,
						top,//para->faceBoxRGB->rect[1],
						left,//para->faceBoxRGB->rect[0],
						3);


            	//int util_resize(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h, int c);
            	//resize to special size, for example, 50*50
            	uint8_t* resized = (uint8_t*)pvPortMalloc(OASIS_JPEG_IMG_WIDTH*OASIS_JPEG_IMG_HEIGHT*3);
            	util_resize(croped,w,h,resized,OASIS_JPEG_IMG_WIDTH,OASIS_JPEG_IMG_HEIGHT,3);
            	vPortFree(croped);

            	//pay attention: our image format is BGR888, need convert to RGB888
            	for (int ii = 0;ii<OASIS_JPEG_IMG_WIDTH*OASIS_JPEG_IMG_HEIGHT;ii++)
            	{
            		uint8_t tmp = resized[3*ii];
            		resized[3*ii] = resized[3*ii + 2];
            		resized[3*ii + 2] = tmp;

            	}

            	s_dataSizeInJpeg = 0;
            	auto ok = TooJpeg::writeJpeg(Oasis_WriteJpegBuffer, resized, OASIS_JPEG_IMG_WIDTH, OASIS_JPEG_IMG_HEIGHT);
            	//UsbShell_Printf("[OASIS]:TooJpeg ret:%d file size:%d\r\n", ok,s_dataSizeInJpeg);
            	LOGD("[OASIS]:TooJpeg ret:%d file size:%d\r\n", ok,s_dataSizeInJpeg);
            	vPortFree(resized);

            }else if(recResult == OASIS_REC_RESULT_UNKNOWN_FACE){
                LOGD("[OASIS]:????????????!\r\n");
            }else if(recResult == OASIS_REC_RESULT_INVALID ){
                if( (ws_systime %10) ==0 ){
                    LOGD("[OASIS]:  ?????????!\r\n");
                }
            }
//          xshx add
            face_info.recognize_result = recResult;//??????????????????
//            VIZN_RecognizeEvent(gApiHandle, face_info);  ?????????????????? ????????????
            if( face_info.recognize == false ){
                if( failedCount > 4){
                    failedCount=0;
                    VIZN_RecognizeEvent(gApiHandle, face_info);
                }else{
                    failedCount++;
                }
            }else{
                VIZN_RecognizeEvent(gApiHandle, face_info);
            }

        }
        break;

        case OASISLT_EVT_REG_START:
        {
            LOGD("[OASIS]:???????????? !\r\n");

        }
        break;

        case OASISLT_EVT_REG_IN_PROGRESS:
            break;
        case OASISLT_EVT_REG_COMPLETE:
        {
            unsigned id              = para->faceID;
            OASISLTRegisterRes_t res = para->regResult;
            //UsbShell_Printf("[OASIS]:registration complete:%d\r\n", res);
            LOGD("[OASIS]:???????????? :%d \r\n", res);
            face_info.enrolment_result = res;
            memset(gui_info.name, 0x0, sizeof(gui_info.name));
            if ((res == OASIS_REG_RESULT_OK) || (res == OASIS_REG_RESULT_DUP))
            {
                std::string name;
                int count;
                DB_GetName(id, name);
                LOGD("[OASIS]:DB_GetName :%d ,%s\r\n", id, name.c_str() );

                memcpy(gui_info.name, name.c_str(), name.size());
                DB_Count(&count);
                // gFaceInfoMsg.msg.info.registeredFaces = featurenames.size();
                gui_info.registeredFaces = count;
                // gui_info.updateFlag |= DISPLAY_INFO_UPDATE_NAME_SIM_RT|DISPLAY_INFO_UPDATE_NEW_REG_FACE;
                face_info.dt        = gui_info.dt;
                face_info.rt        = gui_info.rt;
                face_info.name      = std::string(gui_info.name);
                face_info.enrolment = true;
            }
            else
            {
                face_info.enrolment = false;
            }
            VIZN_EnrolmentEvent(gApiHandle, face_info);

            if ((para->faceBoxRGB != NULL) && ((res == OASIS_REG_RESULT_OK) || (res == OASIS_REG_RESULT_DUP)))
            {
                int w = para->faceBoxRGB->rect[2] - para->faceBoxRGB->rect[0] + 1;
                int h = para->faceBoxRGB->rect[3] - para->faceBoxRGB->rect[1] + 1;
                uint8_t* croped = (uint8_t*)pvPortMalloc(w*h*3);
                assert(croped != NULL);
                util_crop(frames[OASISLT_INT_FRAME_IDX_RGB]->data,
                          frames[OASISLT_INT_FRAME_IDX_RGB]->width,
                          frames[OASISLT_INT_FRAME_IDX_RGB]->height,
                          croped,
                          w,
                          h,
                          para->faceBoxRGB->rect[1],
                          para->faceBoxRGB->rect[0],
                          3);


                //int util_resize(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h, int c);
                //resize to special size, for example, 50*50
                uint8_t* resized = (uint8_t*)pvPortMalloc(OASIS_JPEG_IMG_WIDTH*OASIS_JPEG_IMG_HEIGHT*3);
                util_resize(croped,w,h,resized,OASIS_JPEG_IMG_WIDTH,OASIS_JPEG_IMG_HEIGHT,3);
                vPortFree(croped);

                //pay attention: our image format is BGR888, need convert to RGB888
                for (int ii = 0;ii<OASIS_JPEG_IMG_WIDTH*OASIS_JPEG_IMG_HEIGHT;ii++)
                {
                    uint8_t tmp = resized[3*ii];
                    resized[3*ii] = resized[3*ii + 2];
                    resized[3*ii + 2] = tmp;

                }


                s_dataSizeInJpeg = 0;
                auto ok = TooJpeg::writeJpeg(Oasis_WriteJpegBuffer, resized, OASIS_JPEG_IMG_WIDTH, OASIS_JPEG_IMG_HEIGHT);
                //UsbShell_Printf("[OASIS]:TooJpeg ret:%d file size:%d\r\n", ok,s_dataSizeInJpeg);
                LOGD("[OASIS]:TooJpeg ret:%d file size:%d\r\n", ok,s_dataSizeInJpeg);
                vPortFree(resized);


#if 0
                char fn[32];
                sprintf(fn,"rec_%d.jpg", rand());
                int ret = fatfs_write(fn, (char *)s_tmpBuffer4Jpeg, 0, s_dataSizeInJpeg);
                UsbShell_Printf("[OASIS]:%s saved ret:%d\r\n",fn,ret);
#endif
            }
        }
        break;
        default:
            break;
    }

    if (evt == OASISLT_EVT_DET_COMPLETE || evt == OASISLT_EVT_REC_COMPLETE ||
        evt == OASISLT_EVT_QUALITY_CHK_COMPLETE || evt == OASISLT_EVT_REG_COMPLETE)
    {
        gui_info.similar = para->reserved[0];
        Oasis_SendFaceInfoMsg(gui_info);

		if(evt == OASISLT_EVT_REC_COMPLETE) {
//		    Uart5_GetFaceRecResult((uint8_t)face_info.recognize, gui_info.name);
            Uart5_GetFaceInfo( &face_info );
		}else if(evt == OASISLT_EVT_REG_COMPLETE) {
//		    Uart5_GetFaceRegResult((uint8_t)face_info.enrolment, gui_info.name);
            Uart5_GetFaceInfo( &face_info );
		}
    }
    return;
}

static int GetRegisteredFacesHandler(uint16_t *face_ids, void **faces, uint32_t *size, void* user_data)
{
    /*caller ask for the total records numbers*/
    if (*size == 0)
    {
        DB_Count((int*)size);
        LOGD("????????? return size %d\r\n", *size);
        return 0;
    }

    DB_GetID_FeaturePointers(face_ids,faces,*size);
    //UsbShell_Printf("GetRegisteredFacesHandler size is %d\r\n", *size);
    LOGD("????????? size is %d\r\n", *size);

    return 0;
}

static int AddNewFaceHandler(uint16_t *face_id, void *face,void* snapshot, int snapshot_len, void* user_data)
{
    vizn_api_status_t status;
    int ret = 0;
    struct TimeStat *timeState = (struct TimeStat *)user_data;

    char* name = timeState->new_name;
    memcpy(s_OasisFeature, (float *)face, getOasisFeatureSize());
    status = VIZN_EnrolmentAddNewFace(NULL, face_id, face, (strlen(name) == 0? NULL:name));
    if (status != kStatus_API_Layer_Success)
    {
        if (status == kStatus_API_Layer_EnrolmentAddNewFace_NoMemory)
        {
            //UsbShell_Printf("Maximum number of users reached\r\n");
            LOGD("Maximum number of users reached\r\n");
       }
        ret = -1;
    }
    return ret;
}

static int UpdateFaceHandler(uint16_t face_id, void *face,void* snapshot_data, int length, int offset, void* user_data)
{
    int ret;
    float *feature_data = (float *)face;
    LOGD("[OASIS] %s %d\r\n", __func__, __LINE__);
//  xshx add
    return 0;
    ret = DB_Update(face_id, feature_data);

    if (ret >= 0)
    {
        return 0;
    }
    else
    {
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
    //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[LED:%d][curPWM:%d]\r\n", led, curPWM);
    LOGD("[LED:%d][curPWM:%d][pwm:%d]\r\n", led, curPWM, pwm);
    Camera_QMsgSetPWM(led, pwm);
    return;
}

//For GC0308, we can combine maunal exposure and pwm to adjust rgb face brightness.
//For MT9M114, only use pwm to adjust rgb face brightness.
//static void Oasis_RGBControl(uint8_t direction)
//{
//    uint8_t mode = Camera_GetRGBExposureMode();
//    uint8_t pwm;
//    Camera_GetPWM(LED_WHITE,&pwm);
//    UsbShell_Printf("[OASIS]:AdjustBrightnessHandler,RGB dir:%d pwm:%d mode:%d\r\n",direction, pwm, mode);
//    Oasis_PWMControl(LED_WHITE, pwm, direction);
//
//    if ()
//    if (direction)
//    {
//        //Camera_QMsgSetPWM(LED_WHITE, pwm);
//        mode = (mode < CAMERA_EXPOSURE_MODE_AUTO_LEVEL3)? (mode + 1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL3;
//    }else
//    {
//        //Camera_QMsgSetPWM(LED_WHITE,0);
//        mode = (mode > CAMERA_EXPOSURE_MODE_AUTO_LEVEL0)? (mode-1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL0;
//    }
//    Camera_SetRGBExposureMode(mode);
//}

//For GC0308, we can combine maunal exposure and pwm to adjust rgb face brightness.
//For MT9M114, only use pwm to adjust rgb face brightness.
static void Oasis_LedControl(cfg_led_t ledID,uint8_t direction, uint8_t enableRGBModeCtrl)
{

    uint8_t pwm;
    Camera_GetPWM(ledID,&pwm);
    //UsbShell_DbgPrintf(VERBOSE_MODE_L2,"Oasis_LedControl,led:%d dir:%d pwm:%d\r\n",ledID, direction, pwm);
    LOGD("Oasis_LedControl,led:%d dir:%d pwm:%d\r\n",ledID, direction, pwm);
    Oasis_PWMControl(ledID, pwm, direction);

    if (LED_WHITE == ledID && enableRGBModeCtrl)
    {
        uint8_t mode = Camera_GetRGBExposureMode();
        //UsbShell_Printf("[OASIS]:Oasis_LedControl mode %d",mode);
        LOGD("[OASIS]:Oasis_LedControl mode %d\r\n",mode);
		if (direction)
		{
			//Camera_QMsgSetPWM(LED_WHITE, pwm);
			mode = (mode < CAMERA_EXPOSURE_MODE_AUTO_LEVEL3)? (mode + 1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL3;
		}else
		{
			//Camera_QMsgSetPWM(LED_WHITE,0);
			mode = (mode > CAMERA_EXPOSURE_MODE_AUTO_LEVEL0)? (mode-1):CAMERA_EXPOSURE_MODE_AUTO_LEVEL0;
		}
        //UsbShell_Printf("----> %d\r\n",mode);
		LOGD("----> %d\r\n",mode);
		Camera_SetRGBExposureMode(mode);
    }
}

/* Used to dynamically adjust face brightness, user can adjust brightness by modifing LED's light intensity or using manual exposure.
 *  frame_idx: which frame needs to be adjusted on, OASISLT_INT_FRAME_IDX_RGB or OASISLT_INT_FRAME_IDX_IR ?
 *  direction: 1: up, need to increase brightness;  0: down, need to reduce brightness.
 */
static void AdjustBrightnessHandler(uint8_t frame_idx, uint8_t direction, void* user_data)
{

    if (frame_idx == OASISLT_INT_FRAME_IDX_IR)
    {
        Oasis_LedControl(LED_IR,direction,0);
    }
    else
    {
        //There is a HW limitation to control LED_WHITE and LED_IR at the same time, so diable RGB part.
#if RT106F_ELOCK_BOARD
    	Oasis_LedControl(LED_WHITE,direction,1);
#endif
    }
}

static int Oasis_Printf(const char *formatString)
{
    //UsbShell_DbgPrintf(VERBOSE_MODE_L2, formatString);
    LOGD(formatString);
    return 0;
}

static void Oasis_SetState(OasisState state)
{
    s_CurOasisState = state;
    //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:oasis_state = %d\r\n", state);
}

OasisState Oasis_GetState(void)
{
   return s_CurOasisState;
}

static void Oasis_Task(void *param)
{
    BaseType_t ret;
    QMsg *rxQMsg              = NULL;
    OASISLTInitPara_t *init_p = (OASISLTInitPara_t *)param;
    ImageFrame_t frameRGB     = {(short)init_p->height, (short)init_p->width, 0, NULL};
    ImageFrame_t frameIR      = {(short)init_p->height, (short)init_p->width, 0, NULL};
    ImageFrame_t *frames[]    = {&frameRGB, &frameIR, NULL};
//    uint8_t reg_mode          = 0;
    uint8_t run_flag          = OASIS_DET_REC;
    //UsbShell_Printf("[OASIS DETECT]:running\r\n");
    LOGD("[OASIS DETECT]:running\r\n");

    // ask for the first frame
    clearFaceInfoMsg(&gui_info);
    Oasis_SendFaceInfoMsg(gui_info);
    Oasis_SendFaceDetReqMsg(s_FaceRecBuf.dataIR, s_FaceRecBuf.dataRGB);

    memset(&gTimeStat, 0, sizeof(gTimeStat));

    VIZN_StartRecognition(NULL);
    while (1)
    {
        // pick up one response message.
        ret = xQueueReceive(gFaceDetMsgQ, (void *)&rxQMsg, portMAX_DELAY);

        if (ret)
        {
            switch (rxQMsg->id)
            {
                case QMSG_FACEREC_FRAME_RES:
                {
//                    LOGD("[OASIS]:QMSG_FACEREC_FRAME_RES!\r\n");
                    if (s_lockstatus != 0)
                    {
                        g_OASISLT_heap_debug = OASISHeapGetMinimumEverFreeHeapSize();
                        frameRGB.data        = s_FaceRecBuf.dataRGB;
                        frameIR.data         = s_FaceRecBuf.dataIR;
                        // if user request to add new user, enable reg mode, it not, use default mode (get enrolment
                        // mode)
                        int ret = OASISLT_run_extend(frames, run_flag, init_p->min_face, &gTimeStat);
                        if (ret)
                        {
                            //UsbShell_Printf("N:%d %d\r\n", ret, g_OASISLT_heap_debug);
                            LOGD("N:%d %d\r\n", ret, g_OASISLT_heap_debug);
                           while (1)
                                ;
                        }
                        Oasis_SendFaceDetReqMsg(s_FaceRecBuf.dataIR, s_FaceRecBuf.dataRGB);
                    }
                }
                break;

                case QMSG_FACEREC_RECFACE:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_RECFACE!\r\n");
                    if (rxQMsg->msg.cmd.data.rec_face)
                    {
                        Oasis_SetState(OASIS_STATE_FACE_REC_START);
                    }
                    else
                    {
                        Oasis_SetState(OASIS_STATE_FACE_REC_STOP);
                    }
                }
                break;

                case QMSG_FACEREC_DELFACE:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_DELFACE!\r\n");
                    if (rxQMsg->msg.cmd.data.del_face)
                    {
                        Oasis_SetState(OASIS_STATE_FACE_DEREG_START);
                    }
                    else
                    {
                        Oasis_SetState(OASIS_STATE_FACE_DEREG_STOP);
                    }
                }
                break;

                case QMSG_FACEREC_ADDNEWFACEBY_FEA:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_ADDNEWFACEBY_FEA!\r\n");
                    memcpy(gTimeStat.new_name,rxQMsg->msg.cmd.data.add_face.new_face_name,
                			strlen(rxQMsg->msg.cmd.data.add_face.new_face_name) + 1);
                	uint16_t face_id;
                	OASISLTRegisterRes_t ret = OASISLT_registration_by_feature(rxQMsg->msg.cmd.data.add_face.feature,NULL,0,&face_id,&gTimeStat);
                	//UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:add new face by FEA, ret:%d!\r\n",ret);
                	LOGD("[OASIS]:add new face by FEA, ret:%d!\r\n",ret);
                	vPortFree(rxQMsg->msg.cmd.data.add_face.feature);


                	break;
                }

                case QMSG_FACEREC_ADDNEWFACE:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_ADDNEWFACE!\r\n");
                    if (rxQMsg->msg.cmd.data.add_face.add_newface)
                	{
                		run_flag = OASIS_DET_REC_REG;
                		Oasis_SetState(OASIS_STATE_FACE_REG_START);

                		memcpy(gTimeStat.new_name,
                				rxQMsg->msg.cmd.data.add_face.new_face_name,
								sizeof(rxQMsg->msg.cmd.data.add_face.new_face_name));
                	}else
                	{
                		run_flag = OASIS_DET_REC;
                		Oasis_SetState(OASIS_STATE_FACE_REG_STOP);
                	}
//                    run_flag &= ~(OASIS_REG_MODE);
//                    reg_mode = (rxQMsg->msg.cmd.data.add_newface) ? OASIS_REG_MODE : 0;
//                    run_flag |= reg_mode;
//                    if (reg_mode == OASIS_REG_MODE)
//                    {
//                        Oasis_SetState(OASIS_STATE_FACE_REG_START);
//                    }
//                    else
//                    {
//                        Oasis_SetState(OASIS_STATE_FACE_REG_STOP);
//                    }
                }
                break;

                case QMSG_FACEREC_STOP:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_STOP!\r\n");
                    s_lockstatus = 0;
                    //Camera_QMsgSetPWM(LED_IR, 0);
                    Camera_QMsgSetPWM(LED_WHITE, 0);
                    Camera_SetRGBExposureMode(CAMERA_EXPOSURE_MODE_AUTO_LEVEL0);
                }
                break;

                case QMSG_FACEREC_START:
                {
                    LOGD("[OASIS]:QMSG_FACEREC_START!\r\n");
                    uint8_t pwm  = 0;
                    s_lockstatus = 1;
                    //UsbShell_DbgPrintf(VERBOSE_MODE_L2, "[OASIS]:QMSG_FACEREC_START!\r\n");
                    LOGD("[OASIS]:QMSG_FACEREC_START!\r\n");
                    VIZN_GetPulseWidth(NULL, LED_IR, &pwm);
                    Camera_QMsgSetPWM(LED_IR, pwm);

                    clearFaceInfoMsg(&gui_info);
                    Oasis_SendFaceInfoMsg(gui_info);
                    Oasis_SendFaceDetReqMsg(s_FaceRecBuf.dataIR, s_FaceRecBuf.dataRGB);                    
                }
                break;

                default:
                    assert(0);
            }
        }
        vPortFree(rxQMsg);
    }
}

static int Oasis_SetImgType(OASISLTImageType_t *img_type)
{
    if (s_appType == APP_TYPE_USERID)
    {
        *img_type = OASIS_IMG_TYPE_RGB_SINGLE;
    }
    else if (s_appType == APP_TYPE_ELOCK_LIGHT || s_appType == APP_TYPE_ELOCK_HEAVY)
    {
        *img_type = OASIS_IMG_TYPE_IR_RGB_DUAL;
    }
    else if (s_appType == APP_TYPE_DOOR_ACCESS_LIGHT || s_appType == APP_TYPE_DOOR_ACCESS_HEAVY)
    {
        *img_type = OASIS_IMG_TYPE_RGB_IR_DUAL;
    }
    else
    {
        return -1;
    }
    return 0;
}

static int Oasis_SetModelClass(OASISLTModelClass_t *model_class)
{
    if (s_appType == APP_TYPE_USERID)
    {
        *model_class = OASISLT_MODEL_CLASS_LIGHT;
    }
    if (s_appType == APP_TYPE_ELOCK_LIGHT || s_appType == APP_TYPE_DOOR_ACCESS_LIGHT)
    {
        *model_class = OASISLT_MODEL_CLASS_LIGHT;
    }
    else if (s_appType == APP_TYPE_ELOCK_HEAVY || s_appType == APP_TYPE_DOOR_ACCESS_HEAVY)
    {
        *model_class = OASISLT_MODEL_CLASS_HEAVY;
    }
    else
    {
        return -1;
    }

    return 0;
}

int Oasis_Start()
{
    LOGD("[OASIS]:starting\r\n");
    //uint8_t mode = Cfg_AppDataGetEmotionRecTypes();
    s_appType    = Cfg_AppDataGetApplicationType();
    int ret      = 0;

    if (s_appType != APP_TYPE_USERID)
    {
        s_FaceRecBuf.dataIR = (uint8_t *)pvPortMalloc(REC_RECT_WIDTH * REC_RECT_HEIGHT * 3);
        if (s_FaceRecBuf.dataIR == NULL)
        {
        	ret = -1;
        	goto error_cases;
        }
    }

    s_FaceRecBuf.dataRGB = (uint8_t *)pvPortMalloc(REC_RECT_WIDTH * REC_RECT_HEIGHT * 3);
    if (s_FaceRecBuf.dataRGB == NULL)
    {
        ret = -2;
        goto error_cases;
    }

    memset(&s_InitPara, 0, sizeof(s_InitPara));

    s_InitPara.img_format = OASIS_IMG_FORMAT_BGR888;

    Oasis_SetImgType(&s_InitPara.img_type);
    Oasis_SetModelClass(&s_InitPara.mod_class);

    s_InitPara.min_face = OASIS_DETECT_MIN_FACE;
    s_InitPara.cbs.EvtCb = EvtHandler;
	s_InitPara.cbs.GetFaces = GetRegisteredFacesHandler;
	s_InitPara.cbs.AddFace = AddNewFaceHandler;
	s_InitPara.cbs.reserved = (void*)Oasis_Printf;
    s_InitPara.cbs.UpdateFace = UpdateFaceHandler;
    if (s_appType != APP_TYPE_USERID)
    {
        s_InitPara.cbs.AdjustBrightness = AdjustBrightnessHandler;
    }
    s_InitPara.cbs.lock = s_InitPara.cbs.unlock = NULL;

    s_InitPara.enable_flags = 0;
    if (s_appType != APP_TYPE_USERID)
    {
        s_InitPara.enable_flags |= OASIS_ENABLE_MULTI_VIEW;
    }
    s_InitPara.false_accept_rate = OASIS_FAR_1_1000000;
    s_InitPara.enable_flags |= (Cfg_AppDataGetLivenessMode() == LIVENESS_MODE_ON) ? OASIS_ENABLE_LIVENESS : 0;


    s_InitPara.height = REC_RECT_HEIGHT;
    s_InitPara.width  = REC_RECT_WIDTH;

    ret = OASISLT_init(&s_InitPara);

    if (ret == OASIS_INIT_INVALID_MEMORYPOOL)
    {
#if configSUPPORT_STATIC_ALLOCATION
        if (s_InitPara.size <= (int)sizeof(s_OasisMemPool))
        {
            s_InitPara.mem_pool = (char *)s_OasisMemPool;
            s_InitPara.size = sizeof(s_OasisMemPool);
        }
        else
        {
        	//allocate from heap
        	s_InitPara.mem_pool = (char *)pvPortMalloc(s_InitPara.size);
        }
#else
        s_InitPara.mem_pool = (char *)pvPortMalloc(s_InitPara.size);
#endif

        if (s_InitPara.mem_pool == NULL)
        {
            //UsbShell_Printf("[ERROR]: Unable to allocate memory for oasis mem pool\r\n");
            LOGD("[ERROR]: Unable to allocate memory for oasis mem pool\r\n");
            ret = -3;
            goto error_cases;
        }else
        {
        	ret = OASISLT_init(&s_InitPara);
        }
    }

    if (ret != OASISLT_OK)
    {
    	//if library init error, just return
    	ret = -4;
    	goto error_cases;
    }

    gFaceDetMsgQ = xQueueCreate(FACEREC_MSG_Q_COUNT, sizeof(QMsg *));

    if (gFaceDetMsgQ == NULL)
    {
        //UsbShell_Printf("[ERROR]:xQueueCreate facedet queue\r\n");
        LOGD("[ERROR]:xQueueCreate facedet queue\r\n");
        ret = -5;
        goto error_cases;
    }

    //UsbShell_Printf("[OASIS]:starting\r\n");

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(Oasis_Task, "Oasis Task", OASISDETTASK_STACKSIZE, &s_InitPara, OASISDETTASK_PRIORITY,
                                  s_OasisTaskStack, &s_OasisTaskTCB))
#else
    if (xTaskCreate(Oasis_Task, "Oasis Task", OASISDETTASK_STACKSIZE, &s_InitPara, OASISDETTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        //UsbShell_Printf("[ERROR]:oasis Task created failed\r\n");
        LOGD("[ERROR]:oasis Task created failed\r\n");
        ret = -6;
        goto error_cases;
    }

    //UsbShell_Printf("[OASIS]:started\r\n");

error_cases:
    if (ret != 0)
    {
    	//some errors happened
        LOGD("[OASIS]:error %d\r\n", ret);
        Oasis_Exit();
    }

    LOGD("[OASIS]:started %d\r\n",ret);
    return ret;
}

static int Oasis_Exit()
{

    if (s_appType != APP_TYPE_USERID)
    {
    	if (s_FaceRecBuf.dataIR != NULL)
    	{
    		vPortFree(s_FaceRecBuf.dataIR);
    		s_FaceRecBuf.dataIR = NULL;
    	}
    }
    if (s_InitPara.mem_pool)
    {
#if configSUPPORT_STATIC_ALLOCATION
		if (s_InitPara.size > (int)sizeof(s_OasisMemPool))
		{
			vPortFree(s_InitPara.mem_pool);
		}
#else
    	vPortFree(s_InitPara.mem_pool);
#endif
    	s_InitPara.mem_pool = NULL;
    }

    if (s_FaceRecBuf.dataRGB != NULL)
    {
    	vPortFree(s_FaceRecBuf.dataRGB);
    	s_FaceRecBuf.dataRGB = NULL;
    }

    if (gFaceDetMsgQ)
    {
    	vQueueDelete(gFaceDetMsgQ);
    	gFaceDetMsgQ = NULL;
    }

    int ret = OASISLT_uninit();

    return ret;
}

int Oasis_SendQMsg(void *msg)
{
    BaseType_t ret;
    if (gFaceDetMsgQ != NULL)
    {
		ret = xQueueSend(gFaceDetMsgQ, msg, (TickType_t)0);

		if (ret != pdPASS)
		{
			//UsbShell_Printf("[ERROR]:Oasis_SendQMsg failed\r\n");
			LOGD("[ERROR]:Oasis_SendQMsg failed\r\n");
			return -1;
		}
    }else
    {
    	//UsbShell_Printf("[ERROR]:Oasis_SendQMsg queue is NULL!\r\n");
    	LOGD("[ERROR]:Oasis_SendQMsg queue is NULL!\r\n");
    	return -2;
    }

    return 0;
}
