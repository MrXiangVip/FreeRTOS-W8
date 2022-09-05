//
// Created by xshx on 2022/5/18.
//

#ifndef USEREXTEND_USEREXTENDMANAGER_H
#define USEREXTEND_USEREXTENDMANAGER_H
#ifdef __cplusplus
extern "C"  {
#endif

#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "sln_flash.h"

//#define FEATUREDATA_MAX_COUNT 100
#define MAX_EXTEND_COUNT    600   //扩展记录最大的数量
#define UUID_MAGIC_UNUSE    0xFF
#define UUID_MAGIC_DELET    0x00

//#define USER_EXTEND_FS_ADDR         (0xE20000U)
#define USER_UUID_FS_ADDR           (0xD00000U) //xshx mod 20220831
#define USER_EXTEND_FS_ADDR         (0xD20000U) //xshx mod 20220831
#define USER_EXTEND_PAGE_SIZE       256

#define FLASH_FULL         -1
#define UUID_NOTMATCH   -1

#ifdef __cplusplus
}
#endif

//#define UERID  "UERID"
#define TIMES   "TIMES"
#define TIMEE   "TIMEE"
#define ADEV    "ADEV"
#define EXUSR   "EXUSR"

#define DATE    "DATE"
#define WEEK    "WEEK"
#define AMin  (60)
#define AHour (60*AMin)
#define ADay  (24*AHour)
#define AWeek (7*ADay)

/*8字节UUID*/
typedef union{
    struct{
        uint32_t L;
        uint32_t H;
    }tUID;
    uint8_t UID[8];
}uUID;

typedef struct {
    uint8_t  maxUUID[ MAX_EXTEND_COUNT * sizeof(uUID)];
}UUIDMap;

typedef union {
        struct {
            char UUID[20];
//            char jsonData[100];  //json 格式的数据  例如 GUI :int , Time:time用来存储 柜子ID,   可能增加时间段等信息
            char jsonData[200];  //json 格式的数据  例如 GUI :int , Time:time用来存储 柜子ID,   可能增加时间段等信息
        };
        unsigned char raw[USER_EXTEND_PAGE_SIZE ]; // 4-->8
}UserJson, *pUserJson;

//注册时的结构体    uuid , 起始时间, 结束时间, 设备ID
//
typedef struct{
    uint8_t HexUID[8];      //十六进制uuid
    char    UUID[17];       //uuid 的字符串
//    long    uStartTime;     // 用户订单的开始时间
//    long    uEndTime;       // 用户订单的结束时间
//    char    cDeviceId[48];  // 设备列表
//    long    lCreateTime;    //用户在内存里创建的时间 用于过滤频繁注册和识别
    char     dateDuration[40];  // 年月日 格式的日期时间段
    char     weekDuration[200]; // 星期时分 格式的时间段

}UserExtendClass;

#ifdef __cplusplus
class UserExtendManager {
    private:
//  单例模式
        static UserExtendManager *m_instance;
//  头地址
        static uint32_t         userExtend_FS_Head;
        static uint32_t         userUUID_FS_Head;
//  当前的用户信息
        static UserExtendClass       gUserExtend;

        static uint8_t               *gUIDMap;
        UserExtendManager();

        int get_free_index(bool *needErase);

        int get_index_by_uuid(char *uuid);
    public:

        int max_size = MAX_EXTEND_COUNT;

        static UserExtendManager *getInstance();

        int addUserJson(UserJson * userExtend);

        int queryUserJsonByUUID( char *uuid, UserJson *userJson);

//        int updateUserJsonByUUID( char *uuid, UserJson *userExtend);

        int delUserJsonByUUID( char *uuid );

        int clearAllUserJson(  );
//
        void setCurrentUser(char *uuid);// set a user

        UserExtendClass *getCurrentUser( );//get a user

        int addStrUser(char * userInfo);//

//      将字符串转成用户类实例
        void convertStr2UserExtendClass( char *strUserInfo, UserExtendClass *userExtendType);

//      将用户类转成 json 格式
        void convertUserExtendClass2UserJson( UserExtendClass *userExtendType, UserJson *userJson);
//      将json 格式转用户类
        void convertUserJson2UserExtendClass(UserJson *userJson, UserExtendClass *userExtendClass );

//      检查用户权限
        bool checkUUIDUserPermission( char *uuid );

};
#endif
#endif //USEREXTEND_USEREXTENDMANAGER_H
