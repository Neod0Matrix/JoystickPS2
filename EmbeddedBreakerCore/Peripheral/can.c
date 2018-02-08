#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//CAN总线

//CAN初始化模式
u8 CAN_Mode_Init (u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
#if CAN_RX0_INT_ENABLE
    NVIC_InitTypeDef  NVIC_InitStructure;
#endif

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);				//使能CAN1时钟
	
	//开启CAN1端口重映射，将PA11 PA12换成PB8 PB9
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_Remap2_CAN1将从映射到PD0 PD1
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	//PB9 CAN1_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						//复用推挽
    GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化IO

	//PB8 CAN1_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;						//上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化IO

    //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE;								//非时间触发通信模式
    CAN_InitStructure.CAN_ABOM = DISABLE;						 		//软件自动离线管理	
    CAN_InitStructure.CAN_AWUM = DISABLE;						 		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;						 		//禁止报文自动传送
    CAN_InitStructure.CAN_RFLM = DISABLE;						 		//报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP = DISABLE;						 		//优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode = mode;	         						//模式设置： mode:0,普通模式;1,回环模式;
    //设置波特率
    CAN_InitStructure.CAN_SJW = tsjw;									//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1 = tbs1; 									//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2 = tbs2;									//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler = brp;					            //分频系数(Fdiv)为brp+1
    CAN_Init(CAN1, &CAN_InitStructure);            						//初始化CAN1

    CAN_FilterInitStructure.CAN_FilterNumber = 0;	  					//过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//32位
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;					//32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				//32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 				//激活过滤器0

    CAN_FilterInit(&CAN_FilterInitStructure);							//滤波器初始化
	
#if CAN_RX0_INT_ENABLE
    CAN_ITConfig(CAN1, CAN_IT_FMP0,ENABLE);								//FIFO0消息挂号中断允许
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;     	//主优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;            	//次优先级为4
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

    return 0;
}

#if CAN_RX0_INT_ENABLE													//使能RX0中断
//中断服务函数
void USB_LP_CAN1_RX0_IRQHandler (void)
{
    CanRxMsg RxMessage;
    int i;
    CAN_Receive(CAN1, 0, &RxMessage);
	if (PD_Switch == PD_Enable && No_Data_Receive)
	{
		for (i = 0; i < 8; i++)
		{
			printf("rxbuf[%d]: %d\r\n", i, RxMessage.Data[i]);
			while ((USART1 -> SR & 0X40) == 0u);
		}
	}
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)
//len:数据长度(最大为8)
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 Can_Send_Msg (u8* msg, u8 len)
{
    u8 mbox;
    u16 i;
    CanTxMsg TxMessage;
	if (CAN_Switch == CAN_Enable)
	{
		TxMessage.StdId = 0x12;					 						//标准标识符为0
		TxMessage.ExtId = 0x12;				 							//设置扩展标示符(29位)
		TxMessage.IDE = 0;			 									//使用扩展标识符
		TxMessage.RTR = 0;												//消息类型为数据帧，一帧8位
		TxMessage.DLC = len;							 				//发送两帧信息
		for (i = 0; i < len; i++)
			TxMessage.Data[i] = msg[i];				 					//第一帧信息
		mbox = CAN_Transmit(CAN1, &TxMessage);
		i = 0;
		while ((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
			i++;														//等待发送结束
		if (i >= 0XFFF)
			return 1;
	}
    return 0;
}

//can口接收数据查询
//buf:数据缓存区;
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 Can_Receive_Msg (u8 *buf)
{
    u32 i;
    CanRxMsg RxMessage;
	if (CAN_Switch == CAN_Enable)
	{
		if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
			return 0;													//没有接收到数据,直接退出
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);						//读取数据
		for (i = 0; i < 8; i++)
			buf[i] = RxMessage.Data[i];
	}
    return RxMessage.DLC;
}

//CAN应用
//需要结合两块板子和按键
void CAN1_Application (void)
{
	u8 i = 0;
	u8 canbuf[8];
	u8 mode = CAN_Mode_LoopBack;										//CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式
	if (CAN_Switch == CAN_Enable)
	{
		if (KEY_Scan(NotSupportLongPress) == KEY0_PRES)					//KEY0按下,发送一次数据
		{
			if (PD_Switch == PD_Enable && No_Data_Receive)
			{
				for (i = 0; i < 8; i++)
				{
					canbuf[i] = i;										//填充发送缓冲区
					printf("%d", canbuf[i]);							//显示数据
					while ((USART1 -> SR & 0X40) == 0);					//等待发送结束
				}	
			}				
			//发送8个字节 
			if (Can_Send_Msg(canbuf, 8)) 
			{	
				U1SD("Failed");											//提示发送失败	
			}
			else U1SD("OK");											//提示发送成功				
		}
		else if (KEY_Scan(NotSupportLongPress) == KEY1_PRES)			//KEY1按下，改变CAN的工作模式
		{	   
			mode = !mode;
			CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_7tq, 5, mode);//CAN普通模式初始化, 波特率450Kbps 
			if (mode == 0)												//普通模式，需要2个客户端
			{
				U1SD("CAN Turn to Normal Mode");
			}
			else 														//回环模式,一个开发板就可以测试了.
			{
				U1SD("CAN Turn to Loopback Mode");
			}	
		}		 
		if (Can_Receive_Msg(canbuf))									//接收到有数据
		{	
			if (PD_Switch == PD_Enable && No_Data_Receive)
			{
				for (i = 0; i < Can_Receive_Msg(canbuf); i++)
				{									    
					printf("%d", canbuf[i]);							//显示数据
					while ((USART1 -> SR & 0X40) == 0);					//等待发送结束
				}
			}
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
