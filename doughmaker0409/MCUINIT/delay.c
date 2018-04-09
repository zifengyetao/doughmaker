#include "datatransfer.h"
#include "delay.h"
#include  "xmc1100.h"

void delay_us(int num) //us延时
{
	   for(int i=0; i<num; i++)
			{
		    __NOP();
 
			}
}

void  delay_ms(int num)//ms延时  
{
	for(int i =0 ; i<num; i++)
	{
			for(int j =0; j<1000;j++)
			{
					delay_us(1);
			}
  }
}
 
 void delay_us_1()
 {
	// Start Timer
  	WR_REG(CCU40_CC42->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//启动定时器CCU40;   
 }
 
 void delay_us_xmc(int num)
 {
		for(int i = 0 ;i < num ; i++)
	 {
		    delay_us_1();
	 }
 }
 
 