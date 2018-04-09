/****************************************************************************
*    Filename:     ERU0.c
*    IDE:          DAVE_3.1.2
*    MCU:          XMC13OO
*
*    Revision History:
*    -------------------------------------------------------------------------
*    VXXX       2013-8-7    Created
*
*    Support :  IFCN.MCU@Infineon.com
*
*    Copyright (c) Infineon Semiconductor 2012. All Rights Reserved.
*****************************************************************************/


/*****************************************************************************
 *    Include Files
 *****************************************************************************/
#include <XMC1100.h>
#include "ERU0.h"
#include "datatransfer.h"
 #include <stdbool.h>
 #include "UART.h"
 #include "delay.h"

int s=0,n=0;
/*****************************************************************************
*     Macro Definitions
*****************************************************************************/


/*****************************************************************************
*     Local Type Definitions
*****************************************************************************/


/*****************************************************************************
*     Static(Local) Variable Definitions
*****************************************************************************/


/*****************************************************************************
*     Local Function Declarations
*****************************************************************************/


/*****************************************************************************
*     Global Variable Definitions
*****************************************************************************/


/*****************************************************************************
 *    Global Function Definitions
 *****************************************************************************/


/*****************************************************************************
*     Variable setting for testing, delete it when release
*****************************************************************************/


/*****************************************************************************
 * Function:      void  ERU0_vInit(void)
 * Description:   ERU0 ��ʼ��
 *
 * Caveats:       2013-8-7
 *
 *****************************************************************************/
void ERU0_vInit(void)
{

		//-------------------------------------------------------------------------
    // Step 1: ����SCUʱ��
	//         PASSWD ---RM1.0 Page326
	//         CLKCR  ---RM1.0 Page298
    //-------------------------------------------------------------------------

	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CLKCR = 0x3FF00400UL;													// 8 MHz MCLK, 8 MHz PCLK
	while((SCU_CLK->CLKCR)&0x40000000UL);											// �ȴ�VDDC��ѹ�ȶ�
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	
//PORT2->HWSEL = PORT2->HWSEL & 0xFFFFFFF3;
	//-------------------------------------------------------------------------
    // Step 2: ��������Դ  P2.5����ERU0_1A1 �� P2.1����ERU0_1B0
	//         ERU_EXISEL  ---RM1.0 Page133
    //-------------------------------------------------------------------------
		WR_REG(ERU0->EXISEL, ERU_EXISEL_EXS3B_Msk, ERU_EXISEL_EXS3B_Pos, 1);			//ѡ��ERU0_1B0����  p2.8
	  WR_REG(ERU0->EXISEL, ERU_EXISEL_EXS3B_Msk, ERU_EXISEL_EXS3B_Pos, 0);			//ѡ��ERU0_1B0����  p2.9
    WR_REG(ERU0->EXISEL, ERU_EXISEL_EXS3A_Msk, ERU_EXISEL_EXS3A_Pos, 1);			//ѡ��ERU0_1A1����  p2.7 
    //-------------------------------------------------------------------------
    // Step 3: P2.1�ߵ�ƽ�ڼ䣬P2.5���½���ʱ����OGU0�˿ڴ�������
	//         ERU_EXICON  ---RM1.0 Page135
    //-------------------------------------------------------------------------

//	WR_REG(ERU0->EXICON[0], ERU_EXICON_SS_Msk, ERU_EXICON_SS_Pos, 0);				//ѡ�������ź���Ϸ�ʽ���߼����롱  A&B
//	WR_REG(ERU0->EXICON[0], ERU_EXICON_PE_Msk, ERU_EXICON_PE_Pos, 1);				//ʹ�ܲ�����������
//	WR_REG(ERU0->EXICON[0], ERU_EXICON_RE_Msk, ERU_EXICON_RE_Pos, 1);				//�����ز�����
//	WR_REG(ERU0->EXICON[0], ERU_EXICON_FE_Msk, ERU_EXICON_FE_Pos, 1);				//�½��ش���
//	WR_REG(ERU0->EXICON[0], ERU_EXICON_OCS_Msk, ERU_EXICON_OCS_Pos, 0);				//ѡ����OGU0�˿ڴ�������
//	
//		WR_REG(ERU0->EXICON[1], ERU_EXICON_SS_Msk, ERU_EXICON_SS_Pos, 0);				//ѡ�������ź���Ϸ�ʽ���߼����롱  A&B
//	WR_REG(ERU0->EXICON[1], ERU_EXICON_PE_Msk, ERU_EXICON_PE_Pos, 1);				//ʹ�ܲ�����������
//	WR_REG(ERU0->EXICON[1], ERU_EXICON_RE_Msk, ERU_EXICON_RE_Pos, 1);				//�����ز�����
//	WR_REG(ERU0->EXICON[1], ERU_EXICON_FE_Msk, ERU_EXICON_FE_Pos, 1);				//�½��ش���
//	WR_REG(ERU0->EXICON[1], ERU_EXICON_OCS_Msk, ERU_EXICON_OCS_Pos, 1);				//ѡ����OGU0�˿ڴ�������
//	
//  WR_REG(ERU0->EXICON[2], ERU_EXICON_SS_Msk, ERU_EXICON_SS_Pos, 0);				//ѡ�������ź���Ϸ�ʽ���߼����롱  A&B
//	WR_REG(ERU0->EXICON[2], ERU_EXICON_PE_Msk, ERU_EXICON_PE_Pos, 1);				//ʹ�ܲ�����������
//	WR_REG(ERU0->EXICON[2], ERU_EXICON_RE_Msk, ERU_EXICON_RE_Pos, 1);				//�����ز�����
//	WR_REG(ERU0->EXICON[2], ERU_EXICON_FE_Msk, ERU_EXICON_FE_Pos, 1);				//�½��ش���
//	WR_REG(ERU0->EXICON[2], ERU_EXICON_OCS_Msk, ERU_EXICON_OCS_Pos, 2);				//ѡ����OGU0�˿ڴ�������
	
  WR_REG(ERU0->EXICON[3], ERU_EXICON_SS_Msk, ERU_EXICON_SS_Pos, 1);				//ѡ�������ź���Ϸ�ʽ���߼����롱  A&B
	WR_REG(ERU0->EXICON[3], ERU_EXICON_PE_Msk, ERU_EXICON_PE_Pos, 1);				//ʹ�ܲ�����������
	WR_REG(ERU0->EXICON[3], ERU_EXICON_RE_Msk, ERU_EXICON_RE_Pos, 0);				//�����ز�����
	WR_REG(ERU0->EXICON[3], ERU_EXICON_FE_Msk, ERU_EXICON_FE_Pos, 1);				//�½��ش���
	WR_REG(ERU0->EXICON[3], ERU_EXICON_OCS_Msk, ERU_EXICON_OCS_Pos, 0);				//ѡ����OGU0�˿ڴ�������
    //-------------------------------------------------------------------------
    // Step 4:  ʹ��OGU0�ϵ��ж�
	  //          EXOCON  ---RM1.0 Page118
    //-------------------------------------------------------------------------

	WR_REG(ERU0->EXOCON[0], ERU_EXOCON_GP_Msk, ERU_EXOCON_GP_Pos, 1);
//		WR_REG(ERU0->EXOCON[3], ERU_EXOCON_GP_Msk, ERU_EXOCON_GP_Pos, 1);
//	WR_REG(ERU0->EXOCON[1], ERU_EXOCON_GP_Msk, ERU_EXOCON_GP_Pos, 1);
//		WR_REG(ERU0->EXOCON[2], ERU_EXOCON_GP_Msk, ERU_EXOCON_GP_Pos, 1);
// 	PORT2 ->PDISC =0;// 	P2_8_enable_digital();

    //-------------------------------------------------------------------------
    // Step 5: ����IO�ڣ� P2.1�� P2.5 ���룻P0.0���
	//         PORT_IOCR0  ---RM1.0 Page1205
	//         PORT_IOCR4  ---RM1.0 Page1206
	//         PORT_PDISC  ---RM1.0 Page1214
    //-------------------------------------------------------------------------
	P2_8_enable_digital();
	P2_8_set_mode(INPUT);															// ����P2.8����

	P2_9_enable_digital();
	P2_9_set_mode(INPUT);															// ����P2.9����
	
//	P2_7_enable_digital();
//	P2_7_set_mode(INPUT_PU);															// ����P2.9����
 
	//-------------------------------------------------------------------------
  // Step 6: �����ж�
	//         NVIC_ICPR  ---RM1.0 Page112
	//         NVIC_ISER  ---RM1.0 Page109
  //-------------------------------------------------------------------------

	NVIC_ClearPendingIRQ(ERU0_0_IRQn);												//����ڵ�ĵȴ�״̬
	NVIC_EnableIRQ(ERU0_0_IRQn);													//ʹ�ܽڵ��ж�����
	
//		NVIC_ClearPendingIRQ(ERU0_3_IRQn);												//����ڵ�ĵȴ�״̬
//	NVIC_EnableIRQ(ERU0_3_IRQn);													//ʹ�ܽڵ��ж�����
//	
//		NVIC_ClearPendingIRQ(ERU0_1_IRQn);												//����ڵ�ĵȴ�״̬
//	NVIC_EnableIRQ(ERU0_1_IRQn);													//ʹ�ܽڵ��ж�����
//	
//			NVIC_ClearPendingIRQ(ERU0_2_IRQn);												//����ڵ�ĵȴ�״̬
//	NVIC_EnableIRQ(ERU0_2_IRQn);													//ʹ�ܽڵ��ж�����
//	
//	NVIC_SetPriority(ERU0_1_IRQn,0xC0 );
//	NVIC_SetPriority(ERU0_0_IRQn,0xC0 );
}

/*****************************************************************************
 * Function:      void  ERU0_0_IRQHandler(void)
 * Description:   ERU0_0 �жϴ����������ERU_FL��־��������IO�ڷ�ת
 *
 * Caveats:       2013-8-7
 *
 *****************************************************************************/

u8 timehouflag=0,timeminflag=0,timesecflag=0;
extern u16  timehour,timemin ,timesec;
bool SetTimeFlag=0,SetSpeedFlag=0, speed_val_flag = 0;

 
extern u16 turn_speed4[2];
extern u8 Speed_DATA[4]; 
extern unsigned char databuf[6];
extern unsigned int CRC16 ;
extern u16 i,j;

extern int fast_slow_flag;
extern u8 Time_data_number[7];
extern int proce_flag[5];
extern int page_flag;
extern int motor_flag;
void ERU0_0_IRQHandler(void)//������
{
	//-------------------------------------------------------------------------
    // Step 1: ���ERU_FL��־������P0.0��ת
	//         NVIC_EXICON  ---RM1.0 Page135
    //-------------------------------------------------------------------------

//	for(int i=0;i<2000;i++)
//	     __nop();//���ȥ��
	  if(((P2_8_read())==0 )&&((P2_9_read())==0 )) // ˳ʱ��
		{	 
					
			if(page_flag == hand)
			{
				 	if(fast_slow_flag == 0)//����״̬
					{					
						 send_lcd_pack(WriteData,0x0008000E,++Time_data_number[0]);// ����
					}	
				  else if(fast_slow_flag == 1 )//����״̬
					{
						 send_lcd_pack(WriteData,0x00080010,++Time_data_number[1]);// ����;
					}
			}
			else if(page_flag == automatic)
			{
					if(motor_flag == 0)//���湴����
					{
						send_lcd_pack(WriteData,0x00080004,++Time_data_number[2]);
					}
					else if(motor_flag == 1)//���湴����
					{
						send_lcd_pack(WriteData,0x00080006,++Time_data_number[3]);
					}
					else if(motor_flag == 2)//��ͣ
					{
						send_lcd_pack(WriteData,0x00080008,++Time_data_number[4]);
					}
					else if(motor_flag == 3)//����Ͱ����
					{
						send_lcd_pack(WriteData,0x0008000A,++Time_data_number[5]);
					}
					else if(motor_flag == 4)//����Ͱ����
					{
						send_lcd_pack(WriteData,0x0008000C,++Time_data_number[6]);
					}
			}					
		}
	  else if(((P2_8_read())==0x00000100 )&&((P2_9_read())==0 ))  //  ��ʱ��
		{	 
			if(page_flag == hand)
			{
				  if(fast_slow_flag == 0)//����״̬
					{													 
						 send_lcd_pack(WriteData,0x0008000E,--Time_data_number[0]);// ����
					}	
				  else if(fast_slow_flag == 1 )//����״̬
					{ 
						 send_lcd_pack(WriteData,0x00080010,--Time_data_number[1]);// ����;
					}
			}		
			if(page_flag == automatic)
			{			
					if(motor_flag == 0)//���湴����
					{
						send_lcd_pack(WriteData,0x00080004,--Time_data_number[2]);
					}
					else if(motor_flag == 1)//���湴����
					{
						send_lcd_pack(WriteData,0x00080006,--Time_data_number[3]);
					}
					else if(motor_flag == 2)//��ͣ
					{
						send_lcd_pack(WriteData,0x00080008,--Time_data_number[4]);
					}
					else if(motor_flag == 3)//����Ͱ����
					{
						send_lcd_pack(WriteData,0x0008000A,--Time_data_number[5]);
					}
					else if(motor_flag == 4)//����Ͱ����
					{
						send_lcd_pack(WriteData,0x0008000C,--Time_data_number[6]);
					}	
		  }
		}
			CLR_BIT(ERU0->EXICON[1], ERU_EXICON_FL_Pos);									//���ERU_FL��־   
}
