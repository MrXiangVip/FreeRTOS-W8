#ifndef _MQTTUTIL_H_
#define _MQTTUTIL_H_


#ifdef __cplusplus
extern "C" {
#endif
void freePointer(char **p); 
int mysplit(char *src, char *first, char *last, char *separator);
int mysplit2(char *src, char *first, char *separator);
int del_char(char *src, char sep);
char* get_short_str(const char *str);
char *gen_msgId();
void Remote_convertInt2ascii(void *value, int bytes, unsigned char *ascii);
#ifdef __cplusplus
}
#endif

#endif
