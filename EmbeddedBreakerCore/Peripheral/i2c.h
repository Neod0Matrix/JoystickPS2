#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//软件I2C库

//I2C高低信号
typedef enum {i2c_hsignal = 1, i2c_lsignal = !i2c_hsignal} i2c_pullHL_Signal;
//I2C读写控制
typedef enum {I2C_WR = 0u, I2C_RD = 1u} I2C_RW_Status;
//SDA读写切换
typedef enum {SDA_Ws = 0u, SDA_Rs = !SDA_Ws} i2c_SDA_RW_Switcher;
#define Seti2cSDA_Write	i2c_SDA_RWSwitch(SDA_Ws)
#define Seti2cSDA_Read	i2c_SDA_RWSwitch(SDA_Rs)

void i2c_SDA_RWSwitch (i2c_SDA_RW_Switcher rw_sym);		//SDA读写切换
extern void i2c_Delay (void);							//I2C短暂延时
Bool_ClassType i2c_Start (void);						//I2C起始信号
void i2c_Stop (void);									//I2C停止信号
void i2c_SendByte (uint8_t _ucByte);					//I2C发送一个字节
uint8_t i2c_ReadByte (uint8_t ack);						//I2C读一个字节
Bool_ClassType i2c_WaitAck (void);						//I2C等待响应
void i2c_Ack (void);									//I2C响应
void i2c_NAck (void);									//I2C无响应
void i2c_GPIO_Config (void);							//I2C IO配置
Bool_ClassType i2c_CheckDevice (uint8_t _Address);		//I2C确认设备是否存在

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
