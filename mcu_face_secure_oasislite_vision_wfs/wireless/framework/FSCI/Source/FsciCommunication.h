/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
*
* \file
*
* This is the private header file for the FSCI communication module.
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


#ifndef _FSCI_COMMUNICATION_H_
#define _FSCI_COMMUNICATION_H_


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "fsl_log.h"
/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#ifndef gFsciIncluded_c
    #define gFsciIncluded_c                 1 /* Enable/Disable FSCI module */
#endif

#ifndef gFsciMaxPayloadLen_c
    #define gFsciMaxPayloadLen_c            1536 /* bytes */
#endif

#ifndef gFsciLenHas2Bytes_c
    #define gFsciLenHas2Bytes_c             1 /* boolean */
#endif

#ifndef gFsciMaxVirtualInterfaces_c
    #define gFsciMaxVirtualInterfaces_c     0
#endif

/* Additional bytes added by FSCI to a packet */
#if gFsciMaxVirtualInterfaces_c
    #define gFsci_TailBytes_c               2
#else
    #define gFsci_TailBytes_c               1
#endif


/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef uint8_t clientPacketStatus_t;

typedef uint8_t opGroup_t;

typedef uint8_t opCode_t;

#if gFsciLenHas2Bytes_c
    typedef uint16_t fsciLen_t;
#else
    typedef uint8_t fsciLen_t;
#endif

typedef enum
{
    PACKET_IS_VALID,
    PACKET_IS_TOO_SHORT,
    FRAMING_ERROR,
    FSCI_INTERNAL_ERROR
} fsci_packetStatus_t;

/* Format of packets exchanged between the external client and FSCI. */
typedef PACKED_STRUCT clientPacketHdr_tag
{
    uint8_t    startMarker;
    opGroup_t  opGroup;
    opCode_t   opCode;
    fsciLen_t  len;      /* Actual length of payload[] */
} clientPacketHdr_t;

/* The terminal checksum is actually stored at payload[len]. The additional
   gFsci_TailBytes_c bytes insures that there is always space for it, even
   if the payload is full. */
typedef PACKED_STRUCT clientPacketStructured_tag
{
    clientPacketHdr_t header;
    uint8_t payload[gFsciMaxPayloadLen_c + gFsci_TailBytes_c];
} clientPacketStructured_t;

/* defines the working packet type */
typedef PACKED_UNION clientPacket_tag
{
    /* The entire packet as unformatted data. */
    uint8_t raw[sizeof(clientPacketStructured_t)];
    /* The packet as header + payload. */
    clientPacketStructured_t structured;
    /* A minimal packet with only a status value as a payload. */
    PACKED_STRUCT
    {                              /* The packet as header + payload. */
        clientPacketHdr_t header;
        clientPacketStatus_t status;
    } headerAndStatus;
} clientPacket_t;

#if defined(__cplusplus)
extern "C" {
#endif


typedef void (*pfnFsciCb)(clientPacket_t *pFsciPacket, uint8_t vintf);

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define gFSCI_StartMarker_c     0x02

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
void FSCI_RegisterCb(pfnFsciCb cb);
int FSCI_receivePacket(void);
void FSCI_transmitPayload(uint8_t OG, uint8_t OC, void *pMsg, uint16_t msgLen, uint32_t fsciInterface);

#if defined(__cplusplus)
}
#endif

#endif /* _FSCI_COMMUNICATION_H_ */
