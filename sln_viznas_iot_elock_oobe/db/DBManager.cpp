//
// Created by xshx on 2021/4/8.
//

#include <cstring>
#include "DBManager.h"
#include "FreeRTOS.h"
#include "semphr.h"

DBManager* DBManager::m_instance = NULL;
list<Record*> DBManager::recordList ;

typedef enum {
    RECORD_MGMT_OK      = 0x00,
	RECORD_MGMT_FAILED  = -0x01,
	RECORD_MGMT_NODB    = -0x02,
	RECORD_MGMT_ENOLOCK = -0x03,
	RECORD_MGMT_ERETRY  = -0x04,
	RECORD_MGMT_NOSPACE = -0x05,

} RECORD_LOCK_STATUS;

static SemaphoreHandle_t s_Record_Lock = NULL;
static int Record_Lock()
{
    if (NULL == s_Record_Lock)
    {
        return RECORD_MGMT_ENOLOCK;
    }

    if (pdTRUE != xSemaphoreTake(s_Record_Lock, portMAX_DELAY))
    {
        return RECORD_MGMT_ERETRY;
    }

    return RECORD_MGMT_OK;
}

static void Record_UnLock()
{
    xSemaphoreGive(s_Record_Lock);
}

DBManager::DBManager() {
    // Create a database lock semaphore
    if (NULL == s_Record_Lock)
    {
    	s_Record_Lock = xSemaphoreCreateMutex();
        if (NULL == s_Record_Lock)
        {
            LOGE("[ERROR]:Create DB lock semaphore\r\n");
        }
    }

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
#ifdef  FIX_SIZE
    LOGD("-----0.写回固定文件-----\r\n");

#else
    LOGD("-----0.删除文件-----\r\n");
    fatfs_delete( filePath );
#endif
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
    Record_Lock();

    cJSON * ObjArr = cJSON_CreateArray();
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ){
        Record  *tmpRecord = (Record*)*it;
        //保存未上传的记录
//        if( (tmpRecord->action_upload & 0xFF) != 2 ){
        if( tmpRecord->upload != BOTH_UPLOAD ){
            cJSON * cObj = cJSON_CreateObject();
            cJSON_AddNumberToObject(cObj, DB_KEY_ID, tmpRecord->ID);
            cJSON_AddStringToObject(cObj, DB_KEY_UUID, tmpRecord->UUID);
            cJSON_AddNumberToObject(cObj, DB_KEY_ACTION, tmpRecord->action);
            cJSON_AddNumberToObject(cObj, DB_KEY_TIME_STAMP, tmpRecord->time_stamp);
            //cJSON_AddNumberToObject(cObj, DB_KEY_STATUS, tmpRecord->status);
            cJSON_AddStringToObject(cObj, DB_KEY_IMAGE_PATH, tmpRecord->image_path);
//            cJSON_AddNumberToObject(cObj, DB_KEY_POWER,  tmpRecord->power);
            cJSON_AddStringToObject(cObj, DB_KEY_DATA, tmpRecord->data);// 蓝牙测温
            //cJSON_AddNumberToObject(cObj, DB_KEY_POWER1,  tmpRecord->power);
            //cJSON_AddNumberToObject(cObj, DB_KEY_POWER2, tmpRecord->power2);
            cJSON_AddNumberToObject(cObj, DB_KEY_UPLOAD, tmpRecord->upload);
//            cJSON_AddNumberToObject(cObj, DB_KEY_ACTION_UPLOAD, tmpRecord->action_upload);
            //4.2 把对象添加到数组
            cJSON_AddItemToArray(ObjArr, cObj);
        }
    }
    Record_UnLock();
    //4.3 ObjArr加入到jsonroot
    cJSON_AddItemToObject(jsonroot, DB_KEY_INFO, ObjArr);
    //cjson_str = cJSON_Print(jsonroot);
    cjson_str = cJSON_PrintUnformatted(jsonroot);
    if(cjson_str!=NULL){
		LOGD("-----2.将json格式数据存文件----%d 条记录--\r\n", cJSON_GetArraySize(ObjArr));
#ifdef  FIX_SIZE
		//LOGD("%s sizeof(Record) is %d, strlen(cjson_str) is %d\r\n", __FUNCTION__, sizeof(Record), strlen(cjson_str));
		//LOGD("%s cjson_str is %s \r\n", __FUNCTION__, cjson_str);
        LOGD("%d , max size %d \r\n", strlen(cjson_str), MAX_BYTE);
        assert( (strlen(cjson_str) < MAX_BYTE) );
		memset(buf, 0, sizeof(buf));
		memcpy(buf, cjson_str, strlen(cjson_str));
		fatfs_write( filePath, buf, 0, MAX_BYTE);
#else
		LOGD("%d , max size %d \r\n", strlen(cjson_str), MAX_BYTE);
        assert( (strlen(cjson_str) < MAX_BYTE) );
		fatfs_write( filePath, cjson_str, 0, strlen(cjson_str));
#endif
		vPortFree(cjson_str);
    }
    cJSON_Delete(jsonroot);
    LOGD("----- 保存记录文件结束-----\r\n");
    return true;
}
list<Record*> DBManager::readRecordFromFile(char *filePath){
    LOGD("-----1.从文件中读出json格式的记录-----\r\n");
    int ArrLen = 0;
    int fsize=0;
#ifdef FIX_SIZE
    fsize=MAX_BYTE;

#else
    fsize = fatfs_getsize(filePath);

#endif

    if (fsize == -1) {
        return recordList;
    } else {
        memset(buf, 0, fsize);
        int status = fatfs_read(filePath, buf, 0, fsize);
        //LOGD("status %d file size %d ,%s \r\n", status, fsize, buf);
        LOGD("status %d file size %d\r\n", status, fsize);

        if (status == 0)
        {
			cJSON *jsonroot = cJSON_Parse(buf);
			//4. 解析数组成员是json对象的数组ObjArr
			LOGD("------2.将json 格式数据组织成链表---------\r\n");
			cJSON *ObjArr = cJSON_GetObjectItem(jsonroot, DB_KEY_INFO);
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
					strcpy(record->UUID, cJSON_GetObjectItem(SubObj, DB_KEY_UUID)->valuestring);
					record->action = cJSON_GetObjectItem(SubObj, DB_KEY_ACTION)->valueint;
					record->time_stamp = cJSON_GetObjectItem(SubObj, DB_KEY_TIME_STAMP)->valuedouble;
					//record->status = cJSON_GetObjectItem(SubObj, DB_KEY_STATUS)->valueint;
					strcpy(record->image_path , cJSON_GetObjectItem(SubObj, DB_KEY_IMAGE_PATH)->valuestring);
//					record->power = cJSON_GetObjectItem(SubObj, DB_KEY_POWER)->valueint;
					strcpy(record->data,  cJSON_GetObjectItem(SubObj, DB_KEY_DATA)->valuestring);//蓝牙测温
					//record->power1 = cJSON_GetObjectItem(SubObj, DB_KEY_POWER1)->valueint;
					//record->power2 = cJSON_GetObjectItem(SubObj, DB_KEY_POWER2)->valueint;
					record->upload = cJSON_GetObjectItem(SubObj, DB_KEY_UPLOAD)->valueint;
//					record->action_upload = cJSON_GetObjectItem(SubObj, DB_KEY_ACTION_UPLOAD)->valueint;
					
				    Record_Lock();
					recordList.push_back(record);
    				Record_UnLock();
					//LOGD("i: [%d] id:%d, uuid:%s, action %d, time_stamp:%d, upload:%d\r\n", i, record->ID, record->UUID, record->action, record->time_stamp, record->upload);
					//LOGD("i: [%d] id:%d, uuid:%s, time_stamp:%d, action_upload:%d\r\n", i, record->ID, record->UUID, record->time_stamp, record->action_upload);
					//LOGD("i: [%d] uuid:%s, time_stamp:%d, action_upload:%d\r\n", i, record->UUID, record->time_stamp, record->action_upload);
					//LOGD("[%d] %s, %d, %d\r\n", i, record->UUID, record->time_stamp, record->action_upload);
					//LOGD("[%d] %d, %d\r\n", i, record->time_stamp, record->action_upload);
				}
			}

			cJSON_Delete(jsonroot);
        }else {
        	 LOGD("readRecordFromFile status = %d \r\n", status);
        }
    }

    LOGD("%s end\r\n", __FUNCTION__);
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

	int id = getLastRecordID()+1;
    int count = recordList.size();
    LOGD("增加操作记录 %d  最大记录限制到 %d\r\n", id, max_size);
    if( count >= MAX_COLUMN ){
        record->ID=id;
#if 0
        Record record0;
		int ret = getRecordByID(0, &record0);
		if (ret == 0) {
			LOGD("record0.image_path is %s\r\n", record0.image_path);
			fatfs_delete(record0.image_path);
		}
#endif
	    Record_Lock();

		Record* firstOne = recordList.front();
		recordList.pop_front();
		fatfs_delete(firstOne->image_path);
		vPortFree(firstOne);
        recordList.push_back( record );
    	Record_UnLock();
    } else{
        record->ID= id;
	    Record_Lock();
        recordList.push_back( record);
	    Record_UnLock();
    }

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

int  DBManager::getRecordCount()
{
    return  recordList.size();
}

list<Record*>  DBManager::getAllUnuploadRecord()
{
    Record_Lock();

    LOGD("获取全部开门成功，但未上传的识别记录 \r\n");
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); ) {
        Record *tmpRecord = (Record *) *it;
//        if( (tmpRecord->action_upload & 0xFF) == 2 ){
          if( tmpRecord->upload  == BOTH_UPLOAD ){
        	vPortFree(*it);
            it = recordList.erase(it);
        }else{
            it++;
        }
    }
    Record_UnLock();
    return  recordList;
}

Record*  DBManager::getLastRecord(   )
{
    Record_Lock();

    LOGD("获取最后一条记录 \r\n");
    Record *record= NULL;
    if( recordList.empty() != true ){
        record = recordList.back();

        //LOGD("%s \n", record->UUID);
        //LOGD("%ld\n", record->time_stamp);
        LOGD("%s %d\r\n", record->UUID, record->time_stamp);

    }
    Record_UnLock();

    return  record;
}

int DBManager::getLastRecordID(){
        LOGD("获取最后一条记录\r\n");
        Record_Lock();

        int ID=0;
        Record *record =NULL;
        if( recordList.empty() != true ){
            record = recordList.back();
            ID = record->ID;
        }
        Record_UnLock();
        return ID;
}




int DBManager::getRecordByID( int id, Record *record )
{
    LOGD("查找ID为 %d 的记录 \r\n", id);
    Record_Lock();

    int ret=-1;
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->ID == id ){
            memcpy( record, tmpRecord, sizeof(Record));
            ret = 0;
        }
    }
    Record_UnLock();
    return  ret;
}
bool  DBManager::updateRecordByID(Record *record)
{
    LOGD("更新ID匹配的识别记录\r\n");
    Record_Lock();

    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); it++ ) {
        Record *tmpRecord = (Record *) *it;
        if( tmpRecord->ID == record->ID ){
            memcpy( tmpRecord, record, sizeof(Record));
        }
    }
//    saveRecordToFile(recordList, RECORD_PATH);
    Record_UnLock();
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
    Record_Lock();

    int ret = 0;
    list <Record*>::iterator it;
    LOGD("clear records before vPortFree\r\n");

    for ( it = recordList.begin( ); it != recordList.end( );it++ ) {
    	vPortFree(*it);
    }
    LOGD("clear records after vPortFree\r\n");

    recordList.clear();
    Record_UnLock();
    LOGD("clear records before fatfs_delete\r\n");

    ret = fatfs_delete(RECORD_PATH);
    LOGD("clear records after fatfs_delete %d\r\n", ret);

    return  ret;
}
bool DBManager::deleteRecordByUUID(char *UUID )
{
    LOGD("删除用户UUID %s 对应的操作记录 \r\n", UUID);
    Record_Lock();

    bool flag=false;
    list <Record*>::iterator it;
    for ( it = recordList.begin( ); it != recordList.end( ); ) {
        LOGD("%p\n", *it);
        Record *record = (Record *) *it;
        LOGD("%s \n", record->UUID);
        int ret= strcmp(record->UUID, UUID );
        if( ret == 0 ){
        	vPortFree(*it);
            it=recordList.erase(it);
            flag = true;
        }else{
            ++it;
        }
    }
    Record_UnLock();
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
