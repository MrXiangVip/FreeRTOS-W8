//
// Created by xshx on 2021/4/8.
//

#include <cstring>
#include "DBManager.h"
#include <iostream>

using namespace std;


DBManager* DBManager::m_instance = NULL;
list<Record*> DBManager::recordList ;
list<Record*> DBManager::unUploadRecordList ;


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
    LOGD("-----0.将链表记录保存到文件-----\r\n");
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
    LOGD("-----1.将链表数据组织成jason格式-----\r\n");
    cJSON * ObjArr = cJSON_CreateArray();
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ){
        Record  *tmpRecord = (Record*)*it;

        cJSON * cObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(cObj, "ID", tmpRecord->ID);
        cJSON_AddStringToObject(cObj, "UUID", tmpRecord->UUID);
        cJSON_AddNumberToObject(cObj, "time_stamp", tmpRecord->time_stamp);
        cJSON_AddNumberToObject(cObj, "status", tmpRecord->status);

        cJSON_AddNumberToObject(cObj, "action", tmpRecord->action);
        cJSON_AddNumberToObject(cObj, "power",  tmpRecord->power);
        cJSON_AddNumberToObject(cObj, "power2", tmpRecord->power2);

        //4.2 把对象添加到数组
        cJSON_AddItemToArray(ObjArr, cObj);
    }
    //4.3 ObjArr加入到jsonroot
    cJSON_AddItemToObject(jsonroot, "ObjInfo", ObjArr);
    //cjson_str = cJSON_PrintUnformatted(jsonroot);
    cjson_str = cJSON_Print(jsonroot);

    LOGD("jsonroot : \n%s\r\n", cjson_str);
    LOGD("-----2.将jason格式数据存文件-----\r\n");
//    FILE *file = fopen( filePath, "w+");
//    fwrite(cjson_str,  strlen(cjson_str), 1,file);
    fatfs_write( filePath, cjson_str, 0, strlen(cjson_str));
//    fflush( file );
//    fclose( file );

//    free(cjson_str);
    vPortFree(cjson_str);
    cJSON_Delete(jsonroot);
    return true;
}
list<Record*> DBManager::readRecordFromFile(char *filePath){
    LOGD("-----1.从文件中读出jason格式的记录-----\r\n");

    int ArrLen = 0;

    int fsize = 0;

    fsize = fatfs_getsize(filePath);
    if (fsize == -1) {
        return recordList;
    } else {
        char *buf = (char *) pvPortMalloc(fsize);
        memset(buf, 0, fsize);
        fatfs_read(filePath, buf, 0, fsize);
        LOGD("%s \r\n", buf);


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
                record->time_stamp = cJSON_GetObjectItem(SubObj, "time_stamp")->valuedouble;
                recordList.push_back(record);
                LOGD("[%d] id:%d\r\n", i, record->ID);
                LOGD("[%d] uuid:%s\r\n", i, record->UUID);
                LOGD("[%d] time_stamp:%ld\r\n", i, record->time_stamp);

            }
        }

        cJSON_Delete(jsonroot);
        free(buf);
    }


    return  recordList;
}
// 初始化DB
void DBManager::initDB()
{

    LOGD("------初始化------ \r\n");
    readRecordFromFile(RECORD_PATH);
    return ;
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
    if( unUploadRecordList.empty()== true ){
        getAllUnuploadRecord();
    }
    int nrow = unUploadRecordList.size();
    return  nrow;
}

list<Record*>  DBManager::getAllUnuploadRecord()
{
    LOGD("获取全部开门成功，但未上传的识别记录 \r\n");
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->upload == false ){
            unUploadRecordList.push_back(tmpRecord);
        }
    }
    return  unUploadRecordList;
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
    int ret=0;

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
    saveRecordToFile(recordList, RECORD_PATH);
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
    int ret;
    recordList.clear();
    remove(RECORD_PATH);
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
    saveRecordToFile( recordList, RECORD_PATH);
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
