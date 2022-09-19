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
#include "commondef.h"
#include "database.h"
#include "WAVE_COMMON.h"
#include "sln_flash.h"
#include "DBManager.h"
#include "sln_api.h"

static  const char * logtag="[UserExtendManager]: ";

//全局的用户 包含的数据集合

UserExtendManager*  UserExtendManager::m_instance = NULL; //用户管理类的实例
UserExtendClass     UserExtendManager::gUserExtendClass ; // 表示一个全局的用户
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
    LOGD("%s %s UUID:%s\r\n", logtag, __func__, uuid);
    for (int i = 0; i <MAX_EXTEND_COUNT ; ++i) {

        if( strncmp( (char*)gUIDMap+i* sizeof(uUID), (char*)uuid, sizeof(uUID) )==0 ){
            LOGD("%s page: %d 匹配到 uuid: %s \r\n", logtag, i, uuid);

            return  i;
        }
    }
    LOGD("%s 未匹配到 uuid: %s\r\n", logtag, uuid);
    return UUID_NOTMATCH;
}

/*
 * 函数功能： 将json 格式的用户信息存储到 flash
 * */
int UserExtendManager::saveUserJsonToFlash(UserJson * userJson){
    LOGD("%s %s  %s, %s \r\n", logtag, __func__, userJson->UUID, userJson->jsonData);

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
//        memcpy( (void *)(gUIDMap+pageIndex* sizeof(uUID)), userJson->UUID, sizeof(uUID) );
        strncpy( (char*)(gUIDMap+pageIndex* sizeof(uUID)), userJson->UUID, sizeof(uUID) );
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
//            memcpy( s_DataCache +FLASH_PAGE_SIZE *pageIndex, userJson->jsonData, sizeof(userJson->jsonData) );
            strncpy( (char *)s_DataCache +FLASH_PAGE_SIZE *pageIndex, userJson->jsonData, sizeof(userJson->jsonData) );
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
        LOGD("%s 修改用户信息 \r\n",logtag);
        SLN_Read_Flash_At_Address( userUUID_FS_Head, gUIDMap, sizeof(UUIDMap) );
        strncpy( (char *)( gUIDMap+pageIndex* sizeof(uUID) ), userJson->UUID, sizeof(uUID) );

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
//        strncpy( (char *)s_DataCache + pageIndex %(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE), userJson->jsonData, sizeof(userJson->jsonData) );
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


    return pageIndex;

}

int UserExtendManager::saveUserExtendClassToFlash(UserExtendClass * userExtendClass) {
    LOGD("%s    %s \r\n", logtag, __func__ );
    UserJson  userJson;
    memset( &userJson, 0, sizeof(UserJson) );
    convertUserExtendClass2UserJson( &userJson, userExtendClass );
    saveUserJsonToFlash( &userJson );
    return 0;
}
/*
 * 函数功能： 通过uuid 查找userJson
 * 输入参数： uuid
 * 输出参数： userJson
 * 返回值 :成功返回 uuid 对应的下标  失败返回-1
 * */

int  UserExtendManager::queryUserJsonByUUID(  UserJson *userJson, char *uuid ) {
    LOGD("%s queryUserJsonByUUID  %s\r\n",logtag, uuid);
    int pageIndex = get_index_by_uuid(uuid);
    if( pageIndex != UUID_NOTMATCH ){
        strncpy(userJson->UUID, uuid, sizeof(uUID));
        int status = SLN_Read_Flash_At_Address( userExtend_FS_Head+pageIndex* (FLASH_PAGE_SIZE), (uint8_t *)userJson->jsonData, sizeof(userJson->jsonData));
        if (status == 0) {
            LOGD("SLN_Read_Flash_At_Address 0x%x  Success  uuid %s, data %s\r\n", userExtend_FS_Head + pageIndex*(FLASH_PAGE_SIZE), userJson->UUID, userJson->jsonData );
        }
    }else{
        LOGD("%s 没有查到对应 UUID %s 的记录 \r\n",logtag, uuid);
        strcpy( userJson->UUID, uuid );
        cJSON * jsonObj = cJSON_CreateObject();
        cJSON_AddNullToObject(jsonObj, WORKNO);
        cJSON_AddNullToObject(jsonObj, WORKMD);
        cJSON_AddNullToObject(jsonObj, DATE);
        cJSON_AddNullToObject(jsonObj, WEEK);
        char *cjson_str = cJSON_PrintUnformatted( jsonObj);
        printf("%s \r\n", cjson_str );
        strcpy( userJson->jsonData, cjson_str);
//       删除cJsonObject
    }
    return  pageIndex;
}

/*
 * 函数功能： 通过uuid  获得 对应的用户类信息
 * 输入参数： uuid
 * 输出参数： userExtendClass
 * */
int UserExtendManager::getUserExtendClassByUUID( UserExtendClass *userExtendClass, char *uuid ){
    LOGD("%s %s %s  \r\n", logtag, __func__,   uuid );
//  1. 先从flash 中读出 uuid 对应的userJson
    UserJson userJson;
    memset( &userJson, 0, sizeof(UserJson) );
    int index = queryUserJsonByUUID( &userJson, uuid);
    LOGD("查到 %s ? %s\r\n", uuid, (index>=0)?" 是":"否");
//   2. 将json 转成 class

    convertUserJson2UserExtendClass( userExtendClass, &userJson );

    return  index;
}

/*
 * 函数功能：
 * */
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
                LOGD("SLN_Erase_Sector 0x%x Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
            }
        }
//      写uuid
        for( int i=0; i<uuidSectorCount;i++ ){
            int status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
            if( status ==0){
                LOGD("SLN_Write_Sector 0x%x Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
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
        status = SLN_Erase_Sector( userUUID_FS_Head +i*FLASH_SECTOR_SIZE );
        if( status ==0){
            LOGD("SLN_Erase_Sector 0x%x Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
        }
    }
    for( int i=0; i<uuidSectorCount;i++ ){
        status = SLN_Write_Sector( userUUID_FS_Head+ i* (FLASH_SECTOR_SIZE), (uint8_t *)gUIDMap+i*FLASH_SECTOR_SIZE );
        if( status ==0){
            LOGD("SLN_Write_Sector 0x%x Success \r\n", userUUID_FS_Head+ i*FLASH_SECTOR_SIZE );
        }
    }
    return status;
}

/*
 * 函数功能: 设置系统中当前用户的uuid
 * */
void UserExtendManager::setCurrentUser(char *uuid){
    LOGD("%s set current user  %s\r\n", logtag, uuid);
    memset( &gUserExtendClass, 0, sizeof(gUserExtendClass) );
    strcpy( gUserExtendClass.UUID, uuid);
    StrToHex( gUserExtendClass.HexUID, gUserExtendClass.UUID, sizeof(gUserExtendClass.HexUID) );//将uuid 转成16进制 hexuid
    return;
}
/*
 * 函数功能:获取系统中当前用户
 * */
UserExtendClass *UserExtendManager::getCurrentUser( ){
    LOGD("%s get current user \r\n", logtag);
    getUserExtendClassByUUID( &gUserExtendClass, gUserExtendClass.UUID );
    return &gUserExtendClass;
}


/*
 * 函数功能: 增加用户通行日期
 * */
int UserExtendManager::addDurationUUIDStr(char * userInfo){
    LOGD("%s %s %s\r\n", logtag, __func__, userInfo);
    int ret =0;
    UserExtendClass userExtendClass, tmpUserExtendClass;
    memset( &userExtendClass, 0, sizeof(UserExtendClass) );
    memset( &tmpUserExtendClass, 0, sizeof(UserExtendClass) );
//    1. 先从字符串 中分隔出  date_duration, week_duration, uuid
//    convertStr2UserExtendClass( userInfo,  &userExtendClass);
    spliteDurationUUIDFromStr( tmpUserExtendClass.weekDuration, tmpUserExtendClass.dateDuration, tmpUserExtendClass.UUID, userInfo );
//    2.  从flash 中查出 userExtendClass
    ret = getUserExtendClassByUUID( &userExtendClass, tmpUserExtendClass.UUID );
    //    更新week  date
    strcpy( userExtendClass.weekDuration, tmpUserExtendClass.weekDuration);
    strcpy( userExtendClass.dateDuration, tmpUserExtendClass.dateDuration);
//  3.  将class 写入 flash 中存储
    saveUserExtendClassToFlash( &userExtendClass );
    LOGD("%s 加到第%d处 \r\n",logtag, ret);
    return  ret;
}
/*
 * 函数功能: 从字符串中分隔出 通行日期 和uuid
 * 输入参数  strUserInfo
 * 输出参数  week_duration, date_duration, uuid
 * */
void UserExtendManager::spliteDurationUUIDFromStr( char *week_duration, char *date_duration, char *uuid,  char *strUserInfo ) {
    LOGD("%s %s: %d ,%s \r\n",logtag,__func__, strlen(strUserInfo), strUserInfo);
//1. 先将strUserInfo 拷贝到临时变量
    int infoLength = strlen( strUserInfo )+1;
    char tempUsrInfo[1000]={0};
    if( sizeof(tempUsrInfo) < infoLength ){
        LOGD("error \r\n");
    }
    strcpy( tempUsrInfo, strUserInfo);
//2. 第一个"," 分隔出 uuid
    char *tmpUuid =NULL;
    tmpUuid = strtok(tempUsrInfo, ",");
    if( tmpUuid ==NULL ){
        return;
    }
    strcpy( uuid, tmpUuid );
    LOGD( "uuid: %s \r\n", tmpUuid);

//3. 第二个"," 分隔出 日期字段
    char *dateDuration=NULL;
    dateDuration =strtok(NULL, ",");
    if( dateDuration ==NULL ){
        return;
    }
    strcpy( date_duration, dateDuration );
    LOGD( "date: %s \r\n", dateDuration);

//4. 第三个","  分隔出 一周内时间字段
    char *weekDuration=NULL;
    while( weekDuration =strtok(NULL, ",") ){
//        LOGD( "week: %s \r\n", weekDuration);
        strcat( week_duration, weekDuration);
        strcat( week_duration, ",");
    }
    LOGD( "week: %s \r\n", week_duration);

    return;
}

/*
 * 函数功能： 设置uuid 对应的特征值
 * */
int UserExtendManager::setFeatureWithUUID(  char * name , float *feature ){
    LOGD("%s %s   \r\n", logtag, __func__    );
    int ret = DB_AddFeature_WithName( name,  feature );
    return  ret;
}
/*
 * 函数功能: 设置uuid  对应的 worKNo  workMode 到 flash
 * */
int UserExtendManager::addUserNoModeWithUUID( char *workNo, uint8_t workMode,char *uuid  ){
    LOGD("%s %s workNo: %s, Mode: %d, uuid: %s\r\n", logtag, __func__, workNo, workMode, uuid );
//  1 .先从 flash 中 读出 uuid 对应的userExtendClass
    UserExtendClass userExtendClass;
    memset( &userExtendClass, 0, sizeof(UserExtendClass) );
    int ret = getUserExtendClassByUUID( &userExtendClass, uuid );
//  2.修改 userNo, workMode
    strcpy( userExtendClass.work_no, workNo);
    userExtendClass.work_mode |= workMode;

//  3. 将class 转成json 写回flash
    ret = saveUserExtendClassToFlash( &userExtendClass );
    return  0;
}
/*
 * 函数功能 :分隔字符串  分隔出 workMode 和uuid,
 * */
int UserExtendManager::delUserModeWithUUIDStr(char *strInfo){
    LOGD("%s %s  \r\n",logtag, __func__);
    UserExtendClass userExtendClass, tmpUserExtendClass;
    memset( &userExtendClass, 0, sizeof(UserExtendClass) );
    memset( &tmpUserExtendClass, 0, sizeof(UserExtendClass) );
//    1.分隔字符串
    spliteUserModeUUIDFromStr( tmpUserExtendClass.work_mode, tmpUserExtendClass.UUID, strInfo);

//  2. 将 workMode 和对应的 uuid 加入flash
    int pageIndex = getUserExtendClassByUUID( &userExtendClass, tmpUserExtendClass.UUID);
    if( pageIndex == UUID_NOTMATCH ){
        return 0;
    }
//  3.修改workmode
    userExtendClass.work_mode &= !tmpUserExtendClass.work_mode;
    if( userExtendClass.work_mode == 0){
        vizn_api_status_t status = VIZN_DelUser(NULL, userExtendClass.UUID);
        DBManager::getInstance()->deleteRecordByUUID( userExtendClass.UUID);
        delUserJsonByUUID( userExtendClass.UUID );
        return 0;
    }else{
        //  4. 写回flash
        saveUserExtendClassToFlash( &userExtendClass );
        return  0;
    }
}

/* 分隔 出workMode uuid*/
int UserExtendManager::spliteUserModeUUIDFromStr(uint8_t workMode, char *uuid, char *strInfo){
    LOGD("%s %s  \r\n",logtag, __func__);
//1. 先将strUserInfo 拷贝到临时变量
    int infoLength = strlen( strInfo )+1;
    char tempUsrInfo[1000]={0};
    if( sizeof(tempUsrInfo) < infoLength ){
        LOGD("error \r\n");
    }
    strcpy( tempUsrInfo, strInfo);
//2. 第一个"," 分隔出 uuid
    char *tmpWorkMode =NULL;
    tmpWorkMode = strtok(tempUsrInfo, "-");
    if( tmpWorkMode ==NULL ){
        return -1;
    }
    workMode =atoi(tmpWorkMode);
    LOGD( "workMode: %d \r\n", workMode);

//3. 第二个"," 分隔出 日期字段
    char *tmpUuid=NULL;
    tmpUuid =strtok(NULL, ",");
    if( tmpUuid ==NULL ){
        return -1;
    }
    strcpy( uuid, tmpUuid );
    LOGD( "uuid: %s \r\n", uuid);
    return 0;

}

/*
 * 函数功能： 将userExtendClass 转成 userJson
 * 输入参数: userExtendClass
 * 输出参数: userJson
 * */
void UserExtendManager::convertUserExtendClass2UserJson( UserJson *userJson,  UserExtendClass *userExtendClass ){
    LOGD("%s %s  \r\n",logtag, __func__);

    char 	*cjson_str=NULL;
    cJSON * cObj = cJSON_CreateObject();
    cJSON_AddStringToObject(cObj, WORKNO, userExtendClass->work_no);
    cJSON_AddNumberToObject(cObj, WORKMD, userExtendClass->work_mode);
    cJSON_AddStringToObject(cObj, DATE, userExtendClass->dateDuration);
    cJSON_AddStringToObject(cObj, WEEK, userExtendClass->weekDuration);

    cjson_str = cJSON_PrintUnformatted( cObj);
    LOGD("cjson %s \r\n",  cjson_str);

    memcpy( userJson->UUID, userExtendClass->UUID, sizeof(userExtendClass->UUID));
    memcpy( userJson->jsonData, cjson_str, strlen(cjson_str));

    cJSON_Delete(cObj);
    return;
}
/*
 *  函数功能： 将UserJson 转成 UserExtendClass
 * 输入  userJson
 * 输出  userExtendClass
 * */
void UserExtendManager::convertUserJson2UserExtendClass( UserExtendClass *userExtendClass ,UserJson *userJson ){
    LOGD("%s %s  uuid:%s, jsonData: %s\r\n",logtag, __func__, userJson->UUID, userJson->jsonData );
    strcpy( userExtendClass->UUID , userJson->UUID);
    StrToHex( userExtendClass->HexUID, userExtendClass->UUID, sizeof(userExtendClass->HexUID));//将uuid 转成16进制 hexuid
    cJSON *jsonObj = cJSON_Parse(userJson->jsonData);
    if( jsonObj == NULL ){
        LOGD("%s %s userJson can't null \r\n", logtag, __func__);
    }else{
        cJSON *cJson;
        cJson= cJSON_GetObjectItem(jsonObj, WORKNO);
        if( cJson->valuestring == NULL ){
            printf( "work no %s \n", userExtendClass->work_no);
        }else{
            strcpy( userExtendClass->work_no,  cJson->valuestring );
        }
        cJson = cJSON_GetObjectItem(jsonObj, WORKMD);
        if( cJson-> valueint ==NULL ){
            userExtendClass->work_mode =0;
        }else{
            userExtendClass->work_mode = cJson->valueint;
        }
        cJson =  cJSON_GetObjectItem(jsonObj, DATE);
        if( cJson->valuestring ==NULL ){
            printf( "dateDuration %s \n", userExtendClass->dateDuration);

        }else{
            strcpy( userExtendClass->dateDuration , cJson->valuestring );
        }
        cJson = cJSON_GetObjectItem(jsonObj, WEEK);
        if( cJson->valuestring ==NULL ){
            printf( "weekDuration %s \n", userExtendClass->weekDuration);

        } else{
            strcpy( userExtendClass->weekDuration , cJSON_GetObjectItem(jsonObj, WEEK)->valuestring );
        }
    }

    LOGD("%s %s uuid: %s , work no %s, work mode %d ,date %s ,week %s \r\n", logtag, __func__, userExtendClass->UUID,
            userExtendClass->work_no, userExtendClass->work_mode, userExtendClass->dateDuration, userExtendClass->weekDuration);
    return;
}



/** 函數功能： 检查用户通行权限
 *  参数一 :   用户字串
 */
int UserExtendManager::checkUUIDUserModePermission( char *uuid ) {
    LOGD("%s %s 检查 uuid :%s 工作模式\r\n",logtag, __func__ , uuid);
    int flag=-1;
    bool pass =false;
    UserExtendClass *userExtendClass    = (UserExtendClass *)pvPortMalloc( sizeof(UserExtendClass) );
    memset( userExtendClass, 0, sizeof(UserExtendClass) );

    int userIndex  = getUserExtendClassByUUID(  userExtendClass, uuid );
    LOGD("%s %s index %d\r\n",logtag, __func__, userIndex );
    switch( userExtendClass->work_mode ){
        case NO_WORK_MODE: //
            LOGD("没有设置考勤和门禁模式");
            flag = OASIS_REC_RESULT_UNKNOWN_FACE;
            break;
        case ATTENDANCE_MODE: //仅考勤模式, 考勤不检查,直接通过
            LOGD("考勤模式 \r\n");
            flag = R60_REC_ATTSUCC_ACCFAIL_FACE;
            break;
        case ACCESS_MODE: //仅仅 门禁模式, 需要检查通行时段
            pass= checkUUIDUserDurationPermission( uuid );
            if( pass ){
                flag = R60_REC_ATTFAIL_ACCSUCC_FACE; //考勤不通过,门禁通过
            }else{
                flag = R60_REC_ATTFAIL_ACCFAIL_FACE; // 考勤和门禁都不通过
            }
            break;
        case ATT_ACC_MODE:// 门禁和考勤模式, 需要检查通行时段
            pass = checkUUIDUserDurationPermission( uuid );
            if( pass ){
                flag = R60_REC_ATTSUCC_ACCSUCC_FACE; //考勤通过,门禁通过
            } else{
                flag = R60_REC_ATTSUCC_ACCFAIL_FACE; // 考勤通过, 门禁不通过

            }
            break;
        default:
            LOGD("未知工作模式 \r\n");
            break;
    }
//   释放内存, 返回结果
    vPortFree( userExtendClass );
    return  flag;
}
bool UserExtendManager::checkUUIDUserDurationPermission( char *uuid ) {
    LOGD("%s %s 检查 uuid :%s 通行日期\r\n",logtag, __func__ , uuid);
    bool flag   = false;
    UserExtendClass *userExtendClass    = (UserExtendClass *)pvPortMalloc( sizeof(UserExtendClass) );
    UserExtendClass *groupExtendClass   = (UserExtendClass *)pvPortMalloc( sizeof(UserExtendClass) );
    memset( userExtendClass, 0, sizeof(UserExtendClass) );
    memset( groupExtendClass, 0, sizeof(UserExtendClass) );

    while(1){// 用while break 取代if goto
        int userIndex  = getUserExtendClassByUUID(  userExtendClass, uuid );
        int groupIndex = getUserExtendClassByUUID(  groupExtendClass, "ALL");
        LOGD("%s %s , userIndex: %d , groupIndex: %d\r\n", logtag, __func__, userIndex, groupIndex);

        //  未下发组用户的规则 则通行日期检查不通过
        if( groupIndex == UUID_NOTMATCH ){
            LOGD("%s group user not set , forbiden \r\n", logtag);
            flag = false;
            break;
        }
//  还没有下发通行时段的用户, 默认使用组用户通行规则
        if( (groupIndex != UUID_NOTMATCH) && (userIndex == UUID_NOTMATCH) ) {
            LOGD("%s 还没有下发通行时段的用户, 默认使用组用户通行规则\r\n", logtag);
            memcpy( userExtendClass, groupExtendClass, sizeof(UserExtendClass) );
            userIndex = groupIndex;
        }
//  有效的用户,拿到当前时间比较通行规则
        if( (groupIndex != UUID_NOTMATCH) && (userIndex!= UUID_NOTMATCH) ){

            LOGD("%s :%s get user:%s ,时段 %s ,周时段 %s \r\n",logtag, userExtendClass->UUID, userExtendClass->dateDuration, userExtendClass->weekDuration);
            LOGD("%s :%s get all: %s ,时段 %s ,周时段 %s \r\n",logtag, groupExtendClass->UUID, groupExtendClass->dateDuration, groupExtendClass->weekDuration);

            long currentTime  = ws_systime ; //时间戳不区分时区
            long currentTimeSlot = (currentTime-4*ADay+ 8*AHour )%AWeek; //1970年 0分0秒是星期四,从星期一开始计时  ,时间区分时区

            int weekday = currentTimeSlot /ADay;
            int hour = currentTimeSlot %ADay /AHour;
            int min  = currentTimeSlot %AHour /AMin;
            int sec  = currentTimeSlot %AMin;
            LOGD("%s 现在的时间是%d 取模后%d, 周 %d - %d 时: %d 分: %d 秒\r\n", logtag,currentTime, currentTimeSlot, weekday +1, hour, min, sec);
//  1.比较 date 范围
//          如果dateDuration 长度为0 表示 时间段没有设置,使用组用户的时间
            if( strlen(userExtendClass->dateDuration) == 0 ){
                LOGD("%s 未设置用户时段 , 使用组用户时间段\r\n", logtag );
                strcpy( userExtendClass->dateDuration, groupExtendClass->dateDuration );
                strcpy( userExtendClass->weekDuration, groupExtendClass->weekDuration );
            }
            char * userStartDate = strtok(userExtendClass->dateDuration,"-");
            char * userEndDate = strtok( NULL, "-");
            long lStartDate = atol( userStartDate );
            long lEndDate = atol( userEndDate );


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

    if( userExtendClass != NULL){
        vPortFree( userExtendClass );
    }

    if( groupExtendClass != NULL){
        vPortFree( groupExtendClass );
    }
    //    时间段比较通过
    return  flag;
}
