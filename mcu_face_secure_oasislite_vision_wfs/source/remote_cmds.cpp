/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "remote_cmds.h"
#include "database.h"
#include "oasis.h"
#include "sln_shell.h"
#include "sln_vizn_api.h"
#include "cJSON.h"
#include "fsl_log.h"

#include <vector>
#include <string>

extern "C"
{
    int WicedBLE_Send(remote_frame_cmd_t cmd, uint8_t *str, int len);
}

/*this structure is bind with part of FeatureItem, do editing until you know what you are doing*/
typedef struct{
    unsigned char id[4];
    char name[FEATUREDATA_NAME_MAX_LEN];
    //it's length is dynamic,so we defined it as 0
    unsigned char feature[0];
} FaceString_t;

/* remote control payload json */
#define RC_PL_USERNAME "username"
#define RC_PL_EMAIL    "email"
#define RC_PL_FEATURE  "feature"
#define RC_PL_OP       "op"
#define RC_PL_ID       "id"
#define RC_PL_SUCCESS  "success"
#define RC_PL_FEATURE_TABLE "featureTable"

#define RC_PL_UPDATE_RECORD_ADD "add"
#define RC_PL_UPDATE_RECORD_DELETE "delete"

typedef enum {
    UPDATE_RECORD_ADD = 1,
    UPDATE_RECORD_DELETE = 2,
    UPDATE_RECORD_UNKNOWN = -1,
} update_record_op_t;


static void Remote_convertAscii2int(unsigned char* ascii,int len,void* value)
{
    unsigned char nibble[8];
    unsigned char *value_char = (unsigned char *)value;

    assert(len == 4 || len == 8);

    for (int j = 0; j < len; j++)
    {
       if (ascii[j] <= 'f' && ascii[j] >= 'a')
       {
           nibble[j] = ascii[j] - 'a' + 10;
       }
       else if (ascii[j] <= '9' && ascii[j] >= '0')
       {
           nibble[j] = ascii[j] - '0';
       }
       else
       {
           nibble[j] = 0;
       }
    }

    value_char[0]  = nibble[0] << 4 | nibble[1];
    value_char[1]  = nibble[2] << 4 | nibble[3];
    if (len == 8)
    {
        value_char[2]  = nibble[4] << 4 | nibble[5];
        value_char[3]  = nibble[6] << 4 | nibble[7];
    }
}

static void Remote_convertInt2ascii(void* value, int bytes, unsigned char* ascii)
{
    unsigned char nibble;
    unsigned char hex_table[] = "0123456789abcdef";
    unsigned char *p_value = (unsigned char *)value;
    unsigned char *p_ascii = ascii;

    for (int j = 0; j < bytes; j++)
    {
        nibble = *p_value++;
        int low = nibble & 0x0f;
        int high = (nibble>>4) & 0x0f;
        *p_ascii++ = hex_table[high];
        *p_ascii++ = hex_table[low];
    }

}

static int32_t compareString(const char *str1, const char *str2, int32_t count)
{
    while (count--)
    {
        if (*str1++ != *str2++)
        {
            return *(unsigned char *)(str1 - 1) - *(unsigned char *)(str2 - 1);
        }
    }
    return 0;
}
/*!
 * @brief remote face registration request callback
 *
 * @param buf   the remote face registration request packet.
 * @param len   the length of the remote face registration request packet.
 * @return 0 success else error code of the failure.
 */
int Remote_FaceAdd(char *buf, int len)
{
    int ret = 0;

    FaceString_t *facestr = (FaceString_t*)buf;

    //each byte in face need 2 bytes because of ASCII code
    /*Format is: ID(2*2 bytes) + name(FEATUREDATA_NAME_MAX_LEN bytes) + face feature(FEATUREDATA_FEATURE_SIZE*4*2 bytes)*/
    // TODO:
    if ((unsigned int)len < sizeof(FaceString_t) + FEATUREDATA_FEATURE_SIZE * 4 * 2)
    {
        return -1;
    }

    facestr->name[FEATUREDATA_NAME_MAX_LEN - 1] = 0;

    for (int i = 0; i < FEATUREDATA_NAME_MAX_LEN; i++)
    {
        if (facestr->name[i] == 0x0a)
        {
            facestr->name[i] = 0;
        }
    }

    std::string name = facestr->name;

    UsbShell_Printf("[Add:][%s]\r\n", name.c_str());

    //get the id
    uint16_t id_remote, id_local;

    ret = DB_GenID(&id_local);
    if(ret < 0)
        return -1;
    Remote_convertAscii2int(facestr->id,sizeof(facestr->id),&id_remote);

    //cmp ids between remote and local.
    if(id_local != id_remote)
    {
        UsbShell_Printf("[remote id error!][id_local]: [id_remote]\r\n", id_local, id_remote);
        //return -1;
    }

    unsigned char *pFeature = facestr->feature;
    // TODO:
    float* feature = (float*)pvPortMalloc(OASISLT_getFaceItemSize());
    if(feature == NULL){
        return -1;
    }

    float fValue;
    for (unsigned int i = 0; i < FEATUREDATA_FEATURE_SIZE; i++)
    {
        Remote_convertAscii2int(pFeature,sizeof(fValue)*2,&fValue);
        pFeature += sizeof(fValue)*2;
        feature[i]=fValue;
    }

    ret = DB_Add(id_local,name, feature);
	vPortFree(feature);

    return ret;
}

/*!
 * @brief remote registration button press request callback
 *
 * @param buf   the remote registration button press request packet.
 * @param len   the length of the remote registration button press request packet.
 * @return 0 success else error code of the failure.
 */
int Remote_KeyAdd(char *buf, int len)
{
    vizn_api_status_t status;

    if (len < FEATUREDATA_NAME_MAX_LEN)
    {
        return -1;
    }

    buf[FEATUREDATA_NAME_MAX_LEN - 1] = 0;

    for (int i = 0; i < FEATUREDATA_NAME_MAX_LEN; i++)
    {
        if (buf[i] == 0x0a)
        {
            buf[i] = 0;
        }
    }

    UsbShell_Printf("[keyAdd:][%s]\r\n", buf);
    status = VIZN_AddUser(NULL, buf);
    switch (status)
    {
        case kStatus_API_Layer_AddUser_NoAddCommand:
            UsbShell_Printf("No add command registered\r\n");
            break;
        case kStatus_API_Layer_AddUser_AddCommandStopped:
            UsbShell_Printf("Stopped adding %s \r\n");
            break;
        case kStatus_API_Layer_AddUser_InvalidUserName:
            UsbShell_Printf("Invalid User Name\r\n");
            break;
        case kStatus_API_Layer_AddUser_MultipleAddCommand:
            UsbShell_Printf(
                "Add command already in pending. Please complete the pending registration or stop the adding\r\n");
            break;
        case kStatus_API_Layer_Success:
            UsbShell_Printf("Start registering '%s'\r\n", buf);
            break;
        default:
            UsbShell_Printf("ERROR API ENGINE");
            break;
    }

    return status;
}

int Remote_command_configuration_get_request(const char *payload_str)
{
//    UsbShellCmdQueue_t queueMsg;
    return 0;
}

int Remote_command_face_record_get_request(const char *payload_str)
{
//    cJSON *payload_json = cJSON_Parse((const char*)payload_str);
//    if (payload_json == NULL) {
//        LOGE("payload json parse error.\r\n");
//        goto end;
//    }

    //get ids
    std::vector<uint16_t> allIDs;
    DB_GetIDs(allIDs);

    float *face = (float *)pvPortMalloc(OASISLT_getFaceItemSize());
    uint8_t *hex_str = (uint8_t *)pvPortMalloc(OASISLT_getFaceItemSize()*2+1);
    hex_str[OASISLT_getFaceItemSize()*2] = '\0';

    cJSON *features_json = cJSON_CreateObject();
    {
        if (features_json == NULL) {
            goto end;
        }
        cJSON *array_json = cJSON_CreateArray();
        if (array_json == NULL) {
            goto end;
        }

        cJSON_AddItemToObject(features_json, RC_PL_FEATURE_TABLE, array_json);

        for(uint32_t i = 0; i < allIDs.size(); i++)
        {
            cJSON *object_json = cJSON_CreateObject();
            if (object_json == NULL) {
                goto end;
            }

            if (cJSON_AddNumberToObject(object_json, RC_PL_ID, allIDs[i]) == NULL) {
                cJSON_Delete(object_json);
                goto end;
            }

            string name;
            DB_GetName(allIDs[i], name);
            if (cJSON_AddStringToObject(object_json, RC_PL_USERNAME, name.c_str()) == NULL) {
                cJSON_Delete(object_json);
                goto end;
            }

            DB_GetFeature(allIDs[i], face);
            Remote_convertInt2ascii(face, OASISLT_getFaceItemSize(), hex_str);
            if (cJSON_AddStringToObject(object_json, RC_PL_FEATURE, (const char*)hex_str) == NULL) {
                cJSON_Delete(object_json);
                goto end;
            }

            cJSON_AddItemToArray(array_json, object_json);

        }

        if (cJSON_AddBoolToObject(features_json, RC_PL_SUCCESS, true) == NULL){
            goto end;
        }

        char *json_str = cJSON_Print(features_json);
//        LOGD((json_str));
        WicedBLE_Send(FACE_RECORD_GET_RESPONSE, (uint8_t *)json_str, strlen(json_str));

        cJSON_free(json_str);
    }

end:
    cJSON_Delete(features_json);
    vPortFree(face);
    vPortFree(hex_str);
    return 0;

}

int Remote_command_face_record_update_get_op(const char *op_str)
{
    if (compareString(op_str, RC_PL_UPDATE_RECORD_ADD, sizeof(RC_PL_UPDATE_RECORD_ADD)) == 0)
            return UPDATE_RECORD_ADD;
    else if(compareString(op_str, RC_PL_UPDATE_RECORD_DELETE, sizeof(RC_PL_UPDATE_RECORD_DELETE)) == 0)
            return UPDATE_RECORD_DELETE;
    else
        return UPDATE_RECORD_UNKNOWN;
}

int Remote_command_face_record_update_request(const char *payload_str)
{
    int ret = -1;
    char *json_str;

    cJSON *payload_json = cJSON_Parse((const char*)payload_str);
    {
        if (payload_json == NULL) {
            goto end;
        }

        cJSON *op_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_OP);
        if (op_json == NULL) {
            goto end;
        }

        switch (Remote_command_face_record_update_get_op(op_json->valuestring)){
            case UPDATE_RECORD_ADD:
            {
                cJSON *id_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_ID);
                if (id_json == NULL) {
                    goto end;
                }

                cJSON *username_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_USERNAME);
                if (username_json == NULL) {
                    goto end;
                }

//                cJSON *email_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_EMAIL);
//                if (email_json == NULL) {
//                    goto end;
//                }

                cJSON *feature_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_FEATURE);
                if (feature_json == NULL) {
                    goto end;
                }

                unsigned char *pFeature = (unsigned char *)feature_json->valuestring;
                float* feature = (float*)pvPortMalloc(OASISLT_getFaceItemSize());
                if(feature == NULL){
                    goto end;
                }

                float fValue;
                for (unsigned int i = 0; i < FEATUREDATA_FEATURE_SIZE; i++)
                {
                    Remote_convertAscii2int(pFeature, sizeof(fValue)*2, &fValue);
                    pFeature += sizeof(fValue)*2;
                    feature[i]=fValue;
                }

                //get the id
                uint16_t id_remote, id_local;

                id_remote = id_json->valuedouble;
                ret = DB_GenID(&id_local);
                if(ret < 0) {
                    vPortFree(feature);
                    goto db_end;
                }

                //cmp ids between remote and local.
                if(id_local != id_remote)
                {
                    UsbShell_Printf("remote id different with local, [r]%d [l]%d\r\n", id_remote, id_local);
                    vPortFree(feature);
                    goto db_end;
                }

                string name = username_json->valuestring;
                ret = DB_Add(id_local, name, feature);
                if (ret == DB_MGMT_FAILED)
                {
                    UsbShell_Printf("remote add user into local fail, [%d]: [%s]\r\n", id_remote, name.c_str());
                    vPortFree(feature);
                    goto db_end;
                }

                UsbShell_Printf("remote register [%d][%s] success.\r\n", id_local, name.c_str());
                vPortFree(feature);
                break;
            }

            case UPDATE_RECORD_DELETE:
            {
                string name;

                cJSON *id_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_ID);
                if (id_json == NULL) {
                    goto end;
                }

                cJSON *username_json = cJSON_GetObjectItemCaseSensitive(payload_json, RC_PL_USERNAME);
                if (username_json == NULL) {
                    goto end;
                }

                ret = DB_GetName(id_json->valuedouble, name);
                if (ret == DB_MGMT_FAILED) {
                    LOGD("remote delete user failed.\r\n");
                    goto db_end;
                }

                if (compareString(name.c_str(), username_json->valuestring, strlen(username_json->valuestring)) != 0) {
                    UsbShell_Printf("remote database different with local [%d]:[r]%s [l]%s\r\n", id_json->valueint, username_json->valuestring, name.c_str());
                    goto db_end;
                }

                name = username_json->valuestring;
                int ret = DB_Del(name);
                if (ret == DB_MGMT_FAILED) {
                    LOGD("remote delete user failed.\r\n");
                    goto db_end;
                }
                UsbShell_Printf("remote delete user[%s] success.\r\n", name.c_str());
                break;
            }

            default:
                break;

        }
    }

    if (cJSON_AddBoolToObject(payload_json, RC_PL_SUCCESS, true) == NULL) {
        goto end;
    }

    json_str = cJSON_Print(payload_json);
    if(json_str == NULL) {
        goto end;
    }
    WicedBLE_Send(FACE_RECORD_UPDATE_RESPONSE, (uint8_t *)json_str, strlen(json_str));

    cJSON_free(json_str);

    return 0;

db_end:
    if (cJSON_AddBoolToObject(payload_json, RC_PL_SUCCESS, false) == NULL) {
        goto end;
    }

    json_str = cJSON_Print(payload_json);
    if(json_str == NULL) {
        goto end;
    }

    WicedBLE_Send(FACE_RECORD_UPDATE_RESPONSE, (uint8_t *)json_str, strlen(json_str));

    cJSON_free(json_str);

    return -2;

end:
    cJSON_Delete(payload_json);
    LOGD("remote json parse error.\r\n");
    return -1;


}
