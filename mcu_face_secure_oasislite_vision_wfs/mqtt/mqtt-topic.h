#ifndef _MQTT_TOPIC_H_
#define _MQTT_TOPIC_H_

//#define CODE_SUCCESS 0
//#define CODE_FAILURE 1

#ifdef __cplusplus
extern "C" {
#endif

/*** 订阅TOPIC ***/
// 后台命令
char *get_sub_topic_cmd();
// 时间同步订阅
char *get_sub_topic_time_sync();

/*** 发布TOPIC ***/
// 时间同步发布
char *get_pub_topic_time_sync();
// 图片上传
char *get_pub_topic_pic_report();
// 图片用户
char *get_pub_topic_pic_report_u();
// 心跳
char *get_pub_topic_heart_beat();
// 遗愿消息
char *get_pub_topic_last_will();
// 命令返回给mqtt后台
char *get_pub_topic_cmd_res();
// 开门记录上报
char *get_pub_topic_record_request();

#ifdef __cplusplus
}
#endif

#endif
