/*
* Copyright 2019 NXP.
* This software is owned or controlled by NXP and may only be used strictly in accordance with the
* license terms that accompany it. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you
* agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
* applicable license terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef SLN_VIZN_API_H_
#define SLN_VIZN_API_H_

#include "generic_list.h"
#include "sln_dev_cfg.h"
#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#include <string>


#define MAX_VIZN_CLIENTS (5)
#define VIZN_API_HANDLE_SIZE (100U)


typedef enum _vizn_api_status
{
    kStatus_API_Layer_Success = 0,             /*!< Success */
    kStatus_API_Layer_Error = 1,                /*!< Failed */

    //Semaphore Errors
    kStatus_API_Sem_NoLock,      /*!< No lock defined  */
    kStatus_API_Sem_ERETRY,      /*!< Failed to aquire mutex */

    //RegisterClient
    kStatus_API_RegisterClient_LimitReached,   /*!< Clients number limit reached */

    //SetRegisteredMode
    kStatus_API_Layer_SetRegisteredMode_SaveFailed,  /*!< Failed to save configuration for registration mode*/

    //AddUser
    kStatus_API_Layer_AddUser_NoAddCommand,     /*!< No add command was previously issued */
    kStatus_API_Layer_AddUser_AddCommandStopped,  /*!< Last add command was succesfully stoped*/
    kStatus_API_Layer_AddUser_InvalidUserName,    /*!< The user name is invalid */
    kStatus_API_Layer_AddUser_MultipleAddCommand,   /*!< Can't place more than one add at a time */

    //DelUser
    kStatus_API_Layer_DelUser_InvalidUserName,      /*!< The user name is invalid*/
    kStatus_API_Layer_DelUser_FailedDelete,         /*!< Failed to delete the user*/
    kStatus_API_Layer_DelUser_FailedDeleteAll,      /*!< Failed to delete all the  users*/
    kStatus_API_Layer_DelUser_DelCommandStopped,   /*!<  Last add command was succesfully stoped */

    //RenameUser
    kStatus_API_Layer_RenameUser_FailedRename,       /*!< Failed to rename user*/
    kStatus_API_Layer_RenameUser_InvalidUserName,	 /*!< The user name is invalid*/
    kStatus_API_Layer_RenameUser_AlreadyExists,      /*!< The user name already exists in the database */

    //SetVerboseMode
    kStatus_API_Layer_SetVerboseMode_SaveFailed,    /*!< Failed to save configuration for verbose mode*/

    //SetCameraIRFilterMode
    kStatus_API_Layer_SetCameraIRFilterMode_SaveFailed,  /*!< Failed to save configuration for camera irfilter mode*/
    kStatus_API_Layer_SetCameraIRFilterMode_NotSupported,   /*!<Camera irfilter configuration is not supported for the curent system*/

    //SetCameraWhite/IRPulseWidth
    kStatus_API_Layer_SetCameraPulseWidth_SaveFailed,  /*!< Failed to save configuration for camera pulse width*/
    kStatus_API_Layer_SetCameraPulseWidth_NotSupported,    /*!<Camera white/ir pulse width modulation is not supported for the curent system*/
    kStatus_API_Layer_SetCameraPulseWidth_WrongValue, /*!< Pulse width should be less than 100*/

    //SetEmotionTypes
    kStatus_API_Layer_SetEmotionTypes_SaveFailed,  /*!< Failed to save configuration for emotion types*/
    kStatus_API_Layer_SetEmotionTypes_InvalidTypes,  /*!< Valid values 0, 2, 4, 7*/
    kStatus_API_Layer_SetEmotionTypes_Disabled,  /*!< value 0 to disable*/

    //SetLivenessMode
    kStatus_API_Layer_SetLivenessMode_SaveFailed,  /*!< Failed to save configuration for liveness mode*/

    //SetDetResolutionMode
    kStatus_API_Layer_SetDetResolutionMode_SaveFailed,  /*!< Failed to save configuration for detect resolution mode*/
    kStatus_API_Layer_SetDetResolutionMode_Same,  /*!< detect resolution mode is same, don't reset*/

    //SetDispMode
    kStatus_API_Layer_SetDispMode_NotSupported,    /*!<Display mode setting is not supported for the curent system*/

    //Select LED
    kStatus_API_Layer_SelectCameraLED_NotSupported,    /*!<Select Camera LED is not supported for the curent system*/

    //SetAppType
    kStatus_API_Layer_SetAppType_NotSupported,    /*!<Application type setting is not supported for the curent system*/
} vizn_api_status_t;


/*! @brief The handle of the VIZN_api_handle module */
typedef void *VIZN_api_handle_t;
typedef struct _client_operations client_operations_t;

typedef struct _face_info{
    char name[64];
    bool recognize;
    bool enrolment;
    int rt;
    unsigned face_id;
}face_info_t;

typedef struct _VIZN_api_client
{
    const char *clientID;
    char *pcHelpString;
    const struct _client_operations *ops;
    list_element_t link;
}VIZN_api_client_t;

struct _client_operations
{
    int (*detect)(VIZN_api_client_t *pClient, int dt);
    int (*recognize)(VIZN_api_client_t *pClient, face_info_t face_info );
    int (*enrolment)(VIZN_api_client_t *pClient, face_info_t face_info );
};



extern VIZN_api_handle_t gApiHandle;



#define VIZN_API_CLIENT_DEFINE(client, descriptor, ops) \
                                                                        \
    VIZN_api_client_t g_APICommand##client = {                         \
        (#client), (descriptor), (ops), {0},        \
    }

#define VIZN_API_CLIENT(client) g_APICommand##client

#define VIZN_CLIENT_STRUCT_OFFSET(type, field) ((size_t) & (((type *)0)->field))
#define VIZN_CLIENT_POINTER(node) ((VIZN_api_client_t *)(((uint32_t)node) - VIZN_CLIENT_STRUCT_OFFSET(VIZN_api_client_t, link)))


/**
 * @brief Init the handler and internal structures
 *
 * @param apiHandle    Pointer of an API handler which will store a list of clients. API Handler will point to an internal pre-allocated buffer of the API;
 * @return             Status of VIZN_API_Init
 */
vizn_api_status_t VIZN_API_Init(VIZN_api_handle_t *apiHandle);

/**
 * @brief Register a client in the api handler provided
 *
 * @param apiHandle    The API handler that the client will be attach to;
 * @param clientHandle Pointer to a structure (VIZN_api_client_t) which contain informations about the new client;
 * @return             Status of VIZN_RegisterClient
 */
vizn_api_status_t VIZN_RegisterClient(VIZN_api_handle_t apiHandle ,VIZN_api_client_t *clientHandle);

/**
 * @brief Unregister a client from the api handler provided
 *
 * @param apiHandle    The API handler that the client will be removed from
 * @param clientHandle Pointer to a structure (VIZN_api_client_t) which contain informations about the former client;
 * @return             Status of VIZN_UnregisterClient
 */
vizn_api_status_t VIZN_UnregisterClient(VIZN_api_handle_t apiHandle ,VIZN_api_client_t *clientHandle);

/**
 * @brief Get the list with all the users
 *
 * @param clientHandle    The client handler which required the list of users
 * @param nameList      Vector of strings in which the API will place the list of registered users
 * @return             Status of VIZN_GetRegisteredUsers
 */
vizn_api_status_t VIZN_GetRegisteredUsers(VIZN_api_client_t *clientHandle, std::vector<std::string> *nameList);

/**
 * @brief Set the mode in which the registration will be made
 *
 * @param clientHandle    The client handler which required this action
 * @param mode          The value of the new mode. Two values supported ENROLMENT_MODE_MANUAL, ENROLMENT_MODE_AUTO
 * @return             Status of VIZN_SetRegisteredMode
 */
vizn_api_status_t VIZN_SetRegisteredMode(VIZN_api_client_t *clientHandle, cfg_enrolment_t mode);

/**
 * @brief Set the verbose mode. Enabling this will send more information towards the clients registered
 *
 * @param clientHandle    The client handler which required this action
 * @param mode          The value of the new mode. Two values supported ENROLMENT_MODE_MANUAL, ENROLMENT_MODE_AUTO
 * @return             Status of VIZN_SetVerboseMode
 */
vizn_api_status_t VIZN_SetVerboseMode(VIZN_api_client_t *clientHandle, cfg_verbose_t mode);

/**
 * @brief Add the name of the user. The user will be registered with the next empty name.
 *
 * @param clientHandle    The client handler which required this action
 * @return             Status of VIZN_AddUser
 */
vizn_api_status_t VIZN_AddUser (VIZN_api_client_t *clientHandle);

/**
 * @brief Add the name of the user. The user will be registered with that name by the oasis.
 *
 * @param clientHandle    The client handler which required this action
 * @param userName        Pointer to a buffer which contains the name of the new user.
 * @return             Status of VIZN_AddUser
 */
vizn_api_status_t VIZN_AddUser (VIZN_api_client_t *clientHandle, char *userName);

/**
 * @brief Delete the user based on the name provided.
 *
 * @param clientHandle    The client handler which required this action
 * @param userName        Pointer to a buffer which contains the name of the user that will be deleted.
 * @return             Status of VIZN_DelUser
 */
vizn_api_status_t VIZN_DelUser (VIZN_api_client_t *clientHandle, char *userName);

/**
 * @brief Delete current recognized user
 *
 * @param clientHandle    The client handler which required this action
 * @return             Status of VIZN_DelCurrentUser
 */
vizn_api_status_t VIZN_DelCurrentUser (VIZN_api_client_t *clientHandle);

/**
 * @brief Delete all users
 *
 * @param clientHandle    The client handler which required this action
 * @return             Status of VIZN_DelUser
 */
vizn_api_status_t VIZN_DelUser (VIZN_api_client_t *clientHandle);

/**
 * @brief Rename a registered user with the new name provided.
 *
 * @param clientHandle    The client handler which required this action
 * @param oldUserName     The name of the user that will be renamed
 * @param newUserName     The new name of the user
 * @return                Status of VIZN_RenameUser
 */
vizn_api_status_t VIZN_RenameUser (VIZN_api_client_t *clientHandle, char *oldUserName , char *newUserName );

/**
 * @brief Save database stored on RAM into the FLASH
 *
 * @param clientHandle    The client handler which required this action
 * @param count           Number of users to be saved
 * @return                Status of VIZN_RenameUser
 */
vizn_api_status_t VIZN_SaveDB (VIZN_api_client_t *clientHandle, int count);

/**
 * @brief Reset the System
 *
 * @param clientHandle    The client handler which required this action
 * @return                Status of VIZN_SystemReset
 */
vizn_api_status_t VIZN_SystemReset (VIZN_api_client_t *clientHandle);

/**
 * @brief Get the camera state off the IR filter
 *
 * @param clientHandle    The client handler which required this action
 * @param mode			  Pointer to a cfg_irfilter_t variable in which will be stored the irfilter
 * @return                Status of VIZN_GetFilter
 */

vizn_api_status_t VIZN_GetIRFilter (VIZN_api_client_t *clientHandle, cfg_irfilter_t *mode);

/**
 * @brief Set  the camera IR filter is on or off
 *
 * @param clientHandle    The client handler which required this action
 * @param mode            The new state of the camera irfilter <IRFILTER_MODE_OFF|IRFILTER_MODE_ON>
 * @return                Status of VIZN_SetFilter
 */
vizn_api_status_t VIZN_SetIRFilter (VIZN_api_client_t *clientHandle, cfg_irfilter_t mode);

/**
 * @brief Set  the camera white/ir pulse width to a specify value
 *
 * @param clientHandle    The client handler which required this action
 * @param led     The value of  led type.  LED_WHITE or LED_IR.
 * @param pulse_width     The value of the width. Only values between 0 and 100.
 * @return                Status of VIZN_SetPulseWidth
 */
vizn_api_status_t VIZN_SetPulseWidth (VIZN_api_client_t *clientHandle,  cfg_led_t led,  uint8_t pulse_width);

/**
 * @brief Get the camera white/ir pulse width
 *
 * @param clientHandle    The client handler which required this action
 * @param led     The value of  led type.  LED_WHITE or LED_IR.
 * @param pulse_width     Pointer to an uint8_t variable in which will be stored the duty cycle(0-100) of the led pulse width modulation
 * @return                Status of VIZN_GetPulseWidth
 */
vizn_api_status_t VIZN_GetPulseWidth (VIZN_api_client_t *clientHandle,  cfg_led_t led,  uint8_t *pulse_width);

/**
 * @brief Set the emotion types to a specify value
 *
 * @param clientHandle    The client handler which required this action
 * @param types           The value of the emotion types, 0 - disable, 2- two types, 4 - four types, 7 - seven types.
 * @return                Status of VIZN_SetEmotionTypes
 */
vizn_api_status_t VIZN_SetEmotionTypes (VIZN_api_client_t *clientHandle, uint8_t types);

/**
 * @brief Get the emotion types to a specify value
 *
 * @param clientHandle    The client handler which required this action
 * @param types           The value of the emotion types, 0 - disable, 2- two types, 4 - four types, 7 - seven types.
 * @return                Status of VIZN_GetEmotionTypes
 */
vizn_api_status_t VIZN_GetEmotionTypes (VIZN_api_client_t *clientHandle, uint8_t *types);

/**
 * @brief Set the liveness mode. Enabling this will send more information towards the clients registered
 *
 * @param clientHandle    The client handler which required this action
 * @param mode          The value of the new mode. Two values supported LIVENESS_MODE_OFF, LIVENESS_MODE_ON
 * @return                Status of VIZN_SetLivenessMode
 */
vizn_api_status_t VIZN_SetLivenessMode (VIZN_api_client_t *clientHandle, cfg_liveness_t mode);

/**
 * @brief Get the liveness mode is on or off
 *
 * @param clientHandle    The client handler which required this action
 * @param mode     Pointer to a cfg_record_t variable.
 * @return                Status of VIZN_GetLivenessMode
 */
vizn_api_status_t VIZN_GetLivenessMode (VIZN_api_client_t *clientHandle, cfg_liveness_t *mode);

/**
 * @brief Set the detect resolution mode is qvga or vga
 *
 * @param clientHandle    The client handler which required this action
 * @param mode     Pointer to a cfg_record_t variable.
 * @return                Status of VIZN_SetLivenessMode
 */
vizn_api_status_t VIZN_SetDetResolutionMode(VIZN_api_client_t *clientHandle, cfg_detresolution_t mode);

/**
 * @brief Set the display mode.
 *
 * @param clientHandle    The client handler which required this action
 * @param mode          The value of display mode. Two values supported DISPALY_MODE_RGB, DISPALY_MODE_IR
 * @return                Status of VIZN_SetDispMode
 */
vizn_api_status_t VIZN_SetDispMode (VIZN_api_client_t *clientHandle, cfg_displaymode_t mode);

/**
 * @brief Set the application type.
 *
 * @param clientHandle    The client handler which required this action
 * @param mode          The value of the application types. Two values supported APP_TYPE_ELOCK, APP_TYPE_DOOR_ACCESS
 * @return                Status of VIZN_SetAppType
 */
vizn_api_status_t VIZN_SetAppType (VIZN_api_client_t *clientHandle, cfg_apptype_t type);

/**
 * @brief Get the application type is elock or door access.
 *
 * @param clientHandle    The client handler which required this action
 * @param mode     Pointer to a cfg_record_t variable.
 * @return                Status of VIZN_GetAppType
 */
vizn_api_status_t VIZN_GetAppType (VIZN_api_client_t *clientHandle, cfg_apptype_t *type);


#endif /* SLN_VIZN_API_H_ */
