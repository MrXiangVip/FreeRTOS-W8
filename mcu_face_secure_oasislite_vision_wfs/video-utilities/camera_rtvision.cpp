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

#if RTVISION_BOARD

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
#if (APP_CAMERA_TYPE == APP_CAMERA_MT9M114)
#include "fsl_mt9m114.h"
#elif (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
#include "fsl_gc0308.h"
#endif
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
#if FOXLINK_CAMERA || DUAL_CAMERA
#define CAMERA_QTMR_BASEADDR TMR3
#define CAMERA_QTMR_PWM_CHANNEL kQTMR_Channel_1
#define CAMERA_QTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_IpgClk)
#define CAMERA_QTMR_PWM_FREQ 3000
#endif

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
static QMsg CmdMsg;
/*!< Message sent by Camera task to signal the interface mode for which the next frame will be processed */
static QMsg DIntMsg;
/*!< Pointer to the buffer that stores camera frames */
static uint16_t* bufferQueue = NULL;
uint16_t* rotateBuff = NULL;
static uint8_t pwm_ir;
#if DUAL_CAMERA
#define RGB_IR_FRAME_RATIO 2
static int8_t gCurrentCameraID = COLOR_CAMERA;
static uint8_t pwm_white;
static bool  isOddFrame = false;
#endif

/* OV7725 connect to CSI. */
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

static void Camera_LedTimer_Init()
{
#if FOXLINK_CAMERA || DUAL_CAMERA
    qtmr_config_t qtmrConfig;

    QTMR_GetDefaultConfig(&qtmrConfig);
    qtmrConfig.primarySource = kQTMR_ClockDivide_64;
    qtmrConfig.secondarySource = kQTMR_Counter2InputPin;
    QTMR_Init(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL,  &qtmrConfig);

    QTMR_SetupPwm(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL, CAMERA_QTMR_PWM_FREQ, 0, false, CAMERA_QTMR_SOURCE_CLOCK / 64);

    /* Start the counter */
    QTMR_StartTimer(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL, kQTMR_PriSrcRiseEdge);
#endif
    return;
}

static void Camera_LedTimer_Deinit()
{
#if FOXLINK_CAMERA || DUAL_CAMERA
    QTMR_StopTimer(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL);
    QTMR_Deinit(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL);
#endif
    return;
}

static void BOARD_PullCameraResetPin(bool pullUp)
{
    /* Reset pin is connected to DCDC_3V3. */
#if FOXLINK_CAMERA
    if (pullUp)
    {
        set_iox_port_pin(OUTPUT_PORT0,7,1);
    }
    else
    {
        set_iox_port_pin(OUTPUT_PORT0,7,0);
    }
#endif   
   return;
}

#if (APP_CAMERA_TYPE == APP_CAMERA_OV7725)
static void BOARD_PullCameraPowerDownPin(bool pullUp)
{
    if (pullUp)
    {
        GPIO_PinWrite(GPIO1, 4, 1);
    } else {
        GPIO_PinWrite(GPIO1, 4, 0);
    }
}

static ov7725_resource_t ov7725Resource = {
    .i2cSendFunc = BOARD_Camera_I2C_SendSCCB,
    .i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
    .inputClockFreq_Hz = 24000000,
};

camera_device_handle_t cameraDevice = {
    .resource = &ov7725Resource, .ops = &ov7725_ops,
};
#else
/*
 * MT9M114 camera module has PWDN pin, but the pin is not
 * connected internally, MT9M114 does not have power down pin.
 * The reset pin is connected to high, so the module could
 * not be reseted, so at the begining, use GPIO to let camera
 * release the I2C bus.
 */
static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;

    for (i = 0; i < 0x200; i++) 
    {
        __NOP();
    }
}

#define CAMERA_I2C_SCL_GPIO GPIO1
#define CAMERA_I2C_SCL_PIN 16
#define CAMERA_I2C_SDA_GPIO GPIO1
#define CAMERA_I2C_SDA_PIN 17

int Camera_QMsgSetPWM(uint8_t led, uint8_t pulse_width)
{
    int status = -1;
#if FOXLINK_CAMERA || DUAL_CAMERA
    QMsg* pQMsg = &CmdMsg;
    pQMsg->msg.cmd.id = QCMD_SET_PWM;
    pQMsg->msg.cmd.data.led_pwm[0] = led;
    pQMsg->msg.cmd.data.led_pwm[1] = pulse_width;
    status = Camera_SendQMsg((void*)&pQMsg);
#endif
    return status;
}

static status_t Camera_SetPWM(uint8_t pulse_width)
{
    status_t status = kStatus_Fail;
#if FOXLINK_CAMERA || DUAL_CAMERA
    QTMR_StopTimer(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL);
    status = QTMR_SetupPwm(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL, CAMERA_QTMR_PWM_FREQ, pulse_width, false, CAMERA_QTMR_SOURCE_CLOCK / 64);
    QTMR_StartTimer(CAMERA_QTMR_BASEADDR, CAMERA_QTMR_PWM_CHANNEL, kQTMR_PriSrcRiseEdge);
#endif
    return status;
}


void BOARD_I2C_ReleaseBus(void)
{
    uint8_t i = 0;
    const gpio_pin_config_t pin_config = {.direction = kGPIO_DigitalOutput, .outputLogic = 1};

    CLOCK_EnableClock(kCLOCK_Iomuxc);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_GPIO1_IO17, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16, 0U);

    GPIO_PinInit(CAMERA_I2C_SCL_GPIO, CAMERA_I2C_SCL_PIN, &pin_config);
    GPIO_PinInit(CAMERA_I2C_SDA_GPIO, CAMERA_I2C_SDA_PIN, &pin_config);

#if RTVISION_BOARD
    /* Put A71CH in reset */
    GPIO_PinInit(GPIO1, 11, &pin_config);
    GPIO_PinWrite(GPIO1, 11, 0U);
    i2c_release_bus_delay();
#endif

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(CAMERA_I2C_SDA_GPIO, CAMERA_I2C_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++) 
    {
        GPIO_PinWrite(CAMERA_I2C_SCL_GPIO, CAMERA_I2C_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(CAMERA_I2C_SDA_GPIO, CAMERA_I2C_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(CAMERA_I2C_SCL_GPIO, CAMERA_I2C_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(CAMERA_I2C_SCL_GPIO, CAMERA_I2C_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(CAMERA_I2C_SDA_GPIO, CAMERA_I2C_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(CAMERA_I2C_SCL_GPIO, CAMERA_I2C_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(CAMERA_I2C_SDA_GPIO, CAMERA_I2C_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

static void BOARD_PullCameraPowerDownPin(bool pullUp)
{
#if DUAL_CAMERA
    if (pullUp)
    {
        set_iox_port_pin(BOARD_CAMERA_PWD_GPIO, BOARD_CAMERA_PWD_GPIO_PIN, 1);
    } else {
        set_iox_port_pin(BOARD_CAMERA_PWD_GPIO, BOARD_CAMERA_PWD_GPIO_PIN, 0);
    }
#endif
}
static void BOARD_PullCameraIRPowerDownPin(bool pullUp)
{
#if DUAL_CAMERA
    if (pullUp)
    {
        set_iox_port_pin(BOARD_CAMERA_IR_PWD_GPIO, BOARD_CAMERA_IR_PWD_GPIO_PIN, 1);
    } else {
        set_iox_port_pin(BOARD_CAMERA_IR_PWD_GPIO, BOARD_CAMERA_IR_PWD_GPIO_PIN, 0);
    }
#endif
}

#if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
/*!
 * @brief Enables the swap of image data fields in the CSI module.
 *
 * To support the endianess of GC0308 camera data output, this function enables the byte swap in the CSI module.
 *
 */
void CAMERA_RECEIVER_SwapBytes(camera_receiver_handle_t *handle)
{
    uint32_t regvalue;

    /* in CSICR1 (0x402BC000) set SWAP16_EN & PACK_DIR fields to 1 */
    regvalue = (((csi_resource_t *)handle->resource)->csiBase)->CSICR1;
    *((volatile uint32_t *)( &(((csi_resource_t *)handle->resource)->csiBase)->CSICR1) ) = regvalue | 0x80000080;
}
#endif

#if DUAL_CAMERA
#if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
static gc0308_resource_t gc0308Resource[2] = {
    {//RGB
        .i2cSendFunc = BOARD_Camera_I2C_Send,
        .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
        .pullResetPin = BOARD_PullCameraResetPin,
        .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
        .inputClockFreq_Hz = 24000000,
    },
    {//IR
        .i2cSendFunc = BOARD_Camera_I2C_Send,
        .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
        .pullResetPin = BOARD_PullCameraResetPin,
        .pullPowerDownPin = BOARD_PullCameraIRPowerDownPin,
        .inputClockFreq_Hz = 24000000,
    }
};
camera_device_handle_t cameraDevice[2] = {
    {.resource = &gc0308Resource[0], .ops = &gc0308_ops,},
    {.resource = &gc0308Resource[1], .ops = &gc0308_ops}
};
#else
static mt9m114_resource_t mt9m114Resource[2] = {
    {//RGB
        .i2cSendFunc = BOARD_Camera_I2C_Send,
        .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
        .pullResetPin = BOARD_PullCameraResetPin,
        .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
        .inputClockFreq_Hz = 24000000,
        .i2cAddr = MT9M114_I2C_ADDR,
    },
    {//IR
        .i2cSendFunc = BOARD_Camera_I2C_Send,
        .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
        .pullResetPin = BOARD_PullCameraResetPin,
        .pullPowerDownPin = BOARD_PullCameraIRPowerDownPin,
        .inputClockFreq_Hz = 24000000,
        .i2cAddr = MT9M114_I2C_ADDR_IR,
    }
};
camera_device_handle_t cameraDevice[2] = {
    {.resource = &mt9m114Resource[0], .ops = &mt9m114_ops,},
    {.resource = &mt9m114Resource[1], .ops = &mt9m114_ops}
};
#endif
void Camera_RgbIrSwitch(int8_t cameraID)
{
    static int8_t last_id = -1;
    if(last_id == cameraID)
        return;
    last_id = cameraID;
    if (cameraID == COLOR_CAMERA) {
        //Camera_SelectLED(LED_WHITE);
        //Camera_SetPWM(pwm_white);
        CAMERA_DEVICE_Stop(&cameraDevice[1]);
        set_iox_port_pin(BOARD_CAMERA_SWITCH_GPIO, BOARD_CAMERA_SWITCH_GPIO_PIN, 0);
        CAMERA_DEVICE_Start(&cameraDevice[0]);
    } else if (cameraID == IR_CAMERA) {
        Camera_SelectLED(LED_IR);
        Camera_SetPWM(pwm_ir);
        CAMERA_DEVICE_Stop(&cameraDevice[0]);
        set_iox_port_pin(BOARD_CAMERA_SWITCH_GPIO, BOARD_CAMERA_SWITCH_GPIO_PIN, 1);
        CAMERA_DEVICE_Start(&cameraDevice[1]);
    }else{
        Camera_SetPWM(0);
        CAMERA_DEVICE_Stop(&cameraDevice[0]);
        CAMERA_DEVICE_Stop(&cameraDevice[1]);
    }
}

#else

#if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
static gc0308_resource_t gc0308Resource = {
    .i2cSendFunc = BOARD_Camera_I2C_Send,
    .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
    .inputClockFreq_Hz = 24000000,
};
camera_device_handle_t cameraDevice = {
     .resource = &gc0308Resource, .ops = &gc0308_ops,
};
#else
static mt9m114_resource_t mt9m114Resource = {
    .i2cSendFunc = BOARD_Camera_I2C_Send,
    .i2cReceiveFunc = BOARD_Camera_I2C_Receive,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
    .inputClockFreq_Hz = 24000000,
    .i2cAddr = MT9M114_I2C_ADDR,
};
camera_device_handle_t cameraDevice = {
    .resource = &mt9m114Resource, .ops = &mt9m114_ops,
};
#endif
#endif
#endif


int Camera_SetMonoMode(uint8_t enable )
{
    int status;
    QMsg* pQMsg = &CmdMsg;
    pQMsg->msg.cmd.id = QCMD_SET_LIVENESS_MODE;
    pQMsg->msg.cmd.data.liveness_mode = enable;
    status = Camera_SendQMsg((void*)&pQMsg);
    return status;
}

int  Camera_SetDispMode(uint8_t displayMode )
{
    int status;
    QMsg* pQMsg = &CmdMsg;
    pQMsg->msg.cmd.id = QCMD_CHANGE_RGB_IR_DISP_MODE;
    pQMsg->msg.cmd.data.display_mode = displayMode;
    status = Camera_SendQMsg((void*)&pQMsg);
    return status;
}

int  Camera_SelectLED(uint8_t led )
{
#if DUAL_CAMERA
    if (led == LED_WHITE)
    {
        set_iox_port_pin(BOARD_CAMERA_LED_SWITCH_GPIO, BOARD_CAMERA_LED_SWITCH_GPIO_PIN, 0);
    } else {
        set_iox_port_pin(BOARD_CAMERA_LED_SWITCH_GPIO, BOARD_CAMERA_LED_SWITCH_GPIO_PIN, 1);
    }
#endif
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
#if !FOXLINK_CAMERA    
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalOutput, 1,
    };
    GPIO_PinInit(GPIO1, 4, &pinConfig);
#endif

    CLOCK_EnableClock(kCLOCK_Iomuxc);           /* iomuxc clock (iomuxc_clk_enable): 0x03u */

    /* DISP_EXTCOMIN pin */
    IOMUXC_SetPinMux(
            IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,        /* GPIO_B0_05 is configured as QTIMER2_TIMER2 */
            0U);                                    /* Software Input On Field: Input Path is determined by functionality */

    IOMUXC_GPR->GPR6 = ((IOMUXC_GPR->GPR6 &
       (~( IOMUXC_GPR_GPR6_QTIMER3_TRM1_INPUT_SEL_MASK))) /* Mask bits to zero which are setting */
         | IOMUXC_GPR_GPR6_QTIMER3_TRM1_INPUT_SEL(0x00u) /* QTIMER2 TMR2 input select: 0x00u */
       );

    IOMUXC_SetPinConfig(
            IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,       /* GPIO_B0_05 PAD functional properties : */
            0x10B0u);                               /* Slew Rate Field: Slow Slew Rate
                                                     Drive Strength Field: R0/6
                                                     Speed Field: medium(100MHz)
                                                     Open Drain Enable Field: Open Drain Disabled
                                                     Pull / Keep Enable Field: Pull/Keeper Enabled
                                                     Pull / Keep Select Field: Keeper
                                                     Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                     Hyst. Enable Field: Hysteresis Disabled */

     Camera_LedTimer_Init();
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
    cameraConfig.pixelFormat = kVIDEO_PixelFormatYUYV;//kVIDEO_PixelFormatRGB888;//kVIDEO_PixelFormatRGB565;
    cameraConfig.bytesPerPixel = APP_BPP;
    cameraConfig.resolution = FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);//kVIDEO_ResolutionQVGA;//FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);
    cameraConfig.frameBufferLinePitch_Bytes = APP_CAMERA_WIDTH * APP_BPP;
    cameraConfig.interface = kCAMERA_InterfaceGatedClock;
    cameraConfig.controlFlags = APP_CAMERA_CONTROL_FLAGS;
#if DUAL_CAMERA
    #if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
        cameraConfig.framePerSec = 25;
    #else
        cameraConfig.framePerSec = 15;
    #endif
#else
    #if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
        cameraConfig.framePerSec = 14;
    #else
        cameraConfig.framePerSec = 15;
    #endif
#endif

    NVIC_SetPriority(CSI_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, Camera_Callback, NULL);
#if DUAL_CAMERA
#if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
    CAMERA_RECEIVER_SwapBytes(&cameraReceiver);
    //IR camera
    CAMERA_DEVICE_Init(&cameraDevice[1], &cameraConfig);
    CAMERA_DEVICE_Control(&cameraDevice[1],  kCAMERA_DeviceMonoMode, CAMERA_MONO_MODE_ENABLED);
    CAMERA_DEVICE_Stop(&cameraDevice[1]);
    //RGB camera
    CAMERA_DEVICE_Init(&cameraDevice[0], &cameraConfig);
    CAMERA_DEVICE_Control(&cameraDevice[0],  kCAMERA_DeviceMonoMode, CAMERA_MONO_MODE_DISABLED);
    //CAMERA_DEVICE_Control(&cameraDevice[0],  kCAMERA_DeviceExposureMode, CAMERA_EXPOSURE_MODE_MANUAL_LOW_LIGHT);
    //RGB camera on
    CAMERA_DEVICE_Start(&cameraDevice[0]);
#else
    //IR camera
    CAMERA_DEVICE_Init(&cameraDevice[1], &cameraConfig);
    CAMERA_DEVICE_Control(&cameraDevice[1],  kCAMERA_DeviceMonoMode, CAMERA_MONO_MODE_ENABLED);
    CAMERA_DEVICE_Stop(&cameraDevice[1]);
    //RGB camera

    CAMERA_DEVICE_Init(&cameraDevice[0], &cameraConfig);
    //RGB camera on
    CAMERA_DEVICE_Start(&cameraDevice[0]);
#endif
#else
#if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
    CAMERA_RECEIVER_SwapBytes(&cameraReceiver);
#endif
    if (LIVENESS_MODE_ON == Cfg_AppDataGetLivenessMode())
    {
        CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
        CAMERA_DEVICE_Start(&cameraDevice);
        CAMERA_DEVICE_Control(&cameraDevice,  kCAMERA_DeviceMonoMode, CAMERA_MONO_MODE_ENABLED);
    }
    else
    {
        CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
        CAMERA_DEVICE_Start(&cameraDevice);
        CAMERA_DEVICE_Control(&cameraDevice,  kCAMERA_DeviceMonoMode, CAMERA_MONO_MODE_DISABLED);
    }
#endif
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
#if DUAL_CAMERA
    uint8_t dispMode = DISPALY_MODE_RGB;
    VIZN_GetPulseWidth(NULL, LED_IR, &pwm_ir);
    VIZN_GetPulseWidth(NULL, LED_WHITE, &pwm_white);
#endif
    QMsg* pQMsg;
    QUIInfoMsg infoMsg;
    uint8_t* pDetData = NULL;
#if DUAL_CAMERA
    uint8_t* pDetData2 = NULL;
    bool   rgbReady = false;
    bool   irReady = false;
#endif
    uint16_t* pDispData = NULL;
    memset(&infoMsg, 0x0, sizeof(infoMsg));

    /* Open IR Cut Filter: 100ms Forward pulse */
#if FOXLINK_CAMERA
    cfg_irfilter_t mode;
    VIZN_GetIRFilter(NULL, &mode);    /* Get the IRfilter value from flash */
    VIZN_SetIRFilter(NULL, mode);    /* Set the IRfilter based on what is saved in flash */
    VIZN_GetPulseWidth(NULL, LED_IR, &pwm_ir);
    Camera_SetPWM(pwm_ir);
#endif

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
#if DUAL_CAMERA
                #if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
                    if(DQIndex % 2 == 0)  // 0 2 4 6
                    {
                        isOddFrame = false;
                        if (DQIndex % 4 == 0) // 0 4
                        {
                            gCurrentCameraID = IR_CAMERA;
                            Camera_RgbIrSwitch(COLOR_CAMERA);
                        }
                        else // 2 6
                        {
                            gCurrentCameraID = COLOR_CAMERA;
                            Camera_RgbIrSwitch(IR_CAMERA);
                        }
                    }
                    else //drop odd frames
                    {
                        isOddFrame = true;
                    }
               	#endif

                #if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
                    if((false == isOddFrame) && (COLOR_CAMERA == gCurrentCameraID))
                #else
                    if(EQIndex%(RGB_IR_FRAME_RATIO+1) < RGB_IR_FRAME_RATIO)
                #endif 
                    { //RBG frame
                        if((dispMode == DISPALY_MODE_RGB) && pDispData) {
                            if (infoMsg.rect2[0] > 0) // means can detect face in the RGB frame, so use RGB facebox to display
                                memcpy(infoMsg.rect, infoMsg.rect2, sizeof(infoMsg.rect));

                            DProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDispData;
                            DProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            DProcessMsg.msg.pxp.info        = &infoMsg;
                            pQMsg = &DProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            pDispData = NULL;
                        }

                        if (pDetData2 && irReady) {
                            FProcessMsg.msg.pxp.out_buffer  = (uint32_t) pDetData2;
                            FProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            pQMsg = &FProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            irReady = false;
                            rgbReady = false;
                            pDetData2 = NULL;
                            pDetData = NULL;
                        }
                    }
                #if (APP_CAMERA_TYPE == APP_CAMERA_GC0308)
                    else if((false == isOddFrame) && (IR_CAMERA == gCurrentCameraID))
                #else
                    else if(EQIndex%(RGB_IR_FRAME_RATIO+1) == RGB_IR_FRAME_RATIO)
                #endif
                    { //IR frame
                        if((dispMode == DISPALY_MODE_IR ) && pDispData) {
                            DProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDispData;
                            DProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            DProcessMsg.msg.pxp.info        = &infoMsg;
                            pQMsg = &DProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            pDispData = NULL;
                        }

                        if(pDetData && (!irReady)) {
                            FProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDetData;
                            FProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                            pQMsg = &FProcessMsg;
                            PXP_SendQMsg((void*)&pQMsg);
                            irReady = true;
                        }
                    }

                #if (APP_CAMERA_TYPE == APP_CAMERA_MT9M114)
                    DisableIRQ(CSI_IRQn);
                    if(DQIndex%(RGB_IR_FRAME_RATIO+1) == RGB_IR_FRAME_RATIO) {
                        Camera_RgbIrSwitch(IR_CAMERA);
                    }else{
                        Camera_RgbIrSwitch(COLOR_CAMERA);
                    }
                    EnableIRQ(CSI_IRQn);
                #endif
#else
                    if( pDispData) {
                        DProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDispData;
                        DProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                        DProcessMsg.msg.pxp.info        = &infoMsg;
                        pQMsg = &DProcessMsg;
                        PXP_SendQMsg((void*)&pQMsg);
                        pDispData = NULL;
                    }
                    if(pDetData) {
                        FProcessMsg.msg.pxp.out_buffer  = (uint32_t)pDetData;
                        FProcessMsg.msg.pxp.in_buffer   = activeFrameAddr;
                        pQMsg = &FProcessMsg;
                        PXP_SendQMsg((void*)&pQMsg);
                        pDetData = NULL;
                    }
#endif
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
#if DUAL_CAMERA
               if(!pDetData && !pDetData2)
#endif
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
            #if DUAL_CAMERA
                if (pQMsg->msg.raw.data2)
                {
                    pDetData2 = (uint8_t*)pQMsg->msg.raw.data2;
                }
            #endif
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
            case QMSG_SWITCH_1:
            {
                s_isLiveView = *(uint8_t*)pQMsg->msg.raw.data;
            }
            break;
            case QMSG_CMD:
            {
#if !DUAL_CAMERA
                if(pQMsg->msg.cmd.id == QCMD_SET_LIVENESS_MODE)
                {
                    CAMERA_DEVICE_Control(&cameraDevice,  kCAMERA_DeviceMonoMode, pQMsg->msg.cmd.data.liveness_mode);
                }
                else if(pQMsg->msg.cmd.id == QCMD_SET_PWM)
                {
                     if(pQMsg->msg.cmd.data.led_pwm[0] == LED_IR)
                     {
                        pwm_ir = pQMsg->msg.cmd.data.led_pwm[1];
                        Camera_SetPWM(pwm_ir);
                     }
                }
#else
                if(pQMsg->msg.cmd.id == QCMD_CHANGE_RGB_IR_DISP_MODE)
                {
                    dispMode = pQMsg->msg.cmd.data.display_mode;
                }
                else if(pQMsg->msg.cmd.id == QCMD_SET_PWM)
                {
                     if(pQMsg->msg.cmd.data.led_pwm[0] == LED_IR)
                     {
                         pwm_ir = pQMsg->msg.cmd.data.led_pwm[1];
                     }
                     else
                     {
                         pwm_white = pQMsg->msg.cmd.data.led_pwm[1];
                     }
                }
#endif
            }
            break;
            default:
                break;
            }
        }
    }
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
    CmdMsg.id = QMSG_CMD;
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
