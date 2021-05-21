/*
 * Copyright 2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SHELL_UTILS_H
#define _SHELL_UTILS_H

/*!
\file       shell_utils.h
\brief      This is a header file for the SHELL Utils module.
*/
/*==================================================================================================
Include Files
==================================================================================================*/
#include "fsl_debug_console.h"
#include "fsl_shell.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#define NEWLINE                 "\r\n"
#define shell_printf            PRINTF
#define SHELL_NEWLINE()         shell_writeN(NEWLINE, 2)
#define SHELL_CMD_MAX_ARGS      (20U)
#define SHELL_ETX               "\x03"      /* ASCII end of text, used for Ctrl-C */

/*==================================================================================================
Public type definitions
==================================================================================================*/
typedef enum
{
    CMD_RET_SUCCESS  = 0,       /* 0 = Success */
    CMD_RET_FAILURE  = 1,       /* 1 = Failure */
    CMD_RET_ASYNC    = 2,       /* 2 = Success, do not refresh the prompt */
    CMD_RET_USAGE    = -1,      /* Failure, please report 'usage' error */
} command_ret_t;

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public function prototypes
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

void shell_refresh(void);
void shell_change_prompt(char *prompt);
void shell_writeN(char *pBuff, uint16_t n);
void shell_write(char *pBuff);
char *shell_get_opt(int32_t argc, char **argv, char *pOption);
void shell_print_buffer(uint8_t *buff, uint32_t length);
void shell_print_buffer_reversed(uint8_t *buff, uint32_t length);
void shell_read_buffer(char *pValue, uint8_t *pDest, uint32_t length);

#ifdef __cplusplus
}
#endif
/*================================================================================================*/
#endif  /* _SHELL_UTILS_H */
