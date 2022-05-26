//
// Created by xshx on 2022/5/24.
//

#ifndef W8_UART_FAKER_H
#define W8_UART_FAKER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "WAVE_COMMON.h"
typedef enum {
    MCU_READY = 0,		/* (0) 准备模式 */
    MCU_WORK,            /*(1) 工作模式*/
    MCU_FREE,			/* (2) 空闲模式 */

} MCU_STATUS;
int uFakeUartTaskStart(  );
extern void vSetFakeCommandIndex( int index );
extern void vSetFakeCommandBuffer(char *buf);

#ifdef __cplusplus
}
#endif

#endif //W8_UART_FAKER_H
