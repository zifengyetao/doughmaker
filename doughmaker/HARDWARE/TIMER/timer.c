#include "timer.h"
#include <XMC1100.h>		//SFR declarations of the selected device
#include "gpio_xmc1100_tssop38.h"
#include "UART.h"
// values calculated for 6.4 ms timer period
#define TMR_VAL	50      // ����=�� 1024(��Ƶ)/8M ��*(TMR_VAL)
#define TMR_VAL_1	1      // ����=�� 1024(��Ƶ)/8M ��*(TMR_VAL)
extern  uint8_t date1flag;
extern  uint8_t date1flag_down;
void CCU40_0_IRQHandler(void)						//CCU40�жϺ���   ��ʱ��
{
	
	if(RD_REG(CCU40_CC40->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //��ȡ���ϼ����жϱ�־λ;
	{
			SET_BIT(CCU40_CC40->SWR, CCU4_CC4_SWR_RPM_Pos);  //������ϼ����жϱ�־λ;
			date1flag = 1;//������ɱ�־
		  WR_REG(CCU40_CC40->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//�رն�ʱ��
//		   step++;
	}
//	if (step == 10)			//����ʮ���жϣ�
//	{
//		P0_1_toggle();
//		stepGJX++;
//		step = 0;
//	}
	
}
void CCU40_1_IRQHandler(void)						//CCU40�жϺ���   ��ʱ��
{
	
	if(RD_REG(CCU40_CC41->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //��ȡ���ϼ����жϱ�־λ;
	{
			SET_BIT(CCU40_CC41->SWR, CCU4_CC4_SWR_RPM_Pos);  //������ϼ����жϱ�־λ;
			date1flag_down=1;//������ɱ�־
		  WR_REG(CCU40_CC41->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//�رն�ʱ��
//		   step++;
	}
//	if (step == 10)			//����ʮ���жϣ�
//	{
//		P0_1_toggle();
//		stepGJX++;
//		step = 0;
//	}
	
}	
void CCU4_Init(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //Ԥ��Ƶ����λ;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC40->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 10);//f_ccu4  2��10�η�=1024��Ƶ�WWR_REG()д�����ʮ������

	// Configure the Period register
	WR_REG(CCU40_CC40->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL);//�����ڼĴ�����д������ֵ

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S0SE_Msk, CCU4_GCSS_S0SE_Pos, 1); //��ʱ��Ƭ0ӳ�䴫��;

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS0I_Msk, CCU4_GIDLC_CS0I_Pos, 1);	//CC40�˳�����ģʽ;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC40->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//���ϼ�������ƥ��ʹ��  �жϣ������ж�����
	WR_REG(CCU40_CC40->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 0);//���ж�ת�Ƶ�CC40 SR0���������ߣ�

	// Start Timer
// 	WR_REG(CCU40_CC40->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//������ʱ��CCU40;  
  //�رն�ʱ��
  WR_REG(CCU40_CC40->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 0);	//�رն�ʱ��CCU40;
}
void CCU4_Init1(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //Ԥ��Ƶ����λ;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC41->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 10);//f_ccu4  2��10�η�=1024��Ƶ�WWR_REG()д�����ʮ������

	// Configure the Period register
	WR_REG(CCU40_CC41->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL);//�����ڼĴ�����д������ֵ

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S1SE_Msk, CCU4_GCSS_S1SE_Pos, 1); //��ʱ��Ƭ0ӳ�䴫��;  CCU4_GCSS_S1SE_Pos  ע���޸�1

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS1I_Msk, CCU4_GIDLC_CS1I_Pos, 1);	//CC40�˳�����ģʽ;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC41->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//���ϼ�������ƥ��ʹ��  �жϣ������ж�����
	WR_REG(CCU40_CC41->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 1);		//���ж�ת�Ƶ�CC40 SR0���������ߣ�

	// Start Timer
// 	WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//������ʱ��CCU40;  �����ر�û��Ӱ��
  //�رն�ʱ��
  WR_REG(CCU40_CC41->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//�رն�ʱ��CCU40;
}
void CCU40_2_IRQHandler(void)						//CCU40�жϺ���   ��ʱ��
{
	
	if(RD_REG(CCU40_CC42->INTS, CCU4_CC4_INTS_PMUS_Msk, CCU4_CC4_INTS_PMUS_Pos)==1) //��ȡ���ϼ����жϱ�־λ;
	{
			SET_BIT(CCU40_CC42->SWR, CCU4_CC4_SWR_RPM_Pos);  //������ϼ����жϱ�־λ;
		  WR_REG(CCU40_CC42->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//�رն�ʱ��
	}
}
void CCU4_Init2(void)
{
	// Set Prescaler run bit
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);   //Ԥ��Ƶ����λ;
	//Configure the Prescaler for f_CCU4/1024
	WR_REG(CCU40_CC42->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, 3);//f_ccu4  2��10�η�=1024��Ƶ�WWR_REG()д�����ʮ������

	// Configure the Period register
	WR_REG(CCU40_CC42->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, TMR_VAL_1);//�����ڼĴ�����д������ֵ   TMR_VAL_1 =1  ����һ��

	// Shadow transfer for slice 0 for CCU40 Kernel
	WR_REG(CCU40->GCSS, CCU4_GCSS_S2SE_Msk, CCU4_GCSS_S2SE_Pos, 1); //��ʱ��Ƭ0ӳ�䴫��;

	// Clear idle bit slice 0 for CCU40 Kernel
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS2I_Msk, CCU4_GIDLC_CS2I_Pos, 1);	//CC40�˳�����ģʽ;

	// Enable period match while counting up interrupt
	WR_REG(CCU40_CC42->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);	//���ϼ�������ƥ��ʹ��  �жϣ������ж�����
	WR_REG(CCU40_CC42->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 1);		//���ж�ת�Ƶ�CC40 SR0���������ߣ�

	// Start Timer
// WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);	//������ʱ��CCU40;   
  //�رն�ʱ��
  WR_REG(CCU40_CC42->TCCLR, CCU4_CC4_TCCLR_TRBC_Msk, CCU4_CC4_TCCLR_TRBC_Pos  , 1);	//�رն�ʱ��CCU40;
}
