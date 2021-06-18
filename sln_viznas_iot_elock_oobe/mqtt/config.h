#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "config-cnf.h"

#define	SUPPORT_CONFIG_JSON		1
#if SUPPORT_CONFIG_JSON != 0
#define DEFAULT_CONFIG_FILE "config.json"
#else
//#define DEFAULT_CONFIG_FILE "/opt/smartlocker/config.ini"
#define DEFAULT_CONFIG_FILE "config.ini"
#endif

#define CONFIG_KEY_SYS_VERSION		"SYS"
#define CONFIG_KEY_MCU_VERSION		"MCU"
#define CONFIG_KEY_OASIS_VERSION	"OASIS"
#define CONFIG_KEY_BT_VERSION		"BT_VERSION"

#define CONFIG_KEY_BT_MAC			"BT_MAC"
#define CONFIG_KEY_WIFI_MAC			"WIFI_MAC"
#define CONFIG_KEY_SSID				"SSID"
#define CONFIG_KEY_WIFI_PWD			"PASSWORD"

#define CONFIG_KEY_MQTT_USER		    "MQTT_USER"
#define CONFIG_KEY_MQTT_PASSWORD		"MQTT_PASSWORD"
#define CONFIG_KEY_MQTT_SERVER_URL		"MQTT_URL"
#define CONFIG_KEY_MQTT_SERVER_IP		"MQTT_IP"
#define CONFIG_KEY_MQTT_SERVER_PORT		"MQTT_PORT"

#define MAC_LEN 20
#define CONFIG_ITEM_LEN 128
#define CONFIG_VERSION_ITEM_LEN         32
#define CONFIG_BT_WIFI_SSID_LEN         32
#define CONFIG_BT_WIFI_PASSWORD_LEN     32
#define CONFIG_MQTT_ITEM_LEN            32
#define DEFAULT_MAC "FFFFFFFFFFFF"

typedef struct version_config {
	char sys_ver[CONFIG_VERSION_ITEM_LEN];
	char bt_ver[CONFIG_VERSION_ITEM_LEN];
	char oasis_ver[CONFIG_VERSION_ITEM_LEN];
	char mcu_ver[CONFIG_VERSION_ITEM_LEN];
} VERSIONCONFIG;

typedef struct btwifi_config {
	char bt_mac[MAC_LEN];
	char wifi_mac[MAC_LEN];

	char ssid[CONFIG_BT_WIFI_SSID_LEN];
	char password[CONFIG_BT_WIFI_PASSWORD_LEN];
} BTWIFICONFIG;

typedef struct mqtt_config {
	char server_ip[CONFIG_MQTT_ITEM_LEN];
	char server_port[CONFIG_MQTT_ITEM_LEN];
	char client_id[CONFIG_MQTT_ITEM_LEN];
	char username[CONFIG_ITEM_LEN];
	char password[CONFIG_ITEM_LEN];
} MQTTCONFIG;

extern VERSIONCONFIG versionConfig;
extern BTWIFICONFIG btWifiConfig;
extern MQTTCONFIG mqttConfig;

#ifdef __cplusplus
extern "C"  {
#endif

	void init_config();
	void check_config();
	int read_config();
	int read_config_value(char *dst, char *key);
	int read_default_config_from_file(char *file, char *dst, char *section, char *key, char *default_value);
	int read_config_value_default(char *dst, Config* cnf, char *section, char *key, char *default_value) ;
	int update_username_from_mac(char *file, char *mac);
	int update_mac(char *mac);
	void print_project_config(void);
	// int write_config(const char *file, const char *entry, const char *value);
	int write_config(char *file, char *section, char *key, char *value);
	
	int update_wifi_ssid(char *ssid) ;
	int update_wifi_pwd(char *password) ;
	int update_mqtt_opt(char *username, char *password) ;
	int update_bt_info(char *version, char *mac) ;
	int update_MqttSvr_opt(char *MqttSvrUrl) ;
	int update_mcu_info(char *version) ;
	int update_project_info(char *version) ;
    int update_sys_info(char *version);
	int update_NetworkOptVer_info(char *file, char *version) ;//???????????????ð汾??

#ifdef __cplusplus
}
#endif

#endif // _CONFIG_H_
