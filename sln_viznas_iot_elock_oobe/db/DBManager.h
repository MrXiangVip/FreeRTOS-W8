//
// Created by xshx on 2021/4/8.
//

#ifndef DB_DBMANAGER_H
#define DB_DBMANAGER_H

#include <list>
#include "cJSON.h"
#include "fsl_log.h"
#include "fatfs_op.h"


#define RECORD_PATH "record.jsn"

#if 0
#define DB_KEY_INFO         "ObjInfo"
#define DB_KEY_ID		    "ID"
#define DB_KEY_UUID		    "UUID"
#define DB_KEY_ACTION	    "action"
#define DB_KEY_TIME_STAMP   "time_stamp"
#define DB_KEY_STATUS		"status"
#define DB_KEY_IMAGE_PATH   "image_path"
//#define DB_KEY_POWER        "power"
//#define DB_KEY_POWER1       "power1"
//#define DB_KEY_POWER2       "power2"
#define DB_KEY_UPLOAD       "upload"
//#define DB_KEY_ACTION_ULOAD "action_upload"
#else
#define DB_KEY_INFO            "IF"
#define DB_KEY_ID                "ID"
#define DB_KEY_UUID                "UUID"
#define DB_KEY_ACTION            "AC"
#define DB_KEY_TIME_STAMP    "TS"
#define DB_KEY_STATUS            "ST"
#define DB_KEY_IMAGE_PATH    "IP"
#define DB_KEY_POWER            "P"
#define DB_KEY_DATA         	"D"
#define DB_KEY_POWER1        "P1"
#define DB_KEY_POWER2        "P2"
#define DB_KEY_UPLOAD        "UP"
//#define DB_KEY_ACTION_UPLOAD 	"AU"
#endif

//记录最大column
#define MAX_COLUMN 200    //200
//记录最大字节数
#define MAX_BYTE (MAX_COLUMN*200)

#define FIX_SIZE   1

typedef enum {
    REGISTE = 0,			/* (0) 注册 */
    UNLOCK,			        /* (1) 一键开锁*/
    KEY_UNLOCK,				/* (2)  钥匙开锁 */
    FACE_UNLOCK,			/* (3) 人脸识别蓝牙开锁 */
    MECH_UNLOCK,            /* (4) 机械开锁 */
    FACE_TEMPER				/* (5) 先识别再测温 */
} ACTION;

typedef enum {
    BOTH_UNUPLOAD = 0,			/* (0) 记录和图片都没有上传 */
    RECORD_UPLOAD,			        /* (1) 记录上传图片没有上传*/
    BOTH_UPLOAD				/* (2)  记录和图片都上传 */
} UPLOAD;


using namespace std;

typedef struct {
    int ID;
    char UUID[20];
    int action;       //  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁 
    char image_path[16];    //64
    //int     status;   // 0,操作成功 1,操作失败.(暂时不要,每次都是上传0)
    long time_stamp; //时间戳 从1970年开始的秒数
    char     data[32];  //data , 用作蓝牙锁时, 存储锁电量.  用作蓝牙测温时,存储蓝牙测温信息.
    int     upload; //   enum UPLOAD
    // short power;    // 电池电量 高8位为power1, 低8位为power2
    //int     power1;    // 电池电量
    //int     power2;   // 电池电量
    // short action_upload;    //高8位为action, 操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3:人脸识别开锁  11:机械开锁
    //低8位upload, 0代表没上传 1代表记录上传图片未上传 2代表均已
    //int     upload; //   0代表没上传 1代表记录上传图片未上传 2代表均已
} Record, *pRecord;

class DBManager {
private:
    static DBManager *m_instance;

    static list<Record *> recordList;

    DBManager();

    bool saveRecordToFile(list<Record *> recordList, char *filePath);

    list<Record *> readRecordFromFile(char *filePath);

public:

    int max_size = MAX_COLUMN;
    char buf[MAX_BYTE] = {0};
    int recordNum = -1;

    static DBManager *getInstance();

    void initDB();

    list<Record *> getRecord();

//  通行记录
    int addRecord(Record *record);


    Record *getLastRecord();

    int getLastRecordID();

    int getRecordByID(int id, Record *record);


    int getAllUnuploadRecordCount();

    int getRecordCount();

    list<Record *> getAllUnuploadRecord();

    bool updateRecordByID(Record *record);

    bool updateLastRecordStatus(int status, long currTime);

    int clearRecord();

    bool deleteRecordByUUID(char *UID);

    void flushRecordList();

};


#endif //DB_DBMANAGER_H
