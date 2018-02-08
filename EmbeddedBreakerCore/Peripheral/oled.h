#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//使用IIC方法驱动OLED显示

#define __OLED_Model__	"SSD1306"		//OLED型号

//OLED相关定义声明
#define SIZE 			8u				//64/8=8
#define Max_Column		128u			//横轴显示分辨率
#define Max_Row			64u				//纵轴显示分辨率
//字体选择
#define Font_Size 		16u				//16号字体
#define ROW1			Font_Size * 2u	//第一行
#define ROW2			Font_Size * 3u	//第二行

//显示序列，pos取值0-14(只能写15位，第16位(pos=15)有bug)
//格式：x位置，y位置，内容，字号
#ifndef strPos
#define strPos(pos) 	(Font_Size / 2u) * pos
#endif

//使用IIC串行接口时使用
#define OLED_SCL 		PBout(13)		//IIC1_SCL
#define OLED_SDA 		PBout(15)		//IIC1_SDA

//16进制指令
typedef enum {wr_cmd = 0x00, wr_dat = 0x01} OLED_WriteCD;//写命令/写数据
#define IIC_ADD_WC 		0x78			//写命令时的地址
#define IIC_ADD_WD 		0x78			//写数据时的地址
#define IIC_CONC 		0x00			//控制字节指定下一个字节为命令
#define IIC_COND 		0x40			//控制字节指定下一个字节为数据

//IIC处理
void IIC_Start (void);
void IIC_Stop (void);
void IIC_Wait_Ack (void);
void IIC_WriteC (u8 cmd);				//IIC写入命令
void IIC_writeD (u8 data);				//IIC写入数据
void IIC_WriteChar (u8 byte);

//OLED控制用函数
void OLED_WR_Byte (u8 ord, OLED_WriteCD opt);//写一个字节
void OLED_Display_On (void);			//显示开启
void OLED_Display_Off (void);			//显示关闭
void OLED_Refresh_Gram (void);			//刷新图像
void OLED_Set_Pos (u8 x, u8 y);

//OLED显示屏操作函数
void OLED_Clear (void);					//清除显示
void OLED_DrawPoint (u8 x, u8 y, u8 t);	//画点
void OLED_ShowChar (u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_ShowNum (u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_ShowString (u8 x, u8 y, const u8 *p, u8 size);
void OLED_ShowTime (u8 x, u8 y, u32 num, u8 size);
void OLED_CmdSetMode (void);			//指令设置OLED显示模式
void OLED_Init (void);					//初始化SSD1300

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
