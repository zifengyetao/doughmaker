/*****************************************************************************
*    Filename:     UART.h
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
#ifndef UART_H_
#define UART_H_

/*****************************************************************************
 *    Include Files
 *****************************************************************************/
#include <XMC1100.h>
#include "gpio_xmc1100_tssop38.h"
#include "UART.h"

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
void UART_0_vInit(void);
void UART_1_vInit(void);
void USIC0_2_IRQHandler(void);
void USIC0_3_IRQHandler(void);
 
void U0C0_UART_vSendData(uint16_t Data);
void U0C0_UART_SendData(uint16_t Data);
void U0C0_0_UART_SendData(uint16_t Data);
void U0C0_UART_ASCII_SendData(unsigned char Data);
unsigned char HexToChar(unsigned char bChar);

/*****************************************************************************
*     Variable setting for testing, delete it when release
*****************************************************************************/
unsigned int CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT);
void OutPut_Data(unsigned char *speed);
void DealLcdMsg(void);
#endif /* UART_H_ */
