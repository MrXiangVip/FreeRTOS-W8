// 20201121 wszgx added for date/time display
#include <aw_wstime.h>

// typedef unsigned int time_t;
// 
// struct tm {
// 	int tm_sec; /* 秒 – 取值区间为[0,59] */
// 	int tm_min; /* 分 - 取值区间为[0,59] */
// 	int tm_hour; /* 时 - 取值区间为[0,23] */
// 	int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
// 	int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
// 	int tm_year; /* 年份，其值等于实际年份减去1900 */
// };

const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void ws_localtime(ws_time_t time,struct ws_tm *t)
{
	unsigned int Pass4year;
	int hours_per_year;
	if(time < 0)
	{
		time = 0;
	}
	//取秒时间
	t->tm_sec=(int)(time % 60);
	time /= 60;
	//取分钟时间
	t->tm_min=(int)(time % 60);
	time /= 60;
	//取过去多少个四年，每四年有 1461*24 小时
	Pass4year=((unsigned int)time / (1461L * 24L));
	//计算年份
	t->tm_year=(Pass4year << 2) + 1970;
	//四年中剩下的小时数
	time %= 1461L * 24L;
	//校正闰年影响的年份，计算一年中剩下的小时数
	for (;;)
	{
		//一年的小时数
		hours_per_year = 365 * 24;
		//判断闰年
		if ((t->tm_year & 3) == 0)
		{
			//是闰年，一年则多24小时，即一天
			hours_per_year += 24;
		}
		if (time < hours_per_year)
		{
			break;
		}
		t->tm_year++;
		time -= hours_per_year;
	}
	//小时数
	t->tm_hour=(int)(time % 24);
	//一年中剩下的天数
	time /= 24;
	//假定为闰年
	time++;
	//校正闰年的误差，计算月份，日期
	if((t->tm_year & 3) == 0)
	{
		if (time > 60)
		{
			time--;
		}
		else
		{
			if (time == 60)
			{
				t->tm_mon = 1;
				t->tm_mday = 29;
				return ;
			}
		}
	}
	//计算月日
	for (t->tm_mon = 0; Days[t->tm_mon] < time;t->tm_mon++)
	{
		time -= Days[t->tm_mon];
	}
	t->tm_mday = (int)(time);
	return;
}

static ws_time_t mon_yday[2][12] =
{
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
	{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

int ws_isleap(int year)
{
	return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

ws_time_t ws_mktime(struct ws_tm dt)
{
	ws_time_t result;
	int i =0;
	// 以平年时间计算的秒数
	result = (dt.tm_year - 1970) * 365 * 24 * 3600 +
		(mon_yday[ws_isleap(dt.tm_year)][dt.tm_mon-1] + dt.tm_mday - 1) * 24 * 3600 +
		dt.tm_hour * 3600 + dt.tm_min * 60 + dt.tm_sec;
	// 加上闰年的秒数
	for(i=1970; i < dt.tm_year; i++)
	{
		if(ws_isleap(i))
		{
			result += 24 * 3600;
		}
	}
	return (result);
}

