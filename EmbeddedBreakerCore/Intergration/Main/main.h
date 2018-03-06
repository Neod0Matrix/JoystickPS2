#pragma once							
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//配合main函数所在文件进行声明

//MCU资源
#define __MCU_Model__			"STM32F103RET6"			//主控芯片型号
#define __STM32F10x_FWLib__		"v3p5"					//STM32固件库版本
#define __MCU_Flash_Size__		"512"					//Flash
#define __MCU_SRAM_Size__		"64"					//SRAM

//工程声明
#define __Project_Type__		"Frame"					//工程类型
#define __Frame_Name__			"EmbeddedBreakerCore"	//架构名称
#define __AbbrFrameName__		"EBCr"					//名称缩写(4字以内)
#define __Code_Version__ 		"OS_v0p6_LTE"			//长期演进版
#define __Laboratory__			"T.WKVER"				//实验室
#define __Logo__				"Absolute Liberal"		//logo
#define __Developer__			"Neod Anderjon"			//开发者
#define __AbbrDeveloper__		"NA"					//开发者缩写
#define __Organization__		"</MATRIX>"				//组织
#define __Topple_Over__			"_(:з」∠)_ _(┐「ε:)_"	//趴下颜文字
#define __FunnyWord__			"(ಡωಡ)"					//滑稽颜文字

//嵌入式系统版本
#define __OS_Version__			"uC/OS-III v3p03"		//ucosiii		

//时钟设置
#define InputOSCFreq			8						//输入晶振频率
#define RCCMultipConst			(72 / InputOSCFreq)		//倍频系数

/*
	常用数据类型宏定义本质：
	uint8_t -- u8 --- unsigned char
	uint16_t - u16 -- unsigned short int
	uint32_t - u32 -- unsigned int
	uint64_t - u64 -- unsigned long int
*/

//bool型
typedef enum {True = 1, False = !True} Bool_ClassType;
//定义enum型IO口电平高低状态
typedef enum {hvl = 1, lvl = !hvl} IO_VoltLevel_Pull;
//工作状态标志
typedef enum {JBoot = 0, StartW = 1} PrgmaWork_Status;
extern PrgmaWork_Status pwsf;		

void bspPeriSysCalls (void);							//初始化函数封装，好让做的工作看起来比原来多
static void preSetUpHardware (void);					//开始进入任务前的准备工作
static void stackTaskManager (void);					//任务管理栈
int main (int argc, char *argv[], char *envp[]);		

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
