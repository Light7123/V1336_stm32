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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"
#include "time.h"
#include "sim.h"
#include "GlobalDataKeeper.h"
#include "task.h"
#include "queue.h"
#include "xmlParse.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
bool modem_flag=false;
bool init_flag=false;
char buffer[500];
int err_flag=0;
string mftest;
typedef struct
{
	union{
		uint16_t regaddres;
		struct{
			uint8_t regaddresLO;
			uint8_t regaddresHI;
		};
	};
	uint8_t func;
	string res;

} test;

test device[10];
void modem_status()
{

	uint32_t sim_start_Delay = 0;

	HAL_UART_Receive_DMA(&huart1,  (uint8_t*)buffer, 500);
	HAL_Delay(200);
	//	if(strstr(buffer,"LOG"))
	//	{
	//	HAL_UART_Transmit_DMA(&huart2,(uint8_t*)buffer, 500);
	//	}
	//else
	if(strstr(buffer,"OK"))
	{
		//UARTPrint("MODEM READY\r\n");
		//	glcd_int_puts(modem_ok, 0, 0);
		//			string c;
		//			memcpy(buffer,strstr((char*)buffer,"OK "),50);
		//			for(int i=3;i<100;i++)
		//			{
		//				if(buffer[i]!='\n')
		//				{
		//				c.push_back(buffer[i]);
		//				}
		//				else
		//				{
		//					break;
		//				}
		//			}
		//			pub_count=atoi(c.c_str())%10;
		//			if(pub_count<9)
		//			{
		//				pub_count++;
		//			}
		//			else
		//			{
		//				pub_count=0;
		//			}
//		if(pub_count<3)
//		{
//			pub_count++;
//		}
//		else
//		{
//			pub_count=0;
//		}
		modem_flag=true;
		memset(buffer, 9, sizeof(buffer));
	}
	else
	{
		//UARTPrint("MODEM ERROR\r\n");
		//glcd_int_puts(modem_error, 0, 0);
		//auth_flag=0;
		modem_flag = 0;
	}
	if(strstr(buffer,"INIT"))
	{
		init_flag=true;
		err_flag=0;
	}
	else
	{
		if(err_flag>15)
		{
			init_flag=false;
			err_flag=0;
		}
		err_flag++;
	}

	if(strstr(buffer,"*"))
	{
		if(pub_count<3)
		{
			pub_count++;
		}
		else
		{
			pub_count=0;
		}
		memset(buffer, 9, sizeof(buffer));
		err_flag=0;
	}
	else if(strstr(buffer,"+CIPRXGET:"))
	{
		//	memcpy(buffer,strstr(buffer,"999999"),200);


		int j=0;
		string mess;
		string topic;
		int len1=0;
		int len2=0;

		for(int i=0;i<180;i++)
		{
			if(buffer[i]=='9'&&buffer[i]=='9'&&buffer[i]=='9'&&buffer[i]=='9'&&buffer[i]=='9'&&buffer[i]=='9')
			{
				for(int ii=i;ii<180;ii++)
				{
					if(buffer[ii]=='/')
					{
						topic.push_back(buffer[ii+4]);
						topic.push_back(buffer[ii+5]);
						topic.push_back(buffer[ii+6]);
						topic.push_back(buffer[ii+7]);
						topic.push_back(buffer[ii+8]);
						topic.push_back(buffer[ii+9]);
						mess.push_back(buffer[ii+10]);
						mess.push_back(buffer[ii+11]);
						mess.push_back(buffer[ii+12]);
						mess.push_back(buffer[ii+13]);
						break;
					}
				}
				string reg = topic.substr(topic.find("x")+1, 4);
				std::stringstream ss;
				uint16_t add;
				ss << std::hex << reg;
				ss >> add;


				std::stringstream ss1;
				uint16_t data1;
				data1=atoi(mess.c_str());
				uint8_t data[2];
				data[0]=(data1>>8)&0xFF;
				data[1]=data1&0xFF;
				write_modbus(0x01,0x10, (add>>8)&0xFF,add&0xFF, 1,data);
				memset(buffer, 9, sizeof(buffer));
				break;
			}
		}

	}
	else
	{
		if(err_flag>15)
		{
			init_flag=false;
			err_flag=0;
		}
		err_flag++;
	}
	//	if(HAL_GetTick() > sim_start_Delay && status_SIM == 0) //ресетим модем если он завис
	//	{
	//		sim_start_Delay = HAL_GetTick()+ 30000;
	//		//UARTPrint("MODEM RESET\r\n");
	//		//ресетим модем
	//		//PC2 - on/off питание modem
	//		//PC3 - reset modem
	//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
	//		HAL_Delay(100);
	//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
	//	}
}
//void modem_status()
//{
//
//	uint32_t sim_start_Delay = 0;
//
//	HAL_UART_Receive_DMA(&huart1,  (uint8_t*)buffer, 5);
//	HAL_Delay(200);
////	if(strstr(buffer,"LOG"))
////	{
////	HAL_UART_Transmit_DMA(&huart2,(uint8_t*)buffer, 500);
////	}
//	//else
//		if(strstr(buffer,"OK"))
//	{
//		//UARTPrint("MODEM READY\r\n");
//		//	glcd_int_puts(modem_ok, 0, 0);
//			if(pub_count<10)
//			{
//				pub_count++;
//			}
//			else
//			{
//				pub_count=0;
//			}
//			memset(buffer, 9, sizeof(buffer));
//		modem_flag=true;
//	//	memset(buffer, 9, sizeof(buffer));
//	}
//	else
//	{
//		//UARTPrint("MODEM ERROR\r\n");
//		//glcd_int_puts(modem_error, 0, 0);
//		//auth_flag=0;
//		modem_flag = 0;
//	}
//	if(strstr(buffer,"INIT"))
//	{
//		init_flag=true;
//		err_flag=0;
//	}
//	else
//	{
//		if(err_flag>15)
//		{
//			init_flag=false;
//			err_flag=0;
//		}
//		err_flag++;
//	}
//
//	if(strstr(buffer,"PUB"))
//	{
//
//		memset(buffer, 9, sizeof(buffer));
//		err_flag=0;
//	}
//	else
//	{
//		if(err_flag>15)
//		{
//			init_flag=false;
//			err_flag=0;
//		}
//		err_flag++;
//	}
//	//	if(HAL_GetTick() > sim_start_Delay && status_SIM == 0) //ресетим модем если он завис
//	//	{
//	//		sim_start_Delay = HAL_GetTick()+ 30000;
//	//		//UARTPrint("MODEM RESET\r\n");
//	//		//ресетим модем
//	//		//PC2 - on/off питание modem
//	//		//PC3 - reset modem
//	//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
//	//		HAL_Delay(100);
//	//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
//	//	}
//}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
GlobalDataKeeper data;
string modemRx;
vector<device_conf>  dev __attribute__((section(".ccmram")));

//device_conf  dev[400] __attribute__((section(".ccmram")));
xQueueHandle modbus_to_mqtt_queue;
xQueueHandle mqtt_to_modbus_queue;


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
  MX_DMA_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */

	modbus_to_mqtt_queue =xQueueCreate( 128, sizeof( unsigned char ) );
	mqtt_to_modbus_queue=xQueueCreate( 128, sizeof( unsigned char ) );

  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

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
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

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
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PG8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
string res;
RTC_InitTypeDef rtc;
RTC_TimeTypeDef time_op;
int polled_reg=0;
int pub_count=0;


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */



	uint8_t buf[10];

	int read_modbus_count=0;
	device[0].regaddres=0x00FE;
	device[0].func=0x04;
	//	device[1].regaddres=0x0102;
	//	device[1].func=0x04;
	//	device[2].regaddresHI=0x01;
	//	device[2].regaddresLO=0x03;
	//	device[2].func=0x04;
	//	device[3].regaddresHI=0x01;
	//	device[3].regaddresLO=0x04;
	//	device[3].func=0x04;
	//	device[4].regaddresHI=0x01;
	//	device[4].regaddresLO=0x05;
	//	device[4].func=0x04;
	//	device[5].regaddresHI=0x00;
	//	device[5].regaddresLO=0xFF;
	//	device[5].func=0x04;
	//	device[6].regaddresHI=0x01;
	//	device[6].regaddresLO=0x01;
	//	device[6].func=0x04;
	device[1].regaddresHI=0x04;
	device[1].regaddresLO=0x0E;
	device[1].func=0x03;
	device[2].regaddresHI=0x04;
	device[2].regaddresLO=0x0D;
	device[2].func=0x03;
	device[3].regaddresHI=0x04;
	device[3].regaddresLO=0x0C;
	device[3].func=0x03;
	//	uint8_t datatest[2]={0xFF,0xFF};
	//	uint8_t regaddresHI[10]={0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x04,0x04,0x04};
	//	uint8_t regaddresLO[10]={0xFE,0x02,0x03,0x04,0x05,0xFF,0x01,0x0E,0x0D,0x0C};
	uint32_t del=0;
	/* Infinite loop */
	for(;;)
	{

		//write_modbus(0x01, 0x00, 0x01, 1, datatest);
		//osDelay(500);
		string topic;
		char top[10];
		modem_status();
		sprintf(top,"%01Xx%02X%02X",device[pub_count].func,device[pub_count].regaddresHI,device[pub_count].regaddresLO);
		//topic ="1001999/1/";
		topic ="999999/1/";
		topic+=top;

		//device[read_modbus_count].res=read_modbus(0x01,device[read_modbus_count].regaddresHI,device[read_modbus_count].regaddresLO,device[read_modbus_count].func, 1);
		if(read_modbus_count<3)
		{
			read_modbus_count++;
		}
		else
		{
			//mqtt_init("999999/Modem/MB_ConnStat", "OK","");
			read_modbus_count=0;
		}
		device[read_modbus_count].res=read_modbus(0x01,device[read_modbus_count].regaddresHI,device[read_modbus_count].regaddresLO,device[read_modbus_count].func, 1);
		if(modem_flag)
		{
			//char send_modem[512];
			mftest=device[pub_count].res;
			//mftest=device[pub_count].res;

			MQTT_SUB("999999/1//#");
			//sprintf(send_modem,"sub: %s\n\r",buffer_sub);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer_sub, sub_len+20);
			HAL_Delay(70);


			if(!mftest.empty())
			{
				sendMQTTConnect("bob","GG168jcbD");
				//sprintf(send_modem,"ini: %s\n\r",packet);
				HAL_UART_Transmit_DMA(&huart1, (uint8_t*)packet, packetSize+20);
				HAL_Delay(70);
				publishMessage(topic, mftest);
				//	sprintf(send_modem,"pub: %s\n\r",buffer_pub);
				HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer_pub, buflen+40);
				HAL_Delay(70);
			}


		}
		//		write_modbus(0x01, 0x00, 0x01, 1, datatest);
		//		osDelay(500);
		//		mftest=read_modbus(0x01, 0x00, 0x01, 1);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		osDelay(10);
	}
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
