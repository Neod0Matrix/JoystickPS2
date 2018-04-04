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

__align(4) u8 oled_graphic_memory[SCREENROW][GMCOLUMN];	
__align(4) char oled_dtbuf[OneRowMaxWord];			

//适用于OLED IIC显示的延时
void oledDelay (void)
{
	delay_us(1);
}

//启动IIC
void IIC_Start (void)
{
    OLED_SCL = 1;
    OLED_SDA = 1;
	oledDelay();
    OLED_SDA = 0;
    oledDelay();
    OLED_SCL = 0;
	oledDelay();
}

//关闭IIC
void IIC_Stop (void)
{
	OLED_SDA = 0;
    OLED_SCL = 1;
    oledDelay();
    OLED_SDA = 1;
}

//等待IIC响应
void IIC_Wait_Ack (void)
{
	OLED_SDA = 1;
	oledDelay();
    OLED_SCL = 1;
	oledDelay();
	
	//这里本来应该要等待设备应答的，但OLED没有读SDA这一选项
    OLED_SCL = 0;
	oledDelay();
}

//向SSD1306写入一个字节
void IIC_WriteChar (u8 byte)
{	
	uint8_t i;

	//先发送字节的高7位
	for (i = 0; i < 8; i++)
	{		
		OLED_SDA = (byte & 0x80)? 1:0;
		oledDelay();
		OLED_SCL = 1;
		oledDelay();
		OLED_SCL = 0;
		//释放总线
		if (i == 7) 
			OLED_SDA = 1;			
		byte <<= 1;							//左移一个bit
		oledDelay();
	}
}

//IIC写入命令CMD
void IIC_WriteC (u8 cmd)
{
    IIC_Start();							//开始传输
    IIC_WriteChar(IIC_ADD_WC);				//传输地址
    IIC_Wait_Ack();							//接收1306返回的确认信号
    oledDelay();
    IIC_WriteChar(IIC_CONC);				//发送控制字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    oledDelay();
    IIC_WriteChar(cmd);						//发送命令字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    oledDelay();
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
	oledDelay();
    IIC_WriteChar(data);					//发送数据字节
    IIC_Wait_Ack();							//接收1306返回的确认信号
    oledDelay();
    IIC_Stop();								//结束传输
}

//OLED写一个字节
void OLED_WR_Byte (u8 ord, OLED_WriteCD opt)
{
    (opt == wr_cmd)? 
		IIC_WriteC(ord):IIC_WriteD(ord);
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
    OLED_WR_Byte(0x8D, wr_cmd);  			//SET DCDC命令
    OLED_WR_Byte(0x14, wr_cmd);  			//DCDC ON
    OLED_WR_Byte(0xAF, wr_cmd);  			//DISPLAY ON
}

//关闭OLED显示
void OLED_Display_Off (void)
{
    OLED_WR_Byte(0x8D, wr_cmd);  			//SET DCDC命令
    OLED_WR_Byte(0x10, wr_cmd);  			//DCDC OFF
    OLED_WR_Byte(0xAE, wr_cmd);  			//DISPLAY OFF
}

//更新显存到OLED
void OLED_Refresh_Gram (void)
{
    u8 i, j;
	
    for (i = 0; i < GMCOLUMN; i++)			//显存列
    {
		//进入重写模式
        OLED_WR_Byte(0xb0 + i, wr_cmd);  	
		OLED_WR_Byte(0x04, wr_cmd);      	//设置显示位置列低地址
		OLED_WR_Byte(0x10, wr_cmd);      	//设置显示位置列高地址
		
		//更新显存内容
        for (j = 0; j < SCREENROW; j++)		//显存行
            OLED_WR_Byte(*(*(oled_graphic_memory + j) + i), wr_dat);
    }
}

//清除屏幕所有旧显示
void OLED_Clear (void)
{
	/*
    u8 i, j;
	
	//显存清空
    for (i = 0; i < GMCOLUMN; i++)			//显存列
	{
        for (j = 0; j < SCREENROW; j++)		//显存行
			*(*(oled_graphic_memory + j) + i) = 0x00;	
	}
	*/
	
	//使用标准库实现显存数组一次性清零，效率最高
	memset(oled_graphic_memory, 0x00, sizeof(oled_graphic_memory));
	OLED_Refresh_Gram();					
}

//点阵，画点函数	
void OLED_DrawPoint (u8 x, u8 y, OLED_ContrastMode mode)
{
	short poi, surcol;
	
	//超出显存限定
    if (x > SCREENROW - 1 || y > SCREENCOLUMN - 1)
		return;		
	
	//转换显示模式
	poi = 1 << (7 - y % 8), surcol = 7 - y / 8;
    (mode)? (*(*(oled_graphic_memory + x) + surcol) |= poi):
		(*(*(oled_graphic_memory + x) + surcol) &= ~poi);
}

/*
	在指定位置显示一个字符,包括部分字符
	x:		0~127 (X_MAX-1)
	y:		0~63  (Y_MAX-1)
	mode:	0,反白显示;1,正常显示
	size:	选择字号
*/
void OLED_ShowChar (u8 x, u8 y, u8 chr, Font_Column_Size size, OLED_ContrastMode mode)
{
    u8 getArrayWord, j, i, ty = y, forward_word;
	FontArray* font_select;

	//字号选型(默认固定字号16号字)
	switch (size)
	{
	case f12: font_select = (FontArray*)ascii_1206; break;
	case f16: font_select = (FontArray*)ascii_1608; break;
	case f24: font_select = (FontArray*)ascii_2412; break;
	}
	
	forward_word = chr - ' ';				//ascii序列偏移一个空格
    for (j = 0; j < size; j++)
    {
		//通过一阶指针访问二阶数组取字
		getArrayWord = *(font_select + (forward_word * size + j));	
		//该部分不同于标准库，是修改过显示效果的操作
        for (i = 8; i > 0; i--)						
        {
			OLED_DrawPoint(x, ty, (getArrayWord & 0x80)? mode:(OLED_ContrastMode)!mode);
            getArrayWord <<= 1;								
            if (++ty == (size + y))
            {
                ty = y, x++;
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
	num	:显示数值
	(某个迭代版本已把这个函数替代掉)
*/
void OLED_ShowNum (u8 x, u8 y, int32_t num, u8 len, Font_Column_Size size)
{
    u8 t, temp, size_cache, enshow = 0u;
	
    for (t = 0; t < len; t++)
    {
        temp = (num / (u32)pow(10, len - t - 1)) % 10u;
        if (!enshow && t < (len - 1))
        {
			size_cache = (size / 2) * t;
            if (!temp)
            {
                OLED_ShowChar(x + size_cache, y, ' ', size, pos_ctr);
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + size_cache, y, temp + '0', size, pos_ctr);
    }
}

//数字显示，带有不足位补零效果(某个迭代版本已把这个函数替代掉)
void OLED_ShowNum_Supple0 (u8 x, u8 y, int32_t num, u8 space, Font_Column_Size size)
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
void OLED_ShowString (u8 x, u8 y, StringCache* p, Font_Column_Size size)
{
	u8 tx = x, ty = y, half_size = size / 2;
	
	//遍历字符串指针所有内容
	while (*p != '\0')
    {
        if (tx > SCREENROW - half_size) 
		{
            tx = 0;
			//TODO:20180329pm1326修改size为half_size，TMD居然也能正常显示，但还是有长时间显示错位的bug
            ty += half_size;					
        }
        if (ty > SCREENCOLUMN - half_size) 
		{
            ty = 0, tx = 0;
            OLED_Clear();
        }
        OLED_ShowChar(tx, ty, *p, size, pos_ctr);
        tx += half_size;
        p++;								//下一个字
    }
}

//OLED指令初始化
void OLED_CmdSetMode (void)
{
	OLED_WR_Byte(0xAE, wr_cmd);				//关闭显示
	
    OLED_WR_Byte(0x40, wr_cmd);				//设置起始行地址(0x40第一行行首，0x80第二行行首)

    OLED_WR_Byte(0x81, wr_cmd);				//设置对比度
    OLED_WR_Byte(0x7f, wr_cmd);				//设置亮度
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
	OLED_WR_Byte(0x80, wr_cmd);				//set divide ratio, Set Clock as 100 Frames/Sec

    OLED_WR_Byte(0xd9, wr_cmd);				//set pre-charge period
    //OLED_WR_Byte(0x22, wr_cmd);
	OLED_WR_Byte(0xf1, wr_cmd);				//set pre-charge as 15 clocks & discharge as 1 clock

    OLED_WR_Byte(0xda, wr_cmd);				//set com pins hardware configuration
    OLED_WR_Byte(0x02, wr_cmd);
	//OLED_WR_Byte(0x12, wr_cmd);

    OLED_WR_Byte(0xdb, wr_cmd);				//set vcomh
    OLED_WR_Byte(0x49, wr_cmd);
	OLED_WR_Byte(0x40, wr_cmd);				//set vcom deselect level
	
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
						GPIO_Mode_Out_PP,	//这里设置成开漏输出也可以
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
