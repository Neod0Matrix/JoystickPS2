#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//模块AttitudeAlgorithm对框架EmbeddedBreakerCore的链接
//该文件需要添加到stdafx.h内生效

//MCU资源
#define _MCU_Model_				"STM32F103RET6"			//主控芯片型号
#define _STM32F10x_FWLib_		"v3p5"					//STM32固件库版本
#define _MCU_Flash_Size_		"512"					//Flash
#define _MCU_SRAM_Size_			"64"					//SRAM

//工程声明
#define _Project_Type_			"GPS"					//工程类型
#define _Frame_Name_			"EmbeddedBreakerCore"	//架构名称
#define _Project_Name_			"GlobalPositioningSystem"//工程名称
#define _Code_Version_ 			"OS_v0p1_LTE"			//长期演进版
#define _Laboratory_			"T.WKVER"				//实验室
#define _Logo_					"Absolute Liberal"		//logo
#define _Developer_				"Neod Anderjon"			//开发者
#define _AbbrDeveloper_			"NA"					//开发者缩写
#define _Organization_			"</MATRIX>"				//组织
#define _Topple_Over_			"_(:з」∠)_ _(┐「ε:)_"	//趴下颜文字
#define _FunnyWord_				"(ಡωಡ)"					//滑稽颜文字

//嵌入式系统版本
#define _OS_Version_			"uC/OS-III v3p03"		//ucosiii	

//是否开启急停外部中断
typedef enum {StewEXTI_Enable = 1, StewEXTI_Disable = !StewEXTI_Enable}	Stew_EXTI_Setting;
extern Stew_EXTI_Setting			StewEXTI_Switch;
//是否开启GPS信息打印
typedef enum {GPSP_Enable = 1, GPSP_Disable = !GPSP_Enable} GPS_InfoPrint;
extern GPS_InfoPrint				GPSP_Switch;

//urc开源链接编号
typedef enum 
{
	urc_stew 	= 15,
	urc_gpsp	= 16,
} GPS_SwitchNbr;

//裁去protocol.h中的定义放到这里来重新定义urc协议长度
#define Max_Option_Value		16u

//裁去ui.h中定义的总切屏数到这里来重新定义
#define ScreenPageCount			5u						

void OLED_ScreenP4_Const (void);
void OLED_DisplayGPS (void);

//对外API接口
extern void U1RSD_example (void);						//串口处理例程封装
void ModuleGPS_UniResConfig (void);
void ModuleGPS_URCMap (void);
void ModuleGPS_urcDebugHandler (u8 ed_status, GPS_SwitchNbr sw_type);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
