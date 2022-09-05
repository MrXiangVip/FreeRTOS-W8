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
UserExtendClass     UserExtendManager::gUserExtend ; // 表示一个全局的用户
uint32_t            UserExtendManager::userExtend_FS_Head = NULL;//头地址
uint32_t            UserExtendManager::userUUID_FS_Head = NULL;//头地址
uint8_t            *UserExtendManager::gUIDMap;

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
    userUUID_FS_Head   = USER_UUID_FS_ADDR;
//  XSHX ADD
    gUIDMap     = (uint8_t *)pvPortMalloc(sizeof(UUIDMap));
    memset(gUIDMap , 0, sizeof(UUIDMap));
    int status = SLN_Read_Flash_At_Address( userUUID_FS_Head, (uint8_t *)gUIDMap, sizeof(UUIDMap) );
    if( status !=0 ) {
        LOGD("%s read uuid failed \r\n", logtag);
    }
    LOGD("%s Init UserExtendManager UUID_ADDR 0x%x, EXTEND_ADDR 0x%x  \r\n",logtag,  USER_UUID_FS_ADDR, USER_EXTEND_FS_ADDR);
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

/*
 * index 为 -1 表示没有找到 空闲的可写区域
 * */
int UserExtendManager::get_free_index(bool *needErase) {
    LOGD("%s %s \r\n", logtag, __func__);
    for (int i = 0; i <MAX_EXTEND_COUNT ; ++i) {
//  如果是没有用过的page 不需要 擦除
        if(  *(gUIDMap+i* sizeof(uUID)) == UUID_MAGIC_UNUSE ){
            *needErase = false;
            return  i;
//  如果是被标记为 delete 的page ,则需要擦除后 再写
        }else if( *(gUIDMap+i* sizeof(uUID)) == UUID_MAGIC_DELET ){
            *needErase = true;
            return  i;
        }
    }
    return FLASH_FULL;
}


//-1 表示没有找到
int UserExtendManager::get_index_by_uuid(char *uuid) {
    LOGD("%s %s %s\r\n", logtag, __func__, uuid);
    for (int i = 0; i <MAX_EXTEND_COUNT ; ++i) {

        if( strncmp( (char *)(gUIDMap+i* sizeof(uUID)), (char *)uuid, sizeof(uUID) )==0 ){
            LOGD("%s page: %d 匹配到 uuid %s\r\n", logtag, i, uuid);

            return  i;
        }
    }
    LOGD("%s 未匹配到 uuid %s\r\n", logtag, uuid);
    return UUID_NOTMATCH;
}

/*
 *
 * */
int UserExtendManager::addUserJson(UserJson * userJson){
    LOGD("%s addUserJson  %s, %s \r\n", logtag, userJson->UUID, userJson->jsonData);

//    1 查询uuid 是否已经存在,
    int pageIndex = get_index_by_uuid( userJson->UUID );
    LOGD("%s page index %d\r\n",logtag, pageIndex);
//     如果uuid不存在 则找到一块可写的, 标记为删除 和未使用都为可写
    if( pageIndex == UUID_NOTMATCH ){
        LOGD("%s 新增用户时段 \r\n",logtag);
        bool needErase = false;
        pageIndex = get_free_index( &needErase );
        LOGD("%s 可用的index %d, 是否需要擦除 %d\r\n", logtag, pageIndex, needErase);
        if( pageIndex == FLASH_FULL){
            LOGD("空间已满\r\n");
            return  FLASH_FULL;
        }
        LOGD("%s 在 %d 处新增 \r\n", logtag, pageIndex);
        SLN_Read_Flash_At_Address( userUUID_FS_Head, gUIDMap, sizeof(UUIDMap));
//      不需要擦除 extend 的 page, 直接写 extend 的 page
        memcpy( (void *)(gUIDMap+pageIndex* sizeof(uUID)), userJson->UUID, sizeof(uUID) );
//       擦除
        int uuidSectorCount = sizeof(UUIDMap)/ FLASH_SECTOR_SIZE +1;
        for(int i=0; i< uuidSectorCount; i++){
            int status = SLN_Erase_Sector( userUUID_FS_Head +i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Erase_Sector %d  0x%x Success \r\n", i, userUUID_FS_Head +i*FLASH_SECTOR_SIZE);
            }
        }
//      写uuid
        for( int i=0; i<uuidSectorCount;i++ ){
            int status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Write_Sector %d 0x%x Success \r\n", i, userUUID_FS_Head+ i* FLASH_SECTOR_SIZE );
            }
        }

        if( needErase == false ){
//          写extend
            int status = SLN_Write_Flash_Page( userExtend_FS_Head + pageIndex* FLASH_PAGE_SIZE, (uint8_t *)userJson->jsonData, sizeof(userJson->jsonData));
            if( status ==0){
                LOGD("SLN_Write_Flash_Page %d 0x%x Success \r\n", pageIndex, userExtend_FS_Head + pageIndex* FLASH_PAGE_SIZE );
            }
        }else{
//          先擦除 再写入 读整个sector出来
            int sectorIndex = pageIndex %FLASH_PAGE_SIZE /FLASH_SECTOR_SIZE ;
            uint8_t   *s_DataCache = (uint8_t *)pvPortMalloc( FLASH_SECTOR_SIZE );
            memset( s_DataCache, 0, sizeof(FLASH_SECTOR_SIZE) );
            SLN_Read_Flash_At_Address( userExtend_FS_Head + sectorIndex*FLASH_SECTOR_SIZE, s_DataCache,  FLASH_SECTOR_SIZE);
            memcpy( s_DataCache +FLASH_PAGE_SIZE *pageIndex, userJson->jsonData, sizeof(userJson->jsonData) );
            int status = SLN_Erase_Sector( userExtend_FS_Head +sectorIndex*FLASH_SECTOR_SIZE);
            if( status ==0){
                LOGD("SLN_Erase_Sector  %d,0x%x Success \r\n", sectorIndex, userExtend_FS_Head + sectorIndex*FLASH_SECTOR_SIZE );
            }
            status = SLN_Write_Sector( userExtend_FS_Head+ sectorIndex*FLASH_SECTOR_SIZE, s_DataCache);
            if( status ==0){
                LOGD("SLN_Write_Sector  %d,0x%x Success \r\n", sectorIndex, userExtend_FS_Head + sectorIndex* FLASH_SECTOR_SIZE );
            }
            vPortFree( s_DataCache);
        }
    }else{
//    如果uuid 已经存在则删除原有的 再写入,
        LOGD("%s 修改用户时段 \r\n",logtag);
        SLN_Read_Flash_At_Address( userUUID_FS_Head, gUIDMap, sizeof(UUIDMap) );
        memcpy( (void *)( gUIDMap+pageIndex* sizeof(uUID) ), userJson->UUID, sizeof(uUID) );
        int uuidSectorCount = sizeof(UUIDMap)/ FLASH_SECTOR_SIZE +1;
        for(int i=0; i< uuidSectorCount; i++){
            int status =SLN_Erase_Sector( userUUID_FS_Head +i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Erase_Sector %d  0x%x Success \r\n", i, userUUID_FS_Head +i*FLASH_SECTOR_SIZE);
            }
        }
        for( int i=0; i<uuidSectorCount;i++ ){
            int status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Write_Sector %d  0x%x Success  \r\n",i, userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE));
            }
        }
//    先删除原有的jsonData sector 再写入
        uint8_t   *s_DataCache = (uint8_t *)pvPortMalloc( FLASH_SECTOR_SIZE );
        memset( s_DataCache, 0, FLASH_SECTOR_SIZE );
        int  extendSectorIndex = pageIndex* FLASH_PAGE_SIZE/FLASH_SECTOR_SIZE;
        SLN_Read_Flash_At_Address( userExtend_FS_Head + extendSectorIndex*FLASH_SECTOR_SIZE, s_DataCache,  FLASH_SECTOR_SIZE);
        memcpy( s_DataCache + pageIndex %(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE), userJson->jsonData, sizeof(userJson->jsonData) );
        int status =SLN_Erase_Sector( userExtend_FS_Head +extendSectorIndex*FLASH_SECTOR_SIZE );
        if( status ==0){
            LOGD("SLN_Erase_Sector %d 0x%x Success \r\n", extendSectorIndex, userExtend_FS_Head +extendSectorIndex*FLASH_SECTOR_SIZE);
        }
        SLN_Write_Sector( userExtend_FS_Head +extendSectorIndex*FLASH_SECTOR_SIZE, s_DataCache);
        if( status ==0){
            LOGD("SLN_Write_Sector %d 0x%x Success \r\n", extendSectorIndex, userExtend_FS_Head + extendSectorIndex* FLASH_SECTOR_SIZE );
        }
        vPortFree( s_DataCache);
    }

//    for(int i=0; i<MAX_EXTEND_COUNT; i++){
//
//        char  cUUID[9]={0};
//
//        memcpy( cUUID, gUIDMap+i* sizeof(uUID), sizeof(uUID));
//        LOGD(" 遍历UUID %d %s\r\n",i, cUUID);
//    }
    return pageIndex;

}

int  UserExtendManager::queryUserJsonByUUID( char *uuid, UserJson *userJson) {
    LOGD("%s queryUserJsonByUUID  %s\r\n",logtag, uuid);
    memcpy(userJson->UUID, uuid, sizeof(uUID));
    int pageIndex = get_index_by_uuid(uuid);
    if( pageIndex != UUID_NOTMATCH ){

        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+pageIndex* (FLASH_PAGE_SIZE), (uint8_t *)userJson->jsonData, sizeof(userJson->jsonData));
        if (status == 0) {
            LOGD("SLN_Read_Flash_At_Address %d  Success  uuid %s, data %s\r\n", userExtend_FS_Head + pageIndex*(FLASH_PAGE_SIZE), userJson->UUID, userJson->jsonData );
        }
    }else{
        LOGD("%s 没有查到对应 UUID %s 的记录\r\n",logtag, uuid);
    }
    return  pageIndex;
}


int UserExtendManager::delUserJsonByUUID( char *uuid ) {
    LOGD("%s delUserJsonByUUID  %s\r\n",logtag, uuid);

    int pageIndex = get_index_by_uuid(uuid );
    LOGD("%s 删除在 %d 处用户时段 \r\n",logtag, pageIndex);

    if( pageIndex != UUID_NOTMATCH ){
//      1.先删除 uuid
        SLN_Read_Flash_At_Address( userUUID_FS_Head, gUIDMap, sizeof(UUIDMap));
//      不需要擦除 extend 的 page, 直接写 extend 的 page
        memset( (void *)( gUIDMap + pageIndex* sizeof(uUID) ), UUID_MAGIC_DELET, sizeof(uUID) );
//       擦除
        int uuidSectorCount = sizeof(UUIDMap)/ FLASH_SECTOR_SIZE +1;
        for(int i=0; i< uuidSectorCount; i++){
            int status =SLN_Erase_Sector( userUUID_FS_Head +i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Erase_Sector %d Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
            }
        }
//      写uuid
        for( int i=0; i<uuidSectorCount;i++ ){
            int status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Write_Sector %d Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
            }
        }
//      2.  extend 不删除,也不管
    }else{
        LOGD("UUID %s 不存在\r\n", uuid);
    }
    return  pageIndex;
}



int UserExtendManager::clearAllUserJson(  ){
    LOGD("%s clearAllUserJson  \r\n",logtag);
    int status =-1;
    //      1.先删除 uuid
    SLN_Read_Flash_At_Address( userUUID_FS_Head, gUIDMap, sizeof(UUIDMap));
//      不需要擦除 extend 的 page, 直接写 extend 的 page
    memset( (void *)gUIDMap, UUID_MAGIC_DELET, sizeof(UUIDMap) );
//       擦除
    int uuidSectorCount = sizeof(UUIDMap)/ FLASH_SECTOR_SIZE +1;
    for(int i=0; i< uuidSectorCount; i++){
        SLN_Erase_Sector( userUUID_FS_Head +i*FLASH_SECTOR_SIZE );
    }
    for( int i=0; i<uuidSectorCount;i++ ){
        status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
        if( status ==0){
            LOGD("SLN_Write_Sector Success \r\n");
        }
    }
    return status;
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
    LOGD("%s %s %s\r\n", logtag, __func__, userInfo);
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
    LOGD("%s 加到第%d处 \r\n",logtag, ret);
//    LOGD("%s 加入后查询 \r\n",logtag);
//    ret = queryUserJsonByUUID( userJson.UUID, &userJson );
    return  ret;
}



void UserExtendManager::convertStr2UserExtendClass(char *strUserInfo, UserExtendClass *userExtendType) {
    LOGD("%s %s: %d ,%s \r\n",logtag,__func__, strlen(strUserInfo), strUserInfo);
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
    LOGD("%s UUID: %s ,date %s ,week %s \r\n", logtag, userExtendClass->UUID, userExtendClass->dateDuration, userExtendClass->weekDuration);
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
                    LOGD("0-0 forbiden user  \r\n");
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
                    LOGD("合法的日通行时段 \r\n");
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

