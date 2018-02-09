#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PS2手柄驱动

//全局键值缓存
u8 KeyValueCache[9] = {0}; 		
u16 AnologSumValue = 0;
StickKeyValueMap globalPS2keyValue = ps2none;

//发送命令
void PS2_SendCommand (u8 cmd)
{
    volatile u16 ref = 0x01;
	
    KeyValueCache[1] = 0;
    for (ref = 0x01; ref < 0x0100; ref <<= 1)
    {
		//输出以为控制位
        if (ref & cmd)	IO_Cmd_H;                   
        else 			IO_Cmd_L;
		
        IO_Clk_H, delay_us(10);
        IO_Clk_L, delay_us(10);
        IO_Clk_H;
        if (IO_DataIn) 
			KeyValueCache[1] |= ref;
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

	return (KeyValueCache[1] == 0x73)? False : True;
}

//读取手柄数据
void PS2_ReadStickData (void)
{
    volatile u8 byte = 0;
    volatile u16 ref = 0x01;

    IO_CS_L;
    PS2_SendCommand(0x01);  				//开始命令
    PS2_SendCommand(0x42);  				//请求数据

    for (byte = 2; byte < 9; byte++)        //开始接受数据
    {
        for (ref = 0x01; ref < 0x100; ref <<= 1)
        {
            IO_Clk_H, delay_us(10);
            IO_Clk_L, delay_us(10);
            IO_Clk_H;
            if (IO_DataIn) 
				KeyValueCache[byte] |= ref;
        }
        delay_us(16);
    }
    IO_CS_H;
}

/*
	对读出来的PS2的数据进行处理
	只处理了按键部分，默认数据是红灯模式
*/
StickKeyValueMap PS2_MatchStickKeyValue (void)
{
    u8 index;
	u16 handkey;
	//键值列表
	static StickKeyValueMap kvm[16] = {
		ps2select, ps2l3, ps2r3, ps2start, ps2padup,
		ps2padright, ps2paddown, ps2padleft, ps2l2,
		ps2r2, ps2l1, ps2r1, ps2green, ps2red,
		ps2blue, ps2pink		
	};

    for (index = 0; index < 9; index++)
        KeyValueCache[index] = 0x00;
    PS2_ReadStickData();

	//这是16个按键，按下为0，未按下为1
    handkey = (KeyValueCache[4] << 8) | KeyValueCache[3];   
	//计算摇杆模拟量总和
	AnologSumValue = KeyValueCache[ps2lx] + KeyValueCache[ps2ly] 
		+ KeyValueCache[ps2rx] + KeyValueCache[ps2ry];
    for (index = 0; index < 16; index++)
    {
        if (!(handkey & (1 << (kvm[index] - 1))))
            return (StickKeyValueMap)(index + 1);
    }
	
    return ps2none;         
}

//short ppll
void PS2_ShortPoll (void)
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
void PS2_EnterConfigMode (void)
{
	IO_Clk_L;
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
	IO_Clk_H;
	delay_us(16);
}

//发送模式设置
void PS2_TurnOnAnalogMode (void)
{
	IO_Clk_L;
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
	IO_Clk_H;
	delay_us(16);
}

//震动设置
void PS2_VibrationMode (void)
{
	IO_Clk_L;
	delay_us(16);
	PS2_SendCommand(0x01);
	PS2_SendCommand(0x4d);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x00);
	PS2_SendCommand(0x01);
	IO_Clk_H;
	delay_us(16);
}

//完成并保存设置
void PS2_ExitConfigMode (void)
{
	IO_Clk_L;
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
	IO_Clk_H;
	delay_us(16);
}

//m1 右侧电机，0x00关，其他开，m2左侧电机，0x44-0xff开，值越大震动越大
void PS2_VibrationMotor (u8 m1, u8 m2)
{
	IO_Clk_L;
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
	IO_Clk_H;
	delay_us(16);
}

//配置初始化
void PS2_ModeConfigInit (void)
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

//测试用显示键值
void PS2_StickTestDisplay (void)
{
	//静态更新
	static StickKeyValueMap localkv = ps2none;
	static u16 anologSum = 0;
	
	//全局传参
	globalPS2keyValue = PS2_MatchStickKeyValue();
	
	//电机驱动
	if (globalPS2keyValue == ps2l1)
		PS2_VibrationMotor(0xff, 0x00);
	else if (globalPS2keyValue == ps2r1)
		PS2_VibrationMotor(0x00, 0x41);
	else
		PS2_VibrationMotor(0x00, 0x00);
	//打印测试
	if (No_Data_Receive && PC_Switch == PC_Enable && PS2P_Switch == PS2P_Enable)
	{
		if (localkv != globalPS2keyValue)
		{
			localkv = globalPS2keyValue;
			printf("\r\nKey Value Map: %d\r\n", localkv);
			usart1WaitForDataTransfer();
			if (oledScreenFlag == 4)		//指向PS2键码显示屏
				OLED_DisplayPS2();
		}
		if (anologSum != AnologSumValue)
		{
			anologSum = AnologSumValue;
			printf("\r\nAnolog Data: %5d %5d %5d %5d\r\n", 
				KeyValueCache[ps2lx], KeyValueCache[ps2ly],
				KeyValueCache[ps2rx], KeyValueCache[ps2ry]);
			usart1WaitForDataTransfer();
			if (oledScreenFlag == 4)		//指向PS2键码显示屏
				OLED_DisplayPS2();
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
