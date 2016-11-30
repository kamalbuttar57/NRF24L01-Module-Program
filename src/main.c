#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_accelerometer.h"
#include "stm32f3_discovery_gyroscope.h"
#include "main.h"

#include "common.h"


/* Private variables ---------------------------------------------------------*/
const Led_TypeDef LEDs[] = {LED3, LED4, LED5, LED6, LED7, LED8, LED9, LED10};
const uint32_t numLEDs = sizeof(LEDs)/sizeof(LEDs[0]);

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
unsigned char rx_buf[TX_PLOAD_WIDTH];
void TX_test();
void RX_test();
void TX_test2();
void RX_test2();
void Check_Rec();
void Check_Rec2();
void AB_test();
void Init_test(); 
void checkMessage();
void Read_Reg();

int main(int argc, char **argv)
{
  uint32_t i;
  uint8_t accelRc, gyroRc;
  /* Configure the system clock */
  SystemClock_Config();

  HAL_Init();
  // STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  //STM_EVAL_LEDInit(LED3);
  //STM_EVAL_LEDInit(LED4);
  //STM_EVAL_LEDInit(LED5);
  //STM_EVAL_LEDInit(LED6);

  /* Start the Watchdog */


  TerminalInit();  /* Initialize UART and USB */
  /* Configure the LEDs... */
  for(i=0; i<numLEDs; i++) {
    BSP_LED_Init(LEDs[i]);
  }

  /* Initialize the pushbutton */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  /* Initialize the Accelerometer */
  accelRc = BSP_ACCELERO_Init();
  if(accelRc != ACCELERO_OK) {
    printf("Failed to initialize acceleromter\n");
    Error_Handler();
  }

  /* Initialize the Gyroscope */
  gyroRc = BSP_GYRO_Init();
  if(gyroRc != GYRO_OK) {
    printf("Failed to initialize Gyroscope\n");
    Error_Handler();
  }

  my_Init();

  /* This is the main loop */  
  while(1) {
    TaskInput();
    my_Loop();
    /* Tickle the watchdog */
  }

  return 0;
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    Error_Handler();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

void Error_Handler(void)
{
  BSP_LED_On(LED6);
  /* Infinite loop */
  while(1)
  {
  }
}


void CmdTaskTest(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
    printf("code works");
    TX_test();
     
}

ADD_CMD("TestTX",CmdTaskTest,"  transmission successfull");

void CmdTaskTest2(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
    printf("code works");
    TX_test2();
     
}

ADD_CMD("TestTX2",CmdTaskTest2,"  transmission successfull");
void CmdRecTest(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
    printf("code OK");
    RX_test();
   // checkMessage();
     
}

ADD_CMD("TestRX",CmdRecTest,"  Reception successfull");


void CmdRecTest2(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
    printf("code OK");
    RX_test2();
    //checkMessage();
     
}
ADD_CMD("TestRX2",CmdRecTest2,"  Reception successfull");


void CmdMsgTest(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
    printf("ASk whether msg is received");
    checkMessage();
    //Check_Rec();
    
     
}

ADD_CMD("TestMX",CmdMsgTest,"  Reception Message successfull");

void CmdInitTest(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
     Init_test();  
}

ADD_CMD("TestInit",CmdInitTest,"  Initialization successfull");


void CmdRegTest(int mode)      
{
    uint32_t reg;
    if(mode != CMD_INTERACTIVE) return;
    reg = 0;
    fetch_uint32_arg(&reg);
    printf("Register 0x%02x : 0x%02x\n",
            (unsigned int)reg,    SPI3_readReg(reg));
        
}

ADD_CMD("TestRG",CmdRegTest,"  Register Value stored");

void CmdReg_Val(int mode)      
{
    if(mode != CMD_INTERACTIVE) return;
     Read_Reg(); 
}

ADD_CMD("TestVal",CmdReg_Val,"  register value ");

void CmdTestRegWrite(int mode)      
{
    uint32_t val;
    uint32_t reg;
    if(mode != CMD_INTERACTIVE) return;
    reg = 0;
    val = 0;
    fetch_uint32_arg(&reg);
    fetch_uint32_arg(&val);
    SPI3_readWriteReg(RF_WRITE_REG+reg, val);
    printf("Register 0x%02x : 0x%02x\n",
            (unsigned int)reg,    SPI3_readReg(reg));       
}
ADD_CMD("TestRegW",CmdTestRegWrite,"  register value ");


void SysTick_Handler(void)
{
    HAL_IncTick();
    my_Tick();
}

void CmdLED(int mode)
{
  uint32_t led,val;
  int rc;
  if(mode != CMD_INTERACTIVE) {
    return;
  }

  rc = fetch_uint32_arg(&led);
  if(rc) {
    printf("Missing LED index\n");
    return;
  }
    
  rc = fetch_uint32_arg(&val);
  if(rc) {
    printf("Missing state value, 0 for Off, 1 for On\n");
    return;
  }
  
  if((led < 3) || (led > 10)) {
    printf("Led index of %u is out of the range (3..10)\n",
	   (unsigned int)led);
    return;
  }

  led -= 3;
  if(val) {
    BSP_LED_On(LEDs[led]);
  } else {
    BSP_LED_Off(LEDs[led]);
  }

} 

ADD_CMD("led",CmdLED,"<index> <state> Turn off/on LED")

void CmdAccel(int mode)
{
  int16_t xyz[3];

  if(mode != CMD_INTERACTIVE) {
    return;
  }

  BSP_ACCELERO_GetXYZ(xyz);

  printf("Accelerometer returns:\n"
	 "   X: %d\n"
	 "   Y: %d\n"
	 "   Z: %d\n",
	 xyz[0],xyz[1],xyz[2]);


}

ADD_CMD("accel", CmdAccel,"                Read Accelerometer");

void CmdGyro(int mode)
{
  float xyz[3];

  if(mode != CMD_INTERACTIVE) {
    return;
  }

  BSP_GYRO_GetXYZ(xyz);

  printf("Gyroscope returns:\n"
	 "   X: %d\n"
	 "   Y: %d\n"
	 "   Z: %d\n",
	 (int)(xyz[0]*256),
	 (int)(xyz[1]*256),
	 (int)(xyz[2]*256));
}

ADD_CMD("gyro", CmdGyro,"                Read Gyroscope");

void CmdButton(int mode)
{
  uint32_t button;

  if(mode != CMD_INTERACTIVE) {
    return;
  }

  button = BSP_PB_GetState(BUTTON_USER);
  
  printf("Button is currently: %s\n",
	 button ? "Pressed" : "Released");

  return;
}

ADD_CMD("button", CmdButton,"                Print status of user button");
