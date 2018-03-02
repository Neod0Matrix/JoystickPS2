#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	SSD1306 OLED驱动IC驱动代码
	驱动方式：软件模拟I2C(伪I2C)
	128x32分辨率，显示16号字，字符数正好是16x02
	因为是1602,128x32，所以只能显示16号字，其他字号会乱码
	注意字符占位之间的关系，128x64不能越位(0<x<128, 0<y<64)
*/

//OLED I2C引脚定义
#define RCC_AxBxPeri_GPIOx	RCC_APB2Periph_GPIOB
#define	GPIOx				GPIOB
//PB13-SCL PB15-SDA
#define OLI2C_SCL_PIN		GPIO_Pin_13
#define OLI2C_SDA_PIN		GPIO_Pin_15

/*
	OLED的显存
	[0]0 1 2 3 ... 127
	[1]0 1 2 3 ... 127
	[2]0 1 2 3 ... 127
	[3]0 1 2 3 ... 127
	[4]0 1 2 3 ... 127
	[5]0 1 2 3 ... 127
	[6]0 1 2 3 ... 127
	[7]0 1 2 3 ... 127
*/
u8 OLED_GRAM[Max_Column][SIZE];				//128x8显存数组

//启动IIC
void IIC_Start (void)
{
    OLED_SCL = 1;
    OLED_SDA = 1;
	delay_us(1);
    OLED_SDA = 0;
    delay_us(1);
    OLED_SCL = 0;
	delay_us(1);
}

//关闭IIC
void IIC_Stop (void)
{
	OLED_SDA = 0;
    OLED_SCL = 1;
    delay_us(1);
    OLED_SDA = 1;
}

//等待IIC响应
void IIC_Wait_Ack (void)
{
	OLED_SDA = 1;
	delay_us(1);
    OLED_SCL = 1;
	delay_us(1);
	
	//这里本来应该要等待设备应答的，但OLED没有读SDA这一选项
	
    OLED_SCL = 0;
	delay_us(1);
}

//向SSD1306写入一个字节
void IIC_WriteChar (u8 byte)
{	
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (byte & 0x80) OLED_SDA = 1;
		else OLED_SDA = 0;
		
		delay_us(1);
		OLED_SCL = 1;
		delay_us(1);
		OLED_SCL = 0;
		
		if (i == 7u) OLED_SDA = 1;			//释放总线
		
		byte <<= 1;							//左移一个bit
		delay_us(1);
	}
}

//IIC写入命令CMD
void IIC_WriteC (u8 cmd)
{
    IIC_Start();							//开始传输
    IIC_WriteChar(IIC_ADD_WC);				//传输地址
    IIC_Wait_Ack();							//接收1306返回的确认信号
    delay_us(1);
    IIC_WriteChar(IIC_CONC);				//发送控制字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    delay_us(1);
    IIC_WriteChar(cmd);						//发送命令字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    delay_us(1);
    IIC_Stop();								//结束传输
}

//IIC写入数据DATA
void IIC_WriteD (u8 data)
{
    IIC_Start();							//开始传输
    IIC_WriteChar(IIC_ADD_WD);				//传输地址
    IIC_Wait_Ack();							//接收1306返回的确认信号
    IIC_WriteChar(IIC_COND);				//发送控制字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    delay_us(1);
    IIC_WriteChar(data);					//发送数据字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    delay_us(1);
    IIC_Stop();								//结束传输
}

//OLED写一个字节
void OLED_WR_Byte (u8 ord, OLED_WriteCD opt)
{
    if (opt == wr_cmd) 	
		IIC_WriteC(ord);					//写命令
    else 			
		IIC_WriteD(ord);					//写数据
}

//OLED位置设置
void OLED_Set_Pos (u8 x, u8 y)
{
    OLED_WR_Byte(0xb0 + y, wr_cmd);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, wr_cmd);
    OLED_WR_Byte((x & 0x0f), wr_cmd);
}

//开启OLED显示
void OLED_Display_On (void)
{
    OLED_WR_Byte(0X8D, wr_cmd);  			//SET DCDC命令
    OLED_WR_Byte(0X14, wr_cmd);  			//DCDC ON
    OLED_WR_Byte(0XAF, wr_cmd);  			//DISPLAY ON
}

//关闭OLED显示
void OLED_Display_Off (void)
{
    OLED_WR_Byte(0X8D, wr_cmd);  			//SET DCDC命令
    OLED_WR_Byte(0X10, wr_cmd);  			//DCDC OFF
    OLED_WR_Byte(0XAE, wr_cmd);  			//DISPLAY OFF
}

//更新显存到OLED
void OLED_Refresh_Gram (void)
{
    u8 i, n;
	
    for (i = 0; i < SIZE; i++)
    {
        OLED_WR_Byte(0xb0 + i, wr_cmd);  	//设置页地址0-3
        OLED_WR_Byte(0x04, wr_cmd);      	//设置显示位置—列低地址
        OLED_WR_Byte(0x10, wr_cmd);      	//设置显示位置—列高地址
		
        for (n = 0; n < Max_Column; n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i], wr_dat);
        }
    }
}

//清屏函数，清完屏，整个屏幕是黑色的，和没点亮一样
void OLED_Clear (void)
{
    u8 i, n;
	
    for (i = 0; i < SIZE; i++)
    {
        for (n = 0; n < Max_Column; n++)
        {
			OLED_GRAM[n][i] = 0X00;			//清除数据
        }
    }
	
	OLED_Refresh_Gram();					//更新显示
}

/*
	点阵，画点函数
	x:0~127,y:0~63
	t:1 填充 0,清空
*/
void OLED_DrawPoint (u8 x, u8 y, u8 t)
{
    if (x > Max_Column - 1 || y > Max_Row - 1)
		return;								//超出范围直接结束程序
    if (t)
		OLED_GRAM[x][SIZE - 1 - y / SIZE] |= (1 << (SIZE - 1 - y % SIZE));
    else 
		OLED_GRAM[x][SIZE - 1 - y / SIZE] &= ~(1 << (SIZE - 1 - y % SIZE));
}

/*
	在指定位置显示一个字符,包括部分字符
	x:0~127 (X_MAX-1)
	y:0~63  (Y_MAX-1)
	mode:0,反白显示;1,正常显示
	size:选择字体 16/12
*/
void OLED_ShowChar (u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    chr -= ' ';								//得到偏移后的值
	
    for (t = 0; t < size; t++)
    {
        if (size == 12)
            temp = asc2_1206[chr][t];		//调用1206字体
        else if (size == 16)
            temp = asc2_1608[chr][t];		//调用1608字体
		else if (size == 24)
			temp = asc2_2412[chr][t];		//调用2412字体
		
        for (t1 = SIZE; t1 > 0; t1--)		//取反偏差值
        {
            if (temp & 0x80)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/*
	显示数字
	x,y :起点坐标
	len :数字的位数
	size:字体大小
	num:数值(0~4294967295)
*/
void OLED_ShowNum (u8 x, u8 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0u;
	
    for (t = 0u; t < len; t++)
    {
        temp = (num / (u32)pow(10, len - t - 1)) % 10u;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}

//数字显示，带有不足位补零效果
void OLED_ShowNum_Supple0 (u8 x, u8 y, u32 num, u8 space, u8 size)
{
	u8 i, bitNum;
	
	bitNum = Nbr10BitCalcus(num);				//获取数字的位数(十进制)
	//缺省补零
	for (i = 0; i < (space - bitNum); i++)
		OLED_ShowNum(x + i * (size / 2), y, 0, 1u, size);		
	//末尾数字显示
	OLED_ShowNum(x + (space - bitNum) * (size / 2), y, num, bitNum, size);		
}

/*
	显示字符串
	x,y:起点坐标
	size:字体大小
	*p:字符串起始地址
*/
void OLED_ShowString (u8 x, u8 y, const u8 *p, u8 size)
{
	while (*p != '\0')
    {
        if (x > Max_Column - size / 2) 
		{
            x = 0;
            y += size;
        }
        if (y > Max_Row - size / 2) 
		{
            y = x = 0;
            OLED_Clear();
        }
        OLED_ShowChar(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

//OLED指令初始化
void OLED_CmdSetMode (void)
{
	OLED_WR_Byte(0xAE, wr_cmd);				//关闭显示
	
    OLED_WR_Byte(0x40, wr_cmd);				//设置起始行地址(0x40第一行行首，0x80第二行行首)

    OLED_WR_Byte(0x81, wr_cmd);				//设置对比度
    //OLED_WR_Byte(0x7f, OLED_CMD);			//设置亮度
	OLED_WR_Byte(0xCf, wr_cmd);

    OLED_WR_Byte(0xa1, wr_cmd);				//0xa0左右反置，0xa1正常
	OLED_WR_Byte(0xc0, wr_cmd);				//0xc0上下反置，0xc8正常

    OLED_WR_Byte(0xa6, wr_cmd);				//set normal display

    OLED_WR_Byte(0xa8, wr_cmd);				//set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x1f, wr_cmd);	

    OLED_WR_Byte(0xd3, wr_cmd);				//set display offset
    OLED_WR_Byte(0x00, wr_cmd);				//not offset

    OLED_WR_Byte(0xd5, wr_cmd);				//set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0xf0, wr_cmd);
	//OLED_WR_Byte(0x80, wr_cmd);			//set divide ratio, Set Clock as 100 Frames/Sec

    OLED_WR_Byte(0xd9, wr_cmd);				//set pre-charge period
    //OLED_WR_Byte(0x22, wr_cmd);
	OLED_WR_Byte(0xf1, wr_cmd);				//set pre-charge as 15 clocks & discharge as 1 clock

    OLED_WR_Byte(0xda, wr_cmd);				//set com pins hardware configuration
    OLED_WR_Byte(0x02, wr_cmd);
	//OLED_WR_Byte(0x12, wr_cmd);

    OLED_WR_Byte(0xdb, wr_cmd);				//set vcomh
    OLED_WR_Byte(0x49, wr_cmd);
	//OLED_WR_Byte(0x40, wr_cmd);			//set vcom deselect level
	
	//OLED_WR_Byte(0x20, wr_cmd);			
	//OLED_WR_Byte(0x02, wr_cmd);			//set page addressing mode (0x00/0x01/0x02)

    OLED_WR_Byte(0x8d, wr_cmd);				//set charge pump enable/disable
    OLED_WR_Byte(0x14, wr_cmd);				//set(0x10) disable
	
	OLED_WR_Byte(0xa4, wr_cmd);				//0xa4字体正常，0xa5字体淡化
	OLED_WR_Byte(0xa6, wr_cmd);				//0xa7对比度反转，0xa6正常

    OLED_WR_Byte(0xaf, wr_cmd);				//turn on oled panel						
}

//初始化SSD1306
void OLED_Init (void)
{
	ucGPIO_Config_Init (RCC_AxBxPeri_GPIOx,			
						GPIO_Mode_Out_OD,	//数据传输开漏	
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						OLI2C_SCL_PIN | OLI2C_SDA_PIN,					
						GPIOx,					
						IHL,				
						EBO_Disable);

    OLED_CmdSetMode();						//写入初始化指令
    OLED_Clear();							//开机擦除原来的显示画面
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
