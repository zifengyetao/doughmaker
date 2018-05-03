/****************************************************************************
*    Filename:     UART.c
*    IDE:          DAVE_3.1.2
*    MCU:          XMC13OO
*
*    Revision History:
*    -------------------------------------------------------------------------
*    VXXX       2013-8-13    Created
*
*    Support :  IFCN.MCU@Infineon.com
*
*    Copyright (c) Infineon Semiconductor 2012. All Rights Reserved.
*****************************************************************************/


/*****************************************************************************
 *    Include Files
 *****************************************************************************/
#include <XMC1100.h>
#include "UART.h"
#include "datatransfer.h"	
#include <gpio_xmc1100_tssop38.h>
/*****************************************************************************
*     Macro Definitions
*****************************************************************************/


/*****************************************************************************
*     Local Type Definitions
*****************************************************************************/


/*****************************************************************************
*     Static(Local) Variable Definitions
*****************************************************************************/
 
#define   USIC_CH_TBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_TBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_TBCTR_DPTRSIZE_Pos)

#define   USIC_CH_RBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_RBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_RBCTR_DPTRSIZE_Pos)
/*****************************************************************************
*     Local Function Declarations
*****************************************************************************/


/*****************************************************************************
*     Global Variable Definitions
*****************************************************************************/
extern uint8_t USART_RX_BUF[1024];  
extern uint8_t USART_RX_BUF_DOWN[1024];  
uint8_t date1flag=0;
uint8_t date1flag_down=0;
uint32_t recev_start=0,recev_end=0;
uint32_t recev_start_down=0,recev_end_down=0;
 
uint16_t Read_Data = 0 ;        			//接收数据变量
uint16_t Data = 0;
uint8_t message[] = {1,2,3,4,5,6,7};
extern uint8_t i;  

// (8*10^6)/((BRG_PDIV+1)*(BRG_DCTQ+1)(BRG_PCTQ+1))
#define FDR_STEP	590
#define BRG_PDIV	3
#define BRG_DCTQ	9
#define BRG_PCTQ	0

//(8*10^6)/((BRG_PDIV+1)*(BRG_DCTQ+1)(BRG_PCTQ+1))
#define FDR_STEP_1	920
#define BRG_PDIV_1	0
#define BRG_DCTQ_1	3
#define BRG_PCTQ_1	0

int receive_length = 0,receive_length_down = 0;

/*****************************************************************************
 *    Global Function Definitions
 *****************************************************************************/

/*****************************************************************************
*     Variable setting for testing, delete it when release
*****************************************************************************/

/*****************************************************************************
 * Function:      void  UART_vInit(void)
 * Description:   Initialize the UART Module
 *                (Please add more details)
 * Caveats:       2013-8-13
 *                test only (Please add more details)
 *****************************************************************************/
void UART_0_vInit(void)//触摸屏串口
{
  
	//-------------------------------------------------------------------------
    // Step 1: 配置SCU时钟
	//         PASSWD ---RM1.0 Page326
	//         CLKCR  --- RM1.0 Page298
    //-------------------------------------------------------------------------
	SCU_GENERAL->PASSWD = 0x000000C0UL;        //对应参考手册各寄存器配置 
	SCU_CLK->CGATCLR0 |= 0x00000008;												// stop gating USIC
	SCU_CLK->CLKCR = 0x3FF00400UL;													// 8 MHz MCLK, 8 MHz PCLK
	while((SCU_CLK->CLKCR)&0x40000000UL);											// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

		//-------------------------------------------------------------------------
  // Step 2: 串口引脚设置：P1.2输出，P1.3输入
	//         PORT_IOCR0 ---RM1.0 Page1205
  //-------------------------------------------------------------------------
 
	P1_0_set_mode(OUTPUT_PP_AF7);
	P1_1_set_mode(INPUT);
	
 //    // Step 3: 内核状态寄存器配置 - 使能模块，模块 + MODEN位保护
//	//         USIC_CH_KSCFG ---RM1.1 Page500
	//-------------------------------------------------------------------------
	USIC0_CH0->KSCFG |= (1 << USIC_CH_KSCFG_MODEN_Pos) | (1 << USIC_CH_KSCFG_BPMODEN_Pos);

	//-------------------------------------------------------------------------
  // Step 4:  选择分数分频器，并给相应寄存器赋值
	//     		USIC_CH_FDR ---RM1.0 Page513
	//-------------------------------------------------------------------------

	WR_REG(USIC0_CH0->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);               //选择分数分频器
	WR_REG(USIC0_CH0->FDR, USIC_CH_FDR_STEP_Msk, USIC_CH_FDR_STEP_Pos, FDR_STEP);    //步长值



	//Configuration of USIC Input Stage
//	WR_REG(USIC0_CH0->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 3);//输入引脚设置  p1.1接收
   USIC0_CH0->DX0CR |= 0x0003;//DX0D  P1.1
	//-------------------------------------------------------------------------
  // Step 5:  配置波特率，PreDivider for CTQ, PCTQ = 0, Denominator for CTQ, DCTQ = 16
	//     USIC_CH_BRG       ---RM1.0 Page514
	//-------------------------------------------------------------------------
	WR_REG(USIC0_CH0->BRG, USIC_CH_BRG_PCTQ_Msk, USIC_CH_BRG_PCTQ_Pos, BRG_PCTQ);
	WR_REG(USIC0_CH0->BRG, USIC_CH_BRG_DCTQ_Msk, USIC_CH_BRG_DCTQ_Pos, BRG_DCTQ);
	WR_REG(USIC0_CH0->BRG, USIC_CH_BRG_PDIV_Msk, USIC_CH_BRG_PDIV_Pos, BRG_PDIV);

	//-------------------------------------------------------------------------
    // Step 6:配置USIC移位控制寄存器 ：
	//        USIC_CH_SCTR      ---RM1.0 Page518
	//-------------------------------------------------------------------------
	// 发送/接收 LSB先， Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1 //数据格式配置 
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1); // PDL:定义没有数据可发送时位移数据输出信号的输出电平
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);//MSK:发送模式
	// Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);//帧长度
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);//字长

	//-------------------------------------------------------------------------
    // Step 7: 配置发送控制状态寄存器
	//         USIC_CH_TCSR ---RM1.0 Page521
	//-------------------------------------------------------------------------
	//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);//数据发送使能
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);//数据发送单次模式

	//-------------------------------------------------------------------------
  // Step 8: 配置协议控制寄存器
	//         USIC_CH_PCR_ASCMode ---RM1.0 Page401
	//-------------------------------------------------------------------------
	//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,	USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);  //停止位数目1
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
	//TBIF is set to simplify polling
	WR_REG(USIC0_CH0->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);


	//Configuration of USIC Input Stage
//	WR_REG(USIC0_CH0->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 3);//输入引脚设置  p1.1接收
//	USIC0_CH0->DX0CR |= 0x0003;//DX0D  P1.1
	
	
//	WR_REG(USIC0_CH0->DX1CR, USIC_CH_DX1CR_DSEL_Msk, USIC_CH_DX1CR_DSEL_Pos, 4);
	//-------------------------------------------------------------------------
    // Step 9: 配置通道控制寄存器
	//         USIC_CH_CCR ---RM1.0 Page518
	//-------------------------------------------------------------------------
	WR_REG(USIC0_CH0->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, 2);				//选择 UART协议
	WR_REG(USIC0_CH0->CCR, USIC_CH_CCR_PM_Msk, USIC_CH_CCR_PM_Pos, 0);					//不产生奇偶校验
	WR_REG(USIC0_CH0->CCR, USIC_CH_CCR_RIEN_Msk, USIC_CH_CCR_RIEN_Pos, 1);				//使能接收中断
 
	//-------------------------------------------------------------------------
	// Step 10: 使能中断节点
	//         ICPR ---RMv1.0 Page112
	//         ISER ---RMv1.0 Page109
	//-------------------------------------------------------------------------
 
	USIC0_CH0->INPR = 0x22222;
//  NVIC_SetPriority(USIC0_1_IRQn,128);
	NVIC_ClearPendingIRQ(USIC0_2_IRQn);													//清除节点的等待状态
	NVIC_EnableIRQ(USIC0_2_IRQn);														//使能节点中断请求

////-------------------------------------------------------------------------
 
}

void UART_1_vInit(void)//下位机串口
{
  
	//-------------------------------------------------------------------------
    // Step 1: 配置SCU时钟
	//         PASSWD ---RM1.0 Page326
	//         CLKCR  --- RM1.0 Page298
    //-------------------------------------------------------------------------
	SCU_GENERAL->PASSWD = 0x000000C0UL;        //对应参考手册各寄存器配置 
	SCU_CLK->CGATCLR0 |= 0x00000008;												// stop gating USIC
	SCU_CLK->CLKCR = 0x3FF00400UL;													// 8 MHz MCLK, 8 MHz PCLK
	while((SCU_CLK->CLKCR)&0x40000000UL);											// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

		//-------------------------------------------------------------------------
  // Step 2: 串口引脚设置：P1.2输出，P1.3输入
	//         PORT_IOCR0 ---RM1.0 Page1205
  //-------------------------------------------------------------------------
 
	P1_2_set_mode(OUTPUT_PP_AF7);
	P1_3_set_mode(INPUT);
	
  WR_REG(USIC0_CH1->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 1);//输入引脚设置  p1.2接收
	USIC0_CH1->DX0CR |= 0x0001;//DX0D  P1.2
 //    // Step 3: 内核状态寄存器配置 - 使能模块，模块 + MODEN位保护
//	//         USIC_CH_KSCFG ---RM1.1 Page500
	//-------------------------------------------------------------------------
	USIC0_CH1->KSCFG |= (1 << USIC_CH_KSCFG_MODEN_Pos) | (1 << USIC_CH_KSCFG_BPMODEN_Pos);

	//-------------------------------------------------------------------------
  // Step 4:  选择分数分频器，并给相应寄存器赋值
	//     		USIC_CH_FDR ---RM1.0 Page513
	//-------------------------------------------------------------------------

	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 1);               //选择分数分频器
	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_STEP_Msk, USIC_CH_FDR_STEP_Pos, FDR_STEP_1);    //步长值

	//-------------------------------------------------------------------------
  // Step 5:  配置波特率，PreDivider for CTQ, PCTQ = 0, Denominator for CTQ, DCTQ = 16
	//     USIC_CH_BRG       ---RM1.0 Page514
	//-------------------------------------------------------------------------
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PCTQ_Msk, USIC_CH_BRG_PCTQ_Pos, BRG_PCTQ_1);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_DCTQ_Msk, USIC_CH_BRG_DCTQ_Pos, BRG_DCTQ_1);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PDIV_Msk, USIC_CH_BRG_PDIV_Pos, BRG_PDIV_1);

	//-------------------------------------------------------------------------
    // Step 6:配置USIC移位控制寄存器 ：
	//        USIC_CH_SCTR      ---RM1.0 Page518
	//-------------------------------------------------------------------------
	// 发送/接收 LSB先， Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1 //数据格式配置 
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1); // PDL:定义没有数据可发送时位移数据输出信号的输出电平
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);//MSK:发送模式
	// Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);//帧长度
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);//字长

	//-------------------------------------------------------------------------
    // Step 7: 配置发送控制状态寄存器
	//         USIC_CH_TCSR ---RM1.0 Page521
	//-------------------------------------------------------------------------
	//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);//数据发送使能
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);//数据发送单次模式

	//-------------------------------------------------------------------------
  // Step 8: 配置协议控制寄存器
	//         USIC_CH_PCR_ASCMode ---RM1.0 Page401
	//-------------------------------------------------------------------------
	//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,	USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);  //停止位数目1
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
	//TBIF is set to simplify polling
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);


	//Configuration of USIC Input Stage
//	WR_REG(USIC0_CH0->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 3);//输入引脚设置  p1.1接收
//	USIC0_CH0->DX0CR |= 0x0003;//DX0D  P1.1
	
	
//	WR_REG(USIC0_CH0->DX1CR, USIC_CH_DX1CR_DSEL_Msk, USIC_CH_DX1CR_DSEL_Pos, 4);
	//-------------------------------------------------------------------------
    // Step 9: 配置通道控制寄存器
	//         USIC_CH_CCR ---RM1.0 Page518
	//-------------------------------------------------------------------------
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, 2);				//选择 UART协议
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_PM_Msk, USIC_CH_CCR_PM_Pos, 0);					//不产生奇偶校验
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_RIEN_Msk, USIC_CH_CCR_RIEN_Pos, 1);				//使能接收中断
 
	//-------------------------------------------------------------------------
	// Step 10: 使能中断节点
	//         ICPR ---RMv1.0 Page112
	//         ISER ---RMv1.0 Page109
	//-------------------------------------------------------------------------
USIC0_CH1->INPR = 0x33333;
//	NVIC_SetPriority(USIC0_0_IRQn,64);
	NVIC_ClearPendingIRQ(USIC0_3_IRQn);													//清除节点的等待状态
	NVIC_EnableIRQ(USIC0_3_IRQn);														//使能节点中断请求
 
////-------------------------------------------------------------------------
 
}






/*****************************************************************************
 * Function:      void  USIC0_0_IRQHandler(void)
 * Description:
 *                (Please add more details)
 * Caveats:       2013-8-13
 *                test only (Please add more details)
 *****************************************************************************/

void USIC0_2_IRQHandler(void)
{

	//-------------------------------------------------------------------------
	// Step 1: 报警中断， 
	//         PSR  ---RMv1.0 Page505
	//         RBUF ---RMv1.0 Page537
	//         TBUF ---RMv1.0 Page530
	//-------------------------------------------------------------------------
	// USIC_CH_PSR_RIF_Msk:接收标志  0：未发生接收 1：发生接收
	if(RD_REG(USIC0_CH0->PSR, USIC_CH_PSR_RIF_Msk, USIC_CH_PSR_RIF_Pos) == 1)		//判断接收中断标志
	{
		CLR_BIT(USIC0_CH0->PSCR, USIC_CH_PSCR_CRIF_Pos);		//清除接收中断标志

	  USART_RX_BUF[recev_end] =(USIC0_CH0->RBUF) ;												//读取接收到的数据
		
		if(USART_RX_BUF[recev_end]!= 0x0D)
			receive_length++;
		Data = RD_REG(USIC0_CH0->RBUF ,USIC_CH_RBUF_DSR_Msk ,USIC_CH_RBUF_DSR_Pos);
	  recev_end++;		
		if(recev_end>=1024)
		{
			recev_end=0;
		}
		if(receive_length == 0x0C)//最长是12
			date1flag = 1;
		else date1flag=0;
		//开启定时器 
		WR_REG(CCU40_CC40->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);
//		USIC0_CH1->TBUF[1] = Read_Data;												//将接收到的数据返回给主机
//    USIC0_CH1->TBUF[0] = 0xff;	
//  for (i=0;i<7;i++)
//    {
//      USIC0_CH1->TBUF[i] = message[i];
//      
//  
//    
//    
//    }
   
	}

}


void USIC0_3_IRQHandler(void)
{
		 //下位机通道1
		//-------------------------------------------------------------------------
	// Step 1: 报警中断， 
	//         PSR  ---RMv1.0 Page505
	//         RBUF ---RMv1.0 Page537
	//         TBUF ---RMv1.0 Page530
	//-------------------------------------------------------------------------
	// USIC_CH_PSR_RIF_Msk:接收标志  0：未发生接收 1：发生接收
	if(RD_REG(USIC0_CH1->PSR, USIC_CH_PSR_RIF_Msk, USIC_CH_PSR_RIF_Pos) == 1)		//判断接收中断标志
	{
		CLR_BIT(USIC0_CH1->PSCR, USIC_CH_PSCR_CRIF_Pos);		//清除接收中断标志

	  USART_RX_BUF_DOWN[recev_end_down] =(USIC0_CH1->RBUF) ;												//读取接收到的数据
		if(USART_RX_BUF_DOWN[recev_end_down] != 0x0D)
			receive_length_down++;
		Data = RD_REG(USIC0_CH1->RBUF ,USIC_CH_RBUF_DSR_Msk ,USIC_CH_RBUF_DSR_Pos  );
	  recev_end_down++;	
		
		if(recev_end_down>=1024)
		{
			recev_end_down=0;
		}
		if(receive_length_down == 0x0F)//最长是16
			receive_length_down = 1;
		else date1flag_down=0;
		//开启定时器 
		WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);
//		USIC0_CH1->TBUF[1] = Read_Data;												//将接收到的数据返回给主机
//    USIC0_CH1->TBUF[0] = 0xff;	
//  for (i=0;i<7;i++)
//    {
//      USIC0_CH1->TBUF[i] = message[i];
//      
//  
//    
//    
//    }
   
	}
}

void DealLcdMsg(void)
{//(RD_REG(USIC0_CH1->PSR, USIC_CH_PSR_RIF_Msk, USIC_CH_PSR_RIF_Pos) == 0)&&
	if(date1flag==1)//接收完成标志   定时器中断 
	{
		if(recev_start!=recev_end)
		{	
			manage_one_pack();
		}
	} 
}
void Deal_Down_Msg(void)
{
	if(date1flag_down==1)//接收完成标志   定时器中断 
	{
		if(recev_start_down!=recev_end_down)
		{
			manage_one_down_pack();
		}
	}
}
void U0C0_UART_vSendData(uint16_t Data)//通道1    
{

  while(!(USIC0_CH1->PSR_ASCMode & 0x2000));   //  wait until tx buffer indication flag is set

  USIC0_CH1->PSCR     |= 0x2000;   //  clear transmit buffer indication flag
  USIC0_CH1->TBUF[0]   = Data;     //  load transmit buffer register

} //  End of function U1C1_SSC_vSendData

void U0C0_UART_SendData(uint16_t Data)// 通道1 下位机串口
{

  while(!(USIC0_CH1->PSR_ASCMode & 0x2000));   //  wait until tx buffer indication flag is set

  USIC0_CH1->PSCR     |= 0x2000;   //  clear transmit buffer indication flag
  USIC0_CH1->TBUF[1]   = Data;     //  load transmit buffer register

} //  End of function U1C1_SSC_vSendData
void U0C0_0_UART_SendData(uint16_t Data)//通道0  触摸屏串口
{

  while(!(USIC0_CH0->PSR_ASCMode & 0x2000));   //  wait until tx buffer indication flag is set

  USIC0_CH0->PSCR     |= 0x2000;   //  clear transmit buffer indication flag
  USIC0_CH0->TBUF[1]   = Data;     //  load transmit buffer register

} //  End of function U1C1_SSC_vSendData



unsigned int CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT) //CRC校验
{
    unsigned int CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++)
  {      
        CRC_Temp ^= Buf[i];//按位异或
        for (j=0;j<8;j++) 
      {
				if (CRC_Temp & 0x01)
						CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
				else
						CRC_Temp = CRC_Temp >> 1;
			
      }
  }
    return(CRC_Temp);
}

void OutPut_Data(u8 *data)
{
 uint16_t  databuf[9];
	for(i=0;i<9;i++)
	{
			databuf[0]=0x3F;
			databuf[1]=0x01;
	}
	for(i=0;i<10;i++)
	U0C0_UART_vSendData(databuf[i]);				
}
