/*!
* \file
* This is a source file for shell_utils.c
*
* Copyright 2018 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied, transcribed, printed
* or by any electronic means - without specific written permission from NXP.
*
*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "network_utils.h"
#include "shell_utils.h"

/*==================================================================================================
Private macros
==================================================================================================*/

/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private prototypes
==================================================================================================*/

/*==================================================================================================
Private global variables declarations
==================================================================================================*/

/*==================================================================================================
Public global variables declarations
==================================================================================================*/
#if 0
extern shell_context_struct user_context;
#endif

/*==================================================================================================
Public functions
==================================================================================================*/
/*! *********************************************************************************
* \brief  Print command prompt
*
********************************************************************************** */
void shell_refresh(void)
{
    SHELL_NEWLINE();
#if 0
    shell_write(user_context.prompt);
#endif
}

/*! *********************************************************************************
* \brief  Changes the prompt name for the SHELL module .
*
* \param[in]  prompt the string which will be used for command prompt
*
* \remarks
*
********************************************************************************** */
void shell_change_prompt(char *prompt)
{
#if 0
    user_context.prompt = prompt;
#endif
}

/*! *********************************************************************************
* \brief  This function will write N bytes over the serial interface
*
* \param[in]  pBuff pointer to a string
* \param[in]  n number of chars to be written
*
* \remarks
*
********************************************************************************** */
void shell_writeN(char *pBuff, uint16_t n)
{
    while (n--)
    {
        PUTCHAR(*(pBuff++));
    }
}

/*! *********************************************************************************
* \brief  This function will write a NULL terminated string over the serial interface
*
* \param[in]  pBuff pointer to a string
*
* \remarks
*
********************************************************************************** */
void shell_write(char *pBuff)
{
    shell_writeN(pBuff, strlen((char *)pBuff));
}

/*! *********************************************************************************
* \brief  This function is used to get the pointer to the value of the option
*         specified by pOption parameter.
*
* \param [in]   argc      number of available arguments
* \param [in]   argv      pointer to the list of arguments
* \param [in]   pOption   pointer to the string which describes the option
*
* \return       char*     pointer to the value of the option
*
********************************************************************************** */
char *shell_get_opt
(
    int32_t argc,
    char **argv,
    char *pOption
)
{
    uint8_t idx;
    char *pRes = NULL;

    /* Check all arguments */
    for (idx = 0; (idx < argc) && (!pRes); idx++)
    {
        /* The option was found */
        if (FLib_MemCmp(argv[idx], pOption, MIN(strlen(argv[idx]), strlen(pOption))) && argv[idx + 1])
        {
            /* The value should be in this argument after the pOption text */
            if (*(argv[idx] + strlen(pOption)))
            {
                pRes = argv[idx] + strlen(pOption);
            }
            /* The value should be in the next argument */
            else
            {
                pRes = argv[idx + 1];
            }
        }
    }

    return pRes;
}

/*!*************************************************************************************************
\fn     void shell_print_buffer(uint8_t *buff, uint32_t length)
\brief  Hexlify a byte array

\param  [in]    buff      data
\param  [in]    length    length of data
***************************************************************************************************/
void shell_print_buffer
(
    uint8_t *buff,
    uint32_t length
)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        shell_printf("%02X", buff[i]);
    }
}

/*!*************************************************************************************************
\fn     void shell_print_buffer_reversed(uint8_t *buff, uint32_t length)
\brief  Hexlify a byte array, reversed

\param  [in]    buff      data
\param  [in]    length    length of data
***************************************************************************************************/
void shell_print_buffer_reversed
(
    uint8_t *buff,
    uint32_t length
)
{
    for (int i = length - 1; i >= 0; i--)
    {
        shell_printf("%02X", buff[i]);
    }
}

/*!*************************************************************************************************
\fn     void shell_read_buffer(char *pValue, uint8_t *pDest, uint32_t length)
\brief  Read hex stream bytes from ASCII.

\param  [in]     pValue       Pointer to start of the ASCII stream
\param  [out]    pDest        Pointer to the start of the output buffer
\param  [in]     length       The length of the ASCII stream
***************************************************************************************************/
void shell_read_buffer
(
    char *pValue,
    uint8_t *pDest,
    uint32_t length
)
{
    uint32_t iCount;
    uint16_t hexDig;

    for (iCount = 0; iCount < length; iCount++)
    {
        hexDig = NWKU_AsciiToHex((uint8_t *)pValue, sizeof(hexDig));
        pValue += sizeof(hexDig);
        pDest[iCount] = hexDig;
    }
}

/*==================================================================================================
Private functions
==================================================================================================*/

/*==================================================================================================
Private debug functions
==================================================================================================*/
