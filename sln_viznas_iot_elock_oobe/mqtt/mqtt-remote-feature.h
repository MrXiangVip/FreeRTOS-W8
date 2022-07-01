#ifndef _MQTT_REMOTE_FEATURE_H_
#define _MQTT_REMOTE_FEATURE_H_

#ifdef __cplusplus
extern "C" {
#endif
int analyzeRemoteFeature(char *data, char *msgId);
int requestFeatureUpload(char *data, char *msgId);
#ifdef __cplusplus
}
#endif

#endif
