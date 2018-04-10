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
u8  time_ee = 0;

u16  EE_ALL_DATA[type_num][data_max_num]={0};
extern u8  EE_ALL_DATA_test[type_num][data_max_num]; 

 

 void show_init()//显示页面一的图标  显示页面二的图标 
 {
 
//	 for(i =0;i<=10; i++)
//	 {
//			switch(EE_ALL_DATA[0][i])
//			{
//				case 1: 
//											choose2flag =1;	
//											Page1_control[i] =1;
//										  send_lcd_pack(WriteData,0x0008001A,0x0001);////页面二图标显示
//											send_lcd_pack(WriteData,(0x00080004+(2*i)),0x0001);//页面一图标显示   面条										
//											break;
//				case 2: 
//											choose3flag =1;		
//											Page1_control[i] =2;
//											send_lcd_pack(WriteData,0x0008001C,0x0001);////页面二图标显示   包子
//			              	send_lcd_pack(WriteData,(0x00080004+(2*i)),0x0002);															
//											break;
//				case 3:   
//											choose1flag =1;				
//											Page1_control[i] =3;
//											send_lcd_pack(WriteData,0x00080018,0x0001);////页面二图标显示    水饺
//											send_lcd_pack(WriteData,(0x00080004+(2*i)),0x0003); //必须放到页面二 显示之后											
//											break;
//				default :break;
//			}
//	 }
		 
 }
 
 
void LCD_EE_Init()
{
		AT24CXX_Init();
		AT24CXX_Read(Page1_start_pointer,EE_ALL_DATA_test[0],10);//页面一 十个位置地址  读入第一行
		AT24CXX_Read(Page5_nodles_slowmix_start_pointer,EE_ALL_DATA_test[1],21);//面条自定义所有数据读入第二行
		AT24CXX_Read(Page5_bun_slowmix_start_pointer,EE_ALL_DATA_test[2],21);//包子自定义所有数据读入第二行
		AT24CXX_Read(Page5_dump_slowmix_start_pointer,EE_ALL_DATA_test[3],21);//水饺自定义所有数据读入第二行
    u8_to_u16();
}

void Lcd_Show_Init()
{
	//----------------页面1 手动
		send_lcd_pack(WriteData,0x00080000,0x0020);//面团温度已完成
	
		send_lcd_pack(WriteTime,0x0002FFE0,0x0000);//剩余时间
		send_lcd_pack(EnableTime,0x00FFFF00,0x00);//关闭定时器0 倒计时0x00
		
		send_lcd_pack(WriteData,0x0008000E,0x0000);// 快速时间
		send_lcd_pack(WriteData,0x00080010,0x0000);// 慢速时间
		
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
		      send_lcd_pack(WriteData,0x00080004,0x0001);//时间 
					send_lcd_pack(WriteData,0x00080006,0x0001);// 
					send_lcd_pack(WriteData,0x00080008,0x0001);// 
					send_lcd_pack(WriteData,0x0008000A,0x0001);// 
					send_lcd_pack(WriteData,0x0008000C,0x0001);// 
					
					send_lcd_pack(WriteData,0x00080020,0x0001);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080022,0x0000);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080024,0x0001);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080026,0x0000);//打钩 1是打钩 0是不打钩
					send_lcd_pack(WriteData,0x00080028,0x0001);//打钩 1是打钩 0是不打钩

	
		send_lcd_pack(WriteTime,0x0002FFE0,0x0000);//赋值
 		send_lcd_pack(WriteTime,0x0002FFE4,0x0020);//赋值
		send_lcd_pack(WriteTime,0x0002FFE8,0x0020);
		send_lcd_pack(WriteTime,0x0002FFEC,0x0020);
		send_lcd_pack(WriteTime,0x0002FFF0,0x0020);
		send_lcd_pack(WriteTime,0x0002FFF4,0x0020);
		
		send_lcd_pack(EnableTime,0x00FFFF00,0x00);//关闭
  	send_lcd_pack(EnableTime,0x00FFFF01,0x00);//关闭
		send_lcd_pack(EnableTime,0x00FFFF02,0x00);//关闭
		send_lcd_pack(EnableTime,0x00FFFF03,0x00);
		send_lcd_pack(EnableTime,0x00FFFF04,0x00);
		send_lcd_pack(EnableTime,0x00FFFF05,0x00);
		
}
void  u8_to_u16()
	{
		   	for(i=0;i<4;i++)
					{
						for(j = 0;j<25;j++)
						{
												EE_ALL_DATA[i][j] = EE_ALL_DATA_test[i][j] &0xffff;
						}
					}
	}
	
 

 





