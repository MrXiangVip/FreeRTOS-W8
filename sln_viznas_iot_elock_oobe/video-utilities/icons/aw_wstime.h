// 20201121 wszgx added for date/time display
#ifndef _WS_TIME_H
#define _WS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int ws_time_t;

struct ws_tm {
	int tm_sec; /* 秒 – 取值区间为[0,59] */
	int tm_min; /* 分 - 取值区间为[0,59] */
	int tm_hour; /* 时 - 取值区间为[0,23] */
	int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
	int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	int tm_year; /* 年份，其值等于实际年份减去1900 */
};
void ws_localtime(ws_time_t time,struct ws_tm *t);
ws_time_t ws_mktime(struct ws_tm dt);

#ifdef __cplusplus
}
#endif

#endif
