#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//对系统内设/外设资源进行开关配置

//资源配置器enum型声明
Error_Warning_Switch 		EW_Switch;					//是否启用错误报警机制
Print_Debug_Switch 			PD_Switch;					//是否启用打印调试
OLED_Use_Switch 			OLED_Switch;				//是否启用OLED
PVD_Check_Switch 			PVD_Switch;					//是否启用PVD电源电压低额监测
Temperature_Warning_Switch 	Temp_Switch;				//是否启用内部温度监测报警
Protocol_Com_Switch 		PC_Switch;					//是否启用协议通信机制
TaskManage_Mode_Select 		TMMS;						//选择任务管理模式
Low_Voltage_Detect_Warning	LVD_Switch;					//是否开启24V欠压报警
hex_ErrorWarning_Switch		hexEW_Switch;				//是否开启16进制报警
pidDebugSpeed_Switch		pidDS_Switch;				//是否启用PID算法调速
psaux_CheckTaskRound		psaux_Switch;				//是否开启任务资源切换查看
DataScope_DetectData		DSD_Switch;					//是否允许使用DataScope查看数据
HardwareErrorDirectReset	HEDR_Switch;				//是否允许触发硬件错误后直接软件复位
ModuleOLEDDisplay_Effect 	MOE_Switch;					//是否使模块OLED显示生效
FrameDefaultLight_Effect	Light_Switch;				//是否启用框架默认的灯效(包括呼吸灯和随机闪烁灯)
UIRefresh_QuickMode			UIRef_ModeFlag;				//是否启用UI快速刷新模式

/*	
	统一资源配置，开机读取
	一堆具有深刻哲♂学♀意义的调试开关
	一下分为三大类：功能类，测试类，开关类
	以下选项为无配置状态下的缺省值
	开源接口给shell进行配置
*/
void Universal_Resource_Config (void)
{
#ifdef Frame_PreConfig									//定义框架预配置
	
	/*|选项|				|已选值|					|备选值|*/

/*$PAGE*/
/*->> 功能类*/											
    
	/*
		报警机制是重要的重复性测试指标
		自带系统预警和相关上下位机信息交互错误类型	
		无论进行什么测试都请务必开启，它会代替SB的编译器给你报错
	*/
    EW_Switch 			= EW_Enable;					//EW_Enable			EW_Disable
	
	/*
		在第三层封装库运动控制调用层中加入PID算法
		在保证其他功能正常前建议失能
		使用PID反馈需要判断精度是否达到PID的需求
	*/
	pidDS_Switch		= pidDS_Disable;				//pidDS_Enable		pidDS_Disable
	
/*$PAGE*/
/*->> 测试类*/
	
	/*
		STM32自带的电源管理监测
		触发它实际是在调用外部中断
		如果它挂了，那么外部中断可能也挂了
		那你就得留神了
	*/
    PVD_Switch 			= PVD_Enable;					//PVD_Enable		PVD_Disable
	
	/*
		STM32内部温度管理器
		虽然精确度不高，但还是很有必要的预警
	*/
    Temp_Switch 		= Temp_Enable;					//Temp_Enable		Temp_Disable
	
	/*
		使用工业现场用电24V供电时才开启	
		调试时建议关闭
		5V或者3.3V供电关闭
	*/
	LVD_Switch			= LVD_Disable;					//LVD_Enable		LVD_Disable
	
	/*
		如果该框架用于工控设备
		则需要设置为使能，使错误发生后直接复位
	*/
	HEDR_Switch			= HEDR_Disable;					//HEDR_Enable		HEDR_Disable
	
	/*
		框架自带娱乐使用的LED特效
		包括随机灯和呼吸灯，如果外设模块需要LED指示
		需要先关闭这些功能
	*/
	Light_Switch		= Light_Enable;					//Light_Enable		Light_Disable
	
	/*
		某些情况下需要UI(0.91 OLED)进行快速刷新显示来提高显示效果
		简单来说就是取决于OLED刷新函数的算法执行效率和调用频率
		最快速的方法当然在中断函数中更新，但中断函数中若包含延时函数
		则会引起意想不到的神奇现象，故不建议
	*/
	UIRef_ModeFlag		= Quick_Ref;					//Quick_Ref			Stable_Ref

/*$PAGE*/
/*->> 开关类*/
	
	/*
		人性化调试日志反馈机制
		与机器视觉进行坐标调试时为了避免干扰协议必须关闭
	*/
    PD_Switch 			= PD_Enable;					//PD_Enable			PD_Disable
	
	/*
		使用协议格式与上位机进行通信，主要应用于报警
		针对于机器视觉数据传输而设计
	*/
	hexEW_Switch 		= hexEW_Disable;				//hexEW_Enable		hexEW_Disable
	
	/*
		程序状态机直观窗口
	*/
    OLED_Switch 		= OLED_Enable;					//OLED_Enable		OLED_Disable

    /*
		工程封印大法
		如果不开启MCU几乎不受外界控制
	*/
    PC_Switch			= PC_Enable;					//PC_Enable			PC_Disable

    /*
		本工程属于可剪裁嵌入式项目，可以选择是否使用μCOS
	*/
	TMMS				= Streak;						//RTOS				Streak
	
	/*
		ps -aux
		查看任务切换，调试任务管理器使用
	*/
	psaux_Switch		= psaux_Disable;				//psaux_Enable		psaux_Disable
	
	/*
		DataScope串口虚拟示波器
		功能炫酷，但是和串口助手有冲突
		装逼专用
	*/
	DSD_Switch			= DSD_Disable;					//DSD_Enable		DSD_Disable
	
	/*
		对框架而言，不显示模块的OLED部分
		对应用的模块而言，不显示框架的常量字符
		且需要使自己本身的显示生效
		框架设置为失能，模块设置为使能
	*/
	MOE_Switch			= MOE_Enable;					//MOE_Enable		MOE_Disable
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	Modules_UniResConfig();
	
#endif													//end of Frame_PreConfig flag
}

//打印URC映射表
void urcMapTable_Print (void)
{
	U1SD("Universal Resource Config Map Table");
	//长了吧唧映射表
	U1SD("\r\n%02d	Print URC Map Table", urc_map);
	U1SD("\r\n%02d	Error Warning", urc_ew);
	U1SD("\r\n%02d 	Print Debug", urc_pd);
	U1SD("\r\n%02d 	OLED", urc_oled);
	U1SD("\r\n%02d 	PVD", urc_pvd);
	U1SD("\r\n%02d 	Temperature", urc_temp);
	U1SD("\r\n%02d 	Protocol COM Interface", urc_pc);
	U1SD("\r\n%02d 	TaskManager", urc_task);
	U1SD("\r\n%02d 	LowVoltage Detector", urc_lvd);
	U1SD("\r\n%02d 	Hex Print Debug", urc_hex);
	U1SD("\r\n%02d 	PID", urc_pid);
	U1SD("\r\n%02d 	ps -aux", urc_psaux);
	U1SD("\r\n%02d 	DataScope Detect Data Curve", urc_dsd);
	U1SD("\r\n%02d 	Hardware Error Direct Reset", urc_hedr);
	U1SD("\r\n%02d 	Module OLED Display Effect", urc_moe);
	U1SD("\r\n%02d 	Frame Default Light Effect", urc_light);
	U1SD("\r\n%02d 	Module UI Refresh Mode", urc_uifm);
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	Modules_URCMap();
	U1SD("\r\n");
}

//基于协议控制的URC调试处理
void pclURC_DebugHandler (void)
{
	//取开关编号和状态位数据
	Global_Switch_Nbr sw_type = (Global_Switch_Nbr)(*(USART1_RX_BUF + URC_SW_1Bit) * 10u 
												+ 	*(USART1_RX_BUF + URC_SW_2Bit));
	u8 ed_status = *(USART1_RX_BUF + URC_ED_Bit);
	
	//接收数据处理
	__ShellHeadSymbol__;
	if (sw_type <= Max_Option_Value)		
	{
		//打印映射表则不需要打印这段
		if (sw_type > 0)								
		{
			U1SD("Setting Switch Number: [%d] Set to Status: [%d]\r\n", sw_type, ed_status);
		}
		
		//编号选取
		switch (sw_type)
		{
		case urc_map:		urcMapTable_Print();										break;
		case urc_ew: 		EW_Switch 		= (Error_Warning_Switch)ed_status;	 		break;
		case urc_pd: 		PD_Switch 		= (Print_Debug_Switch)ed_status; 			break;
		case urc_oled: 		OLED_Switch 	= (OLED_Use_Switch)ed_status; 				break;
		case urc_pvd: 		PVD_Switch 		= (PVD_Check_Switch)ed_status; 				break;
		case urc_temp: 		Temp_Switch 	= (Temperature_Warning_Switch)ed_status; 	break;
		case urc_pc: 		PC_Switch		= (Protocol_Com_Switch)ed_status;			break;	
		case urc_task: 		TMMS			= (TaskManage_Mode_Select)ed_status;		break;	
		case urc_lvd: 		LVD_Switch		= (Low_Voltage_Detect_Warning)ed_status;	break;	
		case urc_hex: 		hexEW_Switch	= (hex_ErrorWarning_Switch)ed_status;		break;
		case urc_pid: 		pidDS_Switch	= (pidDebugSpeed_Switch)ed_status;			break;
		case urc_psaux: 	psaux_Switch	= (psaux_CheckTaskRound)ed_status;			break;
		case urc_dsd:		DSD_Switch		= (DataScope_DetectData)ed_status;			break;
		case urc_hedr:		HEDR_Switch		= (HardwareErrorDirectReset)ed_status;		break;
		case urc_moe:		MOE_Switch		= (ModuleOLEDDisplay_Effect)ed_status;		break;
		case urc_light:		Light_Switch	= (FrameDefaultLight_Effect)ed_status;		break;
		case urc_uifm:		UIRef_ModeFlag	= (UIRefresh_QuickMode)ed_status;			break;
		}
		
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		Modules_urcDebugHandler(ed_status, (Modules_SwitchNbr)sw_type);
	
		__ShellHeadSymbol__; U1SD("URC Setting Update\r\n");//URC配置更新
	}
	else if (sw_type > Max_Option_Value)				//选项超值报错
	{
		U1SD("Option Value Excess Error\r\n");			
		SERIALDATAERROR;		
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
