/*
* Copyright 2019 NXP.
* This software is owned or controlled by NXP and may only be used strictly in accordance with the
* license terms that accompany it. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you
* agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
* applicable license terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "oasis.h"
#include "database.h"
#include "sln_vizn_cli.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "sln_shell.h"
#include "queue.h"
#include "sln_dev_cfg.h"
#include "sln_vizn_api.h"
#include "sln_lpm.h"

#include "flash_ica_driver.h"
#include "fica_definition.h"

static shell_status_t FFI_CLI_ListCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< list command */

static shell_status_t FFI_CLI_ModeCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< mode command */

static shell_status_t FFI_CLI_AddCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< add command */

static shell_status_t FFI_CLI_DelCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< del command */

static shell_status_t FFI_CLI_RenCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< ren command */

static shell_status_t FFI_CLI_Verbose(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< verbose command */

static shell_status_t FFI_CLI_SaveCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< save command */

static shell_status_t FFI_CLI_ResetCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< reset command */

static shell_status_t FFI_CLI_CameraCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< camera command */

static shell_status_t FFI_CLI_VersionCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< version command */

static shell_status_t FFI_CLI_FwUpdateOTW(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< fwupdate-otw command */

static shell_status_t FFI_CLI_EmotionTypes(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< emotion types command */

static shell_status_t FFI_CLI_LivenessCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< liveness command */

static shell_status_t FFI_CLI_DetResolutionCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< det resolution command */

static shell_status_t FFI_CLI_DisplayCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< display command */

static shell_status_t FFI_CLI_AppCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< App command */


static shell_status_t FFI_CLI_LpmCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv); /*!< low power mode command */

extern "C"{

shell_status_t SHELL_HelpCommand(shell_handle_t shellContextHandle,

        int32_t argc,
        char **argv); /*!< help command */

shell_status_t SHELL_ExitCommand(shell_handle_t shellContextHandle,
        int32_t argc,
        char **argv); /*!< exit command */
}

SHELL_COMMAND_DEFINE(list, (char *)"\r\n\"list\": List all registered users\r\n", FFI_CLI_ListCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(mode, (char *)"\r\n\"mode <auto|manual>\": User add mode\r\n", FFI_CLI_ModeCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(add, (char *)"\r\n\"add username\": Add user\r\n"
                                  "\r\n\"add -s\": Stop the former add command issued\r\n", FFI_CLI_AddCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(del, (char *)"\r\n\"del username\": Delete user \r\n"
                                  "\r\n\"del -a\": Delete all users \r\n", FFI_CLI_DelCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(rename, (char *)"\r\n\"rename oldusername newusername\": Rename user\r\n", FFI_CLI_RenCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(verbose, (char *)"\r\n\"verbose <0|1|2|3> (none-all)\": Debug message detail level setting\r\n", FFI_CLI_Verbose, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(save, (char *)"\r\n\"save n\": Save database into flash memory\r\n", FFI_CLI_SaveCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(reset, (char *)"\r\n\"reset\": Reset the MCU\r\n", FFI_CLI_ResetCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(detection, (char *)"\r\n\"detection resolution <qvga/vga>\": Set detection resolution and reset\r\n", FFI_CLI_DetResolutionCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(camera, (char *)"\r\n\"camera irfilter <on|off>\r\n"
                            "\r\n\"camera ir_pwm <value>\": PWM pulse width for IR led, value should be between 0 (inactive) to 100\r\n"
                            "\r\n\"camera white_pwm <value>\": PWM pulse width for white led, value should be between 0 (inactive) to 100\r\n", FFI_CLI_CameraCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(version, (char *)"\r\n\"version\": The version information\r\n", FFI_CLI_VersionCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(updateotw, (char *)"\r\n\"updateotw\": Reboot the board and start the OTW firmware update\r\n", FFI_CLI_FwUpdateOTW, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(emotion, (char *)"\r\n\"emotion\": Emotion recognition types <0|2|4|7>\r\n", FFI_CLI_EmotionTypes, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(liveness, (char *)"\r\n\"liveness\": liveness <on|off>\r\n", FFI_CLI_LivenessCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(display, (char *)"\r\n\"display\": display <rgb|ir>\r\n", FFI_CLI_DisplayCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(app, (char *)"\r\n\"app type <0|1|2|3>\":\r\n 0-Elock(light)\r\n 1-Elock(heavy)\r\n 2-Door access(light)\r\n 3-Door access(heavy)\r\n", FFI_CLI_AppCommand, SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(lpm, (char *)"\r\n\"lpm <n> \": select number into low power mode.\r\n\
    7. SNVS           - SNVS mode\r\n"\
    , FFI_CLI_LpmCommand, SHELL_IGNORE_PARAMETER_COUNT);
/*
0. Over RUN       - System Over Run mode\r\n\
1. Full RUN       - System Full Run mode\r\n\
2. Low Speed RUN  - System Low Speed Run mode\r\n\
3. Low Power RUN  - System Low Power Run mode\r\n\
4. System Idle    - System Wait mode\r\n\
5. Low Power Idle - Low Power Idle mode\r\n\
6. Suspend        - Suspend mode\r\n\
*/
extern QueueHandle_t g_UsbShellQueue;
extern VIZN_api_handle_t gApiHandle;
extern VIZN_api_client_t VIZN_API_CLIENT(Shell);
static std::string s_AddNewFaceName;

shell_status_t RegisterFFICmds(shell_handle_t shellContextHandle)
{
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(list));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(mode));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(add));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(del));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(rename));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(verbose));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(camera));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(version));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(save));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(updateotw));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(reset));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(emotion));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(liveness));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(detection));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(display));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(app));
    SHELL_RegisterCommand(shellContextHandle, SHELL_COMMAND(lpm));
    return kStatus_SHELL_Success;
}

static shell_status_t UsbShell_QueueSendFromISR(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv,
                                        char shellCommand)
{
    UsbShellCmdQueue_t queueMsg;

    if (argc > USB_SHELL_PARAMS_COUNT)
    {
        return kStatus_SHELL_Error;
        SHELL_Printf(shellContextHandle, "Parameters count overflow\r\n");
    }

    for (int i = 0; i < argc; i++)
    {
        if (strlen(argv[i]) < USB_SHELL_PARAMS_SIZE)
        {
            strcpy(queueMsg.argv[i], argv[i]);
        }
        else
        {
            SHELL_Printf(shellContextHandle, "Parameter '%s' overflow\r\n", argv[i]);
            return kStatus_SHELL_Error;
        }
    }
    queueMsg.argc = argc;
    queueMsg.shellContextHandle = shellContextHandle;
    queueMsg.shellCommand = shellCommand;

    if (pdTRUE != xQueueSendFromISR(g_UsbShellQueue, (void*)&queueMsg, NULL))
    {
        SHELL_Printf(shellContextHandle, "ERROR: Cannot send command to shell processing queue\r\n");
        return kStatus_SHELL_Error;
    }

    return kStatus_SHELL_Success;
}


static shell_status_t FFI_CLI_ListCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc > 1)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_LIST);
}

static shell_status_t FFI_CLI_ModeCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc > 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    if (argc == 1)
    {
        // List current mode
        SHELL_Printf(shellContextHandle,"Current mode: %s\r\n",
                (ENROLMENT_MODE_AUTO == Cfg_AppDataGetEnrolmentMode()) ? "auto":"manual");
        return kStatus_SHELL_Success;
    }
    else
    {
        return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_MODE);
    }
}

static shell_status_t FFI_CLI_AddCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc != 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_ADD);
}

static shell_status_t FFI_CLI_DelCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc != 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_DEL);
}

static shell_status_t FFI_CLI_RenCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc != 3)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_RENAME);
}

static shell_status_t FFI_CLI_Verbose(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc > 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    if (argc == 1)
    {
        // List current mode
        SHELL_Printf(shellContextHandle, "verbose level: %d\r\n", Cfg_AppDataGetVerbosity());
        return kStatus_SHELL_Success;
    }
    else
    {
        return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_VERBOSE);
    }
}

static shell_status_t FFI_CLI_SaveCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc > 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_SAVE);
}

static shell_status_t FFI_CLI_ResetCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_RESET);
}

static shell_status_t FFI_CLI_DetResolutionCommand(shell_handle_t shellContextHandle,
                                                   int32_t argc,
                                                   char **argv)
{
    if ((argc > 3) || (argc < 2))
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }

    if (argc == 2)
    {
        SHELL_Printf(shellContextHandle,"Current detection resolution mode: %s\r\n",
                (DETECT_RESOLUTION_QVGA == Cfg_AppDataGetDetectResolutionMode()) ? "qvga":"vga");
        return kStatus_SHELL_Success;
    }
    else
    {
        return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_DET_RESOLUTION);
    }
}


static shell_status_t FFI_CLI_CameraCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_CAMERA);
}

static shell_status_t FFI_CLI_VersionCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc != 1)
    {
        SHELL_Printf(shellContextHandle, "Version command has no arguments\r\n");
        return kStatus_SHELL_Error;
    }
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_VERSION);
}

static shell_status_t FFI_CLI_FwUpdateOTW(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_FWUPDATE_OTW);
}

static shell_status_t FFI_CLI_EmotionTypes(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_EMOTION_TYPES);
}

static shell_status_t FFI_CLI_LivenessCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_LIVENESS);
}

static shell_status_t FFI_CLI_DisplayCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_DISPLAY);
}

static shell_status_t FFI_CLI_AppCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_APP);
}

static shell_status_t FFI_CLI_LpmCommand(shell_handle_t shellContextHandle,
                                        int32_t argc,
                                        char **argv)
{
    if (argc != 2)
    {
        SHELL_Printf(shellContextHandle, "Wrong parameters\r\n");
        return kStatus_SHELL_Error;
    }
    return UsbShell_QueueSendFromISR(shellContextHandle, argc, argv, SHELL_EV_FFI_CLI_LPM);
}

void UsbShell_CmdProcess_Task(void *arg)
{
    vTaskDelay(portTICK_PERIOD_MS * 1000);
    SHELL_Printf(usb_shellHandle[USB_SHELL_PROMPT_INDEX], "Type \"help\" to see what this shell can do!\r\n");
    SHELL_Printf(usb_shellHandle[USB_SHELL_PROMPT_INDEX], USB_SHELL_PROMPT);

    while (1)
    {
        vizn_api_status_t status;
        UsbShellCmdQueue_t queueMsg;
        xQueueReceive(g_UsbShellQueue, &queueMsg, portMAX_DELAY);

        shell_handle_t shellContextHandle = queueMsg.shellContextHandle;
        if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_LIST)
        {
            std::vector<std::string> names;
            uint32_t namescount;

            VIZN_GetRegisteredUsers(&VIZN_API_CLIENT(Shell),&names);
            namescount = names.size();

            SHELL_Printf(shellContextHandle, "Registered users count:%d\r\n", namescount);
            for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++)
            {
                SHELL_Printf(shellContextHandle, "%s\r\n",(*it).c_str());
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_MODE)
        {
            uint32_t status;
            //Set mode
            if (!strcmp((char *)queueMsg.argv[1], "manual"))
            {
                status = VIZN_SetRegisteredMode(&VIZN_API_CLIENT(Shell),ENROLMENT_MODE_MANUAL);
                if (status == kStatus_API_Layer_Success)
                {
                    SHELL_Printf(shellContextHandle, "Manual mode selected\r\n");
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Cannot save registration mode\r\n");
                }
            }
            else if (!strcmp((char *)queueMsg.argv[1], "auto"))
            {
                status = VIZN_SetRegisteredMode(&VIZN_API_CLIENT(Shell),ENROLMENT_MODE_AUTO);
                if (status == kStatus_API_Layer_Success)
                {
                    SHELL_Printf(shellContextHandle, "Auto mode selected\r\n");
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Cannot save registration mode\r\n");
                }
            }
            else
            {
                SHELL_Printf(shellContextHandle, "Undefined mode\r\n");
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_ADD)
        {
            status = VIZN_AddUser(&VIZN_API_CLIENT(Shell), (char *)queueMsg.argv[1]);
            switch(status)
            {
                case kStatus_API_Layer_AddUser_NoAddCommand:
                    SHELL_Printf(shellContextHandle, "No add command registered\r\n");
                    break;
                case kStatus_API_Layer_AddUser_AddCommandStopped:
                    SHELL_Printf(shellContextHandle, "Stopped adding %s \r\n", s_AddNewFaceName.c_str());
                    break;
                case kStatus_API_Layer_AddUser_InvalidUserName:
                    SHELL_Printf(shellContextHandle, "Invalid User Name\r\n");
                    break;
                case kStatus_API_Layer_AddUser_MultipleAddCommand:
                    SHELL_Printf(shellContextHandle, "Add command already in pending. Please complete the pending registration for user %s or stop the adding\r\n", s_AddNewFaceName.c_str());
                    break;
                case kStatus_API_Layer_Success:
                    SHELL_Printf(shellContextHandle, "Start registering '%s'\r\n", queueMsg.argv[1]);
                    break;
                default:
                    SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_DEL)
        {
            if (0 == strcmp((char *)queueMsg.argv[1], "-a"))
            {
                status = VIZN_DelUser(&VIZN_API_CLIENT(Shell));
                if ( status == kStatus_API_Layer_DelUser_FailedDeleteAll)
                {
                    SHELL_Printf(shellContextHandle, "Failed deleting all users\r\n");
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "All users successfuly deleted\r\n");
                }
            }else{
                status = VIZN_DelUser(&VIZN_API_CLIENT(Shell), (char *)queueMsg.argv[1]);
                switch(status)
                {
                    case kStatus_API_Layer_DelUser_InvalidUserName:
                        SHELL_Printf(shellContextHandle, "Invalid User Name\r\n");
                        break;
                    case kStatus_API_Layer_DelUser_FailedDelete:
                        SHELL_Printf(shellContextHandle, "Failed deleting user '%s'\r\n", queueMsg.argv[1]);
                        break;
                    case kStatus_API_Layer_Success:
                        SHELL_Printf(shellContextHandle, "User '%s' successfuly deleted\r\n", queueMsg.argv[1]);
                        break;
                    default:
                        SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
                }
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_RENAME)
        {
        	status = VIZN_RenameUser(&VIZN_API_CLIENT(Shell), (char *)queueMsg.argv[1],(char *)queueMsg.argv[2]);
        	switch(status)
        	{
        		case kStatus_API_Layer_RenameUser_InvalidUserName:
					SHELL_Printf(shellContextHandle, "Invalid User Name\r\n");
					break;
                case kStatus_API_Layer_RenameUser_AlreadyExists:
                    SHELL_Printf(shellContextHandle, "Failed renaming, user '%s' already exists\r\n", queueMsg.argv[2]);
                    break;
        		case kStatus_API_Layer_RenameUser_FailedRename:
        			SHELL_Printf(shellContextHandle, "Failed renaming user '%s'\r\n", queueMsg.argv[1]);
					break;
        		case kStatus_API_Layer_Success:
                    SHELL_Printf(shellContextHandle, "User '%s' successfully renamed\r\n", queueMsg.argv[1]);
                    break;
                default:
                    SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_VERBOSE)
        {
            int verb_lvl = atoi(queueMsg.argv[1]);
            if (verb_lvl >= VERBOSE_MODE_OFF && verb_lvl <= VERBOSE_MODE_L3)
            {
                status = VIZN_SetVerboseMode(&VIZN_API_CLIENT(Shell), (cfg_verbose_t)verb_lvl);
                if (status == kStatus_API_Layer_Success)
                {
                    SHELL_Printf(shellContextHandle, "Verbose level set to %d\r\n", verb_lvl);
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Cannot save verbose level\r\n");
                }
            }
            else
            {
                SHELL_Printf(shellContextHandle, "Undefined mode\r\n");
            }
        }
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_SAVE)
        {
            int list_length = atoi(queueMsg.argv[1]);

            if(list_length == 0)
            {
                uint32_t time_tick = Time_Now();
                SHELL_Printf(shellContextHandle, "all sdram database will save into flash database\r\n");
                VIZN_SaveDB(&VIZN_API_CLIENT(Shell),0);
                SHELL_Printf(shellContextHandle, "database save success, %d\r\n", time_tick - Time_Now());
            }
            else
            {
                uint32_t time_tick = Time_Now();
                SHELL_Printf(shellContextHandle, "%d list will be save into flash database\r\n", list_length);
                VIZN_SaveDB(&VIZN_API_CLIENT(Shell),list_length);
                SHELL_Printf(shellContextHandle, "database save success, %d\r\n", time_tick - Time_Now());
            }

        }
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_RESET)
        {
            VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
        }
		else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_DET_RESOLUTION)
        {
            uint32_t status;
            if(strcmp((char *)queueMsg.argv[1], "resolution"))
            {
				SHELL_Printf(shellContextHandle, "wrong command\r\n");
            }
			else
            {
	            if (!strcmp((char *)queueMsg.argv[2], "qvga"))
	            {
	                status = VIZN_SetDetResolutionMode(&VIZN_API_CLIENT(Shell),DETECT_RESOLUTION_QVGA);
	                if (status == kStatus_API_Layer_Success)
	                {
	                    SHELL_Printf(shellContextHandle, "Detection resolution QVGA mode selected, reset system\r\n");
						VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
	                }
	                else if(status == kStatus_API_Layer_SetDetResolutionMode_Same)
	                {
	                    SHELL_Printf(shellContextHandle, "Detection resolution is already QVGA mode\r\n");
	                }
					else
	                {
	                    SHELL_Printf(shellContextHandle, "Cannot set detection resolution QVGA mode\r\n");
	                }
	            }
	            else if (!strcmp((char *)queueMsg.argv[2], "vga"))
	            {
	                status = VIZN_SetDetResolutionMode(&VIZN_API_CLIENT(Shell),DETECT_RESOLUTION_VGA);
	                if (status == kStatus_API_Layer_Success)
	                {
	                    SHELL_Printf(shellContextHandle, "Detection resolution VGA mode selected, reset system\r\n");
						VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
	                }
					else if(status == kStatus_API_Layer_SetDetResolutionMode_Same)
	                {
	                    SHELL_Printf(shellContextHandle, "Detection resolution is already VGA mode\r\n");
	                }
	                else
	                {
	                    SHELL_Printf(shellContextHandle, "Cannot set detection resolution VGA mode\\r\n");
	                }
	            }
	            else
	            {
	                SHELL_Printf(shellContextHandle, "Undefined mode\r\n");
	            }
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_CAMERA)
        {
            if (!strcmp((char *)queueMsg.argv[1],"irfilter"))
            {
                if (queueMsg.argc == 2)
                {
                    cfg_irfilter_t mode;
                    status = VIZN_GetIRFilter(&VIZN_API_CLIENT(Shell),&mode);
                    if (status == kStatus_API_Layer_Success)
                    {
                        SHELL_Printf(shellContextHandle, "Camera IR Filter mode %s\r\n",(mode == IRFILTER_MODE_ON) ? "on":"off");
                    }
                    else
                    {
                        SHELL_Printf(shellContextHandle, "Camera irfilter not supported\r\n");
                    }

                }
                else
                {
                    if (!strcmp(queueMsg.argv[2],"on"))
                    {
                        status = VIZN_SetIRFilter(&VIZN_API_CLIENT(Shell),IRFILTER_MODE_ON);

                    }
                    else if (!strcmp(queueMsg.argv[2],"off"))
                    {
                        status = VIZN_SetIRFilter(&VIZN_API_CLIENT(Shell),IRFILTER_MODE_OFF);

                    }
                    else
                    {
                        SHELL_Printf(shellContextHandle,"Undefined mode\r\n");
                        SHELL_Printf(shellContextHandle, USB_SHELL_PROMPT);
                        continue;
                    }

                    switch(status)
                    {
                        case kStatus_API_Layer_Success:
                            SHELL_Printf(shellContextHandle, "Camera IR Filter mode %s\r\n", queueMsg.argv[2]);
                            break;
                        case kStatus_API_Layer_SetCameraIRFilterMode_SaveFailed:
                            SHELL_Printf(shellContextHandle, "Cannot save camera irfilter mode\r\n");
                            break;
                        case kStatus_API_Layer_SetCameraIRFilterMode_NotSupported:
                            SHELL_Printf(shellContextHandle, "Camera irfilter not supported\r\n");
                            break;
                        default:
                            SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
                    }
                }
            }
            else if(!strcmp((char *)queueMsg.argv[1],"ir_pwm") || !strcmp((char *)queueMsg.argv[1],"white_pwm"))
            {
                uint8_t pulse_width;
                if (queueMsg.argc == 2)
                {
                   if(!strcmp((char *)queueMsg.argv[1],"ir_pwm")) {
                        status = VIZN_GetPulseWidth(&VIZN_API_CLIENT(Shell),LED_IR, &pulse_width);
                        if (status == kStatus_API_Layer_Success)
                        {
                            SHELL_Printf(shellContextHandle, "Camera IR duty cycle %d%\r\n",pulse_width);
                        }
                        else
                        {
                            SHELL_Printf(shellContextHandle, "Camera IR PWM not supported\r\n");
                        }
                   }else{
                       status = VIZN_GetPulseWidth(&VIZN_API_CLIENT(Shell),LED_WHITE, &pulse_width);
                       if (status == kStatus_API_Layer_Success)
                       {
                           SHELL_Printf(shellContextHandle, "Camera white duty cycle %d%\r\n",pulse_width);
                       }
                       else
                       {
                           SHELL_Printf(shellContextHandle, "Camera white PWM not supported\r\n");
                       }
                    }
                }
                else
                {
                    char *pEnd;
                    pulse_width = (uint8_t)strtol(queueMsg.argv[2], &pEnd, 10);
                    if(pEnd == queueMsg.argv[2]){
                        SHELL_Printf(shellContextHandle, "\"%s\" not a valid number \r\n",queueMsg.argv[2]);
                    }
                    else
                    {
                        if(!strcmp((char *)queueMsg.argv[1],"ir_pwm"))
                            status = VIZN_SetPulseWidth(&VIZN_API_CLIENT(Shell), LED_IR, (uint8_t)pulse_width);
                        else
                            status = VIZN_SetPulseWidth(&VIZN_API_CLIENT(Shell), LED_WHITE, (uint8_t)pulse_width);
                        switch(status)
                        {
                            case kStatus_API_Layer_Success:
                                SHELL_Printf(shellContextHandle, "Camera PWM duty cycle set to %d%\r\n", pulse_width);
                                break;
                            case kStatus_API_Layer_SetCameraPulseWidth_SaveFailed:
                                SHELL_Printf(shellContextHandle, "Cannot save PWM duty cycle\r\n");
                                break;
                            case kStatus_API_Layer_SetCameraPulseWidth_NotSupported:
                                SHELL_Printf(shellContextHandle, "Camera PWM duty cycle not supported\r\n");
                                break;
                            case kStatus_API_Layer_SetCameraPulseWidth_WrongValue:
                                SHELL_Printf(shellContextHandle, "Camera PWM duty cycle wrong value\r\n");
                                break;
                            default:
                                SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
                        }
                    }
                }
            }
            else
            {
                SHELL_Printf(shellContextHandle,"Undefined command for the camera\r\n");
            }
        } 
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_VERSION)
        {
            SHELL_Printf(shellContextHandle, "Engine:v%d.%d\r\n", VERSION_MAJOR, VERSION_MINOR);
        }
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_FWUPDATE_OTW)
        {
            /* Set the FWUPDATE update indicator in flash memory */
            int32_t fica_status = FICA_set_comm_flag(FICA_COMM_FWUPDATE_BIT);
            if (fica_status == SLN_FLASH_NO_ERROR)
            {
                SHELL_Printf(shellContextHandle, "The OTW firmware update will be started\r\n");

                VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
            }
            else
            {
                SHELL_Printf(shellContextHandle, "Cannot start the OTW firmware update\r\n");
            }
        }
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_EMOTION_TYPES)
        {
            uint8_t eTypes;
            if (queueMsg.argc == 2)
            {
                /* set the emotion types */
                eTypes = atoi(queueMsg.argv[1]);
                status = VIZN_SetEmotionTypes(&VIZN_API_CLIENT(Shell), (uint8_t)eTypes);
                switch(status)
                {
                    case kStatus_API_Layer_Success:
                        SHELL_Printf(shellContextHandle, "%d types emotion recognition is set, system will reset\r\n", eTypes);
                        VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
                        break;
                    case kStatus_API_Layer_SetEmotionTypes_InvalidTypes:
                        SHELL_Printf(shellContextHandle, "%d types emotion recognition is invalid\r\n", eTypes);
                        break;
                    case kStatus_API_Layer_SetEmotionTypes_Disabled:
                        SHELL_Printf(shellContextHandle, "Emotion recognition is disabled, system will reset\r\n");
                        VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
                        break;
                    default:
                        SHELL_Printf(shellContextHandle, "ERROR API ENGINE");
                        break;
                }
            }
            else
            {
                /* show the current emotion types */
                VIZN_GetEmotionTypes(&VIZN_API_CLIENT(Shell), &eTypes);
                if ((eTypes == EMOTION_2_TYPES) || (eTypes == EMOTION_4_TYPES) ||
                    (eTypes == EMOTION_7_TYPES))
                {
                    SHELL_Printf(shellContextHandle, "%d types emotion recognition\r\n", eTypes);
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "emotion recognition is disabled\r\n");
                }
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_LIVENESS)
        {
            uint32_t status;
            if (queueMsg.argc == 2)
            {
                if (!strcmp((char *)queueMsg.argv[1], "on"))
                {
                    status = VIZN_SetLivenessMode(&VIZN_API_CLIENT(Shell),LIVENESS_MODE_ON);
                    if(status == kStatus_API_Layer_Success){
                        SHELL_Printf(shellContextHandle, "liveness mode enabled, system will reset.\r\n");
                        VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
                    }else{
                        SHELL_Printf(shellContextHandle, "Cannot save liveness mode\r\n");
                    }
                }
                else if (!strcmp((char *)queueMsg.argv[1], "off"))
                {
                    status = VIZN_SetLivenessMode(&VIZN_API_CLIENT(Shell),LIVENESS_MODE_OFF);
                    if(status == kStatus_API_Layer_Success){
                        SHELL_Printf(shellContextHandle, "liveness mode disabled, system will reset.\r\n");
                        VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
                    }else{
                        SHELL_Printf(shellContextHandle, "Cannot save liveness mode\r\n");
                    }
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Undefined mode\r\n");
                }
            }
            else
            {
            	cfg_liveness_t mode;
                status = VIZN_GetLivenessMode(&VIZN_API_CLIENT(Shell),&mode);
                if (status == kStatus_API_Layer_Success)
                {
                    SHELL_Printf(shellContextHandle, "Liveness mode %s\r\n",(mode == LIVENESS_MODE_ON) ? "on":"off");
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Liveness not supported\r\n");
                }
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_DISPLAY)
        {
            uint32_t status;
            if (queueMsg.argc == 2)
            {
                if (!strcmp((char *)queueMsg.argv[1], "rgb"))
                {
                    status = VIZN_SetDispMode(&VIZN_API_CLIENT(Shell),DISPALY_MODE_RGB);
                    if(status == kStatus_API_Layer_Success){
                        SHELL_Printf(shellContextHandle, "display rgb frame\r\n");
                    }else{
                        SHELL_Printf(shellContextHandle, "display mode not supported\r\n");
                    }
                }
                else if (!strcmp((char *)queueMsg.argv[1], "ir"))
                {
                    status = VIZN_SetDispMode(&VIZN_API_CLIENT(Shell),DISPALY_MODE_IR);
                    if(status == kStatus_API_Layer_Success){
                        SHELL_Printf(shellContextHandle, "display ir frame\r\n");
                    }else{
                        SHELL_Printf(shellContextHandle, "display mode not supported\r\n");
                    }
                }
                else
                {
                    SHELL_Printf(shellContextHandle, "Undefined mode\r\n");
                }
            }
            else
            {
                SHELL_Printf(shellContextHandle, "Undefined command for display\r\n");
            }
        }
        else if (queueMsg.shellCommand == SHELL_EV_FFI_CLI_APP)
        {
            uint32_t status;
            uint8_t type;
            if(strcmp((char *)queueMsg.argv[1], "type") ||(queueMsg.argc > 3))
            {
                SHELL_Printf(shellContextHandle, "wrong command\r\n");
            }
            else
            {
                if (queueMsg.argc == 3)
                {
                    type = atoi(queueMsg.argv[2]);

                    if (type <= APP_TYPE_DOOR_ACCESS_HEAVY)
                    {
                        status = VIZN_SetAppType(&VIZN_API_CLIENT(Shell),(cfg_apptype_t)type);
                        if(status == kStatus_API_Layer_Success){
                            SHELL_Printf(shellContextHandle, "set app type %d, system will remove existing db & reset.\r\n",type);
                            status = VIZN_DelUser(&VIZN_API_CLIENT(Shell));
                            if ( status == kStatus_API_Layer_DelUser_FailedDeleteAll)
                            {
                                SHELL_Printf(shellContextHandle, "Failed deleting all users\r\n");
                            }
                            else
                            {
                                SHELL_Printf(shellContextHandle, "All users successfuly deleted\r\n");
                            }
                            VIZN_SystemReset(&VIZN_API_CLIENT(Shell));
                        }else{
                            SHELL_Printf(shellContextHandle, "Cannot save elock app type \r\n");
                        }
                    }
                    else
                    {
                        SHELL_Printf(shellContextHandle, "Undefined app type\r\n");
                    }
                }
                else if(queueMsg.argc == 2)
                {
                    status = VIZN_GetAppType(&VIZN_API_CLIENT(Shell),(cfg_apptype_t*)&type);
                    if (status == kStatus_API_Layer_Success)
                    {
                        SHELL_Printf(shellContextHandle, "App type:%d\r\n",type);
                    }
                    else
                    {
                        SHELL_Printf(shellContextHandle, "App type not supported\r\n");
                    }
                }
            }
        }
        else if(queueMsg.shellCommand == SHELL_EV_FFI_CLI_LPM)
        {
            lpm_power_mode_t lpm_targetsetting = (lpm_power_mode_t)atoi(queueMsg.argv[1]);
                SLN_PowerModeSwitch(lpm_targetsetting);
        }
        SHELL_Printf(shellContextHandle, USB_SHELL_PROMPT);
    }

    vTaskDelete(NULL);
}

