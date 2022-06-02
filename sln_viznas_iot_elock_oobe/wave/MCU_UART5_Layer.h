/**************************************************************************
 * 	FileName:	 	MCU_UART5.h
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu.
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:        xshx on 2022/5/20.
 *
**************************************************************************/
#ifndef W8_MCU_UART5_LAYER_H
#define W8_MCU_UART5_LAYER_H



#include "../video-utilities/aw_wstime.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "WAVE_COMMON.h"


/*******************************************************************************
					 Protocol Msg Define
 ******************************************************************************/
#define FAILED		1
#define SUCCESS		0

#define UUID_LEN				8	// uuid长度
#define WIFI_SSID_LEN_MAX		31	// wifi ssid 最大长度
#define WIFI_PWD_LEN_MAX		63	// wifi pwd 最大长度
#define MQTT_USER_LEN 			6	// MQTT USER 最大长度
#define MQTT_PWD_LEN 			8	// MQTT pwd 最大长度
#define MQTT_SVR_URL_LEN_MAX	63	// mqtt server url 最大长度

#define UART5_MSG_Q_COUNT 	10
#define RT106F_MAX_WORKTIME		10

#define HEADMARK_LEN		1
#define HEAD_MARK			0x23     /*  ‘#’*/
#define DIRECT_SEND			0x01     /*  方向从w8 发出的 */
#define HEAD_MARK_MQTT		0x24     /*  ‘for mqtt’*/
#define CRC16_LEN			2
#define Version_LEN			5   /*版本长度固定为5, e.g 1.0.1*/
//#define SYS_VERSION		"0.0.2"
#define VERSION         "0.1"
#define REG_ACTIVE_OUTTIME   6000 //6s

#define CMD_INITOK_SYNC          		0x01		// 开机同步
#define CMD_HEART_BEAT		          	0x02		// 心跳
#define CMD_OPEN_DOOR                   0x03		// 开门
#define CMD_CLOSE_FACEBOARD				0x04		// 关机
#define CMD_FACE_REG					0x05		// 注册请求
#define CMD_REG_STATUS_NOTIFY			0x06		// 注册状态通知
#define CMD_FACE_REG_RLT				0x07		//注册结果
#define CMD_REG_ACTIVE_BY_PHONE			0x08		// 注册激活
#define CMD_FACE_DELETE_USER			0x09		// 删除用户
#define CMD_NTP_SYS_TIME				0x0A		// 网络对时
#define CMD_REQ_RESUME_FACTORY			0x0B		// 请求恢复出厂设置， MCU发起
#define CMD_UPGRADE_CTRL				0x0C		// 升级控制
#define CMD_UPGRADE_PACKAGE				0x0D		// 升级单包传输
#define CMD_FACE_RECOGNIZE				0x0E		// MCU主动请求人脸识别
#define CMD_WIFI_SSID					0x0F		// 设置wifi的SSID
#define CMD_WIFI_PWD					0x10		// 设置wifi的PWD
#define CMD_WIFI_MQTT					0x11		// 设置MQTT参数
#define CMD_WIFI_CONN_STATUS			0x12		// 上报wifi连接状态
#define CMD_DEV_DEBUG					0x13		// 上报设备故障状态
#define CMD_OPEN_LOCK_REC				0x14		// 上报开门记录
#define CMD_ORDER_TIME_SYNC				0x15		// 远程订单时间同步
#define CMD_BT_INFO						0x16		// 上报蓝牙模块信息
#define CMD_WIFI_MQTTSER_URL			0x17		// 设置wifi的MQTT server登录URL（可能是IP+Port，可能是域名+Port）
#define CMD_GETNETWORK_OPTVER			0x18		// 主控获取MCU中flash存储的网络参数设置版本号
#define CMD_SETNETWORK_OPTVER			0x19		// 设置网络参数版本号,每次设置后都自加1，存入flash
#define CMD_NETWORK_OPT					0x1A		// 网络参数设置
#define CMD_MQTT_UPLOAD					0x1B		// 请求mqtt 上传记录

#define CMD_TEMPER_DATA                 0x30        //蓝牙测温

#define CMD_REQ_POWER_DOWN              0x40        //mcu 请求下电

#define CMD_BOOT_MODE                   0x82  //上报开机模式 (与其它命令有区别 06.21)
#define CMD_WIFI_OPEN_DOOR				0x83	// 远程wifi开门
#define CMD_WIFI_TIME_SYNC				0x8A	// 通过wifi设置系统时间
#define CMD_MECHANICAL_LOCK				0x93	//机械开锁
#define CMD_IRLIGHT_PWM_Req				0xE0	// 设置IR补光灯

/*注册消息时响应类型*/
#define ID_REG_DISTANSE_NEAR				0xff04
#define ID_REG_DISTANSE_FAR				0xff05
#define ID_REG_ADDFACE_NULL				0xff10
#define ID_REG_PITCH_UP					0xff11
#define ID_REG_PITCH_UP_TOO_BIG			0xff12   //0xff12抬头角度过大
#define ID_REG_PITCH_UP_TOO_SMALL		0xff13	//0xff13抬头角度过小
#define ID_REG_PITCH_DOWN					0xff21
#define ID_REG_PITCH_DOWN_TOO_BIG		0xff22	//0xff22低头角度过大
#define ID_REG_PITCH_DOWN_TOO_SMALL		0xff23	//0xff23低头角度过小
#define ID_REG_YAW_LEFT					0xff31
#define ID_REG_YAW_LEFT_TOO_BIG			0xff32	//0xff32左偏脸角度过大
#define ID_REG_YAW_LEFT_TOO_SMALL		0xff33	//0xff33左偏脸角度过小
#define ID_REG_YAW_RIGHT					0xff41
#define ID_REG_YAW_RIGHT_TOO_BIG			0xff42	//0xff42右偏脸角度过大
#define ID_REG_YAW_RIGHT_TOO_SMALL		0xff43	//0xff43右偏脸角度过小
#define ID_REG_ROLL_LEFT					0xff51
#define ID_REG_ROLL_LEFT_TOO_BIG			0xff52	//0xff52头靠左肩角度过大
#define ID_REG_ROLL_LEFT_TOO_SMALL		0xff53	//0xff53头靠左肩角度过小
#define ID_REG_ROLL_RIGHT					0xff61
#define ID_REG_ROLL_RIGHT_TOO_BIG		0xff62	//0xff62头靠右肩角度过大
#define ID_REG_ROLL_RIGHT_TOO_SMALL		0xff63	//0xff63头靠右肩角度过小

#define TEST_ANY_FACE_REC // 任意人脸均可识别成功
#undef TEST_ANY_FACE_REC

#define DEMO_LPUART          LPUART5
#define DEMO_LPUART_IRQn     LPUART5_IRQn
#define DEMO_LPUART_CLK_FREQ BOARD_DebugConsoleSrcFreq()

#define UART5_RX_MAX_DATA_PACKAGE_SIZE 32    //20

typedef struct _stRpMsgHead
{
    unsigned char		HeadMark;		/* 前导标识符（长度为1bytes）用于校验消息是否合法 */
    unsigned char  	CmdID;			/* 消息命令（长度为1bytes） */
    unsigned char  	DirectFilt;			/* 方向过滤*/
    unsigned char 	MsgLen;			/* 数据长度（数据长度为1bytes） */
}MESSAGE_HEAD, *PMESSAGE_HEAD;

typedef struct _stRpRspHead
{
    unsigned char		HeadMark;		/* 前导标识符（长度为1bytes）用于校验消息是否合法 */
    unsigned char  	RspID;			/* 0xFE */
    unsigned char  	DirectFilt;			/* 方向过滤*/
    unsigned char 	MsgLen;			/* 数据长度（数据长度为1bytes） */
}RESPONSE_HEAD, *PRESPONSE_HEAD;

/*硬件故障类型定义*/
typedef enum
{
    HW_BUG_NOERROR=0,
    HW_BUG_CAMERA=1,
    HW_BUG_LCD,
    HW_BUG_ENCRYPTIC,
    HW_BUG_RPMSG,
    HW_BUG_EMMC,
    HW_BUG_UART5,
}HwAbnormalType;

/*注册结果状态*/
typedef enum
{
    REG_STATUS_OK=0,	// 注册成功
    REG_STATUS_FAILED=1,	// 注册失败
    REG_STATUS_DUP=2,    // 重复注册
    REG_STATUS_ING=3,	//注册中
    REG_STATUS_WAIT=4,	//等待注册
}Reg_Status;

//0:短按;1：长按;2:蓝牙设置;3:蓝牙人脸注册;4:睡眠状态
typedef enum
{
    BOOT_MODE_POWERON  =  -1,     //  只是上电
    BOOT_MODE_RECOGNIZE = 0,		// 短按--识别模式
    BOOT_MODE_REMOTE = 1,		// 长按--远程开锁模式
    BOOT_MODE_BLE = 2,      	// 蓝牙设置
    BOOT_MODE_REGIST = 3,			//蓝牙人脸注册
    BOOT_MODE_PREVIEW = 4,      // 预览
    BOOT_MODE_MECHANICAL_LOCK = 0xB,	//机械锁模式
    BOOT_MODE_INVALID = 0XFF,	//非法模式
}EBootMode;

///*8字节UUID*/
//typedef union{
//    struct{
//        uint32_t L;
//        uint32_t H;
//    }tUID;
//    uint8_t UID[8];
//}uUID;

typedef struct wf_face_info{
    char name[64];
    bool recognize;
    bool enrolment;
    int rt;
    unsigned face_id;
}wf_face_info_t;

typedef struct userInfo_data_t
{
    uUID uuId;  /**APP 端产生的UUID，8字节 */
    uint8_t userName[12];     /**NXP 算法维护的username*/
} userInfo_data_t;


int MsgTail_Pack(char *Message, int iTailIndex );

//
extern int cmdSysInitOKSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdOpenDoorRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdTemperRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdPowerDownRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdUserRegReqProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdSetSysTimeSynProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdDeleteUserReqProcByHead(unsigned char nHead, unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdReqResumeFactoryProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdReqActiveByPhoneProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdWifiPwdProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdMqttParamSetProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdWifiTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdWifiOrderTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdWifiOpenDoorRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdBTInfoRptProc(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdMqttSvrURLProc(unsigned char nMessageLen, const unsigned char *pszMessage);

extern int cmdMechicalLockRsp(unsigned char nMessageLen, const unsigned char *pszMessage);
//
extern int cmdReqPoweDown(unsigned char nMessageLen, const unsigned char *pszMessage);

//extern int cmdRegResultNotifyReq(uUID uu_id, uint8_t regResult);
extern int cmdRegResultNotifyReq(UserExtendType *userExtendType, uint8_t regResult);

extern int cmdWifiSSIDProc(unsigned char nMessageLen, const unsigned char *pszMessage);

//send qMsg to uart5 task when face register over



//从oasis_rt106f_elcok.cpp中获取识别或者注册时的face_info
extern void Set_curFaceInfo(char *name, bool recognize, bool enrolment, int rt);

//int ProcMessage(unsigned char nCommand, unsigned char nMessageLen, const unsigned char *pszMessage);

extern int SendMsgToMCU(uint8_t *MsgBuf, uint8_t MsgLen);
extern int cmdSysInitOKSyncReq(const char *strVersion);
extern void SysTimeSet(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

// 主控发送指令:请求MQTT 上传记录
extern int cmdRequestMqttUpload(int id);
//extern int cmdOpenDoorReq(uUID uu_id);
extern int cmdOpenDoorReq(UserExtendType *userExtendType);
//关机请求
extern int cmdCloseFaceBoardReq();
extern int cmdCloseFaceBoardReqExt(bool save_file);
extern int cmdCommRsp2MqttByHead(unsigned char nHead, unsigned char CmdId, uint8_t ret);//106F->MQTT: 通用响应
extern int cmdCommRsp2Mqtt(unsigned char CmdId, uint8_t ret);//106F->MQTT: 通用响应
//
extern int save_config_feature_file();
//
//
extern int save_files_before_pwd();

extern const unsigned char *MsgHead_Unpacket(
        const unsigned char *pszBuffer,
        unsigned char iBufferSize,
        unsigned char *HeadMark,
        unsigned char *CmdId,
        unsigned char *MsgLen);
#ifdef __cplusplus
}
#endif

// 20201119 wszgx modified for display correct battery information in the screen
extern bool	bInitSyncInfo;
extern bool bSysTimeSynProc;
// 20201119 wszgx end
// 20201120 wszgx modified for display correct date/time information in the screen
extern ws_time_t ws_systime;
// 20201120 wszgx end

extern  Reg_Status g_reging_flg;//正在注册流程标记,等待被激活，对识别task发过来的识别结果进行过滤
extern  char username[17];
extern  uUID g_uu_id;  //记录当前应用的uuid
extern  int boot_mode;
extern  lpuart_rtos_handle_t handle5;


#endif //W8_MCU_UART5_LAYER_H
