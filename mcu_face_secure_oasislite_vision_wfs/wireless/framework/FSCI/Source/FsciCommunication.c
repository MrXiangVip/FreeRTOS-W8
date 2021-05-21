/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
*
* \file
*
* This is a source file for the FSCI communication.
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
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_lpuart_freertos.h"
#include "fsl_debug_console.h"
#include "FsciCommunication.h"

#if gFsciIncluded_c
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define MIN_VALID_PACKET_LEN    (sizeof(clientPacketHdr_t))

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static fsci_packetStatus_t FSCI_checkPacket(clientPacket_t *pData, uint16_t bytes, uint8_t *pVIntf);
static uint8_t FSCI_computeChecksum(void *pBuffer, uint16_t size);
static void FSCI_SendPacketToSerialManager(uint32_t fsciInterface, uint8_t *pPacket, uint16_t packetLen);

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern lpuart_rtos_handle_t handle;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static pfnFsciCb mValidPacketCb = NULL;
static clientPacket_t gFsciPacket;
static fsciLen_t mFsciBytes;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Add a callback to apply on valid received FSCI packets.
*
* \param[in]  pfnFsciCb     user callback
*
********************************************************************************** */
void FSCI_RegisterCb(pfnFsciCb cb)
{
    mValidPacketCb = cb;
}

/*! *********************************************************************************
* \brief  Receives data from the serial interface and checks to see if we have a
          valid packet.

* \return serial operation status
********************************************************************************** */
int FSCI_receivePacket(void)
{
    int error;
    uint8_t c, vintf;
    size_t received;
    fsci_packetStatus_t status;

    error = LPUART_RTOS_Receive(&handle, &c, sizeof(c), &received);
    //PRINTF("[R]:[%0x]\r\n", c);
    if ((kStatus_Success == error) && (received > 0))
    {
        /* Wait for start marker */
        if (0 == mFsciBytes)
        {
            if (c == gFSCI_StartMarker_c)
            {
                gFsciPacket.raw[mFsciBytes++] = c;
            }
        }
        /* FSCI packet reception is in progress */
        else
        {
            gFsciPacket.raw[mFsciBytes++] = c;

            status = FSCI_checkPacket(&gFsciPacket, mFsciBytes, &vintf);

            switch (status)
            {
                case PACKET_IS_TOO_SHORT:
                    /* wait for more data to arrive */
                    break;

                case PACKET_IS_VALID:
                    if (mValidPacketCb)
                    {
                        mValidPacketCb(&gFsciPacket, vintf);
                    }

                    mFsciBytes = 0;
                    break;

                case FRAMING_ERROR:
                default:
                    mFsciBytes = 0;
                    break;
            }
        }
    }

    return error;
}

/*! *********************************************************************************
* \brief  Encode and send messages over the serial interface
*
* \param[in] OG operation Group
* \param[in] OC operation Code
* \param[in] pMsg pointer to payload
* \param[in] msgLen length of the payload
* \param[in] fsciInterface the interface on which the packet should be sent
*
********************************************************************************** */
void FSCI_transmitPayload(uint8_t OG, uint8_t OC, void *pMsg, uint16_t msgLen, uint32_t fsciInterface)
{
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_size, index;
    uint8_t checksum, checksum2;
    clientPacketHdr_t header;

    //LOGD("[Send]:[%x%x] [%d]\r\n", OG, OC, fsciInterface);
    if (msgLen > gFsciMaxPayloadLen_c)
    {
        return;
    }

    /* Compute size */
    buffer_size = sizeof(clientPacketHdr_t) + msgLen + 2 * sizeof(checksum);

    /* Allocate buffer */
    buffer_ptr = MEM_BufferAlloc(buffer_size);

    if (NULL == buffer_ptr)
    {
        return;
    }

    /* Message header */
    header.startMarker = gFSCI_StartMarker_c;
    header.opGroup = OG;
    header.opCode = OC;
    header.len = msgLen;

    /* Compute CRC for TX packet, on opcode group, opcode, payload length, and payload fields */
    checksum = FSCI_computeChecksum((uint8_t *)&header + 1, sizeof(header) - 1);
    checksum ^= FSCI_computeChecksum((uint8_t *)pMsg, msgLen);

    if (fsciInterface)
    {
        checksum2 = checksum ^ (checksum + fsciInterface);
        checksum += fsciInterface;
    }

    index = 0;
    FLib_MemCpy(&buffer_ptr[index], &header, sizeof(header));
    index += sizeof(header);
    FLib_MemCpy(&buffer_ptr[index], pMsg, msgLen);
    index += msgLen;
    /* Store the Checksum */
    buffer_ptr[index++] = checksum;

    if (fsciInterface)
    {
        buffer_ptr[index++] = checksum2;
    }

    /* send message to Serial Manager */
    FSCI_SendPacketToSerialManager(fsciInterface, buffer_ptr, index);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Checks to see if we have a valid packet
*
* \param[in] pData The message containing the incoming data packet to be handled.
* \param[in] bytes the number of bytes inside the buffer
* \param[Out] pVIntf pointer to the location where the virtual interface Id will be stored
*
* \return the status of the packet
*
********************************************************************************** */
static fsci_packetStatus_t FSCI_checkPacket(clientPacket_t *pData, uint16_t bytes, uint8_t *pVIntf)
{
    uint8_t checksum = 0;
    uint16_t len;

    if (bytes < MIN_VALID_PACKET_LEN)
    {
        return PACKET_IS_TOO_SHORT;            /* Too short to be valid. */
    }

    if (bytes >= sizeof(clientPacket_t))
    {
        return FRAMING_ERROR;
    }

    if (NULL == pData)
    {
        return FSCI_INTERNAL_ERROR;
    }

    /* The packet's len field does not count the STX, the opcode group, the */
    /* opcode, the len field, or the checksum. */
    len = pData->structured.header.len;

    /* If the length appears to be too long, it might be because the external */
    /* client is sending a packet that is too long, or it might be that we're */
    /* out of sync with the external client. Assume we're out of sync. */
    if (len > gFsciMaxPayloadLen_c)
    {
        return FRAMING_ERROR;
    }

    if (bytes < len + sizeof(clientPacketHdr_t) + sizeof(checksum))
    {
        return PACKET_IS_TOO_SHORT;
    }

    /* If the length looks right, make sure that the checksum is correct. */
    if (bytes >= len + sizeof(clientPacketHdr_t) + sizeof(checksum))
    {
        checksum = FSCI_computeChecksum(pData->raw + 1, len + sizeof(clientPacketHdr_t) - 1);
        *pVIntf = pData->structured.payload[len] - checksum;
    }

    if (bytes == len + sizeof(clientPacketHdr_t) + sizeof(checksum))
    {
        if (0 == *pVIntf)
        {
            return PACKET_IS_VALID;
        }

#if gFsciMaxVirtualInterfaces_c
        else
        {
            if (*pVIntf < gFsciMaxVirtualInterfaces_c)
            {
                return PACKET_IS_TOO_SHORT;
            }
        }

#endif
    }

#if gFsciMaxVirtualInterfaces_c

    /* Check virtual interface */
    if (bytes == len + sizeof(clientPacketHdr_t) + 2 * sizeof(checksum))
    {
        checksum ^= checksum + *pVIntf;

        if (pData->structured.payload[len + 1] == checksum)
        {
            return PACKET_IS_VALID;
        }
    }

#endif

    return FRAMING_ERROR;
}

/*! *********************************************************************************
* \brief  This function performs a XOR over the message to compute the CRC
*
* \param[in]  pBuffer - pointer to the message
* \param[in]  size - the length of the message
*
* \return  the CRC of the message
*
********************************************************************************** */
static uint8_t FSCI_computeChecksum(void *pBuffer, uint16_t size)
{
    uint16_t index;
    uint8_t  checksum = 0;

    for (index = 0; index < size; index++)
    {
        checksum ^= ((uint8_t *)pBuffer)[index];
    }

    return checksum;
}

/*! *********************************************************************************
* \brief  This function is used to send a FSCI packet to the serial manager
*
* \param[in]  fsciInterface     FSCI interface on which the packet is to be sent
* \param[in]  pPacket           serial packet to be sent
* \param[in]  packetLen         length of the serial packet in bytes
*
********************************************************************************** */
static void FSCI_SendPacketToSerialManager(uint32_t fsciInterface, uint8_t *pPacket, uint16_t packetLen)
{
    int rc = LPUART_RTOS_Send(&handle, pPacket, packetLen);

    if (rc != kStatus_Success)
    {
        PRINTF("LPUART_RTOS_Send: fail status %d\r\n", rc);
    }

    MEM_BufferFree(pPacket);
}

#endif /* gFsciIncluded_c */
