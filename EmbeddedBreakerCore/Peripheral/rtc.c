#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	RTC实时时钟 驱动代码
	STM32自带的RTC时钟并不准确，而且诟病繁多，建议使用外部时钟做RTC
	必须保证外部低速晶振32.768khz处于正常状态，否则RTC就不正常
	如果32.768khz晶振异常，上电初始化RTC初始化时会占用很长时间
*/

_calendar_obj calendar, _start;

//月份数据表
const u8 table_week[12] = {0u, 3u, 3u, 6u, 1u, 4u, 6u, 2u, 5u, 0u, 3u, 5u};
//平年的月份日期表
const u8 mon_table[12] = {31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u};
//全局缓存数组
u16 rtcWholeData[7];

//RTC中断初始化
static void RTC_NVIC_Config (void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;			//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能该通道中断
    NVIC_Init(&NVIC_InitStructure);									//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//时间初值设定
void RTC_TimeInitSetting (void)
{
	//初设定，RTC寄存器断电更新
	_start.w_year 	= 2018u;										//年
	_start.w_month 	= 1u;											//月	
	_start.w_date 	= 27u;											//日
	_start.hour 	= 18u;											//时
	_start.min 		= 33u;											//分
	_start.sec 		= 34u;											//秒
	
	//设置时间
	RTC_Set(	_start.w_year, 
				_start.w_month, 
				_start.w_date, 
				_start.hour, 
				_start.min, 
				_start.sec
				);  										
}

/*
	实时时钟配置
	初始化RTC时钟,同时检测时钟是否工作正常
	BKP->DR1用于保存是否第一次配置的设置
	如果外部低速晶振有问题，RTC初始化将占用很长一段时间，这时建议更换晶振测试
*/
Bool_ClassType RTC_Init (void)
{
    u8 temp = 0u;													//检查是不是第一次配置时钟
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);									//使能后备寄存器访问
	
	LED0_On;														//红灯异常预警
	
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)					//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    {	
        BKP_DeInit();												//复位备份区域
        RCC_LSEConfig(RCC_LSE_ON);									//设置外部低速晶振(LSE),使用外设低速晶振
		
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            delay_ms(10);											//等待初始化检测
        }
        if (temp >= 250) 											//2500ms初始化时钟失败，晶振有问题
			return True;																			

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//设置RTC时钟(RTCCLK)，选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE);										//使能RTC时钟
        RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
        RTC_WaitForSynchro();										//等待RTC寄存器同步
        RTC_ITConfig(RTC_IT_SEC, ENABLE);							//使能RTC秒中断
        RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
        RTC_EnterConfigMode();										//允许配置
        RTC_SetPrescaler(Ext_Lowspeed_psc); 						//设置RTC预分频的值
        RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
		
        RTC_TimeInitSetting();										//初设时间
		
        RTC_ExitConfigMode();		 								//退出配置模式
        BKP_WriteBackupRegister(BKP_DR1, 0X5050);					//向指定的后备寄存器中写入用户程序数据
    }
	
    //系统继续计时
    else
    {
		//以下两步包含了两个等待，可能会占用较长时间
        RTC_WaitForSynchro();										//等待最近一次对RTC寄存器的写操作完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);							//使能RTC秒中断
        RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
    }
	
	LED0_Off;														//关闭预警
    RTC_NVIC_Config();												//RCT中断分组设置
    RTC_Get();														//更新时间
	
    return False;													//初始化成功										
}

/*
	RTC时钟中断
	每秒触发一次
*/
void RTC_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 												//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
    OSIntEnter();
#endif

    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
	{
		RTC_Get();													//秒钟中断，更新时间
		RTC_DataStorage(calendar);									//RTC时间寄存
	}

    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)						//闹钟中断
        RTC_ClearITPendingBit(RTC_IT_ALR);							//清闹钟中断

    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);					//清闹钟中断
    RTC_WaitForLastTask();

#if SYSTEM_SUPPORT_OS 												//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
    OSIntExit();
#endif
}

/*
	判断是否是闰年函数
	月份   1  2  3  4  5  6  7  8  9  10 11 12
	闰年   31 29 31 30 31 30 31 31 30 31 30 31
	非闰年 31 28 31 30 31 30 31 31 30 31 30 31
	输入:年份
	输出:该年份是不是闰年.1,是.0,不是
*/
Bool_ClassType Is_Leap_Year (u16 year)
{
    if (year % 4u == 0u) 											//必须能被4整除
    {
        if (year % 100u == 0u)
        {
            if (year % 400u == 0u) 
				return True;										//如果以00结尾，还要能被400整除
            else 
				return False;
        }
        else 
			return True;
    }
    else 
		return False;
}

/*
	设置时钟
	把输入的时钟转换为秒钟
	时间基数1970年1月1日
	1970~2099年为合法年份
	返回值:0,成功;其他:错误代码
*/
Bool_ClassType RTC_Set (u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0u;
	
    if (syear < 1970u || syear > 2099u) 
		return True;
    for (t = 1970u; t < syear; t++)									//把所有年份的秒钟相加
		seccount += (Is_Leap_Year(t) == True)? 31622400u:31536000u;	//闰平年秒钟数转换
    smon -= 1u;

    for (t = 0u; t < smon; t++)	 									//把前面月份的秒钟数相加
    {
        seccount += (u32)mon_table[t] * 86400u;						//月份秒钟数相加
        if (Is_Leap_Year(syear) && t == 1) 
			seccount += 86400u; 									//闰年2月份增加一天的秒钟数
    }

    seccount += (u32)(sday - 1u) * 86400u;							//把前面日期的秒钟数相加
    seccount += (u32)hour * 3600u;									//小时秒钟数
    seccount += (u32)min * 60u;	 									//分钟秒钟数
    seccount += sec;												//最后的秒钟加上去

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);									//使能RTC和后备寄存器访问
    RTC_SetCounter(seccount);										//设置RTC计数器的值
    RTC_WaitForLastTask();											//等待最近一次对RTC寄存器的写操作完成
    RTC_Get();														//更新时间
	
    return False;
}

/*
	得到当前的时间
	返回值:0,成功;其他:错误代码
*/
Bool_ClassType RTC_Get (void)
{
    static u16 daycnt = 0u;
    u32 timecount = 0u;
    u32 temp = 0u;
    u16 temp1 = 0u;
    timecount = RTC_GetCounter();
	
    temp = timecount / 86400u;   									//得到天数(秒钟数对应的)
    if (daycnt != temp)												//超过一天了
    {
        daycnt = temp;
        temp1 = 1970u;												//从1970年开始
        while (temp >= 365u)
        {
            if (Is_Leap_Year(temp1))								//是闰年
            {
                if (temp >= 366u) 
					temp -= 366u;									//闰年的秒钟数
                else
                {
                    temp1++;
                    break;
                }
            }
            else 
				temp -= 365u;	  									//平年
            temp1++;
        }

        calendar.w_year = temp1;									//得到年份
        temp1 = 0u;

        while (temp >= 28u)											//超过了一个月
        {
            if (Is_Leap_Year(calendar.w_year) && temp1 == 1u)		//当年是不是闰年/2月份
            {
                if (temp >= 29u) 
					temp -= 29u;									
                else 
					break;
            }
            else
            {
                if (temp >= mon_table[temp1]) 
					temp -= mon_table[temp1];
                else 
					break;
            }
            temp1++;
        }
        calendar.w_month = temp1 + 1u;								//得到月份
        calendar.w_date = temp + 1u;  								//得到日期
    }

    temp = timecount % 86400u;							     		//得到秒钟数
    calendar.hour = temp / 3600u;							     	//小时
    calendar.min = (temp % 3600u) / 60u; 							//分钟
    calendar.sec = (temp % 3600u) % 60u; 							//秒钟
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date);//获取星期
	
    return False;
}

/*
	获得现在是星期几
	功能描述:输入公历日期得到星期(只允许1901-2099年)
	输入参数：公历年月日
	返回值：星期号
*/
u8 RTC_Get_Week (u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100u;
    yearL = year % 100u;

    //如果为21世纪,年份数加100
    if (yearH > 19u) yearL += 100u;

    //所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4u;
    temp2 %= 7u;
    temp2 += (day + table_week[month - 1u]);
    if (yearL % 4u == 0u && month < 3u) temp2--;
	
    return (temp2 % 7u);
}

//RTC初始化检查
void RTC_Init_Check (void)
{
	//RTC初始化，一定要初始化成功，失败原因：晶振及时钟电路有问题
    while (RTC_Init())												
    {
		//如果第一次检测通过一下则不显示
		__ShellHeadSymbol__; U1SD("RTC Error\r\n");
        delay_ms(800);
        __ShellHeadSymbol__; U1SD("RTC Retrying\r\n");
    }
}

//RTC全局数据寄存
void RTC_DataStorage (_calendar_obj rtc)
{
	static vu8 temp_sec;
	
	//时间更新
	if (temp_sec != rtc.sec)
	{
		temp_sec = rtc.sec;	
		
		rtcWholeData[0] = rtc.w_year;
		rtcWholeData[1] = rtc.w_month;
		rtcWholeData[2] = rtc.w_date;
		rtcWholeData[3] = rtc.week;
		rtcWholeData[4] = rtc.hour;
		rtcWholeData[5] = rtc.min;
		rtcWholeData[6] = rtc.sec;
	}
}

//RTC指令请求处理
void RTC_ReqOrderHandler (void)
{
	char* week;
	
	__ShellHeadSymbol__;	
	if (SendDataCondition)
	{
		//显示年月日
		printf("System Built-in RTC Clock Log: %04d/%02d/%02d ", rtcWholeData[0], rtcWholeData[1], rtcWholeData[2]);
		usart1WaitForDataTransfer();
	
		//显示当前时间
		printf("%02d:%02d:%02d ", rtcWholeData[4], rtcWholeData[5], rtcWholeData[6]);
		usart1WaitForDataTransfer();
		
		//显示当前星期							
		switch (rtcWholeData[3])
		{
		case 0: week = "Sun"; break;
		case 1: week = "Mon"; break;
		case 2: week = "Tue"; break;
		case 3: week = "Wed"; break;
		case 4: week = "Thu"; break;
		case 5: week = "Fri"; break;
		case 6: week = "Sat"; break;
		}
		printf("%s\r\n", week);
		usart1WaitForDataTransfer();
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
