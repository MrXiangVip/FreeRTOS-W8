#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "mqtt-mcu.h"
#include "base64.h"
//#include "log.h"
#include "fsl_log.h"
//#include "mqtt-mq.h"
#include "mqtt-common.h"
#include "mqtt-ota.h"
#include "cJSON.h"
#include "md5.h"
#include "util.h"
#include "wifi_main.h"

#define MD5_SIZE        16
#define MD5_STR_LEN     (MD5_SIZE * 2)
#define PACK_SIZE 6000
#define OTA_DIR "/opt/smartlocker/ota/"

#include <ctype.h>
int g_total = -1;	// total saved in file
int g_total_from_server = -1; // total from server
int g_start_pack = -1; // start_pack saved in file
char g_ota_ver[MQTT_AT_LEN] = {0};

int reset_start_pack() {
	char file[MQTT_AT_LEN];
	memset(file, '\0', MQTT_AT_LEN);
	sprintf(file, "rm -rf %s%s%s", OTA_DIR, g_ota_ver, ".ota");
	system(file);
	return 0;
}

int set_start_pack() {
	if (g_ota_ver == NULL) {
		return -1;
	}
	if (g_start_pack < 0) {
		g_start_pack = 0;
	}
	FILE *ptr;
	char file[MQTT_AT_LEN];
	memset(file, '\0', MQTT_AT_LEN);
	sprintf(file, "%s%s%s", OTA_DIR, g_ota_ver, ".ota");
	ptr = fopen(file,"w");  // r for read, b for binary
	if (ptr != NULL) {
		fprintf(ptr,"%d,%d", g_total, g_start_pack);  
		fclose(ptr);
		system("sync");
		return 0;
	} else {
		LOGD("Failed to save ota g_start_pack file %s", file);
		return -1;
	}
}

int get_start_pack() {
	if (g_ota_ver == NULL) {
		return -1;
	}
	if (g_total > 0 && g_total_from_server > 0 && g_total_from_server != g_total) {
		LOGE("total not match g_total is %d g_total_from_server is %d", g_total, g_total_from_server);
		return -1;
	} else if (g_total < 0 && g_total_from_server > 0) {
		g_total = g_total_from_server;
	}
	if (g_start_pack == -1) {
		FILE *ptr;
		char file[MQTT_AT_LEN];
		memset(file, '\0', MQTT_AT_LEN);
		sprintf(file, "%s%s%s", OTA_DIR, g_ota_ver, ".ota");
		ptr = fopen(file,"r");  // r for read, b for binary
		if (ptr != NULL) {
			fscanf(ptr,"%d,%d",&g_total, &g_start_pack);  
			fclose(ptr);
			if (g_start_pack < 0) {
				g_start_pack = 0;
			}
			set_start_pack();
		} else {
			g_start_pack = 0;
			set_start_pack();
		}
	}
	if (g_total > 0 && g_total == g_start_pack) {
		LOGD("OTA is done");
		// TODO: tell mcu ota is done
	}
	return g_start_pack;
}

int start_ota() {
	LOGD("start ota 1");
	char pub_msg[MQTT_AT_LEN];
	memset(pub_msg, '\0', sizeof(pub_msg));
	get_start_pack();
	char *mid = gen_msgId();
	sprintf(pub_msg, "%s%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"pack_size\\\":%d\\,\\\"start_pack\\\":%d}", DEFAULT_HEADER, "startota:", mid , PACK_SIZE, (g_start_pack+1));
	freePointer(&mid);
	// NOTE: 此处必须异步操作
	//MessageSend(1883, pub_msg, strlen(pub_msg));
	SendMsgToMQTT(pub_msg, strlen(pub_msg));
	LOGD("start ota 2");
	return 0;
}

int flash_ota_pack(char *payload_bin, int payload_bin_len) {
#if 1
	if (g_ota_ver == NULL) {
		return -1;
	}
	FILE *ptr;
	char file[MQTT_AT_LEN];
	memset(file, '\0', MQTT_AT_LEN);
	sprintf(file, "%s/%s", OTA_DIR, g_ota_ver);

	/*if (0 != access(file, 0))
	{
		// if this folder not exist, create a new one.
		int ret = mkdir(file, 0777);   // 返回 0 表示创建成功，-1 表示失败
		if (ret != 0) {
			return ret;
		}
	}*/

	sprintf(file, "%s%s/%04d%s", OTA_DIR, g_ota_ver, get_start_pack(), ".ota");
	ptr = fopen(file,"wb"); 
	if (ptr != NULL) {
		fwrite(payload_bin, payload_bin_len, 1, ptr);
		fclose(ptr);
		system("sync");
		g_start_pack++;
		set_start_pack();
		// TODO: tell mcu keep ota alive
		if (g_total > 0 && g_start_pack == g_total) {
			LOGD("OTA is done");
			// TODO; tell mcu ota is done
		} else {
			start_ota();
		}
		return 0;
	} else {
		LOGD("Failed to save ota file %s", file);
		return -1;
	}
#else
	// TODO: do flash data to ota area
	// can readback from ota area and make sure write is complete correctly
#endif
}

int handleBase64(unsigned char *payload, char *sign, int length) {
	// decode base64
	int payload_len = strlen((const char*)payload);
	char payload_bin[UART_RX_BUF_LEN];
	int payload_bin_len = base64_decode((const char*)payload, payload_bin);
	LOGD("decode %s to %s payload_bin_len is %d \r\n", payload, payload_bin, payload_bin_len);
#if 0
	for (int i = 0; i < payload_bin_len; i++) {
		log_info("0x%02x ", (unsigned char)payload_bin[i]);
	}
#endif
	//LOGD("\n");
	if (payload_bin_len != length) {
		LOGD("length is not equal: payload_bin_len is %d but length is %d", payload_bin_len, length);
		return -1;
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

	flash_ota_pack(payload_bin, payload_bin_len);
}

int analyzeOTA(char *jsonMsg, char *msgId) {
	cJSON *mqtt = NULL;
	cJSON *msg_id = NULL;
	cJSON *j_total = NULL;
	cJSON *j_index = NULL;
	cJSON *j_sign = NULL;
	cJSON *j_length = NULL;
	cJSON *j_data = NULL;
	cJSON *j_curr_ver = NULL;
	cJSON *j_ota_ver = NULL;
	cJSON *j_need_ota = NULL;
	char *msg_idStr = NULL;
	int total = 0;
	int index = 0;
	char *dataStr = NULL;
	char *sign = NULL;
	int length = 0;
	char *curr_ver= NULL;
	char *ota_ver= NULL;
	int need_ota = -1;

	// log_info("ota jsonMsg is %s", jsonMsg);

	mqtt = cJSON_Parse(jsonMsg);
	msg_id = cJSON_GetObjectItem(mqtt, "msgId");
	msg_idStr = cJSON_GetStringValue(msg_id);
	LOGD("ota msgId is %s", msg_idStr);
	strcpy(msgId, msg_idStr);

	j_total = cJSON_GetObjectItem(mqtt, "t");
	total = (int)cJSON_GetNumberValue(j_total);
	LOGD("ota total is %d", total);
	if (total > 0) {
		g_total_from_server = total;
	}
	
	j_index = cJSON_GetObjectItem(mqtt, "i");
	index = (int)cJSON_GetNumberValue(j_index);
	LOGD("ota index is %d", index);

	j_sign = cJSON_GetObjectItem(mqtt, "s");
	sign = cJSON_GetStringValue(j_sign);
	LOGD("ota sign is %s", sign);

	j_length = cJSON_GetObjectItem(mqtt, "l");
	length = (int)cJSON_GetNumberValue(j_length);
	LOGD("ota length is %d", length);

	j_curr_ver = cJSON_GetObjectItem(mqtt, "curr_ver");
	curr_ver = cJSON_GetStringValue(j_curr_ver);
	LOGD("curr ver is %s", curr_ver);

	j_ota_ver = cJSON_GetObjectItem(mqtt, "ota_ver");
	ota_ver = cJSON_GetStringValue(j_ota_ver);
	LOGD("ota ver is %s", ota_ver);

	j_need_ota = cJSON_GetObjectItem(mqtt, "need_ota");
	need_ota = (int)cJSON_GetNumberValue(j_need_ota);
	LOGD("need ota is %d", need_ota);

	if (need_ota == 1) {
		if (curr_ver != NULL && ota_ver != NULL) {
			if (strcmp(curr_ver, ota_ver) == 0) {
				LOGD("version is the same but need_ota is 1 should be wrong");
				// TODO: tell mcu version 
				return -1;
			} else {
				// TODO: start ota request {"pack_size": PACK_SIZE, "start_pack": 5}
				strcpy(g_ota_ver, ota_ver);
				start_ota();
#if 0
				char pub_msg[80];
				memset(pub_msg, '\0', 80);
				get_start_pack();
				if (msg_idStr == NULL) {
					char *mid = gen_msgId();
					sprintf(pub_msg, "%s%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"pack_size\\\":%d\\,\\\"start_pack\\\":%d}", DEFAULT_HEADER, "startota:", mid , PACK_SIZE, (g_start_pack+1));
					freePointer(&mid);
				} else {
					sprintf(pub_msg, "%s%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"pack_size\\\":%d\\,\\\"start_pack\\\":%d}", DEFAULT_HEADER, "startota:", msg_idStr, PACK_SIZE, (g_start_pack+1));
				}
				// NOTE: 此处必须异步操作
				MessageSend(1883, pub_msg, strlen(pub_msg));
#endif
				return 0;
			}
		}
	} else if (need_ota == 0) {
		// TODO: tell mcu no need to do ota
	}

	int result = 0;

	j_data = cJSON_GetObjectItem(mqtt, "d");
	if (j_data != NULL) { 
		dataStr = cJSON_GetStringValue(j_data);
		LOGD("---JSON j_data is %s\n", dataStr);
		result = handleBase64((unsigned char*)dataStr, sign, length);
	}

	if (mqtt != NULL) {
		cJSON_Delete(mqtt);
		mqtt = NULL;
	}
	return result;
}
