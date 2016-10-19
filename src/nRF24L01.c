#include "nRF24L01.h"

volatile unsigned int sta;

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address
static SPI_HandleTypeDef spi;


/*******************************************************************************
 * Function Name   : SPI1_RW
 * Description : Sends a byte through the SPI interface and return the byte
 *                received from the SPI bus.
 * Input       : byte : byte to send.
 * Output       : None
 * Return       : The value of the received byte.
 *******************************************************************************/
void SPIInit(void){
  HAL_StatusTypeDef rc;

  /*uint8_t ID, type, size;*/
 spi.Instance = SPI3;
  spi.Init.Mode = SPI_MODE_MASTER;
  spi.Init.Direction = SPI_DIRECTION_2LINES;
  spi.Init.DataSize = SPI_DATASIZE_8BIT;
  spi.Init.CLKPolarity = SPI_POLARITY_LOW;
  spi.Init.CLKPhase = SPI_PHASE_1EDGE;
  spi.Init.NSS = SPI_NSS_SOFT;   /* We want to control the timing */
#ifdef USE_DMA
  spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
#else  
  spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;/* /8 for polling mode */
#endif
  spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi.Init.TIMode = SPI_TIMODE_DISABLED;
  spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  spi.Init.CRCPolynomial = 7 ;
  spi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
 
  rc = HAL_SPI_Init(&spi);
  if(rc != HAL_OK) {
	printf("SPI initialization failed with rc=%u\n",rc);
	return;
  }

  
  return;
}

int8_t GPIOInit(void){
GPIO_InitTypeDef GPIO_InitStruct;
	
	/*enable port c gpio clock*/
	__GPIOC_CLK_ENABLE();

	/*enable all 4 stepper gpios*/
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 |GPIO_PIN_12;
	
	/*set gpios as output with no pull ups*/
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = 6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	return 0;
}
  


uint8_t SPI1_readWrite(uint8_t byte) {
  HAL_StatusTypeDef rc;
  uint8_t recv;
  rc = HAL_SPI_TransmitReceive(&spi, &byte, &recv, 1, 100);
  if(rc) {
	printf("Failed to send command in %s()\n",__FUNCTION__);
	return 0;
  }
  return recv;
}

/*	// Loop while DR register in not empty
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	// Send byte through the SPI1 peripheral
	SPI_I2S_SendData(SPI1, byte);
	// Wait to receive a byte
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	// Return the byte read from the SPI bus
	return SPI_I2S_ReceiveData(SPI1); */


unsigned char SPI1_readWriteReg(unsigned char reg, unsigned char value) {
	unsigned char status;
	CSN_L();
	// Select register
	status = SPI1_readWrite(reg);
	// Set value
	SPI1_readWrite(value);
	CSN_H();
	return (status);
}

unsigned char SPI1_readReg(unsigned char reg) {
	unsigned char reg_val;
	CSN_L();
	SPI1_readWrite(reg);
	reg_val = SPI1_readWrite(0);
	CSN_H();
	return (reg_val);
}

unsigned char SPI1_readBuf(unsigned char reg, unsigned char *pBuf,
		unsigned char bytes) {
	unsigned char status, i;
	CSN_L();
	// Select register to write to and read status byte
	status = SPI1_readWrite(reg);
	for (i = 0; i < bytes; i++){
		pBuf[i] = SPI1_readWrite(0);
        }
	CSN_H();
	return (status);
}

unsigned char SPI1_writeBuf(unsigned char reg, unsigned char *pBuf,
		unsigned char bytes) {
	unsigned char status, i;
	CSN_L();
	// Select register to write to and read status byte
	status = SPI1_readWrite(reg);
	for (i = 0; i < bytes; i++) // then write all byte in buffer(*pBuf)
		SPI1_readWrite(*pBuf++);
	CSN_H();
	return (status);
}

void RX_Mode(void) {
	CE_L(); //GPIO_ResetBits // Set CE pin low
        
        //scrittura dell'indirizzo di ricezione
	SPI1_writeBuf(RF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
        //Enable ‘Auto Acknowledgment'
	SPI1_readWriteReg(RF_WRITE_REG + EN_AA, 0x01);          // Enable Auto.Ack:Pipe0
        //Enabled RX Addresses
	SPI1_readWriteReg(RF_WRITE_REG + EN_RXADDR, 0x01);      // Enable Pipe0
        //Selezione del canale (0-125)
	SPI1_readWriteReg(RF_WRITE_REG + RF_CH, 40);    	// Select RF channel 40
        //Number of bytes in RX payload in data pipe 0
	SPI1_readWriteReg(RF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
        //Selezione della velocità di trasmissione (1 o 2 Mbps) e dell'output power
	SPI1_readWriteReg(RF_WRITE_REG + RF_SETUP, 0x07);       // TX_PWR:0dBm, Datarate:1Mbps, // LNA:HCURR
        //Scrittura nel registo di configurazione degli altri parametri necessari
	SPI1_readWriteReg(RF_WRITE_REG + CONFIG, 0x0f); 	// Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled
        
	CE_H(); // GPIO_SetBits // Set CE pin high
        
        
	// This device is now ready to receive one packet of TX_PLOAD_WIDTH bytes payload from a TX device
	//sending to address TX_ADDRESS, with auto acknowledgment, retransmit count of 10, RF channel 40 and
	//datarate = 2Mbps.
}
        
void RX_Mode_Adv(unsigned char* addr,
                 unsigned char addr_width,
                 unsigned char pload_leng,
                 unsigned char aa,
                 unsigned char rxaddr,
                 unsigned char rfch,
                 unsigned char rfset,
                 unsigned char conf)
{
	CE_L(); //GPIO_ResetBits // Set CE pin low
        
        //scrittura dell'indirizzo di ricezione
	SPI1_writeBuf(RF_WRITE_REG + RX_ADDR_P0, addr, addr_width);
        //Enable ‘Auto Acknowledgment'
	SPI1_readWriteReg(RF_WRITE_REG + EN_AA, aa);          // Enable Auto.Ack:Pipe0
        //Enabled RX Addresses
	SPI1_readWriteReg(RF_WRITE_REG + EN_RXADDR, rxaddr);      // Enable Pipe0
        //Selezione del canale (0-125)
	SPI1_readWriteReg(RF_WRITE_REG + RF_CH, rfch);    	// Select RF channel 40
        //Number of bytes in RX payload in data pipe 0
	SPI1_readWriteReg(RF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
        //Selezione della velocità di trasmissione (1 o 2 Mbps) e dell'output power
	SPI1_readWriteReg(RF_WRITE_REG + RF_SETUP, rfset);       // TX_PWR:0dBm, Datarate:2Mbps, // LNA:HCURR
        //Scrittura nel registo di configurazione degli altri parametri necessari
	SPI1_readWriteReg(RF_WRITE_REG + CONFIG, conf); 	// Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled
        
	CE_H(); // GPIO_SetBits // Set CE pin high
}

void TX_Mode(void) {
        CE_L(); //GPIO_ResetBits // Set CE pin low
        
        //scrittura nel registro dell'indirizzo di trasmissione e di ricezione
	SPI1_writeBuf(RF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	SPI1_writeBuf(RF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
        //Enable ‘Auto Acknowledgment'
	SPI1_readWriteReg(RF_WRITE_REG + EN_AA, 0x01);          // Enable Auto.Ack:Pipe0
        //Enabled RX Addresses
	SPI1_readWriteReg(RF_WRITE_REG + EN_RXADDR, 0x01);      // Enable Pipe0
        //Setup ritrasmissione automatica
	SPI1_readWriteReg(RF_WRITE_REG + SETUP_RETR, 0x1a);     // 500us + 86us, 10 retrans...
        //Selezione del canale (0-125)
	SPI1_readWriteReg(RF_WRITE_REG + RF_CH, 40);            // Select RF channel 40
        //Selezione della velocità di trasmissione (1 o 2 Mbps) e dell'output power
	SPI1_readWriteReg(RF_WRITE_REG + RF_SETUP, 0x07);       // TX_PWR:0dBm, Datarate:1Mbps, // LNA:HCURR
        //scrittura nel registo di configurazione degli altri parametri necessari
	SPI1_readWriteReg(RF_WRITE_REG + CONFIG, 0x0e);         // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled
        
        CE_H(); // GPIO_SetBits // Set CE pin high
}
        
void TX_Mode_Adv(unsigned char* addr,
                 unsigned char addr_width,
                 unsigned char pload_leng,
                 unsigned char aa,
                 unsigned char rxaddr,
                 unsigned char set_retr,
                 unsigned char rfch,
                 unsigned char rfset,
                 unsigned char conf)
{
        CE_L(); //GPIO_ResetBits // Set CE pin low
        
        //scrittura nel registro dell'indirizzo di trasmissione e di ricezione
	SPI1_writeBuf(RF_WRITE_REG + TX_ADDR, addr, addr_width);
	SPI1_writeBuf(RF_WRITE_REG + RX_ADDR_P0, addr, addr_width);
        //Enable ‘Auto Acknowledgment'
	SPI1_readWriteReg(RF_WRITE_REG + EN_AA, aa);
        //Enabled RX Addresses
	SPI1_readWriteReg(RF_WRITE_REG + EN_RXADDR, rxaddr);
        //Setup ritrasmissione automatica
	SPI1_readWriteReg(RF_WRITE_REG + SETUP_RETR, set_retr);
        //Selezione del canale (0-125)
	SPI1_readWriteReg(RF_WRITE_REG + RF_CH, rfch);
        //Selezione della velocità di trasmissione (1 o 2 Mbps) e dell'output power
	SPI1_readWriteReg(RF_WRITE_REG + RF_SETUP, rfset);
        //scrittura nel registo di configurazione degli altri parametri necessari
	SPI1_readWriteReg(RF_WRITE_REG + CONFIG, conf);
        
        CE_H(); // GPIO_SetBits // Set CE pin high
}
