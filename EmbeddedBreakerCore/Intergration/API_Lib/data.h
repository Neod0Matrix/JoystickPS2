#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//串口ASCII字符数据处理

#define snprintfStackSpace		(sizeof(char) * 100u)		//snprintf打印空间，打印类型为char字符

#ifndef Get_Array_Size  									//数组格式大小
#define Get_Array_Size(array) 	(sizeof(array) / sizeof((array)[0]))//计算出数组宽度(整个数组存储宽度/一个元素的宽度)
#endif  

#ifndef RangeRandom
#define RangeRandom(a, b)		rand()%(b - a + 1) + a		//求取一定范围随机数
#endif

#ifndef U1TSP
#define U1TSP(str) 				U1SD(#str)					//#str把传进来的参数直接变成字符串
#endif

char* varChange_Str (char str_space[], int var);			//变量转字符
char* indStr_Catenate (char *s1, char *s2);					//独立字符串拼接
u16 Nbr10BitCalcus (int nbr);								//10进制位数获取
char *itoa (int value, char *string, int radix);			//整型数据转换为字符串
char* strData_Catenate (char* Array, char result[]);		//整体字符串拼接

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
