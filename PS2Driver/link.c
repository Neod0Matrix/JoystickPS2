#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	模块JoystickPS2对框架EmbeddBreakerCore的链接
	该文件写入对框架的函数调用支持
*/

#define PS2TimerInterval	50000

Stew_EXTI_Setting			StewEXTI_Switch;
PS2_PrintKeyValue 			PS2P_Switch;

//链接到Universal_Resource_Config函数的模块库
void ModulePS2_UniResConfig (void)
{
    /*
    	急停状态判断复杂，不适合外部中断
    	但也有可能普通监测不够快
    */
    StewEXTI_Switch 	= StewEXTI_Enable;				//StewEXTI_Enable	StewEXTI_Disable
	PS2P_Switch 		= PS2P_Enable;					//PS2P_Enable		PS2P_Disable
}

//模块选项映射表，链接到urcMapTable_Print函数
void ModulePS2_URCMap (void)
{
    printf("\r\n%02d 	Stew EXTI Setting", urc_stew);
    usart1WaitForDataTransfer();
	printf("\r\n%02d 	PS2 Joystick Print", urc_ps2p);
    usart1WaitForDataTransfer();
}

//选项处理，链接到pclURC_DebugHandler函数
void ModulePS2_urcDebugHandler (u8 ed_status, PS2_SwitchNbr sw_type)
{
    switch (sw_type)
    {
    case urc_stew:	StewEXTI_Switch	= (Stew_EXTI_Setting)ed_status; break;
	case urc_ps2p:	PS2P_Switch		= (PS2_PrintKeyValue)ed_status; break;
    }
}

//串口接收数据示例，不调用
void U1RSD_example (void)
{
    u8 t, len;

    if (PD_Switch == PD_Enable && Data_Receive_Over)	//接收数据标志
    {
        len = Data_All_Length;							//得到此次接收到的数据长度(字符串个数)
        __ShellHeadSymbol__;
        U1SD("Controller Get The Data: \r\n");
        if (No_Data_Receive)							//没有数据接收，可以发送
        {
            for (t = 0u; t < len; t++)
            {
                USART_SendData(USART1, USART1_RX_BUF[t]);//将所有数据依次发出
                usart1WaitForDataTransfer();			//等待发送结束
            }
        }
        U1SD("\r\n");									//插入换行
        USART1_RX_STA = 0u;								//接收状态标记
    }
}

//OLED常量第四屏，链接到OLED_DisplayInitConst和UIScreen_DisplayHandler函数
void OLED_ScreenP4_Const (void)
{
    OLED_ShowString(strPos(0u), ROW1, (const u8*)" Joystick PS2  ", Font_Size);
    OLED_ShowString(strPos(0u), ROW2, (const u8*)"SonyPlaystation", Font_Size);
    OLED_Refresh_Gram();
}

//OLED JoystickPS2数据显示
void OLED_DisplayPS2 (void)
{
	//显示键值
	OLED_ShowString(strPos(0u), ROW1, (const u8*)"KeyValueMap: ", Font_Size);
	OLED_ShowNum(strPos(13u), ROW1, globalPS2keyValue, 2u, Font_Size);
	//显示摇杆模拟值
	OLED_ShowNum(strPos(0u), ROW2, KeyValueCache[ps2lx], 3u, Font_Size);
	OLED_ShowNum(strPos(4u), ROW2, KeyValueCache[ps2ly], 3u, Font_Size);
	OLED_ShowNum(strPos(8u), ROW2, KeyValueCache[ps2rx], 3u, Font_Size);
	OLED_ShowNum(strPos(12u), ROW2, KeyValueCache[ps2ry], 3u, Font_Size);
    OLED_Refresh_Gram();
}

//PS2手柄扫描实时任务
void PS2_MatchStickMapTask (void)
{
	static u16 ps2ScanSem = 0u;
	
	if (Return_Error_Type == Error_Clear && pwsf != JBoot) 
	{
		if (ps2ScanSem++ == TickDivsIntervalus(PS2TimerInterval) - 1)
		{
			ps2ScanSem = 0u;
			PS2_StickTestDisplay();			
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
