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

#pragma once
#ifndef _FEATUREDB_H_
#define _FEATUREDB_H_

#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <assert.h>
#include "fsl_common.h"
#include "generic_list.h"
#include "commondef.h"

using namespace std;

typedef enum {
    DB_SUCCESS = 0,
    DB_FAIL = -1,
    DB_FULL = -2,
    DB_LINK_FULL = -3,

} featuredb_status_t;

typedef list_t db_context_handle_t;

typedef enum
{
    DB_ADD    = 1,
    DB_DELETE = 2,
    DB_UPDATE = 3,
    DB_RENAME = 4
} db_op_type_t;

typedef struct
{
    uint16_t id;
    char oldname[FEATUREDATA_NAME_MAX_LEN];
    char name[FEATUREDATA_NAME_MAX_LEN];
    float *feature;
} db_content_t;

typedef struct
{
    db_op_type_t op;
    db_content_t *content;
    list_element_t link;
} db_op_link_t;

typedef struct
{
    uint16_t id;
    char name[FEATUREDATA_NAME_MAX_LEN];
} db_key_t;

class FeatureDB
{
public:
    FeatureDB(float thres);
    ~FeatureDB();
    int add_feature(uint16_t id,const string name, float* feature);
    int update_feature(uint16_t id,const string name, float* feature);
    int del_feature(uint16_t id, string name);
    int del_feature(const string name);
    int del_feature_all();
    int ren_name(const std::string oldname, const std::string newname);
    int db_sync();

    vector<db_key_t> get_keys();

    vector<uint16_t> get_ids();
    int get_name(uint16_t id, string& name);
    vector<string> get_names(void);
    int get_feature(uint16_t id, float* feature);
    int feature_count();

    int get_free(int& index);
    int database_save(int count);

private:
    int load_feature();
    int reassign_feature();
    int erase_feature(int index);
    int save_feature(int index = 0);

    int get_free_map();
    int get_remain_map();
    void reset_env();
    void self_test();

private:
    float threshold;
};

#endif /* _FEATUREDB_H_ */
