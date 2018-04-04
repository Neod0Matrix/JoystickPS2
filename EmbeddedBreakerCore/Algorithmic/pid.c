#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//运动控制普遍存在的稳态误差调节
//这里只做算法公式代码化和测试
/*
	△u(k) = u(k) - u(k-1)
	△u(k) = Kp[e(k) - e(k - 1)] + Kie(k) + Kd[e(k) - 2e(k - 1) + e(k - 2)]
	进一步可以改写成
	△u(k) = Ae(k) - Be(k - 1) + Ce(k - 2)
	输入值即设定值，输出值即实际值
*/

#define Debug_COUNT 500u									//摆动上限，依照输入值做适当变化

//InitializePID Structure  PID参数初始化
void pidStructPara_Init (
							u16 input_value, 
							PID_Param *sptr, 
							float kp, 
							float ki, 
							float kd)
{
	//结构体成员全部初始化
	sptr -> SetPoint = 0.f;									//设置点
	sptr -> SumError = 0.f;									//总误差
	sptr -> ActValue = 0.f;									//实际值
    sptr -> LastError = 0.f; 								//上一个误差
	//pid系数可调
    sptr -> Kp = kp;										//比例常数
	sptr -> Ki = ki;										//积分常数
	sptr -> Kd = kd;										//微分常数
	//积分设定
	sptr -> integral = 0.f;									//积分值
	sptr -> uOutput = 0.f;									//输出值u(t)
	sptr -> uMax = input_value * 2.f;						//上限饱和
	sptr -> uMin = -input_value * 1.f;						//下限饱和
}
 
//增量式PID计算部分 
//传参：输入值，对象指针，选择公式
float pidFma_IncreCalcu (u16 input_value, PID_Param *sptr, Formula_Mode fomd)
{
	u16 index;												//积分分离
#define INTEGRAL_PARAM	0.9f								//变积分系数
	
	/*
		抗积分饱和补偿
		积分分离缩短系统进入稳定的时间
		变积分PID 可以看成是积分分离的PID算法的更一般的形式
		在普通的PID控制算法中，由于积分系数ki是常数，所以在整个控制过程中，积分增量是不变的
		但是，系统对于积分项的要求是，系统偏差大时，积分作用应该减弱甚至是全无，而在偏差小时，则应该加强
		积分系数取大了会产生超调，甚至积分饱和，取小了又不能短时间内消除静差
	*/
	sptr -> SetPoint = input_value;							//设置传参
	sptr -> SumError = sptr -> SetPoint - sptr -> ActValue;	//增量计算
	
	if (sptr -> ActValue > sptr -> uMax)
	{
		if (fabs(sptr -> SumError) > input_value) index = 0u;//积分分离
		else if (fabs(sptr -> SumError) < input_value * INTEGRAL_PARAM)
		{
			index = 1u;
			if (sptr -> SumError < 0u) 
				sptr -> integral += sptr -> SumError;					
		}
		else
		{
			index = (input_value - fabs(sptr -> SumError)) / (input_value * (1.f - INTEGRAL_PARAM));
			sptr -> integral += sptr -> SumError;
		}
	}
	else if (sptr -> ActValue < sptr -> uMin)
	{
		if (fabs(sptr -> SumError) > input_value) index = 0u;//积分分离
		else if (fabs(sptr -> SumError) < input_value * INTEGRAL_PARAM)//变积分
		{
			index = 1u;
			if (sptr -> SumError > 0u) 
				sptr -> integral += sptr -> SumError;	
		}
		else
		{
			index = (input_value - fabs(sptr -> SumError)) / (input_value * (1.f - INTEGRAL_PARAM));
			sptr -> integral += sptr -> SumError;
		}
	}
	else
	{
		if (fabs(sptr -> SumError) > input_value) index = 0u;//积分分离
		else if (fabs(sptr -> SumError) < input_value * INTEGRAL_PARAM)//变积分
		{
			index = 1u;
			if (sptr -> SumError > 0u) 
				sptr -> integral += sptr -> SumError;	
		}
		else
		{
			index = (input_value - fabs(sptr -> SumError)) / (input_value * (1.f - INTEGRAL_PARAM));
			sptr -> integral += sptr -> SumError;
		}
	}
	
	/*
		带入PID控制算法公式实现误差摆动分析
		选择一般积分分离公式或者是梯形积分公式
		可在多处设置摆幅参量
	*/
	//积分分离公式
	if (fomd == normal)	
		sptr -> uOutput = (sptr -> Kp * sptr -> SumError)	//比例
						+ (index * sptr -> Ki * sptr -> integral)//积分
						+ ((sptr -> Kd) * (sptr -> SumError - sptr -> LastError));//微分
	
	//梯形积分公式
	else if (fomd == echelon) 
		sptr -> uOutput = (sptr -> Kp * sptr -> SumError)	//比例
						+ (index * sptr -> Ki * sptr -> integral / 2)//梯形积分
						+ ((sptr -> Kd) * (sptr -> SumError - sptr -> LastError));//微分
	
	sptr -> LastError = sptr -> SumError;					//存储误差，用于下次计算
	sptr -> ActValue = sptr -> uOutput;						//传参
  
	return sptr -> ActValue;								//返回实际值
}

//测试函数
void PID_Test (void)
{
#define Standy_Range 200u									//摆动上限，依照输入值做适当变化
	
	static PID_Param testPID;
	static PID_Param *test_sptr = &testPID;

	u16 test_count = 1u;									//摆动计数
	u16 input_value = 2800u;								//测试摆动输入值
	
	//初始化结构体参数
	pidStructPara_Init(
							input_value, 
							test_sptr, 
							//pid系数设定
							2.f, 							//kp
							0.01f, 							//ki
							0.1f);							//kd
	
	while (test_count < Standy_Range)						//测试结果：当误差累积到300次左右，实际值与测量值十分接近
	{
		__ShellHeadSymbol__; 
		U1SD("PID Test: No.%04d Output: %04.3f\r\n", 
			test_count, pidFma_IncreCalcu(input_value, test_sptr, normal));
		test_count++;										//重复操作					
	}
}

//利用PID加成运动控制：频率
float motorPID_DebugFreq (u16 input_freq)
{	
	static PID_Param mfpid;
	static PID_Param *mf_sptr = &mfpid;

	u16 test_count = 1u;									//摆动计数
	float pidOutputfreq;									//传参，最后结果

	if (pidDS_Switch == pidDS_Enable)
	{
		//初始化结构体参数
		pidStructPara_Init(
								input_freq, 
								mf_sptr, 
								//pid系数设定
								0.2f, 						//kp
								0.04f, 						//ki
								0.2f);						//kd

		//重复操作，刷值
		while (test_count < Debug_COUNT)						
		{
			pidOutputfreq = pidFma_IncreCalcu(input_freq, mf_sptr, normal);
			test_count++;													
		} 
	}
	
	//如果关闭则相当于设定值不变
	else 
		pidOutputfreq = input_freq;
	
	return pidOutputfreq;
}

//利用PID加成运动控制：行距
float motorPID_DebugDis (u16 input_dis)
{	
	static PID_Param mdpid;
	static PID_Param *md_sptr = &mdpid;

	u16 test_count = 1u;									//摆动计数
	float pidOutputdis;										//传参，最后结果

	if (pidDS_Switch == pidDS_Enable)
	{
		//初始化结构体参数
		pidStructPara_Init(
								input_dis, 
								md_sptr, 
								//pid系数设定
								0.2f, 						//kp
								0.04f, 						//ki
								0.2f);						//kd

		//重复操作，刷值
		while (test_count < Debug_COUNT)						
		{
			pidOutputdis = pidFma_IncreCalcu(input_dis, md_sptr, normal);
			test_count++;													
		} 
	}
	
	//如果关闭则相当于设定值不变
	else 
		pidOutputdis = input_dis;
	
	return pidOutputdis;
}
 
//====================================================================================================
//code by </MATRIX>@Neod Anderjon
