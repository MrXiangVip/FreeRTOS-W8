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

#if RT106F_ELOCK_BOARD && CAMERA_SUPPORT_3D
#include <facerecicon.h>
#include <nxplogo.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "camera.h"
#include "board.h"
#include "pin_mux.h"

#include "fsl_camera.h"
#include "fsl_camera_receiver.h"
#include "fsl_camera_device.h"

#include "fsl_gpio.h"
#include "fsl_csi.h"
#include "fsl_csi_camera_adapter.h"

#include "fsl_himax.h"

#include "fsl_iomuxc.h"
#include "fsl_log.h"
#include "fsl_pxp.h"

#include "commondef.h"
#include "oasis.h"
#include "util.h"
#include "pxp.h"
#include "font.h"
#include "display.h"


#include "sln_dev_cfg.h"
#include "sln_vizn_api.h"
#include "fsl_pcal.h"
#include "fsl_common.h"
#include "fsl_qtmr.h"
#include "sln_shell.h"


/* bit 0  sync display
   bit 1  sync camera
   bit 2  sync pxp
*/
EventGroupHandle_t g_syncVideoEvents;
uint32_t activeFrameAddr;
static uint32_t inactiveFrameAddr;
static unsigned int EQIndex = 0;
static unsigned int DQIndex = 0;
static uint8_t s_isLiveView = 1;
/*!< Queue used by Camera Task to receive messages*/
static QueueHandle_t CameraMsgQ = NULL;
/*!< Message sent from CameraISR to  Camera task to signal that a frame is available */
static QMsg DQMsg;

/*!< Message sent from PXP task, then to Oasis Task by  Camera task to signal that a frame is available */
static QMsg FResMsg;
/*!< Message sent from PXP task, then to Display Task by Camera task to signal that a frame is available */
static QMsg DResMsg;

/*!< Message sent to PXP Task by Camera task to signal that a frame is available */
static QMsg FProcessMsg; //for facerec
/*!< Message sent to PXP Task by Camera task to signal that a frame is available */
static QMsg DProcessMsg; //for display
/*!< TBD */
//static QMsg CmdMsg;
/*!< Message sent by Camera task to signal the interface mode for which the next frame will be processed */
static QMsg DIntMsg;
/*!< Pointer to the buffer that stores camera frames */
static uint16_t* bufferQueue = NULL;
uint16_t* rotateBuff = NULL;
uint16_t* tempBuff = NULL;

static uint8_t pwm_ir;

static uint8_t pwm_white;
static bool  isOddFrame = false;

static uint8_t curRGBExposureMode = CAMERA_EXPOSURE_MODE_AUTO;
static uint8_t dispMode = DISPALY_MODE_RGB;

static void Camera_SetExposureMode(uint8_t mode);



static csi_resource_t csiResource = {
    .csiBase = CSI,
};

static csi_private_data_t csiPrivateData;

camera_receiver_handle_t cameraReceiver = {
    .resource = &csiResource, .ops = &csi_ops, .privateData = &csiPrivateData,
};


// Allocates an aligned memory buffer
static    void* alignedMalloc(size_t size)
{
    uint8_t* udata = (uint8_t*)pvPortMalloc(size + sizeof(void*) + FRAME_BUFFER_ALIGN);
    if (!udata)
    {
        return NULL;
    }
    uint8_t** adata = (uint8_t**)((uint32_t)(udata + FRAME_BUFFER_ALIGN) & (int32_t)(-FRAME_BUFFER_ALIGN));
    adata[-1] = udata;
    return adata;
}

// Deallocates a memory buffer
static void alignedFree(void* ptr)
{
    if (ptr)
    {
        uint8_t* udata = ((uint8_t**)ptr)[-1];
        vPortFree(udata);
    }
}


void Camera_GetPWM(uint8_t led, uint8_t* pulse_width)
{

}

int Camera_QMsgSetPWM(uint8_t led, uint8_t pulse_width)
{
    return 0;
}

static status_t Camera_SetPWM(uint8_t pwm_index, uint8_t pulse_width)
{
    return 0;
}


int Camera_SetMonoMode(uint8_t enable )
{
    return 0;
}

uint8_t Camera_GetDispMode(void)
{
    return dispMode;
}

int  Camera_SetDispMode(uint8_t displayMode )
{
    int status;
    QMsg* pQMsg = (QMsg*)pvPortMalloc(sizeof(QMsg));
    pQMsg->id = QMSG_CMD;
    pQMsg->msg.cmd.id = QCMD_CHANGE_RGB_IR_DISP_MODE;
    pQMsg->msg.cmd.data.display_mode = displayMode;
    status = Camera_SendQMsg((void*)&pQMsg);
    return status;
}

int  Camera_SelectLED(uint8_t led )
{
    return 0;
}

static void Camera_SetExposureMode(uint8_t mode)
{

}

uint8_t Camera_GetExposureMode(void)
{
    return curRGBExposureMode;
}

int Camera_QMsgSetExposureMode(uint8_t mode)
{
    return 0;
}

void BOARD_InitCameraResource(void)
{
    BOARD_Camera_I2C_Init();

    /* CSI MCLK select 24M. */
  /*
     * CSI clock source:
     *
     * 00 derive clock from osc_clk (24M)
     * 01 derive clock from PLL2 PFD2
     * 10 derive clock from pll3_120M
     * 11 derive clock from PLL3 PFD1
     */
    CLOCK_SetMux(kCLOCK_CsiMux, 0);
   /*
        * CSI clock divider:
        *
        * 000 divide by 1
        * 001 divide by 2
        * 010 divide by 3
        * 011 divide by 4
        * 100 divide by 5
        * 101 divide by 6
        * 110 divide by 7
        * 111 divide by 8
        */
    CLOCK_SetDiv(kCLOCK_CsiDiv, 0);

    /*
             * For RT1060, there is not dedicate clock gate for CSI MCLK, it use CSI
             * clock gate.
          */

    /* Set the pins for CSI reset and power down. */

    /* GPIO configuration on GPIO_AD_B0_05 (pin G14) */
    gpio_pin_config_t gpio1_pinG14_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U,
        .interruptMode = kGPIO_NoIntmode
    };
    /* Initialize GPIO functionality on GPIO_AD_B0_05 (pin G14) */

    GPIO_PinInit(BOARD_3DCAMERA_DVPMIPI_RST_GPIO, BOARD_3DCAMERA_DVPMIPI_RST_GPIO_PIN, &gpio1_pinG14_config);

    gpio_pin_config_t gpio3_pin3DWAK_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 1U,
        .interruptMode = kGPIO_NoIntmode
    };

    GPIO_PinInit(BOARD_3DCAMERA_IR_WAK_GPIO, BOARD_3DCAMERA_IR_WAK_GPIO_PIN, &gpio3_pin3DWAK_config);

    gpio_pin_config_t gpio3_pin3DRST_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U,
        .interruptMode = kGPIO_NoIntmode
    };
    GPIO_PinInit(BOARD_3DCAMERA_IR_RST_GPIO, BOARD_3DCAMERA_IR_RST_GPIO_PIN, &gpio3_pin3DRST_config);

}

void Camera_Callback(camera_receiver_handle_t* handle, status_t status, void* userData)
{
    BaseType_t HigherPriorityTaskWoken = pdFALSE;
    QMsg* pQMsg = &DQMsg;
    xQueueSendToBackFromISR(CameraMsgQ, (void*)&pQMsg, &HigherPriorityTaskWoken);
    portYIELD_FROM_ISR(HigherPriorityTaskWoken);

    //LOGD("[DQ:%d]\r\n", DQIndex);
    if (DQIndex < EQIndex) 
    {
        LOGE("Camera BQ overrun [%d:%d]\r\n", EQIndex, DQIndex);
    }

    DQIndex++;
}




static void Camera_Init_Task(void* param)
{
    camera_config_t cameraConfig;
    memset(&cameraConfig, 0, sizeof(cameraConfig));
    cameraConfig.pixelFormat = kVIDEO_PixelFormatRGB565; //kVIDEO_PixelFormatYUYV;//kVIDEO_PixelFormatRGB888;//kVIDEO_PixelFormatRGB565;
    cameraConfig.bytesPerPixel = APP_BPP;
    cameraConfig.resolution = FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);//kVIDEO_ResolutionQVGA;//FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);
    cameraConfig.frameBufferLinePitch_Bytes = APP_CAMERA_WIDTH * APP_BPP;
    cameraConfig.interface = kCAMERA_InterfaceGatedClock;
    cameraConfig.controlFlags = APP_CAMERA_CONTROL_FLAGS;
    cameraConfig.framePerSec = 15;

    NVIC_SetPriority(CSI_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, Camera_Callback, NULL);

    hx_Init();

    /* Submit the empty frame buffers to buffer queue. */
    for (uint32_t i = 0; i < APP_FRAME_BUFFER_COUNT; i++)
    {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(bufferQueue+i*APP_CAMERA_HEIGHT*APP_CAMERA_WIDTH));
    }

    CAMERA_RECEIVER_Start(&cameraReceiver);

    LOGD("[Camera]:running\r\n");
    xEventGroupSetBits(g_syncVideoEvents, 1 << SYNC_VIDEO_CAMERA_INIT_BIT);

    vTaskDelete(NULL);
}

static void Camera_Task(void* param)
{
    BaseType_t ret;
    QMsg* pQMsg;
    QUIInfoMsg infoMsg;
    uint8_t* pDetData = NULL;
    uint8_t* pDetDataDepth = NULL;
    uint16_t* pDispData = NULL;
    memset(&infoMsg, 0x0, sizeof(infoMsg));

    xEventGroupWaitBits(g_syncVideoEvents, 1 << SYNC_VIDEO_CAMERA_INIT_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    while (1)
    {
        // pick up message
        ret = xQueueReceive(CameraMsgQ, (void*)&pQMsg, portMAX_DELAY);

        if (ret == pdTRUE) {
            switch (pQMsg->id) {
            case QMSG_CAMERA_DQ: {
                if (kStatus_Success == CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &activeFrameAddr)) 
                {
                    uint8_t frameType = hx_cam_ctrl_get_frame_type((uint8_t *)activeFrameAddr, APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);

                    if(FRAME_TYPE_IR == frameType)
                    { //ir frame
                        if(pDispData){
                            DProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDispData;
                            DProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            DProcessMsg.msg.pxp.info        = &infoMsg;
                            DProcessMsg.msg.pxp.type        = 0;
                            pQMsg = &DProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            pDispData = NULL;
                        }

                        if(pDetData) {
                            FProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDetData;
                            FProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            FProcessMsg.msg.pxp.type        = 1;
                            pQMsg = &FProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            pDetData = NULL;
                        }
                    }
                    else if (FRAME_TYPE_DEPTH == frameType)
                    { //depth frame
                        if(pDetDataDepth) {
                            FProcessMsg.msg.pxp.out_buffer  = (uint32_t) pDetDataDepth;
                            FProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            FProcessMsg.msg.pxp.type        = 2;
                            pQMsg = &FProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            pDetDataDepth = NULL;
                        }
                    }
                    inactiveFrameAddr = activeFrameAddr;
                    CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, inactiveFrameAddr);

                    //LOGD("[EQ:%d]\r\n", EQIndex);
                    if (DQIndex < EQIndex) 
                    {
                        LOGE("Camera BQ overrun [%d:%d]\r\n", EQIndex, DQIndex);
                    }

                    EQIndex++;
                }
                else
                {
                    LOGE("[ERROR]:Camera DQ buffer\r\n");
                }
            }
            break;

            case QMSG_PXP_DISPLAY:
            {
                pQMsg = &DResMsg;
                Display_SendQMsg((void*)&pQMsg);
            }
            break;

            case QMSG_PXP_FACEREC:
            {
               if(!pDetData && !pDetDataDepth)
               {
                    pQMsg = &FResMsg;
                    Oasis_SendQMsg((void*)&pQMsg);
               }
            }
            break;

            case QMSG_FACEREC_FRAME_REQ:
            {
                if (pQMsg->msg.raw.data)
                {
                    pDetData = (uint8_t*)pQMsg->msg.raw.data;
                }
                if (pQMsg->msg.raw.data2)
                {
                    pDetDataDepth = (uint8_t*)pQMsg->msg.raw.data2;
                }
            }
            break;

            case QMSG_DISPLAY_FRAME_REQ:
            {
                if (pQMsg->msg.raw.data)
                {
                    pDispData = (uint16_t*)pQMsg->msg.raw.data;
                }
            }
            break;

            case QMSG_FACEREC_INFO_UPDATE:
            {
                infoMsg = pQMsg->msg.info;
                //LOGD("[rect:%d/%d/%d/%d]\r\n", infoMsg.rect[0], infoMsg.rect[1], infoMsg.rect[2], infoMsg.rect[3]);
                //LOGD("[name:%s/%f]\r\n", pQMsg->msg.info.name, pQMsg->msg.info.similar);
            }
            break;
            case QMSG_CMD:
            {

            }
            break;
            default:
                break;
            }
        }
    }

    if(tempBuff)
        alignedFree(tempBuff);
    if(bufferQueue)
        alignedFree(bufferQueue);
#if (1 == CAMERA_ROTATE_FLAG)
    if(rotateBuff)
        alignedFree(rotateBuff);
#endif
}

int Camera_Start()
{
    LOGD("[Camera]:starting...\r\n");
    //BOARD_InitCameraResource();
    DQMsg.id = QMSG_CAMERA_DQ;
    FResMsg.id = QMSG_FACEREC_FRAME_RES;
    DResMsg.id = QMSG_DISPLAY_FRAME_RES;


    FProcessMsg.id = QMSG_PXP_FACEREC;
    DProcessMsg.id = QMSG_PXP_DISPLAY;

    CameraMsgQ = xQueueCreate(CAMERA_MSG_Q_COUNT, sizeof(QMsg*));
    if (CameraMsgQ == NULL)
    {
        LOGE("[ERROR]:xQueueCreate camera queue\r\n");
        return -1;
    }

    g_syncVideoEvents = xEventGroupCreate();
    if(g_syncVideoEvents == NULL)
    {
        LOGE("[ERROR]Event Group failed\r\n");
        while (1);
    }

    int buffersize = APP_FRAME_BUFFER_COUNT*APP_CAMERA_HEIGHT*APP_CAMERA_WIDTH;
    bufferQueue = (uint16_t*)alignedMalloc(buffersize * sizeof(uint16_t));
    if(bufferQueue == NULL){
        while(1);
    }

    buffersize = APP_CAMERA_HEIGHT*APP_CAMERA_WIDTH;
    tempBuff = (uint16_t*)alignedMalloc(buffersize * sizeof(uint16_t));
    if(tempBuff == NULL){
        LOGE("get temp buff failed\n");
        while(1);
    }

#if (1 == CAMERA_ROTATE_FLAG)
    buffersize = APP_CAMERA_HEIGHT*APP_CAMERA_WIDTH;
    rotateBuff = (uint16_t*)alignedMalloc(buffersize * sizeof(uint16_t));
    if(rotateBuff == NULL){
        LOGE("get rotate buff failed\n");
        while(1);
    }
#else
    rotateBuff = NULL;
#endif

    memset(bufferQueue, 0x0, buffersize * sizeof(uint16_t));

    if (xTaskCreate(Camera_Init_Task, "Camera_Init", CAMERAINITTASK_STACKSIZE,
                    NULL, CAMERAINITTASK_PRIORITY, NULL) != pdPASS)
    {
        LOGE("[ERROR]Camera Init created failed\r\n");
        while (1);
    }

    if (xTaskCreate(Camera_Task, "Camera Task", CAMERATASK_STACKSIZE,
                    NULL, CAMERATASK_PRIORITY, NULL) != pdPASS) 
    {
        LOGE("[ERROR]Camera Task created failed\r\n");
        while (1);
    }

    LOGD("[Camera]:started\r\n");
    return 0;
}

int Camera_SendQMsg(void* msg)
{
    BaseType_t ret;

    ret = xQueueSend(CameraMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS) 
    {
        LOGE("[ERROR]:Camera_SendQMsg failed %d\r\n", ret);
        return -1;
    }

    return 0;
}

#endif
