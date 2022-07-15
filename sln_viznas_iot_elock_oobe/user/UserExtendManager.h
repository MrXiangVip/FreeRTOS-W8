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

//#define MAX_COUNT 100
#define MAX_EXTEND_COUNT    100
#define UUID_MAGIC_UNUSE    0xFF
#define FLASH_SECTOR_SIZE   0x1000


#define USER_EXTEND_FS_ADDR         (0xE20000U)
#define USER_EXTEND_PAGE_SIZE       200

#ifdef __cplusplus
}
#endif

//#define UERID  "UERID"
#define TIMES   "TIMES"
#define TIMEE   "TIMEE"
#define ADEV    "ADEV"
#define EXUSR   "EXUSR"
/*8字节UUID*/
typedef union{
    struct{
        uint32_t L;
        uint32_t H;
    }tUID;
    uint8_t UID[8];
}uUID;

typedef union {
        struct {
            char UUID[20];
            char jsonData[100];  //json 格式的数据  例如 GUI :int , Time:time用来存储 柜子ID,   可能增加时间段等信息
        };
        unsigned char raw[USER_EXTEND_PAGE_SIZE ]; // 4-->8
}UserExtend, *pUserExtend;

//注册时的结构体    uuid , 起始时间, 结束时间, 设备ID
typedef struct{
    uint8_t HexUID[8];      //十六进制uuid
    char    UUID[17];       //uuid 的字符串
//    long    uStartTime;     // 用户订单的开始时间
//    long    uEndTime;       // 用户订单的结束时间
//    char    cDeviceId[48];  // 设备列表
//    long    lCreateTime;    //用户在内存里创建的时间 用于过滤频繁注册和识别
}UserExtendType;

//全局的用户信息
extern  UserExtendType  objUserExtend;

//xshx add 将 UserExtendType 转成UserExtend json
extern void vConvertUserExtendType2Json(UserExtendType *regist, UserExtend  *userExtend);
//xshx 将UserExtend json 转成UserExtendType
extern void vConverUserExtendJson2Type(UserExtend  *userExtend, unsigned int lCreateTime,  UserExtendType *userExtendType);

#ifdef __cplusplus
class UserExtendManager {
    private:
        static UserExtendManager *m_instance;
        static uint32_t        userExtend_FS_Head;

        UserExtendManager();

        int get_free_index();

        int get_index_by_uuid(char *uuid);
    public:

        int max_size = MAX_EXTEND_COUNT;

        static UserExtendManager *getInstance();

        void initManager();

        int addUserExtend(UserExtend * userExtend);

        int  queryUserExtendByUUID( char *uuid, UserExtend *userExtend);

        int updateUserExtendByUUID( char *uuid, UserExtend *userExtend);

        int delUserExtendByUUID( char *uuid );

        int clearAllUserExtend(  );


};
#endif
#endif //USEREXTEND_USEREXTENDMANAGER_H
