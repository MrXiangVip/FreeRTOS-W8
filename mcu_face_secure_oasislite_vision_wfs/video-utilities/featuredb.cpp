/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#include "featuredb.h"
#include "fsl_log.h"
#include <algorithm>

//#define TEST_DB_PREBUILT
#ifdef TEST_DB_PREBUILT
//#include "feaLibIR9921.h"
//#include "feaLibRGB9921.h"
//#include "feaLibRGB9975.h"
#endif


#if RTFFI_USE_FATFS
#include "fatfs_op.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define	FEATURE_FILE_LENNAME	(128)

extern uint8_t remote_change_fs;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

db_context_handle_t s_feature_db_context_list;

#define STRUCT_OFFSET(type, field) ((size_t) & (((type *)0)->field))
#define OP_LINK_POINTER(node) ((db_op_link_t *)(((uint32_t)node) - STRUCT_OFFSET(db_op_link_t, link)))

int link(db_context_handle_t *context_handle, db_op_link_t *op_link)
{
    list_status_t status;
    memset(&op_link->link, 0 , sizeof(op_link->link));

    status = LIST_AddTail(context_handle, &op_link->link );
    if (status != kLIST_Ok)
    {
        if (status == kLIST_Full)
            return DB_LINK_FULL;
        else
            return DB_FAIL;
    }
    else
    {
        return DB_SUCCESS;
    }
}

int unlink(db_context_handle_t *context_handle, db_op_link_t *op_link)
{
    list_status_t status;
    status = LIST_RemoveElement(&op_link->link);

    memset(&op_link->link, 0, sizeof(op_link->link));

    if (status != kLIST_Ok)
        return DB_FAIL;
    else
        return DB_SUCCESS;
}

int add_sync(uint16_t id, const string name, float* feature)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    sprintf(file_name, "%010d,%s", id, name.c_str());
    fatfs_write(file_name, (const char *)feature, 0, OASISLT_getFaceItemSize());

    return 0;
}

int del_sync(const string name)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    //1.find id
    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            if(strcmp(file_tmp+11, name.c_str()) == 0)
            {
                fatfs_delete(file_tmp);
            }
        }
    }

    fatfs_closedir();

    return 0;
}

int clear_sync()
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];
    vector<string> file_list;

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            fatfs_delete(file_tmp);
            file_list.push_back(string(file_tmp));
        }
    }

    fatfs_closedir();

    for(unsigned int i = 0; i < file_list.size(); i++)
    {
        fatfs_delete(file_list[i].c_str());
    }

    return 0;
}

int update_sync(uint16_t id, const string name, float* feature)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    sprintf(file_name, "%010d,%s", id, name.c_str());
    fatfs_write(file_name, (const char *)feature, 0, OASISLT_getFaceItemSize());

    return 0;
}

int rename_sync(const string oldname, const string newname)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            if(strcmp(file_tmp+11, oldname.c_str()) == 0)
            {
                string new_filename = string(file_tmp).replace(11, oldname.length(), newname);
                fatfs_rename(file_tmp, new_filename.c_str());
            }
        }
    }

    fatfs_closedir();

    return 0;
}

vector<db_key_t> get_keys_sync()
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];
    char id_str[11] = "";
    db_key_t key_tmp;
    vector<db_key_t> keys;

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            strncpy(id_str, file_tmp, 10);
            key_tmp.id = atoi(id_str);
            strcpy(key_tmp.name, file_tmp+11);
            keys.push_back(key_tmp);
        }
    }

    fatfs_closedir();

    return keys;
}

int get_feature_sync(uint16_t id, float* feature)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    vector<db_key_t> keys = get_keys_sync();

    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
        if((*it).id == id) {
            sprintf(file_name, "%010d,%s", id, (*it).name);
            break;
        }
    }

    fatfs_read(file_name, (char*)feature, 0, OASISLT_getFaceItemSize());

    return 0;
}

//int get_feature_sync(char *name, float* feature)
//{
//    if( remote_change_fs ) {
//        fatfs_mount_with_mkfs();
//        remote_change_fs = 0;
//    }
//
//    char file_name[255];
//    vector<db_key_t> keys = get_keys();
//
//    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
//         if(!strcmp((*it).name.c_str(), name)) {
//             sprintf(file_name, "%010d,%s", (*it).id, (*it).name.c_str());
//             break;
//         }
//    }
//
//    fatfs_read(file_name, (char*)feature, 0, OASISLT_getFaceItemSize());
//
//    return 0;
//}

FeatureDB::FeatureDB(float thres)
{
#if 0
    Flash_FacerecFsInit();
#endif

    fatfs_mount_with_mkfs();

    LIST_Init(&s_feature_db_context_list, 20);

//    self_test();

//    load_feature();
}

FeatureDB::~FeatureDB()
{
}

int FeatureDB::add_feature(uint16_t id, const string name, float* feature)
{
	LOGD("%s", __FUNCTION__);
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    sprintf(file_name, "%010d,%s", id, name.c_str());
    fatfs_write(file_name, (const char *)feature, 0, OASISLT_getFaceItemSize());
    return 0;
}

int FeatureDB::update_feature(uint16_t id, const string name, float* feature)
{
	LOGD("%s", __FUNCTION__);
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    sprintf(file_name, "%010d,%s", id, name.c_str());
    fatfs_write(file_name, (const char *)feature, 0, OASISLT_getFaceItemSize());

    return 0;
}

int FeatureDB::del_feature(uint16_t id, string name)
{
	LOGD("%s", __FUNCTION__);
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    char file_name[FEATURE_FILE_LENNAME];
    sprintf(file_name, "%010d,%s", id, name.c_str());
    fatfs_delete(file_name);
    return 0;
}

int FeatureDB::del_feature(const string name)
{
	LOGD("%s", __FUNCTION__);
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    //1.find id
    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            if(strcmp(file_tmp+11, name.c_str()) == 0)
            {
                fatfs_delete(file_tmp);
            }
        }
    }

    fatfs_closedir();
    return 0;
}

int FeatureDB::del_feature_all()
{
	LOGD("%s", __FUNCTION__);
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];
    std::vector<std::string> file_list;

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            fatfs_delete(file_tmp);
            file_list.push_back(std::string(file_tmp));
        }
    }

    fatfs_closedir();

    for(unsigned int i = 0; i < file_list.size(); i++)
    {
        fatfs_delete(file_list[i].c_str());
    }
    return 0;
}

int FeatureDB::ren_name(const string oldname, const string newname)
{
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    char file_tmp[FEATURE_FILE_LENNAME];

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            if(strcmp(file_tmp+11, oldname.c_str()) == 0)
            {
                std::string new_filename = std::string(file_tmp).replace(11, oldname.length(), newname);
                fatfs_rename(file_tmp, new_filename.c_str());
            }
        }
    }

    fatfs_closedir();
    return 0;
}

int FeatureDB::db_sync(void)
{
    list_element_handle_t p = s_feature_db_context_list.head;
    while(p != NULL)
    {
        db_op_link_t *op_link = OP_LINK_POINTER(p);
        switch (op_link->op)
        {
            case DB_ADD:
            {
                add_sync(op_link->content->id, op_link->content->name, op_link->content->feature);
            }
            break;

            case DB_DELETE:
            {
//                if(op_link->content->id != 0 && op_link->content->name != NULL)
//                {
//                    del_feature(op_link->content->id, op_link->content->name);
//                }
//                else
                if(op_link->content->name[0] != '\0')
                {
                    del_sync(op_link->content->name);
                }
                else
                {
                    clear_sync();
                }
            }
            break;

            case DB_UPDATE:
            {
                update_sync(op_link->content->id, op_link->content->name, op_link->content->feature);
            }
            break;

            case DB_RENAME:
            {
                rename_sync(op_link->content->oldname, op_link->content->name);
            }
            break;

            default:
            break;
        }

        p = LIST_GetNext(p);
        LIST_RemoveHead(&s_feature_db_context_list);
        vPortFree(op_link->content->feature);
        vPortFree(op_link);

    }

    return DB_SUCCESS;
}

vector<db_key_t> FeatureDB::get_keys()
{
    vector<db_key_t> keys = get_keys_sync();

    list_element_handle_t p = s_feature_db_context_list.head;

    while(p != NULL) {
        db_op_link_t *op_link = OP_LINK_POINTER(p);

        switch (op_link->op) {
            case DB_ADD: {
                db_key_t key_tmp;
                key_tmp.id = op_link->content->id;
                strcpy(key_tmp.name, op_link->content->name);
                keys.push_back(key_tmp);
            }
            break;

            case DB_DELETE: {
                if(op_link->content->name[0] != '\0') {
                    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
                        if(!strcmp((*it).name, op_link->content->name)) {
                            keys.erase(it);
                            break;
                        }
                    }
                }
                else {
                    keys.clear();
                }
            }
            break;

            case DB_UPDATE: {
                //nothing to do.
            }
            break;

            case DB_RENAME: {
                for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
                    if(!strcmp((*it).name, op_link->content->oldname)) {
                        strcpy((*it).name, op_link->content->name);
                        break;
                    }
                }
            }
            break;

            default:
            break;
        }

        p = LIST_GetNext(p);
    }

    return keys;
}

vector<uint16_t> FeatureDB::get_ids()
{
    vector<uint16_t> ids;
    vector<db_key_t> keys = get_keys();

    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
        ids.push_back((*it).id);
    }

    return ids;
}

int FeatureDB::get_name(uint16_t id, string& name)
{
    vector<db_key_t> keys = get_keys();

    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
        if((*it).id == id)
           name = (*it).name;
    }

    return 0;
}

vector<string> FeatureDB::get_names(void)
{
    vector<string> names;
    vector<db_key_t> keys = get_keys();

    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
        names.push_back((*it).name);
    }

    return names;
}

int FeatureDB::get_feature(uint16_t id, float* feature)
{
    vector<db_key_t> keys = get_keys();

    get_feature_sync(id, feature);

    list_element_handle_t p = s_feature_db_context_list.head;

    while(p != NULL) {
        db_op_link_t *op_link = OP_LINK_POINTER(p);

        switch (op_link->op) {
            case DB_ADD: {
                db_key_t key_tmp;
                key_tmp.id = op_link->content->id;
                strcpy(key_tmp.name, op_link->content->name);
                if(op_link->content->id == id){
                    memcpy(feature, op_link->content->feature, OASISLT_getFaceItemSize());
                }
            }
            break;

            case DB_DELETE: {
                if(op_link->content->name[0] != '\0') {
                    for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
                        if(!strcmp((*it).name, op_link->content->name)) {
                            keys.erase(it);
                            memset(feature, 0x00, OASISLT_getFaceItemSize());
                            break;
                        }
                    }
                }
                else {
                    keys.clear();
                    memset(feature, 0x00, OASISLT_getFaceItemSize());
                }
            }
            break;

            case DB_UPDATE: {
                if(op_link->content->id == id) {
                    memcpy(feature, op_link->content->feature, OASISLT_getFaceItemSize());
                }
            }
            break;

            case DB_RENAME: {
                for(vector<db_key_t>::iterator it = keys.begin(); it != keys.end(); it++) {
                    if(!strcmp((*it).name, op_link->content->oldname)) {
                        strcpy((*it).name, op_link->content->name);
                        break;
                    }
                }
            }
            break;

            default:
            break;
        }

        p = LIST_GetNext(p);
    }
    return 0;
}

int FeatureDB::feature_count()
{
	LOGD("%s", __FUNCTION__);

    //return get_keys().size();
#if 1
    if( remote_change_fs ) {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    int ret;
    int count = 0;
    char file_tmp[FEATURE_FILE_LENNAME];

    fatfs_opendir("/");

    // find valid item
    for(;;)
    {
        ret = fatfs_readdir(file_tmp);
        if( ret == 0 ) {
            break;
        } else if ( ret == 3) {
            count++;
        }
    }

    fatfs_closedir();

    return count;
#endif
}

int FeatureDB::get_free(int& index)
{
    //discard
    return 0;
}

int FeatureDB::database_save(int count)
{
    db_sync();
    //discard
    return 0;
}

int FeatureDB::load_feature()
{
    //discard
    return 0;
}

int FeatureDB::erase_feature(int index)
{
    //discard
    return 0;
}

int FeatureDB::save_feature(int index)
{
    //discard
    return 0;
}

int FeatureDB::reassign_feature()
{
    //discard
    return 0;
}

int FeatureDB::get_free_map()
{
    //discard
    return 0;
}

int FeatureDB::get_remain_map()
{
    //discard
    return 0;
}

//void FeatureDB::verify()
//{
//    vector<db_key_t> keys = get_keys_sync();
//
//}
void FeatureDB::self_test()
{
    float *p_feature = (float*)pvPortMalloc(OASISLT_getFaceItemSize());
    float *v_feature = (float*)pvPortMalloc(OASISLT_getFaceItemSize());
    memset(p_feature, 0xff, OASISLT_getFaceItemSize());
    memset(v_feature, 0x0,  OASISLT_getFaceItemSize());
    vector<db_key_t> keys;

    //case1: add 1, del 1
    add_feature(1, "user_1", p_feature);
    keys = get_keys();
    assert(keys[0].id == 1);
    assert(strcmp(keys[0].name, "user_1") == 0);
    get_feature(1, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    db_sync();
    keys = get_keys();
    assert(keys[0].id == 1);
    assert(strcmp(keys[0].name, "user_1") == 0);
    get_feature(1, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    del_feature("user_1");
    keys = get_keys();
    assert(keys.size() == 0);

    db_sync();
    keys = get_keys();
    assert(keys.size() == 0);

    //case1: add 1,2; del 1
    add_feature(1, "user_1", p_feature);
    keys = get_keys();
    assert(keys[0].id == 1);
    assert(strcmp(keys[0].name, "user_1") == 0);
    get_feature(1, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    add_feature(2, "user_2", p_feature);
    keys = get_keys();
    assert(keys[1].id == 2);
    assert(strcmp(keys[1].name, "user_2") == 0);
    get_feature(2, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    db_sync();
    keys = get_keys();
    assert(keys.size() == 2);
    assert(keys[0].id == 1);
    assert(strcmp(keys[0].name, "user_1") == 0);
    assert(keys[1].id == 2);
    assert(strcmp(keys[1].name, "user_2") == 0);
    get_feature(1, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);
    get_feature(2, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    del_feature("user_1");
    keys = get_keys();
    assert(keys.size() == 1);
    assert(keys[0].id == 2);
    assert(strcmp(keys[0].name, "user_2") == 0);
    get_feature(2, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);

    db_sync();
    keys = get_keys();
    assert(keys.size() == 1);
    assert(keys[0].id == 2);
    assert(strcmp(keys[0].name, "user_2") == 0);
    get_feature(2, v_feature);
    assert(memcmp(p_feature, v_feature, OASISLT_getFaceItemSize()) == 0);
}

#else

#include "flashdb.h"

//#define TEST_DB
#define FLASH_DB 1

#ifdef TEST_DB
#include "test_db_bin.h"
#endif

#define ENABLE_DB_TEST 0

static FeatureData gFeatureData;

FeatureDB::FeatureDB(float thres)
{
    threshold = thres;

    PRINTF("[DB]:start:0x%x size:0x%x\r\n", FACEREC_FS_FIRST_SECTOR * FLASH_SECTOR_SIZE,
         ((sizeof(FeatureItem) * FEATUREDATA_MAX_COUNT  + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE + 1)* FLASH_SECTOR_SIZE);
#if 0
    Flash_FacerecFsInit();
#endif

    load_feature();

    reassign_feature();

#if ENABLE_DB_TEST
    self_test();
#endif
}

FeatureDB::~FeatureDB()
{
}

int FeatureDB::load_feature()
{
    int ret;
    int index = 0;

#ifdef TEST_DB
    #define FLASH_SECTOR_SIZE 0x1000
    // copy the map
    memcpy(&gFeatureData.map, test_db_bin, sizeof(gFeatureData.map));
    // copy the items
    memcpy(&gFeatureData.item, test_db_bin + FLASH_SECTOR_SIZE, sizeof(gFeatureData.item));
#endif

    memset(&gFeatureData, FEATUREDATA_MAGIC_UNUSE, sizeof(FeatureData));

    Flash_FacerecFsReadMapMagic(&gFeatureData.map);

    int item_max = FEATUREDATA_MAX_COUNT;

    // find valid item
    for (int i = 0; i < item_max; i++) {
        if(gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_VALID) {
            ret = Flash_FacerecFsReadItem(i, &gFeatureData.item[index++]);
            if(ret != FLASH_OK)
                return ret;
        }
    }

    PRINTF("%d valid data.\r\n", index);

#if 0
    PRINTF("\r\nRTFS List--------------------------------------------------------");
    PRINTF("\r\nValid -> ");
    for (int i = 0; i < item_max; i++) {
        if (gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_VALID) {
            PRINTF("%d,", i);
        }
    }

    PRINTF("\r\nUnuse -> ");
    for (int i = 0; i < item_max; i++) {
        if (gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_UNUSE) {
            PRINTF("%d,", i);
        }
    }

    PRINTF("\r\nDelet -> ");
    for (int i = 0; i < item_max; i++) {
        if (gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_DELET) {
            PRINTF("%d,", i);
        }
    }
    PRINTF("\r\n-----------------------------------------------------------------\r\n");
#endif

    return index;
}

int FeatureDB::reassign_feature()
{
    int size = 0;
    int item_max = FEATUREDATA_MAX_COUNT;
    // flash occupy size.
    for(int i = 0; i < item_max; i++) {
        if(gFeatureData.map.magic[i] != FEATUREDATA_MAGIC_UNUSE) {
            size++;
        }
    }

    if(size < item_max / 2) {
        return DB_SUCCESS;
    }

    Flash_FacerecFsEraseMapBlock();

    Flash_FacerecFsEraseItemBlock();

    int count = feature_count();

    memset(&gFeatureData.map, FEATUREDATA_MAGIC_UNUSE, sizeof(gFeatureData.map));

    int reassign_index = 0;
    for(int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if(reassign_index >= count) {
            break;
        }

        if(gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            gFeatureData.item[i].index = reassign_index;
            Flash_FacerecFsUpdateItem(reassign_index, &gFeatureData.item[i]);

            gFeatureData.map.magic[reassign_index] = FEATUREDATA_MAGIC_VALID;
            Flash_FacerecFsUpdateMapMagic(reassign_index, &gFeatureData.map);

            reassign_index++;
        }
    }

    return DB_SUCCESS;
}

//get free index from flash database
int FeatureDB::get_free_map()
{
    int item_max = FEATUREDATA_MAX_COUNT;
    // find new map index
    for(int i = 0; i < item_max; i++) {
        if(gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            return i;
    }

    return DB_FAIL;
}

int FeatureDB::get_remain_map()
{
    int item_max = FEATUREDATA_MAX_COUNT;
    int remain_size = 0;

    // find new map index
    for(int i = 0; i < item_max; i++) {
        if(gFeatureData.map.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            remain_size++;
    }

    return remain_size;
}

int FeatureDB::save_feature(int index)
{
#ifdef FLASH_DB
    /* 1. compare feature to confirm if content is same. */
    int old_map_index = gFeatureData.item[index].index;

    if(old_map_index == -1) {
        //means new feature, user is not in flash
    } else {
        FeatureItem *pItem_t = (FeatureItem*)pvPortMalloc(sizeof(FeatureItem));
        Flash_FacerecFsReadItem(old_map_index, pItem_t);

        //means feature is same, do not need update.
        if(memcmp(pItem_t, &gFeatureData.item[old_map_index], sizeof(FeatureItem)) == 0) {
            vPortFree(pItem_t);
            return DB_SUCCESS;
        }
        vPortFree(pItem_t);
    }

    /* 2. new feature or feature data need update. */
    //find new map index
    int new_map_index = get_free_map();

    //find new index fail, reassign feature database.
    if(new_map_index == DB_FAIL)
    {
        reassign_feature();
        new_map_index = get_free_map();
        if(new_map_index == DB_FAIL)
        {
            return DB_FULL;
        }
    }

    //update feature, first delete old feature(change magic value).
    if(old_map_index != -1) {
        // set old map magic to DELET
        gFeatureData.map.magic[old_map_index] = FEATUREDATA_MAGIC_DELET;
        Flash_FacerecFsUpdateMapMagic(old_map_index, &gFeatureData.map);

        // set old item magic to DELET
        Flash_FacerecFsUpdateItemMagic(old_map_index, FEATUREDATA_MAGIC_DELET);
    }

    // set new map magic to VALID
    gFeatureData.map.magic[new_map_index] = FEATUREDATA_MAGIC_VALID;
    Flash_FacerecFsUpdateMapMagic(new_map_index, &gFeatureData.map);

    // update new item
    gFeatureData.item[index].index = new_map_index;
    Flash_FacerecFsUpdateItem(new_map_index, &gFeatureData.item[index]);

    PRINTF("[RTFS]: %d feature item&map saved.\r\n", index);
#endif

    return DB_SUCCESS;
}

int FeatureDB::erase_feature(int index)
{
    int map_index = gFeatureData.item[index].index;
    //user is not in flash, do nothing and return with no error.
    if(map_index == -1)
    {
        return DB_SUCCESS;
    }

    // update index item magic to delet
    Flash_FacerecFsUpdateItemMagic(map_index, FEATUREDATA_MAGIC_DELET);

    // update index map magic to delet
    gFeatureData.map.magic[map_index] = FEATUREDATA_MAGIC_DELET;
    Flash_FacerecFsUpdateMapMagic(map_index, &gFeatureData.map);

    PRINTF("[RTFS]: %d feature magic deleted.\r\n", index);

    return DB_SUCCESS;
}

int FeatureDB::ren_name(const std::string oldname, const std::string newname)
{
    int index = FEATUREDATA_MAX_COUNT;
    int newIndex = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (!strcmp(oldname.c_str(), gFeatureData.item[i].name)) {
                index = i;
            }
            if (!strcmp(newname.c_str(), gFeatureData.item[i].name)) {
                newIndex = i;
            }

        }
    }

    if (index == FEATUREDATA_MAX_COUNT) {
        return DB_FAIL;
    }

    if (newIndex != FEATUREDATA_MAX_COUNT) {
        // nothing to do if the name is the same
        if (newIndex == index) {
            return DB_SUCCESS;
        }
        // skip renaming if the newname already exists
        return -2;
    }
    // rename the oldname entry
    strcpy(gFeatureData.item[index].name, newname.c_str());
    if(save_feature(index) != DB_SUCCESS)
    {
        return DB_FAIL;
    }

    return DB_SUCCESS;
}

std::vector<std::string> FeatureDB::get_names()
{
    std::vector<std::string> names;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            names.push_back(std::string(gFeatureData.item[i].name));
        }
    }

    return names;
}

int FeatureDB::feature_count()
{
    int count = 0;
#ifdef TEST_DB_PREBUILT
    count =  TESTER_NUM;
#else

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            count++;
        }
    }

#endif
    return count;
}

//get free index from ram database
int FeatureDB::get_free(int& index)
{
    index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_UNUSE) {
            index = i;
            break;
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        return DB_SUCCESS;
    }

    return DB_FAIL;
}

int FeatureDB::del_feature(uint16_t id, std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (( gFeatureData.item[i]. id == id) && (!strcmp(name.c_str(), gFeatureData.item[i].name))) {
                index = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        if (DB_SUCCESS == erase_feature(index)) {
                memset(&gFeatureData.item[index], 0xFF, sizeof(gFeatureData.item[index]));
            return DB_SUCCESS;
        }
    }

    return DB_FAIL;
}

int FeatureDB::del_feature(const std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (!strcmp(name.c_str(), gFeatureData.item[i].name)) {
                index = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        if (DB_SUCCESS == erase_feature(index)) {
            memset(&gFeatureData.item[index], 0xFF, sizeof(gFeatureData.item[index]));
            return DB_SUCCESS;
        }
    }

    return DB_FAIL;
}

int FeatureDB::del_feature_all()
{
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (DB_SUCCESS == erase_feature(i)) {
                memset(&gFeatureData.item[i], 0xFF, sizeof(gFeatureData.item[i]));
            }
        }
    }

    return DB_SUCCESS;
}

int FeatureDB::database_save(int count)
{
    int ret;

    ret = feature_count();

    if(count > ret) {
        count = ret;
    }

    if(count == 0) {
        return DB_SUCCESS;
    }

    // find the save index position.
    for(int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if(gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if(save_feature(i) != DB_SUCCESS)
            {
                PRINTF("[RTFS]: %d feature item&map save failed.\r\n", i);
            }
        }
    }

    return DB_SUCCESS;
}

int FeatureDB::add_feature(uint16_t id,const std::string name, float* feature)
{

    int index = FEATUREDATA_MAX_COUNT;
    int freeIndex = FEATUREDATA_MAX_COUNT;
    int opIndex = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (id == gFeatureData.item[i].id) {
                index = i;
            }
        } else if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_UNUSE) {
            if (freeIndex == FEATUREDATA_MAX_COUNT) {
                freeIndex = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        // update
        opIndex = index;
    } else if (freeIndex != FEATUREDATA_MAX_COUNT) {
        // add new
        opIndex = freeIndex;
    }

    if (opIndex != FEATUREDATA_MAX_COUNT) {
        gFeatureData.item[opIndex].magic  = FEATUREDATA_MAGIC_VALID;
        strcpy(gFeatureData.item[opIndex].name, name.c_str());
        gFeatureData.item[opIndex].id = id;
        memcpy(gFeatureData.item[opIndex].feature, feature, OASISLT_getFaceItemSize());
        if(save_feature(opIndex) == DB_SUCCESS)
            return opIndex;
    }

    PRINTF("[ERROR]:Database out of space");
    return DB_FAIL;
}

int FeatureDB::update_feature(uint16_t id,const std::string name, float* feature)
{
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (id == gFeatureData.item[i].id) {
                index = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        memcpy(gFeatureData.item[index].name, name.c_str(), min(name.size(), sizeof(gFeatureData.item[index].name)));
        gFeatureData.item[index].name[sizeof(gFeatureData.item[index].name)-1] = 0;
        memcpy(gFeatureData.item[index].feature, feature, OASISLT_getFaceItemSize());
        if(save_feature(index) == DB_SUCCESS)
            return DB_SUCCESS;
    }

    return DB_FAIL;
}

int FeatureDB::get_feature(uint16_t id, float* feature)
{
#ifdef TEST_DB_PREBUILT
    int index = TESTER_NUM;

    for (int i = 0; i < TESTER_NUM; i++) {
        if (id == testersFea[i].faceID) {
            index = i;
        }
    }

    if (index != TESTER_NUM) {
        memcpy(feature, &testersFea[index], OASISLT_getFaceItemSize());
        return 0;
    } else {
        memset(feature, 0x0, OASISLT_getFaceItemSize());
        return -1;
    }

#else
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (id == gFeatureData.item[i].id) {
                index = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        memcpy(feature, gFeatureData.item[index].feature, OASISLT_getFaceItemSize());
        return DB_SUCCESS;
    } else {
        memset(feature, 0x0, OASISLT_getFaceItemSize());
        return DB_FAIL;
    }

#endif
}

std::vector<uint16_t> FeatureDB::get_ids()
{
    std::vector<uint16_t> ids;
#ifdef TEST_DB_PREBUILT

    for (int i = 0; i < TESTER_NUM; i++) {
        ids.push_back(testersFea[i].faceID);
    }

#else

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            ids.push_back(gFeatureData.item[i].id);
        }
    }

#endif
    return ids;
}

int FeatureDB::get_name(uint16_t id,std::string& name)
{
#ifdef TEST_DB_PREBUILT
    char temp[32];
    sprintf(temp, "User_%u", id);
    name = temp;
    return 0;
#else
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++) {
        if (gFeatureData.item[i].magic == FEATUREDATA_MAGIC_VALID) {
            if (id == gFeatureData.item[i].id) {
                index = i;
            }
        }
    }

    if (index != FEATUREDATA_MAX_COUNT) {
        name = std::string(gFeatureData.item[index].name);
        return DB_SUCCESS;
    } else {
        name.clear();
        return DB_FAIL;
    }

#endif
}

#if ENABLE_DB_TEST
static FeatureData test_feature_data;

void FeatureDB::reset_env(void)
{
    Flash_FacerecFsEraseMapBlock();

    Flash_FacerecFsEraseItemBlock();

    load_feature();

    reassign_feature();
}

void compare_db(void)
{
    Flash_FacerecFsReadMapMagic(&test_feature_data.map);

    if(memcmp(&test_feature_data.map, &gFeatureData.map, sizeof(FeatureMap)) != 0){
        assert(0);
    }

    int item_max = FEATUREDATA_MAX_COUNT;
    int index = 0;

    // find valid item
    for (int i = 0; i < item_max; i++)
    {
        if(test_feature_data.map.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            Flash_FacerecFsReadItem(i, &test_feature_data.item[index++]);
        }
    }

    int i_t = 0, i_f = 0;
    for (; i_f < FEATUREDATA_MAX_COUNT; i_f++)
    {
        for(int i = 0; i < index; i++)
        {
            if(memcmp(&gFeatureData.item[i_f], &test_feature_data.item[i], sizeof(FeatureItem)) == 0)
            {
                i_t++;
            }
        }
    }

    assert(i_t == index);
}

void FeatureDB::self_test()
{
    float *p_feature = (float*)pvPortMalloc(OASISLT_getFaceItemSize());

    memset(p_feature, 0xff, OASISLT_getFaceItemSize());

//    add_feature(1, "x_1", p_feature);
//    add_feature(3, "x_2", p_feature);
//    add_feature(8, "x_3", p_feature);
//    add_feature(7, "x_4", p_feature);
//    add_feature(5, "x_5", p_feature);
//    int id = -1;
//    std::vector<std::uint16_t> ids = get_ids();
//
//    if(ids.size() == 0)
//    {
//        id = 1;
//    }
//    else
//    {
//        std::sort(ids.begin(), ids.end());
//        for(vector<uint16_t>::iterator it=ids.begin(); it != ids.end(); it++)
//        {
//            if(*it == (std::distance(ids.begin(), it) + 1))
//            {
//                id = std::distance(ids.begin(), it) + 2;
//            }
//            else
//            {
//                break;
//            }
//        }
//    }

    //case1: add 0
    reset_env();
    add_feature(0, "user_0", p_feature);
    compare_db();

    //case2: add 0,,,max_count-1
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    compare_db();

    //case3: add 0,,,max_count
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT+1; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    compare_db();

    //case4: add 0; del 0;
    reset_env();
    add_feature(0, "user_0", p_feature);
    del_feature("user_0");
    compare_db();

    //case5: add 0,,,max_count-1; del 0;
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    del_feature("user_0");
    compare_db();

    //case6: add 0,,,max_count; del 0;
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT+1; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    del_feature("user_0");
    compare_db();

    //case7: add 0,,,max_count-1; del 0; add 0;
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    del_feature("user_0");
    add_feature(0, "user_0", p_feature);
    compare_db();

    //case8: add 0,,,max_count; del 0; add 0;
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT+1; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    del_feature("user_0");
    add_feature(0, "user_0", p_feature);
    compare_db();

    //case9: add 0,,,max_count/2-1; del 0; add max_count/2,,,max_count-1; add 0;
    reset_env();
    for(int i = 0; i < FEATUREDATA_MAX_COUNT/2; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    del_feature("user_0");
    compare_db();
    for(int i = FEATUREDATA_MAX_COUNT/2; i < FEATUREDATA_MAX_COUNT; i++)
    {
        char name_tmp[32] = {0};
        sprintf(name_tmp, "user_%d", i);
        add_feature(i, name_tmp, p_feature);
    }
    compare_db();
    add_feature(0, "user_0", p_feature);
    compare_db();

    //case10: add 1; upd 1;
    reset_env();
    add_feature(1, "user_1", p_feature);
    update_feature(1, "user_upd", p_feature);
    compare_db();


    vPortFree(p_feature);
}
#endif
#endif
