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
#define MAX_COUNT 100    //200
#define UUID_MAGIC_UNUSE    "FF"


#define USER_EXTEND_FS_ADDR         (0xE20000U)
#define USER_EXTEND_SECTOR_SIZE     0x1000
#define USER_EXTEND_PAGE_SIZE       1000

#ifdef __cplusplus
}
#endif

typedef union {
        struct {
            char UUID[20];
            char jsonData[32];  //json 格式的数据  例如 GUI :int , Time:time用来存储 柜子ID,   可能增加时间段等信息
        };
        unsigned char raw[USER_EXTEND_PAGE_SIZE ]; // 4-->8
}UserExtend, *pUserExtend;

class UserExtendManager {
    private:
        static UserExtendManager *m_instance;
        static uint32_t        userExtend_FS_Head;

        UserExtendManager();

        int get_free_index();

        int get_index_by_uuid(char *uuid);
    public:

        int max_size = MAX_COUNT;

        static UserExtendManager *getInstance();

        void initManager();

        int addUserExtend(UserExtend * userExtend);

        int  queryUserExtendByUUID( char *uuid, UserExtend *userExtend);

        int updateUserExtendByUUID( char *uuid, UserExtend *userExtend);

        int delUserExtendByUUID( char *uuid );

};

#endif //USEREXTEND_USEREXTENDMANAGER_H
