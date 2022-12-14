#include <stdio.h>
#include "config.h"
#include "util.h"
#include "string.h"
//#include "log.h"
#include "fsl_log.h"

#include <fcntl.h>
#include <unistd.h>
#include "mqtt_util.h"
#include "cJSON.h"
#include "fatfs_op.h"
#include "board_rt106f_elock.h"

#define CONFIG_USE_FATFS 0
//
#define CONFIG_FS_ADDR  (0xF00000U)


VERSIONCONFIG versionConfig;
BTWIFICONFIG btWifiConfig;
MQTTCONFIG mqttConfig;

static char buf[4096] = {0};
//char buf2[1024] = {0};
static bool config_json_changed = false;

void init_config() {
    cJSON_Hooks hooks;

    /* Initialize cJSON library to use FreeRTOS heap memory management. */
    hooks.malloc_fn = pvPortMalloc;
    hooks.free_fn   = vPortFree;
    cJSON_InitHooks(&hooks);

    //fatfs_mount_with_mkfs();

#if  CONFIG_USE_FATFS
    int status = fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf));
    LOGD("read fatfs, buf length is %d, size is %d, status is %d\r\n", strlen(buf), sizeof(buf), status);
#else
    int status = SLN_Read_Flash_At_Address(CONFIG_FS_ADDR, buf, sizeof(buf));
    LOGD( "read flash %x , buf length is %d, size is %d , status id %d \r\n", CONFIG_FS_ADDR, strlen(buf), sizeof(buf), status );
#endif
    {
        cJSON *root = NULL;
        root = cJSON_CreateObject();
        if(root != NULL) {
            cJSON_AddItemToObject(root, CONFIG_KEY_SYS_VERSION, cJSON_CreateString((const char *)FIRMWARE_VERSION));
            cJSON_AddItemToObject(root, CONFIG_KEY_MCU_VERSION, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_OASIS_VERSION, cJSON_CreateString((const char *)PROJECT_VERSION));

            cJSON_AddItemToObject(root, CONFIG_KEY_BT_VERSION, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_BT_MAC, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_RESET, cJSON_CreateString((const char *)"true"));
            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_RECONNECT, cJSON_CreateString((const char *)"true"));
            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_MAC, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_SSID, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_PWD, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_IP, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_PORT, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_URL, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_USER, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_PASSWORD, cJSON_CreateString((const char *)""));

            char* tmp = NULL;

            tmp = cJSON_Print(root);
#if CONFIG_USE_FATFS
            cJSON *bufRoot = NULL;
            bufRoot = cJSON_Parse(buf);
            if( bufRoot ==NULL ){
				memset(buf, 0, sizeof(buf));
				memcpy(buf, tmp, strlen(tmp));
	            LOGD("%s tmp is %s\r\n", __FUNCTION__, tmp);

	            fatfs_write(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf));
            }

#else
            cJSON *bufRoot = NULL;
            bufRoot = cJSON_Parse(buf);
            if( bufRoot == NULL ) {
                memset(buf, 0, sizeof(buf));
                memcpy(buf, tmp, strlen(tmp));
                int status = SLN_Write_Sector(CONFIG_FS_ADDR, buf);
                if (status != 0) {
                    LOGD("write flash failed %d \r\n", status);
                }
            }
#endif
            LOGD("buf %s \r\n", buf);
            if(tmp != NULL) {
                vPortFree(tmp);
            }

            cJSON_Delete(root);
        }
    }

    read_config();

}

void check_config() {
#if	0
	if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) == -1) {
		LOGD("%s return \n", __FUNCTION__);
		init_config();
	}
#endif
	read_config();
}

int update_section_key(char *section, char *key) {
	LOGD("%s section:%s key:%s \r\n", __FUNCTION__, section, key);
	cJSON *root = NULL;
	//LOGD("buf is %s\n", buf);


	root = cJSON_Parse(buf);
	if(root != NULL) {
        cJSON *item = cJSON_GetObjectItem(root, section);
        if(item != NULL) {
            char *item_string;
            item_string = cJSON_GetStringValue(item);
            if( strcmp( item_string, key)==0 ){
                cJSON_Delete(root);
                return 0;
            }
        }

		cJSON_ReplaceItemInObject(root, section, cJSON_CreateString((const char *)key));
        char *tmp = NULL;
        tmp = cJSON_Print(root);
		memset(buf, 0, sizeof(buf));
        memcpy(buf, tmp, strlen(tmp));

        if(tmp != NULL) {
            vPortFree(tmp);
        }
		//fatfs_write(DEFAULT_CONFIG_FILE, buf2, 0, strlen(buf2));

		cJSON_Delete(root);
        config_json_changed = true;
	}else{
	    LOGD("ERROR  root  is NULL \r\n");
	}

	return 0;
}

int update_mac(char *mac) {
	//LOGD("%s\n", __FUNCTION__);
	
	//update_section_key(CONFIG_KEY_WIFI_MAC, mac);

	return 0;
}

int update_need_reset(char *need_reset) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_WIFI_RESET, need_reset);

	return 0;
}

int update_need_reconnect(char *need_reconnect) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_WIFI_RECONNECT, need_reconnect);

	return 0;
}


int update_wifi_ssid(char *ssid) {
//	LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_SSID, ssid);
	strcpy(btWifiConfig.ssid, ssid);
    save_json_config_file();

	return 0;
}

int update_wifi_pwd(char *password) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_WIFI_PWD, password);
    strcpy(btWifiConfig.password, password);
    save_json_config_file();

	return 0;
}

int update_mqtt_username(char *username) {
    update_section_key(CONFIG_KEY_MQTT_USER, username);
    memcpy(mqttConfig.username, username, strlen(username));

    save_json_config_file();
    return 0;
}

int update_mqtt_password(char *password) {
    update_section_key(CONFIG_KEY_MQTT_PASSWORD, password);
    memcpy(mqttConfig.password, password, strlen(password));

    save_json_config_file();
    return 0;
}
int update_mqtt_ip(char *ip) {
//    update_section_key(CONFIG_KEY_MQTT_SERVER_IP, ip);
//    strcpy(mqttConfig.server_ip, ip);
//    save_json_config_file();
    char serverUrl[60] = {0};
    sprintf(serverUrl, "%s:%s", ip, mqttConfig.server_port);
    update_MqttSvr_opt(serverUrl);

    return 0;
}
int update_mqtt_port(char *port) {
    char serverUrl[60] = {0};
    sprintf(serverUrl, "%s:%s", mqttConfig.server_ip, port);
    update_MqttSvr_opt(serverUrl);
//    update_section_key(CONFIG_KEY_MQTT_SERVER_PORT, port);
//    strcpy(mqttConfig.server_port, port);
//
//    save_json_config_file();
    return 0;
}
int update_mqtt_id(char *id) {
    strcpy(mqttConfig.client_id, id);
    return 0;
}

int update_mqtt_opt(char *username, char *password)
{
    //LOGD("%s\n", __FUNCTION__);

    update_section_key(CONFIG_KEY_MQTT_USER, username);
    update_section_key(CONFIG_KEY_MQTT_PASSWORD, password);
    memcpy(mqttConfig.username, username, strlen(username));
    memcpy(mqttConfig.password, password, strlen(password));

    save_json_config_file();

    return 0;
}

int update_MqttSvr_opt(char *MqttSvrUrl)
{
    if (MqttSvrUrl == NULL || strchr(MqttSvrUrl, ':') == NULL) {
        LOGD("Failed to save MqttSvrUrl %s\n", MqttSvrUrl);
        return -1;
    }

    update_section_key(CONFIG_KEY_MQTT_SERVER_URL, MqttSvrUrl);

    mysplit(MqttSvrUrl, mqttConfig.server_ip, mqttConfig.server_port, ":");
    LOGD("%s server_ip is %s, server_port is %s\r\n", __FUNCTION__, mqttConfig.server_ip, mqttConfig.server_port);
    update_section_key(CONFIG_KEY_MQTT_SERVER_IP, mqttConfig.server_ip);
    update_section_key(CONFIG_KEY_MQTT_SERVER_PORT, mqttConfig.server_port);

    save_json_config_file();

    return 0;
}

int update_bt_info(char *version, char *mac)
{
	//LOGD("%s\n", __FUNCTION__);

	update_section_key(CONFIG_KEY_BT_VERSION, version);
	update_section_key(CONFIG_KEY_BT_MAC, mac);
    memcpy(mqttConfig.client_id, mac, strlen(mac));
    memcpy(btWifiConfig.bt_mac, mac, strlen(mac));
    //memcpy(mqttConfig.username, mac, strlen(mac));

	return 0;
}

int update_mcu_info(char *version)
{
	//LOGD("%s\n", __FUNCTION__);

	update_section_key(CONFIG_KEY_MCU_VERSION, version);

	return 0;
}

int update_sys_info(char *version)
{
	//LOGD("%s\n", __FUNCTION__);

	update_section_key(CONFIG_KEY_SYS_VERSION, version);

    return 0;
}


int update_project_info(char *version)
{
	//LOGD("%s\n", __FUNCTION__);

	update_section_key(CONFIG_KEY_OASIS_VERSION, version);

	return 0;
}

int read_config_value(char *dst, char *key) {
	cJSON *root = NULL;
    //LOGD("buf is %s\n", buf);
    root = cJSON_Parse(buf);

    if(root != NULL) {
        cJSON *item = cJSON_GetObjectItem(root, key);
        if(item != NULL) {
            char *item_string;
            item_string = cJSON_GetStringValue(item);

            //LOGD("key is %s, item_string is %s\r\n", key, item_string);
            if(item_string != NULL) {
                memcpy(dst, item_string, strlen(item_string));
            }
        }

        cJSON_Delete(root);
    }else{
        LOGD("Error root is NULL \r\n");
    }
    return 0;
}

int read_config() {
    LOGD("read_config \r\n");
    memset(&versionConfig, '\0', sizeof(VERSIONCONFIG));
    memset(&btWifiConfig, '\0', sizeof(BTWIFICONFIG));
    memset(&mqttConfig, '\0', sizeof(MQTTCONFIG));

    memset(buf, 0, sizeof(buf));
#if  CONFIG_USE_FATFS
    if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) != 0) {
        LOGD("%s read error\r\n", __FUNCTION__);
        return -1;
    }
#else
    int status = SLN_Read_Flash_At_Address(CONFIG_FS_ADDR, buf, sizeof(buf));
    if( status != 0 ){
        LOGD("%s read error\r\n", __FUNCTION__);
        return -1;
    }
#endif
    //LOGD("buf is %s\r\n", buf);
    LOGD("buf %s  buf length is %d, size is %d\r\n",buf,  strlen(buf), sizeof(buf));

    LOGD("========= reading config =========\r\n");
    read_config_value(versionConfig.sys_ver, CONFIG_KEY_SYS_VERSION);
    read_config_value(versionConfig.bt_ver, CONFIG_KEY_BT_VERSION);
    read_config_value(versionConfig.oasis_ver, CONFIG_KEY_OASIS_VERSION);
    read_config_value(versionConfig.mcu_ver, CONFIG_KEY_MCU_VERSION);

    read_config_value(btWifiConfig.need_reset, CONFIG_KEY_WIFI_RESET);
    read_config_value(btWifiConfig.need_reconnect, CONFIG_KEY_WIFI_RECONNECT);
    read_config_value(btWifiConfig.bt_mac, CONFIG_KEY_BT_MAC);
    read_config_value(btWifiConfig.wifi_mac, CONFIG_KEY_WIFI_MAC);
    read_config_value(btWifiConfig.ssid, CONFIG_KEY_SSID);
    read_config_value(btWifiConfig.password, CONFIG_KEY_WIFI_PWD);

    char server_url[CONFIG_MQTT_ITEM_LEN * 2 + 1];
    memset(server_url, '\0', sizeof(server_url));
    read_config_value(server_url, CONFIG_KEY_MQTT_SERVER_URL);
    mysplit(server_url, mqttConfig.server_ip, mqttConfig.server_port, ":");

    read_config_value(mqttConfig.client_id, CONFIG_KEY_BT_MAC);
    read_config_value(mqttConfig.username, CONFIG_KEY_MQTT_USER);
    read_config_value(mqttConfig.password, CONFIG_KEY_MQTT_PASSWORD);

    print_project_config();

    return 0;
}

void print_project_config(void) {
    LOGD("version config:\r\n");
    LOGD("sys_ver: %s\r\n", versionConfig.sys_ver);
    LOGD("bt_ver: %s\r\n", versionConfig.bt_ver);
    LOGD("oasis_ver: %s\r\n", versionConfig.oasis_ver);
    LOGD("mcu_ver: %s\r\n", versionConfig.mcu_ver);

    LOGD("bt wifi config:\r\n");
    LOGD("reset: %s\r\n", btWifiConfig.need_reset);
    LOGD("reconnect: %s\r\n", btWifiConfig.need_reconnect);
    LOGD("bt_mac: %s\r\n", btWifiConfig.bt_mac);
    LOGD("wifi_mac: %s\r\n", btWifiConfig.wifi_mac);
    LOGD("ssid: %s\r\n", btWifiConfig.ssid);
    LOGD("wifi password: %s\r\n", btWifiConfig.password);

    LOGD("mqtt config:\r\n");
    LOGD("server_ip: %s\r\n", mqttConfig.server_ip);
    LOGD("server_port: %s\r\n", mqttConfig.server_port);
    LOGD("client_id: %s\r\n", mqttConfig.client_id);
    LOGD("username: %s\r\n", mqttConfig.username);
    LOGD("mqtt password: %s\r\n", mqttConfig.password);
}

int save_json_config_file() {
    LOGD("?????? config ?????? %d \r\n", config_json_changed);
    if (config_json_changed) {
#if CONFIG_USE_FATFS
        fatfs_write(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf));
        LOGD("write fatfs %s \r\n", buf);

#else
        int status = SLN_Write_Sector(CONFIG_FS_ADDR, buf);
        if (status != 0)
        {
            LOGD("write flash %x %s failed %d \r\n",CONFIG_FS_ADDR, buf, status);
        }
#endif
        config_json_changed = false;

    }else{
        LOGD("config ??????????????????  \r\n");
    }
    LOGD("??????config ???????????? \r\n");
    return 0;
}

bool clear_json_config_file(){
    LOGD("?????? config ?????? %d \r\n");
    int status = SLN_Erase_Sector( CONFIG_FS_ADDR );
    if( status !=0 ){
        LOGD("erase flash failed %d \r\n", status);
        return  false;
    }else{
        LOGD("config ???????????? \r\n" );
    }
    return true;
}

