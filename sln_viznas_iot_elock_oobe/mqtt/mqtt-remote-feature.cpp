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
#include "WIFI_UART8.h"
#include "fsl_log.h"
#include "mqtt-common.h"
#include "mqtt-remote-feature.h"
#include "cJSON.h"
#include "md5.h"
#include "util.h"
#define MD5_SIZE        16
#define MD5_STR_LEN     (MD5_SIZE * 2)
#include "database.h"

#include <ctype.h>

int save_remote_feature(char *uuid, char *payload_bin, int payload_bin_len) {
	LOGD("start save ffd\r\n");
	DB_save_feature((float*)payload_bin);
	return 0;
}

int handleBase64RemoteFeature(char *uuid, unsigned char *payload, char *sign, int length) {
	// decode base64
	int payload_len = strlen((const char*)payload);
	char payload_bin[UART_RX_BUF_LEN];
	//int payload_bin_len = base64_decode((const char*)payload, payload_bin);
	//LOGD("decode %s to %s payload_bin_len is %d ", payload, payload_bin, payload_bin_len);
#if 0
	for (int i = 0; i < payload_bin_len; i++) {
		log_info("0x%02x ", (unsigned char)payload_bin[i]);
	}
#endif
	//LOGD("\n");
	int payload_bin_len = (payload_len / 4 - 1) * 3 + 1;
	int ret = base64_decode((const char*)payload, payload_bin);
	char payload_str[UART_RX_BUF_LEN + UART_RX_BUF_LEN];
	HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), payload_bin_len);
	LOGD("\nhandleBase64FFD payload_bin<len:%d > ret is %d %s\r\n", payload_bin_len, ret, payload_str);


	if (payload_bin_len != length) {
		if(ret != length) {
			LOGD("length is not equal: ret is %d but length is %d\r\n", ret, length);
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
	LOGD("md5 is %s", md5_str);
	if (strncmp((const char*)md5_str, sign, MD5_STR_LEN) != 0) {
		LOGD("verify failed");
		return -1;
	}

	int result = save_remote_feature(uuid, payload_bin, payload_bin_len);

    return result;
}

int analyzeRemoteFeature(char *jsonMsg, char *msgId) {
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

	mqtt = cJSON_Parse(jsonMsg);
	msg_id = cJSON_GetObjectItem(mqtt, "msgId");
	msg_idStr = cJSON_GetStringValue(msg_id);
	LOGD("rfd msgId is %s\r\n", msg_idStr);
	strcpy(msgId, msg_idStr);

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
		result = handleBase64RemoteFeature(uuid, (unsigned char*)dataStr, sign, length);
        char pub_msg[100];
        memset(pub_msg, '\0', 100);
//        sprintf(pub_msg, "%s{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", DEFAULT_HEADER,
//                msgId, btWifiConfig.bt_mac, (result == 0 ? 0 : 1));
        sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", DEFAULT_HEADER,
                msgId, btWifiConfig.bt_mac, (result == 0 ? 0 : 1));
        // NOTE: 此处必须异步操作
        //MessageSend(1883, pub_msg, strlen(pub_msg));
        SendMsgToMQTT(pub_msg, strlen(pub_msg));
	}

	if (mqtt != NULL) {
		cJSON_Delete(mqtt);
		mqtt = NULL;
	}
	return result;
}

int getFeature(char *uuid, char *payload_data) {
return 0;
}

int requestFeatureUpload(char *jsonMsg, char *msgId) {
    cJSON *mqtt = NULL;
    cJSON *msg_id = NULL;
    cJSON *j_uuid = NULL;
    char *msg_idStr = NULL;
    char *uuid = 0;
    int length = 0;

    mqtt = cJSON_Parse(jsonMsg);
    msg_id = cJSON_GetObjectItem(mqtt, "msgId");
    msg_idStr = cJSON_GetStringValue(msg_id);
    LOGD("rfd msgId is %s\r\n", msg_idStr);
    strcpy(msgId, msg_idStr);

    j_uuid = cJSON_GetObjectItem(mqtt, "u");
    uuid = cJSON_GetStringValue(j_uuid);
    LOGD("rfd uuid is %s\r\n", uuid);

    char pub_msg[100];
    memset(pub_msg, '\0', 100);
    sprintf(pub_msg, "%s%s:%s:%s", DEFAULT_HEADER, CMD_FEATURE_UP, msg_idStr, uuid);
    // NOTE: 此处必须异步操作
    //MessageSend(1883, pub_msg, strlen(pub_msg));
    SendMsgToMQTT(pub_msg, strlen(pub_msg));

    if (mqtt != NULL) {
        cJSON_Delete(mqtt);
        mqtt = NULL;
    }
    return 0;
}
