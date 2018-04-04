#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//user interface

#define PageAlterInterval			5000000		//切屏间隔5s

UI_ScreenAlterCtrl ui_oled;

//结构体初始化
void UISAC_StructureInit (void)
{
	ui_oled.ui_screen_nbr 		= 2u;
	ui_oled.ui_confirm_alter 	= 2u;
	ui_oled.ui_fresh_cnt		= 0u;
	ui_oled.ui_alter_flag		= True;
}

//OLED睡眠状态显示
void OLED_SleepStaticDisplay (void)
{
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("System  Suspend"));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("InterruptAwaken"));
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第零屏
void OLED_ScreenP0_Const (void)
{	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("EmbeddedBreaker"));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("DevelopCoreKits"));
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第一屏
void OLED_ScreenP1_Const (void)
{	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("    T.WKVER    "));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);
	//此处区分系统报警状态	
	snprintf((char*)oled_dtbuf, OneRowMaxWord, 
		((Return_Error_Type == Error_Clear)? "Working Correct":" Error Warning "));
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第二屏
void OLED_ScreenP2_Const (void)
{	
	//logo: </MATRIX>@Neod Anderjon
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("   </MATRIX>   "));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);
	snprintf((char*)oled_dtbuf, OneRowMaxWord, (" Neod Anderjon "));
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);	
	OLED_Refresh_Gram();
}

//OLED常量第三屏RTC时间
void OLED_Display_RTC (void)
{
	//时间
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("    %02d:%02d:%02d    "), 
		*(rtcTotalData + 4), *(rtcTotalData + 5), *(rtcTotalData + 6));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);
	//年月日，当前星期							
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("%4d/%02d/%02d  %s"), 
		*(rtcTotalData + 0), *(rtcTotalData + 1), *(rtcTotalData + 2), week_str[*(rtcTotalData + 3)]);
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);
	OLED_Refresh_Gram();
}

//OLED常量第四屏系统状态
void OLED_StatusDetector (void)
{
	//显示OS CPU占用率、报警情况 
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("CPU %3d EW %02d"), 
		RTOSCPUUsage / 100, Return_Error_Type);
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);
	//显示mem占用率、内部温度
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("MEM %3d IT %.1f"), 
		mem_perused(), globalMCU_Temperature);
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);
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
		//替换框架预设置logo
		(MOE_Switch == MOE_Disable)? 
			OLED_ScreenP0_Const():OLED_ScreenModules_Const();
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
		OLED_ScreenP1_Const();	
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
		OLED_ScreenP2_Const();
		delay_ms(300);						//logo延迟
		OLED_Clear();						//擦除原先的画面
		
		UISAC_StructureInit();				//初始化切屏结构体
	}		
}

//切屏控制
void OLED_PageAlterCtrl (void)
{
	//初始化完成后非睡眠状态才能进行UI控制
	if (pwsf != JBoot && globalSleepflag == SysOrdWork)						
	{
		//自动切屏使能控制
		if (KEY1_NLTrigger)
		{
			ui_oled.ui_alter_flag = (ui_oled.ui_alter_flag)? False:True;
			ui_oled.ui_fresh_cnt = 0u;		
			while(KEY1_NLTrigger);			
		}
		//手动切屏使能控制
		if (KEY0_NLTrigger)					
		{
			if (++ui_oled.ui_screen_nbr == ScreenPageCount)					
				ui_oled.ui_screen_nbr = 0u;	
			while(KEY0_NLTrigger);			
		}
		//时间扩展5s自动切屏
		if (ui_oled.ui_fresh_cnt++ == 
			TickDivsIntervalus(PageAlterInterval) - 1 && ui_oled.ui_alter_flag)
		{
			ui_oled.ui_fresh_cnt = 0u;	
			if (++ui_oled.ui_screen_nbr == ScreenPageCount)					
				ui_oled.ui_screen_nbr = 0u;	
		}
	}
}

//UI显示器
void UIScreen_DisplayHandler (void)
{	
	if (OLED_Switch == OLED_Enable)
	{
		//擦除原先的画面，KEY1按下停止切屏
		if (ui_oled.ui_confirm_alter != ui_oled.ui_screen_nbr)
		{				
			ui_oled.ui_confirm_alter = ui_oled.ui_screen_nbr;
			OLED_Clear();						
		}
		switch (ui_oled.ui_confirm_alter)
		{
		case 0: 
			/*
				@EmbeddedBreakerCore Extern API Insert
			*/
			//替换框架预设置logo
			(MOE_Switch == MOE_Disable)? 
				OLED_ScreenP0_Const():OLED_ScreenModules_Const(); break;
		case 1: OLED_ScreenP1_Const(); 	break;
		case 2: OLED_ScreenP2_Const(); 	break;
		case 3: OLED_Display_RTC(); 	break;
		case 4: OLED_StatusDetector();	break;	
		}
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		if (MOE_Switch == MOE_Enable)
			OLED_DisplayModules(ui_oled.ui_confirm_alter);	
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
