#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mqtt_util.h"
#include "config.h"
#include "FreeRTOS.h"
#include "MCU_UART5_Layer.h"

#include <ctype.h>


void freePointer(char **p) {
    if (*p != NULL) {
        vPortFree(*p);
        *p = NULL;
    }
}

int mysplit(char *src, char *first, char *last, char *separator) {
	if (src == NULL || strlen(src) <= 0 || separator == NULL || strlen(separator) <= 0) {
		return -1;
	}
	char *second=(char*)strstr(src, separator); 
	int first_len = strlen(src);
	if (second != NULL) {
		first_len = strlen(src) - strlen(second);
		strncpy(first, src, first_len);
		first[first_len] = '\0';
		strncpy(last, "b", 1);
		last[1] = '\0';
		int last_len = strlen(second) - strlen(separator);
		if (last_len > 0) {
			strcpy(last, second + strlen(separator));
		} else {
			last[0] = '\0';
		}
		return 0;
	}
	return -1;
}

int mysplit2(char *src, char *first, char *separator) {
	if (src == NULL || strlen(src) <= 0 || separator == NULL || strlen(separator) <= 0) {
		return -1;
	}
	char *second=(char*)strstr(src, separator); 
	int first_len = strlen(src);
	if (second != NULL) {
		first_len = strlen(src) - strlen(separator);
		if(first_len != 0) {
			strncpy(first, src, first_len);
		}
		first[first_len] = '\0';
		return 0;
	}
	return -1;
}

int del_char(char *src, char sep)
{
	char *pTmp = src;
	unsigned int iSpace = 0;

	while (*src != '\0') {
		if (*src != sep) {
			*pTmp++ = *src;
		} else {
			iSpace++;
		}

		src++;
	}

	*pTmp = '\0';

	return iSpace;
}

char short_str[256] = {0};
char* get_short_str(const char *str) {
    if(strlen(str) < 256) {
        return (char *)str;
    }else {
        memset(short_str, 0, sizeof(short_str));
        memcpy(short_str, str, 127);
        return (char *)short_str;
    }
}

//10 MAC + 9 tv_sec + 1 random
#define MSG_ID_LEN 20
#define MSG_BT_MAC_LEN	10
static int random_gen = 1;
char *gen_msgId() {
    char *msgId = (char *) pvPortMalloc(MSG_ID_LEN + 1);
    memset(msgId, '\0', MSG_ID_LEN + 1);
    // mac
    struct timeval tv;
    tv.tv_sec = ws_systime;
    tv.tv_usec = 0;
    //gettimeofday(&tv, NULL);
    // 可能秒就够了
    // long id = tv.tv_sec*1000000 + tv.tv_usec;
    // sprintf(msgId, "%s%d%06d%03d", btWifiConfig.wifi_mac, tv.tv_sec, tv.tv_usec, random_gen);
    // sprintf(msgId, "%s%d%d", btWifiConfig.wifi_mac, tv.tv_sec, random_gen);
    long a = tv.tv_sec % 1000000000;
    char bt_mac_string[MSG_BT_MAC_LEN + 1];
    memset(bt_mac_string, '\0', MSG_BT_MAC_LEN + 1);
    memcpy(bt_mac_string, btWifiConfig.bt_mac + 2, MSG_BT_MAC_LEN);
    if (++random_gen >= 10) {
        random_gen = 1;
    }
    sprintf(msgId, "%s%09d%d", bt_mac_string, a, random_gen);
    return msgId;
}

void Remote_convertInt2ascii(void *value, int bytes, unsigned char *ascii)
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

