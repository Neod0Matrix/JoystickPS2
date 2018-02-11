#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//user interface

#define PageAlterInterval			5000000		//切屏间隔5s

u8 oledScreenFlag = 2u;

//-----------------------------------常量处理-------------------------------------->

//OLED睡眠状态显示
void OLED_SleepStaticDisplay (void)
{
	OLED_ShowString(strPos(0u), ROW1, (const u8*)"System  Suspend", Font_Size);	
	OLED_ShowString(strPos(0u), ROW2, (const u8*)"InterruptAwaken", Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第零屏
void OLED_ScreenP0_Const (void)
{	
	OLED_ShowString(strPos(0u), ROW1, (const u8*)"EmbeddedBreaker", Font_Size);	
	OLED_ShowString(strPos(0u), ROW2, (const u8*)"DevelopCoreKits", Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第一屏
void OLED_ScreenP1_Const (void)
{	
	OLED_ShowString(strPos(1u), ROW1, (const u8*)"</MATRIX>@NdAn", Font_Size);	
	OLED_ShowString(strPos(1u), ROW2, (const u8*)"System Running", Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第二屏
void OLED_ScreenP2_Const (void)
{
	char* week;					
	
	RTC_DataStorage(calendar);
	//时间
	OLED_ShowTime(strPos(4u), ROW1, rtcWholeData[4], Font_Size);
	OLED_ShowString(strPos(6u), ROW1, (const u8*)":", Font_Size);
	OLED_ShowTime(strPos(7u), ROW1, rtcWholeData[5], Font_Size);
	OLED_ShowString(strPos(9u), ROW1, (const u8*)":", Font_Size);
	OLED_ShowTime(strPos(10u), ROW1, rtcWholeData[6], Font_Size);
	
	//年月日
	OLED_ShowNum(strPos(0u), ROW2, rtcWholeData[0], 4u, Font_Size);	
	OLED_ShowString(strPos(4u), ROW2, (const u8*)"/", Font_Size);
	OLED_ShowTime(strPos(5u), ROW2, rtcWholeData[1], Font_Size);
	OLED_ShowString(strPos(7u), ROW2, (const u8*)"/", Font_Size);
	OLED_ShowTime(strPos(8u), ROW2, rtcWholeData[2], Font_Size);
	
	//当前星期							
	switch (rtcWholeData[3])
	{
	case 0: week = "Sun"; break;
	case 1: week = "Mon"; break;
	case 2: week = "Tue"; break;
	case 3: week = "Wed"; break;
	case 4: week = "Thu"; break;
	case 5: week = "Fri"; break;
	case 6: week = "Sat"; break;
	}
	OLED_ShowString(strPos(12u), ROW2, (const u8*)week, Font_Size);
	OLED_Refresh_Gram();
}

//OLED常量第三屏
void OLED_ScreenP3_Const (void)
{
	//显示OS CPU 占用率
	OLED_ShowString(strPos(0u), ROW1, (const u8*)"CPU", Font_Size);							
	OLED_ShowNum(strPos(3u), ROW1, RTOSCPUUsage / 100, 3u, Font_Size); 
	//OLED_ShowString(strPos(6u), ROW1, (const u8*)"%", Font_Size);		
	
	if (EW_Switch == EW_Enable)
	{
		//显示error_warning
		OLED_ShowString(strPos(7u), ROW1, (const u8*)"EW", Font_Size);					
		OLED_ShowNum(strPos(10u), ROW1, Return_Error_Type, 2u, Font_Size);
	}		
	
	//显示mem占用率
	OLED_ShowString(strPos(0u), ROW2, (const u8*)"MEM", Font_Size);							
	OLED_ShowNum(strPos(3u), ROW2, mem_perused(), 3u, Font_Size);
	//OLED_ShowString(strPos(6u), ROW2, (const u8*)"%", Font_Size);	
	
	if (Temp_Switch == Temp_Enable)
	{
		//显示inner_temperature_detect
		//OLED显示位数有限，仅精确到小数点后一位
		OLED_ShowString(strPos(7u), ROW2, (const u8*)"ITD", Font_Size);							
		OLED_ShowNum(strPos(10u), ROW2, globalMCU_Temperature, 2u, Font_Size);//十位和个位
		OLED_ShowString(strPos(12u), ROW2, (const u8*)".", Font_Size);//显示.
		OLED_ShowNum(strPos(13u), ROW2, ((u32)(globalMCU_Temperature * 10) % 10), 1u, Font_Size);//十分位
		OLED_ShowNum(strPos(14u), ROW2, ((u32)(globalMCU_Temperature * 100) % 10), 1u, Font_Size);//百分位
	}
	OLED_Refresh_Gram();
}

//初始化logo&第一屏
void OLED_DisplayInitConst (void)
{
	if (OLED_Switch == OLED_Enable)
	{
		if (MOE_Switch == MOE_Disable)
			OLED_ScreenP0_Const();	
		else
			/*
				@EmbeddedBreakerCore Extern API Insert
			*/
			OLED_ScreenModules_Const();
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
		OLED_ScreenP1_Const();	
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
	}		
}

//-----------------------------------变量处理-------------------------------------->

//显示RTC时间
void OLED_Display_RTC (void)
{
	char* week;
			
	//时间
	OLED_ShowTime(strPos(4u), ROW1, rtcWholeData[4], Font_Size);
	OLED_ShowTime(strPos(7u), ROW1, rtcWholeData[5], Font_Size);
	OLED_ShowTime(strPos(10u), ROW1, rtcWholeData[6], Font_Size);
	
	//年月日
	OLED_ShowNum(strPos(0u), ROW2, rtcWholeData[0], 4u, Font_Size);	
	OLED_ShowTime(strPos(5u), ROW2, rtcWholeData[1], Font_Size);
	OLED_ShowTime(strPos(8u), ROW2, rtcWholeData[2], Font_Size);
	
	//当前星期							
	switch (rtcWholeData[3])
	{
	case 0: week = "Sun"; break;
	case 1: week = "Mon"; break;
	case 2: week = "Tue"; break;
	case 3: week = "Wed"; break;
	case 4: week = "Thu"; break;
	case 5: week = "Fri"; break;
	case 6: week = "Sat"; break;
	}
	OLED_ShowString(strPos(12u), ROW2, (const u8*)week, Font_Size);
	OLED_Refresh_Gram();
}

//显示错误报警类型(仅在错误变更时调用)
void OLED_Display_EW (void)
{
	if (EW_Switch == EW_Enable)
	{		
		OLED_ShowNum(strPos(10u), ROW1, detErrorStatus(), 2u, Font_Size);
		OLED_Refresh_Gram();				//更新显示
	}
}

//显示内部温度检测(仅在温度变化时载入进程)
void OLED_Display_ITD (void)
{
	static float displayTemperature = 0.f;	//显示温度
	static u8 tempInt = 0u;					//温度整数位
	static u8 tempFlaot1, tempFlaot2 = 0u;	//温度小数位
	
	if (displayTemperature != globalMCU_Temperature)
	{
		displayTemperature = globalMCU_Temperature;//检测到温度变化才更新
		tempInt = displayTemperature;		//十位、个位
		tempFlaot1 = ((u32)(displayTemperature * 10)) % 10;//十分位
		tempFlaot2 = ((u32)(displayTemperature * 100)) % 10;//百分位
		
		if (Temp_Switch == Temp_Enable)			
		{
			//OLED显示位数有限，仅精确到小数点后一位
			OLED_ShowNum(strPos(10u), ROW2, tempInt, 2u, Font_Size);				
			OLED_ShowNum(strPos(13u), ROW2, tempFlaot1, 1u, Font_Size);	
			OLED_ShowNum(strPos(14u), ROW2, tempFlaot2, 1u, Font_Size);				
			OLED_Refresh_Gram();	
		}
	}
}

//显示OS CPU占用率(仅在变化时载入进程)
void OLED_Display_CPU (void)
{
	static u8 cpuUsagePercent = 0u;
	
	if (cpuUsagePercent != RTOSCPUUsage / 100)//值变化后才更新
	{
		cpuUsagePercent = RTOSCPUUsage / 100;
		if (TMMS == RTOS)
		{					
			OLED_ShowNum(strPos(3u), ROW1, cpuUsagePercent, 3u, Font_Size); 		
			OLED_Refresh_Gram();			//更新显示
		}
	}
}

//OLED显示内存占用率(仅在变化时载入进程)
void OLED_Display_Mem (void)
{
	static u8 memUsagePercent = 0u;
	
	if (memUsagePercent != MemUsagePercent)	//值变化后才更新
	{
		memUsagePercent = MemUsagePercent;
		
		OLED_ShowNum(strPos(3u), ROW2, memUsagePercent, 3u, Font_Size); 		
		OLED_Refresh_Gram();				//更新显示
	}
}

//OLED系统状态显示监视
void OLED_StatusDetector (void)
{
	OLED_Display_CPU(); 
	OLED_Display_EW(); 
	OLED_Display_Mem(); 
	OLED_Display_ITD(); 	
}

//切屏控制
void OLED_PageAlterCtrl (void)
{
	static unsigned long oledFreshcnt = 0u;
	static Bool_ClassType oledFlushEnable = True;
	
	if (pwsf != JBoot && globalSleepflag == SysOrdWork)						
	{
		//自动切屏使能控制
		if (KEY1_NLTrigger)
		{
			//反转使能
			if (oledFlushEnable)				
				oledFlushEnable = False;	
			else 					
				oledFlushEnable = True;
			oledFreshcnt = 0u;				//扩展时间复位
			
			while(KEY1_NLTrigger);			//等待按键释放
		}
		
		//手动切屏使能控制
		if (KEY0_NLTrigger)					
		{
			oledScreenFlag++;				//切屏
			if (oledScreenFlag == ScreenPageCount)					
				oledScreenFlag = 0u;		//现有屏数复位
			
			while(KEY0_NLTrigger);			//等待按键释放
		}
		
		//时间扩展5s自动切屏/按键KEY0手动切屏
		if (oledFlushEnable)
		{
			if (oledFreshcnt == TickDivsIntervalus(PageAlterInterval) - 1 || KEY0_NLTrigger)
			{
				oledFreshcnt = 0u;			//扩展时间复位
				
				oledScreenFlag++;			//切屏
				if (oledScreenFlag == ScreenPageCount)					
					oledScreenFlag = 0u;	//现有屏数复位
				
				while(KEY0_NLTrigger);		//等待按键释放
			}
			oledFreshcnt++;					//扩展时间累加
		}
	}
}

//UI显示器
void UIScreen_DisplayHandler (void)
{	
	static u8 pageUpdate = 2u;				//默认第二屏开始
	
	if (OLED_Switch == OLED_Enable)
	{
		//擦除原先的画面，KEY1按下停止切屏
		if (pageUpdate != oledScreenFlag)
		{				
			pageUpdate = oledScreenFlag;
			OLED_Clear();		
		}
		
		//更新显示
		switch (pageUpdate)
		{
		case 0: 
			if (MOE_Switch == MOE_Disable)
				OLED_ScreenP0_Const();	
			else
				/*
					@EmbeddedBreakerCore Extern API Insert
				*/
				OLED_ScreenModules_Const();
			break;
		case 1: 
			OLED_ScreenP1_Const();
			break;
		case 2: 
			OLED_ScreenP2_Const();
			OLED_Display_RTC();
			break;
		case 3:
			OLED_ScreenP3_Const();
			OLED_StatusDetector();	
			break;	
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		case 4:
			if (MOE_Switch == MOE_Enable)
				OLED_DisplayModules();
			break;
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
