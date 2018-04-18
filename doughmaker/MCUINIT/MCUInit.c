
#include <XMC1100.h>
#include "MCUInit.h"
#include  "timer.h"
/*********************************************************************************************************
** 函数名称: void MCUInit(void)
** 功能描述: 配置XMC1300相关寄存器
**
** 说　 明:
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

	// UART初始化
//	UART_vInit();
	UART_1_vInit();
	UART_0_vInit();
  SystemCoreClockUpdate(); //更新系统时钟
 // SysTick_Config(SystemCoreClock/1000); //1ms中断一次
	
	
	

//定时器初始化	
//----CLOCK-SETUP-----------------------------------------------------------------------------------
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 = 0x00000004UL;		// stop gating CCU40  //禁止门控CCU40;    UART_vInit已有
	SCU_CLK->CLKCR = 0x3FF00400UL;			// 8 MHz MCLK, 8 MHz PCLK
	SCU_GENERAL->PASSWD = 0x000000C3UL;
//--------------------------------------------------------------------------------------------------
	CCU4_Init();	//CCU4初始化;  捕获比较单元4     
	CCU4_Init1();	//CCU4初始化;  捕获比较单元4     
	CCU4_Init2();	//CCU4初始化;  捕获比较单元4     
	//----INTERRUPT-SETUP-------------------------------------------------------------------------------
	NVIC_ClearPendingIRQ(CCU40_0_IRQn);		//This function clears node 1 pending status	//清除挂起状态;
	NVIC_EnableIRQ(CCU40_0_IRQn);			//This function enables node 1 for interrupt request generation	//使能中断;
	
  NVIC_ClearPendingIRQ(CCU40_1_IRQn);		//This function clears node 1 pending status	//清除挂起状态;
	NVIC_EnableIRQ(CCU40_1_IRQn);			//This function enables node 1 for interrupt request generation	//使能中断;
	//--------------------------------------------------------------------------------------------------
	
//  //主时钟中断配置
//  SystemCoreClockUpdate();
//	SysTick_Config(SystemCoreClock / 10);	// 0.1s中断一次
    // ERU初始化
	ERU0_vInit();   // 事件请求单元 
  GPIO(); //IO口初始化
	
}



void SysTick_Handler  (void)   // 中断服务函数
{
   P0_1_toggle(); //翻转 
   P0_3_set_mode(OUTPUT_PP_GP);
   P0_3_set();
}
