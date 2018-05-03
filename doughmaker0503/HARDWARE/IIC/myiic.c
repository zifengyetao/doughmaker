#include "myiic.h"
#include <gpio_xmc1100_tssop38.h>
#include <xmc1100.h>
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IO方向设置  ---xmc
#define SDA_IN()   	P0_1_set_mode(INPUT_PU);//输入  上拉  
#define SDA_OUT()  P0_1_set_mode(OUTPUT_PP_GP) //输出   通用输出

//IO操作函数	  ----xmc
#define IIC_SCL_1     P0_0_set() //SCL    输出  OMR寄存器
#define IIC_SCL_0     P0_0_reset() //SCL     输出  OMR寄存器

#define IIC_SDA_1     P0_1_set() //SDA	 
#define IIC_SDA_0     P0_1_reset() //SDA	  

#define  READ_SDA     P0_1_read()    //SDA	   操作输入寄存器  读取当前电平
 
void IIC_Init(void)
{			
//GPIO_InitTypeDef  GPIO_InitStructure;

//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

//  //GPIOB8,B9初始化设置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	IIC_SCL=1;
//	IIC_SDA=1;
 
	P0_0_set_mode(OUTPUT_PP_GP);//    SCL   这里通用推挽输出    
	P0_1_set_mode(OUTPUT_PP_GP);   // SDA   这里通用推挽输出    
  IIC_SCL_1;//P0_0_set  默认输出高电平
 	IIC_SDA_1;//P0_1_set
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_1;   //P0_1_set	  	  
	IIC_SCL_1;    //P0_0_set
	delay_us(4);
 	IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_0;
	IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_1; 
	IIC_SDA_1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA_1;delay_us(1);	   
	IIC_SCL_1;delay_us(1);	 
	while(READ_SDA)//READ_SDA
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0;//时钟输出0 	  写入成功   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_0;
	delay_us(2);
	IIC_SCL_1;
	delay_us(2);
	IIC_SCL_0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_1;
	delay_us(2);
	IIC_SCL_1;
	delay_us(2);
	IIC_SCL_0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
u8  num_addr;
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL_0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {     
      num_addr = (txd&0x80)>>7; //自修改
         if(num_addr==0)
		  	{
				    IIC_SDA_0;
			  }
				else if(num_addr == 1)
					  IIC_SDA_1;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_1;
		delay_us(2); 
		IIC_SCL_0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_0; 
        delay_us(2);
		IIC_SCL_1;
        receive<<=1;
        if(READ_SDA)//sda设为输入
					receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


























