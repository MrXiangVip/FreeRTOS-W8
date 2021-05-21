#ifndef _MQTTUTIL_H_
#define _MQTTUTIL_H_


#ifdef __cplusplus
extern "C" {
#endif
int mysplit(char *src, char *first, char *last, char *separator);
int mysplit2(char *src, char *first, char *separator);
int del_char(char *src, char sep);
#ifdef __cplusplus
}
#endif

#endif
