#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DataScope下位机库

u8 DataScope_OutPut_Buffer[StableWidthSize] = {0};	   		//串口发送缓冲区

/*
	将单精度浮点数据转成4字节数据并存入指定地址
	传参：目标单精度数据，待写入数组，指定从数组第几个元素开始写入
*/
void Float2Byte (float *target, u8 *buf, u8 beg)
{
    u8 i, *point;
	
	point = (u8*)target;									//得到float的地址
	for (i = 0; i < 4; i++)
		*(buf + (beg + 1)) = *(point + i);
}

/*
	将待发送通道的单精度浮点数据写入发送缓冲区
	传参：通道数据，选择通道(1-10)
*/
void DataScope_Get_Channel_Data (float Data, u8 Channel)
{
    if ((Channel > 10) || (Channel == 0)) 
		return;  											//通道个数大于10或等于0，直接跳出，不执行函数
    else
		Float2Byte(&Data, DataScope_OutPut_Buffer, Channel * 4 - 3);
}

/*
	生成DataScope能正确识别的帧格式
	传参：需要发送的通道个数
	返回发送缓冲区数据个数
	返回0表示帧格式生成失败
*/
u8 DataScope_Data_Generate (u8 Channel_Number)
{
	u8 channel_offset;
	
    if ((Channel_Number > 10) || (Channel_Number == 0)) 
        return 0;    										//通道个数大于10或等于0，直接跳出，不执行函数
    else
    {
        DataScope_OutPut_Buffer[0] = '$';  					//帧头
		channel_offset = Channel_Number * 4 + 1;
		*(DataScope_OutPut_Buffer + channel_offset) = channel_offset;
		
		return channel_offset + 1;
	}
}

//DataScope内部构建发送栈
void DataScope_LocalBuild (float dat, u8 ch)
{
	u8 i;
	
	if (DSD_Switch == DSD_Enable)
	{
		DataScope_Get_Channel_Data(dat, ch); 				//将数据写入通道
		//生成10个通道的格式化帧数据，返回帧数据长度
		for(i = 0; i < DataScope_Data_Generate(10); i++) 
		{
			//将所有数据依次发出
			USART_SendData(USART1, *(DataScope_OutPut_Buffer + i));	
			usart1WaitForDataTransfer();					
		}								
	}
}

//简易测试(可放到PTFAI里测试)
void DataScope_Test (void)
{
	u16 i = 0u;
	
#define MaxRand 	200
#define MinRand		20
	while (i++ < 500u)
	{
		DataScope_LocalBuild(RangeRandom(MaxRand, MinRand), 1);
		DataScope_LocalBuild(RangeRandom(MaxRand, MinRand), 2);
		DataScope_LocalBuild(RangeRandom(MaxRand, MinRand), 3);
		DataScope_LocalBuild(RangeRandom(MaxRand, MinRand), 4);
		delay_ms(50);
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
