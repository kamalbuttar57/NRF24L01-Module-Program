/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx.h"
#include "stm32f3_discovery.h"
#include "stm32f3xx_hal_spi.h"
#include "nRF24L01.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

/* Exported typedef ----------------------------------------------------------*/

// SPIx Communication boards Interface
#define SPI                             SPI1
#define GPIO_CS_CE                  	GPIOA
#define GPIO_Pin_CE              	GPIO_Pin_3
#define GPIO_Pin_CS              	GPIO_Pin_4
#define RCC_AHB1Periph_GPIO_CS_CE   	RCC_AHB1Periph_GPIOA

#define GPIO_SPI                        GPIOA
#define GPIO_Pin_SPI_IRQ                GPIO_Pin_2
#define GPIO_Pin_SPI_SCK      	        GPIO_Pin_5
#define GPIO_Pin_SPI_MISO               GPIO_Pin_6
#define GPIO_Pin_SPI_MOSI               GPIO_Pin_7
#define RCC_APBPeriph_SPI               RCC_APB2Periph_SPI1

#define GPIO_Pin_SPI_CS_SOURCE          GPIO_PinSource4
#define GPIO_Pin_SPI_SCK_SOURCE         GPIO_PinSource5
#define GPIO_Pin_SPI_MISO_SOURCE        GPIO_PinSource6
#define GPIO_Pin_SPI_MOSI_SOURCE        GPIO_PinSource7
#define RCC_AHB1Periph_GPIO_SPI         RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD
   
//Chip Enable Activates RX or TX mode
#define CE_H()          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1) 
#define CE_L()          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0)

//SPI Chip Select
#define CSN_H()         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,1)
#define CSN_L()         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,0)
   
// USARTx Communication boards Interface
#define RCC_AHB1Periph_GPIO_USART       RCC_AHB1Periph_GPIOD
#define GPIO_USART                      GPIOD
#define GPIO_Pin_USART_TX               GPIO_Pin_8
#define GPIO_Pin_USART_TX_SOURCE        GPIO_PinSource8

  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void USART3_Init(void);
void SPI1_Init(void);
void USART_test();

void TX_test();
void RX_test();

void receiveMode();

void print(char*);
void Delay_us(unsigned int);
void tsMessage (uint32_t , unsigned char *);
void spMessage (unsigned char *);
void checkMessage();

#endif /* __MAIN_H */
