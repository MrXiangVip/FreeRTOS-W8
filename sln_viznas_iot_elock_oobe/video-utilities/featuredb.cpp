/*
 * Copyright 2019-2021 NXP.
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

#include "commondef.h"

#define RTFFI_USE_FATFS 1

#if RTFFI_USE_FATFS
#include "fatfs_op.h"

static FeatureMap s_FeatureMap;
#define SDRAM_DB 1
#if SDRAM_DB
static FeatureItem s_FeatureItem[FEATUREDATA_MAX_COUNT];
#endif
#define MAP_FILE_NAME "featureMap"

#define FEATURE_FILE_INDEX_LENGTH  10
#define FEATURE_FILE_SUFFIX ".db"
#define FEATURE_FILE_NAME_LEN (FEATURE_FILE_INDEX_LENGTH + strlen(FEATURE_FILE_SUFFIX)+1)

extern uint8_t remote_change_fs;

static int File_FacerecFsReadMap(FeatureMap *pMap)
{
    int ret;
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }
    fatfs_opendir("/");
    ret = fatfs_read(MAP_FILE_NAME, (char *)pMap, 0,sizeof(FeatureMap));
    fatfs_closedir();
    return ret;
}

static int File_FacerecFsUpdateMap(FeatureMap *pMap)
{
    int ret;
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    fatfs_opendir("/");
    ret = fatfs_write(MAP_FILE_NAME, (char *)pMap, 0,sizeof(FeatureMap));
    fatfs_closedir();
    return ret;
}

static int File_FacerecFsEraseMap(void)
{
    FeatureMap map;
    memset(&map, FEATUREDATA_MAGIC_UNUSE, sizeof(FeatureMap));
    File_FacerecFsUpdateMap(&map);
    return 0;
}

static int File_FacerecFsUpdateMapMagic(int index, FeatureMap *pMap)
{
    int ret;
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }

    fatfs_opendir("/");
    ret = fatfs_write(MAP_FILE_NAME, &pMap->magic[index], index ,sizeof(char));
    fatfs_closedir();
    return ret;
}

static int File_FacerecFsReadItem(int index, FeatureItem *pItem)
{
    int ret;
    int offset = offsetof(FeatureItem,feature);
    char file_name[FEATURE_FILE_NAME_LEN];
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }
    fatfs_opendir("/");
    sprintf(file_name, "%010d%s", index, FEATURE_FILE_SUFFIX);
    ret = fatfs_read(file_name, (char *)pItem, 0, offset + OASISLT_getFaceItemSize());
    fatfs_closedir();
    return ret;
}

static int File_FacerecFsUpdateItem(int index, FeatureItem *pItem)
{
    int ret;
    int offset = offsetof(FeatureItem,feature);
    char file_name[FEATURE_FILE_NAME_LEN];
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }
    fatfs_opendir("/");
    sprintf(file_name, "%010d%s", index, FEATURE_FILE_SUFFIX);
    ret = fatfs_write(file_name, (char *)pItem, 0, offset + OASISLT_getFaceItemSize());
    fatfs_closedir();
    return ret;
}

static int File_FacerecFsReadItemHeader(int index, FeatureItem *pItem)
{
    int ret;
    int offset = offsetof(FeatureItem,feature);
    char file_name[FEATURE_FILE_NAME_LEN];
    if (remote_change_fs)
    {
        fatfs_mount_with_mkfs();
        remote_change_fs = 0;
    }
    fatfs_opendir("/");
    sprintf(file_name, "%010d%s", index, FEATURE_FILE_SUFFIX);
    ret = fatfs_read(file_name, (char *)pItem, 0, offset);
    fatfs_closedir();
    return ret;
}

FeatureDB::FeatureDB()
{
    fatfs_mount_with_mkfs();
    load_feature();
    reassign_feature();
}

FeatureDB::~FeatureDB()
{
}

int FeatureDB::add_feature(uint16_t id, const std::string name, float *feature)
{
    reassign_feature();
    int index = get_free_mapmagic();
    if( index == -1)
    {
        LOGD("[ERROR]:Database space is full");
        return -1;
    }
    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_VALID;
    
    FeatureItem item_t;
    item_t.id = id;
    item_t.index = index;
    strcpy(item_t.name, name.c_str());
    memcpy(item_t.feature, feature, OASISLT_getFaceItemSize());
#if SDRAM_DB
    memcpy(&s_FeatureItem[index], &item_t, sizeof(item_t));
#endif
    File_FacerecFsUpdateItem(index,&item_t);
    File_FacerecFsUpdateMapMagic(index, &s_FeatureMap);
    return 0;
}

int FeatureDB::update_feature(uint16_t id, const std::string name, float *feature)
{
    int ret;
    ret = del_feature(id, name);
    if (ret == -1)
    {
        return ret;
    }
    ret = add_feature(id, name, feature);
    return ret;
}

int FeatureDB::del_feature(uint16_t id, std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;
 
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItem(i,&item_t);
#endif
            if ((item_t.id == id) && (!strcmp(name.c_str(), item_t.name)))
            {
                index = i;
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
        return -1;

    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_DELET;
    File_FacerecFsUpdateMapMagic(index, &s_FeatureMap);
    return 0;
}

int FeatureDB::del_feature(const std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItem(i,&item_t);
#endif
            if (!strcmp(name.c_str(), item_t.name))
            {
                index = i;
                break;
            }
        }
    }
    if (index == FEATUREDATA_MAX_COUNT)
        return -1;

    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_DELET;
    File_FacerecFsUpdateMapMagic(index, &s_FeatureMap);
    return 0;
}

int FeatureDB::del_feature_all()
{
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            s_FeatureMap.magic[i] = FEATUREDATA_MAGIC_DELET;
            File_FacerecFsUpdateMapMagic(i, &s_FeatureMap);
        }
    }
    return 0;
}

std::vector<std::string> FeatureDB::get_names()
{
    FeatureItem item_t;
    std::vector<std::string> names;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItemHeader(i,&item_t);
#endif
            names.push_back(std::string(item_t.name));
        }
    }
    return names;
}

int FeatureDB::get_name(uint16_t id, std::string &name)
{
    FeatureItem item_t;
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItemHeader(i,&item_t);
#endif
            if (id == item_t.id)
            {
                index = i;
                name = std::string(item_t.name);
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        name.clear();
        return -1;
    }
    else
    {
        return index;
    }
}

int FeatureDB::get_ID_featurePointers(uint16_t* ids, void**featureP, int num)
{
    int index = 0;
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (index >= num)
        {
            break;
        }

        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            ids[index] = s_FeatureItem[i].id;
            featureP[index] = s_FeatureItem[i].feature;
#else
            //File_FacerecFsReadItem(i, &s_OneFeatureItem);
            //ids[index] = s_OneFeatureItem.id;
            //featureP[index] = s_OneFeatureItem.feature;
#endif
            index++;
        }
    }

    return 0;
}

std::vector<uint16_t> FeatureDB::get_ids()
{
    FeatureItem item_t;
    std::vector<uint16_t> ids;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItemHeader(i,&item_t);
#endif
            ids.push_back(item_t.id);
        }
    }
    return ids;
}

int FeatureDB::ren_name(const std::string oldname, const std::string newname)
{
    int ret;
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItem(i,&item_t);
#endif
            if (!strcmp(oldname.c_str(), item_t.name))
            {
                index = i;
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        return -1;
    }

    ret = del_feature(item_t.id, item_t.name);
    if (ret == -1)
    {
        return ret;
    }
    strcpy(item_t.name, newname.c_str());

    ret = add_feature(item_t.id, item_t.name, item_t.feature);

    return ret;
}

int FeatureDB::feature_count()
{
    int count = 0;
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            count++;
        }
    }
    return count;
}

int FeatureDB::get_free(int &index)
{
    index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
        {
            index = i;
            break;
        }
    }

    if (index != FEATUREDATA_MAX_COUNT)
    {
        return 0;
    }

    return -1;
}

int FeatureDB::database_save(int count)
{
    // discard
    return 0;
}

int FeatureDB::get_feature(uint16_t id, float *feature)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            File_FacerecFsReadItem(i,&item_t);
#endif
            if (id == item_t.id)
            {
                index = i;
                memcpy(feature, item_t.feature, OASISLT_getFaceItemSize());
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        return -1;
    }
    else
    {
        return index;
    }

}

int FeatureDB::load_feature()
{
    memset(&s_FeatureMap, FEATUREDATA_MAGIC_UNUSE, sizeof(s_FeatureMap));
    int ret = File_FacerecFsReadMap(&s_FeatureMap);
    if (ret == 0)
    {
#if SDRAM_DB
        memset(s_FeatureItem, FEATUREDATA_MAGIC_UNUSE, sizeof(s_FeatureItem));
        for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
        {
            if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
            {
                ret = File_FacerecFsReadItem(i, &s_FeatureItem[i]);
                if (ret != 0)
                {
                    s_FeatureMap.magic[i] = FEATUREDATA_MAGIC_UNUSE;
                    File_FacerecFsUpdateMapMagic(i,&s_FeatureMap);
                }
            }
        }
#endif
    }
    else 
    {
        File_FacerecFsUpdateMap(&s_FeatureMap);
    }
    return 0;
}

int FeatureDB::erase_feature(int index)
{
    // discard
    return 0;
}

int FeatureDB::save_feature(int index)
{
    // discard
    return 0;
}

int FeatureDB::reassign_feature()
{
    int unuse_size     = 0;
    int valid_size     = 0;
    int delet_size     = 0;
    int other_size     = 0;
    int item_max = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
        {
            unuse_size++;
        }
        else if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            valid_size++;
        }
        else if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_DELET)
        {
            delet_size++;
        }
        else
        {
            other_size++;
        }
    }

    if (other_size > 0)
    {
        LOGD("Feature Map is broken!\r\n");
        return 0;
    }

    if (unuse_size > 0)
    {
        LOGD("space is enough, no need to reassign_feature!\r\n");
        return 0;
    }

    if (delet_size == 0)
    {
        LOGD("no deleted db, no need to reassign_feature!\r\n");
        return 0;
    }

    //remove the deleted db
    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_DELET)
        {
            s_FeatureMap.magic[i] = FEATUREDATA_MAGIC_UNUSE;
            FeatureItem item_t;
            memset(&item_t, FEATUREDATA_MAGIC_UNUSE, sizeof(item_t));
#if SDRAM_DB
            memset(&s_FeatureItem[i], FEATUREDATA_MAGIC_UNUSE, sizeof(item_t));
#endif
            File_FacerecFsUpdateItem(i, &item_t);
            File_FacerecFsUpdateMapMagic(i, &s_FeatureMap);
        }
    }
    return 0;


}

int FeatureDB::get_free_mapmagic()
{
    int item_max = FEATUREDATA_MAX_COUNT;
    // find new map index
    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            return i;
    }

    return -1;
}
int FeatureDB::get_remain_map()
{
    int remain_size = 0;
    // find new map index
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            remain_size++;
    }
    return remain_size;
}
void FeatureDB::set_autosave(bool auto_save)
{
  return;
}

bool FeatureDB::get_autosave()
{
    return 1;
}

#else
#include "flashdb.h"

//#define TEST_DB
#ifdef TEST_DB
#include "test_db_bin.h"
#endif

/*******************************************************************************
 * Definitions
 *******************************************************************************/
#define SDRAM_DB 0

#ifdef TEST_DB
#undef SDRAM_DB
#define SDRAM_DB 1
#define FLASH_SECTOR_SIZE 0x1000
#endif
/*******************************************************************************
 * Variables
 *******************************************************************************/
static FeatureMap s_FeatureMap;


#if SDRAM_DB
static FeatureItem s_FeatureItem[FEATUREDATA_MAX_COUNT];
#endif

/*******************************************************************************
 * Code
 *******************************************************************************/

FeatureDB::FeatureDB()
{
    PRINTF("[DB]:start:0x%x size:0x%x\r\n", FACEREC_FS_FIRST_SECTOR * FLASH_SECTOR_SIZE,
           ((sizeof(FeatureItem) * FEATUREDATA_MAX_COUNT + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE + 1) *
               FLASH_SECTOR_SIZE);
#if 0
    Flash_FacerecFsInit();
#endif
    this->auto_save = false;
    load_feature();
    reassign_feature();
}

FeatureDB::~FeatureDB()
{
}

int FeatureDB::load_feature()
{
#ifdef TEST_DB
    // copy the map
    memcpy(&s_FeatureMap, test_db_bin, sizeof(s_FeatureMap));
    // copy the items
    memcpy(s_FeatureItem, test_db_bin + FLASH_SECTOR_SIZE, sizeof(s_FeatureItem));
#else
    memset(&s_FeatureMap, FEATUREDATA_MAGIC_UNUSE, sizeof(s_FeatureMap));
    Flash_FacerecFsReadMapMagic(&s_FeatureMap);
#if SDRAM_DB
    memset(s_FeatureItem, FEATUREDATA_MAGIC_UNUSE, sizeof(s_FeatureItem));
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        Flash_FacerecFsReadItem(i, &s_FeatureItem[i]);
    }
#endif
#endif
    return 0;
}

int FeatureDB::reassign_feature()
{
    int unuse_size     = 0;
    int valid_size     = 0;
    int delet_size     = 0;
    int other_size     = 0;
    int item_max = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
        {
            unuse_size++;
        }
        else if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            valid_size++;
        }
        else if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_DELET)
        {
            delet_size++;
        }
        else
        {
            other_size++;
        }
    }

    if (other_size > 0)
    {
        Flash_FacerecFsEraseAllBlock();
        LOGD("Flash is not clear, need to erase flash first!\r\n");
        return 0;
    }

    if (unuse_size > 0)
    {
        LOGD("space is enough, no need to reassign_feature!\r\n");
        return 0;
    }

    if (delet_size == 0)
    {
        LOGD("no deleted db, no need to reassign_feature!\r\n");
        return 0;
    }

    //remove the deleted db
    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_DELET)
        {
            s_FeatureMap.magic[i] = FEATUREDATA_MAGIC_UNUSE;
            FeatureItem item_t;
            memset(&item_t, FEATUREDATA_MAGIC_UNUSE, sizeof(item_t));
#if SDRAM_DB
            memset(&s_FeatureItem[i], FEATUREDATA_MAGIC_UNUSE, sizeof(item_t));
#endif
            Flash_FacerecFsUpdateItem(i, &item_t, true);
            Flash_FacerecFsUpdateMapMagic(i, &s_FeatureMap, true);
        }
    }
    return 0;
}

int FeatureDB::get_free_mapmagic()
{
    int item_max = FEATUREDATA_MAX_COUNT;
    // find new map index
    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            return i;
    }

    return -1;
}

int FeatureDB::get_remain_map()
{
    int item_max    = FEATUREDATA_MAX_COUNT;
    int remain_size = 0;

    // find new map index
    for (int i = 0; i < item_max; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
            remain_size++;
    }

    return remain_size;
}

int FeatureDB::save_feature(int index)
{
    return 0;
}

int FeatureDB::erase_feature(int index)
{
    return 0;
}

int FeatureDB::ren_name(const std::string oldname, const std::string newname)
{
    int ret;
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItem(i,&item_t);
#endif
            if (!strcmp(oldname.c_str(), item_t.name))
            {
                index = i;
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        return -1;
    }

    ret = del_feature(item_t.id, item_t.name);
    if (ret == -1)
    {
        return ret;
    }
    strcpy(item_t.name, newname.c_str());

    ret = add_feature(item_t.id, item_t.name, item_t.feature);

    return ret;
}

std::vector<std::string> FeatureDB::get_names()
{
    FeatureItem item_t;
    std::vector<std::string> names;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItemHeader(i,&item_t);
#endif
            names.push_back(std::string(item_t.name));
        }
    }
    return names;
}

int FeatureDB::feature_count()
{
    int count = 0;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            count++;
        }
    }

    return count;
}

int FeatureDB::get_free(int &index)
{
    index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_UNUSE)
        {
            index = i;
            break;
        }
    }

    if (index != FEATUREDATA_MAX_COUNT)
    {
        return 0;
    }

    return -1;
}

int FeatureDB::del_feature(uint16_t id, std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;
 
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItem(i,&item_t);
#endif
            if ((item_t.id == id) && (!strcmp(name.c_str(), item_t.name)))
            {
                index = i;
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
        return -1;

    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_DELET;
    Flash_FacerecFsUpdateMapMagic(index, &s_FeatureMap, false);
    return 0;
}

int FeatureDB::del_feature(const std::string name)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItem(i,&item_t);
#endif
            if (!strcmp(name.c_str(), item_t.name))
            {
                index = i;
                break;
            }
        }
    }


    if (index == FEATUREDATA_MAX_COUNT)
        return -1;

    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_DELET;
    Flash_FacerecFsUpdateMapMagic(index, &s_FeatureMap, false);
    return 0;
}

int FeatureDB::del_feature_all()
{
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
            s_FeatureMap.magic[i] = FEATUREDATA_MAGIC_DELET;
            Flash_FacerecFsUpdateMapMagic(i, &s_FeatureMap, false);
        }
    }

    return 0;
}

int FeatureDB::database_save(int count)
{
    return 0;
}

int FeatureDB::add_feature(uint16_t id, const std::string name, float *feature)
{
    reassign_feature();

    int index = get_free_mapmagic();
    if( index == -1)
    {
        LOGD("[ERROR]:Database space is full");
        return -1;
    }
    s_FeatureMap.magic[index] = FEATUREDATA_MAGIC_VALID;

    FeatureItem item_t;
    item_t.id = id;
    item_t.index = index;
    strcpy(item_t.name, name.c_str());
    memcpy(item_t.feature, feature, OASISLT_getFaceItemSize());
#if SDRAM_DB
    memcpy(&s_FeatureItem[index], &item_t, sizeof(item_t));
#endif
    Flash_FacerecFsUpdateItem(index,&item_t, false);
    Flash_FacerecFsUpdateMapMagic(index, &s_FeatureMap, false);
    return 0;
}

int FeatureDB::update_feature(uint16_t id, const std::string name, float *feature)
{
    int ret;
    ret = del_feature(id, name);
    if (ret == -1)
    {
        return ret;
    }
    ret = add_feature(id, name, feature);
    return ret;
}

int FeatureDB::get_feature(uint16_t id, float *feature)
{
    int index = FEATUREDATA_MAX_COUNT;
    FeatureItem item_t;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItem(i,&item_t);
#endif
            if (id == item_t.id)
            {
                index = i;
                memcpy(feature, item_t.feature,OASISLT_getFaceItemSize());
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        return -1;
    }
    else
    {
        return index;
    }
}


int FeatureDB::get_ID_featurePointers(uint16_t* ids, void**featureP, int num)
{
    int index = 0;
    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
    	if (index >= num)
    	{
    		break;
    	}

        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            ids[index] = s_FeatureItem[i].id;
            featureP[index] = s_FeatureItem[i].feature;
#else
            Flash_FacerecFsReadIDFeaturePointer(i,&ids[index],&featureP[index]);
#endif
            index++;
        }
    }

    return 0;
}

std::vector<uint16_t> FeatureDB::get_ids()
{
    FeatureItem item_t;
    std::vector<uint16_t> ids;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItemHeader(i,&item_t);
#endif
            ids.push_back(item_t.id);
        }
    }
    return ids;
}

int FeatureDB::get_name(uint16_t id, std::string &name)
{
    FeatureItem item_t;
    int index = FEATUREDATA_MAX_COUNT;

    for (int i = 0; i < FEATUREDATA_MAX_COUNT; i++)
    {
        if (s_FeatureMap.magic[i] == FEATUREDATA_MAGIC_VALID)
        {
#if SDRAM_DB
            memcpy(&item_t, &s_FeatureItem[i], sizeof(item_t));
#else
            Flash_FacerecFsReadItemHeader(i,&item_t);
#endif
            if (id == item_t.id)
            {
                index = i;
                name = std::string(item_t.name);
                break;
            }
        }
    }

    if (index == FEATUREDATA_MAX_COUNT)
    {
        name.clear();
        return -1;
    }
    else
    {
        return index;
    }
}

void FeatureDB::set_autosave(bool auto_save)
{
    this->auto_save = auto_save;
}

bool FeatureDB::get_autosave()
{
    return this->auto_save;
}
#endif
