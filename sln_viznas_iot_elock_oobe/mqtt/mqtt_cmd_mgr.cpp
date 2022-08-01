#include <string.h>
#include <stdio.h>
#include "mqtt-common.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_cmd_mgr.h"
#include "MCU_UART5_Layer.h"

char* MqttCmdMgr::genMsgId() {
    struct timeval tv;
    tv.tv_sec = ws_systime;
    tv.tv_usec = 0;
    long a = tv.tv_sec % 1000000000;
    char bt_mac_string[MSG_BT_MAC_LEN + 1];
    memset(bt_mac_string, '\0', sizeof(bt_mac_string));
    memcpy(bt_mac_string, btWifiConfig.bt_mac + 2, MSG_BT_MAC_LEN);
    if (++random_gen >= 10) {
        random_gen = 1;
    }
    sprintf(m_msg_id, "%s%09d%d", bt_mac_string, a, random_gen);
    return m_msg_id;
}
