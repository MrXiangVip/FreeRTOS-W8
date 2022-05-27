  /*
 * util_crc16.h
 */
#ifndef _LINUX_CRC16_H
#define _LINUX_CRC16_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

uint16_t crc16_ccitt(const void *buf, int len);

uint16_t crc16_ccitt_continue(uint16_t init_crc,const void *buf, int len);

unsigned short CRC16_X25(unsigned char *puchMsg, unsigned int usDataLen);

//累加校验
bool  bCheckSum(uint8_t *p_Buf,uint16_t  uBufLen);
//
uint8_t  uGetCheckSum(uint8_t *pBuff, uint16_t uBuffLen );

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_CRC16_H */
