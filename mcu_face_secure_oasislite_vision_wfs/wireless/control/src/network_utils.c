/*!
* \file
* This is a source file for network_utils.c
*
* Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
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
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_pit.h"
#include "network_utils.h"

/*==================================================================================================
Private macros
==================================================================================================*/
#define UINT_MAX    0xFFFFFFFFU
#define ULONG_MAX   0xFFFFFFFFFFFFFFFFU

/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private prototypes
==================================================================================================*/

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static bool_t gPITInitFlag = FALSE;

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public functions
==================================================================================================*/
/*! -------------------------------------------------------------------------
 * \brief Initialize the timestamp module
 *---------------------------------------------------------------------------*/
void TMR_TimeStampInit(void)
{
    if (!gPITInitFlag)
    {
        /* Structure of initialize PIT */
        pit_config_t pitConfig;

        PIT_GetDefaultConfig(&pitConfig);

        /* Init pit module */
        PIT_Init(PIT, &pitConfig);

        /* Set timer period for channels 0 and 1 */
        PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, UINT_MAX);
        PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, UINT_MAX);

        PIT_SetTimerChainMode(PIT, kPIT_Chnl_1, true);
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        PIT_StartTimer(PIT, kPIT_Chnl_1);

        gPITInitFlag = TRUE;
    }
}

/*! -------------------------------------------------------------------------
 * \brief  return an 64-bit absolute timestamp
 * \return absolute timestamp in us
 *---------------------------------------------------------------------------*/
uint64_t TMR_GetTimestamp(void)
{
    /* timestamp with 10-nanoseconds resolution */
    uint64_t tstamp = ULONG_MAX - PIT_GetLifetimeTimerCount(PIT);

    return tstamp / 100;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSA_TimeDelay
 * Description   : This function is used to suspend the active thread for the given number of milliseconds.
 *
 *END**************************************************************************/
void OSA_TimeDelay(uint32_t millisec)
{
    vTaskDelay(pdMS_TO_TICKS(millisec));
}

/*! *********************************************************************************
* \brief  The byte at index i from the source buffer is copied to index ((n-1) - i)
*         in the destination buffer (and vice versa).
*
* \param[in, out]  pDst Pointer to the destination buffer.
*
* \param[in]  pSrc Pointer to the source buffer.
*
* \param[in]  cBytes Number of bytes to copy.
*
* \post
*
* \remarks
*
********************************************************************************** */
void FLib_MemCpyReverseOrder(void *pDst,
                             void *pSrc,
                             uint32_t cBytes)
{
    if (cBytes)
    {
        pDst = (uint8_t *)pDst + (uint32_t)(cBytes - 1);

        while (cBytes)
        {
            *((uint8_t *)pDst) = *((uint8_t *)pSrc);
            pDst = (uint8_t *)pDst - 1;
            pSrc = (uint8_t *)pSrc + 1;
            cBytes--;
        }
    }
}

/*!*************************************************************************************************
\fn    bool_t NWKU_CmpAddrPrefix6(ipAddr_t *addr1, ipAddr_t *addr2, uint32_t prefixLen)
\brief Compares first "prefixLen" bits of the ipv6 addresses.

\param [in]  addr1      first prefix to compare
\param [in]  addr2      second prefix to compare
\param [in]  prefixLen  lenght in bits to compare

\return      bool_t     TRUE if match
                        FALSE otherwise
***************************************************************************************************/
bool_t NWKU_CmpAddrPrefix6
(
    uint8_t *pAddr1,
    uint8_t *pAddr2,
    uint32_t prefixLen
)
{
    bool_t result;
    uint32_t prefixLenBytes = prefixLen >> 3;
    uint32_t prefixLenBitsMask = ((1 << (prefixLen % 8)) - 1) << (8 - (prefixLen % 8));

    if ((prefixLen <= 128)
            && (FLib_MemCmp(pAddr1, pAddr2, prefixLenBytes))
            && ((pAddr1[prefixLenBytes]&prefixLenBitsMask) == (pAddr2[prefixLenBytes]&prefixLenBitsMask)))
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}

/*!*************************************************************************************************
\fn    bool_t NWKU_MemCmpToVal(uint8_t *pAddr, uint8_t val, uint32_t len)
\brief Compare each octet of a given location to a value.

\param [in]     pAddr      location to be compared
\param [in]     val        reference value
\param [in]     len        length of location to be compared

\return         TRUE       If match
\return         FALSE      Otherwise
***************************************************************************************************/
bool_t NWKU_MemCmpToVal
(
    uint8_t     *pAddr,
    uint8_t     val,
    uint32_t    len
)
{
    while (len)
    {
        len--;

        if (pAddr[len] != val)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*!*************************************************************************************************
\fn     bool_t NWKU_BitCmp(uint8_t *pStr1, uint8_t *pStr2, uint8_t startBit, uint8_t stopBit)
\brief  Compare two strings bit by bit

\param [in]   pStr1            the start address of the first string to be compared
\param [in]   pStr2            the start address of the second string to be compared
\param [in]   startBit         the start bit number in the the 2 strings
\param [in]   stopBit          the stop bit number in the the 2 strings

\retval       bool_t           TRUE - if the strings match
                               FALSE - if the strings don't match
***************************************************************************************************/
bool_t NWKU_BitCmp
(
    uint8_t *pStr1,
    uint8_t *pStr2,
    uint8_t startBit,
    uint8_t stopBit
)
{
    uint8_t mask;

    /* Advance to the first byte that contains a bit to be compared */
    while (startBit >= 8)
    {
        pStr1++;
        pStr2++;
        startBit -= 8;
        stopBit -= 8;
    }

    /* Compare the first byte */
    if (startBit != 0)
    {
        mask = ((uint8_t)THR_ALL_FFs8 >> startBit);

        if (stopBit < 8)
        {
            mask = mask ^ ((uint8_t)THR_ALL_FFs8 >> stopBit);
        }

        if ((*pStr1 ^ *pStr2) & mask)
        {
            return FALSE;
        }

        /* If the stopBit and the startBit are in the same byte the comparison is over */
        if (stopBit < 8)
        {
            return TRUE;
        }

        stopBit -= 8;
        pStr1++;
        pStr2++;
    }

    while (stopBit >= 8)
    {
        if (*pStr1 != *pStr2)
        {
            return FALSE;
        }

        stopBit -= 8;
        pStr1++;
        pStr2++;
    }

    /* Compare the last byte */
    if (stopBit != 0)
    {
        mask = THR_ALL_FFs8 << (8 - stopBit);

        if ((*pStr1 ^ *pStr2) & mask)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*!*************************************************************************************************
\fn     bool_t NWKU_IsLLAddrEqual(uint8_t *pFirstLlAddr, uint32_t firstLlAddrSize,
                               uint8_t *pSecondLlAddr,uint32_t secondLlAddrSize)
\brief  Compare two Link Layer addresses

\param [in]   pFirstLlAddr      the start address of the first address to be compared
\param [in]   firstLlAddrSize   the size of the first address to be compared
\param [in]   pSecondLlAddr     the start address of the second address to be compared
\param [in]   secondLlAddrSize  the size of the second address to be compared

\retval       bool_t           TRUE - if the Link Layer addresses are the same
                               FALSE - if the Link Layer addresses are different
***************************************************************************************************/
bool_t NWKU_IsLLAddrEqual
(
    uint8_t *pFirstLlAddr,
    uint32_t firstLlAddrSize,
    uint8_t *pSecondLlAddr,
    uint32_t secondLlAddrSize
)
{
    bool_t bRetValue = FALSE;

    if ((firstLlAddrSize == secondLlAddrSize)
            && !memcmp(pFirstLlAddr, pSecondLlAddr, firstLlAddrSize))
    {
        bRetValue = TRUE;
    }

    return bRetValue;
}

/*!*************************************************************************************************
\fn    bool_t NWKU_CmpAddrPrefix6(ipAddr_t *addr1, ipAddr_t *addr2, uint32_t prefixLen)
\brief The common prefix length CommonPrefixLen(A, B) of two addresses A and B is the length of
       the longest prefix (looking at the most significant, or leftmost, bits) that the two
       addresses have in common.

\param [in]  addr1      first prefix to compare
\param [in]  addr2      second prefix to compare

\return      uin32_t    longhet prefix lenght in bits (0 - 128)
 ***************************************************************************************************/
uint32_t NWKU_GetCommonPrefixLen6
(
    ipAddr_t *addr1,
    ipAddr_t *addr2
)
{
    uint32_t length = 0;
    uint32_t i = 0;
    uint32_t bitI;
    bool_t loopStop = FALSE;
    uint8_t byteXor;

    while ((i < sizeof(ipAddr_t)) && (loopStop == FALSE))
    {
        /* XOR */
        byteXor = addr1->addr8[i] ^ addr2->addr8[i];

        for (bitI = 0; bitI < 8; bitI++)
        {
            if ((byteXor & 0x80) == 0x00)
            {
                length++;
                byteXor <<= 1; /* Shift to the next bit*/
            }
            else
            {
                loopStop = TRUE;
                break;
            }
        }

        i++;
    }

    return length;
}

/*!*************************************************************************************************
\fn     uint16_t NWKU_Revert16(uint16_t value)
\brief  Reverts a 16 bit numeric value.

\param [in]    value            the value to be converted

\return        uint16_t         the converted value
 ***************************************************************************************************/
uint16_t NWKU_Revert16(uint16_t value)
{
    return (uint16_t)((value << 8) | (value >> 8));
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_Revert32(uint32_t value)
\brief  Reverts a 32 bit numeric value.

\param [in]    value            the value to be converted

\return       uint32_t         the converted value
 ***************************************************************************************************/
uint32_t NWKU_Revert32(uint32_t value)
{
    return (uint32_t)(
               (value >> 24) |
               (value << 24) |
               ((value >> 8) & 0x0000FF00) |
               ((value << 8) & 0x00FF0000));
}

/*!*************************************************************************************************
\fn     uint64_t NWKU_Revert64(uint64_t value)
\brief  Reverts a 64 bit numeric value.

\param [in]    value            the value to be converted

\return        uint64_t         the converted value
 ***************************************************************************************************/
uint64_t NWKU_Revert64(uint64_t value)
{
    uuint64_t in;
    uuint64_t out;
    in.u64 = value;

    out.u8[0] = in.u8[7];
    out.u8[1] = in.u8[6];
    out.u8[2] = in.u8[5];
    out.u8[3] = in.u8[4];
    out.u8[4] = in.u8[3];
    out.u8[5] = in.u8[2];
    out.u8[6] = in.u8[1];
    out.u8[7] = in.u8[0];

    return out.u64;
}

/*!*************************************************************************************************
\fn     uint16_t NWKU_TransformArrayToUint16(uint8_t *pArray)
\brief  Converts an big endian array to a 16 bit numeric value.

\param [in]    pArray           the start address of the array

\return        uint16_t         the converted value
 ***************************************************************************************************/
uint16_t NWKU_TransformArrayToUint16(uint8_t *pArray)
{
    uuint16_t out;

    out.u8[1] = *pArray++;
    out.u8[0] = *pArray;

    return out.u16;
}

/*!*************************************************************************************************
\fn    uint32_t NWKU_TransformArrayToUint32(uint8_t *pArray)
\brief  Converts an big endian array to a 32 bit numeric value.

\param [in]    pArray           the start address of the array

\return        uint32_t         the converted value
 ***************************************************************************************************/
uint32_t NWKU_TransformArrayToUint32
(
    uint8_t *pArray
)
{
    uuint32_t out;

    out.u8[3] = *pArray++;
    out.u8[2] = *pArray++;
    out.u8[1] = *pArray++;
    out.u8[0] = *pArray;

    return out.u32;

}

/*!*************************************************************************************************
\fn     uint64_t NWKU_TransformArrayToUint64(uint8_t *pArray)
\brief  Converts an big endian array to a 64 bit numeric value.

\param [in]     pArray           the start address of the array

\return         uint64_t         the converted value
 ***************************************************************************************************/
uint64_t NWKU_TransformArrayToUint64
(
    uint8_t *pArray
)
{
    uuint64_t out;

    out.u8[7] = *pArray++;
    out.u8[6] = *pArray++;
    out.u8[5] = *pArray++;
    out.u8[4] = *pArray++;
    out.u8[3] = *pArray++;
    out.u8[2] = *pArray++;
    out.u8[1] = *pArray++;
    out.u8[0] = *pArray;

    return out.u64;
}

/*!*************************************************************************************************
\fn     void NWKU_TransformUint16ToArray(uint8_t *pArray, uint16_t value)
\brief  Converts a 16 bit numeric value to array.

\param [in]    value            the value to be converted
\param [out]   pArray           the start address of the array

\return         none
 ***************************************************************************************************/
void NWKU_TransformUint16ToArray
(
    uint8_t *pArray,
    uint16_t value
)
{
    *pArray++ = (uint8_t)(value >> 8);
    *pArray   = (uint8_t)(value);
}

/*!*************************************************************************************************
\fn     void NWKU_TransformUint32ToArray(uint8_t *pArray, uint32_t value)
\brief  Converts a 32 bit numeric value to array.

\param [in]    value            the value to be converted
\param [out]   pArray           the start address of the array

\return        none
 ***************************************************************************************************/
void NWKU_TransformUint32ToArray
(
    uint8_t *pArray,
    uint32_t value
)
{
    *pArray++ = (uint8_t)(value >> 24);
    *pArray++ = (uint8_t)(value >> 16);
    *pArray++ = (uint8_t)(value >> 8);
    *pArray   = (uint8_t)(value);
}

/*!*************************************************************************************************
\fn    void NWKU_TransformUint64ToArray(uint8_t *pArray, uint64_t value)
\brief  Converts a 64 bit numeric value to array.

\param [in]    value            the value to be converted
\param [out]   pArray           the start address of the array

\return        none
 ***************************************************************************************************/
void NWKU_TransformUint64ToArray
(
    uint8_t *pArray,
    uint64_t value
)
{
    uuint64_t in;
    in.u64 = value;

    *pArray++ = in.u8[7];
    *pArray++ = in.u8[6];
    *pArray++ = in.u8[5];
    *pArray++ = in.u8[4];
    *pArray++ = in.u8[3];
    *pArray++ = in.u8[2];
    *pArray++ = in.u8[1];
    *pArray   = in.u8[0];
}

/*!*************************************************************************************************
\fn    uint64_t NWKU_TransformArrayToValue
\brief  Converts an array to a numeric value.

\param [in]   pArray          the start address of the array
\param [in]   nbOfBytes       the length of the data to be converted

\return       uint32_t        the value converted from the array
 ***************************************************************************************************/
uint64_t NWKU_TransformArrayToValue
(
    uint8_t *pArray,
    uint32_t nbOfBytes
)
{
    uuint64_t  value;
    uint32_t   iCount;
    value.u64 = 0;

    for (iCount = 0; iCount < nbOfBytes; iCount ++)
    {
        value.u8[nbOfBytes - iCount - 1] = pArray[iCount];
    }

    return value.u64;
}

/*!*************************************************************************************************
\fn    void NWKU_TransformValueToArray(uint64_t value, uint8_t *pArray, uint32_t nbOfBytes)
\brief  Converts a numeric value to array.

\param [in]    value            the value to be converted
\param [out]   pArray           the start address of the array
\param [in]    nbOfBytes        the length of the data to be converted

\retval        none
 ***************************************************************************************************/
void NWKU_TransformValueToArray
(
    uint64_t value,
    uint8_t *pArray,
    uint32_t nbOfBytes
)
{
    uint32_t   iCount;

    uuint64_t  u_value;
    u_value.u64 = value;

    for (iCount = 0; iCount < nbOfBytes; iCount ++)
    {
        pArray[nbOfBytes - iCount - 1] = u_value.u8[iCount];
    }
}

/*!*************************************************************************************************
\fn    bool_t NWKU_GetLut8(lut8_t *pLutTable, uint8_t lutTableSize, uint8_t type,
                           uint8_t *pEntryIndex)
\brief  Searches an entry in the lookup table indicated by pLutTable.

\param [in]    pLutTable     pointer to the lookup table
\param [in]    lutTableSize  lookup table size
\param [in]    type          type to find

\param [out]   pEntryIndex   index of the entry in case the entry is found

\retval        TRUE          returned when the entry is found
\retval        FALSE         otherwise
 ***************************************************************************************************/
bool_t NWKU_GetLut8
(
    lut8_t *pLutTable,
    uint8_t lutTableSize,
    uint8_t type,
    uint8_t *pEntryIndex
)
{
    uint8_t iCount;
    bool_t bRetValue = FALSE;

    if ((NULL != pLutTable) &&
            (NULL != pEntryIndex) &&
            (lutTableSize > 0U))
    {
        for (iCount = 0U; iCount < lutTableSize; iCount ++)
        {
            if ((pLutTable + iCount)->type == type)
            {
                /* Entry found */
                bRetValue = TRUE;
                *pEntryIndex = (pLutTable + iCount)->idx;
                break;
            }
        }
    }

    return bRetValue;
}

/*!*************************************************************************************************
\fn    int32_t NWKU_atoi(char *str)
\brief  Converts a string into an integer.

\param [in]    pStr       pointer to string

\retval     int32_t       integer converted from string.
***************************************************************************************************/
int32_t NWKU_atoi
(
    char *pStr
)
{
    int32_t res = 0;

    while ((*pStr != '\0') && (*pStr != ' ') && (*pStr >= '0') && (*pStr <= '9'))
    {
        res = res * 10 + *pStr - '0';
        pStr++;
    }

    return res;
}

/*!*************************************************************************************************
\fn    int64_t NWKU_atol(char *str)
\brief  Converts a string into an long integer.

\param [in]    pStr       pointer to string

\retval     int64_t       integer converted from string.
***************************************************************************************************/
int64_t NWKU_atol
(
    char *pStr
)
{
    int64_t res = 0;

    while ((*pStr != '\0') && (*pStr != ' ') && (*pStr >= '0') && (*pStr <= '9'))
    {
        res = res * 10 + *pStr - '0';
        pStr++;
    }

    return res;
}
/*!*************************************************************************************************
\fn    void NWKU_PrintDec(uint64_t value, uint8_t *pString, uint32_t nbOfDigits, bool_t bLeadingZeros)
\brief  Prints in a string decimal values.

\param [in]         value           integer value
\param [in/out]     pString         pointer to output location
\param [in]         nbPrintDigits   number of digits to be printed
\param [in]         bLeadingZeros   indicate if leading zeros are put or omitted
                                    TRUE - print leading zeros
                                    FALSE - do not print leading zeros
***************************************************************************************************/
void NWKU_PrintDec
(
    uint64_t value,
    uint8_t *pString,
    uint32_t nbPrintDigits,
    bool_t bLeadingZeros
)
{
    uint32_t nbOfDigits = 0;
    uint64_t tempValue = value;
    int32_t delta = 0;

    /* Get the number of digits for the number */
    if (0 == value)
    {
        /* If value == 0, print only one char */
        if (nbPrintDigits == THR_ALL_FFs8)
        {
            nbPrintDigits = 1;
        }

        FLib_MemSet(pString, '0', nbPrintDigits);
    }
    else
    {
        while (tempValue)
        {
            nbOfDigits++;
            tempValue = tempValue / 10;
        }

        delta = nbPrintDigits - nbOfDigits;
        tempValue = value;

        FLib_MemSet(pString, '0', nbOfDigits);

        /* Write only the number of digits wanted */
        while (nbPrintDigits && nbOfDigits)
        {
            if ((TRUE == bLeadingZeros) || (delta == 0))
            {
                *(pString + nbPrintDigits - 1) = (tempValue % 10) + '0';
            }
            else if (delta > 0)
            {
                *(pString + nbOfDigits - 1) = (tempValue % 10) + '0';
            }

            tempValue = tempValue / 10;
            nbPrintDigits--;
            nbOfDigits--;
        }
    }
}


/*!*************************************************************************************************
\fn    int32_t pton(uint8_t af, char *pTxt, ipAddr_t *pIpAddr)
\brief  Converts a string into an ipAddr_t. Presentation to network function.

\param [in]    af       address family(AF_INET, AF_INET6)
\param [in]    pTxt     pointer to the start of the string to be parsed
\param [in]    pIpAddr  pointer to the start of the allocated ipAddr_t structure

\retval     1 on success
            0 string address is not valid
            -1 on error
***************************************************************************************************/
int32_t pton
(
    uint8_t af,
    char *pTxt,
    ipAddr_t *pIpAddr
)
{
    int32_t result = 1;
    uint8_t *pStr;
    uint8_t pText[INET6_ADDRSTRLEN];
    uint8_t iIpAddr;
    uint8_t iTmp;

    if (af == AF_INET6)
    {
        uint8_t tmp[5] = {0};
        bool_t shortedAddr = FALSE;

        /* Copy original string and compute size */
        FLib_MemSet(pText, 0, INET6_ADDRSTRLEN);
        FLib_MemCpy(pText, pTxt, strlen((const char *)pTxt));

        /* Clear ipAddress */
        FLib_MemSet(pIpAddr->addr16, 0, 8 * sizeof(uint16_t));

        /* Parse forward until '::' or end of string */
        pStr = pText;
        iIpAddr = 0;
        iTmp = 0;

        while (1)
        {
            if ((*pStr) && (!(((*pStr >= 'a') && (*pStr <= 'f'))
                              || ((*pStr >= 'A') && (*pStr <= 'F'))
                              || ((*pStr >= '0') && (*pStr <= '9'))
                              || (*pStr == ':'))))
            {
                result = -1;
                return result;
            }

            if (*pStr != ':' && *pStr)
            {
                /* Check for address validity:
                 *     - the first group is not greater than 4 characters
                 *     - there is a '.' character inside */
                if ((iTmp >= 4) || (*pStr == '.'))
                {
                    result = -1;
                    break;
                }

                tmp[iTmp++] = *pStr;
            }
            else
            {
                uint32_t val = 0;
                val = NWKU_AsciiToHex(tmp, 5);
                pIpAddr->addr16[iIpAddr] = ntohas((uint8_t *)&val);

                iTmp = 0;
                FLib_MemSet(tmp, 0, sizeof(tmp));

                if (*pStr)
                {
                    /* Safe guard */
                    if (iIpAddr >= 8)
                    {
                        break;
                    }

                    if (*(pStr + 1) == ':')
                    {
                        shortedAddr = TRUE;
                        pStr += 2;
                        break; /* begin to parse the second part */
                    }

                    /* Set variables */
                    iIpAddr++;
                }
                else
                {
                    iTmp = 0;
                    FLib_MemSet(tmp, 0, sizeof(tmp));
                    break;
                }
            }

            pStr++;
        }

        /* Parse the string after :: */
        if (shortedAddr)
        {
            uint8_t cRemComma = 0;
            uint8_t *p2 = pStr;

            /* Count remaining commas */
            while (*p2)
            {
                if (*p2 == ':') { cRemComma++; }

                p2++;
            }

            iIpAddr = 7 - cRemComma;

            while (1)
            {
                if (*pStr != ':' && *pStr)
                {
                    tmp[iTmp++] = *pStr;
                }
                else
                {
                    uint32_t val = 0;
                    val = NWKU_AsciiToHex(tmp, 5);
                    pIpAddr->addr16[iIpAddr] = ntohas((uint8_t *)&val);

                    iTmp = 0;
                    FLib_MemSet(tmp, 0, sizeof(tmp));

                    if (*pStr)
                    {
                        /* String address is not valid */
                        if (*(pStr + 1) == ':')
                        {
                            result = 0;
                            break;
                        }

                        /* Safe guard */
                        if (iIpAddr >= 7)
                        {
                            break;
                        }

                        /* Set variables */
                        iIpAddr++;
                    }
                    else
                    {
                        break;
                    }
                }

                pStr++;
            }
        }
    }

    if (af == AF_INET)
    {
        uint8_t tmp[3] = {0};

        /* Copy original string and compute size */
        FLib_MemSet(pText, 0, INET_ADDRSTRLEN);
        FLib_MemCpy(pText, pTxt, strlen((const char *)pTxt));

        /* Clear ipAddress */
        FLib_MemSet(pIpAddr->addr8, 0, 16 * sizeof(uint8_t));

        /* Parse forward until '::' or end of string */
        pStr = pText;
        pIpAddr->addr8[10] = 255U;
        pIpAddr->addr8[11] = 255U;
        iIpAddr = 12;
        iTmp = 0;

        while (1)
        {
            if (!(((*pStr >= '0') && (*pStr <= '9')) || (*pStr == '.') || (*pStr == 0)))
            {
                result = -1;
                return result;
            }

            if (*pStr != '.' && *pStr)
            {
                tmp[iTmp++] = *pStr;
            }
            else
            {
                uint32_t val = 0;
                val = NWKU_AsciiToDec(tmp, 5);
                /* Invalid IPv4 address */

                if (val > 255)
                {
                    result = 0;
                    break;
                }

                pIpAddr->addr8[iIpAddr] = (uint8_t) val;

                iTmp = 0;
                FLib_MemSet(tmp, 0, sizeof(tmp));

                if (*pStr)
                {
                    /* Safe guard */
                    if (iIpAddr >= 16)
                    {
                        break;
                    }

                    iIpAddr++;
                }
                else
                {
                    iTmp = 0;
                    FLib_MemSet(tmp, 0, sizeof(tmp));
                    break;
                }
            }

            pStr++;
        }
    }

    return result;
}

/*!*************************************************************************************************
\fn    char *ntop(uint8_t af, char *pTxt, ipAddr_t *pIpAddr, uint32_t strLen)
\brief  Converts an ipAddr_t into a string. Network to presentation function.

\param [in]    af       address family(AF_INET, AF_INET6)
\param [in]    pIpAddr  pointer to the start of the allocated ipAddr_t structure
\param [out]   pStr     pointer to the allocated string where to put the result
\param [in]    strLen   size of the input buffer

\return        char*    pointer to the resulted buffer
***************************************************************************************************/
char *ntop
(
    uint8_t af,
    ipAddr_t *pIpAddr,
    char *pStr,
    uint32_t strLen
)
{
    uint8_t iIp, iStr;
    bool_t skipWrite = FALSE;

    /* Clear the output buffer */
    FLib_MemSet(pStr, 0, strLen);

    /* IPV6 */
    if (af == AF_INET6)
    {
        /* Find the position where the biggest collection of zeros starts */
        uint8_t pos = THR_ALL_FFs8;
        uint8_t activeCount = 0;
        uint8_t highestCount = 0;

        for (iIp = 0, iStr = 0; iIp < 8; iIp++)
        {
            /* Check whether we have found a 0 and we are not the last element */
            if ((pIpAddr->addr16[iIp] == 0) && (iIp != 7))
            {
                activeCount++;
            }
            /* if we don't have a zero or we are the last element */
            else
            {
                /* if we were previously counting zeros and now we encountered a number != 0 we have
                 * to reset the activeCount and see if the activeCount is greater than the
                 * highestCount */
                if (pIpAddr->addr16[iIp])
                {
                    if (activeCount)
                    {
                        if (activeCount > highestCount)
                        {
                            highestCount = activeCount; /* update the highestCount */
                            pos = iIp - activeCount; /* Update the position */
                        }

                        activeCount = 0;
                    }
                }
                /* We read 0 and this is the last uint16 element */
                else
                {
                    if (activeCount)
                    {
                        if (activeCount > highestCount)
                        {
                            highestCount = activeCount + 1; /* update the highestCount */
                            pos = iIp - activeCount; /* Update the position */
                        }

                        activeCount = 0;
                    }
                }
            }
        }

        /* Write the output string */
        for (iIp = 0, iStr = 0; iIp < 16; iIp++)
        {
            /* If we have to fill "::" */
            if ((pos != THR_ALL_FFs8) && (pos * 2) == iIp)
            {
                pStr[iStr] = ':';
                iStr += 1;
                pStr[iStr] = ':';
                iStr += 1;
                skipWrite = TRUE;
            }

            /* If we finished to write zeroes enable writing */
            if (skipWrite && pIpAddr->addr16[iIp / 2])
            {
                skipWrite = FALSE;
            }

            /* If the write process is not suspended: write data in the output buffer */
            if (!skipWrite)
            {
                /* Put bytes in string */
                if ((iIp % 2 == 0) && (pIpAddr->addr16[iIp / 2] == 0))
                {
                    pStr[iStr] = '0';
                    iStr++;
                    iIp++;
                }
                else
                {
                    /* Do not put '0' before numbers */
                    if (pIpAddr->addr8[iIp] || (iIp % 2))
                    {
                        /* The number's representation has 2 nibbles for the:
                         * - first byte if it is composed from 2 nibbles
                         * - second byte if we have a byte in the left different than 0
                         * - second byte if it is composed from 2 nibbles */
                        if (
                            ((iIp % 2 == 0) && (pIpAddr->addr8[iIp] & 0xF0)) ||
                            ((iIp % 2) && (pIpAddr->addr8[iIp - 1])) ||
                            ((iIp % 2) && (pIpAddr->addr8[iIp] & 0xF0))
                        )
                        {
                            NWKU_HexToAscii(&pIpAddr->addr8[iIp], 1, (uint8_t *)(pStr + iStr), 2, FALSE);
                            iStr += 2;
                        }
                        /* the number's representation has 1 byte */
                        else
                        {
                            NWKU_HexToAscii(&pIpAddr->addr8[iIp], 1, (uint8_t *)(pStr + iStr), 1, FALSE);
                            iStr += 1;
                        }
                    }
                }

                /* After each two written bytes(after each odd index) we have to put ":". Don't put
                 * it after the last byte, or before a "::" sequence */
                if ((iIp % 2) && (iIp < 15) && (iIp + 1 != pos * 2))
                {
                    pStr[iStr] = ':';
                    iStr += 1;
                }
            }
        }
    } /* IPV6 */
    /* IPV4 */
    else
    {
        iStr = 0;
        NWKU_PrintDec(pIpAddr->addr8[12], (uint8_t *)(pStr + iStr), THR_ALL_FFs8, FALSE);
        iStr++;

        if (pIpAddr->addr8[12] > 9)
        {
            iStr++;
        }

        if (pIpAddr->addr8[12] > 99)
        {
            iStr++;
        }

        pStr[iStr] = '.';
        iStr++;
        NWKU_PrintDec(pIpAddr->addr8[13], (uint8_t *)(pStr + iStr), THR_ALL_FFs8, FALSE);
        iStr++;

        if (pIpAddr->addr8[13] > 9)
        {
            iStr++;
        }

        if (pIpAddr->addr8[13] > 99)
        {
            iStr++;
        }

        pStr[iStr] = '.';
        iStr++;
        NWKU_PrintDec(pIpAddr->addr8[14], (uint8_t *)(pStr + iStr), THR_ALL_FFs8, FALSE);
        iStr++;

        if (pIpAddr->addr8[14] > 9)
        {
            iStr++;
        }

        if (pIpAddr->addr8[14] > 99)
        {
            iStr++;
        }

        pStr[iStr] = '.';
        iStr++;
        NWKU_PrintDec(pIpAddr->addr8[15], (uint8_t *)(pStr + iStr), THR_ALL_FFs8, FALSE);
        iStr++;

        if (pIpAddr->addr8[15] > 9)
        {
            iStr++;
        }

        if (pIpAddr->addr8[15] > 99)
        {
            iStr++;
        }

        pStr[iStr] = '\0';
    } /* IPV4 */

    return pStr;
}
/*!*************************************************************************************************
\fn     ptoll(uint8_t *pIn, uint32_t len, llAddr_t *pLlAddr)
\brief  Converts a string into an llAddr_t. Presentation to ll function.

\param [in]    pIn      pointer to the input buffer
\param [in]    len      size of the input buffer
\param [in]    pLlAddr  pointer to the start of the allocated llAddr_t structure

\retval     TRUE on success
            FALSE on error
***************************************************************************************************/
bool_t ptoll
(
    uint8_t *pIn,
    uint32_t len,
    llAddr_t *pLlAddr
)
{
    bool_t res = TRUE;

    /* EUI64 0xaabbccddeeff1122 */
    if ((pIn[0] == '0') && (pIn[1] == 'x') && (len == 18))
    {
        uint8_t idx;

        pIn += 2;
        pLlAddr->addrSize = gLlayerAddrEui64_c;
        pLlAddr->eui[0] = (NWKU_ByteToDec(*pIn) << 4) | NWKU_ByteToDec(*(pIn + 1));

        for (idx = 1; idx < 8; idx++)
        {
            pIn += 2;
            pLlAddr->eui[idx] = (NWKU_ByteToDec(*pIn) << 4) | NWKU_ByteToDec(*(pIn + 1));
        }
    }
    /* EUI64 aa:bb:cc:dd:ee:ff:11:22 */
    else if ((len == 23) && (pIn[2] == ':'))
    {
        uint8_t idx;

        pLlAddr->addrSize = gLlayerAddrEui64_c;
        pLlAddr->eui[0] = (NWKU_ByteToDec(*pIn) << 4) | NWKU_ByteToDec(*(pIn + 1));

        for (idx = 1; idx < 8; idx++)
        {
            pIn += 3;
            pLlAddr->eui[idx] = (NWKU_ByteToDec(*pIn) << 4) | NWKU_ByteToDec(*(pIn + 1));
        }
    }
    /* Unknown */
    else
    {
        res = FALSE;
    }

    return res;
}

/*!*************************************************************************************************
\fn     uint8_t NWKU_ParseHexValue(char *pInput)
\brief  Converts a hex string to byte.

\param  [in]    pInput     pointer to string

\return uint8_t value
***************************************************************************************************/
uint8_t NWKU_ParseHexValue(char *pInput)
{
    uint8_t i, length = strlen(pInput);
    uint32_t value;

    /* If the hex misses a 0, return error */
    if (length % 2)
    {
        return 0;
    }

    if (!strncmp(pInput, "0x", 2))
    {
        length -= 2;

        /* Save as little endian hex value */
        value = strtoul(pInput + 2, NULL, 16);

        FLib_MemCpy(pInput, &value, sizeof(uint32_t));

        return (length - 2);
    }
    else
    {
        char octet[2];

        /* Save as big endian hex */
        for (i = 0; i < length / 2; i++)
        {
            FLib_MemCpy(octet, &pInput[i * 2], 2);

            pInput[i] = strtoul(octet, NULL, 16);
        }

        return length / 2;
    }
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_AsciiToHex(uint8_t *pString, uint32_t strLen)
\brief  Converts a string into hex.

\param  [in]    pString     pointer to string
\param  [in]    strLen      string length

\return uint32_t value in hex
***************************************************************************************************/
uint32_t NWKU_AsciiToHex
(
    uint8_t *pString,
    uint32_t strLen
)
{
    uint32_t length = strLen;
    uint32_t retValue = 0, hexDig = 0;
    bool_t validChar;

    while (length && pString)
    {
        hexDig = 0;
        validChar = FALSE;

        if (*pString >= '0' && *pString <= '9')
        {
            hexDig = *pString - '0';
            validChar = TRUE;
        }

        if (*pString >= 'a' && *pString <= 'f')
        {
            hexDig = *pString - 'a' + 10;
            validChar = TRUE;
        }

        if (*pString >= 'A' && *pString <= 'F')
        {
            hexDig = *pString - 'A' + 10;
            validChar = TRUE;
        }

        if (validChar == TRUE)
        {
            retValue = (retValue << 4) ^ hexDig;
        }

        pString++;
        length--;
    }

    return retValue;
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_AsciiToDec(uint8_t *pString, uint32_t strLen)
\brief  Converts a string into hex.

\param  [in]    pString     pointer to string
\param  [in]    strLen      string length

\return uint32_t value in dec
***************************************************************************************************/
uint32_t NWKU_AsciiToDec
(
    uint8_t *pString,
    uint32_t strLen
)
{
    uint32_t length = strLen;
    uint32_t retValue = 0, hexDig = 0;

    while (length && pString)
    {
        hexDig = 0;

        if (*pString >= '0' && *pString <= '9')
        {
            hexDig = *pString - '0';

            retValue = (retValue * 10) + hexDig;
        }

        pString++;
        length--;
    }

    return retValue;
}

/*!*************************************************************************************************
\fn     uint8_t NWKU_ByteToDec(uint8_t byte)
\brief  Converts a byte from ASCII to decimal.

\param  [in]    byte     byte value in ASCII

\return uint8_t value in decimal
***************************************************************************************************/
uint8_t NWKU_ByteToDec
(
    uint8_t byte
)
{
    if (byte >= '0' && byte <= '9')
    {
        byte -= '0';
    }
    else if ((byte >= 'a' && byte <= 'f'))
    {
        byte = byte - 'a' + 10;
    }
    else if ((byte >= 'A' && byte <= 'F'))
    {
        byte = byte - 'A' + 10;
    }

    return byte;
}
/*!*************************************************************************************************
\fn     void NWKU_HexToAscii(uint8_t *pInputBuff,uint32_t inputBuffLen,uint8_t *pOutputBuffer,
                                uint32_t outputBuffLen, bool_t useUpperCase)
\brief  Converts a byte to ASCII.

\param  [in]    pInputBuff      pointer to input buffer
\param  [in]    inputBuffLen    length of the input buffer
\param  [in]    pOutputBuffer   pointer to output buffer
\param  [in]    outputBuffLen   length of the output buffer
\param  [in]    useUpperCase    indicate if the output shall be in upper/lower case

***************************************************************************************************/
void NWKU_HexToAscii
(
    uint8_t *pInputBuff,
    uint32_t inputBuffLen,
    uint8_t *pOutputBuffer,
    uint32_t outputBuffLen,
    bool_t useUpperCase
)
{
    if ((outputBuffLen >= 2 * inputBuffLen) || ((inputBuffLen == outputBuffLen) == 1))
    {
        for (uint32_t i = 0; i < inputBuffLen; i++)
        {
            int8_t nib0 = 0;
            int8_t nib1 = 0;

            nib0 = (0xF0 & pInputBuff[i]) >> 4;
            nib1 = 0x0F & pInputBuff[i];

            /* if the first nib is 0 and there is only one byte for output,
            the first nib is elided */
            if ((nib0 == 0) && (outputBuffLen == 1))
            {
                pOutputBuffer[2 * i] = NWKU_NibToAscii(nib1, useUpperCase);
            }
            else
            {
                pOutputBuffer[2 * i] = NWKU_NibToAscii(nib0, useUpperCase);
                pOutputBuffer[2 * i + 1] = NWKU_NibToAscii(nib1, useUpperCase);
            }
        }
    }
}

/*!*************************************************************************************************
\fn     uint8_t NWKU_NibToAscii(uint8_t nib)
\brief  Converts a nib from hex to ASCII.

\param  [in]    nib     nib value in hex

\return uint8_t value in ascii
***************************************************************************************************/
uint8_t NWKU_NibToAscii
(
    int8_t nib,
    bool_t useUpperCase
)
{
    if ((nib >= 0) && (nib <= 0x09))
    {
        nib += '0';
    }


    if ((nib >= 0x0A) && (nib <= 0x0F))
    {
        if (TRUE == useUpperCase)
        {
            nib += 'A' - 0x0A;
        }
        else
        {
            nib += 'a' - 0x0A;
        }
    }


    return nib;
}

/*!*************************************************************************************************
\fn    uint32_t NWKU_TmrRtcGetElapsedTimeInSeconds(uint32_t timestamp)
\brief Calculates the time passed in seconds from the provided timestamp.

\param [in]    timestamp     timestamp in seconds

\return        uint32_t      number of seconds that have passed since the provided timestamp
 ***************************************************************************************************/
uint32_t NWKU_TmrRtcGetElapsedTimeInSeconds
(
    uint32_t timestamp
)
{
    return (uint32_t)(TmrMicrosecondsToSeconds(TMR_GetTimestamp()) - timestamp);
}

/*!*************************************************************************************************
\fn    bool_t NWKU_IsNUmber(char *pString)
\brief Check if a string is a number.

\param [in]    pString      pointer to the string

\return        bool_t       TRUE if the string represents a number
                            FALSE if the string does not represent a number
***************************************************************************************************/
bool_t NWKU_IsNUmber
(
    char *pString
)
{
    bool_t ret = TRUE;

    while (*pString)
    {
        if (*pString < '0' || *pString > '9')
        {
            ret = FALSE;
            break;
        }

        pString++;
    }

    return ret;
}

/*!*************************************************************************************************
\fn     void NWKU_IncrementIp6Addr(uint32_t startInterval, uint32_t endInterval)
\brief  This function increments a IPv6 type address

\param    [in]  pIpAddr   pointer to IPv6 address
***************************************************************************************************/
void NWKU_IncrementIp6Addr
(
    ipAddr_t *pIpAddr
)
{
    uint64_t tempIp = 0;

    tempIp = ntohall(&pIpAddr->addr8[8]);
    tempIp++;
    htonall(&pIpAddr->addr8[8], tempIp);
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_RightRotate(uint32_t val, uint8_t amount)
\brief  This function rotate a 32bit number to the right with an amount of bits.

\param    [in]  val     number
\param    [in]  amount  number of bits to rotate

\return   uint32_t      32bit number rotated to the right with an amount of bits
***************************************************************************************************/
uint32_t NWKU_RightRotate
(
    uint32_t val,
    uint8_t amount
)
{
    return (val >> amount) | (val << (uint8_t)(32 - amount));
}

/*!*************************************************************************************************
\fn     void NWKU_GetIIDFromLLADDR(llAddr_t *pLlAddr, uint16_t panId, uint8_t *pIID)
\brief  The function returns the IID from a Link-Layer address.

\param  [in]    pLlAddr         Pointer to the Link-Layer address
\param  [in]    panId           PAN ID
\param  [out]   pIID            Pointer to the variable which will hold the IID
***************************************************************************************************/
void NWKU_GetIIDFromLLADDR
(
    llAddr_t *pLlAddr,
    uint16_t panId,
    uint8_t *pIID
)
{
    if (gLlayerAddrEui64_c == pLlAddr->addrSize)
    {
        FLib_MemCpy(pIID, &pLlAddr->eui, pLlAddr->addrSize);
        *(pIID) ^= 0x02;
    }
    else
    {
        htonas(pIID, panId);
        htonas(pIID + 2, 0x00FF);
        htonas(pIID + 4, 0xFE00);
        FLib_MemCpy(pIID + 6, pLlAddr->eui, gLlayerAddrEui16_c);
    }
}

/*!*************************************************************************************************
\fn     void NWKU_GetLLAddrFromIID(uint8_t *pIID, llAddr_t *pLlAddr)
\brief  This function returns the Link-Layer address from the IID.

\param  [in]    pIID            Pointer to the IID
\param  [out]   pLlAddr         Pointer to the variable which will hold the Link-Layer address
***************************************************************************************************/
void NWKU_GetLLAddrFromIID
(
    uint8_t *pIID,
    llAddr_t *pLlAddr
)
{
    uint8_t aRlocIidMatch[] = {0x00, 0xFF, 0xFE, 0x00};

    if (!memcmp(pIID + 2, aRlocIidMatch, 4))
    {
        /* IID created from short address */
        pLlAddr->addrSize = gLlayerAddrEui16_c;
        FLib_MemCpy(pLlAddr->eui, pIID + 6, gLlayerAddrEui16_c);
    }
    else
    {
        /* IID created from extended address */
        pLlAddr->addrSize = gLlayerAddrEui64_c;
        FLib_MemCpy(pLlAddr->eui, pIID, 8);
        pLlAddr->eui[0] ^= 0x02;
    }
}

/*!*************************************************************************************************
\fn     void bool_t NWKU_IsAddrWithShortAddr(ipAddr_t *pIpAddr)
\brief  This function returns true if the IPv6 address is formed with short MAC address.

\param  [in]    pIpAddr       Pointer to the IPv6 address

\return         bool_t        TRUE if the IPv6 address is formed with short MAC address
***************************************************************************************************/
bool_t NWKU_IsIPAddrBasedOnShort
(
    ipAddr_t *pIpAddr
)
{
    uint8_t aRlocIidMatch[] = {0x00, 0xFF, 0xFE, 0x00};

    if (!memcmp(&pIpAddr->addr8[10], aRlocIidMatch, 4))
    {
        return TRUE;
    }

    return FALSE;
}
/*!*************************************************************************************************
\fn     bool_t NWKU_GetBit(uint32_t bitNr, uint8_t *pArray)
\brief  This function returns the value of a bit in an array.

\param  [in]    bitNr           bit number in the whole array
\param  [in]    pArray          pointer to the start of the array

\retval         TRUE            if the bit is set
\retval         FALSE           if the bit is not set
***************************************************************************************************/
bool_t NWKU_GetBit
(
    uint32_t bitNr,
    uint8_t *pArray
)
{
    return ((pArray[bitNr / 8] & (1 << (bitNr % 8))) ? TRUE : FALSE);
}

/*!*************************************************************************************************
\fn     void NWKU_SetBit(uint32_t bitNr, uint8_t* pArray)
\brief  This function sets a bit in an array.

\param  [in]    bitNr           bit number in the whole array
\param  [in]    pArray          pointer to the start of the array
***************************************************************************************************/
void NWKU_SetBit
(
    uint32_t bitNr,
    uint8_t *pArray
)
{
    pArray[bitNr / 8] |= (1 << (bitNr % 8));
}


/*!*************************************************************************************************
\fn     void NWKU_ClearBit(uint32_t bitNr, uint8_t* pArray)
\brief  This function clears a bit in an array.

\param  [in]    bitNr           bit number in the whole array
\param  [in]    pArray          pointer to the start of the array
***************************************************************************************************/
void NWKU_ClearBit
(
    uint32_t bitNr,
    uint8_t *pArray
)
{
    pArray[bitNr / 8] &= ~(1 << (bitNr % 8));
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_GetFirstBitValueInRange(uint8_t* pArray, uint32_t lowBitNr, uint32_t
        highBitNr, bool_t bitValue)
\brief  This function returns the first bit with value=bitValue in a range in the array.

\param  [in]    pArray          pointer to the start of the array
\param  [in]    lowBitNr        starting bit number
\param  [in]    highBitNr       ending bit number
\param  [in]    bitValue        bit value

\return         uint32_t        bit number
***************************************************************************************************/
uint32_t NWKU_GetFirstBitValueInRange
(
    uint8_t *pArray,
    uint32_t lowBitNr,
    uint32_t highBitNr,
    bool_t bitValue
)
{
    for (; lowBitNr < highBitNr; lowBitNr++)
    {
        if (bitValue == NWKU_GetBit(lowBitNr, pArray))
        {
            return lowBitNr;
        }
    }

    return ((uint32_t) - 1); // invalid
}

/*!*************************************************************************************************
\fn     uint32_t getFirstBitValue(uint8_t* pArray, uint32_t arrayBytes, bool_t bitValue)
\brief  This function returns the index of the first bit with value=bitValue.

\param  [in]    pArray          pointer to the start of the array
\param  [in]    arrayBytes      number of bytes in the array
\param  [in]    bitValue        bit value

\return         uint32_t        bit value
***************************************************************************************************/
uint32_t NWKU_GetFirstBitValue
(
    uint8_t *pArray,
    uint32_t arrayBytes,
    bool_t bitValue
)
{
    return NWKU_GetFirstBitValueInRange(pArray, 0, (arrayBytes * 8), bitValue);
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_GetNumOfBits(uint8_t* pArray, uint32_t arrayBytes, bool_t bitValue);
\brief  This function returns number of bits of value bitValue from an array

\param  [in]    pArray          pointer to the start of the array
\param  [in]    arrayBytes      number of bytes in the array
\param  [in]    bitValue        bit value

\return         uint32_t        bit value
***************************************************************************************************/
uint32_t NWKU_GetNumOfBits
(
    uint8_t *pArray,
    uint32_t arrayBytes,
    bool_t bitValue
)
{
    uint32_t i, j, nrOfBits = 0;

    for (i = 0; i < arrayBytes; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (((pArray[i] >> j) & 0x01) == bitValue)
            {
                nrOfBits++;
            }
        }
    }

    return nrOfBits;
}
/*!*************************************************************************************************
\fn     uint32_t NWKU_ReverseBits(uint32_t num);
\brief  This function returns uint32_t bit in reverse order

\param  [in]    num             uint32_t number

\return         uint32_t        uint32_t number with the bits in reverse order
***************************************************************************************************/
uint32_t NWKU_ReverseBits
(
    uint32_t num
)
{
    uint32_t count = sizeof(num) * 8 - 1;
    uint32_t reverseNum = num;

    num >>= 1;

    while (num)
    {
        reverseNum <<= 1;
        reverseNum |= num & 1;
        num >>= 1;
        count--;
    }

    reverseNum <<= count;
    return reverseNum;
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_AddTblEntry(uint32_t entry, uint32_t *pTable, uint32_t tableSize)
\brief  This function adds a new entry in a table. The table needs to have uint32_t elements.

\param  [in]    entry       entry value
\param  [in]    pTable      pointer to the start of the table
\param  [in]    tableSize   the size of the table

\return         entry index or -1(0xFFFFFFFF) in case of error
***************************************************************************************************/
uint32_t NWKU_AddTblEntry
(
    uint32_t entry,
    uint32_t *pTable,
    uint32_t tableSize
)
{
    uint32_t iEntry = tableSize;
    bool_t status = FALSE;

    for (iEntry = 0; iEntry < tableSize; iEntry++)
    {
        if (!pTable[iEntry])
        {
            pTable[iEntry] = entry;
            status = TRUE;
            break;
        }
    }

    return (status == TRUE) ? iEntry : (uint32_t)(-1);
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_GetTblEntry(uint32_t entry, uint32_t *pTable, uint32_t tableSize)
\brief  This function search for an element in a table.

\param  [in]    entry       entry value
\param  [in]    pTable      pointer to the start of the table
\param  [in]    tableSize   the size of the table

\return         entry index or NULL in case of error
***************************************************************************************************/
uint32_t NWKU_GetTblEntry
(
    uint32_t index,
    uint32_t *pTable,
    uint32_t tableSize
)
{
    return (index < tableSize) ? pTable[index] : 0;
}

/*!*************************************************************************************************
\fn     void NWKU_SwapArrayBytes(uint8_t *pByte, uint8_t numOfBytes)
\brief  This function swaps the bytes in an array and puts the result in the same array.

\param  [in/out]    pByte       pointer to the start of the array
\param  [in]        numOfBytes  size of the array
***************************************************************************************************/
void NWKU_SwapArrayBytes
(
    uint8_t *pByte,
    uint8_t numOfBytes
)
{
    uint8_t tmp, i;

    for (i = 0; i < (numOfBytes >> 1); i++)
    {
        tmp = pByte[i];
        pByte[i] = pByte[numOfBytes - i - 1];
        pByte[numOfBytes - i - 1] = tmp;
    }
}

/*!*************************************************************************************************
\fn     uint32_t NWKU_GetTlvLen(uint8_t type, uint8_t *pStart, uint32_t len)
\brief  This function returns the length of the TLV type specified.

\param  [in]      type    type identifier for the TLV
\param  [in]      pStart  pointer to the start if the TLVs
\param  [in]      len     size of the TLVs buffer

\return uint32_t  tlv length
***************************************************************************************************/
uint32_t NWKU_GetTlvLen
(
    uint8_t type,
    uint8_t *pStart,
    uint32_t len
)
{
    bool_t found = FALSE;
    uint8_t *pBuff = pStart;
    uint32_t res = 0;

    do
    {
        /* Process one TLV */
        {
            if (*pBuff < THR_ALL_FFs8) /* Type field is 1 byte long */
            {
                if (((uint8_t)*pBuff) == type) /* We found the type */
                {
                    found = TRUE;

                    /* Move to the next TLV */
                    pBuff += 1; /* skip type */

                    if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                    {
                        res = *(pBuff);
                    }
                    else /* Length field is 2 bytes long */
                    {
                        pBuff++; /* skip 0xFF */
                        res = ntohas(pBuff);
                    }
                }
                else /* Type was not found */
                {
                    /* Move to the next TLV */
                    pBuff += 1; /* skip type */

                    if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                    {
                        pBuff += (*pBuff); /* Skip Value */
                        pBuff++; /* skip length */
                    }
                    else /* Length field is 2 bytes long */
                    {
                        pBuff++; /* skip 0xFF */
                        pBuff += ntohas(pBuff); /* skip Value field */
                        pBuff += 2; /* skip Length field */
                    }
                }
            }
            else
            {
                /* Only one byte type is supported */
                res = 0;
                break;
            }

        }

        /* Check overflow */
        if (pBuff >= (pStart + len))
        {
            break;
        }
    }
    while (!found);

    return res;
}

/*!*************************************************************************************************
\fn     uint8_t *NWKU_GetTlvValue(uint8_t type, uint8_t *pStart, uint32_t len, uint8_t *pOut)
\brief  This function returns the value of a requested TLV in a list of TLVs. The pointer to the
        value(if found) will be returned and copied in pOut buffer(if pOut is not NULL).

\param  [in]     type    type identifier for the TLV
\param  [in]     pStart  pointer to the start if the TLVs
\param  [in]     len     size of the TLVs buffer
\param  [in]     pOut    pointer to the output preallocated buffer or NULL

\return uint8_t* pointer to the TLV Value, NULL if not found
***************************************************************************************************/
uint8_t *NWKU_GetTlvValue
(
    uint8_t type,
    uint8_t *pStart,
    uint32_t len,
    uint8_t *pOut
)
{
    bool_t found = FALSE;
    uint8_t *pBuff = pStart;
    uint8_t *pRes = NULL;

    do
    {
        if (*pBuff < THR_ALL_FFs8) /* Type field is 1 byte long */
        {
            if (((uint8_t)*pBuff) == type) /* We found the type */
            {
                pBuff++; /* Skip type */
                found = TRUE;

                if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                {
                    if (pOut)
                    {
                        FLib_MemCpy(pOut, pBuff + 1, *pBuff);
                    }

                    pRes = pBuff + 1;
                }
                else /* Length field is 2 bytes long */
                {
                    pBuff++; /* skip 0xFF byte */

                    if (pOut)
                    {
                        FLib_MemCpy(pOut, pBuff + 2, ntohas(pBuff));
                    }

                    pRes = pBuff + 2;
                }
            }
            else /* Type was not found */
            {
                /* Move to the next TLV */
                pBuff++; /* skip type */

                if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                {
                    pBuff += (*pBuff); /* Skip Value */
                    pBuff++; /* skip length */
                }
                else /* Length field is 2 bytes long */
                {
                    pBuff++; /* skip 0xFF */
                    pBuff += ntohas(pBuff); /* skip Value field */
                    pBuff += 2; /* skip Length field */
                }
            }
        }
        else
        {
            /* Only one byte type is supported */
            pRes = NULL;
            break;
        }

        if (pBuff >= (pStart + len))
        {
            break;
        }
    }
    while (!found);

    return pRes;
}

/*!*************************************************************************************************
\fn     uint8_t *NWKU_GetTlv(uint8_t type, uint8_t *pStart, uint32_t len, uint8_t *pOut,
                             uint32_t *pOutLen)
\brief  This function returns the start address of the TLV in the pStart buffer.

\param  [in]    type        Type identifier for the TLV
\param  [in]    pStart      Pointer to the start if the TLVs
\param  [in]    len         Size of the TLVs buffer
\param  [out]   pOut        If this buffer is provided, the found TLVs will be copied inside
\param  [out]   pOutLen     if this variable is provided, the found TLVs length will be copied

\return                     Pointer to the TLV
\return                     NULL if the requested TLV was not found
***************************************************************************************************/
uint8_t *NWKU_GetTlv
(
    uint8_t type,
    uint8_t *pStart,
    uint32_t len,
    uint8_t *pOut,
    uint32_t *pOutLen
)
{
    bool_t found = FALSE;
    uint8_t *pBuff = pStart;
    uint8_t *pRes = NULL;

    if (len)
    {
        if (pOutLen)
        {
            *pOutLen = 0;
        }

        do
        {
            if (*pBuff < THR_ALL_FFs8) /* Type field is 1 byte long */
            {
                if (((uint8_t)*pBuff) == type) /* We found the type */
                {
                    pBuff++; /* Skip type */
                    found = TRUE;
                    pRes = pBuff - 1;

                    if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                    {
                        if (pOutLen)
                        {
                            /* total TLV length */
                            *pOutLen = *pBuff + sizeof(uint16_t);
                        }

                        if (pOut)
                        {
                            FLib_MemCpy(pOut, pBuff - 1, *pBuff + 2);
                        }
                    }
                    else /* Length field is 2 bytes long */
                    {
                        pBuff++; /* skip 0xFF byte */

                        if (pOutLen)
                        {
                            /* total TLV length + 0xFF byte*/
                            *pOutLen = ntohas(pBuff) + sizeof(uint16_t) + 1;
                        }

                        if (pOut)
                        {
                            FLib_MemCpy(pOut, pBuff - 3, ntohas(pBuff) + 3);
                        }
                    }
                }
                else /* Type was not found */
                {
                    /* Move to the next TLV */
                    pBuff++; /* skip type */

                    if (*pBuff < THR_ALL_FFs8) /* Length field is 1 byte long */
                    {
                        pBuff += (*pBuff); /* Skip value */
                        pBuff++; /* skip length */
                    }
                    else /* Length field is 2 bytes long */
                    {
                        pBuff++; /* skip 0xFF */
                        pBuff += ntohas(pBuff); /* skip Value field */
                        pBuff += 2; /* skip Length field */
                    }
                }
            }
            else
            {
                /* Only one byte type is supported */
                pRes = NULL;
                break;
            }

            if (pBuff >= (pStart + len))
            {
                break;
            }
        }
        while (!found);
    }

    return pRes;
}
/*!*************************************************************************************************
\fn     bool_t NWKU_Pbkdf2(pbkdf2Params_t *pInput, uint8_t *pOut, uint32_t outLen)
\brief  This function calculates pbkdf2 for an input.

\param  [in]    pInput  structure containing the input parameters
\param  [in]    pOut    pointer to the output
\param  [in]    outLen  size of the output buffer

\retval         TRUE if the operation has succeeded
                FALSE if the operation hasn't succeeded
***************************************************************************************************/
bool_t NWKU_Pbkdf2
(
    pbkdf2Params_t *pInput,
    uint8_t *pOut,
    uint32_t outLen
)
{
    uint8_t *pSalt;
    uint8_t oBuf[16];
    uint8_t d1[16];
    uint32_t iRounds;
    uint32_t count;
    size_t r;
    bool_t status = FALSE;

    if ((pInput->rounds >= 1) && (outLen != 0) && (pInput->saltLen != 0) &&
            (pInput->saltLen < SIZE_MAX - 4))
    {
        /* pSalt: salt */
        pSalt = pInput->pSalt;

        for (count = 1; outLen > 0; count++)
        {
            /* pSalt: salt || INT_32_BE(count) */
            htonal(&pSalt[pInput->saltLen], count);

            //hmac_sha1(pSalt, pInput->saltLen + 4, pInput->pPass, pInput->passLen, d1);
            //AES_CMAC_PRF_128(pSalt, pInput->saltLen + 4, pInput->pPass, pInput->passLen, d1);

            /* oBuf: PRF(Password, Salt || INT_32_BE(i)) */
            FLib_MemCpy(oBuf, d1, sizeof(oBuf));

            for (iRounds = 1; iRounds < pInput->rounds; iRounds++)
            {
                //hmac_sha1(d1, sizeof(d1), pInput->pPass, pInput->passLen, d2);
                //AES_CMAC_PRF_128(d1, sizeof(d1), pInput->pPass, pInput->passLen, d1);

                /* U1 ^ U2 ^ ... ^ Uc */
                //SecLib_XorN(oBuf, d1, 16);
            }

            r = (outLen < 16) ? outLen : 16;
            FLib_MemCpy(pOut, oBuf, r);
            pOut += r;
            outLen -= r;
        };

        status = TRUE;
    }

    return status;
}

/*!*************************************************************************************************
\fn     NWKU_GetTimestampMs
\brief  Get the timestamp in milliseconds.

\return timestamp in milliseconds
***************************************************************************************************/
uint64_t NWKU_GetTimestampMs
(
    void
)
{
    return (TMR_GetTimestamp() / 1000);
}

/*!*************************************************************************************************
\fn     NWKU_isArrayGreater
\brief  Compare tow numbers represented as array
\param  [in] a first array
\param  [in] b second array
\param  [in] length - how many bytes to compare
\return 0 - are equal
        1 - a > b
        -1 - b < a
***************************************************************************************************/
int8_t NWKU_isArrayGreater
(
    const uint8_t *a,
    const uint8_t *b,
    uint8_t length
)
{
    int i;

    for (i = length - 1; i >= 0; --i)
    {
        if (a[i] > b[i])
        {
            return 1;
        }

        if (a[i] < b[i])
        {
            return -1;
        }
    }

    return 0;
}

/*==================================================================================================
Private functions
==================================================================================================*/

/*==================================================================================================
Private debug functions
==================================================================================================*/
