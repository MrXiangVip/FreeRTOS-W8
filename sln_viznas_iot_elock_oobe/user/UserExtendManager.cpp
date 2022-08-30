//
// Created by xshx on 2022/5/18.
//

#include "UserExtendManager.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "board_rt106f_elock.h"

#include "fsl_log.h"
#include "cJSON.h"
#include "util.h"
#include "MCU_UART5_Layer.h"

static  const char * logtag="[UserExtendManager]: ";

//全局的用户 包含的数据集合

UserExtendManager*  UserExtendManager::m_instance = NULL; //用户管理类的实例
UserExtendClass      UserExtendManager::gUserExtend ; // 表示一个全局的用户
uint32_t            UserExtendManager::userExtend_FS_Head = NULL;//头地址

typedef enum {
    RECORD_MGMT_OK      = 0x00,
    RECORD_MGMT_FAILED  = -0x01,
    RECORD_MGMT_NODB    = -0x02,
    RECORD_MGMT_ENOLOCK = -0x03,
    RECORD_MGMT_ERETRY  = -0x04,
    RECORD_MGMT_NOSPACE = -0x05,

} RECORD_LOCK_STATUS;



UserExtendManager::UserExtendManager() {
    // Create a database lock semaphore
    userExtend_FS_Head = USER_EXTEND_FS_ADDR;

    LOGD("%s Init UserExtendManager \r\n",logtag);
}

UserExtendManager* UserExtendManager::getInstance()
{
//    LOGD("%s getInstance UserExtendManager \r\n", logtag);

    if(NULL == m_instance)
    {
        m_instance = new UserExtendManager();
    }
    return m_instance;
}

int UserExtendManager::get_free_index(){
    LOGD("get free index \r\n");
    int item_max = MAX_EXTEND_COUNT;
    // find new map index
    for (int i = 0; i < item_max; i++)
    {
        UserJson userExtend;
        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+i*(FLASH_SECTOR_SIZE), (uint8_t *)&userExtend, sizeof(UserJson)  );
        if( status == 0 ){
            if(  userExtend.UUID[0] == UUID_MAGIC_UNUSE ) {
                LOGD("第 %d 块 没有被使用  flash address %x  userExtend.UUID[0] %x\r\n", i, userExtend_FS_Head+i*(FLASH_SECTOR_SIZE), userExtend.UUID[0]);
                return i;
            }
        }else{

            LOGD("read flash error \r\n");
        }
    }

    return -1;
}

// -1 表示没有找到
int UserExtendManager::get_index_by_uuid(char *uuid) {
    LOGD("%s get_index_by_uuid %s \r\n",logtag, uuid);
    for( int i=0; i<MAX_EXTEND_COUNT; i++){
        UserJson userExtend;
        memset( &userExtend, 0, sizeof(UserJson));
        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+i*(FLASH_SECTOR_SIZE), (uint8_t *)&userExtend, sizeof(UserJson)  );
        if( status == 0 ){

            if( strcmp( userExtend.UUID, uuid) == 0 ){
                LOGD("%s index %i, uuid %s, json %s  \r\n", logtag, i, userExtend.UUID, userExtend.jsonData);
                return  i;
            }
        }else{
            LOGD("read flash failed \r\n");
        }
    }
    return -1;
}

int UserExtendManager::addUserJson(UserJson * userExtend){
    LOGD("%s addUserJson  %s %s \r\n", logtag, userExtend->UUID, userExtend->jsonData);
//  1.查询uuid 是否已经存在
//    Record_Lock();
    int index = delUserJsonByUUID( userExtend->UUID);
//  2. index ==-1 不存在则找到新的index新增,   否则在原有的位置写入
    if( index ==-1 ){
        LOGD("%s index %d ,uuid 不存在,则新增 \r\n", logtag, index);
        index = get_free_index();
        if( index == -1 ){
            LOGD("Error: Database is full \n");
            return -1;
        }
        int status = SLN_Write_Flash_Page(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userExtend, sizeof(UserJson));
//        int status = SLN_Write_Flash_At_Address(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userExtend);
//        int status = SLN_Write_Sector(userExtend_FS_Head+index* FLASH_SECTOR_SIZE, (uint8_t *)userExtend);
        if (status != 0) {
            LOGD("write flash failed %d \r\n", status);
            return  -1;
        }else{
            LOGD("第 %d 块写入成功 \r\n", index);
        }
        return  index;
    }else{
        LOGD("%s uuid 已存在,则擦除后增加  free index %d ,%d  \r\n", logtag, index, sizeof(UserJson) );
        int status = SLN_Write_Flash_Page(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userExtend, sizeof(UserJson) );
//        int status = SLN_Write_Flash_At_Address(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userExtend );
//        int status = SLN_Write_Sector(userExtend_FS_Head+index* FLASH_SECTOR_SIZE, (uint8_t *)userExtend );
        if (status != 0) {
            LOGD("write flash failed %d \r\n", status);
            return  -1;
        }else{
            LOGD("第 %d 块写入成功 \r\n", index);
        }
    }
//    Record_UnLock();
    return  index;
}
//input : uuid
//output: userExtend
int  UserExtendManager::queryUserJsonByUUID( char *uuid, UserJson *userJson){
    LOGD("%s queryUserJsonByUUID  %s\r\n",logtag, uuid);
//    Record_Lock();
    int index = get_index_by_uuid(uuid);
    if( index != -1 ){

        int status = SLN_Read_Flash_At_Address(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userJson, sizeof(UserJson));
        if (status != 0) {
            LOGD("read flash failed %d \r\n", status);
        }
    }else{
        LOGD("%s 没有查到对应 UUID 的记录\r\n",logtag);
    }
//    Record_UnLock();
    return  index;
}
//
int UserExtendManager::updateUserJsonByUUID( char *uuid, UserJson *userJson){
    LOGD("%s updateUserJsonByUUID  %s\r\n",logtag, uuid);
    int status =-1;
//    Record_Lock();
    int index = get_index_by_uuid( uuid );
    if( index != -1 ){
        status = SLN_Write_Flash_Page(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userJson, sizeof(UserJson) );
//        status = SLN_Write_Flash_At_Address(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userJson );
//        status = SLN_Write_Sector(userExtend_FS_Head+index* (FLASH_SECTOR_SIZE), (uint8_t *)userJson );
        if (status != 0) {
            LOGD("read flash failed %d \r\n", status);
            return  -1;
        }
    }
//    Record_UnLock();
    return  status;
}

int UserExtendManager::delUserJsonByUUID( char *uuid ){
    LOGD("%s delUserJsonByUUID  %s\r\n",logtag, uuid);
    int status=-1, index =-1;
//    Record_Lock();

    index = get_index_by_uuid(uuid );
    if( index != -1 ){
        status = SLN_Erase_Sector( userExtend_FS_Head+index*(FLASH_SECTOR_SIZE));
        if( status !=0 ){
            LOGD("erase flash failed %d \r\n", status);
        }else{
            LOGD("第 %d 块擦除成功 \r\n", index);
        }
    }
//    Record_UnLock();
    return  index;
}

int UserExtendManager::clearAllUserJson(  ){
    LOGD("%s clearAllUserJson  \r\n",logtag);
    int status=-1;
    for( int i =0; i<MAX_EXTEND_COUNT; i++ ){
        status = SLN_Erase_Sector( userExtend_FS_Head+i*(FLASH_SECTOR_SIZE));
        if( status !=0 ){
            LOGD("erase flash failed %d \r\n", status);
        }
    }
    return  status;
}


void UserExtendManager::setCurrentUser(char *uuid){
    LOGD("%s set current user  %s\r\n", logtag, uuid);
    memset( &gUserExtend, 0, sizeof(gUserExtend) );
    strcpy( gUserExtend.UUID, uuid);
    StrToHex( gUserExtend.HexUID, gUserExtend.UUID, sizeof(gUserExtend.HexUID) );//将uuid 转成16进制 hexuid
    return;
}

UserExtendClass *UserExtendManager::getCurrentUser( ){
    LOGD("%s get current user \r\n", logtag);
    return &gUserExtend;
}



int UserExtendManager::addStrUser(char * userInfo){
    LOGD("%s %s \r\n", logtag, __func__);
    int ret =0;
    UserExtendClass userExtendClass;
    memset( &userExtendClass, 0, sizeof(UserExtendClass) );
//    1.将字符串转成用户类
    convertStr2UserExtendClass( userInfo,  &userExtendClass);
//    2. 将将用户类转成 json格式
    UserJson    userJson;
    memset( &userJson, 0, sizeof(UserJson) );
    convertUserExtendClass2UserJson( &userExtendClass, &userJson);
//    3. 存储到flash 中
    ret = addUserJson( &userJson );
    return  ret;
}



void UserExtendManager::convertStr2UserExtendClass(char *strUserInfo, UserExtendClass *userExtendType) {
    LOGD("%s build a user: %d ,%s \r\n",logtag, strlen(strUserInfo), strUserInfo);
//1. 先将strUserInfo 拷贝到临时变量
    int infoLength = strlen( strUserInfo )+1;
    char tempUsrInfo[1000]={0};
    if( sizeof(tempUsrInfo) < infoLength ){
        LOGD("error \r\n");
    }
    strcpy( tempUsrInfo, strUserInfo);
//2. 第一个"," 分隔出 uuid
    char *uuid =NULL;
    uuid = strtok(tempUsrInfo, ",");
    if( uuid ==NULL ){
        return;
    }
    strcpy( userExtendType->UUID, uuid );
    LOGD( "uuid: %s \r\n", userExtendType->UUID);

//3. 第二个"," 分隔出 日期字段
    char *dateDuration=NULL;
    dateDuration =strtok(NULL, ",");
    if( dateDuration ==NULL ){
        return;
    }
    strcpy( userExtendType->dateDuration, dateDuration);
    LOGD( "date: %s \r\n", userExtendType->dateDuration);

//4. 第三个","  分隔出 一周内时间字段
    char *weekDuration=NULL;
    while( weekDuration =strtok(NULL, ",") ){
//        LOGD( "week: %s \r\n", weekDuration);
        strcat( userExtendType->weekDuration, weekDuration);
        strcat( userExtendType->weekDuration,",");
    }
    LOGD( "week: %s \r\n", userExtendType->weekDuration);

    return;
}


void UserExtendManager::convertUserExtendClass2UserJson( UserExtendClass *userExtend, UserJson *userJson){
    LOGD("%s %s  \r\n",logtag, __func__);

    char 	*cjson_str=NULL;
    cJSON * cObj = cJSON_CreateObject();
    cJSON_AddStringToObject(cObj, DATE, userExtend->dateDuration);
    cJSON_AddStringToObject(cObj, WEEK, userExtend->weekDuration);
    cjson_str = cJSON_PrintUnformatted( cObj);
    LOGD("cjson %s \r\n",  cjson_str);

    memcpy( userJson->UUID, userExtend->UUID, sizeof(userExtend->UUID));
    memcpy( userJson->jsonData, cjson_str, strlen(cjson_str));

    return;
}

void UserExtendManager::convertUserJson2UserExtendClass(UserJson *userJson, UserExtendClass *userExtendClass ){
    LOGD("%s %s  \r\n",logtag, __func__);
    strcpy( userExtendClass->UUID , userJson->UUID);
    StrToHex( userExtendClass->HexUID, userExtendClass->UUID, sizeof(userExtendClass->HexUID));//将uuid 转成16进制 hexuid
    cJSON *jsonObj = cJSON_Parse(userJson->jsonData);
    strcpy(userExtendClass->dateDuration , cJSON_GetObjectItem(jsonObj, DATE)->valuestring);
    strcpy(userExtendClass->weekDuration , cJSON_GetObjectItem(jsonObj, WEEK)->valuestring);
    LOGD("%s UUID %s ,date %s ,week %s \r\n", logtag, userExtendClass->UUID, userExtendClass->dateDuration, userExtendClass->weekDuration);
    return;
}


/**
 *  参数一 :   用户字串
 *  参数二 :   组用户类 group
 */
bool UserExtendManager::checkUUIDUserPermission( char *uuid ) {
    LOGD("%s %s \r\n",logtag, __func__ );
    bool flag   = false;
    UserJson        *userJson           = (UserJson *)pvPortMalloc(sizeof(UserJson) );
    UserExtendClass *userExtendClass    = (UserExtendClass *)pvPortMalloc( sizeof(UserExtendClass) );
    UserJson        *groupJson          = (UserJson *)pvPortMalloc( sizeof(UserJson) );
    UserExtendClass *groupExtendClass   = (UserExtendClass *)pvPortMalloc( sizeof(UserExtendClass) );
    memset( userJson, 0, sizeof(UserJson) );
    memset( groupJson, 0, sizeof(groupJson) );
    memset( userExtendClass, 0, sizeof(UserExtendClass) );
    memset( groupExtendClass, 0, sizeof(UserExtendClass) );

    while(1){// 用while break 取代if goto
        int userIndex = queryUserJsonByUUID( uuid, userJson);
        int groupIndex = queryUserJsonByUUID( "ALL", groupJson);
        //  未下发组用户的规则 则禁止通行
        if( groupIndex == -1 ){
            LOGD("%s group user not set , forbiden \r\n", logtag);
            flag = false;
            break;
        }
//  还没有下发通行时段的用户, 默认使用组用户通行规则
        if( (groupIndex != -1) && (userIndex == -1) ) {
            LOGD("%s 还没有下发通行时段的用户, 默认使用组用户通行规则\r\n", logtag);
            memcpy( userJson, groupJson, sizeof(UserJson) );
            userIndex = groupIndex;
        }
//  有效的用户,拿到当前时间比较通行规则
        if( (groupIndex !=-1) && (userIndex!=-1) ){
            convertUserJson2UserExtendClass( userJson, userExtendClass);
            convertUserJson2UserExtendClass( groupJson, groupExtendClass);

            LOGD("%s %s get user:%s, %s, %s \r\n",logtag, userExtendClass->UUID, userExtendClass->dateDuration, userExtendClass->weekDuration);
            LOGD("%s %s get all: %s, %s, %s \r\n",logtag, groupExtendClass->UUID, groupExtendClass->dateDuration, groupExtendClass->weekDuration);

            long currentTime  = ws_systime ; //东八区加8小时
            long currentTimeSlot = (currentTime-4*ADay+ 8*AHour )%AWeek; //1970年 0分0秒是星期四,从星期一开始计时

            int weekday = currentTimeSlot /ADay;
            int hour = currentTimeSlot %ADay /AHour;
            int min  = currentTimeSlot %AHour /AMin;
            int sec  = currentTimeSlot %AMin;
            LOGD("%s 现在的时间是%d 取模后%d, 周 %d - %d 时: %d 分: %d 秒\r\n", logtag,currentTime, currentTimeSlot, weekday +1, hour, min, sec);
//  1.比较 date 范围
            char * startDate = strtok(userExtendClass->dateDuration,"-");
            char * endDate = strtok( NULL, "-");
            long lStartDate = atol( startDate );
            long lEndDate = atol( endDate );


            char * allStartDate = strtok( groupExtendClass->dateDuration ,"-");
            char * allEndDate = strtok(NULL, "-");
            long lAllStartDate = atol( allStartDate );
            long lAllEndDate = atol( allEndDate);

            if( lStartDate >lEndDate ) {
                LOGD("error date format\r\n");
                flag = false;
                break;
            }
//    0-0  forbiden   0-1 临时同行  x-y normal
            if( lStartDate==0 ){
                if( lEndDate == 0 ) {
                    LOGD("forbiden user  \r\n");
                    flag = false;
                    break;
                }
            }
            if( (currentTime > lAllStartDate) && (currentTime <lAllEndDate) ){
                LOGD("%s当前时间在合法的通行时段里  \r\n", logtag);
            }else{
                LOGD("%s不合法的通行时段 \r\n", logtag);
                flag = false;
                break;
            }
            //  2.比较week 范围
            char * groupWeekDurationStart = strtok( groupExtendClass->weekDuration, ",-");
            char *groupWeekDurationEnd = strtok( NULL, ",-");
            LOGD("from %s ,to %s ,current %d \r\n", groupWeekDurationStart, groupWeekDurationEnd, currentTimeSlot);
            long lGroupWeekDurationStart = atol( groupWeekDurationStart );
            long lGroupWeekDurationEnd = atol( groupWeekDurationEnd );
//        如果当前时间在周时间段里 则通过
            if( currentTimeSlot >= lGroupWeekDurationStart && currentTimeSlot <= lGroupWeekDurationEnd ){
                flag = true;
                break;
            }
            while( groupWeekDurationStart=strtok(NULL, ",-")){
                groupWeekDurationEnd = strtok( NULL, ",-");

                LOGD("from %s, to %s ,current %d \r\n", groupWeekDurationStart, groupWeekDurationEnd, currentTimeSlot);
                lGroupWeekDurationStart = atol( groupWeekDurationStart );
                lGroupWeekDurationEnd = atol( groupWeekDurationEnd );
                if( currentTimeSlot >= lGroupWeekDurationStart && currentTimeSlot <= lGroupWeekDurationEnd ){
                    flag = true;
                    break;
                }
            }

        }
        //      最后跳出while
        break;
    }

//  释放内存
    if( userJson !=NULL ){
        vPortFree(userJson);
    }
    if( userExtendClass != NULL){
        vPortFree( userExtendClass );
    }
    if( groupJson != NULL){
        vPortFree( groupJson );
    }
    if( groupExtendClass != NULL){
        vPortFree( groupExtendClass );
    }
    //    时间段比较通过
    return  flag;
}

