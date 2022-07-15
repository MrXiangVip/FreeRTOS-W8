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
#include "database.h"
#include "fsl_log.h"

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif
#include "UART_FAKER.h"


string name;
float *feature;
uint8_t *feature_hex_str;

static const char *logtag ="[FAKE-UART]-";

MCU_STATUS   mcuStatus=MCU_READY;//默认休眠模式

//#ifdef L25
//char Init_CMD[]="2301000B15091B00281044014000013B76";                       //
//char Regist_CMD[]="230500111522115947018588628EE335755E4A00020437";         // 8字节UID + 4 字节起始时间 +4 字节结束时间 + 扩展柜门号
//char Delete_CMD[]="230900091A2B3C4D5F6677880402AA";                         //
//char Clear_CMD[] ="230B0000030E";                                           //
//char OpenDoor_CMD[] ="23030003640200016D";                                  //
//#endif

//#ifdef W8
char Init_CMD[]="23010B150909020B334E01330001DE78";                         //
char Regist_CMD[]="2305089E51973FCD6365A1EEAD";                             //  8字节UUID
char Delete_CMD[]="2309089E51973FCD6365A15B74";                             //
char Clear_CMD[] ="230B003BCE";                                             //
char OpenDoor_CMD[] ="23030003430201004C";                                  //
//#endif

char* CMD_LIST[] = {
        Init_CMD,
        Regist_CMD,
        Delete_CMD,
        Clear_CMD,
        OpenDoor_CMD,

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

            if( gCommandIndex >-1 ){
                if( gCommandIndex < commandSize ){
                    SendMessageToUart5FromFakeUart(CMD_LIST[gCommandIndex]);
                    gCommandIndex = -1;
                }else{
                    vAddAIdNameFeature((char *)name.c_str(), feature);
                    gCommandIndex = -1;
                }
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

static void Remote_convertInt2ascii(void *value, int bytes, unsigned char *ascii)
{
    unsigned char nibble;
    unsigned char hex_table[] = "0123456789abcdef";
    unsigned char *p_value    = (unsigned char *)value;
    unsigned char *p_ascii    = ascii;

    for (int j = 0; j < bytes; j++)
    {
        nibble     = *p_value++;
        int low    = nibble & 0x0f;
        int high   = (nibble >> 4) & 0x0f;
        *p_ascii++ = hex_table[high];
        *p_ascii++ = hex_table[low];
    }
}


static void Remote_convertAscii2int(unsigned char *ascii, int len, void *value)
{
    unsigned char nibble[8];
    unsigned char *value_char = (unsigned char *)value;

    assert(len == 4 || len == 8);

    for (int j = 0; j < len; j++)
    {
        if (ascii[j] <= 'f' && ascii[j] >= 'a')
        {
            nibble[j] = ascii[j] - 'a' + 10;
        }
        else if (ascii[j] <= '9' && ascii[j] >= '0')
        {
            nibble[j] = ascii[j] - '0';
        }
        else
        {
            nibble[j] = 0;
        }
    }

    value_char[0] = nibble[0] << 4 | nibble[1];
    value_char[1] = nibble[2] << 4 | nibble[3];
    if (len == 8)
    {
        value_char[2] = nibble[4] << 4 | nibble[5];
        value_char[3] = nibble[6] << 4 | nibble[7];
    }
}

void vListAllIdNameFeature( ){
    LOGD("%s ,%s \r\n", logtag, __func__);

    std::vector<uint16_t> allIDs;
    DB_GetIDs(allIDs);

    feature                            = (float *)pvPortMalloc(OASISLT_getFaceItemSize());
    feature_hex_str                       = (uint8_t *)pvPortMalloc(OASISLT_getFaceItemSize() * 2 + 1);
    feature_hex_str[OASISLT_getFaceItemSize() * 2] = '\0';

    for (uint32_t i = 0; i < allIDs.size(); i++) {
        // get idx+i ids form DB

        DB_GetName(allIDs[i], name);
        LOGD("%s id %d ,name %s \r\n",logtag, i, name.c_str());
//        DB_GetFeature(allIDs[i], face);
        DB_GetFeature_ByName( name, feature);
        Remote_convertInt2ascii(feature, OASISLT_getFaceItemSize(), feature_hex_str);
        LOGD("%s id %d ,feature %f, feature str %s \r\n",logtag, i, *feature, feature_hex_str);
    }
//    vPortFree(face);
    return;
}

int vAddAIdNameFeatureStr(char *name, uint8_t* feature_hex_str){
    LOGD("%s, %s, names[%s] features[%s] \r\n", logtag, __func__, name, feature_hex_str);
    int ret=-1;
    string strName =name;
    unsigned char *pFeature = feature_hex_str;
    float *feature          = (float *)pvPortMalloc(OASISLT_getFaceItemSize());
    if (feature == NULL)
    {
        LOGD("pvmalloc error \r\n");
    }

    float fValue;
    for (unsigned int i = 0; i < FEATUREDATA_FEATURE_SIZE; i++)
    {
        Remote_convertAscii2int(pFeature, sizeof(fValue) * 2, &fValue);
        pFeature += sizeof(fValue) * 2;
        feature[i] = fValue;
    }

    ret = DB_AddFeature_WithName( strName, feature);
    LOGD("remote register [%s] success.\r\n", strName.c_str() );
    vPortFree(feature);
    return ret;
}

int vAddAIdNameFeature(char *name, float * feature){
    LOGD("%s add id names[%s] features[%s] \r\n", logtag, name, feature_hex_str);
    int ret =-1;
    string strName =name;
    float *pFeature                 = feature;
    unsigned  char *feature_hex_str = (uint8_t *)pvPortMalloc(OASISLT_getFaceItemSize() * 2 + 1);
    if (feature_hex_str == NULL)
    {
        LOGD("pvmalloc error \r\n");
    }

    Remote_convertInt2ascii(pFeature, OASISLT_getFaceItemSize(), feature_hex_str);
    ret = DB_AddFeature_WithName( strName, pFeature);
    LOGD("remote register [%s] success.\r\n", strName.c_str() );
    vPortFree(feature_hex_str);
    return ret;
}


int uFakeUartTaskStart( ){

    vListAllIdNameFeature();

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