 


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


//程序EEPROM部分暂时没来得及写

int main(void)
{  
	 MCU_vInit();     //MCU 初始化
	 //LCD_EE_Init();
   //show_init();
	 Lcd_Show_Init();
	 DS18B20_Init();//暂时没有传感器  但是已调通
//	 WDT_vInit();
	 while(1)
	{
 
	  DealLcdMsg();	//触摸屏
	  //Deal_Down_Msg();//下面单片机串口
		//WDT_Clear();
	}
}
