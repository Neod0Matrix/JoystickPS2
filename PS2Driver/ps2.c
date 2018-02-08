#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PS2手柄驱动

u16 Handkey;
u8 Comd[2] = {0x01, 0x42};									//开始命令，请求数据
u8 Data[9] = {0}; 				//数据存储数组

//键值数组
StickKeyValueMap kvm[16] = {
	ps2select 	,
	ps2l3		,
	ps2r3		,
	ps2start	,
	ps2padup	,
	ps2padright ,
	ps2paddown  ,
	ps2padleft  ,
	ps2l2		,
	ps2r2		,
	ps2l1		,
	ps2r1		,
	ps2green	,
	ps2red		,
	ps2blue		,
	ps2pink		,	
};

//手柄初始化  
void PS2_Init (void)
{
	//DI->PB12
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOB,			
						GPIO_Mode_IPD,			//默认下拉
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_12,					
						GPIOB,					
						NI,				
						EBO_Enable);

	//DO->PB1 
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOB,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_1,					
						GPIOB,					
						IHL,				
						EBO_Enable);
	
	//CS->PC6 CLK->PC7
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOC,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_6 | GPIO_Pin_7,					
						GPIOC,					
						IHL,				
						EBO_Enable);
	
	PS2_SetInit();					//配置初始化
}

//向手柄发送命令
void PS2_Cmd (u8 CMD)
{
    volatile u16 ref = 0x01;
	
    Data[1] = 0;
    for(ref=0x01; ref<0x0100; ref<<=1)
    {
        if(ref&CMD)
        {
            DO_H;                   //输出以为控制位
        }
        else DO_L;

        CLK_H;                        //时钟拉高
        delay_us(50);
        CLK_L;
        delay_us(50);
        CLK_H;
        if(DI)
            Data[1] = ref|Data[1];
    }
}

//判断是否为红灯模式
//返回值；0，红灯模式
//		  其他，其他模式
u8 PS2_RedLight(void)
{
    CS_L;
    PS2_Cmd(Comd[0]);  //开始命令
    PS2_Cmd(Comd[1]);  //请求数据
    CS_H;
    if( Data[1] == 0X73)   return 0 ;
    else return 1;

}

//读取手柄数据
void PS2_ReadData(void)
{
    volatile u8 byte=0;
    volatile u16 ref=0x01;

    CS_L;

    PS2_Cmd(Comd[0]);  //开始命令
    PS2_Cmd(Comd[1]);  //请求数据

    for(byte=2; byte<9; byte++)        //开始接受数据
    {
        for(ref=0x01; ref<0x100; ref<<=1)
        {
            CLK_H;
            CLK_L;
            delay_us(50);
            CLK_H;
            if(DI)
                Data[byte] = ref|Data[byte];
        }
        delay_us(50);
    }
    CS_H;
}

//对读出来的PS2的数据进行处理      只处理了按键部分         默认数据是红灯模式  只有一个按键按下时
//按下为0， 未按下为1
u8 PS2_DataKey()
{
    u8 index;

    PS2_ClearData();
    PS2_ReadData();

    Handkey=(Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
    for(index=0; index<16; index++)
    {
        if((Handkey&(1<<(kvm[index]-1)))==0)
            return index+1;
    }
    return 0;          //没有任何按键按下
}

//得到一个摇杆的模拟量	 范围0~256
u8 PS2_AnologData (StickKeyValueMap button)
{
    return Data[button];
}

//清除数据缓冲区
void PS2_ClearData()
{
    u8 a;
    for(a=0; a<9; a++)
        Data[a]=0x00;
}

void PS2_Task (void)
{
	u8 key;
	key=PS2_DataKey();
		if(key!=0)                   //有按键按下
    	{
			printf("  \r\n   %d  is  pressed  \r\n",key);
			if (key == 11)
			{
				PS2_Vibration(0xff, 0x00);
				delay_ms(1500);
				
			}
			else if (key == 12)
			{
				PS2_Vibration(0x00, 0x41);
				delay_ms(1500);
			}
			else
				PS2_Vibration(0x00, 0x00);
    	}
		printf(" %5d %5d %5d %5d\r\n",PS2_AnologData(ps2lx),PS2_AnologData(ps2ly),
		                              PS2_AnologData(ps2rx),PS2_AnologData(ps2ry));
		delay_ms(50);
}

//short ppll
void PS2_ShortPoll (void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

//进入配置
void PS2_EnterConfig (void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

//发送模式设置
void PS2_TurnOnAnalogMode (void)
{
	CS_L;
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0xee);
	
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

//震动设置
void PS2_VibrationMode (void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x4d);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	CS_H;
	delay_us(16);
}

//完成并保存设置
void PS2_ExitConfig (void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	CS_H;
	delay_us(16);
}

//配置初始化
void PS2_SetInit (void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfig();
	PS2_TurnOnAnalogMode();
	PS2_VibrationMode();
	PS2_ExitConfig();
}

//m1 右侧电机，0x00关，其他开，m2左侧电机，0x44-0xff开，值越大震动越大
void PS2_Vibration (u8 m1, u8 m2)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(m1);
	PS2_Cmd(m2);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}



//====================================================================================================
//code by </MATRIX>@Neod Anderjon
