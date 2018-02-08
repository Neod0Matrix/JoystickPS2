#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//GPIO初始化库

//GPIO通用初始化配置函数
void ucGPIO_Config_Init (	uint32_t 			rcc_axbx_gpiox,		//RCC时钟总线
							GPIOMode_TypeDef 	mode,				//IO模式
							GPIOSpeed_TypeDef 	speed,				//IO速度
							uint32_t			remap,				//重映射
							uint16_t 			pin,				//PIN脚
							GPIO_TypeDef* 		gpiox,				//GPIO总线
							Init_LevelStatus	ils_flag,			//初始化高低电平
							ExternBoardOutput	ebo_flag)			//是否外接输出到板子上
{
	//基本配置
    GPIO_InitTypeDef GPIO_InitStructure;

	//RCC时钟总线处理
    RCC_APB2PeriphClockCmd(rcc_axbx_gpiox, ENABLE);					//使能GPIO口RCC时钟
	if (mode == GPIO_Mode_AF_OD || mode == GPIO_Mode_AF_PP)			//复用
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	if (remap != TIMx_GPIO_Remap_NULL)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//重映射需要使能AFIO总线
		GPIO_PinRemapConfig(remap, ENABLE);							//重映射IO通道(重映射必须在RCC初始化后使用才有效)
	}
	
	//输出模式
	if (	mode == GPIO_Mode_Out_OD 								//开漏(数据输出)
		|| 	mode == GPIO_Mode_Out_PP 								//推挽(普通输出)
		|| 	mode == GPIO_Mode_AF_OD 								//复用开漏(挂载的特殊通道数据输出)
		|| 	mode == GPIO_Mode_AF_PP)								//复用推挽(挂载的特殊通道普通输出)
	{
		GPIO_InitStructure.GPIO_Mode = mode; 		 				//输出模式(关于定时器PWM和OC通道的宏编译配置写在该函数之外)
		GPIO_InitStructure.GPIO_Speed = speed;		 				//IO口速度
		GPIO_InitStructure.GPIO_Pin = pin;							//pin脚
		GPIO_Init(gpiox, &GPIO_InitStructure);					 	//根据设定参数初始化相应端口
		
		//IO口状态初始化
		if (ils_flag == IHL && ebo_flag == EBO_Enable)				//起始配置为高并带ULN2003A反相设置
		{
#ifdef use_ULN2003A
			GPIO_ResetBits(gpiox, pin);										
#else
			GPIO_SetBits(gpiox, pin);		
#endif
		}
		else if (ils_flag == ILL && ebo_flag == EBO_Enable)			//起始配置为低并带ULN2003A反相设置
		{
#ifdef use_ULN2003A
			GPIO_SetBits(gpiox, pin);										
#else
			GPIO_ResetBits(gpiox, pin);		
#endif
		}
		else if (ils_flag == IHL && ebo_flag == EBO_Disable)		//起始配置为高不带ULN2003A反相设置
			GPIO_SetBits(gpiox, pin);		
		else if (ils_flag == ILL && ebo_flag == EBO_Disable)		//起始配置为低不带ULN2003A反相设置
			GPIO_ResetBits(gpiox, pin);	
		else if (ils_flag == NI)									//不配置高低状态
		{
		}
	}	
	
	//输入模式
	else if (	mode == GPIO_Mode_AIN 								//模拟输入(ADC)
			|| 	mode == GPIO_Mode_IN_FLOATING 						//浮空输入(USART_RX)
			|| 	mode == GPIO_Mode_IPD 								//下拉输入(一般不用)
			|| 	mode == GPIO_Mode_IPU)								//上拉输入(常用普通输入)
	{
		GPIO_InitStructure.GPIO_Pin = pin;							//PIN脚				
		GPIO_InitStructure.GPIO_Mode = mode; 						//输入模式
		GPIO_Init(gpiox, &GPIO_InitStructure);						//根据设定参数初始化相应端口
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
