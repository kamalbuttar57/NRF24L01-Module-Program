/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NRF24L01_H
#define __NRF24L01_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx.h"
#include "stm32f3_discovery.h"
#include "main.h"
   
// SPI(nRF24L01) commands
#define RF_READ_REG     0x00  // Define read command to register
#define RF_WRITE_REG    0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

#define  RX_DR          ((sta>>6)&0X01)
#define  TX_DS          ((sta>>5)&0X01)
#define  MAX_RT         ((sta>>4)&0X01)

// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define DYNPD		0x1C  // Enable Dynamic payload length
#define FEATURE		0x1D  // Feature register
#define TX_ADR_WIDTH    5   // 5 bytes TX(RX) address width
#define TX_PLOAD_WIDTH  20  // 20 bytes TX payload   
   
void RX_Mode(void);
void RX_Mode_Adv(unsigned char *, unsigned char, unsigned char, unsigned char,
                 unsigned char, unsigned char, unsigned char, unsigned char);
void TX_Mode(void);
void TX_Mode_Adv(unsigned char*, unsigned char, unsigned char, unsigned char, 
                 unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
unsigned char SPI3_readReg(unsigned char);
unsigned char SPI3_readWriteReg(unsigned char, unsigned char);
unsigned char SPI3_readBuf(unsigned char, unsigned char *, unsigned char);
unsigned char SPI3_writeBuf(unsigned char, unsigned char *, unsigned char);

#endif /* __NRF24L01_H */
