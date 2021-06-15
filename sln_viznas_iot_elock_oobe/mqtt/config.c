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

static char buf[1024] = {0};
//char buf2[1024] = {0};

#if SUPPORT_CONFIG_JSON != 0
void init_config() {
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

		char* buf = NULL;

		buf = cJSON_Print(root);
		//LOGD("%s buf is %s\n", __FUNCTION__, buf);

		fatfs_write(DEFAULT_CONFIG_FILE, buf, 0, strlen(buf));

		cJSON_Delete(root);
	}
}

void check_config() {
	if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) == -1) {
		LOGD("%s return \n", __FUNCTION__);
		init_config();
	}
}

int update_section_key(char *section, char *key) {
	//LOGD("%s\n", __FUNCTION__);
	cJSON *root = NULL;
	char* buf2 = NULL;
	memset(buf, 0, sizeof(buf));

	if(fatfs_read(DEFAULT_CONFIG_FILE, buf, 0, sizeof(buf)) == -1) {
		LOGD("%s return \n", __FUNCTION__);
		init_config();
		//return -1;
	}
	//LOGD("buf is %s\n", buf);


	root = cJSON_Parse(buf);
	//root = cJSON_CreateObject();
	if(root != NULL) {
		cJSON_ReplaceItemInObject(root, section, cJSON_CreateString((const char *)key));

		buf2 = cJSON_Print(root);
		//LOGD("%s buf2 is %s\n", __FUNCTION__, buf2);

		fatfs_write(DEFAULT_CONFIG_FILE, buf2, 0, strlen(buf2));

		cJSON_Delete(root);
	}

	return 0;
}

int update_mac(char *file, char *mac) {
	//LOGD("%s\n", __FUNCTION__);
	
	//update_section_key(CONFIG_KEY_WIFI_MAC, mac);

	return 0;
}

int update_wifi_ssid(char *file, char *ssid) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_SSID, ssid);

	return 0;
}

int update_wifi_pwd(char *file, char *password) {
	//LOGD("%s\n", __FUNCTION__);
	
	update_section_key(CONFIG_KEY_WIFI_PWD, password);

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
