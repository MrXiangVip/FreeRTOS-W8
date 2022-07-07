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

/*******************************************************************************
 * Variables
 *******************************************************************************/

static FeatureDB *s_DB             = NULL;
static SemaphoreHandle_t s_DB_Lock = NULL;

static char *logtag ="[DataBase] ";
/*******************************************************************************
 * Code
 *******************************************************************************/
static int DB_Lock()
{
    if (NULL == s_DB)
    {
        return DB_MGMT_NODB;
    }

    if (NULL == s_DB_Lock)
    {
        return DB_MGMT_ENOLOCK;
    }

    if (pdTRUE != xSemaphoreTake(s_DB_Lock, portMAX_DELAY))
    {
        return DB_MGMT_ERETRY;
    }

    return DB_MGMT_OK;
}

static void DB_UnLock()
{
    xSemaphoreGive(s_DB_Lock);
}

static void DB_ID2Name(char *name, uint32_t length, uint16_t id)
{
    snprintf(name, length, "user_%u", id);
    LOGD("%s :DB_ID2Name %d, %s\r\n", logtag, id, name);
}

int DB_Init()
{
    s_DB = new FeatureDB();

    if (!s_DB)
    {
        LOGE("[ERROR]:Create DB failed\r\n");
        return DB_MGMT_NODB;
    }

    // Create a database lock semaphore
    if (NULL == s_DB_Lock)
    {
        s_DB_Lock = xSemaphoreCreateMutex();
        if (NULL == s_DB_Lock)
        {
            LOGE("[ERROR]:Create DB lock semaphore\r\n");
            return DB_MGMT_ENOLOCK;
        }
    }

    return DB_MGMT_OK;
}

void DB_Exit()
{
    if (s_DB)
    {
        LOGE("[ERROR]:Create DB failed\r\n");
        delete s_DB;
        s_DB = NULL;
    }

    // Delete thes database lock semaphore
    if (NULL != s_DB_Lock)
    {
        vSemaphoreDelete(s_DB_Lock);
        s_DB_Lock = NULL;
    }
}

int DB_Del(uint16_t id, std::string name)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->del_feature(id, name);
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
        ret = s_DB->del_feature(name);
        DB_UnLock();
    }

    return ret;
}

int DB_DelAll()
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->del_feature_all();
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
        ret = s_DB->ren_name(oldname, newname);
        DB_UnLock();
    }

    return ret;
}

int DB_GetFree(int &index)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->get_free(index);
        DB_UnLock();
    }

    return ret;
}

int DB_GetNames(std::vector<std::string> *names)
{
    LOGD("%s DB_GetNames  \r\n", logtag );

    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        *names = s_DB->get_names();
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
        *count = s_DB->feature_count();
        DB_UnLock();
    }

    return ret;
}

int DB_Save(int count)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        if (count == 0)
            count = s_DB->feature_count();

        ret = s_DB->database_save(count);
        DB_UnLock();
    }

    return ret;
}

int DB_GetFeature(uint16_t id, float *feature)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->get_feature(id, feature);
        DB_UnLock();
    }

    return ret;
}

/*this interface is used to add user without name*/
int DB_Add(uint16_t id, float *feature)
{
    LOGD("[%s] DB_Add id %d  \r\n", logtag, id );
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        char name[FEATUREDATA_NAME_MAX_LEN];
        DB_ID2Name(name, sizeof(name), id);
        ret = s_DB->add_feature(id, name, feature);
        /* if ret = -1 database is out of space */
        if (-1 == ret)
            ret = DB_MGMT_NOSPACE;
        DB_UnLock();
    }

    return ret;
}

/*this interface is used to add user with given name*/
int DB_Add(uint16_t id, std::string name, float *feature)
{
    LOGD("[%s] DB_Add id %d , name %s \r\n", logtag, id, name.c_str());

    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->add_feature(id, name, feature);
        /* if ret = -1 database is out of space */
        if (-1 == ret)
            ret = DB_MGMT_NOSPACE;
        DB_UnLock();
    }

    return ret;
}

int DB_Update(uint16_t id, float *feature)
{
    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        std::string name;
        ret = s_DB->get_name(id, name);
        ret = s_DB->update_feature(id, name, feature);
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
        ids = s_DB->get_ids();
        DB_UnLock();
    }

    return ret;
}


int DB_GetID_FeaturePointers(uint16_t* ids,void**pFeatures,int num)
{
    int ret = DB_MGMT_FAILED;
    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
    	s_DB->get_ID_featurePointers(ids,pFeatures,num);
        DB_UnLock();
    }

    return ret;


}

int DB_GetName(uint16_t id, std::string &name)
{
    LOGD("%s DB_GetName id %d  \r\n", logtag, id);

    int ret = DB_MGMT_FAILED;

    ret = DB_Lock();
    if (DB_MGMT_OK == ret)
    {
        ret = s_DB->get_name(id, name);
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
        ids = s_DB->get_ids();
        DB_UnLock();
    }
    if (ids.size() == 0)
    {
        *id = 1;
    }
    else
    {
        std::vector<std::uint16_t>::iterator biggest = std::max_element(ids.begin(), ids.end());
        *id                                          = *biggest + 1;
    }
    return ret;
}

int DB_SetAutoSave(bool auto_save)
{
    int ret = DB_MGMT_FAILED;
    if (s_DB != NULL)
    {
        s_DB->set_autosave(auto_save);
        ret = DB_MGMT_OK;
    }

    return ret;
}

int DB_save_feature(float *feature) {
    int ret = DB_MGMT_FAILED;
    if (s_DB != NULL)
    {
        s_DB->save_feature(feature);
        ret = DB_MGMT_OK;
    }
    return ret;
}

int DB_GetFeature_ByName(std::string name, float* feature)
{
    LOGD("%s, %s , %s \r\n",logtag, __func__, name.c_str());
    std::vector<uint16_t> allIDs;
    DB_GetIDs(allIDs);
    for (uint32_t i = 0; i < allIDs.size(); i++) {
        string tmpName;
        // get idx+i ids form DB
        DB_GetName(allIDs[i], tmpName);
        LOGD("%s id %d ,tmp name [%s] , name[%s]\r\n",logtag, i, tmpName.c_str(), name.c_str() );
        if( tmpName.compare( name ) == 0 ){
            DB_GetFeature(allIDs[i], feature);
            LOGD("%s id %d  \r\n",logtag, i);
            return DB_MGMT_OK;
        }
    }
    return DB_MGMT_FAILED;
}

int DB_AddFeature_WithName( std::string name, float* feature )
{
    LOGD("%s, %s , %s \r\n",logtag, __func__, name.c_str());
    uint16_t    id_local;
    int ret     = DB_GenID(&id_local);
    if (ret < 0)
    {
        LOGD("db gen id error \r\n");
        return DB_MGMT_FAILED;
    }

    ret   = DB_Add(id_local, name, feature);
    if (ret != 0)
    {
        LOGD("remote add user into local fail, [%d]: [%s]\r\n", id_local, name.c_str());
        return DB_MGMT_FAILED;
    }
    LOGD("add user into db success, [%d]: [%s]\r\n", id_local, name.c_str() );
    return  DB_MGMT_OK;
}
