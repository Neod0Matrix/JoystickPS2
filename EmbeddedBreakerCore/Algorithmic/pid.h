#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//运动控制普遍存在的稳态误差调节

//PID参数结构体
typedef __packed struct 
{
	float SetPoint; 				//设定目标 
	float ActValue;					//实际值
	float SumError; 				//误差累计
	float LastError; 				//上一处误差
	float Kp, Ki, Kd;				//PID常数
	float integral;					//积分值
	float uOutput;					//输出值
	float uMax, uMin;				//上下限饱和
} PID_Param;						//参数集

//算法选择
typedef enum {normal = 1u, echelon = 2u} Formula_Mode;
 
//初始化PID参数
void pidStructPara_Init (	
	u16 input_value, 				//输入值	
	PID_Param *sptr, 				//PID指针
	float kp, 						//初始化比例系数
	float ki, 						//初始化积分系数
	float kd);						//初始化微分系数
	
//增量式PID计算
float pidFma_IncreCalcu (					
	u16 input_value, 				//输入值		
	PID_Param *sptr, 				//PID指针
	Formula_Mode fomd);				//公式选择
	
void PID_Test (void);				//摆动测试
float motorPID_DebugFreq (u16 input_freq);//频率pid
float motorPID_DebugDis (u16 input_dis);//行距pid

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
