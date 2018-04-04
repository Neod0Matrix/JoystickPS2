#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//RTC实时时钟 驱动代码

#define Ext_Lowspeed_psc	32768u				//外部低速晶振分频

//时间结构体
typedef __packed struct
{
    vu8 	hour;								//时
    vu8 	min;								//分
    vu8 	sec;								//秒
    vu16 	w_year;								//年
    vu8  	w_month;							//月
    vu8  	w_date;								//日
    vu8  	week;								//周
} _calendar_obj;

extern _calendar_obj calendar, _start;					

//月份日期数据表
extern const u8 month_data_list[12];					
extern const u8 leap_month_list[12];	
extern const char* week_str[7];

//RTC全局数据寄存数组
extern u16 rtcTotalData[7];

void RTC_TimeInitSetting (_calendar_obj *st);	//初设时间
void Disp_Time (u8 x, u8 y, u8 size);			//在制定位置开始显示时间
void Disp_Week (u8 x, u8 y, u8 size, u8 lang);	//在指定位置显示星期
Bool_ClassType RTC_Init (void);        			//初始化RTC
Bool_ClassType Is_Leap_Year (u16 year);			//平年，闰年判断
Bool_ClassType RTC_TimeUpdate (_calendar_obj* cal);//更新时间
u8 RTC_Get_Week (u16 year, u8 month, u8 day);
Bool_ClassType RTC_Set (u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);//设置时间
void RTC_DataStorage (_calendar_obj* rtc);		//RTC数据寄存
void RTC_Init_Check (void);						//初始化检查
extern void RTC_ReqOrderHandler (void);			//打印RTC时间

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
