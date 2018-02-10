#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//串口ASCII字符数据处理

//变量转字符串
//传参：字符串存储数组/指针，变量
//返回：转换后的字符串指针
char* varChange_Str (char str_space[], int var)
{
	snprintf(str_space, snprintfStackSpace, "%d", var);	//ANSI C标准处理，C99可以使用itoa()
	return str_space;
}

//独立字符串拼接
char* indStr_Catenate (char *s1, char *s2)  
{  
#define stackSpace	10u									//安全起见，使用安全的C库
	//动态分配内存，同时警戒溢出
    char *ind_result = malloc(strlen(s1) + strlen(s2) + 1u);
    
    if (ind_result != NULL)								//确定指针没有越界
	{
		strncpy(ind_result, s1, stackSpace);  
		strncat(ind_result, s2, stackSpace); 
	}
	
    return ind_result;  
}  

/*
	将整形数据转换成字符串
	输入radix = 10 表示10进制，其他结果为0
	value 要转换的整形数
	radix = 10
 */
static char *itoa (int value, char *string, int radix)
{
    int i, d;
    int flag = 0;
    char *ptr = string;
	
    if (radix != 10)									//不为10进制
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    if (value < 0)
    {
        *ptr++ = '-';
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
    *ptr = 0;

    return string;
} 

//获取一个数的位数(以十进制为标准)
u16 Nbr10BitCalcus (int nbr)
{
	u16 bit = 0u;
	
	if (nbr > 0)
	{
		while (nbr)
		{
			bit++;
			nbr /= 10u;
		}
	}
	else if (nbr == 0)
	{
		bit = 1;										//0只有一位
	}
	else
	{
		nbr = -nbr;										//负数取绝对值
		while (nbr)
		{
			bit++;
			nbr /= 10u;
		}
		nbr = -nbr;
	}
	return bit;
}

//itoa测试
void itoa_Test (void)
{
	char *test_strings;
	int test_int = 123;
	
	__ShellHeadSymbol__;
	if (SendDataCondition)
	{
		printf("itoa Function Test: Transfer Integer Data: %d Transfer String: %s\r\n", test_int, itoa(test_int, test_strings, 10));
		usart1WaitForDataTransfer();
	}
}

//ASCII字符拼接
//传参：拼接源指针，拼接存放指针
//返回：拼接结果指针
char* strData_Catenate (char* Array, char result[])				
{
	u8 i;
	char *pointer = &Array[0];						
	
	while (*pointer != '\0')							//遇到数组结尾
	{	
		for (i = 0; i < Get_Array_Size(Array); i++)
			if ((*pointer > 'a' && *pointer < 'z') || (*pointer > 'A' && *pointer < 'Z'))//26个字母内
				//传参：存储目标，待拼接字符指针
				result = indStr_Catenate(result, (pointer + i));//将临时数组内的字符全部拼接成字符串
	}
	__ShellHeadSymbol__;
	if (SendDataCondition)
	{
		printf("String Catenate Result: %s\r\n", result);		
		while ((USART1 -> SR & 0X40) == 0){};			//等待发送结束
	}	
	
	return result;							
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

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
