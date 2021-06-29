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

VERSIONCONFIG versionConfig;
BTWIFICONFIG btWifiConfig;
MQTTCONFIG mqttConfig;

static char buf[512] = {0};
//char buf2[1024] = {0};
static bool config_json_changed = false;

#if SUPPORT_CONFIG_JSON != 0
void init_config() {
    cJSON_Hooks hooks;

    /* Initialize cJSON library to use FreeRTOS heap memory management. */
    hooks.malloc_fn = pvPortMalloc;
    hooks.free_fn   = vPortFree;
    cJSON_InitHooks(&hooks);

    if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) !=0) 
    {
        cJSON *root = NULL;
        root = cJSON_CreateObject();
        if(root != NULL) {
            cJSON_AddItemToObject(root, CONFIG_KEY_SYS_VERSION, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_MCU_VERSION, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_OASIS_VERSION, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_BT_VERSION, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_BT_MAC, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_MAC, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_SSID, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_WIFI_PWD, cJSON_CreateString((const char *)""));

            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_IP, cJSON_CreateString((const char *)"10.0.14.61"));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_PORT, cJSON_CreateString((const char *)"9883"));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_SERVER_URL, cJSON_CreateString((const char *)"10.0.14.61:9883"));

            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_USER, cJSON_CreateString((const char *)""));
            cJSON_AddItemToObject(root, CONFIG_KEY_MQTT_PASSWORD, cJSON_CreateString((const char *)""));

            char* tmp = NULL;

            tmp = cJSON_Print(root);
            memset(buf, 0, sizeof(buf));
            memcpy(buf, tmp, strlen(tmp));
            LOGD("%s tmp is %s\r\n", __FUNCTION__, tmp);
            if(tmp != NULL) {
                vPortFree(tmp);
            }

            cJSON_Delete(root);
            fatfs_write(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf));
        }
    }
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
	//LOGD("%s\n", __FUNCTION__);
	cJSON *root = NULL;
	//LOGD("buf is %s\n", buf);


	root = cJSON_Parse(buf);
	if(root != NULL) {
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
	}

	return 0;
}

int update_mac(char *mac) {
	//LOGD("%s\n", __FUNCTION__);
	
	//update_section_key(CONFIG_KEY_WIFI_MAC, mac);

	return 0;
}

int update_wifi_ssid(char *ssid) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_SSID, ssid);
    save_json_config_file();

	return 0;
}

int update_wifi_pwd(char *password) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_WIFI_PWD, password);
    save_json_config_file();

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
    memcpy(mqttConfig.username, mac, strlen(mac));

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
    }
    return 0;
}

int read_config() {
    memset(&versionConfig, '\0', sizeof(VERSIONCONFIG));
    memset(&btWifiConfig, '\0', sizeof(BTWIFICONFIG));
    memset(&mqttConfig, '\0', sizeof(MQTTCONFIG));

    memset(buf, 0, sizeof(buf));

    if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) != 0) {
        LOGD("%s read error\r\n", __FUNCTION__);
        return -1;
    }
    LOGD("buf is %s\r\n", buf);
    LOGD("buf length is %d, size is %d\r\n", strlen(buf), sizeof(buf));

    LOGD("========= reading config =========\r\n");
    read_config_value(versionConfig.sys_ver, CONFIG_KEY_SYS_VERSION);
    read_config_value(versionConfig.bt_ver, CONFIG_KEY_BT_VERSION);
    read_config_value(versionConfig.oasis_ver, CONFIG_KEY_OASIS_VERSION);
    read_config_value(versionConfig.mcu_ver, CONFIG_KEY_MCU_VERSION);

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
    if (config_json_changed) {
        fatfs_write(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf));
        config_json_changed = false;
    }
    return 0;
}

#else
int update_option_if_not_exist(Config *cnf, char *section, char *key, char *value) {
	if (!cnf_has_option(cnf, section, key)) {
		return cnf_add_option(cnf, section, key, value);
	}
	return -1;
}

int update_topic_if_not_exist(Config *cnf, char *topic_name, char *section, char *key, char *mac) {
	char topic[CONFIG_ITEM_LEN];
	memset(topic, '\0', CONFIG_ITEM_LEN);
	sprintf(topic, topic_name, mac);
	return update_option_if_not_exist(cnf, section, key, topic);
}

int update_topic_overwrite(Config *cnf, char *topic_name, char *section, char *key, char *mac) {
	char topic[CONFIG_ITEM_LEN];
	memset(topic, '\0', CONFIG_ITEM_LEN);
	sprintf(topic, topic_name, mac);
	return cnf_add_option(cnf, section, key, topic);
}

int update_username_from_mac(char *file, char *mac) {
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}
	printf("------ change user name to %s", mac);
	cnf_add_option(cnf, "mqtt", "username", mac);
	read_config_value(mqttConfig.username, cnf, "mqtt", "username");
	printf("------ user name is %s", mqttConfig.username);
	cnf_write_file(cnf, file, "username from mac updated");
  	destroy_config(&cnf); // 销毁Config对象

	return 0;
}

int update_mac(char *file, char *mac) {
	LOGD("%s\n", __FUNCTION__);
	Config *cnf = cnf_read_config(file, '#', '=');
	LOGD("%s 1\n", __FUNCTION__);
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	LOGD("%s 2\n", __FUNCTION__);
	cnf_add_option(cnf, "wifi", "wifi_mac", mac);
	// cnf_add_option(cnf, "mqtt", "client_id", mac);

	LOGD("%s 3\n", __FUNCTION__);
	cnf_write_file(cnf, file, "mac updated");
	LOGD("%s 4\n", __FUNCTION__);
  	destroy_config(&cnf); // 销毁Config对象

	LOGD("%s 5\n", __FUNCTION__);

	//read_config(file);
	return 0;
}

int update_wifi_ssid(char *file, char *ssid) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "wifi", "ssid", ssid);//???Ƿ?ʽ????
	read_config_value(btWifiConfig.ssid, cnf, "wifi", "ssid");

	cnf_write_file(cnf, file, "mac updated");
  	destroy_config(&cnf); // 销毁Config对象

	// read_config(file);
	return 0;
}

int update_wifi_pwd(char *file, char *password) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "wifi", "password", password);//???Ƿ?ʽ????
	read_config_value(btWifiConfig.password, cnf, "wifi", "password");

	cnf_write_file(cnf, file, "mac updated");
	destroy_config(&cnf); // 销毁Config对象

	// read_config(file);
	return 0;
}

int update_mqtt_opt(char *file, char *username, char *password) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "mqtt", "username", username);//???Ƿ?ʽ????
	cnf_add_option(cnf, "mqtt", "password", password);//???Ƿ?ʽ????
	read_config_value(mqttConfig.username, cnf, "mqtt", "username");
	read_config_value(mqttConfig.password, cnf, "mqtt", "password");

	cnf_write_file(cnf, file, "mac updated");
	destroy_config(&cnf); // 销毁Config对象

	// read_config(file);
	return 0;
}

int update_MqttSvr_opt(char *file, char *MqttSvrUrl) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	if (MqttSvrUrl == NULL || strchr(MqttSvrUrl, ':') == NULL) {
		printf("Failed to save MqttSvrUrl %s\n", MqttSvrUrl);
		return -1;
	}

	cnf_add_option(cnf, "mqtt", "server_url", MqttSvrUrl);//???Ƿ?ʽ????

	cnf_write_file(cnf, file, "MqttSvrUrl updated");
	destroy_config(&cnf); // 销毁Config对象

	//read_config(file);
	return 0;
}

int update_bt_info(char *file, char *version, char *mac) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "bt", "version", version);
	cnf_add_option(cnf, "bt", "bt_mac", mac);
	// 使用bt_mac作为mqtt的client_id
	cnf_add_option(cnf, "mqtt", "client_id", mac);

	cnf_write_file(cnf, file, "bt info updated");
	destroy_config(&cnf); // 销毁Config对象

	//read_config(file);
	return 0;
}

int update_mcu_info(char *file, char *version) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "mcu", "version", version);//???Ƿ?ʽ????

	cnf_write_file(cnf, file, "mcu info updated");
	destroy_config(&cnf); // 销毁Config对象

	//read_config(file);
	return 0;
}

int update_project_info(char *file, char *version) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	cnf_add_option(cnf, "oasis", "version", version);//???Ƿ?ʽ????

	cnf_write_file(cnf, file, "mcu info updated");
	destroy_config(&cnf); // 销毁Config对象

	//read_config(file);
	return 0;
}

int update_NetworkOptVer_info(char *file, char *NetworkOptVer) 
{
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; 
	}

	cnf_add_option(cnf, "sys", "NetworkOptVer", NetworkOptVer);

	cnf_write_file(cnf, file, "NetworkOptVer updated");
	destroy_config(&cnf); 

	//read_config(file);
	return 0;
}

int write_config(char *file, char *section, char *key, char *value) {
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}
	cnf_add_option(cnf, section, key, value);
	cnf_write_file(cnf, file, "");
  	destroy_config(&cnf); // 销毁Config对象
	return 0;
}

int read_default_config_from_file(char *file, char *dst, char *section, char *key, char *default_value) {
	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}
	int res = read_config_value_default(dst, cnf, section, key, default_value);
	destroy_config(&cnf);
	return res;
}

int read_config_value_default(char *dst, Config* cnf, char *section, char *key, char *default_value) {
	bool res = cnf_get_value(cnf, section, key);
	if (res && !str_empty(cnf->re_string)) {
		strcpy(dst, cnf->re_string);
		return 0;
	} else if (!str_empty(default_value)) {
		cnf_add_option(cnf, section, key, default_value);
		strcpy(dst, default_value);
		return 0;
	}
	return -1;
}

int read_config_value(char *dst, Config* cnf, char *section, char *key) {
	return read_config_value_default(dst, cnf, section, key, "");
}

int read_config(char *file) {
	memset(&versionConfig, '\0', sizeof(VERSIONCONFIG));
	memset(&btWifiConfig, '\0', sizeof(BTWIFICONFIG));
	memset(&mqttConfig, '\0', sizeof(MQTTCONFIG));

	Config *cnf = cnf_read_config(file, '#', '=');
	if (NULL == cnf) {
		return -1; /* 创建对象失败 */
	}

	printf("========= reading config =========\n");
	print_config(cnf);
	int	ret = 0;
	ret = read_config_value(versionConfig.sys_ver, cnf, "sys", "version");
	ret = read_config_value(versionConfig.bt_ver, cnf, "bt", "version");
	ret = read_config_value(versionConfig.oasis_ver, cnf, "oasis", "version");
	ret = read_config_value(versionConfig.mcu_ver, cnf, "mcu", "version");

	ret = read_config_value(btWifiConfig.bt_mac, cnf, "bt", "bt_mac");
	ret = read_config_value(btWifiConfig.wifi_mac, cnf, "wifi", "wifi_mac");
	ret = read_config_value(btWifiConfig.ssid, cnf, "wifi", "ssid");
	ret = read_config_value(btWifiConfig.password, cnf, "wifi", "password");

	ret = read_config_value_default(mqttConfig.need_reset, cnf, "mqtt", "need_reset", "true");
	ret = read_config_value_default(mqttConfig.need_log_error, cnf, "mqtt", "need_log_error", "true");

	char server_url[CONFIG_ITEM_LEN];
	memset(server_url, '\0', CONFIG_ITEM_LEN);
	ret = read_config_value(server_url, cnf, "mqtt", "server_url");
	mysplit(server_url, mqttConfig.server_ip, mqttConfig.server_port, ":");

	ret = read_config_value_default(mqttConfig.client_id, cnf, "mqtt", "client_id", btWifiConfig.bt_mac);
	ret = read_config_value(mqttConfig.username, cnf, "mqtt", "username");
	ret = read_config_value(mqttConfig.password, cnf, "mqtt", "password");

	print_project_config();

  	destroy_config(&cnf); // 销毁Config对象

	return 0;
}
	
void print_project_config(void) {
	printf("version config:\n");
	printf("\tsys_ver: \t%s\n", versionConfig.sys_ver);
	printf("\tbt_ver: \t%s\n", versionConfig.bt_ver);
	printf("\toasis_ver: \t%s\n", versionConfig.oasis_ver);
	printf("\tmcu_ver: \t%s\n", versionConfig.mcu_ver);

	printf("bt wifi config:\n");
	printf("\tbt_mac: \t%s\n", btWifiConfig.bt_mac);
	printf("\twifi_mac: \t%s\n", btWifiConfig.wifi_mac);
	printf("\tssid: \t\t%s\n", btWifiConfig.ssid);
	printf("\tpassword: \t%s\n", btWifiConfig.password);

	printf("mqtt config:\n");
	printf("\tneed_reset: \t%s\n", mqttConfig.need_reset);
	printf("\tneed_log_error: %s\n", mqttConfig.need_log_error);
	printf("\tserver_ip: \t%s\n", mqttConfig.server_ip);
	printf("\tserver_port: \t%s\n", mqttConfig.server_port);
	printf("\tclient_ip: \t%s\n", mqttConfig.client_id);
	printf("\tusername: \t%s\n", mqttConfig.username);
	printf("\tpassword: \t%s\n", mqttConfig.password);
}
#endif
