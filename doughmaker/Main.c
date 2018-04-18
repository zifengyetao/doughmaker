#include <XMC1100.h>		//SFR declarations of the selected device
#include <gpio_xmc1100_tssop38.h>
#include "MCUInit.h"
#include "datatransfer.h"	
#include  "record_map.h"
#include "24cxx.h"
#include "ds18b20.h"
#include "UART.h"
#include "delay.h"
#include "WDT.h"

u8 write_test[1] = {15};
u8 read_test[11] = {0x72,0x41,0x31,0x35,0x30,0x30,0x42,0x30,0x35,0x30,0x30};
extern int i;
int sum_ADD = 0;
int wxt;
u8 sumsum;
int main(void)
{  
	 MCU_vInit();     //MCU 初始化
	 AT24CXX_Init();
	 LCD_EE_Init();
	 Lcd_Show_Init();
	 LCD_EE_Init();
	 DS18B20_Init();//暂时没有传感器 但是已调通
	 WDT_vInit();
		  
	 while(1)
	 {			  
			 DealLcdMsg();	//触摸屏 
			 Deal_Down_Msg();//下面单片机串口
		   //定时询问故障
			 send_speed_pack(ASK,0x3f);
			 WDT_Clear();
	 }
}
