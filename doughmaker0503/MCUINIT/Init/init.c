#include "init.h"
#include "24cxx.h"
#include  "record_map.h"
#include  "datatransfer.h"
#include "stdbool.h"
#include "myiic.h"
#include "delay.h"

 unsigned char Test_Data, EE_ReadData[256],EE_WriteData[256],EE_WriteData1[256];
 unsigned char *p;
u16  EE_ReadData_16[256];

bool choose1flag=0,choose2flag=0,choose3flag=0;

extern u16 i;
extern u16 j;
u8 Page1_control[10];
  
u16  timehour=0,timemin=0,timesec=0,pretimehour=0,pretimemin=0,pretimesec=0,pre_turnspeed=0;
u16  turn_speed4[2];
u8 ee_flag;
u8 time_ee = 0;

u8 Hand_Read_data[4],Auto_Read_data[10];
u8 EE_Flag;//EEPROM读写flag
void LCD_EE_Init()
{		
		 //AT24CXX_Write(EE_Flag_pointer,&EE_Flag,1);
		 
			AT24CXX_Read(Hand_page_pointer,Hand_Read_data,4);
			AT24CXX_Read(auto_page_pointer,Auto_Read_data,10);
										 
}

extern int proce_flag[5];
void Lcd_Show_Init()
{
	//----------------页面1 手动
		send_lcd_pack(WriteData,0x00080000,0x0020);//面团温度已完成
	
		send_lcd_pack(WriteTime,0x0002FFE0,0x0000);//剩余时间
		send_lcd_pack(EnableTime,0x00FFFF00,0x00);//关闭定时器0 倒计时0x00
		
		//send_lcd_pack(WriteData,0x0008000E,0x0000);// 快速时间
		//send_lcd_pack(WriteData,0x00080010,0x0000);// 慢速时间
		
			send_lcd_pack(WriteData,0x0008002C,0x0000);//快速图标
			send_lcd_pack(WriteData,0x0008002A,0x0000);//慢速时间
			
			send_lcd_pack(WriteChinese,0x00000180,0x0001);//网罩开关状态  开
			send_lcd_pack(WriteChinese,0x00000280,0x0001);//和面勾转向  --正转
		
//-----------------页面2 自动
				send_lcd_pack(WriteData,0x00080002,0x0020);//面团温度 
				send_lcd_pack(WriteChinese,0x00000300,0x0001);//网罩开关状态  开
				send_lcd_pack(WriteChinese,0x00000380,0x0001);//和面勾转向  ---正转
				send_lcd_pack(WriteChinese,0x00000080,0x0000);//正在进行中
				send_lcd_pack(WriteChinese,0x00000100,0x0001);//下一操作
//-----------------页面三
		      //send_lcd_pack(WriteData,0x00080004,0x0001);//时间 
					//send_lcd_pack(WriteData,0x00080006,0x0001);// 
					//send_lcd_pack(WriteData,0x00080008,0x0001);// 
					//send_lcd_pack(WriteData,0x0008000A,0x0001);// 
					//send_lcd_pack(WriteData,0x0008000C,0x0001);// 
					
					send_lcd_pack(WriteData,0x00080020,0x0001);//打钩 1是打钩 0是不打钩
					proce_flag[0] = 1;
					send_lcd_pack(WriteData,0x00080022,0x0000);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080024,0x0001);//打钩 1是打钩 0是不打钩
					proce_flag[2] = 1;
					send_lcd_pack(WriteData,0x00080026,0x0000);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080028,0x0001);//打钩 1是打钩 0是不打钩
					proce_flag[4] = 1;

	
		send_lcd_pack(WriteTime,0x0002FFE0,0x0000);//赋值
 		
		
		send_lcd_pack(EnableTime,0x00FFFF00,0x00);//关闭
  	send_lcd_pack(EnableTime,0x00FFFF01,0x00);//关闭
		send_lcd_pack(EnableTime,0x00FFFF02,0x00);//关闭
		send_lcd_pack(EnableTime,0x00FFFF03,0x00);
		send_lcd_pack(EnableTime,0x00FFFF04,0x00);
		send_lcd_pack(EnableTime,0x00FFFF05,0x00);
		
}
	
 

 





