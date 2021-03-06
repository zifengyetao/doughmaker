/****************************************************************************
*    Filename:     WDT.c
*    IDE:          DAVE_3.1.2
*    MCU:          XMC1100
*
*    Revision History:
*    -------------------------------------------------------------------------
*    V0.1       2012-08-01    Created
*
*    Support :  IFCN.MCU@Infineon.com
*
*    Copyright (c) Infineon Semiconductor 2012. All Rights Reserved.
*****************************************************************************/

/*****************************************************************************
 *  Include Files
 *****************************************************************************/
#include <XMC1100.h>
#include "WDT.h"


/*****************************************************************************
*     Macro Definitions
*****************************************************************************/


/*****************************************************************************
*     Local Type Definitions
*****************************************************************************/
volatile unsigned int uwTemp;


/*****************************************************************************
*     Static(Local) Variable Definitions
*****************************************************************************/
static unsigned long timingdelay = 100000000;

/*****************************************************************************
*     Local Function Declarations
*****************************************************************************/


/*****************************************************************************
*     Global Variable Definitions
*****************************************************************************/


/*****************************************************************************
 *  Global Function Definitions
 *****************************************************************************/
void WDT_vInit(void);
void WDT_Clear(void);

/*****************************************************************************
*     Variable setting for testing, delete it when release
*****************************************************************************/






/*****************************************************************************
 * Function:      void  WDT_vInit(void)
 * Description:   Initialize the WDT Module
 *                (Please add more details)
 * Caveats:       2012-08-17 15:45
 *                test only (Please add more details)
 *****************************************************************************/
void  WDT_vInit(void)
{
	//-------------------------------------------------------------------------------------
	// 0 Step:   LED P3.9 for Test
	//-------------------------------------------------------------------------------------
//	PORT3->IOCR8 = 0x00008000;
////	PORT3->OMR = 0x00000200;

//	PORT3->OMR = 0x02000000;
//	while(timingdelay--);
//	PORT3->OMR = 0x00000200;

	//-------------------------------------------------------------------------------------
	// 1 Step: enable WDT clock with SCU_CLKSET.WDTCEN register bit field
	//   CLK Set Register--RM1.1 Page597
	//-------------------------------------------------------------------------------------
//	SCU_CLK->CLKSET |= SCU_CLK_CLKSET_WDTCEN_Msk;

	//-------------------------------------------------------------------------------------
	// 2 Step:  release WDT reset with SCU_PRCLR2.WDTRS register
	//                              --RM1.1 Page593
	//-------------------------------------------------------------------------------------
//	SCU_RESET->PRCLR2  |= SCU_RESET_PRCLR2_WDTRS_Msk;

	//-------------------------------------------------------------------------------------
	// 3 Step:  set lower window bound with WDT_WLB register
	//	      – set upper window bound with WDT_WUB register
	//-------------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------------
	// 4 Step:   select and enable WDT input clock with SCU_WDTCLKCR register
	//  WDTCLKCR   --RM1.1 Page604
	//-------------------------------------------------------------------------------------
//	SCU_CLK-> WDTCLKCR = 0x00020000;           //SCU_CLK_WDTCLKCR_WDTSEL_Msk;
	                                            //SCU_CLK_WDTCLKCR_WDTDIV_Msk;

    SCU_CLK->CGATCLR0 |= 0x00000200UL;                        // Stop gating WDT Bit9 记得打开系统WDT clk    
//	  SCU_RESET->RSTSTAT |= 0x00000020UL;       //只读    
//	  SCU_CLK->CGATSET0 |= 0x00000200UL;                        // 使能WDT门控
	
  WDT->WLB   = 0X00000001;
	WDT->WUB   = 0Xfffffffe;
	//-------------------------------------------------------------------------------------
	//4 Step:   Enable WDT
	//  CTR	WDT Control Register   --RM1.1 Page140
	//-------------------------------------------------------------------------------------
	WDT->CTR = 0X0000FF01;//禁止预警  启动看门狗电路  
}


/*****************************************************************************
 * Function:      WDT_Clear(void)
 * Description:   Clear WDT
 *                Please add more details)
 * Caveats:       2012-08-16 11:26
 *                (Please add more details)
 *****************************************************************************/
void WDT_Clear(void)
{
    // Check weather TIM is greater than low boundary
	if ( WDT->TIM >= WDT->WLB )
    	WDT->SRV = 0xABADCAFE;
}


//----------------------------------------------------------------------------------------------------
//void  WDT_Init(void)
//{
// SCU_CLK->CLKSET |= SCU_CLK_CLKSET_WDTCEN_Msk;
// SCU_RESET->RSTCLR  |= SCU_RESET_PRCLR2_WDTRS_Msk;
// WDT->WLB   = 0X00000001;
// WDT->WUB   = 0Xfffffffe;
// SCU_CLK-> WDTCLKCR = 0x00020000;     
// WDT->CTR = 0X0000FF01;
//}
////------------------------------------------------------------------------------------------------------
//void WDT_Feed(void)
//{
//     if ( WDT->TIM >= WDT->WLB )
//        WDT->SRV = 0xABADCAFE;
//}
 

