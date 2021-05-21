/*
* Copyright 2019 NXP.
* This software is owned or controlled by NXP and may only be used strictly in accordance with the
* license terms that accompany it. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you
* agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
* applicable license terms, then you may not retain, install, activate or otherwise use the software.
*/

#include <assert.h>

#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "fsl_str.h"
#include "sln_shell.h"
#include "commondef.h"
#include "sln_vizn_init_api.h"
#include "sln_system_state.h"
#include "sln_vizn_api.h"

static system_states_t sys_state = sysStateDetectedNoUser;

int ShellDetect(VIZN_api_client_t *pClient, int dt)
{
    if(dt > 0)
        UsbShell_Printf("Face detected - dt = %d ms \n\r", dt);
    return 1;
}

int ShellRec(VIZN_api_client_t *pClient, face_info_t face_info )
{

    if (face_info.recognize)
    {
        UsbShell_Printf("User: %s - rt = %dms \r\n", face_info.name, face_info.rt  );
    }
    else
    {
        UsbShell_Printf("User: unknown - rt = %dms \r\n", face_info.rt);
    }

    return 1;
}

int ShellEnrolment(VIZN_api_client_t *pClient, face_info_t face_info)
{
    if(face_info.enrolment)
        UsbShell_Printf("User: %s enroled \r\n",face_info.name);
    return 1;
}

int LedDetect(VIZN_api_client_t *pClient, int dt)
{
    if(dt < 0){
        sys_state = sysStateDetectedNoUser;
        SysState_Set(sys_state);
    }
    return 1;
}

int LedRec(VIZN_api_client_t *pClient, face_info_t face_info )
{

    if(face_info.recognize)
        sys_state = sysStateDetectedKnownUser;
    else
        sys_state = sysStateDetectedUnknownUser;
    SysState_Set(sys_state);
    return 1;
}

client_operations_t ops = {
    .detect    = ShellDetect,
    .recognize = ShellRec,
    .enrolment = ShellEnrolment,
};
client_operations_t led_ops = {
    .detect    = LedDetect,
    .recognize = LedRec,
    .enrolment = NULL,
};
client_operations_t ops2 = {
    .detect    = NULL,
    .recognize = NULL,
    .enrolment = NULL,
};

VIZN_API_CLIENT_DEFINE(Shell, (char *)"Shell console", &ops);
VIZN_API_CLIENT_DEFINE(Led, (char *)"Led display", &led_ops);
VIZN_API_CLIENT_DEFINE(Buttons, (char *)"Push Buttons", &ops2);

int vizn_api_init()
{
    VIZN_API_Init(&gApiHandle);
    int status = 0;
    status |= (int)VIZN_RegisterClient(gApiHandle, &VIZN_API_CLIENT(Shell));
    status |= (int)VIZN_RegisterClient(gApiHandle, &VIZN_API_CLIENT(Led));
    status |= (int)VIZN_RegisterClient(gApiHandle, &VIZN_API_CLIENT(Buttons));

    if (status != 0)
    {
        return -1;
    }
    return 1;
}