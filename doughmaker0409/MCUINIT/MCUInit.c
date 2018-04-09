
#include <XMC1100.h>
#include "MCUInit.h"
#include  "timer.h"
/*********************************************************************************************************
** ��������: void MCUInit(void)
** ��������: ����XMC1300��ؼĴ���
**
** ˵�� ��:
**
**********************************************************************************************************/
uint32_t i_flag = 0;
//uint16_t speed =0;
//unsigned char Datas = 0;
//uint16_t Datas = 0;
//float speed_R[8] = {0};
extern uint16_t i;

void MCU_vInit(void)
{

	// UART��ʼ��
//	UART_vInit();
	UART_1_vInit();
	UART_0_vInit();
  SystemCoreClockUpdate(); //����ϵͳʱ��
 // SysTick_Config(SystemCoreClock/1000); //1ms�ж�һ��
	
	
	

//��ʱ����ʼ��	
//----CLOCK-SETUP-----------------------------------------------------------------------------------
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 = 0x00000004UL;		// stop gating CCU40  //��ֹ�ſ�CCU40;    UART_vInit����
	SCU_CLK->CLKCR = 0x3FF00400UL;			// 8 MHz MCLK, 8 MHz PCLK
	SCU_GENERAL->PASSWD = 0x000000C3UL;
//--------------------------------------------------------------------------------------------------
	CCU4_Init();	//CCU4��ʼ��;  ����Ƚϵ�Ԫ4     
	CCU4_Init1();	//CCU4��ʼ��;  ����Ƚϵ�Ԫ4     
	CCU4_Init2();	//CCU4��ʼ��;  ����Ƚϵ�Ԫ4     
	//----INTERRUPT-SETUP-------------------------------------------------------------------------------
	NVIC_ClearPendingIRQ(CCU40_0_IRQn);		//This function clears node 1 pending status	//�������״̬;
	NVIC_EnableIRQ(CCU40_0_IRQn);			//This function enables node 1 for interrupt request generation	//ʹ���ж�;
	
  NVIC_ClearPendingIRQ(CCU40_1_IRQn);		//This function clears node 1 pending status	//�������״̬;
	NVIC_EnableIRQ(CCU40_1_IRQn);			//This function enables node 1 for interrupt request generation	//ʹ���ж�;
	//--------------------------------------------------------------------------------------------------
	
//  //��ʱ���ж�����
//  SystemCoreClockUpdate();
//	SysTick_Config(SystemCoreClock / 10);	// 0.1s�ж�һ��
    // ERU��ʼ��
	ERU0_vInit();   // �¼�����Ԫ 
  GPIO(); //IO�ڳ�ʼ��
	
}



void SysTick_Handler  (void)   // �жϷ�����
{
   P0_1_toggle(); //��ת 
   P0_3_set_mode(OUTPUT_PP_GP);
   P0_3_set();
}
