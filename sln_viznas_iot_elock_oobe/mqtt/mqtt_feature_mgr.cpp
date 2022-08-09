//
// Created by wszgx on 2022/7/29.
//

#include <stdio.h>
#include "mqtt_test_mgr.h"

#include "aw_wstime.h"
#include "mqtt-common.h"
#include "mqtt_util.h"
#include "fsl_log.h"
#include "mqtt-topic.h"
#include "cJSON.h"
#include "mqtt-mcu.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_conn_mgr.h"
#include "mqtt_feature_mgr.h"
#include "base64.h"
#include "util.h"
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "DBManager.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "mqtt-mcu.h"
#include "mqtt-api.h"
#include "base64.h"
#include "config.h"
//#include "log.h"
#include "wifi_main.h"
#include "fsl_log.h"
#include "mqtt-common.h"
#include "mqtt-remote-feature.h"
#include "cJSON.h"
#include "md5.h"
#include "util.h"
#define MD5_SIZE        16
#define MD5_STR_LEN     (MD5_SIZE * 2)
#include "database.h"
#include "UART_FAKER.h"
#include "oasis.h"

#include <ctype.h>


int MqttFeatureMgr::saveFeature(char *uuid, char *payload_bin, int payload_bin_len) {
    LOGD("start save ffd len %d\r\n", payload_bin_len);
//	DB_save_feature((float*)payload_bin);
    // xshx TODO: save feature to uuid
//            int featureLen = saveFeatureToUUID(char *uuid, &payload_bin);
    int ret = 0;
    ret = DB_AddFeature_WithName(uuid, (float*)payload_bin);
    return ret;
}

int MqttFeatureMgr::verifyFeature(char *uuid, unsigned char *payload, char *sign, int length) {
    // decode base64
    int payload_len = strlen((const char*)payload);
//    char payload_bin[UART_RX_BUF_LEN];
    //int payload_bin_len = base64_decode((const char*)payload, payload_bin);
    //LOGD("decode %s to %s payload_bin_len is %d ", payload, payload_bin, payload_bin_len);
#if 0
    for (int i = 0; i < payload_bin_len; i++) {
		log_info("0x%02x ", (unsigned char)payload_bin[i]);
	}
#endif
    LOGD("\r\n");
    int payload_bin_len = (payload_len / 4 - 1) * 3 + 1;
    char *payload_bin = (char*)pvPortMalloc(payload_bin_len);
    int ret = base64_decode((const char*)payload, payload_bin);
//    char payload_str[UART_RX_BUF_LEN + UART_RX_BUF_LEN];
//    HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), payload_bin_len);
//    LOGD("handleBase64FFD payload_bin<len:%d > ret is %d %s\r\n", payload_bin_len, ret, payload_str);

    if (payload_bin_len != length) {
        if(ret != length) {
            LOGD("length is not equal: ret is %d but length is %d\r\n", ret, length);
            vPortFree(payload_bin);
            return -1;
        }
    }

    // verify sign
    unsigned char md5_value[MD5_SIZE];
    unsigned char md5_str[MD5_STR_LEN + 1];
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)payload_bin, length);
    MD5Final(&md5, md5_value);
    for(int i = 0; i < MD5_SIZE; i++)
    {
        snprintf((char*)md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }
    md5_str[MD5_STR_LEN] = '\0'; // add end
    LOGD("md5 is %s\r\n", md5_str);
    if (strncmp((const char*)md5_str, sign, MD5_STR_LEN) != 0) {
        LOGD("verify failed\r\n");
        vPortFree(payload_bin);
        return -1;
    }

    int result = saveFeature(uuid, payload_bin, payload_bin_len);
    vPortFree(payload_bin);

    return result;
}

int MqttFeatureMgr::downloadFeature(char *data) {
    cJSON *mqtt = NULL;
    cJSON *msg_id = NULL;
    cJSON *j_uuid = NULL;
    cJSON *j_sign = NULL;
    cJSON *j_length = NULL;
    cJSON *j_data = NULL;
    char *msg_idStr = NULL;
    char *uuid = 0;
    char *dataStr = NULL;
    char *sign = NULL;
    int length = 0;

    mqtt = cJSON_Parse(data);
    msg_id = cJSON_GetObjectItem(mqtt, "id");
    msg_idStr = cJSON_GetStringValue(msg_id);
    LOGD("rfd msgId is %s\r\n", msg_idStr);

    j_uuid = cJSON_GetObjectItem(mqtt, "u");
    uuid = cJSON_GetStringValue(j_uuid);
    LOGD("rfd uuid is %s\r\n", uuid);

    j_sign = cJSON_GetObjectItem(mqtt, "s");
    sign = cJSON_GetStringValue(j_sign);
    LOGD("rfd sign is %s\r\n", sign);

    j_length = cJSON_GetObjectItem(mqtt, "l");
    length = (int)cJSON_GetNumberValue(j_length);
    LOGD("rfd length is %d\r\n", length);

    int result = 0;

    j_data = cJSON_GetObjectItem(mqtt, "d");
    if (uuid != NULL && j_data != NULL) {
        dataStr = cJSON_GetStringValue(j_data);
        LOGD("---JSON j_data is %d %s\r\n", strlen(dataStr), dataStr);
        result = verifyFeature(uuid, (unsigned char*)dataStr, sign, length);
        MqttCmdMgr::getInstance()->atCmdResponse(result, msg_idStr);
    }

    if (mqtt != NULL) {
        cJSON_Delete(mqtt);
        mqtt = NULL;
    }
    return result;
}

int MqttFeatureMgr::uploadFeature(char *uuid) {
    char *msgId = MqttCmdMgr::getInstance()->genMsgId();
    char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicFeatureUpload();

    // Step 0: get feature
    int featureLen = getOasisFeatureSize();
    LOGD("--- get Feature Data size %d\r\n", featureLen);
    float *feature = (float*)pvPortMalloc(featureLen);
    memset(feature, '\0', sizeof(feature));
    int res = DB_GetFeature_ByName(uuid, feature);
    LOGD("--- DB_GetFeature_ByName res %d\r\n", res);
    if (res != DB_MGMT_OK) {
        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, msgId);
        vPortFree(feature);
        return -1;
    }

    // Step 1: base64
    int base64Len = (featureLen/3+1)*4 + 1;
    char *featureBase64 = (char*)pvPortMalloc(base64Len);
    memset(featureBase64, '\0', sizeof(featureBase64));

    base64_encode((char*)feature, featureBase64, featureLen);
//    LOGD("--- featureBase64 %s len is %d\r\n", featureBase64, strlen(featureBase64));

    // Step 2: do sign with md5
    unsigned char md5_value[MD5_SIZE];
    unsigned char md5_str[MD5_STR_LEN + 1];
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)feature, featureLen);
    MD5Final(&md5, md5_value);
    for(int i = 0; i < MD5_SIZE; i++)
    {
        snprintf((char*)md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }
    md5_str[MD5_STR_LEN] = '\0'; // add end

    // Step 3: set data with format {"msgId": msgId, "u": uuid, "s": sign, "d": base64}
    char *featureJson = (char*)pvPortMalloc(base64Len + 100);
    sprintf(featureJson,
            "{\"id\":\"%s\",\"u\":\"%s\",\"s\":%s,\"l\":%d,\"d\":\"%s\"}",
            msgId, uuid, md5_str, featureLen, featureBase64);

    int ret = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, featureJson, strlen(featureJson));
    vPortFree(feature);
    vPortFree(featureBase64);
    vPortFree(featureJson);
    return ret;
}

int MqttFeatureMgr::requestFeature(char *jsonMsg) {
    cJSON *mqtt = NULL;
    cJSON *msg_id = NULL;
    cJSON *j_uuid = NULL;
    char *msg_idStr = NULL;
    char *uuid = 0;
    int length = 0;

    mqtt = cJSON_Parse(jsonMsg);
    msg_id = cJSON_GetObjectItem(mqtt, "id");
    msg_idStr = cJSON_GetStringValue(msg_id);
    LOGD("requestFeature msgId is %s\r\n", msg_idStr);

    j_uuid = cJSON_GetObjectItem(mqtt, "u");
    uuid = cJSON_GetStringValue(j_uuid);
    LOGD("requestFeature uuid is %s\r\n", uuid);

    if (uuid == NULL) {
        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, msg_idStr);
    } else {
        uploadFeature(uuid);
    }

    if (mqtt != NULL) {
        cJSON_Delete(mqtt);
        mqtt = NULL;
    }
    return 0;
}

