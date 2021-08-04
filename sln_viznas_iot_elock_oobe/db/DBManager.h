//
// Created by xshx on 2021/4/8.
//

#ifndef DB_DBMANAGER_H
#define DB_DBMANAGER_H

#include <iostream>
#include <list>
#include "cJSON.h"
#include "fsl_log.h"
#include "fatfs_op.h"



#define RECORD_PATH "record"
//记录最大column
#define MAX_COLUMN 100
//记录最大字节数
#define MAX_BYTE (100*200)

#define FIX_SIZE   1
typedef unsigned char byte;

using namespace std;

typedef struct {
    int ID;
    char UUID[20];
    int action;       //  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁
    char image_path[16];	//64
    int     status;   // 0,操作成功 1,操作失败.
    long    time_stamp; //时间戳 从1970年开始的秒数
    int     power;    // 电池电量
    int     power2;   // 电池电量
    int     upload; //   0代表没上传 1代表记录上传图片未上传 2代表均已
} Record, *pRecord;

class DBManager {
private:
    static DBManager *m_instance;

    static list<Record*> recordList;

    static list<Record*> unUploadRecordList;

    DBManager();

    bool saveRecordToFile(list<Record*> recordList,char *filePath);
    list<Record*> readRecordFromFile(char *filePath);
public:

    int max_size=MAX_COLUMN;
    char buf[MAX_BYTE]={0};
    int recordNum=-1;
    Record *record=NULL;

    static DBManager *getInstance();

    void initDB();

    list<Record*> getRecord();
//  通行记录
    int addRecord(Record *record);




    Record* getLastRecord(  );
    int getLastRecordID();
    int getRecordByID( int id, Record *record );


    int getAllUnuploadRecordCount();

    list<Record *> getAllUnuploadRecord();

    bool  updateRecordByID(Record *record);
    bool  updateLastRecordStatus(int status, long currTime);

    int clearRecord();

    bool deleteRecordByUUID(char *UID);

    void flushRecordList();

};


#endif //DB_DBMANAGER_H
