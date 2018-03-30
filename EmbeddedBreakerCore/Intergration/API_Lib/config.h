#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//对系统内设/外设资源进行开关配置

/*------------------------------宏编译配置(使能配置则保留，不用则注释)---------------------------------*/
#define Frame_PreConfig					//使用框架预配置
#define MATRIX_fraExc_Lock 				//框架执行锁
#define StewLowLevelTrigger				//急停低电平触发
#define use_ULN2003A					//定义是否使用ULN2003A
//#define USART1_Remap_Enable				//是否开启USART1重映射
//#define USART2_Remap_Enable				//是否开启USART2重映射
#define USART1_RX_EN 					//USART1接收中断使能
#define USART2_RX_EN 					//USART2接收中断使能
//#define useRTOSinProject				//在项目中使用RTOS
//#define VECT_TAB_RAM						//是否使用RAM加载向量表
//#define use_cpp_algo						//在N皇后问题中采取C++的算法

/*------------------------------------------enum型选值配置-----------------------------------------------*/
//是否启用错误报警机制
typedef enum {EW_Enable = 1, EW_Disable = !EW_Enable} 					Error_Warning_Switch;
extern Error_Warning_Switch 		EW_Switch;
//是否启用打印调试
typedef enum {PD_Enable = 1, PD_Disable = !PD_Enable} 					Print_Debug_Switch;
extern Print_Debug_Switch 			PD_Switch;
//是否启用OLED显示屏
typedef enum {OLED_Enable = 1, OLED_Disable = !OLED_Enable} 			OLED_Use_Switch;
extern OLED_Use_Switch 				OLED_Switch;
//是否开启PVD电源监测
typedef enum {PVD_Enable = 1, PVD_Disable = !PVD_Enable} 				PVD_Check_Switch;
extern PVD_Check_Switch 			PVD_Switch;
//是否启用温度报警监测
typedef enum {Temp_Enable = 1, Temp_Disable = !Temp_Enable} 			Temperature_Warning_Switch;
extern Temperature_Warning_Switch 	Temp_Switch;
//是否启用协议通信机制
typedef enum {PC_Enable = 1, PC_Disable = !PC_Enable} 					Protocol_Com_Switch;
extern Protocol_Com_Switch 			PC_Switch;
//选择任务管理模式
typedef enum {RTOS = 0, Streak = 1}										TaskManage_Mode_Select;
extern TaskManage_Mode_Select 		TMMS;
//是否开启24V输入电压欠压报警
typedef enum {LVD_Enable = 1, LVD_Disable = !LVD_Enable}				Low_Voltage_Detect_Warning;
extern Low_Voltage_Detect_Warning	LVD_Switch;
//是否启用16进制报警
typedef enum {hexEW_Enable = 1, hexEW_Disable = !hexEW_Enable}			hex_ErrorWarning_Switch;
extern hex_ErrorWarning_Switch		hexEW_Switch;
//是否启用PID算法调速
typedef enum {pidDS_Enable = 1, pidDS_Disable = !pidDS_Enable}			pidDebugSpeed_Switch;
extern pidDebugSpeed_Switch			pidDS_Switch;
//是否开启任务资源切换查看
typedef enum {psaux_Enable = 1, psaux_Disable = !psaux_Enable}			psaux_CheckTaskRound;
extern psaux_CheckTaskRound			psaux_Switch;
//是否允许使用DataScope查看数据
typedef enum {DSD_Enable = 1, DSD_Disable = !DSD_Enable}				DataScope_DetectData;
extern DataScope_DetectData			DSD_Switch;		
//是否允许触发硬件错误后直接软件复位
typedef enum {HEDR_Enable = 1, HEDR_Disable = !HEDR_Enable} 			HardwareErrorDirectReset;
extern HardwareErrorDirectReset		HEDR_Switch;
//是否使模块OLED显示生效
typedef enum {MOE_Enable = 1, MOE_Disable = !MOE_Enable}				ModuleOLEDDisplay_Effect;
extern ModuleOLEDDisplay_Effect 	MOE_Switch;
//是否启用框架默认的灯效(包括呼吸灯和随机闪烁灯)
typedef enum {Light_Enable = 1, Light_Disable = !Light_Enable}			FrameDefaultLight_Effect;
extern FrameDefaultLight_Effect		Light_Switch;
//是否需要调整UI快速刷新显示(不稳定)
typedef enum {Quick_Ref = 0, Stable_Ref = !Quick_Ref}					UIRefresh_QuickMode;
extern UIRefresh_QuickMode			UIRef_ModeFlag;

//总体开关编号
typedef enum
{
	urc_map		= 0,					//打印URC配置的映射表
	urc_ew 		= 1,
	urc_pd 		= 2,
	urc_oled 	= 3,
	urc_pvd 	= 4,
	urc_temp 	= 5,
	urc_pc		= 6,
	urc_task	= 7,
	urc_lvd		= 8,
	urc_hex		= 9,
	urc_pid		= 10,
	urc_psaux	= 11,
	urc_dsd		= 12,
	urc_hedr	= 13,
	urc_moe		= 14,
	urc_light	= 15,
	urc_uifm	= 16,
}
Global_Switch_Nbr;

//设置框架内选项总数值
#define FrameDefault_urcOption_Count	16u			

//统一资源配置器
extern void Universal_Resource_Config (void);
void urcMapTable_Print (void);
extern void pclURC_DebugHandler (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
