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

#include "database.h"
#include "database_api.h"
#include "featuredb.h"
#include "commondef.h"
#include "fsl_log.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include <algorithm>

static FeatureDB* gDB = NULL;
static SemaphoreHandle_t gDB_Lock = NULL;

static int DB_Lock()
{
    if (NULL == gDB)
    {
        return DB_MGMT_NODB;
    }

    if (NULL == gDB_Lock)
    {
        return DB_MGMT_ENOLOCK;
    }

    if (pdTRUE != xSemaphoreTake(gDB_Lock, portMAX_DELAY))
    {
        return DB_MGMT_ERETRY;
    }

    return DB_MGMT_OK;
}

static void DB_UnLock()
{
    xSemaphoreGive(gDB_Lock);
}

static void DB_ID2Name(char* name, uint32_t length,uint16_t id)
{
	snprintf(name,length,"user_%u",id);
}

int DB_Init()
{
    gDB = new FeatureDB(THRESHOLD_PASS);

    if (!gDB) {
        LOGE("[ERROR]:Create DB failed\r\n");
        return DB_MGMT_NODB;
    }

    // Create a database lock semaphore
    if (NULL == gDB_Lock)
    {
        gDB_Lock = xSemaphoreCreateMutex();
        if (NULL == gDB_Lock)
        {
            LOGE("[ERROR]:Create DB lock semaphore\r\n");
            return DB_MGMT_ENOLOCK;
        }
    }

    return DB_MGMT_OK;
}

void DB_Exit()
{
    if (gDB) {
        LOGE("[ERROR]:Create DB failed\r\n");
        delete gDB;
        gDB = NULL;
    }

    // Delete thes database lock semaphore
    if (NULL != gDB_Lock)
    {
        vSemaphoreDelete(gDB_Lock);
        gDB_Lock = NULL;
    }
}

int DB_Del(uint16_t id, std::string name)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret =  gDB->del_feature(id, name);
        DB_UnLock();
    }

    return ret;
}

int DB_Del(std::string name)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret =  gDB->del_feature(name);
        DB_UnLock();
    }

    return ret;
}

int DB_DelAll()
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if(DB_MGMT_OK == ret)
    {
        ret = gDB->del_feature_all();
        DB_UnLock();
    }

    return ret;
}



int DB_Ren(const std::string oldname, const std::string newname)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = gDB->ren_name(oldname, newname);
        DB_UnLock();
    }

    return ret;
}

int DB_GetFree(int& index)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = gDB->get_free(index);
        DB_UnLock();
    }

    return ret;
}


int DB_GetNames(std::vector<std::string> *names)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        *names = gDB->get_names();
        DB_UnLock();
    }

    return ret;
}

int DB_Count(int *count)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        *count = gDB->feature_count();
        DB_UnLock();
    }

    return ret;
}

int DB_Save(int count)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if(DB_MGMT_OK == ret)
    {
        if(count == 0)
            count = gDB->feature_count();

        ret = gDB->database_save(count);
        DB_UnLock();
    }

    return ret;

}



int DB_GetFeature(uint16_t id, float* feature)
{
	int ret = DB_MGMT_FAILED;

	ret = DB_Lock();
	if (DB_MGMT_OK == ret)
	{
		ret = gDB->get_feature(id, feature);
		DB_UnLock();
	}

	return ret;

}


/*this interface is used to add user without name*/
int DB_Add(uint16_t id, float* feature)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
    	char name[FEATUREDATA_NAME_MAX_LEN];
    	DB_ID2Name(name,sizeof(name),id);
        ret = gDB->add_feature(id,name, feature);
        /* if ret = -1 database is out of space */
        if (-1 == ret)
            ret = DB_MGMT_NOSPACE;
        DB_UnLock();
    }

    return ret;

}

/*this interface is used to add user with given name*/
int DB_Add(uint16_t id, std::string name, float* feature)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = gDB->add_feature(id,name, feature);
        /* if ret = -1 database is out of space */
        if (-1 == ret)
            ret = DB_MGMT_NOSPACE;
        DB_UnLock();
    }

    return ret;

}

int DB_Update(uint16_t id, float* feature)
{
    int ret = DB_MGMT_FAILED;
    std::string name;
    DB_GetName(id,name);

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = gDB->update_feature(id,name,feature);
        DB_UnLock();
    }

    return ret;
}

int DB_GetIDs(std::vector<uint16_t> &ids)
{

    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ids = gDB->get_ids();
        DB_UnLock();
    }

    return ret;

}

int DB_GetName(uint16_t id,std::string& name)
{
	int ret = DB_MGMT_FAILED;

	ret = DB_Lock();
	if (DB_MGMT_OK == ret)
	{
		ret = gDB->get_name(id, name);
		DB_UnLock();
	}

	return ret;

}

int DB_GenID(uint16_t *id)
{
    int ret = DB_MGMT_FAILED;
    std::vector<std::uint16_t> ids;
    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ids = gDB->get_ids();
        DB_UnLock();
    }

    if(ids.size() == 0)
    {
        *id = 1;
    }
    else
    {
        std::vector<std::uint16_t>::iterator biggest = std::max_element(ids.begin(), ids.end());
        *id =  *biggest + 1;

//        // id must start from 1
//        // if id is continuously, new_id = biggest_id + 1;
//        // if id is not continuously, new_id = fist miss id;
//        std::sort(ids.begin(), ids.end());
//        for(vector<uint16_t>::iterator it=ids.begin(); it != ids.end(); it++)
//        {
//            if(*it == (std::distance(ids.begin(), it) + 1))
//            {
//                *id = std::distance(ids.begin(), it) + 2;
//            }
//            else
//            {
//                break;
//            }
//        }
    }

    if(*id >= FEATUREDATA_MAX_COUNT)
        ret = DB_MGMT_FAILED;
    return ret;
}

