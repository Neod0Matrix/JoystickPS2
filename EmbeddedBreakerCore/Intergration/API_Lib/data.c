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
char *itoa (int value, char *string, int radix)
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

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
