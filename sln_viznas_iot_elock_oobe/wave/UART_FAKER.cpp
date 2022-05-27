//
// Created by xshx on 2022/5/24.
//
#include <vector>
#include <string>

#include "board.h"
#include "pin_mux.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "sln_api.h"
#include "MCU_UART5.h"
#include "util_crc16.h"
#include "commondef.h"
#include "user_info.h"

#include "fsl_log.h"

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif
#include "UART_FAKER.h"


static const char *logtag ="[FAKE-UART]-";

MCU_STATUS   mcuStatus=MCU_READY;//默认休眠模式

char Init_CMD[]="23010B15091B00281044014000013B76";
char Regist_CMD[]="230500111522115947018588628EE335755E4A00020437";
char Delete_CMD[]="23090812345678";
char Clear_CMD[] ="230908FFFFFFFF";

char* CMD_LIST[] = {
        Init_CMD,
        Regist_CMD,
        Delete_CMD,
        Clear_CMD
};

int     gCommandIndex=-1;
char    sCommandBuf[128]={0};

void vSetFakeCommandIndex( int index ){
    LOGD("%s SetFakeCommandIndex %d\r\n", logtag, index);

    gCommandIndex = index;
}

void vSetFakeCommandBuffer(char *buf) {
    memset(sCommandBuf, '\0', 128);
    strcpy(sCommandBuf, buf);
}

static void vFakeUartMainTask(void *pvParamters){
    LOGD("%s 创建  Fake Uart  Main TasK \r\n", logtag);
    mcuStatus = MCU_WORK;
    int commandSize = sizeof(CMD_LIST)/ sizeof(char *);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if( mcuStatus == MCU_WORK ){

            if( (gCommandIndex >-1) && (gCommandIndex < commandSize) ){
                SendMessageToUart5FromFakeUart( CMD_LIST[gCommandIndex]);
                gCommandIndex =-1;
            }
            if (strlen(sCommandBuf)>0) {
                SendMessageToUart5FromFakeUart(sCommandBuf);
                memset(sCommandBuf, '\0', 128);

            }
        }

    }
    LOGD("删除FAKE 任务 \r\n");
    vTaskDelete(NULL);

}

int uFakeUartTaskStart( ){


#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(vFakeUartMainTask, "fake uart task", FakeUartTASK_STACKSIZE, NULL, FakeUartTASK_PRIORITY,
                                        UartFakeTaskStack, &s_UartFakeTaskTCB))
#else
    if (xTaskCreate(FakeUartMainTask, "fake uart task", FakeUartTASK_STACKSIZE, NULL, FakeUartTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("Task creation failed!.\r\n");
        while (1);
    }
    return  0;
}