/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _REMOTE_CMD_H_
#define _REMOTE_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

typedef enum {
    READY_INDICATION                = 0x00,
    AUTHENTICATION_REQUEST          = 0x02,
    AUTHENTICATION_RESPONSE               ,
    PASSWORD_REQUEST                = 0x04,
    PASSWORD_RESPONSE                     ,
    CONFIGURATION_GET_REQUEST       = 0x06,
    CONFIGURATION_GET_RESPONSE            ,
    CONFIGURATION_UPDATE_REQUEST    = 0x08,
    CONFIGURATION_UPDATE_RESPONSE         ,
    FACE_RECORD_GET_REQUEST         = 0x0A,
    FACE_RECORD_GET_RESPONSE              ,
    FACE_RECORD_UPDATE_REQUEST      = 0x0C,
    FACE_RECORD_UPDATE_RESPONSE           ,
    OPEN_DOOR_RESPONSE              = 0x5A,
    OPEN_DOOR_REQUEST               = 0xA5,
    UNKNOW_COMMAND                  = 0xFF,
} remote_frame_cmd_t;

/* JSON Items */

/* frame header region */
#define REMOTE_CTRL_FRAME_HEADER    "FH"
#define REMOTE_CTRL_PREFIX          "PRE"
#define REMOTE_CTRL_TIME_STAMP      "TS"
#define FH_VERSION                  "VER"
#define FH_FLAG                     "CIP"
#define REMOTE_CTRL_LENGTH          "LEN"

#define REMOTE_CTRL_COMMAND         "CMD"
#define REMOTE_CTRL_PAYLOAD         "PL"
#define REMOTE_CTRL_CRC             "CRC"


/* command region */
#define MAX_PAYLOAD_LENGTH  "max_pl_len"
#define AUTHENTICATION      "auth"
#define PASSWORD            "new_pwd"
//#define ""

typedef struct {
    char     prefix[4];
    int      timestamp;
    uint8_t  version;
    uint8_t  cipher;
    uint16_t length;
} remote_frame_header_t;

typedef struct {
    remote_frame_header_t frame_header;
    remote_frame_cmd_t    command;
    void *                payload;
    int                   crc;
} remote_frame_struct_t;

/*!
 * @brief remote face registration request callback
 *
 * @param buf   the remote face registration request packet.
 * @param len   the length of the remote face registration request packet.
 * @return 0 success else error code of the failure.
 */
int Remote_FaceAdd(char *buf, int len);

/*!
 * @brief remote registration button press request callback
 *
 * @param buf   the remote registration button press request packet.
 * @param len   the length of the remote registration button press request packet.
 * @return 0 success else error code of the failure.
 */
int Remote_KeyAdd(char *buf, int len);

int Remote_command_configuration_get_request(const char *payload_str);

int Remote_command_face_record_get_request(const char *payload_str);

int Remote_command_face_record_update_request(const char *payload_str);

#ifdef __cplusplus
}
#endif

#endif /*_REMOTE_CMD_H_*/
