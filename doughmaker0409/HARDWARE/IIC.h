

#include <XMC1100.h>


/** @brief  TDF code used when Master transmits data. The master sends TDF byte
 * along with data byte (TBUF[7:0]), receives and checks the
 * acknowledge bit sent by the slave */
#define I2C_TDF_MTxData 			(0x0U)

/** @brief  TDF code used when Slave transmits data. The slave only has to send
 *  data if it has been asked by the master. The slave sends its data
 *  byte (TBUF[7:0]) plus the acknowledge bit as a 1 */
#define I2C_TDF_STxData 			(0x1U)

/** @brief  TDF code used when Master acknowledges with 0. The master 
 * acknowledges the transfer with a 0-level to continue the transfer. 
 * The TBUF[7:0]content is ignored */
#define I2C_TDF_MRxAck0 			(0x2U)

/** @brief  TDF code used when Master acknowledges with 1. The master 
 *  acknowledges the transfer with a 1-level to finish the transfer. 
 *  The TBUF[7:0] content is ignored */
#define I2C_TDF_MRxAck1 			(0x3U)

/** @brief  TDF code used when Master transmits Start condition. The start 
 * condition will be generated if the bus is idle, the content of TBUF[7:0] is 
 * taken as first address byte for the transmission. (TBUF[7:1] - address and
 * TBUF[0] - read/write control) */
#define I2C_TDF_MStart 				(0x4U)

/** @brief  TDF code used when Master transmits Repeated start condition. 
 * The repeated start condition will be generated if SCL is low and byte
 * transfer is not in progress. The content of TBUF[7:0] is taken as first 
 * address byte for the transmission. (TBUF[7:1] - address and 
 * TBUF[0] - read/write control).*/
#define I2C_TDF_MRStart 			(0x5U)

/** @brief  TDF code used when Master transmits Stop condition. The Stop 
 *  condition will be generated if the master has finished its last byte 
 *  transfer, the content of TBUF[7:0] is taken as first address byte for the 
 * transmission. */
#define I2C_TDF_MStop 				(0x6U)

/** @brief  Master initiated write to slave device */
#define I2C_WRITE 	0U

/** @brief  Master initiated read from slave device */
#define I2C_READ 	 1U


void U0C0_IIC_vWriteData(unsigned int uwTDF, unsigned int uwData)
{
  while(USIC0_CH0->TCSR & 0x0080);             // check TDV, wait until TBUF is ready

  USIC0_CH0->PSCR   |= 0x2000;                                       // clear PSR_TBIF
  USIC0_CH0->TBUF[0]  = ((uwTDF << 8) & 0x0700) | (uwData & 0x00FF);  // load TBUF00  对应TBUF[10:8]和TBUF[7:0]

} //  End of function U0C0_IIC_vWriteData


unsigned int U0C0_IIC_uwReadData(void)
{
  while(!((USIC0_CH0->PSR & 0x8000) || (USIC0_CH0->PSR & 0x4000)));

  USIC0_CH0->PSCR   |= 0xC000;         // clear PSR_AIF & PSR_RIF
  return(USIC0_CH0->RBUF);             // return receive buffer register   接收的数据在RBUF[7:0],  附加信息在RBUF[12:8]被监视

} //  End of function U0C0_IIC_uwReadData


