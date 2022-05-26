//
// Created by xshx on 2022/5/18.
//

#include "UserExtendManager.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include "board_rt106f_elock.h"

#include "fsl_log.h"


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
        LOGD("%s %i, %s, %s \r\n", logtag, i, userExtend.UUID, userExtend.jsonData);
        if( status == 0 ){
            if( strcmp( userExtend.UUID, uuid) == 0 ){
                return  i;
            }
        }

    }
    return -1;
}

int UserExtendManager::addUserExtend(UserExtend * userExtend){
    LOGD("%s addUserExtend  %s\r\n", logtag, userExtend->UUID);
    int index = get_free_index();
    if( index == -1 ){
        LOGD("Error: Database is full \n");
    }
    int status = SLN_Write_Sector(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend);

    if (status != 0) {
        LOGD("write flash failed %d \r\n", status);
    }
    return  status;
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

int UserExtendManager::updateUserExtendByUUID( char *uuid, UserExtend *userExtend){
    LOGD("%s updateUserExtendByUUID  %s\r\n",logtag, uuid);
    int status =-1;
    int index = get_index_by_uuid( uuid );
    if( index != -1 ){
        status = SLN_Write_Sector(userExtend_FS_Head+index* sizeof(UserExtend), (uint8_t *)userExtend );
        if (status != 0) {
            LOGD("read flash failed %d \r\n", status);
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
