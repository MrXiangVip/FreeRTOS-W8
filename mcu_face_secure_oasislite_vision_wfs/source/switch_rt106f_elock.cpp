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
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "board.h"
#include "commondef.h"
#include "fsl_pcal.h"
#include "fsl_log.h"
#include "sln_shell.h"
#include "display.h"
#include "camera.h"
#include "sln_vizn_api.h"
#include "oasis.h"
#include "FreeRTOS.h"
#include "task.h"
#include "switch.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void TimerCallback(TimerHandle_t xTimer);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SWITCH_MSG_Q_COUNT 4

static QueueHandle_t SwitchMsgQ = NULL;
static QMsg SW1Msg;
static uint8_t keyNum = 0;
static QMsg CmdMsg;

static int sRunning;

#define SET_PULL_UP_EN_VALUE 0xFF
#define SET_PULL_DOWN_VALUE  0x00
#define SET_INT_MASK_VALUE   0x00
#define SET_RISING_EDGE_INT  0x15 // all 3 switch



#define TIMER_DEBOUNCE_SWITCH_1 1
static TimerHandle_t s_DebouncingTimer = NULL;
static int timer_debounce_ID           = TIMER_DEBOUNCE_SWITCH_1;

extern shell_handle_t usb_shellHandle[];
extern VIZN_api_client_t VIZN_API_CLIENT(Buttons);

/*******************************************************************************
 * Code
 ******************************************************************************/



void Switch_EnableInterrupts(void)
{
    GPIO_PortEnableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW1_GPIO_PIN));
    GPIO_PortEnableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW2_GPIO_PIN));
    GPIO_PortEnableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW3_GPIO_PIN));
#if !BOARD_SUPPORT_PARALLEL_LCD
    GPIO_PortEnableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW4_GPIO_PIN));
#endif
}

void Switch_DisableInterrupts(void)
{
    GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW1_GPIO_PIN));
    GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW2_GPIO_PIN));
    GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW3_GPIO_PIN));
#if !BOARD_SUPPORT_PARALLEL_LCD
    GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW4_GPIO_PIN));
#endif
}


/*!
* @brief Switch interface pins initialization.
*/
void Switch_Init( void )
{
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput, 0,
        kGPIO_IntFallingEdge,
    };

    /* Init input switch GPIO. */
    GPIO_PinInit(BOARD_USER_SWITCH_GPIO, SW1_GPIO_PIN, &sw_config);
    GPIO_PinInit(BOARD_USER_SWITCH_GPIO, SW2_GPIO_PIN, &sw_config);
    GPIO_PinInit(BOARD_USER_SWITCH_GPIO, SW3_GPIO_PIN, &sw_config);
#if !BOARD_SUPPORT_PARALLEL_LCD
    GPIO_PinInit(BOARD_USER_SWITCH_GPIO, SW4_GPIO_PIN, &sw_config);
#endif
    GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW1_GPIO_PIN);
    GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW2_GPIO_PIN);
    GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW3_GPIO_PIN);
#if !BOARD_SUPPORT_PARALLEL_LCD
    GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW4_GPIO_PIN);
#endif
}

static void Switch_Task(void* param)
{
    BaseType_t ret;
    QMsg* pQMsg;
    //int timerID = TIMER_DEBOUNCE_SWITCH_1 ;

    LOGD("[Switch]:running\r\n");
    Switch_Init();
    s_DebouncingTimer =
        xTimerCreate("DebounceTimer", pdMS_TO_TICKS(400), pdFALSE, (void *)&timer_debounce_ID, TimerCallback);

    if (s_DebouncingTimer != NULL)
    {
        /* Enable GPIO pin interrupt */
        NVIC_SetPriority(BOARD_USER_SWITCH_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
        EnableIRQ(BOARD_USER_SWITCH_IRQ);
        Switch_EnableInterrupts();
    }else
    {
        LOGE("[ERROR]:Create Timer for Switch Failed\r\n");
    }

    while (1) {
    	sRunning  = 1;
        ret = xQueueReceive(SwitchMsgQ, (void*)&pQMsg, portMAX_DELAY);

        if (ret == pdTRUE) {
            switch (pQMsg->id) {
            case QMSG_SWITCH_1: {
            	if(keyNum == SW3_GPIO_PIN) {
                    if (ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
                    {
                        VIZN_DelCurrentUser(&VIZN_API_CLIENT(Buttons));
                    }
                }else if(keyNum == SW2_GPIO_PIN) {
                    if (ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
                    {
                        vizn_api_status_t status;
                        status = VIZN_AddUser(&VIZN_API_CLIENT(Buttons));
                        if(status == kStatus_API_Layer_AddUser_MultipleAddCommand )
                            VIZN_AddUser(&VIZN_API_CLIENT(Buttons),(char *)"-s");
                    }
                }else if(keyNum == SW1_GPIO_PIN) {
                    if (DISPALY_MODE_RGB == Cfg_AppDataGetDisplayMode())
                    {
                        VIZN_SetDispMode(&VIZN_API_CLIENT(Buttons), DISPALY_MODE_IR);
                    }
                    else
                    {
                        VIZN_SetDispMode(&VIZN_API_CLIENT(Buttons), DISPALY_MODE_RGB);
                    }
#if !BOARD_SUPPORT_PARALLEL_LCD
               }else if(keyNum == SW4_GPIO_PIN) {
                    //vizn_api_status_t status;
                    //if(Cfg_AppDataGetApplicationType()== APP_TYPE_ELOCK)
                        //status = VIZN_SetAppType(&VIZN_API_CLIENT(Buttons),APP_TYPE_DOOR_ACCESS);
                    //else
                        //status = VIZN_SetAppType(&VIZN_API_CLIENT(Buttons),APP_TYPE_ELOCK);
                    //if(status == kStatus_API_Layer_Success) {
                        //(&VIZN_API_CLIENT(Buttons));
                        //VIZN_SystemReset(&VIZN_API_CLIENT(Buttons));
                    //}
#endif
               }
            }
            break;

            default:
                break;
            }
            //vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    vTaskDelete(NULL);
}


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
* @brief Interrupt service fuction of switch.
*/
#if !BOARD_SUPPORT_PARALLEL_LCD
void GPIO3_Combined_0_15_IRQHandler(void)
#else
void GPIO2_Combined_16_31_IRQHandler(void)
#endif
{
    //LOGD("++GPIO++\r\n");
    uint32_t intPin = 0x00;
    //Get interrupt flag for the GPIO
    intPin = GPIO_PortGetInterruptFlags(BOARD_USER_SWITCH_GPIO);

    //Check for the interrupt pin on the GPIO SW 1
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		if ( (intPin >> SW1_GPIO_PIN) & 0x01 ) {
			keyNum = SW1_GPIO_PIN;
			//LOGD("SW1 Pressed.\r\n");
			GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW1_GPIO_PIN));
			/* clear the interrupt status */
			GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW1_GPIO_PIN);
			QMsg* pQMsg = &SW1Msg;
			Switch_SendQMsgFromISR((void*)&pQMsg);
			BaseType_t HigherPriorityTaskWoken = pdFALSE;
			xTimerResetFromISR (s_DebouncingTimer,&HigherPriorityTaskWoken); //Debouncing
			portYIELD_FROM_ISR(HigherPriorityTaskWoken);
		} else if( (intPin >> SW2_GPIO_PIN) & 0x01 ) {
			keyNum = SW2_GPIO_PIN;
			//LOGD("SW2 Pressed.\r\n");
			GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW2_GPIO_PIN));
			/* clear the interrupt status */
			GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW2_GPIO_PIN);
			QMsg* pQMsg = &SW1Msg;
			Switch_SendQMsgFromISR((void*)&pQMsg);
			BaseType_t HigherPriorityTaskWoken = pdFALSE;
			xTimerResetFromISR (s_DebouncingTimer,&HigherPriorityTaskWoken); //Debouncing
			portYIELD_FROM_ISR(HigherPriorityTaskWoken);
		}else if( (intPin >> SW3_GPIO_PIN) & 0x01 ) {
			keyNum = SW3_GPIO_PIN;
			GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW3_GPIO_PIN));
			GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW3_GPIO_PIN);
			QMsg* pQMsg = &SW1Msg;
			Switch_SendQMsgFromISR((void*)&pQMsg);
			BaseType_t HigherPriorityTaskWoken = pdFALSE;
			xTimerResetFromISR (s_DebouncingTimer,&HigherPriorityTaskWoken); //Debouncing
			portYIELD_FROM_ISR(HigherPriorityTaskWoken)
#if !BOARD_SUPPORT_PARALLEL_LCD
		}else if( (intPin >> SW4_GPIO_PIN) & 0x01 ) {
			keyNum = SW4_GPIO_PIN;
			GPIO_PortDisableInterrupts(BOARD_USER_SWITCH_GPIO, (1 << SW4_GPIO_PIN));
			GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW4_GPIO_PIN);
			QMsg* pQMsg = &SW1Msg;
			Switch_SendQMsgFromISR((void*)&pQMsg);
			BaseType_t HigherPriorityTaskWoken = pdFALSE;
			xTimerResetFromISR (s_DebouncingTimer,&HigherPriorityTaskWoken); //Debouncing
			portYIELD_FROM_ISR(HigherPriorityTaskWoken);
#endif
		}

    }
}

int Switch_Start( void )
{
    LOGD("[Switch]:start\r\n");
    sRunning = 0;
    SW1Msg.id = QMSG_SWITCH_1;
    SW1Msg.msg.raw.data = &keyNum;

    CmdMsg.id = QMSG_CMD;
    SwitchMsgQ = xQueueCreate(SWITCH_MSG_Q_COUNT, sizeof(QMsg*));

    if (SwitchMsgQ == NULL) {
        LOGE("[ERROR]:xQueueCreate switch queue\r\n");
        return -1;
    }

    if (xTaskCreate(Switch_Task, "Switch Task", SWITCHTASK_STACKSIZE,
                    NULL, SWITCHTASK_PRIORITY, NULL) != pdPASS) {
        LOGE("[ERROR]:Switch Task created failed\r\n");

        while (1);
    }
    LOGD("[Switch]:started\r\n");
    return 0;
}

int Switch_SendQMsg(void *msg)
{
    BaseType_t ret;
    LOGD("[>>Switch]:sendMsg\r\n");

    while (!sRunning)
    {
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ret = xQueueSend(SwitchMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS)
    {
        LOGE("[ERROR]:Switch_SendQMsg failed\r\n");
        return -1;
    }

    LOGD("[<<Switch]:sendMsg\r\n");
    return 0;
}

int Switch_SendQMsgFromISR(void *msg)
{
    BaseType_t ret;
    LOGD("[>>Switch]:sendMsg\r\n");

    ret = xQueueSendFromISR(SwitchMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS)
    {
        LOGE("[ERROR]:Switch_SendQMsgFromISR failed\r\n");
        return -1;
    }

    LOGD("[<<Switch]:sendMsg\r\n");
    return 0;
}

static void TimerCallback(TimerHandle_t xTimer)
{
	if(*(int*)pvTimerGetTimerID(xTimer) == TIMER_DEBOUNCE_SWITCH_1){
		GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW1_GPIO_PIN);
		GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW2_GPIO_PIN);
		GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW3_GPIO_PIN);
#if !BOARD_SUPPORT_PARALLEL_LCD
		GPIO_PortClearInterruptFlags(BOARD_USER_SWITCH_GPIO, 1U << SW4_GPIO_PIN);
#endif
		Switch_EnableInterrupts();
	}
}
#endif
