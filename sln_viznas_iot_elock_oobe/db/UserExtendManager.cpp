//
// Created by xshx on 2022/5/18.
//

#include "UserExtendManager.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include "board_rt106f_elock.h"

#include "fsl_log.h"
#include "cJSON.h"
#include "util.h"
static  const char * logtag="[UserExtendManager] ";
UserExtendManager* UserExtendManager::m_instance = NULL;
uint32_t UserExtendManager::userExtend_FS_Head = NULL;

UserExtendManager::UserExtendManager() {
    // Create a database lock semaphore
    userExtend_FS_Head = USER_EXTEND_FS_ADDR;
    LOGD("%s Init UserExtendManager \r\n",logtag);
}

UserExtendManager* UserExtendManager::getInstance()
{
    LOGD("%s getInstance UserExtendManager \r\n", logtag);

    if(NULL == m_instance)
    {
        m_instance = new UserExtendManager();
    }
    return m_instance;
}

int UserExtendManager::get_free_index(){

    int item_max = MAX_EXTEND_COUNT;
    // find new map index
    for (int i = 0; i < item_max; i++)
    {
        UserExtend userExtend;
        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+i*sizeof(UserExtend), (uint8_t *)&userExtend, sizeof(UserExtend)  );

        if( status == 0 ){
            if ( strcmp( userExtend.UUID , UUID_MAGIC_UNUSE) !=0)
                return i;
        }else{
            LOGD("error \n");
        }
    }

    return -1;
}

int UserExtendManager::get_index_by_uuid(char *uuid) {
    LOGD("%s get_index_by_uuid %s \r\n",logtag, uuid);
    for( int i=0; i<MAX_EXTEND_COUNT; i++){

        UserExtend userExtend;
        memset( &userExtend, 0, sizeof(UserExtend));
        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+i*sizeof(UserExtend), (uint8_t *)&userExtend, sizeof(UserExtend)  );
        if( status == 0 ){
//            LOGD("%s %i, %s, %s \r\n", logtag, i, userExtend.UUID, userExtend.jsonData);
            LOGD("%s index %i \r\n", logtag, i );
            if( strcmp( userExtend.UUID, uuid) == 0 ){
                return  i;
            }
        }

    }
    return -1;
}

int UserExtendManager::addUserExtend(UserExtend * userExtend){
    LOGD("%s addUserExtend  %s\r\n", logtag, userExtend->UUID);
//  1.查询uuid 是否已经存在
    int index = get_index_by_uuid(userExtend->UUID);

//  2. 存在则更新， 不存在则新增
    if( index ==-1 ){
        LOGD("%s uuid 不存在,则新增 \r\n");
        index = get_free_index();
        if( index == -1 ){
            LOGD("Error: Database is full \n");
            return -1;
        }
        int status = SLN_Write_Sector(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend);
        if (status != 0) {
            LOGD("write flash failed %d \r\n", status);
            return  -1;
        }
        return  index;
    }else{
        LOGD("%s uuid 已存在,则更新 \r\n", logtag);
        int status = SLN_Write_Sector(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend );
        if (status != 0) {
            LOGD("write flash failed %d \r\n", status);
            return  -1;
        }
    }

    return  0;
}
//input : uuid
//output: userExtend
int  UserExtendManager::queryUserExtendByUUID( char *uuid, UserExtend *userExtend){
    LOGD("%s queryUserExtendByUUID  %s\r\n",logtag, uuid);
    int status =-1;
    int index = get_index_by_uuid(uuid);
    if( index != -1 ){

        status = SLN_Read_Flash_At_Address(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend, sizeof(UserExtend));
        if (status != 0) {
            LOGD("read flash failed %d \r\n", status);
        }
    }
    return  status;
}
//
int UserExtendManager::updateUserExtendByUUID( char *uuid, UserExtend *userExtend){
    LOGD("%s updateUserExtendByUUID  %s\r\n",logtag, uuid);
    int status =-1;
    int index = get_index_by_uuid( uuid );
    if( index != -1 ){
        status = SLN_Write_Sector(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend );
        if (status != 0) {
            LOGD("read flash failed %d \r\n", status);
            return  -1;
        }
    }
    return  status;
}

int UserExtendManager::delUserExtendByUUID( char *uuid ){
    LOGD("%s delUserExtendByUUID  %s\r\n",logtag, uuid);
    int status=-1;
    int index = get_index_by_uuid(uuid );
    if( index != -1 ){
        status = SLN_Erase_Sector( userExtend_FS_Head+index*sizeof(UserExtend));
        if( status !=0 ){
            LOGD("erase flash failed %d \r\n", status);
        }
    }
    return  status;
}

int UserExtendManager::clearAllUserExtend(  ){
    LOGD("%s clearAllUserExtend  %s\r\n",logtag);
    int status=-1;
    for( int i =0; i<MAX_EXTEND_COUNT; i++ ){
        status = SLN_Erase_Sector( userExtend_FS_Head+i*sizeof(UserExtend));
        if( status !=0 ){
            LOGD("erase flash failed %d \r\n", status);
        }
    }
    return  status;
}


//
void vConvertUserExtendType2Json(UserExtendType *userExtendType, UserExtend  *userExtend){
    LOGD("%s 转换用户扩展类为 json \r\n",logtag );

    char 	*cjson_str;
    cJSON * cObj = cJSON_CreateObject();
    cJSON_AddStringToObject(cObj, UERID,  userExtendType->UUID);
    cJSON_AddNumberToObject(cObj, TIMES, userExtendType->uStartTime);
    cJSON_AddNumberToObject(cObj, TIMEE, userExtendType->uEndTime);
    cJSON_AddStringToObject(cObj, ADEV, userExtendType->cDeviceId);
    cjson_str = cJSON_PrintUnformatted(cObj);

    LOGD("%s cjson %s \r\n",logtag, cjson_str);
    memcpy( userExtend->UUID, userExtendType->UUID, sizeof(userExtendType->UUID));
    memcpy( userExtend->jsonData, cjson_str, strlen(cjson_str));
    LOGD("UserExtend UUID:%s \r\n", userExtend->UUID);
    LOGD("UserExtend jsonData:%s \r\n", userExtend->jsonData);
};

void vConverUserExtendJson2Type(UserExtend  *userExtend,  UserExtendType *userExtendType){
    LOGD("%s 转换json 为用户扩展类 \r\n",logtag );
    cJSON *jsonObj = cJSON_Parse(userExtend->jsonData);
    strcpy(userExtendType->UUID, cJSON_GetObjectItem(jsonObj, UERID)->valuestring);
    StrToHex( userExtendType->HexUID, userExtendType->UUID, sizeof(userExtendType->HexUID));//将uuid 转成16进制 hexuid
    userExtendType->uStartTime = cJSON_GetObjectItem(jsonObj, TIMES)->valuedouble;
    userExtendType->uEndTime = cJSON_GetObjectItem(jsonObj, TIMEE)->valuedouble;
    strcpy(userExtendType->cDeviceId , cJSON_GetObjectItem(jsonObj, ADEV)->valuestring);
    LOGD( "%s UUID %s ,StartTime %d ,EndTime %d ,Device %s \r\n", logtag, userExtendType->UUID, userExtendType->uStartTime, userExtendType->uEndTime, userExtendType->cDeviceId);
}

