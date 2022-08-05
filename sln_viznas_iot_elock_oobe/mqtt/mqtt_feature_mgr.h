//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_FEATURE_MGR_H_
#define _MQTT_FEATURE_MGR_H_

#include "mqtt-instruction-pool.h"

class MqttFeatureMgr {
private:
    MqttFeatureMgr() {};
    int saveFeature(char *uuid, char *payload_bin, int payload_bin_len);
    int verifyFeature(char *uuid, unsigned char *payload, char *sign, int length);
public:
    static MqttFeatureMgr *getInstance() {
        static MqttFeatureMgr m_instance;
        return &m_instance;
    };

    // Feature Entry
    int downloadFeature(char *data, char *msgId);
    int uploadFeature(char *msgId, char *uuid);
    int requestFeature(char *data, char *msgId);
};

#endif //_MQTT_FEATURE_MGR_H_
