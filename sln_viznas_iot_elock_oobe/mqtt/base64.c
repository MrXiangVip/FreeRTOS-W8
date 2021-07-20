#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "mqtt-api.h"
#include "base64.h"
#include "fsl_log.h"
#include "oasis.h"

//#define MQTT_PUB_PACKAGE_LEN    126
//#define MQTT_PUB_MSG_LEN        256

#define MQTT_PUB_PACKAGE_LEN        1826
#define MQTT_PUB_MSG_LEN            2000


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char data[MQTT_PUB_PACKAGE_LEN + 1];
char pub_msg[MQTT_PUB_MSG_LEN];
bool bPubOasisImage = false;

char * base64_encode( char * bindata, char * base64, int binlength )
{
	int i, j;
	unsigned char current;
	//LOGD("-- 0 %s binlength is %d\n", base64, binlength);

	for ( i = 0, j = 0 ; i < binlength ; i += 3 )
	{
		current = (bindata[i] >> 2) ;
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
		if ( i + 1 >= binlength )
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
		base64[j++] = base64char[(int)current];

		current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
		if ( i + 2 >= binlength )
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
		base64[j++] = base64char[(int)current];

		current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';
	return base64;
}

int base64_decode( const char * base64, char * bindata )
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
	{
		memset( temp, 0xFF, sizeof(temp) );
		for ( k = 0 ; k < 64 ; k ++ )
		{
			if ( base64char[k] == base64[i] )
				temp[0]= k;
		}
		for ( k = 0 ; k < 64 ; k ++ )
		{
			if ( base64char[k] == base64[i+1] )
				temp[1]= k;
		}
		for ( k = 0 ; k < 64 ; k ++ )
		{
			if ( base64char[k] == base64[i+2] )
				temp[2]= k;
		}
		for ( k = 0 ; k < 64 ; k ++ )
		{
			if ( base64char[k] == base64[i+3] )
				temp[3]= k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
			((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
		if ( base64[i+2] == '=' )
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
			((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
		if ( base64[i+3] == '=' )
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
			((unsigned char)(temp[3]&0x3F));
	}
	return j;
}

void get_extension(const char *file_name,char *extension)
{
	int i=0,length;
	length=strlen(file_name);
	while(file_name[i])
	{
		if(file_name[i]=='.')
			break;
		i++;
	}
	if(i<length)
		strcpy(extension,file_name+i+1);
	else
		strcpy(extension,"\0");
}

extern int g_priority;
int pubImage(const char* pub_topic, const char *filename, const char *msgId) {	
	//打开图片
	unsigned int size;         //图片字节数
	char *buffer;
	char *buffer1;
	size_t result;
	char *ret1; 
	unsigned int length;

	//获取图片大小
	size = fatfs_getsize(filename);
    LOGD("%s size is %d\r\n", __FUNCTION__, size);
    if(size == 0) {
    	return (-1);
    }

	//分配内存存储整个图片
	buffer = (char *)pvPortMalloc((size/4+1)*4);
	if (NULL == buffer)
	{
        LOGD("memory_error1");
        return (-1);
	}

	//将图片拷贝到buffer
	result = fatfs_read(filename, buffer, 0, size);

	//base64编码
	buffer1 = (char *)pvPortMalloc((size/3+1)*4 + 1);
	if (NULL == buffer1)
	{
        LOGD("memory_error2");
        vPortFree(buffer);
        return (-1);
	}
	ret1 = base64_encode(buffer, buffer1, size);
	length = strlen(buffer1);
	//LOGD("%d ------- %s\n", length, buffer1);
    //LOGD("%s length is %d\n", __FUNCTION__, length);

	int count = (length + MQTT_PUB_PACKAGE_LEN - 1) / MQTT_PUB_PACKAGE_LEN;
	int fresult = 0;
    bPubOasisImage = true;
	for (int i = 0; i < count; i++) {
		int len = MQTT_PUB_PACKAGE_LEN;
		if (i == (count - 1)) {
			len = length - MQTT_PUB_PACKAGE_LEN * (count - 1);
		}
		//char data[MQTT_PUB_PACKAGE_LEN + 1];
		//char pub_msg[MQTT_PUB_MSG_LEN];
		memset(data, '\0', MQTT_PUB_PACKAGE_LEN + 1);
		memset(pub_msg, '\0', MQTT_PUB_MSG_LEN);
		strncpy(data, buffer1 + (i*MQTT_PUB_PACKAGE_LEN), len);

		// sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"t\\\":%d\\,\\\"i\\\":%d\\,\\\"p\\\":%d\\,\\\"d\\\":\\\"%s\\\"}", msgId, count, i+1, 0, data);
		//sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"t\\\":%d\\,\\\"i\\\":%d\\,\\\"d\\\":\\\"%s\\\"}", msgId, count, i+1, data);
		sprintf(pub_msg, "{\"msgId\":\"%s\",\"t\":%d,\"i\":%d,\"d\":\"%s\"}", msgId, count, i+1, data);
		while (g_priority != 0) {
			//usleep(500000);	// 睡眠0.5s
			vTaskDelay(pdMS_TO_TICKS(500));
		}
        //LOGD("%s pub_topic is %s\r\n", __FUNCTION__, pub_topic);
        //LOGD("%s pub_msg is %s\r\n", __FUNCTION__, pub_msg);
		//int ret = quickPublishMQTT(pub_topic, pub_msg);
        //int ret = quickPublishOasisMQTT(pub_topic, pub_msg);
		int ret = quickPublishRawMQTT(pub_topic, pub_msg, strlen(pub_msg));

        LOGD("%s ret is %d\r\n", __FUNCTION__, ret);

        if (ret != 0) {
			fresult = -1;
			break;
		}
	}

    bPubOasisImage = false;
    vPortFree(buffer);
    vPortFree(buffer1);
	return fresult;
}

int pubOasisImage(const char* pub_topic, const char *msgId) {
    //打开图片
    unsigned int size;         //图片字节数
    char *buffer1;
    unsigned int length;

    //获取图片大小
    size = getOasisBufferSize();

    //base64编码
    buffer1 = (char *)pvPortMalloc((size/3+1)*4 + 1);
    if (NULL == buffer1)
    {
        LOGD("memory_error");
        return (-1);
    }
    base64_encode(getOasisBuffer(), buffer1, size);
    length = strlen(buffer1);
    // printf("%d ------- %s\n", length, buffer1);
    //LOGD("%d ------- %s\n", length, buffer1);

    int count = (length + MQTT_PUB_PACKAGE_LEN - 1) / MQTT_PUB_PACKAGE_LEN;
    int fresult = 0;
    bPubOasisImage = true;
    for (int i = 0; i < count; i++) {
        int len = MQTT_PUB_PACKAGE_LEN;
        if (i == (count - 1)) {
            len = length - MQTT_PUB_PACKAGE_LEN * (count - 1);
        }
        //char data[MQTT_PUB_PACKAGE_LEN + 1];
        //char pub_msg[MQTT_PUB_PACKAGE_LEN + 32];
        memset(data, '\0', MQTT_PUB_PACKAGE_LEN + 1);
        memset(pub_msg, '\0', MQTT_PUB_MSG_LEN);
        strncpy(data, buffer1 + (i*MQTT_PUB_PACKAGE_LEN), len);

        // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"t\\\":%d\\,\\\"i\\\":%d\\,\\\"p\\\":%d\\,\\\"d\\\":\\\"%s\\\"}", msgId, count, i+1, 0, data);
        //sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"t\\\":%d\\,\\\"i\\\":%d\\,\\\"d\\\":\\\"%s\\\"}", msgId, count, i+1, data);
		sprintf(pub_msg, "{\"msgId\":\"%s\",\"t\":%d,\"i\":%d,\"d\":\"%s\"}", msgId, count, i+1, data);
        LOGD("%s g_priority is %d\r\n", __FUNCTION__, g_priority);
        while (g_priority != 0) {
            //usleep(500000);	// 睡眠0.5s
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        //LOGD("%s pub_topic is %s length is %d\r\n", __FUNCTION__, pub_topic, strlen(pub_topic));
        //LOGD("%s pub_msg is %s length is %d\r\n", __FUNCTION__, pub_msg, strlen(pub_msg));
        //int ret = quickPublishOasisMQTT(pub_topic, pub_msg);
		int ret = quickPublishRawMQTT(pub_topic, pub_msg, strlen(pub_msg));

        LOGD("%s ret is %d\r\n", __FUNCTION__, ret);
        if (ret != 0) {
            fresult = -1;
            break;
        }
    }
    bPubOasisImage = false;

    vPortFree(buffer1);
    return fresult;
}
