#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PS2手柄驱动

//全局键值缓存
u8 KeyValueCache[9] = {0x00}; 		
u16 AnologSumValue = 0;
StickKeyValueMap globalPS2keyValue = ps2none;

//发送命令
static void PS2_SendCommand (u8 cmd)
{
    volatile u16 ref;
	
    *(KeyValueCache + 1) = 0;
    for (ref = 0x01; ref < 0x0100; ref <<= 1)
    {
        if (ref & cmd)	IO_Cmd_H;                   
        else 			IO_Cmd_L;
		
        IO_Clk_H, delay_us(10);
        IO_Clk_L, delay_us(10);
        IO_Clk_H;
        if (IO_DataIn) 
			*(KeyValueCache + 1) |= ref;
    }
	delay_us(16);
}

//判断是否为红灯模式
Bool_ClassType PS2_RedLightMode (void)
{
    IO_CS_L;
    PS2_SendCommand(0x01);  				//开始命令
    PS2_SendCommand(0x42);  				//请求数据
    IO_CS_H;

	return (*(KeyValueCache + 1) == 0x73)? False : True;
}

//读取手柄数据
static void PS2_ReadStickData (void)
{
    volatile u8 byte;
    volatile u16 ref;

    IO_CS_L;
    PS2_SendCommand(0x01);  				//开始命令
    PS2_SendCommand(0x42);  				//请求数据

	//开始接受数据
    for (byte = 2; byte < 9; byte++)        
    {
        for (ref = 0x01; ref < 0x100; ref <<= 1)
        {
            IO_Clk_H, delay_us(10);
            IO_Clk_L, delay_us(10);
            IO_Clk_H;
            if (IO_DataIn) 
				*(KeyValueCache + byte) |= ref;
        }
        delay_us(16);
    }
    IO_CS_H;
}

//short ppll
static void PS2_ShortPoll (void)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x42);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	IO_CS_H;
	delay_us(16);
}

//进入配置
static void PS2_EnterConfigMode (void)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x43);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	IO_CS_H;
	delay_us(16);
}

//发送模式设置
static void PS2_TurnOnAnalogMode (void)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x44);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0xee);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	IO_CS_H;
	delay_us(16);
}

//震动设置
static void PS2_VibrationMode (void)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x4d);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x01);
	IO_CS_H;
	delay_us(16);
}

//完成并保存设置
static void PS2_ExitConfigMode (void)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x43);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x5a);
	PS2_SendCommand(0x5a);
	PS2_SendCommand(0x5a);
	PS2_SendCommand(0x5a);
	PS2_SendCommand(0x5a);
	IO_CS_H;
	delay_us(16);
}

/*
	m1 右侧电机，0x00关，其他开，
	m2左侧电机，0x44-0xff开，值越大震动越大
*/
static void PS2_VibrationMotor (u8 m1, u8 m2)
{
	IO_CS_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x42);
	PS2_SendCommand(0x00);
	PS2_SendCommand(m1);
	PS2_SendCommand(m2);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	IO_CS_H;
	delay_us(16);
	//震动延时
	if (m1 || m2)
		delay_ms(300);
}

//配置初始化
static void PS2_ModeConfigInit (void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfigMode();
	PS2_TurnOnAnalogMode();
	PS2_VibrationMode();
	PS2_ExitConfigMode();
}

//手柄初始化  
void PS2_InterfaceInit (void)
{	
	//DI->PB14
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOB,			
						GPIO_Mode_IPD,			//默认下拉
						GPIO_Input_Speed,						
						GPIORemapSettingNULL,			
						GPIO_Pin_14,					
						GPIOB,					
						NI,				
						EBO_Disable);

	//DO->PA1 CS->PA4 CLK->PA5
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5,					
						GPIOA,					
						IHL,				
						EBO_Enable);
	
	PS2_ModeConfigInit();						
}

/*
	对读出来的PS2的数据进行处理
	默认数据是红灯模式
*/
StickKeyValueMap PS2_MatchStickKeyValue (void)
{
    u8 index;
	u16 temp;
	//键值列表
	static StickKeyValueMap kvm[16] = {
		ps2select, ps2l3, ps2r3, ps2start, ps2padup,
		ps2padright, ps2paddown, ps2padleft, ps2l2,
		ps2r2, ps2l1, ps2r1, ps2triangle, ps2circle,
		ps2cross, ps2square,
	};

	//清除键值缓存
    for (index = 0; index < 9; index++)
        *(KeyValueCache + index) = 0x00;
    PS2_ReadStickData();
	//计算摇杆模拟量总和
	AnologSumValue = *(KeyValueCache + ps2lx) + *(KeyValueCache + ps2ly)
		+ *(KeyValueCache + ps2rx) + *(KeyValueCache + ps2ry);
	//临时高低位
	temp = (*(KeyValueCache + 4) << 8) | *(KeyValueCache + 3);   
	//16个按键，按下为0，未按下为1
    for (index = 0; index < 16; index++)
    {
        if (!(temp & (1 << (*(kvm + index) - 1))))
            return (StickKeyValueMap)(index + 1);
    }
	
    return ps2none;         
}

//OLED显示PS2键值和摇杆状态
void OLED_DisplayPS2 (void)
{
	//显示键值
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("KeyValueMap: %2d"), globalPS2keyValue);
	OLED_ShowString(strPos(0u), ROW1, (const u8*)oled_dtbuf, Font_Size);
	//显示摇杆模拟值
	snprintf((char*)oled_dtbuf, OneRowMaxWord, ("%03d %03d %03d %03d"), 
		*(KeyValueCache + ps2lx), *(KeyValueCache + ps2ly), *(KeyValueCache + ps2rx), *(KeyValueCache + ps2ry));
	OLED_ShowString(strPos(0u), ROW2, (const u8*)oled_dtbuf, Font_Size);
	OLED_Refresh_Gram();
}

//手柄指令响应
void PS2_JoyStickResponseHandler (void)
{
	//静态更新
	static StickKeyValueMap localkv = ps2none;
	static u16 anologSum = 0;
	static Bool_ClassType stickMatchReceive = False;
	
	globalPS2keyValue = PS2_MatchStickKeyValue();						//全局传参
	//匹配键值变化
	if (localkv != globalPS2keyValue)									//此处键值会自动复位
	{
		localkv = globalPS2keyValue;
		if (localkv)													//复位到ps2none不打印
		{
			if (SendDataCondition && PS2P_Switch == PS2P_Enable)
			{
				//这里打印的字符越短越节省时间，用户体验会更流畅
				printf("\r\n[JoystickPS2] KeyValue: %d\r\n", localkv);
				usart1WaitForDataTransfer();
			}
			Beep_Once;													//蜂鸣器触发，放到后面体验效果会好一点
		}
		//指向PS2键码显示屏
		if (MOE_Switch == MOE_Enable && ui_oled.ui_confirm_alter == 4)	
			OLED_DisplayPS2();
		
		//键值任务响应(这是一个Demo)
		switch (localkv)
		{
			case ps2none: 		break;
			case ps2select: 	break;									//悬空状态键值为1，不要在这里添加危险代码
			case ps2l3: 		break;
			case ps2r3:			break;
			case ps2start:		break;
			case ps2padup:		break;
			case ps2padright:	break;
			case ps2paddown:	break;
			case ps2padleft:	break;
			case ps2l2: 		VibrateLeftMotor; 				break;	//电机驱动
			case ps2r2: 		VibrateRightMotor; 				break;	//电机驱动
			case ps2l1:			Sys_Soft_Reset(); 				break;	//软重启
			case ps2r1:			nQueen_CalculusHandler();		break;	//8皇后
			case ps2triangle:	CommunicationTest(); 			break;	//通信测试
			case ps2circle:		displaySystemInfo(); 			break;	//系统信息显示
			case ps2cross:		ERROR_CLEAR;					break;	//清除错误报警
			case ps2square:		ManualCtrlEW();					break;	//强制报警
		}
	}
	
	//匹配摇杆模拟量变化
	if (anologSum != AnologSumValue)									//摇杆由于机械弹簧左右会自动复位
	{
		//模拟量变化较快需要实时匹配，不适合加入蜂鸣器
		anologSum = AnologSumValue;
		if (SendDataCondition && PS2P_Switch == PS2P_Enable)
		{
			//这里打印的字符越短越节省时间，用户体验会更流畅
			printf("\r\n[JoystickPS2] JoyAnologValue(0~255): %5d %5d %5d %5d\r\n", 
				*(KeyValueCache + ps2lx), *(KeyValueCache + ps2ly),
				*(KeyValueCache + ps2rx), *(KeyValueCache + ps2ry));
			usart1WaitForDataTransfer();
		}
		if (MOE_Switch == MOE_Enable && ui_oled.ui_confirm_alter == 4)	
			OLED_DisplayPS2();
		/*
			红灯模式配对响应，当接收机和手柄完成配对，rx=lx=128，ry=ly=127，和为510
			丢失响应，当接收机和手柄失去连接，rx=ry=lx=ly=128，和为512
			上电进入绿灯模式，rx=ry=lx=ly=255，不予理会
		*/
		if ((anologSum == 510 && !stickMatchReceive) 
			|| (anologSum == 512 && stickMatchReceive))
		{
			stickMatchReceive = (Bool_ClassType)!stickMatchReceive;
			Beep_Once;												
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
