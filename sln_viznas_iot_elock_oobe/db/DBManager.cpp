//
// Created by xshx on 2021/4/8.
//

#include <cstring>
#include "DBManager.h"
#include <iostream>

using namespace std;


DBManager* DBManager::m_instance = NULL;
list<Record*> DBManager::recordList ;


DBManager::DBManager() {

    initDB();
}
DBManager* DBManager::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new DBManager();
    }
    return m_instance;
}

bool DBManager::saveRecordToFile(list<Record*> recordList,char *filePath){

    LOGD("----- 保存记录文件-----\r\n");
    LOGD("-----0.删除文件-----\r\n");
    fatfs_delete( filePath );
    LOGD("-----1.将链表数据组织成json格式-----\r\n");
    list <Record*>::iterator it;
    char 	*cjson_str = NULL;
    cJSON 	*jsonroot = NULL;
    //新建一个JSON项目：jsonroot
    jsonroot = cJSON_CreateObject();
    if(NULL == jsonroot)
    {
        return false;
    }
    //4.1 先生成每个对象
    cJSON * ObjArr = cJSON_CreateArray();
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ){
        Record  *tmpRecord = (Record*)*it;
        //保存未上传的记录
        if( tmpRecord->upload != 2 ){
            cJSON * cObj = cJSON_CreateObject();
            cJSON_AddNumberToObject(cObj, "ID", tmpRecord->ID);
            cJSON_AddStringToObject(cObj, "UUID", tmpRecord->UUID);
            cJSON_AddNumberToObject(cObj, "action", tmpRecord->action);
            cJSON_AddNumberToObject(cObj, "time_stamp", tmpRecord->time_stamp);
            cJSON_AddNumberToObject(cObj, "status", tmpRecord->status);
            cJSON_AddStringToObject(cObj, "image_path", tmpRecord->image_path);
            cJSON_AddNumberToObject(cObj, "power",  tmpRecord->power);
            cJSON_AddNumberToObject(cObj, "power2", tmpRecord->power2);
            cJSON_AddNumberToObject(cObj, "upload", tmpRecord->upload);
            //4.2 把对象添加到数组
            cJSON_AddItemToArray(ObjArr, cObj);
        }
    }
    //4.3 ObjArr加入到jsonroot
    cJSON_AddItemToObject(jsonroot, "ObjInfo", ObjArr);
    cjson_str = cJSON_Print(jsonroot);

    LOGD("-----2.将json格式数据存文件-----\r\n");
    fatfs_write( filePath, cjson_str, 0, strlen(cjson_str));

    vPortFree(cjson_str);
    cJSON_Delete(jsonroot);
    LOGD("----- 保存记录文件结束-----\r\n");
    return true;
}
list<Record*> DBManager::readRecordFromFile(char *filePath){
    LOGD("-----1.从文件中读出json格式的记录-----\r\n");
    int ArrLen = 0;
    int fsize = 0;
    fsize = fatfs_getsize(filePath);
    if (fsize == -1) {
        return recordList;
    } else {
        char *buf = (char *) pvPortMalloc(fsize);
        memset(buf, 0, fsize);
        fatfs_read(filePath, buf, 0, fsize);
        LOGD("%d %s \r\n", fsize, buf);

        cJSON *jsonroot = cJSON_Parse(buf);
        //4. 解析数组成员是json对象的数组ObjArr
        LOGD("------2.将json 格式数据组织成链表---------\r\n");
        cJSON *ObjArr = cJSON_GetObjectItem(jsonroot, "ObjInfo");
        if (cJSON_IsArray(ObjArr)) {
            ArrLen = cJSON_GetArraySize(ObjArr);
            LOGD("ObjArr Len: %d\r\n", ArrLen);
            for (int i = 0; i < ArrLen; i++) {
                cJSON *SubObj = cJSON_GetArrayItem(ObjArr, i);
                if (NULL == SubObj) {
                    continue;
                }
                Record *record = (Record *) pvPortMalloc(sizeof(Record));
                record->ID = i;
                strcpy(record->UUID, cJSON_GetObjectItem(SubObj, "UUID")->valuestring);
                record->action = cJSON_GetObjectItem(SubObj, "action")->valueint;
                record->time_stamp = cJSON_GetObjectItem(SubObj, "time_stamp")->valuedouble;
                record->status = cJSON_GetObjectItem(SubObj, "status")->valueint;
                strcpy(record->image_path , cJSON_GetObjectItem(SubObj, "image_path")->valuestring);
                record->power = cJSON_GetObjectItem(SubObj, "power")->valueint;
                record->power2 = cJSON_GetObjectItem(SubObj, "power2")->valueint;
                record->upload = cJSON_GetObjectItem(SubObj, "upload")->valueint;

                recordList.push_back(record);
                LOGD("i: [%d] id:%d, uuid:%s, time_stamp:%d\r\n", i, record->ID, record->UUID, record->time_stamp);
            }
        }
        cJSON_Delete(jsonroot);
        vPortFree(buf);
    }
    return  recordList;
}
// 初始化DB
void DBManager::initDB()
{

    LOGD("------初始化------ \r\n");
    readRecordFromFile(RECORD_PATH);
}

list<Record*> DBManager::getRecord() {
	return recordList;
}

int DBManager::addRecord(Record *record){

    int id = recordList.size();
    LOGD("增加操作记录 %d \r\n", id);

    record->ID= id;
    recordList.push_back( record);

    int result= recordList.size();
    return  result;
}


int  DBManager::getAllUnuploadRecordCount()
{
    LOGD("获取操作成功但未上传记录总数 \r\n");
    if( recordList.empty()== true ){
        getAllUnuploadRecord();
    }
    int nrow = recordList.size();
    return  nrow;
}

list<Record*>  DBManager::getAllUnuploadRecord()
{
    LOGD("获取全部开门成功，但未上传的识别记录 \r\n");
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->upload == 2 ){
            it = recordList.erase(it);
        }else{
            it++;
        }
    }
    return  recordList;
}

Record*  DBManager::getLastRecord(   )
{
    LOGD("获取最后一条记录 \r\n");
    Record *record= NULL;
    if( recordList.empty() != true ){
        record = recordList.back();

        LOGD("%s \n", record->UUID);
        LOGD("%ld\n", record->time_stamp);
    }

    return  record;
}

int DBManager::getLastRecordID(){
        LOGD("获取最后一条记录\r\n");
        int ID=0;
        Record *record =NULL;
        if( recordList.empty() != true ){
            record = recordList.back();
            ID = record->ID;
        }
        return ID;
}
int DBManager::getRecordByID( int id, Record *record )
{
    LOGD("查找ID为 %d 的记录 \r\n", id);
    int ret=-1;
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->ID == id ){
            memcpy( record, tmpRecord, sizeof(Record));
            ret = 0;
        }
    }
    return  ret;
}
bool  DBManager::updateRecordByID(Record *record)
{
    LOGD("更新ID匹配的识别记录\r\n");
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->ID == record->ID ){
            memcpy( tmpRecord, record, sizeof(Record));
        }
    }
//    saveRecordToFile(recordList, RECORD_PATH);
    return  true;
}
bool  DBManager::updateLastRecordStatus(int status, long currTime)
{
    LOGD("更新最后一条记录\r\n");

    return  true;
}

int DBManager::clearRecord()
{
    LOGD("清空操作记录 \r\n");
    int ret = 0;
    recordList.clear();
    ret = fatfs_delete(RECORD_PATH);
    return  ret;
}
bool DBManager::deleteRecordByUUID(char *UUID )
{
    LOGD("删除用户UUID %s 对应的操作记录 \r\n", UUID);
    bool flag=false;
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); ) {
        LOGD("%p\n", *it);
        Record *record = (Record *) *it;
        LOGD("%s \n", record->UUID);
        int ret= strcmp(record->UUID, UUID );
        if( ret == 0 ){
            it=recordList.erase(it);
            flag = true;
        }else{
            ++it;
        }
    }
//    saveRecordToFile( recordList, RECORD_PATH);
    return  flag;
}
void DBManager::flushRecordList(){
        bool flag=false;
        flag = saveRecordToFile( recordList, RECORD_PATH);
        if( flag ){
            LOGD("保存文件成功 \r\n");
        }else{
            LOGD("保存文件失败 \r\n");
        }
}
