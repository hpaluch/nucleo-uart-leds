/**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/main.c
  * @author  MCD Application Team
  * @brief   This example shows how to retarget the C library printf function
  *          to the UART.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup UART_Printf
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int hpstm_uart_initialized = 0;
/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

// get character from UART - it never returns error
char hpstm_getchar_from_uart(void){
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t c=0;

	while(1){
		c=0;
		ret = HAL_UART_Receive(&UartHandle,&c,1,0xffff);
		if (ret == HAL_OK){
			// printf("DEBUG: got character (0x%x) from UART\r\n",(unsigned)c);
			return (char)c;
		}
		printf("ERROR: receiving character from UART HAL_STATUS=%d\r\n"
				"Trying again after 1s delay",(int)ret);
		HAL_Delay(1000);
	}
}


// WARNING!
// We can't just redefine `int __io_getchar(void)`
// and then use scanf(3),
// because syscalls.c _read() function
//        is terribly broken- it is hardcoded to read always specified number of characters before return....

int hpstm_gets(char *buf, int bufSize,int echo){
	int n=0;

	while (n < bufSize-1){
		char c = hpstm_getchar_from_uart();
		if (echo && c !='\r' && c!='\n'){
			putchar(c);
			fflush(stdout);
		}
		buf[n] = c;
		n++;
		buf[n] = '\0';
		if (c == '\r' || c == '\n'){
			return n;
		}
	}
	return n;
}

#if 0
static void hpstm_dump_printable(char *str){
	char *p = NULL;

	for(p=str; *p; p++){
		if ( *p >=32 && *p <=126 ){
			putchar(*p);
		} else {
			printf("<0x%x>",(unsigned)*p);
		}
	}
}
#endif


static void hpstm_cmd_help(void){
	printf("Type one of following commands:\r\n");
	printf("help      - to show this help\r\n");
	printf("led1 on   - to turn green LED LD1 on\r\n");
	printf("led1 off  - to turn green LED LD1 off\r\n");
	printf("led2 on   - to turn blue  LED LD2 on\r\n");
	printf("led2 off  - to turn blue  LED LD2 off\r\n");
	printf("\r\nNOTE: command editing does NOT work.\r\n");
}

// parse "on" or "off" arguments
static int  hpstm_parse_switch(char *arg){
	if (strcasecmp(arg,"on")==0){
		return 1;
	}
	if (strcasecmp(arg,"off")==0){
		return 0;
	}
	return -1; // unknown argument
}

static void hpstm_cmd_with_arg(char *cmd, char *arg1){

	if (strcasecmp(cmd,"led1")==0 || strcasecmp(cmd,"led2")==0 ){
		Led_TypeDef ledNumber;
		int ledState = hpstm_parse_switch(arg1);
		if (ledState < 0){
			printf("ERROR: Invalid switch '%s' for '%s' command\r\n",arg1,cmd);
			return;
		}
		// ok handle leds
		int ledNo = (int)cmd[3] - '0';
		if (ledNo < 1 || ledNo > 2){
			printf("INTERNAL ERROR: unexpected LED number %d from '%c'\r\n",ledNo,cmd[3]);
			return;
		}
		if (ledNo == 2){
			ledNumber = LED2;
		} else {
			ledNumber = LED1;
		}
		// and finally control LEDx
		if (ledState){
			BSP_LED_On(ledNumber);
			printf("OK: LED%d turned ON\r\n",ledNo);
		} else {
			BSP_LED_Off(ledNumber);
			printf("OK: LED%d turned OFF\r\n",ledNo);
		}

	} else {
		printf("ERROR: Unknown command with argument '%s'\r\n",cmd);
	}

}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 216 MHz */
  SystemClock_Config();

  BSP_LED_Init(LED1); // green user LED
  BSP_LED_Init(LED2); // blue  user LED
  BSP_LED_Init(LED3); // red LED to signal fatal errors

  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : 
	                  BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_ODD;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  hpstm_uart_initialized = 1;

  /* Output a message on Hyperterminal using printf function */
  printf("\r\nSTM32F767ZI-Nucleo is ready for your commands.\r\nType 'help' for list of commands.\r\n");

  /* Infinite loop */
  while (1)
  {
	  char buf[20] = {0};
	  char cmd[8]  = {0};
	  char arg1[8] = {0};
	  int ret=0;

	  printf("nucleo> ");
	  fflush(stdout);
	  ret = hpstm_gets(buf,sizeof(buf),1);
	  printf("\r\n");
#if 0
	  printf("DEBUG: hpstmp_gets returned '");
	  hpstm_dump_printable(buf);
	  printf("' returned %d chars\r\n",ret);
#endif

	  ret = sscanf(buf,"%7s %7s",cmd,arg1);
	  //printf("DEBUG: scanf(3) returned=%d cmd='%s' arg1='%s'\r\n",ret,cmd,arg1);

	  if (ret <= 0){
		  printf("ERROR: No command entered, type 'help' for help\r\n");
	  } else if (ret == 1){
		  // handle command without argument.
		  if (strcasecmp(cmd,"help")==0){
			  hpstm_cmd_help();
		  } else {
			  printf("ERROR: Unknown command '%s'\r\n",cmd);
		  }
	  } else if (ret == 2){
		  hpstm_cmd_with_arg(cmd,arg1);
	  } else {
		  printf("ERROR: scanf() returned unexpected number of arguments: %d",ret);
	  }
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1) {};
  }
  
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    while(1) {};
  }
}
/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);

  if (hpstm_uart_initialized){
	  printf("ERROR: Fatal error occurred. System HALTED.\r\n");
  }

  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
