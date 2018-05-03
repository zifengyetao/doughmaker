#include "timer.h"
#include <XMC1100.h>		//SFR declarations of the selected device
#include "gpio_xmc1100_tssop38.h"
#include "UART.h"
// values calculated for 6.4 ms timer period
#define TMR_VAL	50      // 周期=（ 1024(分频)/8M ）*(TMR_VAL)
#define TMR_VAL_1	1      // 周期=（ 1024(分频)/8M ）*(TMR_VAL)
extern  uint8_t date1flag;
extern  uint8_t date1flag_down;
void CCU40_0_IRQHandler(void)						//CCU40中断函数   定时器
{
	
	if(RD_REG(CCU40_CC40->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //读取向上计数中断标志位;
	{
			SET_BIT(CCU40_CC40->SWR, CCU4_CC4_SWR_RPM_Pos);  //清除向上计数中断标志位;
			date1flag = 1;//接收完成标志
		  WR_REG(CCU40_CC40->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//关闭定时器
//		   step++;
	}
//	if (step == 10)			//进入十次中断；
//	{
//		P0_1_toggle();
//		stepGJX++;
//		step = 0;
//	}
	
}
void CCU40_1_IRQHandler(void)						//CCU40中断函数   定时器
{
	
	if(RD_REG(CCU40_CC41->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //读取向上计数中断标志位;
	{
			SET_BIT(CCU40_CC41->SWR, CCU4_CC4_SWR_RPM_Pos);  //清除向上计数中断标志位;
			date1flag_down=1;//接收完成标志
		  WR_REG(CCU40_CC41->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//关闭定时器
//		   step++;
	}
//	if (step == 10)			//进入十次中断；
//	{
//		P0_1_toggle();
//		stepGJX++;
//		step = 0;
//	}
	
}	
void CCU4_Init(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //预分频器置位;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC40->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 10);//f_ccu4  2的10次方=1024分频WR_REG()写入的是十进制数

	// Configure the Period register
	WR_REG(CCU40_CC40->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL);//向周期寄存器中写入周期值

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S0SE_Msk, CCU4_GCSS_S0SE_Pos, 1); //定时器片0映射传送;

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS0I_Msk, CCU4_GIDLC_CS0I_Pos, 1);	//CC40退出空闲模式;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC40->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//向上计数周期匹配使能  中断；产生中断脉冲
	WR_REG(CCU40_CC40->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 0);//将中断转移到CC40 SR0服务请求线；

	// Start Timer
// 	WR_REG(CCU40_CC40->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//启动定时器CCU40;  
  //关闭定时器
  WR_REG(CCU40_CC40->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 0);	//关闭定时器CCU40;
}
void CCU4_Init1(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //预分频器置位;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC41->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 10);//f_ccu4  2的10次方=1024分频WR_REG()写入的是十进制数

	// Configure the Period register
	WR_REG(CCU40_CC41->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL);//向周期寄存器中写入周期值

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S1SE_Msk, CCU4_GCSS_S1SE_Pos, 1); //定时器片0映射传送;  CCU4_GCSS_S1SE_Pos  注意修改1

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS1I_Msk, CCU4_GIDLC_CS1I_Pos, 1);	//CC40退出空闲模式;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC41->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//向上计数周期匹配使能  中断；产生中断脉冲
	WR_REG(CCU40_CC41->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 1);		//将中断转移到CC40 SR0服务请求线；

	// Start Timer
// 	WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//启动定时器CCU40;  启动关闭没有影响
  //关闭定时器
  WR_REG(CCU40_CC41->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//关闭定时器CCU40;
}
void CCU40_2_IRQHandler(void)						//CCU40中断函数   定时器
{
	
	if(RD_REG(CCU40_CC42->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //读取向上计数中断标志位;
	{
			SET_BIT(CCU40_CC42->SWR, CCU4_CC4_SWR_RPM_Pos);  //清除向上计数中断标志位;
		  WR_REG(CCU40_CC42->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//关闭定时器
	}
}
void CCU4_Init2(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //预分频器置位;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC42->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 3);//f_ccu4  2的10次方=1024分频WR_REG()写入的是十进制数

	// Configure the Period register
	WR_REG(CCU40_CC42->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL_1);//向周期寄存器中写入周期值   TMR_VAL_1 =1  计数一次

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S2SE_Msk, CCU4_GCSS_S2SE_Pos, 1); //定时器片0映射传送;

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS2I_Msk, CCU4_GIDLC_CS2I_Pos, 1);	//CC40退出空闲模式;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC42->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//向上计数周期匹配使能  中断；产生中断脉冲
	WR_REG(CCU40_CC42->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 1);		//将中断转移到CC40 SR0服务请求线；

	// Start Timer
// WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//启动定时器CCU40;   
  //关闭定时器
  WR_REG(CCU40_CC42->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//关闭定时器CCU40;
}
