#ifndef __DATATRANSFER_H
#define __DATATRANSFER_H	
//#include "sys.h"
#include "stdint.h"

 #define u16 uint16_t
 #define u8 uint8_t
 #define u32 uint32_t
 
 


#define hand 1
#define automatic 2

 
void Lcd_Show_Init(void);
void manage_one_pack(void);
void manage_one_down_pack(void);
void DealLcdMsg(void);
void Deal_Down_Msg(void);
void LCD_EE_Init(void);
void show_init(void);
void Show_Check(void);
void Deal_Lcd_Msg(void);
void u8_to_u16(void);
void temperature_DS(void);
void u8_to_u16_flash(void);
void USIC0_Init(void);
 
u32 deal_one_pack(u32 start_points);
 
u8 send_lcd_pack(u8 grade,u32 address,u16 da );
u8 send_speed_pack(u8 flag,u8 speed_flag,u8 turn_flag);
void  Running_Show(u16 Mode_number,u16 num1);
void  Running_Show_Page3(u16 Mode_number,u16 num1);
void Get_Turn_Speed( u16 mode_now);
void Send_Turnspeed_pack(u16 turn_speed4[],u8 Flag);   

u8 send_lcd_pack_time(u8 grade,u32 address,u32 da);

#define	WriteChinese	0x42
#define	WriteData	    0x3D
#define	WriteTime	    0x44
#define	EnableTime	  0x3B
#define ChangePG	  0x70
#define ReadData      0x3E  
#define ReadData_String     0x43
 
#define type_num  3+1
#define data_max_num 24+1

 

 
#define   BUF_RX_6_7        USART_RX_BUF[(start_points+7)]  // USART_RX_BUF[(start_points+6)]*256+
#define 	BUF_TX_6_7_2		{USART_TX_BUF[6]=da>>8;USART_TX_BUF[7]=(u8)da;len=2;valid_flag=1;}					
 

 
#endif
