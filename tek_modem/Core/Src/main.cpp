/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mqtt.h"
#include <stdarg.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

uint8_t rx_data[200];
int init_flag;
int i=0;
message_mqtt_recive mqtt_message;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void UARTPrint(const char *args, ...) //отладка идет в монетоприемник
{
	char StrBuff[1024];
	va_list ap;
	va_start(ap, args);
	char len = vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
	va_end(ap);
	int timeout = sizeof(StrBuff) / 3;
	HAL_UART_Transmit(&huart2,(uint8_t*)StrBuff,len,timeout);
}
void CSQ_status()
{
	  char buffer[512];
	  char ch[30];
	  Uart_sendstring("AT+CSQ\r\n");
	  HAL_Delay(50);
	  Get_after("",512, buffer);
	  for(int i =0;i<30;i++)
	  		{
	  			if(buffer[i]=='C'&&buffer[i+1]=='S'&&buffer[i+2]=='Q'&&buffer[i+3]==':')
	  			{
	  				int j=0;
	  				for(int ii=i+4;j<6;ii++)
	  				{
	  					ch[j]=buffer[ii];
	  					j++;
	  				}
	  				ch[j]=0;
	  				//ch[j]=0;
	  				break;
	  			}
	  		}

}
void modem_status()
{
	 char buffer[512];
		  char ch[30];
		  Uart_sendstring("AT+CPSI?\r\n");
		  HAL_Delay(50);
		  Get_after("",512, buffer);
		  for(int i =0;i<30;i++)
		  		{
		  			if(buffer[i]=='C'&&buffer[i+1]=='S'&&buffer[i+2]=='Q'&&buffer[i+3]==':')
		  			{
		  				int j=0;
		  				for(int ii=i+4;j<6;ii++)
		  				{
		  					ch[j]=buffer[ii];
		  					j++;
		  				}
		  				ch[j]=0;
		  				//ch[j]=0;
		  				break;
		  			}
		  		}
}
void modem_wait()
{
	HAL_UARTEx_ReceiveToIdle_IT(&huart2, rx_data, 200);
		  for(i=0;i<200;i++)
			  {
				  if(rx_data[i]=='+');
				  {
					 if((rx_data[i+1]=='i'&& rx_data[i+2]=='n'&& rx_data[i+3]=='n'&& rx_data[i+4]=='t'&& rx_data[i+5]==',')&&init_flag!=1)
					 {
						init_flag= modem_init();
					 }
					 if(rx_data[i+1]=='s'&& rx_data[i+2]=='e'&& rx_data[i+3]=='n'&& rx_data[i+4]=='d'&& rx_data[i+5]==',')
					 {
						 modem_send();
					 }
					 if(rx_data[i+1]=='s'&& rx_data[i+2]=='u'&& rx_data[i+3]=='b'&& rx_data[i+4]=='s'&& rx_data[i+5]==',')
					 {
						 modem_sub();
					 }
				  }
			  }
		  if(HAL_GetTick()%10000==0)
		  {
			  mqtt_message=Mqtt_Recive();
		  }
}

int modem_init()
{

	char serv[30];
	char user[30];


					 int j=0;
					 while(true)
					 {
						 if(rx_data[i+j+1]!=',')
						 {
						 serv[j]= rx_data[i+j+1];
						 }
						 else
						 {
							 break;
						 }
						 j++;
					 }
					 while(true)
					 {
						 if(rx_data[i+j+1]!=',')
						 {
						 user[j]= rx_data[i+j+1];
						 }
						 else
						 {
							 break;
						 }
						 j++;
					 }


	return  Mqtt_Init(serv, user);

}
void modem_send()
{
	char topic[50];
	char mess[100];


					 int j=0;
					 while(true)
					 {
						 if(rx_data[i+j+1]!=',')
						 {
						 topic[j]= rx_data[i+j+1];
						 }
						 else
						 {
							 break;
						 }
						 j++;
					 }
					 while(true)
					 {
						 if(rx_data[i+j+1]!=',')
						 {
						mess[j]= rx_data[i+j+1];
						 }
						 else
						 {
							 break;
						 }
						 j++;
					 }


	 Mqtt_Publish(topic, mess);
}
void modem_sub()
{
	char topic[50];
	 int j=0;


					 while(true)
					 {
						 if(rx_data[i+j+1]!=',')
						 {
						 topic[j]= rx_data[i+j+1];
						 }
						 else
						 {
							 break;
						 }
						 j++;
					 }



	  Mqtt_Subscribe(topic, j);
}
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  Ringbuf_init();
  Mqtt_Init("", "");
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  char data[512];
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
  HAL_Delay(500);
 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
 HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
//	  HAL_Delay(500);
//	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
//	  HAL_UART_Transmit_IT(&huart1, "+++\r\n", 4);
//	  HAL_Delay(50);
//
//	  HAL_UART_Receive(&huart1, data,10, 1000);
//	  HAL_UART_Transmit_IT(&huart1, "AT&F\r\n", 4);
//	  HAL_Delay(50);

	 // HAL_UART_Receive(&huart1, data,10, 1000);
	  //CSQ_status();
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
	  modem_wait();
//	  Uart_sendstring( "ATI\r\n");
//	  HAL_Delay(50);
//
//	  Get_after("",512, data);
//	  Uart_sendstring( "ATI\r\n");
//	  HAL_Delay(50);
//	  Get_after("",512, data);
	  HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t*)data, 512);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
