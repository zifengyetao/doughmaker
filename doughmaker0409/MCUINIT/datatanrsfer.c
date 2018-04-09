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
#include  "record_map.h"
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


extern u8 USART_RX_BUF[1024];
extern u32 recev_start,recev_end;
extern u32 recev_start_down,recev_end_down;
 

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
 
////////�������-0204
u8  key_number[2];// 
u8  Time_data_number[7];//0 1 2 3 4 5 6����Ϊ�Զ��ֶ�����ҳ������ת��
int stop_flag = 0;
int speed_flag = 0;
int slow_flag = 0;
int turn_flag = 0;//������ת Ĭ����ת
int proce_flag[5];//��������ҳ��5��flag
int Time_Flag[6];//������ʱ��flag
int proce_number[5];
int Now_Num,Next_Num;
int fast_slow_flag;
int page_flag;//
int motor_flag;
 //�����յ��Ĵ������Ĵ�����Ϣ
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
					 //��ʱ��
					 if((USART_RX_BUF[m_start+10]==0xCC)&&(USART_RX_BUF[m_start+11]==0x33)&&  
						  (USART_RX_BUF[m_start+12]==0xC3)&& (USART_RX_BUF[m_start+13]==0x3C))
					 {
						    recev_start=deal_one_pack(m_start);
						 		mm_ok=1;		 						 
					 }
           else
					 {  mm_ok=2; //���ݲ���						 
					 }
        }
        else
				{  	
					mm_ok=3;//���ݳ��Ȳ���
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
						else mm_ok=2;// ����β�β���
			  }
			  else mm_ok=3;//���ݰ����������˳��ȴ�
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
	
	if((mm_ok==0)||(mm_ok==2))//����
	{
		recev_start=m_end;
	}
}	
 
//�����յ�����λ���Ĵ�����Ϣ
void manage_one_down_pack(void)
{
	u8 databuf_test[9] = {0x00,0x01,0x02,0x05,0x03F,0x55,0xcc,0x33,0xCD};
	;//��������
	 //send_speed_pack(databuf_test);
	 recev_start_down = recev_end_down;
}
//�˺�����Ҫ���ڷ�����λ������������ 
//���ҽ�����Ӧ����
//������֯������ʽ
u32 deal_one_pack(u32 start_points)//��u32
{
	u8  hx5,ty1,ty2,i,hx2;
	u16 hx1;
	u32 h_recev_start;
 
	
	hx5=USART_RX_BUF[start_points+1]; 
	
	if(hx5==0x77)//��������
	{
		if((USART_RX_BUF[(start_points+2)&s_limit]==0x00)&&(USART_RX_BUF[(start_points+3)&s_limit]==0x08))    // 
		{		
       key_number[0]= (USART_RX_BUF[(start_points+6)]);
			 key_number[1]= (USART_RX_BUF[(start_points+7)]);//��ʱֻ����С��ֵ			
			 hx1=(USART_RX_BUF[(start_points+5)]);       //�����ַ 
				switch(hx1)//ʱ��23����
				{
					//----ҳ��1
					case 0x12://����
					{
						if(fast_slow_flag == 0)
						{
							Time_data_number[0] = key_number[1];
							send_lcd_pack(WriteData,0x0008000E,key_number[1]);// ����
							speed_flag = 1;//����
						}
						else key_number[1] = 0;	
						break;
					}
					case 0x14://����
					{
						if(fast_slow_flag == 1)
						{
							Time_data_number[1] = key_number[1];
							send_lcd_pack(WriteData,0x00080010,key_number[1]);// ����;
							speed_flag = 0;//���� 
						}
						else key_number[1] = 0;	
						break;
					}
					//----ҳ��3
					case 0x16://���湴����
					{
							Time_data_number[2] = key_number[1];
							send_lcd_pack(WriteData,0x00080004,key_number[1]);
							send_lcd_pack(WriteTime,0x0002FFE4,key_number[1]);
							break;
					}
					case 0x18://���湴����
					{
						Time_data_number[3] = key_number[1];
						send_lcd_pack(WriteData,0x00080006,key_number[1]);//
						send_lcd_pack(WriteTime,0x0002FFE8,key_number[1]);						
						break;
					}
					case 0x1A://��ͣ
					{
						Time_data_number[4] = key_number[1];
						send_lcd_pack(WriteData,0x00080008,key_number[1]);// ;
						send_lcd_pack(WriteTime,0x0002FFEC,key_number[1]);	
						break;
					}
					case 0x1C://����Ͱ����
					{
						Time_data_number[5] = key_number[1];
						send_lcd_pack(WriteData,0x0008000A,key_number[1]);//;
						send_lcd_pack(WriteTime,0x0002FFF0,key_number[1]);	
						break;
					}
					case 0x1E://����Ͱ����
					{
						Time_data_number[6] = key_number[1];
						send_lcd_pack(WriteData,0x0008000C,key_number[1]);//;
						send_lcd_pack(WriteTime,0x0002FFF4,key_number[1]);	
						break;
					}
					default:
							break;
				}
			
		h_recev_start=(start_points+12);		
		}
	  else if((USART_RX_BUF[(start_points+2)&s_limit]==0x00)&&(USART_RX_BUF[(start_points+3)&s_limit]==0x02)&&
			   (USART_RX_BUF[(start_points+4)&s_limit]==0xFF))    //ʱ������ظ�   ��������ʱ����ʱʱ�����
		{			
					ty1 = USART_RX_BUF[start_points+5];
					switch(ty1)
					{
						case 0xE0://��ʱ��0
						{
							Time_Flag[0] = !Time_Flag[0];
							send_lcd_pack(EnableTime,0x00FFFF00,0x00);//  ���ܶ�ʱ��0 
							send_speed_pack(0x01,speed_flag,turn_flag);//ֹͣ����
							break;
						}
						case 0xE4://��ʱ��1
						{
							Time_Flag[1] = !Time_Flag[1];
							send_lcd_pack(EnableTime,0x00FFFF01,0x00);//  ���ܶ�ʱ��1
							send_lcd_pack(EnableTime,0x00FFFF02,0x01);//  ������ʱ��1
							send_speed_pack(0x00,speed_flag,turn_flag);//
							break;
						}
						case 0xE8:
						{
							Time_Flag[2] = !Time_Flag[2];
							send_lcd_pack(EnableTime,0x00FFFF02,0x00);//  ���ܶ�ʱ��2
							send_lcd_pack(EnableTime,0x00FFFF03,0x01);//  ������ʱ��2
							send_speed_pack(0x00,speed_flag,turn_flag);//							
							break;
						}
						case 0xF0:
						{
							Time_Flag[3] = !Time_Flag[3];
							send_lcd_pack(EnableTime,0x00FFFF03,0x00);//  ���ܶ�ʱ��3 
							send_lcd_pack(EnableTime,0x00FFFF04,0x01);//  ������ʱ��3 
							send_speed_pack(0x01,speed_flag,turn_flag);//ֹͣ
							break;
						}
						case 0xF4:
						{
							Time_Flag[4] = !Time_Flag[4];
							send_lcd_pack(EnableTime,0x00FFFF04,0x00);//  ���ܶ�ʱ��4
							send_lcd_pack(EnableTime,0x00FFFF05,0x01);//  ������ʱ��4 
							send_speed_pack(0x00,speed_flag,turn_flag);//							
							break;
						}
						case 0xF8:
						{
							Time_Flag[5] = !Time_Flag[5];
							send_lcd_pack(EnableTime,0x00FFFF05,0x00);//  ���ܶ�ʱ��5
							send_speed_pack(0x01,speed_flag,turn_flag);//ֹͣ
							break;
						}
					}	
    }

	}			
	else if(hx5==0x78)//��������ֵ
	{
		ty1=USART_RX_BUF[start_points+3];  //ҳ�����
		ty2=USART_RX_BUF[start_points+4];  //�������
	  if(ty1 == 0x00)		//��Դ����
		{
				switch(ty2)
				{
					case 0x00://�Զ�
					{
						  page_flag = automatic;	
							send_lcd_pack(WriteChinese,0x00000080,0x0000);
							send_lcd_pack(WriteChinese,0x00000100,0x0000);
							send_lcd_pack(WriteData,0x00080002,0x0020);
							send_lcd_pack(WriteChinese,0x00000300,0x0001);//���ֿ���״̬  ��
        			send_lcd_pack(WriteChinese,0x00000380,0x0001);//���湴ת��  ---��ת
							break;	
					}
					case 0x01://�ֶ�
					{
						  page_flag = hand;
							break;
					}
				}
		}
	  else if(ty1==0x01)// �ֶ�
		{ 
			  
				switch(ty2)
				{
					case 0x03://����
					{
						//���������ָ��
					 
							send_speed_pack(0x00,speed_flag,turn_flag);// 
						//��ʱ��
						if(stop_flag == 0)//������ͣ״̬
						{
							send_lcd_pack(WriteTime,0x0002FFE0,key_number[1]*60);//�����200 = 00.08.32
						}
						else
						{
								stop_flag = 0;
						}
						send_lcd_pack(EnableTime,0x00FFFF00,0x01);//ʹ�ܶ�ʱ��0 ����ʱ0x01
						
						send_lcd_pack(WriteChinese,0x00000180,0x0001);//���ֿ���״̬  ��
						send_lcd_pack(WriteChinese,0x00000280,0x0001);//���湴ת��  --��ת
						break;
					}
					case 0x04://��ͣ
					{
						//���������ָ��
					 
	          send_speed_pack(0x01,speed_flag,turn_flag);// 
						send_lcd_pack(EnableTime,0x00FFFF00,0x00);//�رն�ʱ��0 ����ʱ0x01
						stop_flag = 1;//��ͣflag
						break;
					}
					case 0x07://����ת
					{
						if(turn_flag == 1)//�����Ƿ�ת
						{
							turn_flag = 0;
							send_speed_pack(0x02,speed_flag,turn_flag);// 
							send_lcd_pack(WriteChinese,0x00000280,0x0001);
						}	
						break;
					}
					case 0x08://����ת
					{
						if(turn_flag == 0)//��������ת
						{
							turn_flag = 1;
							send_speed_pack(0x02,speed_flag,turn_flag);//												
						}
						send_lcd_pack(WriteChinese,0x00000280,0x0000);	
						break;
					}
					case 0x05://Ͱ�㶯
					{
						send_speed_pack(0x02,speed_flag,0x01);
						break;
					}
					case 0x06://Ͱ��ת
					{
						send_speed_pack(0x02,speed_flag,0x00);
						break;
					}
					case 0x00://����ͼ�� 
					{
						fast_slow_flag = 0;
						send_lcd_pack(WriteData,0x0008002A,0x0001);
						send_lcd_pack(WriteData,0x0008002C,0x0000);
						break;
					}
					case 0x0A://����ͼ��
					{
						fast_slow_flag = 1;
						send_lcd_pack(WriteData,0x0008002C,0x0001);
						send_lcd_pack(WriteData,0x0008002A,0x0000);
						break;
					}
				}					
    }
		else if(ty1==0x02)//�Զ�
		{
			
			switch(ty2)
			{
				case 0x03://����
				{
					send_speed_pack(0x00,speed_flag,turn_flag);
					Time_Flag[1] = !Time_Flag[1];//��ʱ��1flag   ʱ�䵽��flagȡ��
					send_lcd_pack(EnableTime,0x00FFFF01,Time_Flag[1]);//��ʱ��1����
					break;
				}
				case 0x04://��ͣ
				{
					send_speed_pack(0x01,speed_flag,turn_flag);
					for(int i = 0; i < 5; ++i)
					{
						if(Time_Flag[i] == 1)
						send_lcd_pack(EnableTime,0x00FFFF00+i,0x00);//��ʱ��1�ر�
					}					
					break;
				}
				case 0x05://�㶯
				{
					send_speed_pack(0x02,speed_flag,0x01);
					break;
				}
				case 0x06://��ת
				{
					send_speed_pack(0x02,speed_flag,0x00);
					break;
				}
				case 0x01://����
				{
					send_speed_pack(0x01,speed_flag,turn_flag); 
					break;
				}
			}
			
		}
		else if(ty1==0x03)//��������
		{
			switch(ty2)
			{
				case 0x05://������
				{
					proce_flag[0] = !proce_flag[0];
					motor_flag = 0;
					send_lcd_pack(WriteData,0x00080020,proce_flag[0]);
					break;
				}
				case 0x06://������
				{
					proce_flag[1] = !proce_flag[1];
					motor_flag = 1;
					send_lcd_pack(WriteData,0x00080022,proce_flag[1]);
					break;
				}
				case 0x07://��ͣ
				{
					proce_flag[2] = !proce_flag[2];
					motor_flag = 2;
					send_lcd_pack(WriteData,0x00080024,proce_flag[2]);
					break;
				}
				case 0x08://Ͱ����
				{
					proce_flag[3] = !proce_flag[3];
					motor_flag = 3;
					send_lcd_pack(WriteData,0x00080026,proce_flag[3]);
					break;
				}
				case 0x09://Ͱ����
				{
					proce_flag[4] = !proce_flag[4];
					motor_flag = 4;
					send_lcd_pack(WriteData,0x00080028,proce_flag[4]);
					break;
				}
				case 0x0B://ȷ�� 
				{
					//�����Զ�ҳ��
					int set_i =0;
					int set_j = 0;
					int num = 0;
					while(set_i < 5)//��ѯ�򹳵�
					{
						if(proce_flag[set_i] == 1)
						{
							num++;
							proce_number[set_j]= set_i+1;//����1 3 5��������  �ʹ洢���������ִ���
							set_j = (set_j + 1)%5;
						}						
						set_i++;
					}
					for(int i = num; i < 5;i++)
					{
						proce_number[i] = 0;
					}
					//1 �ö�ʱ���ж���ʱ������  2 ��LCD��ʱ��
					send_lcd_pack(WriteChinese,0x00000080,(u16)proce_number[0]);//���ڽ���
					send_lcd_pack(WriteChinese,0x00000100,(u16)proce_number[1]);
					break;
				}
				case 0x0A://���� 
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
//��֯Ҫ�ظ������� 
//grade :grade_return and grade_WO�ظ�ָ��������ϴ�
//return_tcp_sta:�������¼�
//spkn:���Ͱ���
u8 send_lcd_pack(u8 grade,u32 address,u16 da)
{
	u8 valid_flag,len=0,sendlen=0,sendflag=0,i=0;
	valid_flag=0;
	u8 j=0;
	 if(grade==WriteChinese)   //д����
	  { 
			USART_TX_BUF[1]=0x42;
				if(address==0x00000080)		//���ڽ����У�
				{					
					USART_TX_BUF[1]=0x42;
					switch(da)
					{
						case 0://�޲���
						 {
							 for(i = 6 ; i<13; i++)
								{
									USART_TX_BUF[i]=no_opera[j++];
								}
						   valid_flag=1;
						 					len=7;
							 break;
						 }			
						 case 1://���湴����
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_fast[j++];
								}
                  valid_flag=1;
						 			len=11;
							 break;
						}
						 case 2://���湴����
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_slow[j++];
								}
						      valid_flag=1;
						 			len=11;
							 break;
						}
						 case 3://��ͣ
						 {
							 for(i = 6 ; i<11; i++)
								{
									USART_TX_BUF[i]=stop[j++];
								}
						      valid_flag=1;
						 					len=5;
							 break;
						 }
						 case 4: //����Ͱ����
						 {
								 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_fast[j++];
								}
						      valid_flag=1;
						 					len=11;
							 break;
							}
						 case 5://����Ͱ����
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_slow[j++];
								}
						   valid_flag=1;
						 					len=11;
							 break;
						 }
						 case 6://�޲���
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
				else if(address==0x00000100)		//��һ����
				{		 
					USART_TX_BUF[1]=0x42;					 
					switch(da)
					{
						case 0://�޲���
						 {
							 for(i = 6 ; i<13; i++)
								{
									USART_TX_BUF[i]=no_opera[j++];
								}
						   valid_flag=1;
						 					len=7;
							 break;
						 }		
						 case 1://���湴����
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_fast[j++];
								}
                  valid_flag=1;
						 			len=11;
							 break;
						}
						 case 2://���湴����
						 {
							 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_slow[j++];
								}
						      valid_flag=1;
						 			len=11;
							 break;
						}
						 case 3://��ͣ
						 {
							 for(i = 6 ; i<11; i++)
								{
									USART_TX_BUF[i]=stop[j++];
								}
						      valid_flag=1;
						 					len=5;
							 break;
						 }
						 case 4: //����Ͱ����
						 {
								 for(i = 6 ; i<17; i++)
								{
									USART_TX_BUF[i]=mixflour_keg_fast[j++];
								}
						      valid_flag=1;
						 					len=11;
							 break;
							}
						 case 5://����Ͱ����
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
			 
	 
			 else if(address==0x00000180) //���ֿ���״̬--ҳ��1
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//��
				 {
					USART_TX_BUF[6] = 0xBF;
					USART_TX_BUF[7] = 0xAA;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
				 else if(da == 0)//��
				 {
					USART_TX_BUF[6] = 0xB9;
					USART_TX_BUF[7] = 0xD8;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
			 }
			 else if(address==0x00000280)//���湴ת��--ҳ��1
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//��ת
				 {
					USART_TX_BUF[6] = 0xD5;
					USART_TX_BUF[7] = 0xFD;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 5;
					 valid_flag=1;
				 }
				 else if(da == 0)//��ת
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
			  else if(address==0x00000300) //���ֿ���״̬--ҳ��2
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//��
				 {
					USART_TX_BUF[6] = 0xBF;
					USART_TX_BUF[7] = 0xAA;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
				 else if(da == 0)//��
				 {
					USART_TX_BUF[6] = 0xB9;
					USART_TX_BUF[7] = 0xD8;
					USART_TX_BUF[8] = 0x00;
					 len = 3;
					 valid_flag=1;
				 }
			 }
			 else if(address==0x00000380)//���湴ת��--ҳ��2
			 {
				 USART_TX_BUF[1]=0x42;
				 if(da == 1)//��ת
				 {
					USART_TX_BUF[6] = 0xD5;
					USART_TX_BUF[7] = 0xFD;
					USART_TX_BUF[8] = 0xD7;
					USART_TX_BUF[9] = 0xAA;
					USART_TX_BUF[10] = 0x00;
					 len = 5;
					 valid_flag=1;
				 }
				 else if(da == 0)//��ת
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
		else if(grade==WriteData) //д����  //�ɼ�  case������ͬ
		{
			USART_TX_BUF[1]=0x3D;
			switch(address)
			{
				//-----------ҳ��1--�ֶ�
					 case  0x00080000://�����¶�
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000E://����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080010://����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008002A://���� 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008002C://���� 
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					//--------------ҳ��2---�Զ�
					 case  0x00080002://�����¶�
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 //-----------ҳ����---��������
					 case  0x00080004://ʱ�� ���湴����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080006://ʱ�� ���湴����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080008://ʱ�� ��ͣ
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000A://ʱ�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x0008000C://ʱ�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080020://�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080022://�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080024://�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080026://�� ����Ͱ����
					 {
					    USART_TX_BUF[6]=da>>8;  
						  USART_TX_BUF[7]=(u8)da; 
							len=2;
							valid_flag=1;
							break;	 	
					 }
					 case  0x00080028://ʱ�� ����Ͱ����
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
		else if(grade==WriteTime)	//��ʱ����ֵ
		{
				if(address==0x0002FFE0)  //��ʱ��0
				{
									USART_TX_BUF[1]=0x44;
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;//u16ʱ������������ 8λ  ��ȡ�߰�λ
									USART_TX_BUF[9]=(u8)da;//ȡ�Ͱ�λ

					        len=4;
					        valid_flag=1;
				}
				else if(address==0x0002FFE4) //��ʱ��1
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFE8) //��ʱ��2
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFEC) //��ʱ��3
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFF0) //��ʱ��4
				{	 	
									USART_TX_BUF[1]=0x44;
					
									USART_TX_BUF[6]=0x00;  		
									USART_TX_BUF[7]=0x00;
									USART_TX_BUF[8]=da>>8;
									USART_TX_BUF[9]=(u8)da;
					
					        len=4;
				          valid_flag=1;
				}
				else if(address==0x0002FFF4) //��ʱ��5
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
		else if(grade==EnableTime)//��ʱ������
		{
	       if(address==0x00FFFF00)//��ʱ��0
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF01)//1
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF02)//2
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF03)//3
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF04)//4
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
				 else if(address==0x00FFFF05)//5
				 {
					 if(da==0x01) //����ʱ
					 {
              valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x01;  
						  len=1;						 
           }
					 else if(da == 0x00)//����ʱ�ر�
					 {
						  valid_flag=1;
						 	USART_TX_BUF[1]=0x3B;  
						  USART_TX_BUF[6]=0x00;  
						  len=1;		
					 }
				 }
    }
		else if(grade==ChangePG)//ҳ����ʾ �ı�ҳ��
		{
       	USART_TX_BUF[1]=0x70;  
		    valid_flag=2;		
		}
		if(valid_flag==1)
		 {
					valid_flag=0;
					USART_TX_BUF[0]=0xAA;  		
		 
					USART_TX_BUF[2]=(u8)((address&0xff000000)>>24); //��ַ
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
			    USART_TX_BUF[2]=da>>8; //ҳ�����
					USART_TX_BUF[3]=(u8)da;
			 		USART_TX_BUF[4]=0xCC;
					USART_TX_BUF[5]=0x33;
					USART_TX_BUF[6]=0xC3;
					USART_TX_BUF[7]=0x3C;
					sendlen=8;
					sendflag=1;
		 }

		 if(sendflag==1)//����
		 {
				sendflag=0;
			 
			 //���ڷ��ͺ���
			 for(i=0;i<sendlen;i++)
			 	{
					U0C0_0_UART_SendData(USART_TX_BUF[i]);
				}
		 }
	return 0;
}

u8 send_lcd_pack_time(u8 grade,u32 address,u32 da)
{ 
	u8 valid_flag,len=0,sendlen=0,sendflag=0,i=0;
	valid_flag=0;
	u8 j=0;
   if(grade==WriteTime)	//��ʱ����ֵ
		{
				if(address==0x0002FFE0)  //��ʱ��0
				{
									USART_TX_BUF[1]=0x44;
					
								  USART_TX_BUF[6]=(u8)((da&0xff000000)>>24);  
									USART_TX_BUF[7]=(u8)(((da&0x00ff0000)>>16));
									USART_TX_BUF[8]=(u8)(((da&0x0000ff00)>>8));
									USART_TX_BUF[9]=(u8)((da&0x000000ff));
//									USART_TX_BUF[8]=da>>8;//u16ʱ������������ 8λ  ��ȡ�߰�λ
//									USART_TX_BUF[9]=(u8)da;//ȡ�Ͱ�λ
					        len=4;
					        valid_flag=1;
				}
		    else if(address==0x0002FFE4) //��ʱ��1
				{	 	
									USART_TX_BUF[1]=0x44;
					
								  USART_TX_BUF[6]=(u8)((da&0xff000000)>>24); //��ַ
									USART_TX_BUF[7]=(u8)(((da&0x00ff0000)>>16));
									USART_TX_BUF[8]=(u8)(((da&0x0000ff00)>>8));
									USART_TX_BUF[9]=(u8)((da&0x000000ff));
					
					        len=4;
				          valid_flag=1;
				}
			}
			if(valid_flag==1)
		 {
					valid_flag=0;
					USART_TX_BUF[0]=0xAA;  		
		 
					USART_TX_BUF[2]=(u8)((address&0xff000000)>>24); //��ַ
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
			    USART_TX_BUF[2]=da>>8; //ҳ�����
					USART_TX_BUF[3]=(u8)da;
			 		USART_TX_BUF[4]=0xCC;
					USART_TX_BUF[5]=0x33;
					USART_TX_BUF[6]=0xC3;
					USART_TX_BUF[7]=0x3C;
					sendlen=8;
					sendflag=1;
		 }

		 if(sendflag==1)//����
		 {
				sendflag=0;
			 
			 //���ڷ��ͺ���
			 for(i=0;i<sendlen;i++)
			 	{
					U0C0_0_UART_SendData(USART_TX_BUF[i]);
				}
		 }
	return 0;
		}
u8 buf[6];
u8 send_speed_pack(u8 flag,u8 speed_flag,u8 turn_flag)
{
	     u8 i= 0;
			 buf[0]=0x3F;
			 buf[1]=flag;	
			 buf[2]=0x00;
			 buf[3]=speed_flag;	   
			 buf[4]=0xDD;	
			 buf[5]=0x3D;	   		
			 for(i=0;i<6;i++)
				{
							U0C0_UART_SendData(buf[i]);	
				}
				return 0;
}
void temperature_DS(void)
{
//	    temperature=read_temperature();	 //ʮ���¶�ֵ
 		 	temperature=DS18B20_Get_Temp();	 //ʮ���¶�ֵ
			if((	temperature%10)>=5)
				temperature= (temperature+10)/10;
			else
				temperature= temperature/10;
			send_lcd_pack(WriteData,0x00080800,temperature);	
 
}
 
//��֯��λ�����Ͱ�
void Send_Turnspeed_pack(u16  turn_speed4[],u8 Flag)
{
	            if(Flag == 0x02)//ֹͣ
							{
								    Speed_DATA[0] = 0x00;
										Speed_DATA[1] = 0x00; 
							      Speed_DATA[2] = 0x00;
										Speed_DATA[3] = 0x00;
							}
							else
							{
	                  Speed_DATA[0] = turn_speed4[1];//ת�ٻ�ȡ
										Speed_DATA[1] = 0x00; 
							      Speed_DATA[2] = 0x00;
										Speed_DATA[3] = 0x00;
							}
						 
										databuf[1]=Flag;//00 ��ת  01��ת 02 ֹͣ
										
										CRC16 = CRC_CHECK(Speed_DATA,8);
										Speed_DATA[2] = CRC16%256;
										Speed_DATA[3] = CRC16/256;
							  
							     for(i=2,j=0;i<6;i++)
										databuf[i]=Speed_DATA[j++];
							 
                  //send_speed_pack(databuf);
}


