#ifndef __DS18B20_H
#define __DS18B20_H 
//#include "sys.h"   
#include "datatransfer.h"
#include <gpio_xmc1100_tssop38.h>
#include <xmc1100.h>
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
#define uchar unsigned char   
#define uint  unsigned int 
//IO��������
#define DS18B20_IO_IN()     P2_0_set_mode(INPUT_PU)	//PG9����ģʽ   P2.3 ��ת��Ϊ��������
#define DS18B20_IO_OUT()     P2_0_set_mode(OUTPUT_PP_GP) 	//PG9���ģʽ
 
////IO��������											   
//#define	DS18B20_DQ_OUT PGout(9) //���ݶ˿�	PG9
//#define	DS18B20_DQ_IN  PGin(9)  //���ݶ˿�	PG9 
   	
#define  DS18B20_DQ_OUT_0		P2_0_reset() 
#define  DS18B20_DQ_OUT_1		P2_0_set() 

#define  DS18B20_DQ_IN 		P2_0_read()
 
		
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		      //��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    


#endif















