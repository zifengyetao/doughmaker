#ifndef __DELAY_H
#define __DELAY_H	
#include "datatransfer.h"

void delay_us(int num); //us��ʱ
void delay_us_1(void); //us��ʱ
void delay_us_xmc(int num); //us��ʱ
void delay_ms(int num);//ms��ʱ  


void delay_ms_1(int num);//ms��ʱ  
void  delay_us_50(int num);//ms��ʱ  
	void  delay_us_60(int num);//ms��ʱ  
		void  delay_us_2(int num);//ms��ʱ  
		
 
			
void CNTADY_Delay(int sencond, int ms, int us, int ns); // Max 3s
void CNTADY_Delay(int sencond, int ms, int us, int ns); // Max 3s

#endif

