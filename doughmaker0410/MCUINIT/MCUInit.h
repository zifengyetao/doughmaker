/*
 * MCUInit.h
 *
 *  Created on: 2013-8-6
 *      Author: qianqin
 */

#ifndef MCUINIT_H_
#define MCUINIT_H_


#include "gpio_xmc1100_tssop38.h"
#include "UART.h"

extern  uint32_t SystemCoreClock;
extern float speed_R[8] ;

void MCU_vInit(void);
void SysTick_Handler(void);//º¯ÊýÉùÃ÷


#endif /* MCUINIT_H_ */
