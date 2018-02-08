#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//软件I2C库

//定义I2C总线连接的GPIO端口
#define GPIO_PORT_I2C	GPIOB											//GPIO端口
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB							//GPIO端口时钟
#define GY_I2C_SCL		GPIO_Pin_10										//SCL
#define GY_I2C_SDA		GPIO_Pin_11										//SDA
#define GYSCL_IO		PBout(10)
#define GYSDA_IO		PBout(11)
//将陀螺仪总线配置到I2C上
#define SCL_PIN			GY_I2C_SCL										//scl pin
#define SDA_PIN			GY_I2C_SDA										//sda pin
#define SCL_IO			GYSCL_IO										//scl io
#define SDA_IO			GYSDA_IO										//sda io
//定义读写SCL和SDA
#define SCL_H		  	GPIO_PORT_I2C -> BSRR = SCL_PIN					//SCL = 1
#define SCL_L		  	GPIO_PORT_I2C -> BRR = SCL_PIN					//SCL = 0
#define SDA_H		  	GPIO_PORT_I2C -> BSRR = SDA_PIN					//SDA = 1
#define SDA_L		  	GPIO_PORT_I2C -> BRR = SDA_PIN					//SDA = 0
#define SDA_Rd			((GPIO_PORT_I2C -> IDR & SDA_PIN) != 0)			//读SDA口线状态

//SDA同时需要读和写
void i2c_SDA_RWSwitch (i2c_SDA_RW_Switcher rw_sym)
{
	if (rw_sym == SDA_Ws)
	{
		ucGPIO_Config_Init (RCC_I2C_PORT,			
							GPIO_Mode_Out_OD,							//数据传输开漏			
							GPIO_Speed_50MHz,						
							GPIORemapSettingNULL,			
							SDA_PIN,					
							GPIO_PORT_I2C,					
							NI,				
							EBO_Disable);
	}
	else
	{
		ucGPIO_Config_Init (RCC_I2C_PORT,			
							GPIO_Mode_IN_FLOATING,						//数据浮空输入
							GPIO_Speed_50MHz,						
							GPIORemapSettingNULL,			
							SDA_PIN,					
							GPIO_PORT_I2C,					
							NI,				
							EBO_Disable);
	}
}

//I2C总线位延迟，最快400KHz
void i2c_Delay (void)
{
	/*　
		CPU主频72MHz时:
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	volatile uint8_t i;													//使用volatile字，阻止编译优化
	for (i = 0; i < 10u; i++);
}

//CPU发起I2C总线启动信号
Bool_ClassType i2c_Start (void)
{
	//当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
	Seti2cSDA_Write;														//设置SDA为写
	SDA_H;
	if (!SDA_Rd) return False;
	SCL_H;
	i2c_Delay();
	SDA_L;
	if (!SDA_Rd) return False;
	i2c_Delay();
	SCL_L;
	
	return True;
}

//CPU发起I2C总线停止信号
void i2c_Stop (void)
{
	//当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
	Seti2cSDA_Write;
//	SDA_L;
//	SCL_H;
//	i2c_Delay();
//	SDA_H;
//	i2c_Delay();
	
	SCL_L;
	SDA_L;
	i2c_Delay();
	SCL_H;
	SDA_H;
	i2c_Delay();
}

//CPU产生一个时钟，并读取器件的ACK应答信号
Bool_ClassType i2c_WaitAck (void)
{
	Bool_ClassType re = False;
	
	Seti2cSDA_Read;														//设置SDA为读
	SDA_H;																//CPU释放SDA总线
	i2c_Delay();
	SCL_H;																//CPU驱动SCL = 1, 此时器件会返回ACK应答
	i2c_Delay();
	//CPU读取SDA口线状态
	if (SDA_Rd)	
		re = True;
	else
		re = False;
	SCL_L;
	i2c_Delay();
	
	return re;
}

//CPU产生一个ACK信号
void i2c_Ack (void)
{
	SCL_L;
	Seti2cSDA_Write;
	SDA_L;																//CPU驱动SDA = 0
	i2c_Delay();
	SCL_H;																//CPU产生1个时钟
	i2c_Delay();
	SCL_L;
	i2c_Delay();
	SDA_H;																//CPU释放SDA总线
}

//CPU产生1个NACK信号
void i2c_NAck (void)
{
	SCL_L;
	Seti2cSDA_Write;
	SDA_H;																//CPU驱动SDA = 1 
	i2c_Delay();
	SCL_H;																//CPU产生1个时钟 
	i2c_Delay();
	SCL_L;
}

//CPU向I2C总线设备发送8bit数据
void i2c_SendByte (uint8_t _ucByte)
{
	uint8_t i;

	Seti2cSDA_Write;
	SCL_L;
	for (i = 0; i < 8; i++)												//先发送字节的高位bit7
	{		
		if (_ucByte & 0x80) 
			SDA_H;
		else 
			SDA_L;
		i2c_Delay();
		SCL_H;
		i2c_Delay();	
		SCL_L;
		//释放总线
		if (i == 7u) SDA_H; 											
		_ucByte <<= 1;													//左移一个bit
		i2c_Delay();
	}
}

//CPU从I2C总线设备读取8bit数据
uint8_t i2c_ReadByte (u8 ack)
{
	uint8_t i, value = 0u;

	Seti2cSDA_Read;		
	//读到第1个bit为数据的bit7
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SCL_H;
		i2c_Delay();
		
		if (SDA_Rd) 
			value++;
		
		SCL_L;
		i2c_Delay();
	}
	
	if (ack == 0) 
		i2c_NAck();
	else 
		i2c_Ack();
	
	return value;
}

//配置I2C总线的GPIO，采用模拟IO的方式实现
void i2c_GPIO_Config (void)
{
	ucGPIO_Config_Init (RCC_I2C_PORT,			
						GPIO_Mode_Out_OD,								//数据传输开漏			
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						SCL_PIN | SDA_PIN,					
						GPIO_PORT_I2C,					
						NI,				
						EBO_Disable);
	i2c_Stop();															//给一个停止信号, 复位I2C总线上的所有设备到待机模式 
}

/*
	检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
	传参：设备的I2C总线地址
*/
Bool_ClassType i2c_CheckDevice (uint8_t _Address)
{
	Bool_ClassType ucAck;
	
	i2c_GPIO_Config();													//配置GPIO 
	i2c_Start();														//发送启动信号

	//发送设备地址+读写控制bit(0 = w，1 = r)bit7先传
	i2c_SendByte(_Address | I2C_WR);
	ucAck = i2c_WaitAck();												//检测设备的ACK应答 
	i2c_Stop();															//发送停止信号 

	return ucAck;
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
