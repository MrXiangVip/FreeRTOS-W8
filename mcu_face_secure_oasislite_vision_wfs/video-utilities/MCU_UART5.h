/**************************************************************************
 * 	FileName:	 	MCU_UART5.h
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu.
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:		
 *					
**************************************************************************/


#ifndef _MCU_UART5_H_
#define _MCU_UART5_H_

#include "aw_wstime.h"

#ifdef __cplusplus
extern "C" {
#endif


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

#define UART5_MSG_Q_COUNT 	5
#define RT106F_MAX_WORKTIME		10

#define HEADMARK_LEN		1  
#define HEAD_MARK			0x23     /*  ‘#’*/
#define HEAD_MARK_MQTT		0x24     /*  ‘for mqtt’*/
#define CRC16_LEN			2  
#define Version_LEN			5   /*版本长度固定为5, e.g 1.0.1*/
#define SYS_VERSION		"0.0.2"
#define VERSION         "0.1"
#define REG_ACTIVE_OUTTIME   6000 //6s

#define CMD_INITOK_SYNC          				1		// 开机同步
#define CMD_HEART_BEAT		          		2		// 心跳
#define CMD_OPEN_DOOR						3		// 开门
#define CMD_CLOSE_FACEBOARD				4		// 关机
#define CMD_FACE_REG						5		// 注册请求
#define CMD_REG_STATUS_NOTIFY			6		// 注册状态通知
#define CMD_FACE_REG_RLT					7		//注册结果
#define CMD_REG_ACTIVE_BY_PHONE			8		// 注册激活
#define CMD_FACE_DELETE_USER				9		// 删除用户
#define CMD_NTP_SYS_TIME					10		// 网络对时
#define CMD_REQ_RESUME_FACTORY			11		// 请求恢复出厂设置， MCU发起
#define CMD_UPGRADE_CTRL					12		// 升级控制
#define CMD_UPGRADE_PACKAGE				13		// 升级单包传输
#define CMD_FACE_RECOGNIZE				14		// MCU主动请求人脸识别
#define CMD_WIFI_SSID						15		// 设置wifi的SSID
#define CMD_WIFI_PWD						16		// 设置wifi的PWD
#define CMD_WIFI_MQTT						17		// 设置MQTT参数
#define CMD_WIFI_CONN_STATUS				18		// 上报wifi连接状态
#define CMD_DEV_DEBUG						19		// 上报设备故障状态
#define CMD_OPEN_LOCK_REC					20		// 上报开门记录
#define CMD_ORDER_TIME_SYNC				21		// 远程订单时间同步
#define CMD_BT_INFO						22		// 上报蓝牙模块信息
#define CMD_WIFI_MQTTSER_URL				23		// 设置wifi的MQTT server登录URL（可能是IP+Port，可能是域名+Port）
#define CMD_GETNETWORK_OPTVER			24		// 主控获取MCU中flash存储的网络参数设置版本号
#define CMD_SETNETWORK_OPTVER			25		// 设置网络参数版本号,每次设置后都自加1，存入flash
#define CMD_NETWORK_OPT					26		// 网络参数设置
#define CMD_MQTT_UPLOAD					27		// 请求mqtt 上传记录
#define CMD_WIFI_OPEN_DOOR				0x83	// 远程wifi开门
#define CMD_WIFI_TIME_SYNC				0x8A	// 通过wifi设置系统时间
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

typedef struct _stRpMsgHead
{
	unsigned char		HeadMark;		/* 前导标识符（长度为1bytes）用于校验消息是否合法 */
	unsigned char  	CmdID;			/* 消息命令（长度为1bytes） */
	unsigned char 	MsgLen;			/* 数据长度（数据长度为1bytes） */    
}MESSAGE_HEAD, *PMESSAGE_HEAD;	


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


/*8字节UUID*/
typedef union{
	struct{
		uint32_t L;
		uint32_t H;
	}tUID;
	uint8_t UID[8];
}uUID;

typedef struct 
{
	unsigned char	Mcu_Ver;			/*MCU版本，如D678表示版本6.7.8*/
	unsigned char 	PowerVal;			/*电量: 1~100*/
	unsigned char 	status;				/*默认为0;置1描述：Bit0 防撬开关单次触发（MCU上传后置0）；Bit1 恢复出厂设置按钮单次触发（MCU上传后置0）；Bit2 老化模式（MCU掉电后清0，由A5配置*/	
	unsigned char 	LightVal;			/*补光灯值:无->0;不要补光->1;补光:2*/	
}InitSyncInfo, *pInitSyncInfo;

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


int MCU_UART5_Start();

//send qMsg to uart5 task when face register over		
int  Uart5_GetFaceRegResult(uint8_t result);

//send qMsg to uart5 task when face recognize over			
int  Uart5_GetFaceRecResult(uint8_t result);

//从oasis_rt106f_elcok.cpp中获取识别或者注册时的face_info
void Set_curFaceInfo(char *name, bool recognize, bool enrolment, int rt);

int SendMsgToMCU(uint8_t *MsgBuf, uint8_t MsgLen);
int cmdSysInitOKSyncReq(const char *strVersion);

int cmdCommRsp2MqttByHead(unsigned char nHead, unsigned char CmdId, uint8_t ret);//106F->MQTT: 通用响应
int cmdCommRsp2Mqtt(unsigned char CmdId, uint8_t ret);//106F->MQTT: 通用响应
int sendSysInit2MQTT(uint16_t version, uint8_t powerValue);//请求MQTT上传开机时的版本，电量，状态信息
#ifdef __cplusplus
}
#endif

// 20201119 wszgx modified for display correct battery information in the screen
extern InitSyncInfo	stInitSyncInfo;		//记录初始化同步信息(电量、MCU版本等)
extern bool	bInitSyncInfo;
extern bool bSysTimeSynProc;
// 20201119 wszgx end
// 20201120 wszgx modified for display correct date/time information in the screen
extern ws_time_t ws_systime;
// 20201120 wszgx end
void OpenLcdBackground();
void CloseLcdBackground();

#endif /* _FACEREC_MCU_UART5L_H_ */
