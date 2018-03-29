#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//使用IIC方法驱动OLED显示

#define __OLED_Model__	"SSD1306"		//OLED型号

//显示效果及字体选择
#define SCREENROW		128u			//横轴显示分辨率
#define SCREENCOLUMN	64u				//纵轴显示分辨率
#define GMCOLUMN		(SCREENCOLUMN / 8)
/*
	OLED显存
	[0]0 1 2 3 ... 127
	[1]0 1 2 3 ... 127
	[2]0 1 2 3 ... 127
	[3]0 1 2 3 ... 127
	[4]0 1 2 3 ... 127
	[5]0 1 2 3 ... 127
	[6]0 1 2 3 ... 127
	[7]0 1 2 3 ... 127
*/
extern __align(4) u8 oled_graphic_memory[SCREENROW][GMCOLUMN];
//默认16号字体(使用两行等字体大小显示只有16号字可以正常显示)
#define Font_Size 		f16				
#define strPos(pos)		8 * pos			//显示序列，pos取值0-14(只能写15位，第16位(pos=15)有bug)
#define ROW1			32u				//f16 * 2
#define ROW2			48u				//f16 * 3
#define OneRowMaxWord	16u				//一行最多16个字符
extern __align(4) char oled_dtbuf[OneRowMaxWord];//OLED显示打印snprintf缓存

//OLED IIC指令
typedef enum {wr_cmd = 0x00, wr_dat = 0x01} OLED_WriteCD;//写命令/写数据
//使用IIC串行接口时使用
#define OLED_SCL 		PBout(13)		//IIC1_SCL
#define OLED_SDA 		PBout(15)		//IIC1_SDA
#define IIC_ADD_WC 		0x78			//写命令时的地址
#define IIC_ADD_WD 		0x78			//写数据时的地址
#define IIC_CONC 		0x00			//控制字节指定下一个字节为命令
#define IIC_COND 		0x40			//控制字节指定下一个字节为数据

//OLED显示效果(对比度反白or正常)
typedef enum {pos_ctr = 1, neg_ctr = !pos_ctr} OLED_ContrastMode;

//IIC处理
void oledDelay (void);
void IIC_Start (void);
void IIC_Stop (void);
void IIC_Wait_Ack (void);
void IIC_WriteC (u8 cmd);				//IIC写入命令
void IIC_writeD (u8 data);				//IIC写入数据
void IIC_WriteChar (u8 byte);

typedef const char StringCache;			//字符缓存数据类型

//OLED控制用函数
void OLED_WR_Byte (u8 ord, OLED_WriteCD opt);//写一个字节
void OLED_Display_On (void);			//显示开启
void OLED_Display_Off (void);			//显示关闭
void OLED_Refresh_Gram (void);			//刷新图像
void OLED_Set_Pos (u8 x, u8 y);

//OLED显示屏操作函数
void OLED_Clear (void);					//清除显示
void OLED_DrawPoint (u8 x, u8 y, OLED_ContrastMode mode);	
void OLED_ShowChar (u8 x, u8 y, u8 chr, Font_Column_Size size, OLED_ContrastMode mode);
void OLED_ShowNum (u8 x, u8 y, int32_t num, u8 len, Font_Column_Size size);
void OLED_ShowNum_Supple0 (u8 x, u8 y, int32_t num, u8 space, Font_Column_Size size);
void OLED_ShowString (u8 x, u8 y, StringCache* p, Font_Column_Size size);
void OLED_CmdSetMode (void);			//指令设置OLED显示模式
void OLED_Init (void);					//初始化SSD1300

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
