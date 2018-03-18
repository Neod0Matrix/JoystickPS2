#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//user interface

#define PageAlterInterval			5000000		//切屏间隔5s

u8 oledScreenFlag = 2u;

//OLED睡眠状态显示
void OLED_SleepStaticDisplay (void)
{
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("System  Suspend"));
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("InterruptAwaken"));
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第零屏
void OLED_ScreenP0_Const (void)
{	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("EmbeddedBreaker"));
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("DevelopCoreKits"));
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第一屏
void OLED_ScreenP1_Const (void)
{	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, (" </MATRIX>@NdAn"));
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);
	//此处区分系统报警状态	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, 
		((Return_Error_Type == Error_Clear)? "Working Correct" : " Error Warning "));
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第二屏RTC时间
void OLED_Display_RTC (void)
{
	char* week;					
	
	//时间
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("    %02d:%02d:%02d    "), 
		*(rtcWholeData + 4), *(rtcWholeData + 5), *(rtcWholeData + 6));
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);
	
	//年月日，当前星期							
	switch (*(rtcWholeData + 3))
	{
	case 0: week = "Sun"; break;
	case 1: week = "Mon"; break;
	case 2: week = "Tue"; break;
	case 3: week = "Wed"; break;
	case 4: week = "Thu"; break;
	case 5: week = "Fri"; break;
	case 6: week = "Sat"; break;
	}
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("%4d/%02d/%02d  %s"), 
		*(rtcWholeData + 0), *(rtcWholeData + 1), *(rtcWholeData + 2), week);
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);
	OLED_Refresh_Gram();
}

//OLED常量第三屏系统状态
void OLED_StatusDetector (void)
{
	//显示OS CPU占用率、报警情况 
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("CPU %3d EW %02d"), RTOSCPUUsage / 100, Return_Error_Type);
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);
	
	//显示mem占用率、内部温度
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("MEM %3d IT %.1f"), mem_perused(), globalMCU_Temperature);
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);
	
	OLED_Refresh_Gram();
}

//初始化logo&第一屏
void OLED_DisplayInitConst (void)
{
	if (OLED_Switch == OLED_Enable)
	{
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		(MOE_Switch == MOE_Disable)? 
			OLED_ScreenP0_Const():OLED_ScreenModules_Const();
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
		OLED_ScreenP1_Const();	
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
	}		
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
			oledFlushEnable = (oledFlushEnable)? False:True;
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
			if (oledFreshcnt == 
				TickDivsIntervalus(PageAlterInterval) - 1 || KEY0_NLTrigger)
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
			delay_ms(20);					//等待刷屏完毕重新显示
		}
		//更新显示
		switch (pageUpdate)
		{
		case 0: 
			/*
				@EmbeddedBreakerCore Extern API Insert
			*/
			(MOE_Switch == MOE_Disable)? 
				OLED_ScreenP0_Const():OLED_ScreenModules_Const();
			break;
		case 1: 
			OLED_ScreenP1_Const();
			break;
		case 2: 
			OLED_Display_RTC();
			break;
		case 3:
			OLED_StatusDetector();	
			break;	
		}
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		if (MOE_Switch == MOE_Enable)
			OLED_DisplayModules(pageUpdate);
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
