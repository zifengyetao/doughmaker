#include "datatransfer.h"	
#include <XMC1100.h>		//SFR declarations of the selected device
#include "gpio_xmc1100_tssop38.h"
#include "ds18b20.h"
#include "MCUInit.h"
#include <stdio.h>
#include "string.h" 
#include "UART.h" 
#include "IIC.h"
#include <stdio.h>
#include "24cxx.h"
#include "string.h" 
#include "ctype.h"
#include "record_map.h"
#include <stdbool.h>
#include "delay.h"

 
#define	TRUE	    1
#define	FALSE	    0

#define s_limit 0x4fff
#define m_offset 1024

//*******************11/26************************ 
#define	WriteChinese	0x42
#define	WriteData	    0x3D
#define	WriteTime	    0x44
#define	EnableTime	  0x3B
#define ChangePG	  0x70 

#define ReadData      0x3E  
#define ReadData_String     0x43


u8 USART_RX_BUF[1024];
u8 USART_RX_BUF_DOWN[1024];
extern u32 recev_start,recev_end;
extern u32 recev_start_down,recev_end_down;
 

//文字编码
u8   mixflour_fast[11]={0xBA,0xCD,0xC3,0xE6,0xB9,0xB4,0xBF,0xEC,0xCB,0xD9,0x00};
u8   mixflour_slow[11]={0xBA,0xCD,0xC3,0xE6,0xB9,0xB4,0xC2,0xFD,0xCB,0xD9,0x00};
u8   mixflour_keg_fast[11]={0xBA,0xCD,0xC3,0xE6,0xCD,0xB0,0xBF,0xEC,0xCB,0xD9,0x00};
u8   mixflour_keg_slow[11]={0xBA,0xCD,0xC3,0xE6,0xCD,0xB0,0xC2,0xFD,0xCB,0xD9,0x00};
u8   no_opera[7]= {0xCE,0xDE,0xB2,0xD9,0xD7,0xF7,0x00};
u8   stop[5]={0xD4,0xDD,0xCD,0xA3,0x00};
u8   Speed_DATA[4];

u8 temperature;
u8 data[21];
u8 USART_TX_BUF[1024];

u8 EE_ALL_DATA_test[type_num][data_max_num]; 
u16 i,j;
unsigned int CRC16;
u8 databuf[6];
//*********************************************
 
////////和面机新-0204
u8  key_number[2];// 
u8  Time_data_number[7];//0 1 2 3 4 5 6依次为自动手动各个页面数据转存

int speed_flag = 0;
int slow_flag = 0;
int turn_flag = 0;//勾正反转 默认正转
int proce_flag[5];//程序设置页面5个flag
int Time_Flag[6];//六个定时器flag
int proce_number[5];
int Now_Num,Next_Num;

int page_flag;//
int motor_flag;
//****************************
//手动页面全局变量
int stop_flag = 0;//运行暂停选择flag
int fast_slow_flag;//快速慢速图标
int start_flag;//运行
int stop_new_flag;//暂停
int hook_for;//勾正转
int hook_reverse;//勾反转
int tub_start;//桶点动
int tub_reverse;//桶反转

//****************************
//自动页面全局变量
int auto_start_flag;//运行
int auto_stop_new_flag;//暂停
int auto_tub_start;//桶点动
int auto_tub_reverse;//桶反转
//*******
u8 mainmotor[5] = {0x00,0x00,0x00,0x00,0x00};//主电机方向和转速
u8 assmotor[5] = {0x00,0x00,0x00,0x00,0x00};//副电机方向和转速
//**********
int timer_num_flag;//定时器运行flag

extern u8 Hand_Read_data[4],Auto_Read_data[10];
//管理收到的触摸屏的串口信息
void manage_one_pack(void)
{

	u32 m_start,m_end,m_end1;
	u8 mm_ok,mm1,mm2;
	m_start=recev_start;
	m_end=recev_end;
	
	if(m_end<m_start) m_end1 = m_end+m_offset;
	else m_end1 = m_end;
	mm_ok=0;	
	
	do
	{
		mm1=USART_RX_BUF[m_start];
		if(mm1 == 0xAA)//||(mm1==0x3A)||(mm1==0x21))
		{	  
			mm2=USART_RX_BUF[m_start+1];
		  if(mm2==0x77)
			{	 
	      if((m_end1-m_start)>=11)
				{
           if((USART_RX_BUF[m_start+8]==0xCC)&&(USART_RX_BUF[m_start+9]==0x33)&&  
						  (USART_RX_BUF[m_start+10]==0xC3)&& (USART_RX_BUF[m_start+11]==0x3C))
					 {   
								
                recev_start=deal_one_pack(m_start);
						 		mm_ok=1;
           }
					 //定时器
					 if((USART_RX_BUF[m_start+10]==0xCC)&&(USART_RX_BUF[m_start+11]==0x33)&&  
						  (USART_RX_BUF[m_start+12]==0xC3)&& (USART_RX_BUF[m_start+13]==0x3C))
					 {
						    recev_start=deal_one_pack(m_start);
						 		mm_ok=1;		 						 
					 }
           else
					 {  mm_ok=2; //数据不对						 
					 }
        }
        else
				{  	
					mm_ok=3;//数据长度不对
				}
      }
			else if(mm2==0x78)
		  {
				if((m_end1-m_start)>=8)
			  {			
           if((USART_RX_BUF[m_start+5]==0xCC)&&(USART_RX_BUF[m_start+6]==0x33)&&  
						  (USART_RX_BUF[m_start+7]==0xC3)&& (USART_RX_BUF[m_start+8]==0x3C))
						{
						 	  
							recev_start=deal_one_pack(m_start);
							mm_ok=1;
						}
						else mm_ok=2;// 数据尾段不符
			  }
			  else mm_ok=3;//数据包不完整，退出等待
		}
			else if(mm2==0x43)
			{
				     recev_start=deal_one_pack(m_start);
						 		mm_ok=1;
			}
			else
			{
				 if(mm2!=0)
				 {
				    m_start=(m_start+1)&s_limit;
				 }
			}
	 }
	 else
		{
			m_start=(m_start+1)&s_limit;
		}
  }while((m_start!=m_end)&&(mm_ok==0));
	
	if((mm_ok==0)||(mm_ok==2))//丢包
	{
		recev_start=m_end;
	}
}	
 
//管理收到的下位机的串口信息
void manage_one_down_pack(void)
{
	u32 m_start,m_end,m_end1;
	u8 mm_ok,mm1,mm2;
	m_start=recev_start_down;
	m_end=recev_end_down;
	
	if(m_end<m_start) m_end1 = m_end+m_offset;
	else m_end1 = m_end;
	mm_ok=0;	
	int sum,sum_add;
	do
	{
		mm1=USART_RX_BUF_DOWN[m_start];
		if(mm1 == 0x7E)//||(mm1==0x3A)||(mm1==0x21))
		{	  
			mm2=USART_RX_BUF_DOWN[m_start+1];
		  if(mm2==0x6F)//回应ok
			{	 
	      if((m_end1-m_start)>=6)//长度
				{
           if(USART_RX_BUF_DOWN[m_start+5]==0x0D)//帧尾
					 {   									 
                recev_start=deal_one_pack_down(m_start);
						 		mm_ok=1;
           }				  
           else
					 {  mm_ok=2; //数据不对						 
					 }
        }
        else
				{  	
					mm_ok=3;//数据长度不对
				}
      }
			else if(mm2==0x3F)//回应转速信息
		  {
				if((m_end1-m_start)>=16)
			  {
						for(i = 1;i<13;++i)
						{
								 sum +=USART_RX_BUF_DOWN[i];
						}
					  sum_add = (u8)sum;	
						//和校验+帧尾校验
           if((USART_RX_BUF[m_start+13]== (0xf0&sum_add+0x30))&&(USART_RX_BUF[m_start+14]==(0x0f&sum_add+0x30))&&(USART_RX_BUF[m_start+15] == 0x0D))
						{						 	  
							recev_start=deal_one_pack_down(m_start);
							mm_ok=1;
						}
						else mm_ok=2;// 数据尾段不符
			  }
			  else mm_ok=3;//数据包不完整，退出等待
		  }
			else if(mm2==0x66)//故障返回
			{
				if((m_end1-m_start)>=14)
			  {
						for(i = 1;i<11;++i)
						{
								 sum +=USART_RX_BUF_DOWN[i];
						}
					  sum_add = (u8)sum;	
						//和校验+帧尾校验
           if((USART_RX_BUF[m_start+12]== (0xf0&sum_add+0x30))&&(USART_RX_BUF[m_start+13]==(0x0f&sum_add+0x30))&&(USART_RX_BUF[m_start+14] == 0x0D))
						{						 	  
							recev_start=deal_one_pack_down(m_start);
							mm_ok=1;
						}
						else mm_ok=2;// 数据尾段不符
			  }
			  else mm_ok=3;//数据包不完整，退出等待
			}
			else if(mm2==0x88)//预留接口--询问信息
			{
			
			}
			else
			{
				 if(mm2!=0)
				 {
				    m_start=(m_start+1)&s_limit;
				 }
			}
	 }
	 else
		{
			m_start=(m_start+1)&s_limit;
		}
  }while((m_start!=m_end)&&(mm_ok==0));
	
	if((mm_ok==0)||(mm_ok==2))//丢包
	{
		recev_start=m_end;
	}
}
//此函数主要用于分析上位机传来的数据 
//并且进行相应动作
//给出组织内容形式
u32 deal_one_pack(u32 start_points)//改u32
{
	u8  hx5,ty1,ty2,i,hx2;
	u16 hx1;
	u32 h_recev_start;
 
	
	hx5=USART_RX_BUF[start_points+1]; 
	
	if(hx5==0x77)//键盘输入
	{
		
		if((USART_RX_BUF[(start_points+2)&s_limit]==0x00)&&(USART_RX_BUF[(start_points+3)&s_limit]==0x08))    // 
		{		
       key_number[0]= (USART_RX_BUF[(start_points+6)]);
			 key_number[1]= (USART_RX_BUF[(start_points+7)]);//暂时只处理小的值			
			 hx1=(USART_RX_BUF[(start_points+5)]);       //具体地址 
				switch(hx1)//时钟23上限
				{
					//----页面1
					case 0x12://快速
					{
						send_lcd_pack(WriteData,0x0008002E,0x0000);
						if(fast_slow_flag == 0)
						{
							Time_data_number[0] = key_number[1];
							send_lcd_pack(WriteData,0x0008000E,key_number[1]);// 快速
							speed_flag = 1;//快速
						}
						else key_number[1] = 0;	
						break;
					}
					case 0x14://慢速
					{
						send_lcd_pack(WriteData,0x0008002E,0x0000);
						if(fast_slow_flag == 1)
						{
							Time_data_number[1] = key_number[1];
							send_lcd_pack(WriteData,0x00080010,key_number[1]);// 慢速;
							speed_flag = 0;//慢速 
						}
						else key_number[1] = 0;	
						break;
					}
					//----页面3
					case 0x16://和面勾快速
					{
							Time_data_number[2] = key_number[1];
							send_lcd_pack(WriteData,0x00080004,key_number[1]);						
						 
							send_lcd_pack(WriteTime,0x0002FFE4,Time_data_number[2]*60);
									 
						 				 
						  						  
							AT24CXX_Write(auto_page_pointer,&key_number[1],1);
							break;
					}
					case 0x18://和面勾慢速
					{
						Time_data_number[3] = key_number[1];
						send_lcd_pack(WriteData,0x00080006,key_number[1]);//
							 
						send_lcd_pack(WriteTime,0x0002FFE8,Time_data_number[3]*60);

						AT24CXX_Write(auto_page_pointer+2,&key_number[1],1);						
						break;
					}
					case 0x1A://暂停
					{
						Time_data_number[4] = key_number[1];
						send_lcd_pack(WriteData,0x00080008,key_number[1]);// ;
						send_lcd_pack(WriteTime,0x0002FFEC,Time_data_number[4]*60);

						AT24CXX_Write(auto_page_pointer+4,&key_number[1],1);
						break;
					}
					case 0x1C://和面桶快速
					{
						Time_data_number[5] = key_number[1];
						send_lcd_pack(WriteData,0x0008000A,key_number[1]);//;
						send_lcd_pack(WriteTime,0x0002FFF0,Time_data_number[5]*60);	
						
						AT24CXX_Write(auto_page_pointer+6,&key_number[1],1);
						break;
					}
					case 0x1E://和面桶慢速
					{
						Time_data_number[6] = key_number[1];
						send_lcd_pack(WriteData,0x0008000C,key_number[1]);//;
						send_lcd_pack(WriteTime,0x0002FFF4,Time_data_number[6]*60);	
						
						AT24CXX_Write(auto_page_pointer+8,&key_number[1],1);
						break;
					}
					default:
							break;
				}			
		h_recev_start=(start_points+12);		
		}
	  else if((USART_RX_BUF[(start_points+2)&s_limit]==0x00)&&(USART_RX_BUF[(start_points+3)&s_limit]==0x02)&&
			   (USART_RX_BUF[(start_points+4)&s_limit]==0xFF))    //时间溢出回复   触摸屏定时器计时时间溢出
		{			
					ty1 = USART_RX_BUF[start_points+5];
					switch(ty1)
					{
						case 0xE0://定时器0
						{
							Time_Flag[0] = !Time_Flag[0];
							send_lcd_pack(EnableTime,0x00FFFF00,0x00);//  除能定时器0 
							send_lcd_pack(WriteData,0x0008002E,0x0000);
							send_lcd_pack(WriteData,0x00080030,0x0001);
							send_speed_pack(SET,stop_speed_flag);//停止操作
							break;
						}
						case 0xE4://定时器1
						{
							//Time_Flag[1] = !Time_Flag[1];
							timer_num_flag = 1;
							send_lcd_pack(EnableTime,0x00FFFF01,0x00);//  除能定时器1
							if(proce_flag[1])
							{
								send_lcd_pack(EnableTime,0x00FFFF02,0x01);//  开启定时器2
							 
							}
							else if(proce_flag[2]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF03,0x01);//  开启定时器3
							 
							}
							else if(proce_flag[3]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF04,0x01);//  开启定时器4
								 
							}
							else if(proce_flag[4]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF05,0x01);//  开启定时器5
								 
							}
//							send_lcd_pack(WriteChinese,0x00000080,(u16)proce_number[1]);//正在进行
//							send_lcd_pack(WriteChinese,0x00000100,(u16)proce_number[2]);
							send_lcd_pack(WriteChinese,0x00000080,0x03);//正在进行
							send_lcd_pack(WriteChinese,0x00000100,0x05);
							
							mainmotor[1] = 0x00;//勾慢速的时间
							mainmotor[2] = 0x00;
							mainmotor[3] = 0x00;
							mainmotor[4] = Time_data_number[3];
							send_speed_pack(SET,0x00);//
							break;
						}
						case 0xE8://定时器2
						{
							//Time_Flag[2] = !Time_Flag[2];
							timer_num_flag = 2;
							send_lcd_pack(EnableTime,0x00FFFF02,0x00);//  除能定时器2
							if(proce_flag[2]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF03,0x01);//  开启定时器3
								 
							}
							else if(proce_flag[3]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF04,0x01);//  开启定时器4			
							 
							}								
							else if(proce_flag[4]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF05,0x01);//  开启定时器5
							
							}
														 
							send_speed_pack(SET,stop_speed_flag);//				
							break;
						}
						case 0xEC://定时器3
						{
							//Time_Flag[3] = !Time_Flag[3];
							timer_num_flag = 3;
							send_lcd_pack(EnableTime,0x00FFFF03,0x00);//  除能定时器3 
							if(proce_flag[3]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF04,0x01);//  开启定时器4								
							}
							else if(proce_flag[4]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF05,0x01);//  开启定时器5						
							}
							
//							send_lcd_pack(WriteChinese,0x00000080,(u16)proce_number[2]);//正在进行
//							send_lcd_pack(WriteChinese,0x00000100,(u16)proce_number[3]);
							send_lcd_pack(WriteChinese,0x00000080,0x05);//正在进行
							send_lcd_pack(WriteChinese,0x00000100,0x00);
							
							mainmotor[1] = 0x00;//勾慢速的时间
							mainmotor[2] = 0x00;
							mainmotor[3] = 0x00;
							mainmotor[4] = Time_data_number[5];
							send_speed_pack(SET,0x00);//
							break;
						}
						case 0xF0://定时器4
						{
							//Time_Flag[4] = !Time_Flag[4];
							timer_num_flag = 4;
							send_lcd_pack(EnableTime,0x00FFFF04,0x00);//  除能定时器4
							if(proce_flag[4]) 
							{
								send_lcd_pack(EnableTime,0x00FFFF05,0x01);//  开启定时器5
							
							}
							
							mainmotor[1] = 0x00;//勾慢速的时间
							mainmotor[2] = 0x00;
							mainmotor[3] = 0x00;
							mainmotor[4] = Time_data_number[6];
							send_speed_pack(SET,0x00);//					
							break;
						}
						case 0xF4://定时器5
						{
							//Time_Flag[5] = !Time_Flag[5];
							timer_num_flag = 0;
							send_lcd_pack(EnableTime,0x00FFFF05,0x00);//  除能定时器5
							
							
							send_lcd_pack(WriteChinese,0x00000080,(u16)proce_number[3]);//正在进行
							send_lcd_pack(WriteChinese,0x00000100,(u16)proce_number[0]);
							send_speed_pack(SET,stop_speed_flag);//停止
							break;
						}
					}	
    }

	}			
	else if(hx5==0x78)//触摸返回值
	{
		ty1=USART_RX_BUF[start_points+3];  //页面序号
		ty2=USART_RX_BUF[start_points+4];  //触摸序号
	  if(ty1 == 0x00)		//电源界面
		{
				switch(ty2)
				{
					case 0x00://自动
					{
						  page_flag = automatic;	
							send_lcd_pack(WriteChinese,0x00000080,0x0000);
							send_lcd_pack(WriteChinese,0x00000100,0x0000);
							send_lcd_pack(WriteData,0x00080002,0x0020);
							send_lcd_pack(WriteChinese,0x00000300,0x0001);//网罩开关状态  开
        			send_lcd_pack(WriteChinese,0x00000380,0x0001);//和面勾转向  ---正转
						
						send_lcd_pack(WriteData,0x00080004,Auto_Read_data[0]);//勾快速
						send_lcd_pack(WriteTime,0x0002FFE4,Auto_Read_data[0]*60);
						Time_data_number[2] = Auto_Read_data[0];
						send_lcd_pack(WriteData,0x00080006,Auto_Read_data[2]);//勾慢速
						send_lcd_pack(WriteTime,0x0002FFE8,Auto_Read_data[2]*60);
						Time_data_number[3] = Auto_Read_data[2];
						send_lcd_pack(WriteData,0x00080008,Auto_Read_data[4]);//暂停
						send_lcd_pack(WriteTime,0x0002FFEC,Auto_Read_data[4]*60);
						Time_data_number[4] = Auto_Read_data[4];
						send_lcd_pack(WriteData,0x0008000A,Auto_Read_data[6]);//桶快速
						send_lcd_pack(WriteTime,0x0002FFF0,Auto_Read_data[6]*60);
						Time_data_number[5] = Auto_Read_data[6];
						send_lcd_pack(WriteData,0x0008000C,Auto_Read_data[8]);//桶慢速
						send_lcd_pack(WriteTime,0x0002FFF4,Auto_Read_data[8]*60);
						Time_data_number[6] = Auto_Read_data[8];
							break;	
					}
					case 0x01://手动
					{
						  page_flag = hand;
						
						
						 send_lcd_pack(WriteData,0x0008000E,Hand_Read_data[0]);// 快速时间
						 send_lcd_pack(WriteData,0x00080010,Hand_Read_data[2]);// 慢速时间
						
						send_lcd_pack(WriteData,0x0008000E,Hand_Read_data[0]);//快速
						Time_data_number[0] = Hand_Read_data[0];
						send_lcd_pack(WriteData,0x00080010,Hand_Read_data[2]);//慢速
						Time_data_number[1] = Hand_Read_data[2];
							break;
					}
				}
		}
	  else if(ty1==0x01)// 手动
		{ 
			  
				switch(ty2)
				{
					case 0x03://运行
					{
						 
						start_flag = 1;
						stop_new_flag = 0;
						send_lcd_pack(WriteData,0x0008002E,0x0001);
						send_lcd_pack(WriteData,0x00080030,0x0000);
					 
						//写eeprom
						AT24CXX_Write(Hand_page_pointer,&Time_data_number[0],1);
						AT24CXX_Write(Hand_page_pointer+2,&Time_data_number[1],1);
						//下面控制器指令
					 
						if(fast_slow_flag == 0)//快速
						{
							mainmotor[1] = 0x00;//勾快速的时间
							mainmotor[2] = 0x00;
							mainmotor[3] = 0x00;
							mainmotor[4] = Time_data_number[0];
							
							assmotor[1] = 0x00;//勾快速的时间
							assmotor[2] = 0x00;
							assmotor[3] = 0x00;
							assmotor[4] = Time_data_number[0];
							 
						}
						else
						{
							mainmotor[1] = 0x00;//勾慢速的时间
							mainmotor[2] = 0x00;
							mainmotor[3] = 0x00;
							mainmotor[4] = Time_data_number[1];
							
							assmotor[1] = 0x00;//勾慢速的时间
							assmotor[2] = 0x00;
							assmotor[3] = 0x00;
							assmotor[4] = Time_data_number[1];
							 
						}
						send_speed_pack(SET,0x00);// 
						//定时器
						if(stop_flag == 0)//不是暂停状态
						{
							if(fast_slow_flag == 0)
							send_lcd_pack(WriteTime,0x0002FFE0,Time_data_number[0]*60);// 
							else if(fast_slow_flag == 1)
							send_lcd_pack(WriteTime,0x0002FFE0,Time_data_number[1]*60);// 
						}
						else
						{
								stop_flag = 0;
						}
						send_lcd_pack(EnableTime,0x00FFFF00,0x01);//使能定时器0 倒计时0x01
						
						send_lcd_pack(WriteChinese,0x00000180,0x0001);//网罩开关状态  开
						send_lcd_pack(WriteChinese,0x00000280,0x0001);//和面勾转向  --正转
						break;
					}
					case 0x04://暂停
					{
						 
							stop_new_flag = 1;
							start_flag = 0;
							send_lcd_pack(WriteData,0x0008002E,0x0000);
							send_lcd_pack(WriteData,0x00080030,0x0001);
					 
						//下面控制器指令						
	          send_speed_pack(SET,stop_speed_flag);// 
						send_lcd_pack(EnableTime,0x00FFFF00,0x00);//关闭定时器0 倒计时0x01
						stop_flag = 1;//暂停flag
						break;
					}
					case 0x07://勾正转
					{
						 
							hook_for = 1;
							hook_reverse = 0;
							send_lcd_pack(WriteData,0x00080032,0x0001);
							send_lcd_pack(WriteData,0x00080034,0x0000);
						 
						 
							turn_flag = 0;
							
							mainmotor[0] = 0;
							send_speed_pack(SET,0x00);// 
							send_lcd_pack(WriteChinese,0x00000280,0x0001);
						 
						break;
					}
					case 0x08://勾反转
					{
						 
							hook_for = 0;
							hook_reverse = 1;
							send_lcd_pack(WriteData,0x00080034,0x0001);
							send_lcd_pack(WriteData,0x00080032,0x0000);
						 
						 	turn_flag = 1;
							mainmotor[0] = 1;
							send_speed_pack(SET,0x00);//												
						 
						send_lcd_pack(WriteChinese,0x00000280,0x0000);	
						break;
					}
					case 0x05://桶点动
					{
						 
							tub_start = 1;
							tub_reverse = 0;
							send_lcd_pack(WriteData,0x00080036,0x0001);
							send_lcd_pack(WriteData,0x00080038,0x0000);
						assmotor[0] = 0; 
						send_speed_pack(SET,0x00);
						break;
					}
					case 0x06://桶反转
					{
						 
							tub_reverse = 1;
							tub_start = 0;
							send_lcd_pack(WriteData,0x00080038,0x0001);
							send_lcd_pack(WriteData,0x00080036,0x0000);
						 
					  assmotor[0] = 1;
						send_speed_pack(SET,0x00);
						break;
					}
					case 0x00://快速图标 
					{
						fast_slow_flag = 0;
						send_lcd_pack(WriteData,0x0008002A,0x0001);
						send_lcd_pack(WriteData,0x0008002C,0x0000);
						break;
					}
					case 0x0A://慢速图标
					{
						fast_slow_flag = 1;
						send_lcd_pack(WriteData,0x0008002C,0x0001);
						send_lcd_pack(WriteData,0x0008002A,0x0000);
						break;
					}
					case 0x09://返回图标--关闭手动模块
					{
						//定时器
						send_lcd_pack(WriteTime,0x0002FFE0,0x00);//定时器清零
						send_lcd_pack(EnableTime,0x00FFFF00,0x00);//除能定时器0  
						
						//停止命令
						send_speed_pack(SET,stop_speed_flag);// 
						
						send_lcd_pack(WriteData,0x0008002E,0x0000);//运行
						send_lcd_pack(WriteData,0x00080030,0x0000);//暂停
						send_lcd_pack(WriteData,0x00080032,0x0000);
						send_lcd_pack(WriteData,0x00080034,0x0000);
						send_lcd_pack(WriteData,0x00080036,0x0000);
						send_lcd_pack(WriteData,0x00080038,0x0000);
						send_lcd_pack(WriteData,0x0008002A,0x0000);
						send_lcd_pack(WriteData,0x0008002C,0x0000);
						
						send_lcd_pack(WriteData,0x0008000E,0x00);// 快速
						send_lcd_pack(WriteData,0x00080010,0x00);// 快速
						
						break;
					}
				}					
    }
		else if(ty1==0x02)//自动
		{
			
			switch(ty2)
			{
				case 0x03://运行
				{

				  auto_start_flag = 1;//运行
					auto_stop_new_flag = 0;//暂停
					
					//相对应图标高亮
				  send_lcd_pack(WriteData,0x0008003E,0x0001);
					send_lcd_pack(WriteData,0x00080040,0x0000);
					
					
					send_lcd_pack(WriteChinese,0x00000080,0x01);//正在进行
					send_lcd_pack(WriteChinese,0x00000100,0x03);
//					//写eeprom
//					AT24CXX_Write(auto_page_pointer,&Time_data_number[2],1);//勾快速
//					AT24CXX_Write(auto_page_pointer+2,&Time_data_number[3],1);//勾慢速
//					AT24CXX_Write(auto_page_pointer+4,&Time_data_number[4],1);//暂停
//					AT24CXX_Write(auto_page_pointer+6,&Time_data_number[5],1);//桶快速
//					AT24CXX_Write(auto_page_pointer+8,&Time_data_number[6],1);//桶慢速
					
					//发送至下面电机
					mainmotor[0] = 0;//勾正转  0正转 1反转
				  mainmotor[1] = 0x00;//勾快速的时间
					mainmotor[2] = 0x00;
					mainmotor[3] = 0x00;
					mainmotor[4] = Time_data_number[2];
					
					assmotor[1] = 0x00;//勾快速的时间
					assmotor[2] = 0x00;
					assmotor[3] = 0x00;
					assmotor[4] = Time_data_number[2];
									 
					send_speed_pack(SET,0x00);//
					//定时器操作
					//Time_Flag[1] = !Time_Flag[1];//定时器1flag   时间到了flag取反
					if(auto_stop_new_flag == 1)
					  send_lcd_pack(EnableTime,0x00FFFF01+timer_num_flag,0x01);//定时器1启动
					else 
						send_lcd_pack(EnableTime,0x00FFFF01,0x01);//定时器1启动
					break;
				}
				case 0x04://暂停
				{
					auto_start_flag = 0;//运行
					auto_stop_new_flag = 1;//暂停
					send_lcd_pack(WriteData,0x0008003E,0x0000);
					send_lcd_pack(WriteData,0x00080040,0x0001);
					
					
					send_speed_pack(SET,stop_speed_flag);//暂停--转速设为0
					for(int i = 0; i < 5; ++i)
					{
						send_lcd_pack(EnableTime,0x00FFFF00+i,0x00);//定时器关闭
					}					
					break;
				}
				case 0x05://点动
				{
				  auto_tub_start = 1;//桶点动
				  auto_tub_reverse = 0;//桶反转
					send_lcd_pack(WriteData,0x0008003A,0x0001);
					send_lcd_pack(WriteData,0x0008003C,0x0000);
					//和面桶快速的时间
					assmotor[0] = 0;
					send_speed_pack(SET,0x00);
					break;
				}
				case 0x06://反转
				{
					auto_tub_start = 0;//桶点动
				  auto_tub_reverse = 1;//桶反转
					send_lcd_pack(WriteData,0x0008003A,0x0000);
					send_lcd_pack(WriteData,0x0008003C,0x0001);
					//和面桶慢速的时间
					assmotor[0] = 1;
					send_speed_pack(SET,0x00);
					break;
				}
				case 0x01://设置---暂时空闲
				{
					//send_speed_pack(0x01,speed_flag,turn_flag); 
					break;
				}
				case 0x00://返回--清零自动模块
				{
					//正在进行
					send_lcd_pack(WriteChinese,0x00000080,0x00);//正在进行
					send_lcd_pack(WriteChinese,0x00000100,0x00);	
					
					send_speed_pack(SET,stop_speed_flag);//暂停--转速设为0
					for(int i = 1; i < 5; ++i)
					{
						send_lcd_pack(EnableTime,0x00FFFF00+i,0x00);//定时器关闭
					}	
					
					//显示
					send_lcd_pack(WriteData,0x0008003E,0x0000);
					send_lcd_pack(WriteData,0x00080040,0x0000);
					send_lcd_pack(WriteData,0x0008003A,0x0000);
					send_lcd_pack(WriteData,0x0008003C,0x0000);
					
					//定时器
					send_lcd_pack(WriteTime,0x0002FFE4,Time_data_number[2]*60);
					send_lcd_pack(WriteTime,0x0002FFE8,Time_data_number[3]*60);
					send_lcd_pack(WriteTime,0x0002FFEC,Time_data_number[4]*60);
					send_lcd_pack(WriteTime,0x0002FFF0,Time_data_number[5]*60);	
					send_lcd_pack(WriteTime,0x0002FFF4,Time_data_number[6]*60);	
					break;
				}
			}
			
		}
		else if(ty1==0x03)//程序设置
		{
			switch(ty2)
			{
				case 0x05://勾快速
				{
					proce_flag[0] = !proce_flag[0];
					motor_flag = 0;
					send_lcd_pack(WriteData,0x00080020,proce_flag[0]);
					break;
				}
				case 0x06://勾慢速
				{
					proce_flag[1] = !proce_flag[1];
					motor_flag = 1;
					send_lcd_pack(WriteData,0x00080022,proce_flag[1]);
					break;
				}
				case 0x07://暂停
				{
					proce_flag[2] = !proce_flag[2];
					motor_flag = 2;
					send_lcd_pack(WriteData,0x00080024,proce_flag[2]);
					break;
				}
				case 0x08://桶快速
				{
					proce_flag[3] = !proce_flag[3];
					motor_flag = 3;
					send_lcd_pack(WriteData,0x00080026,proce_flag[3]);
					break;
				}
				case 0x09://桶慢速
				{
					proce_flag[4] = !proce_flag[4];
					motor_flag = 4;
					send_lcd_pack(WriteData,0x00080028,proce_flag[4]);
					break;
				}
				case 0x0B://确定 
				{
					//更新自动页面
					int set_i =0;
					int set_j = 0;
					int num = 0;
					while(set_i < 5)//轮询打钩的
					{
						if(proce_flag[set_i] == 1)
						{
							num++;
							proce_number[set_j]= set_i+1;//比如1 3 5操作打钩了  就存储这三个数字代表
							set_j = (set_j + 1)%5;
						}						
						set_i++;
					}
					for(int i = num; i < 5;i++)
					{
						proce_number[i] = 0;
					}
					//1 用定时器中断做时间设置  2 用LCD定时器
					send_lcd_pack(WriteChinese,0x00000080,(u16)proce_number[0]);//正在进行
					send_lcd_pack(WriteChinese,0x00000100,(u16)proce_number[1]);
					break;
				}
				case 0x0A://返回 
				{
					//send_speed_pack(0x00,speed_flag,turn_flag);
					break;
				}
			}
		}
		h_recev_start=(start_points+0x09)&s_limit;
	}
	else 
	  {
			h_recev_start=(start_points+0x01)&s_limit;
		}
	 return h_recev_start;
}
u32 deal_one_pack_down(u32 start_points)//改u32
{
	u8  hx5,ty1,ty2,i,hx2;
	u16 hx1;
	u32 h_recev_start;
 
	
	hx5=USART_RX_BUF_DOWN[start_points+1]; 
	
	if(hx5 == 0x6F)//回应ok 
	{
		 h_recev_start=(start_points+0x09)&s_limit;

	}			
	else if(hx5 == 0x3F)//回应转速信息
	{
		//记录主电机转速信息
		mainmotor[0]=USART_RX_BUF_DOWN[start_points+3]-0x41;  // 
		mainmotor[1]=USART_RX_BUF_DOWN[start_points+4];  //
		mainmotor[2]=USART_RX_BUF_DOWN[start_points+5];  // 
	  mainmotor[3]=USART_RX_BUF_DOWN[start_points+6];  // 		
		mainmotor[4]=USART_RX_BUF_DOWN[start_points+7];  //  		
	  //记录副电机转速信息
		assmotor[0]=USART_RX_BUF_DOWN[start_points+8]-0x41;// 
		assmotor[1]=USART_RX_BUF_DOWN[start_points+9];  //
		assmotor[2]=USART_RX_BUF_DOWN[start_points+10];  // 
	  assmotor[3]=USART_RX_BUF_DOWN[start_points+11];  // 		
		assmotor[4]=USART_RX_BUF_DOWN[start_points+12];  //
		h_recev_start=(start_points+0x09)&s_limit;
	}
	else if(hx5 == 0x66)//故障返回
	{
		for(i = 3;i<11;++i)
		{
			if(USART_RX_BUF_DOWN[i] == 0x31)//有故障
			{
				//发送故障复位信号
				send_speed_pack(RESET,0x00);
				switch(i)
				{
					//电源故障
					case 3:
					{
						//过电压
						break;
					}
					case 4:
					{
						//欠电压
						break;
					}
					//主电机故障信息
					case 5:
					{
						//过流
						break;
					}
					case 6:
					{
						//霍尔
						break;
					}
					case 7:
					{
						//保留
						break;
					}
					//副电机故障信息
					case 8:
					{
						//过流
						break;
					}
					case 9:
					{
						//霍尔
						break;
					}
					case 10:
					{
						//保留
						break;
					}
				}
			}
		}
		h_recev_start=(start_points+0x01)&s_limit;
	}
	else if(hx5 == 0x88)//预留接口
	{
	
	}
	else 
	{
			h_recev_start=(start_points+0x01)&s_limit;
	}
	 return h_recev_start;
}
//组织要回复的内容 
//grade :grade_return and grade_WO回复指令和主动上传
//return_tcp_sta:发生的事件
//spkn:发送包号
u8 send_lcd_pack(u8 grade,u32 address,u16 da)
{
	u8 valid_flag,len=0,sendlen=0,sendflag=0,i=0;
	valid_flag=0;
	u8 j=0;
	 if(grade==WriteChinese)   //写汉字
	  { 
			USART_TX_BUF[1]=0x42;
				if(address==0x00000080)		//正在进行中：
				{					
					USART_TX_BUF[1]=0x42;
					switch(da)
					{
						 case 0://无操作
						 {
							 for(i = 6 ; i<13; i++)
								{
									USART_TX_BUF[i]=no_opera[j++];
								}
						   valid_flag=1;
						 					len=7;
							 break;
						 }			
						 case 1://和面勾快速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_fast[j++];
								}
                  valid_flag=1;
						 			len=11;
							 break;
						}
						 case 2://和面勾慢速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_slow[j++];
								}
						      valid_flag=1;
						 			len=11;
							 break;
						}
						 case 3://暂停
						 {
							 for(i = 6 ; i<11; i++)
								{
									USART_TX_BUF[i]=stop[j++];
								}
						      valid_flag=1;
						 					len=5;
							 break;
						 }
						 case 4: //和面桶快速
						 {
								 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_fast[j++];
								}
						      valid_flag=1;
						 					len=11;
							 break;
							}
						 case 5://和面桶慢速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_slow[j++];
								}
						   valid_flag=1;
						 					len=11;
							 break;
						 }
						 case 6://无操作
						 {
							 for(i = 6 ; i<13; i++)
								{
									USART_TX_BUF[i]=no_opera[j++];
								}
						   valid_flag=1;
						 					len=7;
							 break;
						 }					 
							default :break;							
					}			 
				}
				else if(address==0x00000100)		//下一操作
				{		 
					USART_TX_BUF[1]=0x42;					 
					switch(da)
					{
						case 0://无操作
						 {
							 for(i = 6 ; i<13; i++)
								{
									USART_TX_BUF[i]=no_opera[j++];
								}
						   valid_flag=1;
						 					len=7;
							 break;
						 }		
						 case 1://和面勾快速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_fast[j++];
								}
                  valid_flag=1;
						 			len=11;
							 break;
						}
						 case 2://和面勾慢速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_slow[j++];
								}
						      valid_flag=1;
						 			len=11;
							 break;
						}
						 case 3://暂停
						 {
							 for(i = 6 ; i<11; i++)
								{
									USART_TX_BUF[i]=stop[j++];
								}
						      valid_flag=1;
						 					len=5;
							 break;
						 }
						 case 4: //和面桶快速
						 {
								 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_fast[j++];
								}
						      valid_flag=1;
						 					len=11;
							 break;
							}
						 case 5://和面桶慢速
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_slow[j++];
								}
						   valid_flag=1;
						 					len=11;
							 break;
						 }
							default :break;							
					}			 
				}		
			 
	 
			 else if(address==0x00000180) //网罩开关状态--页面1
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//开
				 {
					USART_TX_BUF[6] = 0xBF;
					USART_TX_BUF[7] = 0xAA;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
				 else if(da == 0)//关
				 {
					USART_TX_BUF[6] = 0xB9;
					USART_TX_BUF[7] = 0xD8;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
			 }
			 else if(address==0x00000280)//和面勾转向--页面1
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//正转
				 {
					USART_TX_BUF[6] = 0xD5;
					USART_TX_BUF[7] = 0xFD;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 5;
					 valid_flag=1;
				 }
				 else if(da == 0)//反转
				 {
					USART_TX_BUF[6] = 0xB7;
					USART_TX_BUF[7] = 0xB4;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 5;
					 valid_flag=1;
				 }
			 }
			  else if(address==0x00000300) //网罩开关状态--页面2
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//开
				 {
					USART_TX_BUF[6] = 0xBF;
					USART_TX_BUF[7] = 0xAA;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
				 else if(da == 0)//关
				 {
					USART_TX_BUF[6] = 0xB9;
					USART_TX_BUF[7] = 0xD8;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
			 }
			 else if(address==0x00000380)//和面勾转向--页面2
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//正转
				 {
					USART_TX_BUF[6] = 0xD5;
					USART_TX_BUF[7] = 0xFD;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 5;
					 valid_flag=1;
				 }
				 else if(da == 0)//反转
				 {
					USART_TX_BUF[6] = 0xB7;
					USART_TX_BUF[7] = 0xB4;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
			 }
		}	
		else if(grade==WriteData) //写数据  //可简化  case内容相同
		{
			USART_TX_BUF[1]=0x3D;
			switch(address)
			{
				//-----------页面1--手动
					 case  0x00080000://面团温度
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000E://快速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080010://慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008002A://快速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008002C://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008002E://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080030://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080032://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080034://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080036://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080038://慢速 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					//--------------页面2---自动
					 case  0x00080002://面团温度
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 //-----------页面三---程序设置
					 case  0x00080004://时间 和面勾快速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080006://时间 和面勾慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080008://时间 暂停
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000A://时间 和面桶快速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000C://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080020://打钩 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080022://打钩 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080024://打钩 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080026://打钩 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080028://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008003A://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008003C://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008003E://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080040://时间 和面桶慢速
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					default : break;					
			}	
		}
		else if(grade==WriteTime)	//定时器赋值
		{
				if(address==0x0002FFE0)  //定时器0
				{
									USART_TX_BUF[1]=0x44;
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;//u16时间总秒数右移 8位  即取高八位
									USART_TX_BUF[9]=(u8)da;//取低八位

					        len=4;
					        valid_flag=1;
				}
				else if(address==0x0002FFE4) //定时器1
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFE8) //定时器2
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFEC) //定时器3
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFF0) //定时器4
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFF4) //定时器5
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
		}	
		else if(grade==EnableTime)//定时器启动
		{
	       if(address==0x00FFFF00)//定时器0
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF01)//1
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF02)//2
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF03)//3
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF04)//4
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF05)//5
				 {
					 if(da==0x01) //倒计时
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//倒计时关闭
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
    }
		else if(grade==ChangePG)//页面显示 改变页码
		{
       	USART_TX_BUF[1]=0x70;  
		    valid_flag=2;		
		}
		if(valid_flag==1)
		 {
					valid_flag=0;
					USART_TX_BUF[0]=0xAA;  		
		 
					USART_TX_BUF[2]=(u8)((address&0xff000000)>>24); //地址
					USART_TX_BUF[3]=(u8)(((address&0x00ff0000)>>16));
					USART_TX_BUF[4]=(u8)(((address&0x0000ff00)>>8));
					USART_TX_BUF[5]=(u8)((address&0x000000ff));
			       
					USART_TX_BUF[6+len]=0xCC;
					USART_TX_BUF[7+len]=0x33;
					USART_TX_BUF[8+len]=0xC3;
					USART_TX_BUF[9+len]=0x3C;
					sendlen=len+10;
					sendflag=1;
		 }
		 else if(valid_flag==2)
		 {
			   	USART_TX_BUF[0]=0xAA;  
			    USART_TX_BUF[2]=da>>8; //页面序号
					USART_TX_BUF[3]=(u8)da;
			 		USART_TX_BUF[4]=0xCC;
					USART_TX_BUF[5]=0x33;
					USART_TX_BUF[6]=0xC3;
					USART_TX_BUF[7]=0x3C;
					sendlen=8;
					sendflag=1;
		 }

		 if(sendflag==1)//发送
		 {
				sendflag=0;
			 
			 //串口发送函数
			 for(i=0;i<sendlen;i++)
			 	{
					U0C0_0_UART_SendData(USART_TX_BUF[i]);
				}
		 }
	return 0;
}
u8 buf[15];
//和面勾是主电机 和面桶是副电机
u8 send_speed_pack(u8 flag,u8 speed_flag)
{
			 u8 i= 0;
			 int len;
			 int sum = 0;
			 u8 sum_add;
			 u8 wxt_low;
			 buf[1] = flag;//ASK SET RESET	
			 switch(flag)
			 {
				 case SET://设定转速
				 {
					 len = 15;
					 if(speed_flag == stop_speed_flag)//暂停
				   {
						buf[2] = mainmotor[0]+0x41;//主电机 41正转 42反转
					 //界面只有时间参数 没有时间与主从电机关系函数
					  buf[3] = 0x30;
					  buf[4] = 0x30;
				 	  buf[5] = 0x30;
				    buf[6] = 0x30;
				    buf[7] = assmotor[0]+0x41;//副电机
					  buf[8] = 0x30;
					  buf[9] = 0x30;
				 	  buf[10] = 0x30;
				    buf[11] = 0x30;
						for(i = 1;i<= 11;++i)
						{
								 sum +=buf[i];
						}
							 sum_add = (u8)sum;
							 //wxt_low = 0x0f&sum_add;
							 buf[12] = (u8)(sum_add>>4)+0x30;
							 buf[13] = (0x0f&sum_add)+0x30;
							 buf[14] = 0x0D;
					 }
					 else if(speed_flag == 0x00)
					 {
							 len = 15;
							 //界面只有桶点动
							 buf[2] =  mainmotor[0]+0x41;//主电机 41正转 42反转
							 //界面只有时间参数 没有时间与主从电机关系函数
							 buf[3] =mainmotor[1]+0x30;
							 buf[4] =mainmotor[2]+0x30;
							 buf[5] =mainmotor[3]+0x30;
							 buf[6] =mainmotor[4]+0x30;
							 buf[7] =assmotor[0]+0x41;//副电机
							 buf[8] =assmotor[1]+0x30;
							 buf[9] =assmotor[2]+0x30;
							 buf[10]=assmotor[3]+0x30;
							 buf[11]=assmotor[4]+0x30;
						 
						   for(i = 1; i <= 11;i++)
							 {
								 sum += buf[i];
							 }
							 sum_add = (u8)sum;
							 
							 buf[12] = (u8)(sum_add>>4)+0x30;
							 buf[13] = (0x0f&sum_add)+0x30;
							 buf[14] = 0x0D;
					 }
					 
					 
				   break;
				 }
				 case ASK://询问
				 {
					 buf[1] = ASK;//ASK SET RESET	
					 buf[2] = speed_flag;
					 len = 6;
					 if(speed_flag == 0x72)//询问转速
					 {					 					  
						  					 
						 buf[3] = 0x42;
						 buf[4] = 0x31;
						 buf[5] = 0x0D;
 
					 }
					 else if(speed_flag == 0x66)//询问故障
					 {
						 buf[3] = 0x41;
						 buf[4] = 0x35;
						 buf[5] = 0x0D;
					 }
					 else if(speed_flag == 0x69)//询问信息
					 {
						 buf[3] = 0x41;
						 buf[4] = 0x38;
						 buf[5] = 0x0D;
					 }
					 break;
				 }
				 case RESET://故障复位
				 {
					 buf[2] = 0x36;
					 buf[3] = 0x33;
					 buf[4] = 0x0D;		
					 len = 5;
					 break;
				 }
				 default:
					 break;		 
			 }
	     buf[0]=0x7E; 			  			 
			 for(i=0;i<len;i++)
			 {
							U0C0_UART_SendData(buf[i]);	
			 }
			 return 0;
}
void temperature_DS(void)
{
//	    temperature=read_temperature();	 //十倍温度值
 		 	temperature=DS18B20_Get_Temp();	 //十倍温度值
			if((	temperature%10)>=5)
				temperature= (temperature+10)/10;
			else
				temperature= temperature/10;
			send_lcd_pack(WriteData,0x00080800,temperature);	
 
}
 
//组织下位机发送包
void Send_Turnspeed_pack(u16  turn_speed4[],u8 Flag)
{
	            if(Flag == 0x02)//停止
							{
								    Speed_DATA[0] = 0x00;
										Speed_DATA[1] = 0x00; 
							      Speed_DATA[2] = 0x00;
										Speed_DATA[3] = 0x00;
							}
							else
							{
	                  Speed_DATA[0] = turn_speed4[1];//转速获取
										Speed_DATA[1] = 0x00; 
							      Speed_DATA[2] = 0x00;
										Speed_DATA[3] = 0x00;
							}
						 
										databuf[1]=Flag;//00 正转  01逆转 02 停止
										
										CRC16 = CRC_CHECK(Speed_DATA,8);
										Speed_DATA[2] = CRC16%256;
										Speed_DATA[3] = CRC16/256;
							  
							     for(i=2,j=0;i<6;i++)
										databuf[i]=Speed_DATA[j++];
							 
                  //send_speed_pack(databuf);
}
void Ask_Turn_speed()//询问转速
{
	send_speed_pack(ASK,SET);	
}
void Ask_Error()//询问故障 有故障就复位
{
	send_speed_pack(ASK,ERROR);
}
int count = 0,count_sec = 0;
void Ask_Down_data()
{
			 if(count%1000 == 0)
			 {
				 if(count_sec%100 == 0)
				 {
						Ask_Turn_speed();//询问转速
						Ask_Error();//询问故障 有故障就复位
					  //send_speed_pack(RESET,0x00);
				 }
				 if(count_sec < 50000) count_sec++;
				 else count_sec = 0;
			 }
			 if(count < 50000) count++;
			 else count = 0;
}