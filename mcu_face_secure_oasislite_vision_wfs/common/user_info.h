/**************************************************************************
 * 	FileName:	 	user_info.h
 *	Description:	This file is including the function interface which is
 					used to store cfg in flash
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-11-17
 *	Updated:		
 *					
**************************************************************************/


#ifndef _USER_INFO_H_
#define _USER_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sln_flash.h"
#include "sln_flash_mgmt.h"

/** The flash file where the configuration data will be stored */
#define USER_INFO_FILE_NAME     "user_info.dat"
#define PULSE_WIDTH_NOTSUPPORTED 101

/**
 * A structure to represent the configuration data
 */
typedef struct _user_info
{
	uint8_t reg_wait_active;		/* wf 注册需要等APP激活标记，当不为0时，需要把刚注册的user删除*/

	//uint8_t UID[8];			/**APP 端产生的UUID，8字节 */
    //uint8_t userName[17];     /**NXP 算法维护的username*/
} user_info_t;


/**
 * @brief Reads config data from flash memory
 *
 * @param cfg[out]     Pointer to a pre-allocated user_info_t structure where the data will be stored
 * @return             Status of read
 */
int32_t UserInfo_FlashRead(user_info_t *cfg);

/**
 * @brief Writes config data in flash memory
 *
 * @param cfg[in]      Pointer to a user_info_t structure containing the data that will be written
 * @return             Status of write
 */
int32_t UserInfo_FlashWrite(user_info_t *cfg);

/**
 * @brief Initializes config data with default values
 *
 * @param cfg[out]     Pointer to a pre-allocated user_info_t structure where the data will be stored
 */
void UserInfo_GetDefault(user_info_t *cfg);

/**
 * @brief Initializes the user application configuration data
 */
void UserInfo_AppDataInit();

/**
 * @brief Reads application config data
 *
 * @param cfg[out]     Pointer to a pre-allocated user_info_t structure where the data will be stored
 * @return             Status of read
 */
void UserInfo_AppDataRead(user_info_t *cfg);

/**
 * @brief Saves application config data
 *
 * @param cfg[in]      Pointer to a user_info_t structure containing the data that will be saved
 * @return             Status of save
 */
int32_t UserInfo_AppDataSave(user_info_t *cfg);


#ifdef __cplusplus
}
#endif

#endif /* _USER_INFO_H_ */
