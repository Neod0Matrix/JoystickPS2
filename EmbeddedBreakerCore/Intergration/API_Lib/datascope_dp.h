#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DataScope下位机库

//待发送帧数据缓存区，里面存放经过功能函数格式化后的待发送帧数据
#define StableWidthSize	42u

extern u8 DataScope_OutPut_Buffer[StableWidthSize];	   			//待发送帧数据缓存区

void Float2Byte (float *target, u8 *buf, u8 beg);
extern void DataScope_Get_Channel_Data(float Data, u8 Channel); //写通道数据至 待发送帧数据缓存区
extern u8 DataScope_Data_Generate(u8 Channel_Number);  			//发送帧数据生成函数
extern void DataScope_LocalBuild (float dat, u8 ch);			//内部构建
extern void DataScope_Test (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
