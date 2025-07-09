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
char buffer[3200];
int err_flag=0;
string mftest;
uint32_t unixtime;
int tx_count=0;
int r_last=0;

char csq[6];//="23.99";
char netw[16]="Megafon";
bool imei_f=false;
bool csq_f=false;
bool netw_f=true;
char conf_buf[55000]__attribute__((section(".ccmram")));
char test_buffer[3200]__attribute__((section(".ccmram")));
int conf_buf_counter=0;
void start_read_conf()
{

}
void modem_status()
{

	string log_buf;

	uint32_t sim_start_Delay = 0;

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,  (uint8_t*)buffer, 3200);
	//HAL_UART_Receive_DMA(&huart1,  (uint8_t*)buffer, 3000);;
	//	HAL_UART_Transmit(&huart3, (uint8_t*)"FROM_SIM: ", 9,50);
	//	HAL_UART_Transmit(&huart3, (uint8_t*)buffer, 2000,300);
	//	HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);

	//osDelay(150);
	//	if(strstr(buffer,"LOG"))
	//	{
	//	HAL_UART_Transmit_DMA(&huart2,(uint8_t*)buffer, 500);
	//	}
	//else
	if(r_last>3180)
	{
		r_last=0;
		//memset(buffer,9,10000);
	}
	for(int r=0;r<3190;r++)
	{
		for(int i=0;i<3180;i++)
		{
			if(buffer[i]=='I'&&buffer[i+1]=='M'&&buffer[i+2]=='E'&&buffer[i+3]=='I'&&buffer[i+4]==':')
			{
				int t=0;
				for(int ii=i+5;t<18;ii++)
				{
					imei.push_back(buffer[ii]);
					t++;
				}
				r_last=r+6;
				imei_f=true;
			}
			if(buffer[i]=='N'&&buffer[i+1]=='E'&&buffer[i+2]=='T'&&buffer[i+3]=='W'&&buffer[i+4]==':')
			{
				int t=0;
				for(int ii=i+5;t<10;ii++)
				{
					netw[t]=buffer[ii];
					t++;
				}
				r_last=r+6;
				netw_f=true;
			}
			if(buffer[i]=='C'&&buffer[i+1]=='S'&&buffer[i+2]=='Q'&&buffer[i+3]=='S'&&buffer[i+4]==':')
			{
				int t=0;
				for(int ii=i+5;t<6;ii++)
				{
					csq[t]=buffer[ii];
					t++;
				}
				r_last=r+6;
				csq_f=true;
			}
		}
		if(buffer[r]=='+'&&buffer[r+1]=='C'&&buffer[r+2]=='I'&&buffer[r+3]=='P'&&buffer[r+4]=='R'&&buffer[r+5]=='X'&&buffer[r+6]=='G'&&buffer[r+7]=='E'&&buffer[r+8]=='T')
		{
			//memcpy(buffer,strstr(buffer,"999999"),200);

			memcpy(test_buffer,buffer,3200);
			int j=0;
			string mess;
			string topic;
			int len1=0;
			int len2=0;
			//			HAL_UART_Transmit(&huart3, (uint8_t*)"FROM_F0: ", 9,50);
			//			HAL_UART_Transmit(&huart3, (uint8_t*)buffer, 500,300);
			//			HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
			for(int jj=0;jj<3200;jj++)
			{
				if(test_buffer[jj]=='\0')
				{
					test_buffer[jj]='\t';
				}
			}
			for(int jj=0;jj<3190;jj++)
			{
				if(test_buffer[jj]=='+'&&test_buffer[jj+1]=='C'&&test_buffer[jj+2]=='I'&&test_buffer[jj+3]=='P'&&test_buffer[jj+4]=='R'&&test_buffer[jj+5]=='X'&&test_buffer[jj+6]=='G'&&test_buffer[jj+7]=='E'&&test_buffer[jj+8]=='T')
				{
					for(;jj<3190;jj++)
					{
						if(test_buffer[jj]=='\n')
						{
							jj++;
							break;
						}
					}
					for(;jj<3190;jj++)
					{
						if(test_buffer[jj]!='\n'&&test_buffer[jj]!='\t'&&test_buffer[jj]!='\0')
						{
							jj++;
							break;
						}
					}
					for(int ii=jj;ii<jj+400;ii++)
					{
						if(ii>3200)
						{
							break;
						}

						if(test_buffer[ii]=='+'&&test_buffer[ii+1]=='C'&&test_buffer[ii+2]=='I'&&test_buffer[ii+3]=='P'&&test_buffer[ii+4]=='R'&&test_buffer[ii+5]=='X'&&test_buffer[ii+6]=='G'&&test_buffer[ii+7]=='E'&&test_buffer[ii+8]=='T')
						{
							break;
						}
						conf_buf[conf_buf_counter]=test_buffer[ii];
						test_buffer[ii]='\t';
						conf_buf_counter++;
						if(conf_buf_counter>=55000)
						{
							conf_buf_counter=0;
							ReadFile(conf_buf);
							memset(conf_buf,9,55000);
						}
					}
					jj+=100;
				}
			}
			memset(buffer,9,3200);
			memset(test_buffer,9,3200);


//			for(int i=0;i<9980;i++)
//			{
//				if(buffer[i]=='9'&&buffer[i+1]=='9'&&buffer[i+2]=='9'&&buffer[i+3]=='9'&&buffer[i+4]=='9'&&buffer[i+5]=='9')
//				{
//					for(int ii=i;ii<9985;ii++)
//					{
//						if(buffer[ii]=='/')
//						{
//							topic.push_back(buffer[ii+4]);
//							topic.push_back(buffer[ii+5]);
//							topic.push_back(buffer[ii+6]);
//							topic.push_back(buffer[ii+7]);
//							topic.push_back(buffer[ii+8]);
//							topic.push_back(buffer[ii+9]);
//							mess.push_back(buffer[ii+10]);
//							mess.push_back(buffer[ii+11]);
//							mess.push_back(buffer[ii+12]);
//							mess.push_back(buffer[ii+13]);
//							mess.push_back(buffer[ii+14]);
//							break;
//						}
//						else
//							r_last=r+50;
//					}
//					string reg = topic.substr(topic.find("x")+1, 4);
//					std::stringstream ss;
//					uint16_t add;
//					ss << std::hex << reg;
//					ss >> add;
//
//
//					std::stringstream ss1;
//
//
//					uint16_t data1;
//					data1=atoi(mess.c_str());
//					if(mess.find(".")!= string::npos||mess.find(",")!= string::npos)
//					{
//						data1++;
//					}
//					uint8_t data[2];
//					data[0]=(data1>>8)&0xFF;
//					data[1]=data1&0xFF;
//					tx_count++;
//					memset(buffer,9,10000);
//					for(int i=0;i<3;i++)
//					{
//						write_modbus(0x01,0x10, (add>>8)&0xFF,add&0xFF, 1,data);
//					}
//					break;
//				}
//			}
			r_last=r+100;

			break;
		}
		if(buffer[r]=='O'&&buffer[r+1]=='K')
		{
			rx_counter++;
			modem_flag=true;
			buffer[r]=' ';
			buffer[r+1]=' ';
			//HAL_UART_Transmit(&huart3, (uint8_t*)"FROM_F0: OK",11 ,50);
			//	HAL_UART_Transmit(&huart3, (uint8_t*)buffer, 500,300);
			//	HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
			r_last=r+4;
			break;
		}
		if(buffer[r]=='I'&&buffer[r+1]=='N'&&buffer[r+2]=='I'&&buffer[r+3]=='T')
		{
			init_flag=true;
			err_flag=0;
			r_last=r+3;
			break;
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

		//		if(buffer[r]=='*')
		//		{
		//			HAL_UART_Transmit(&huart3, (uint8_t*)"FROM_F0: *",10,50);
		//			//HAL_UART_Transmit(&huart3, (uint8_t*)buffer, 500,300);
		//			HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
		//			if(pub_count<9)
		//			{
		//				pub_count++;
		//			}
		//			else
		//			{
		//				pub_count=0;
		//			}
		//			r_last=r+5;
		//			err_flag=0;
		//			break;
		//
		//		}
		if(buffer[r]=='t'&&buffer[r+1]=='i'&&buffer[r+2]=='m'&&buffer[r+3]=='e'&&buffer[r+4]==':')
		{
			char timestr[50];
			int t=0;
			while(buffer[r+5]!='\n')
			{
				timestr[t]=buffer[r+5];
				t++;
				r++;
			}
			unixtime=atoi(timestr);
			r_last=r+6;
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

}

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
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
		.name = "defaultTask",
		.stack_size = 7500 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
		.name = "myTask02",
		.stack_size = 512 * 4,
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
void StartTask02(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
GlobalDataKeeper data;
string modemRx;
//vector<device_conf>  dev __attribute__((section(".ccmram")));

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

	/* creation of myTask02 */
	myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

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
	huart1.Init.BaudRate = 460800;
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
	/* DMA2_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	/* DMA2_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	/* DMA2_Stream6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
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

void SetRTC(uint16_t yearC,uint8_t monthC,uint8_t dayC,uint8_t hourC,uint8_t minuteC,uint8_t secondC)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	sTime.Hours = hourC;
	sTime.Minutes = minuteC;
	sTime.Seconds = secondC;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	//sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
	sDate.Month = monthC;
	sDate.Date = dayC;
	sDate.Year = yearC;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

//static const std::string config_str = R"(<Configuration>
//		 <!--modem_template_parameters-->
//		 <Parameters>
//		 </Parameters>
//		 <!--modem_alias_template_parameters-->
//		 <Modem>
//		 </Modem>
//		 <!---->
//		 <!---->
//		 <!---->
//		  <Devices>
//		  <!--device_template_parameters-->
//		  <Device type="Modbus" address="1" guid="1">
//		   <!--device_alias_template_parameters-->
//		   <Variable publish="false" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r0w5dts0_group_0" write_func="5" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="0" alias="0x0201" address="513" deadband_percent="0" read_func="0" deadband_time_sec="0"/>
//		   <Variable publish="false" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r0w5dts0_group_1" write_func="5" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="0" alias="0x0203" address="515" deadband_percent="0" read_func="0" deadband_time_sec="0"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w0dts86400_group_0" write_func="0" mask="0" subscribe="false" byte_order="LittleEndian" data_type="uint32" deadband_value="1" alias="3x041B" address="1051" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w0dts86400_group_1" write_func="0" mask="0" subscribe="false" byte_order="LittleEndian" data_type="date_irz500" deadband_value="1" alias="3x041E" address="1054" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w0dts86400_group_1" write_func="0" mask="0" subscribe="false" byte_order="LittleEndian" data_type="date_irz500" deadband_value="1" alias="3x0422" address="1058" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w0dts86400_group_2" write_func="0" mask="0" subscribe="false" byte_order="LittleEndian" data_type="uint32" deadband_value="1" alias="3x0430" address="1072" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w16dts86400_group_0" write_func="16" mask="0" subscribe="true" byte_order="LittleEndian" data_type="time_irz500" deadband_value="1" alias="3x02FC" address="764" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="false" coef_b="0" update_time_sec="10" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts3600_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="Firmware_update" address="0" deadband_percent="0" read_func="3" deadband_time_sec="3600"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02F6" address="758" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02F7" address="759" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02F8" address="760" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02F9" address="761" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02FA" address="762" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_0" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02FB" address="763" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_1" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x02FF" address="767" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_1" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0300" address="768" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_1" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0301" address="769" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_1" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0302" address="770" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0388" address="904" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0389" address="905" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038A" address="906" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038B" address="907" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038C" address="908" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038D" address="909" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038E" address="910" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x038F" address="911" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0390" address="912" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0391" address="913" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0392" address="914" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0393" address="915" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0394" address="916" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0395" address="917" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0396" address="918" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="false" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0397" address="919" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0398" address="920" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x0399" address="921" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x039A" address="922" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_10" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x039B" address="923" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//		   <Variable publish="true" coef_b="0" update_time_sec="60" write_time_sec="0" coef_k="1" qos="0" group="r3w6dts86400_group_11" write_func="6" mask="0" subscribe="true" byte_order="LittleEndian" data_type="uint16" deadband_value="1" alias="3x03A0" address="928" deadband_percent="0" read_func="3" deadband_time_sec="86400"/>
//
//		  </Device>
//		 </Devices>
//		</Configuration>)";
int read_modbus_count=570;
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
int counter=570;
int slow_counter=0;
uint32_t tx_counter=0;
uint32_t rx_counter=0;
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask (void *argument)
{
	/* USER CODE BEGIN 5 */



	uint8_t buf[10];
	//parseConfigString(config_str);
	//	device[0].regaddres=0x00FE;
	//	device[0].func=0x04;
	//	device[0].pub_delay=10;
	//	device[1].regaddresHI=0x04;
	//	device[1].regaddresLO=0x0E;
	//	device[1].func=0x03;
	//	device[1].pub_delay=10;
	//	device[2].regaddresHI=0x04;
	//	device[2].regaddresLO=0x0D;
	//	device[2].func=0x03;
	//	device[2].pub_delay=10;
	//	device[3].regaddresHI=0x04;
	//	device[3].regaddresLO=0x0C;
	//	device[3].func=0x03;
	//	device[3].pub_delay=10;
	//	device[4].regaddres=0x0101;
	//	device[4].func=0x04;
	//	device[4].pub_delay=10;
	//	device[5].regaddresHI=0x01;
	//	device[5].regaddresLO=0x03;
	//	device[5].func=0x04;
	//	device[5].pub_delay=10;
	//	device[6].regaddresHI=0x01;
	//	device[6].regaddresLO=0x13;
	//	device[6].func=0x04;
	//	device[6].pub_delay=10;
	//	device[7].regaddresHI=0x01;
	//	device[7].regaddresLO=0x15;
	//	device[7].func=0x04;
	//	device[7].pub_delay=10;
	//	device[8].regaddresHI=0x03;
	//	device[8].regaddresLO=0x24;
	//	device[8].func=0x03;
	//	device[8].pub_delay=10;
	//	device[9].regaddresHI=0x03;
	//	device[9].regaddresLO=0x3F;
	//	device[9].func=0x03;
	//	device[9].pub_delay=10;
	//	device[10].regaddres=0x00EF;
	//	device[10].func=0x04;
	//	device[10].pub_delay=10;
	//	device[11].regaddres=0x00F4;
	//	device[11].func=0x04;
	//	device[11].pub_delay=10;
	//	device[12].regaddres=0x00FF;
	//	device[12].func=0x04;
	//	device[12].pub_delay=10;
	//	device[13].regaddres=0x010A;
	//	device[13].func=0x04;
	//	device[13].pub_delay=10;
	//	device[14].regaddres=0x010B;
	//	device[14].func=0x04;
	//	device[14].pub_delay=10;
	//	device[15].regaddres=0x010C;
	//	device[15].func=0x04;
	//	device[15].pub_delay=10;
	//	device[16].regaddres=0x010D;
	//	device[16].func=0x04;
	//	device[16].pub_delay=10;
	//	device[17].regaddres=0x0115;
	//	device[17].func=0x04;
	//	device[17].pub_delay=10;
	//	device[18].regaddres=0x0116;
	//	device[18].func=0x04;
	//	device[18].pub_delay=10;
	//	device[19].regaddres=0x00FC;
	//	device[19].func=0x04;
	//	device[19].pub_delay=10;

	device[0].regaddres=0x041B; device[0].func=3; device[0].pub_delay=60; device[0].write_func=0x10; device[0].group=0; device[0].type=1;
	device[1].regaddres=0x041E; device[1].func=3; device[1].pub_delay=60; device[1].write_func=0x10; device[1].group=1; device[1].type=5;
	device[2].regaddres=0x0422; device[2].func=3; device[2].pub_delay=60; device[2].write_func=0x10; device[2].group=1; device[2].type=5;
	device[3].regaddres=0x0430; device[3].func=3; device[3].pub_delay=60; device[3].write_func=0x10; device[3].group=2; device[3].type=1;
	device[4].regaddres=0x070A; device[4].func=3; device[4].pub_delay=60; device[4].write_func=0x10; device[4].group=3; device[4].type=0;
	device[5].regaddres=0x070B; device[5].func=3; device[5].pub_delay=60; device[5].write_func=0x10; device[5].group=3; device[5].type=0;
	device[6].regaddres=0x02FC; device[6].func=3; device[6].pub_delay=60; device[6].write_func=16; device[6].group=4; device[6].type=6;
	device[7].regaddres=0x0410; device[7].func=3; device[7].pub_delay=60; device[7].write_func=16; device[7].group=5; device[7].type=5;
	device[8].regaddres=0x063B; device[8].func=3; device[8].pub_delay=60; device[8].write_func=16; device[8].group=6; device[8].type=1;
	device[9].regaddres=0x063D; device[9].func=3; device[9].pub_delay=60; device[9].write_func=16; device[9].group=6; device[9].type=1;
	device[10].regaddres=0x0643; device[10].func=3; device[10].pub_delay=60; device[10].write_func=16; device[10].group=7; device[10].type=1;
	device[11].regaddres=0x0650; device[11].func=3; device[11].pub_delay=60; device[11].write_func=16; device[11].group=8; device[11].type=1;
	device[12].regaddres=0x0652; device[12].func=3; device[12].pub_delay=60; device[12].write_func=16; device[12].group=8; device[12].type=1;
	device[13].regaddres=0x0654; device[13].func=3; device[13].pub_delay=60; device[13].write_func=16; device[13].group=8; device[13].type=1;
	device[14].regaddres=0x0656; device[14].func=3; device[14].pub_delay=60; device[14].write_func=16; device[14].group=8; device[14].type=1;
	device[15].regaddres=0x0658; device[15].func=3; device[15].pub_delay=60; device[15].write_func=16; device[15].group=8; device[15].type=1;
	device[16].regaddres=0x065A; device[16].func=3; device[16].pub_delay=60; device[16].write_func=16; device[16].group=8; device[16].type=1;
	device[17].regaddres=0x065C; device[17].func=3; device[17].pub_delay=60; device[17].write_func=16; device[17].group=8; device[17].type=1;
	device[18].regaddres=0x065E; device[18].func=3; device[18].pub_delay=60; device[18].write_func=16; device[18].group=8; device[18].type=1;
	device[19].regaddres=0x0660; device[19].func=3; device[19].pub_delay=60; device[19].write_func=16; device[19].group=8; device[19].type=1;
	device[20].regaddres=0x0662; device[20].func=3; device[20].pub_delay=60; device[20].write_func=16; device[20].group=8; device[20].type=1;
	device[21].regaddres=0x0664; device[21].func=3; device[21].pub_delay=60; device[21].write_func=16; device[21].group=8; device[21].type=1;
	device[22].regaddres=0x0666; device[22].func=3; device[22].pub_delay=60; device[22].write_func=16; device[22].group=8; device[22].type=1;
	device[23].regaddres=0x0668; device[23].func=3; device[23].pub_delay=60; device[23].write_func=16; device[23].group=8; device[23].type=1;
	device[24].regaddres=0x066A; device[24].func=3; device[24].pub_delay=60; device[24].write_func=16; device[24].group=8; device[24].type=1;
	device[25].regaddres=0x066C; device[25].func=3; device[25].pub_delay=60; device[25].write_func=16; device[25].group=8; device[25].type=1;
	device[26].regaddres=0x066E; device[26].func=3; device[26].pub_delay=60; device[26].write_func=16; device[26].group=8; device[26].type=1;
	device[27].regaddres=0x0670; device[27].func=3; device[27].pub_delay=60; device[27].write_func=16; device[27].group=8; device[27].type=1;
	device[28].regaddres=0x0672; device[28].func=3; device[28].pub_delay=60; device[28].write_func=16; device[28].group=8; device[28].type=1;
	device[29].regaddres=0x0674; device[29].func=3; device[29].pub_delay=60; device[29].write_func=16; device[29].group=8; device[29].type=1;
	device[30].regaddres=0x0676; device[30].func=3; device[30].pub_delay=60; device[30].write_func=16; device[30].group=8; device[30].type=1;
	device[31].regaddres=0x0700; device[31].func=3; device[31].pub_delay=60; device[31].write_func=16; device[31].group=9; device[31].type=1;
	device[32].regaddres=0x0426; device[32].func=3; device[32].pub_delay=60; device[32].write_func=16; device[32].group=10; device[32].type=5;
	device[33].regaddres=0x0429; device[33].func=3; device[33].pub_delay=60; device[33].write_func=16; device[33].group=11; device[33].type=1;
	device[34].regaddres=0x0436; device[34].func=3; device[34].pub_delay=60; device[34].write_func=16; device[34].group=12; device[34].type=1;
	device[35].regaddres=0x0438; device[35].func=3; device[35].pub_delay=60; device[35].write_func=16; device[35].group=12; device[35].type=1;
	device[36].regaddres=0x043A; device[36].func=3; device[36].pub_delay=60; device[36].write_func=16; device[36].group=12; device[36].type=1;
	device[37].regaddres=0x0600; device[37].func=3; device[37].pub_delay=60; device[37].write_func=16; device[37].group=13; device[37].type=1;
	device[38].regaddres=0x0602; device[38].func=3; device[38].pub_delay=60; device[38].write_func=16; device[38].group=13; device[38].type=1;
	device[39].regaddres=0x0604; device[39].func=3; device[39].pub_delay=60; device[39].write_func=16; device[39].group=13; device[39].type=1;
	device[40].regaddres=0x0606; device[40].func=3; device[40].pub_delay=60; device[40].write_func=16; device[40].group=13; device[40].type=1;
	device[41].regaddres=0x0608; device[41].func=3; device[41].pub_delay=60; device[41].write_func=16; device[41].group=13; device[41].type=1;
	device[42].regaddres=0x060A; device[42].func=3; device[42].pub_delay=60; device[42].write_func=16; device[42].group=13; device[42].type=1;
	device[43].regaddres=0x060C; device[43].func=3; device[43].pub_delay=60; device[43].write_func=16; device[43].group=13; device[43].type=1;
	device[44].regaddres=0x060E; device[44].func=3; device[44].pub_delay=60; device[44].write_func=16; device[44].group=13; device[44].type=1;
	device[45].regaddres=0x0610; device[45].func=3; device[45].pub_delay=60; device[45].write_func=16; device[45].group=13; device[45].type=1;
	device[46].regaddres=0x0612; device[46].func=3; device[46].pub_delay=60; device[46].write_func=16; device[46].group=13; device[46].type=1;
	device[47].regaddres=0x0616; device[47].func=3; device[47].pub_delay=60; device[47].write_func=16; device[47].group=14; device[47].type=1;
	device[48].regaddres=0x0618; device[48].func=3; device[48].pub_delay=60; device[48].write_func=16; device[48].group=14; device[48].type=1;
	device[49].regaddres=0x061A; device[49].func=3; device[49].pub_delay=60; device[49].write_func=16; device[49].group=14; device[49].type=1;
	device[50].regaddres=0x061D; device[50].func=3; device[50].pub_delay=60; device[50].write_func=16; device[50].group=15; device[50].type=1;
	device[51].regaddres=0x0623; device[51].func=3; device[51].pub_delay=60; device[51].write_func=16; device[51].group=16; device[51].type=1;
	device[52].regaddres=0x0625; device[52].func=3; device[52].pub_delay=60; device[52].write_func=16; device[52].group=16; device[52].type=1;
	device[53].regaddres=0x0627; device[53].func=3; device[53].pub_delay=60; device[53].write_func=16; device[53].group=16; device[53].type=1;
	device[54].regaddres=0x0629; device[54].func=3; device[54].pub_delay=60; device[54].write_func=16; device[54].group=16; device[54].type=1;
	device[55].regaddres=0x062B; device[55].func=3; device[55].pub_delay=60; device[55].write_func=16; device[55].group=16; device[55].type=1;
	device[56].regaddres=0x062D; device[56].func=3; device[56].pub_delay=60; device[56].write_func=16; device[56].group=16; device[56].type=1;
	device[57].regaddres=0x062F; device[57].func=3; device[57].pub_delay=60; device[57].write_func=16; device[57].group=16; device[57].type=1;
	device[58].regaddres=0x0631; device[58].func=3; device[58].pub_delay=60; device[58].write_func=16; device[58].group=16; device[58].type=1;
	device[59].regaddres=0x0634; device[59].func=3; device[59].pub_delay=60; device[59].write_func=16; device[59].group=17; device[59].type=1;
	device[60].regaddres=0x0001; device[60].func=3; device[60].pub_delay=60; device[60].write_func=0x10; device[60].group=18; device[60].type=0;
	device[61].regaddres=0x02C9; device[61].func=3; device[61].pub_delay=60; device[61].write_func=0x10; device[61].group=19; device[61].type=0;
	device[62].regaddres=0x02CA; device[62].func=3; device[62].pub_delay=60; device[62].write_func=0x10; device[62].group=19; device[62].type=0;
	device[63].regaddres=0x02CB; device[63].func=3; device[63].pub_delay=60; device[63].write_func=0x10; device[63].group=19; device[63].type=0;
	device[64].regaddres=0x02CC; device[64].func=3; device[64].pub_delay=60; device[64].write_func=0x10; device[64].group=19; device[64].type=0;
	device[65].regaddres=0x02CD; device[65].func=3; device[65].pub_delay=60; device[65].write_func=0x10; device[65].group=19; device[65].type=0;
	device[66].regaddres=0x02CE; device[66].func=3; device[66].pub_delay=60; device[66].write_func=0x10; device[66].group=19; device[66].type=0;
	device[67].regaddres=0x02CF; device[67].func=3; device[67].pub_delay=60; device[67].write_func=0x10; device[67].group=19; device[67].type=0;
	device[68].regaddres=0x02D0; device[68].func=3; device[68].pub_delay=60; device[68].write_func=0x10; device[68].group=19; device[68].type=0;
	device[69].regaddres=0x02D1; device[69].func=3; device[69].pub_delay=60; device[69].write_func=0x10; device[69].group=19; device[69].type=0;
	device[70].regaddres=0x02D2; device[70].func=3; device[70].pub_delay=60; device[70].write_func=0x10; device[70].group=19; device[70].type=0;
	device[71].regaddres=0x02D3; device[71].func=3; device[71].pub_delay=60; device[71].write_func=0x10; device[71].group=19; device[71].type=0;
	device[72].regaddres=0x02D4; device[72].func=3; device[72].pub_delay=60; device[72].write_func=0x10; device[72].group=19; device[72].type=2;
	device[73].regaddres=0x02D5; device[73].func=3; device[73].pub_delay=60; device[73].write_func=0x10; device[73].group=19; device[73].type=2;
	device[74].regaddres=0x02D6; device[74].func=3; device[74].pub_delay=60; device[74].write_func=0x10; device[74].group=19; device[74].type=2;
	device[75].regaddres=0x02D7; device[75].func=3; device[75].pub_delay=60; device[75].write_func=0x10; device[75].group=19; device[75].type=2;
	device[76].regaddres=0x02D8; device[76].func=3; device[76].pub_delay=60; device[76].write_func=0x10; device[76].group=19; device[76].type=2;
	device[77].regaddres=0x02D9; device[77].func=3; device[77].pub_delay=60; device[77].write_func=0x10; device[77].group=19; device[77].type=2;
	device[78].regaddres=0x02DA; device[78].func=3; device[78].pub_delay=60; device[78].write_func=0x10; device[78].group=19; device[78].type=2;
	device[79].regaddres=0x02DB; device[79].func=3; device[79].pub_delay=60; device[79].write_func=0x10; device[79].group=19; device[79].type=2;
	device[80].regaddres=0x02DC; device[80].func=3; device[80].pub_delay=60; device[80].write_func=0x10; device[80].group=19; device[80].type=2;
	device[81].regaddres=0x02DD; device[81].func=3; device[81].pub_delay=60; device[81].write_func=0x10; device[81].group=19; device[81].type=0;
	device[82].regaddres=0x02DE; device[82].func=3; device[82].pub_delay=60; device[82].write_func=0x10; device[82].group=19; device[82].type=0;
	device[83].regaddres=0x02DF; device[83].func=3; device[83].pub_delay=60; device[83].write_func=0x10; device[83].group=19; device[83].type=2;
	device[84].regaddres=0x02E0; device[84].func=3; device[84].pub_delay=60; device[84].write_func=0x10; device[84].group=19; device[84].type=2;
	device[85].regaddres=0x02E1; device[85].func=3; device[85].pub_delay=60; device[85].write_func=0x10; device[85].group=19; device[85].type=0;
	device[86].regaddres=0x02E2; device[86].func=3; device[86].pub_delay=60; device[86].write_func=0x10; device[86].group=19; device[86].type=0;
	device[87].regaddres=0x02E3; device[87].func=3; device[87].pub_delay=60; device[87].write_func=0x10; device[87].group=19; device[87].type=2;
	device[88].regaddres=0x02E4; device[88].func=3; device[88].pub_delay=60; device[88].write_func=0x10; device[88].group=19; device[88].type=2;
	device[89].regaddres=0x02E5; device[89].func=3; device[89].pub_delay=60; device[89].write_func=0x10; device[89].group=19; device[89].type=2;
	device[90].regaddres=0x02E6; device[90].func=3; device[90].pub_delay=60; device[90].write_func=0x10; device[90].group=19; device[90].type=0;
	device[91].regaddres=0x02E7; device[91].func=3; device[91].pub_delay=60; device[91].write_func=0x10; device[91].group=19; device[91].type=0;
	device[92].regaddres=0x02E8; device[92].func=3; device[92].pub_delay=60; device[92].write_func=0x10; device[92].group=19; device[92].type=0;
	device[93].regaddres=0x02E9; device[93].func=3; device[93].pub_delay=60; device[93].write_func=0x10; device[93].group=19; device[93].type=0;
	device[94].regaddres=0x02EA; device[94].func=3; device[94].pub_delay=60; device[94].write_func=0x10; device[94].group=19; device[94].type=0;
	device[95].regaddres=0x02EB; device[95].func=3; device[95].pub_delay=60; device[95].write_func=0x10; device[95].group=19; device[95].type=0;
	device[96].regaddres=0x02EC; device[96].func=3; device[96].pub_delay=60; device[96].write_func=0x10; device[96].group=19; device[96].type=0;
	device[97].regaddres=0x02ED; device[97].func=3; device[97].pub_delay=60; device[97].write_func=0x10; device[97].group=19; device[97].type=0;
	device[98].regaddres=0x02EE; device[98].func=3; device[98].pub_delay=60; device[98].write_func=0x10; device[98].group=19; device[98].type=0;
	device[99].regaddres=0x02EF; device[99].func=3; device[99].pub_delay=60; device[99].write_func=0x10; device[99].group=19; device[99].type=0;
	device[100].regaddres=0x02F0; device[100].func=3; device[100].pub_delay=60; device[100].write_func=0x10; device[100].group=19; device[100].type=0;
	device[101].regaddres=0x02F1; device[101].func=3; device[101].pub_delay=60; device[101].write_func=0x10; device[101].group=19; device[101].type=0;
	device[102].regaddres=0x02F2; device[102].func=3; device[102].pub_delay=60; device[102].write_func=0x10; device[102].group=19; device[102].type=0;
	device[103].regaddres=0x02F3; device[103].func=3; device[103].pub_delay=60; device[103].write_func=0x10; device[103].group=19; device[103].type=0;
	device[104].regaddres=0x02F4; device[104].func=3; device[104].pub_delay=60; device[104].write_func=0x10; device[104].group=19; device[104].type=0;
	device[105].regaddres=0x02F5; device[105].func=3; device[105].pub_delay=60; device[105].write_func=0x10; device[105].group=19; device[105].type=0;
	device[106].regaddres=0x02F6; device[106].func=3; device[106].pub_delay=60; device[106].write_func=0x10; device[106].group=19; device[106].type=0;
	device[107].regaddres=0x02F7; device[107].func=3; device[107].pub_delay=60; device[107].write_func=0x10; device[107].group=19; device[107].type=0;
	device[108].regaddres=0x02F8; device[108].func=3; device[108].pub_delay=60; device[108].write_func=0x10; device[108].group=19; device[108].type=0;
	device[109].regaddres=0x02F9; device[109].func=3; device[109].pub_delay=60; device[109].write_func=0x10; device[109].group=19; device[109].type=0;
	device[110].regaddres=0x02FA; device[110].func=3; device[110].pub_delay=60; device[110].write_func=0x10; device[110].group=19; device[110].type=0;
	device[111].regaddres=0x02FB; device[111].func=3; device[111].pub_delay=60; device[111].write_func=0x10; device[111].group=19; device[111].type=0;
	device[112].regaddres=0x037A; device[112].func=3; device[112].pub_delay=60; device[112].write_func=0x10; device[112].group=20; device[112].type=0;
	device[113].regaddres=0x037B; device[113].func=3; device[113].pub_delay=60; device[113].write_func=0x10; device[113].group=20; device[113].type=0;
	device[114].regaddres=0x037C; device[114].func=3; device[114].pub_delay=60; device[114].write_func=0x10; device[114].group=20; device[114].type=0;
	device[115].regaddres=0x037D; device[115].func=3; device[115].pub_delay=60; device[115].write_func=0x10; device[115].group=20; device[115].type=0;
	device[116].regaddres=0x037E; device[116].func=3; device[116].pub_delay=60; device[116].write_func=0x10; device[116].group=20; device[116].type=0;
	device[117].regaddres=0x037F; device[117].func=3; device[117].pub_delay=60; device[117].write_func=0x10; device[117].group=20; device[117].type=0;
	device[118].regaddres=0x0380; device[118].func=3; device[118].pub_delay=60; device[118].write_func=0x10; device[118].group=20; device[118].type=0;
	device[119].regaddres=0x0381; device[119].func=3; device[119].pub_delay=60; device[119].write_func=0x10; device[119].group=20; device[119].type=0;
	device[120].regaddres=0x0382; device[120].func=3; device[120].pub_delay=60; device[120].write_func=0x10; device[120].group=20; device[120].type=0;
	device[121].regaddres=0x0383; device[121].func=3; device[121].pub_delay=60; device[121].write_func=0x10; device[121].group=20; device[121].type=0;
	device[122].regaddres=0x0384; device[122].func=3; device[122].pub_delay=60; device[122].write_func=0x10; device[122].group=20; device[122].type=0;
	device[123].regaddres=0x0385; device[123].func=3; device[123].pub_delay=60; device[123].write_func=0x10; device[123].group=20; device[123].type=0;
	device[124].regaddres=0x0386; device[124].func=3; device[124].pub_delay=60; device[124].write_func=0x10; device[124].group=20; device[124].type=0;
	device[125].regaddres=0x0388; device[125].func=3; device[125].pub_delay=60; device[125].write_func=0x10; device[125].group=21; device[125].type=0;
	device[126].regaddres=0x0389; device[126].func=3; device[126].pub_delay=60; device[126].write_func=0x10; device[126].group=21; device[126].type=0;
	device[127].regaddres=0x038A; device[127].func=3; device[127].pub_delay=60; device[127].write_func=0x10; device[127].group=21; device[127].type=0;
	device[128].regaddres=0x038B; device[128].func=3; device[128].pub_delay=60; device[128].write_func=0x10; device[128].group=21; device[128].type=0;
	device[129].regaddres=0x038C; device[129].func=3; device[129].pub_delay=60; device[129].write_func=0x10; device[129].group=21; device[129].type=0;
	device[130].regaddres=0x038D; device[130].func=3; device[130].pub_delay=60; device[130].write_func=0x10; device[130].group=21; device[130].type=0;
	device[131].regaddres=0x038E; device[131].func=3; device[131].pub_delay=60; device[131].write_func=0x10; device[131].group=21; device[131].type=0;
	device[132].regaddres=0x038F; device[132].func=3; device[132].pub_delay=60; device[132].write_func=0x10; device[132].group=21; device[132].type=0;
	device[133].regaddres=0x0390; device[133].func=3; device[133].pub_delay=60; device[133].write_func=0x10; device[133].group=21; device[133].type=0;
	device[134].regaddres=0x0391; device[134].func=3; device[134].pub_delay=60; device[134].write_func=0x10; device[134].group=21; device[134].type=0;
	device[135].regaddres=0x0392; device[135].func=3; device[135].pub_delay=60; device[135].write_func=0x10; device[135].group=21; device[135].type=0;
	device[136].regaddres=0x0393; device[136].func=3; device[136].pub_delay=60; device[136].write_func=0x10; device[136].group=21; device[136].type=0;
	device[137].regaddres=0x0394; device[137].func=3; device[137].pub_delay=60; device[137].write_func=0x10; device[137].group=21; device[137].type=0;
	device[138].regaddres=0x0395; device[138].func=3; device[138].pub_delay=60; device[138].write_func=0x10; device[138].group=21; device[138].type=0;
	device[139].regaddres=0x0396; device[139].func=3; device[139].pub_delay=60; device[139].write_func=0x10; device[139].group=21; device[139].type=0;
	device[140].regaddres=0x0397; device[140].func=3; device[140].pub_delay=60; device[140].write_func=0x10; device[140].group=21; device[140].type=0;
	device[141].regaddres=0x0398; device[141].func=3; device[141].pub_delay=60; device[141].write_func=0x10; device[141].group=21; device[141].type=0;
	device[142].regaddres=0x0399; device[142].func=3; device[142].pub_delay=60; device[142].write_func=0x10; device[142].group=21; device[142].type=0;
	device[143].regaddres=0x039A; device[143].func=3; device[143].pub_delay=60; device[143].write_func=0x10; device[143].group=21; device[143].type=0;
	device[144].regaddres=0x039A; device[144].func=3; device[144].pub_delay=60; device[144].write_func=0x10; device[144].group=21; device[144].type=0;
	device[145].regaddres=0x03A0; device[145].func=3; device[145].pub_delay=60; device[145].write_func=0x10; device[145].group=22; device[145].type=0;
	device[146].regaddres=0x03A2; device[146].func=3; device[146].pub_delay=60; device[146].write_func=0x10; device[146].group=23; device[146].type=0;
	device[147].regaddres=0x03A3; device[147].func=3; device[147].pub_delay=60; device[147].write_func=0x10; device[147].group=23; device[147].type=0;
	device[148].regaddres=0x03A4; device[148].func=3; device[148].pub_delay=60; device[148].write_func=0x10; device[148].group=23; device[148].type=0;
	device[149].regaddres=0x03A5; device[149].func=3; device[149].pub_delay=60; device[149].write_func=0x10; device[149].group=23; device[149].type=0;
	device[150].regaddres=0x03A6; device[150].func=3; device[150].pub_delay=60; device[150].write_func=0x10; device[150].group=23; device[150].type=0;
	device[151].regaddres=0x03A7; device[151].func=3; device[151].pub_delay=60; device[151].write_func=0x10; device[151].group=23; device[151].type=0;
	device[152].regaddres=0x03A8; device[152].func=3; device[152].pub_delay=60; device[152].write_func=0x10; device[152].group=23; device[152].type=0;
	device[153].regaddres=0x03A9; device[153].func=3; device[153].pub_delay=60; device[153].write_func=0x10; device[153].group=23; device[153].type=0;
	device[154].regaddres=0x03AA; device[154].func=3; device[154].pub_delay=60; device[154].write_func=0x10; device[154].group=23; device[154].type=0;
	device[155].regaddres=0x03AB; device[155].func=3; device[155].pub_delay=60; device[155].write_func=0x10; device[155].group=23; device[155].type=0;
	device[156].regaddres=0x03AC; device[156].func=3; device[156].pub_delay=60; device[156].write_func=0x10; device[156].group=23; device[156].type=0;
	device[157].regaddres=0x03AD; device[157].func=3; device[157].pub_delay=60; device[157].write_func=0x10; device[157].group=23; device[157].type=0;
	device[158].regaddres=0x03AE; device[158].func=3; device[158].pub_delay=60; device[158].write_func=0x10; device[158].group=23; device[158].type=0;
	device[159].regaddres=0x03AF; device[159].func=3; device[159].pub_delay=60; device[159].write_func=0x10; device[159].group=23; device[159].type=0;
	device[160].regaddres=0x03B1; device[160].func=3; device[160].pub_delay=60; device[160].write_func=0x10; device[160].group=24; device[160].type=0;
	device[161].regaddres=0x03B2; device[161].func=3; device[161].pub_delay=60; device[161].write_func=0x10; device[161].group=24; device[161].type=0;
	device[162].regaddres=0x03B5; device[162].func=3; device[162].pub_delay=60; device[162].write_func=0x10; device[162].group=25; device[162].type=0;
	device[163].regaddres=0x03B6; device[163].func=3; device[163].pub_delay=60; device[163].write_func=0x10; device[163].group=25; device[163].type=0;
	device[164].regaddres=0x03B7; device[164].func=3; device[164].pub_delay=60; device[164].write_func=0x10; device[164].group=25; device[164].type=0;
	device[165].regaddres=0x03B8; device[165].func=3; device[165].pub_delay=60; device[165].write_func=0x10; device[165].group=25; device[165].type=0;
	device[166].regaddres=0x03B9; device[166].func=3; device[166].pub_delay=60; device[166].write_func=0x10; device[166].group=25; device[166].type=0;
	device[167].regaddres=0x03BA; device[167].func=3; device[167].pub_delay=60; device[167].write_func=0x10; device[167].group=25; device[167].type=0;
	device[168].regaddres=0x03BB; device[168].func=3; device[168].pub_delay=60; device[168].write_func=0x10; device[168].group=25; device[168].type=0;
	device[169].regaddres=0x03BC; device[169].func=3; device[169].pub_delay=60; device[169].write_func=0x10; device[169].group=25; device[169].type=0;
	device[170].regaddres=0x03BD; device[170].func=3; device[170].pub_delay=60; device[170].write_func=0x10; device[170].group=25; device[170].type=0;
	device[171].regaddres=0x03BE; device[171].func=3; device[171].pub_delay=60; device[171].write_func=0x10; device[171].group=25; device[171].type=0;
	device[172].regaddres=0x03BF; device[172].func=3; device[172].pub_delay=60; device[172].write_func=0x10; device[172].group=25; device[172].type=0;
	device[173].regaddres=0x03C0; device[173].func=3; device[173].pub_delay=60; device[173].write_func=0x10; device[173].group=25; device[173].type=0;
	device[174].regaddres=0x03C1; device[174].func=3; device[174].pub_delay=60; device[174].write_func=0x10; device[174].group=25; device[174].type=0;
	device[175].regaddres=0x03C2; device[175].func=3; device[175].pub_delay=60; device[175].write_func=0x10; device[175].group=25; device[175].type=0;
	device[176].regaddres=0x03C3; device[176].func=3; device[176].pub_delay=60; device[176].write_func=0x10; device[176].group=25; device[176].type=0;
	device[177].regaddres=0x03C4; device[177].func=3; device[177].pub_delay=60; device[177].write_func=0x10; device[177].group=25; device[177].type=0;
	device[178].regaddres=0x03C5; device[178].func=3; device[178].pub_delay=60; device[178].write_func=0x10; device[178].group=25; device[178].type=0;
	device[179].regaddres=0x03C6; device[179].func=3; device[179].pub_delay=60; device[179].write_func=0x10; device[179].group=25; device[179].type=0;
	device[180].regaddres=0x03C7; device[180].func=3; device[180].pub_delay=60; device[180].write_func=0x10; device[180].group=25; device[180].type=0;
	device[181].regaddres=0x03C8; device[181].func=3; device[181].pub_delay=60; device[181].write_func=0x10; device[181].group=25; device[181].type=0;
	device[182].regaddres=0x03C9; device[182].func=3; device[182].pub_delay=60; device[182].write_func=0x10; device[182].group=25; device[182].type=0;
	device[183].regaddres=0x03CA; device[183].func=3; device[183].pub_delay=60; device[183].write_func=0x10; device[183].group=25; device[183].type=0;
	device[184].regaddres=0x03CB; device[184].func=3; device[184].pub_delay=60; device[184].write_func=0x10; device[184].group=25; device[184].type=0;
	device[185].regaddres=0x03CC; device[185].func=3; device[185].pub_delay=60; device[185].write_func=0x10; device[185].group=25; device[185].type=0;
	device[186].regaddres=0x03CD; device[186].func=3; device[186].pub_delay=60; device[186].write_func=0x10; device[186].group=25; device[186].type=0;
	device[187].regaddres=0x03CE; device[187].func=3; device[187].pub_delay=60; device[187].write_func=0x10; device[187].group=25; device[187].type=0;
	device[188].regaddres=0x03CF; device[188].func=3; device[188].pub_delay=60; device[188].write_func=0x10; device[188].group=25; device[188].type=0;
	device[189].regaddres=0x03D0; device[189].func=3; device[189].pub_delay=60; device[189].write_func=0x10; device[189].group=25; device[189].type=0;
	device[190].regaddres=0x03D1; device[190].func=3; device[190].pub_delay=60; device[190].write_func=0x10; device[190].group=25; device[190].type=0;
	device[191].regaddres=0x03D2; device[191].func=3; device[191].pub_delay=60; device[191].write_func=0x10; device[191].group=25; device[191].type=0;
	device[192].regaddres=0x03D3; device[192].func=3; device[192].pub_delay=60; device[192].write_func=0x10; device[192].group=25; device[192].type=0;
	device[193].regaddres=0x03D4; device[193].func=3; device[193].pub_delay=60; device[193].write_func=0x10; device[193].group=25; device[193].type=0;
	device[194].regaddres=0x03D5; device[194].func=3; device[194].pub_delay=60; device[194].write_func=0x10; device[194].group=25; device[194].type=0;
	device[195].regaddres=0x03D6; device[195].func=3; device[195].pub_delay=60; device[195].write_func=0x10; device[195].group=25; device[195].type=0;
	device[196].regaddres=0x03D8; device[196].func=3; device[196].pub_delay=60; device[196].write_func=0x10; device[196].group=26; device[196].type=0;
	device[197].regaddres=0x03D9; device[197].func=3; device[197].pub_delay=60; device[197].write_func=0x10; device[197].group=26; device[197].type=0;
	device[198].regaddres=0x03DA; device[198].func=3; device[198].pub_delay=60; device[198].write_func=0x10; device[198].group=26; device[198].type=0;
	device[199].regaddres=0x03DB; device[199].func=3; device[199].pub_delay=60; device[199].write_func=0x10; device[199].group=26; device[199].type=0;
	device[200].regaddres=0x03DC; device[200].func=3; device[200].pub_delay=60; device[200].write_func=0x10; device[200].group=26; device[200].type=0;
	device[201].regaddres=0x03DD; device[201].func=3; device[201].pub_delay=60; device[201].write_func=0x10; device[201].group=26; device[201].type=0;
	device[202].regaddres=0x03DE; device[202].func=3; device[202].pub_delay=60; device[202].write_func=0x10; device[202].group=26; device[202].type=0;
	device[203].regaddres=0x03DF; device[203].func=3; device[203].pub_delay=60; device[203].write_func=0x10; device[203].group=26; device[203].type=0;
	device[204].regaddres=0x03E0; device[204].func=3; device[204].pub_delay=60; device[204].write_func=0x10; device[204].group=26; device[204].type=0;
	device[205].regaddres=0x03E1; device[205].func=3; device[205].pub_delay=60; device[205].write_func=0x10; device[205].group=26; device[205].type=0;
	device[206].regaddres=0x03E2; device[206].func=3; device[206].pub_delay=60; device[206].write_func=0x10; device[206].group=26; device[206].type=0;
	device[207].regaddres=0x03E3; device[207].func=3; device[207].pub_delay=60; device[207].write_func=0x10; device[207].group=26; device[207].type=0;
	device[208].regaddres=0x03E4; device[208].func=3; device[208].pub_delay=60; device[208].write_func=0x10; device[208].group=26; device[208].type=0;
	device[209].regaddres=0x03E5; device[209].func=3; device[209].pub_delay=60; device[209].write_func=0x10; device[209].group=26; device[209].type=0;
	device[210].regaddres=0x03E6; device[210].func=3; device[210].pub_delay=60; device[210].write_func=0x10; device[210].group=26; device[210].type=0;
	device[211].regaddres=0x03E7; device[211].func=3; device[211].pub_delay=60; device[211].write_func=0x10; device[211].group=26; device[211].type=0;
	device[212].regaddres=0x03E8; device[212].func=3; device[212].pub_delay=60; device[212].write_func=0x10; device[212].group=26; device[212].type=0;
	device[213].regaddres=0x03E9; device[213].func=3; device[213].pub_delay=60; device[213].write_func=0x10; device[213].group=26; device[213].type=0;
	device[214].regaddres=0x03EA; device[214].func=3; device[214].pub_delay=60; device[214].write_func=0x10; device[214].group=26; device[214].type=0;
	device[215].regaddres=0x03EB; device[215].func=3; device[215].pub_delay=60; device[215].write_func=0x10; device[215].group=26; device[215].type=0;
	device[216].regaddres=0x03EC; device[216].func=3; device[216].pub_delay=60; device[216].write_func=0x10; device[216].group=26; device[216].type=0;
	device[217].regaddres=0x03ED; device[217].func=3; device[217].pub_delay=60; device[217].write_func=0x10; device[217].group=26; device[217].type=0;
	device[218].regaddres=0x03EE; device[218].func=3; device[218].pub_delay=60; device[218].write_func=0x10; device[218].group=26; device[218].type=0;
	device[219].regaddres=0x03EF; device[219].func=3; device[219].pub_delay=60; device[219].write_func=0x10; device[219].group=26; device[219].type=0;
	device[220].regaddres=0x03F0; device[220].func=3; device[220].pub_delay=60; device[220].write_func=0x10; device[220].group=26; device[220].type=0;
	device[221].regaddres=0x03F1; device[221].func=3; device[221].pub_delay=60; device[221].write_func=0x10; device[221].group=26; device[221].type=0;
	device[222].regaddres=0x03F2; device[222].func=3; device[222].pub_delay=60; device[222].write_func=0x10; device[222].group=26; device[222].type=0;
	device[223].regaddres=0x03F3; device[223].func=3; device[223].pub_delay=60; device[223].write_func=0x10; device[223].group=26; device[223].type=0;
	device[224].regaddres=0x03F4; device[224].func=3; device[224].pub_delay=60; device[224].write_func=0x10; device[224].group=26; device[224].type=0;
	device[225].regaddres=0x03F5; device[225].func=3; device[225].pub_delay=60; device[225].write_func=0x10; device[225].group=26; device[225].type=0;
	device[226].regaddres=0x03F6; device[226].func=3; device[226].pub_delay=60; device[226].write_func=0x10; device[226].group=26; device[226].type=0;
	device[227].regaddres=0x03F7; device[227].func=3; device[227].pub_delay=60; device[227].write_func=0x10; device[227].group=26; device[227].type=0;
	device[228].regaddres=0x03F8; device[228].func=3; device[228].pub_delay=60; device[228].write_func=0x10; device[228].group=26; device[228].type=0;
	device[229].regaddres=0x03F9; device[229].func=3; device[229].pub_delay=60; device[229].write_func=0x10; device[229].group=26; device[229].type=0;
	device[230].regaddres=0x03FA; device[230].func=3; device[230].pub_delay=60; device[230].write_func=0x10; device[230].group=26; device[230].type=0;
	device[231].regaddres=0x03FB; device[231].func=3; device[231].pub_delay=60; device[231].write_func=0x10; device[231].group=26; device[231].type=0;
	device[232].regaddres=0x03FC; device[232].func=3; device[232].pub_delay=60; device[232].write_func=0x10; device[232].group=26; device[232].type=0;
	device[233].regaddres=0x03FD; device[233].func=3; device[233].pub_delay=60; device[233].write_func=0x10; device[233].group=26; device[233].type=0;
	device[234].regaddres=0x03FE; device[234].func=3; device[234].pub_delay=60; device[234].write_func=0x10; device[234].group=26; device[234].type=0;
	device[235].regaddres=0x03FF; device[235].func=3; device[235].pub_delay=60; device[235].write_func=0x10; device[235].group=26; device[235].type=0;
	device[236].regaddres=0x0400; device[236].func=3; device[236].pub_delay=60; device[236].write_func=0x10; device[236].group=26; device[236].type=0;
	device[237].regaddres=0x0401; device[237].func=3; device[237].pub_delay=60; device[237].write_func=0x10; device[237].group=26; device[237].type=0;
	device[238].regaddres=0x0402; device[238].func=3; device[238].pub_delay=60; device[238].write_func=0x10; device[238].group=26; device[238].type=0;
	device[239].regaddres=0x0403; device[239].func=3; device[239].pub_delay=60; device[239].write_func=0x10; device[239].group=26; device[239].type=0;
	device[240].regaddres=0x0404; device[240].func=3; device[240].pub_delay=60; device[240].write_func=0x10; device[240].group=26; device[240].type=0;
	device[241].regaddres=0x0405; device[241].func=3; device[241].pub_delay=60; device[241].write_func=0x10; device[241].group=26; device[241].type=0;
	device[242].regaddres=0x0406; device[242].func=3; device[242].pub_delay=60; device[242].write_func=0x10; device[242].group=26; device[242].type=0;
	device[243].regaddres=0x0407; device[243].func=3; device[243].pub_delay=60; device[243].write_func=0x10; device[243].group=26; device[243].type=0;
	device[244].regaddres=0x0408; device[244].func=3; device[244].pub_delay=60; device[244].write_func=0x10; device[244].group=26; device[244].type=0;
	device[245].regaddres=0x0409; device[245].func=3; device[245].pub_delay=60; device[245].write_func=0x10; device[245].group=26; device[245].type=0;
	device[246].regaddres=0x040A; device[246].func=3; device[246].pub_delay=60; device[246].write_func=0x10; device[246].group=26; device[246].type=0;
	device[247].regaddres=0x040B; device[247].func=3; device[247].pub_delay=60; device[247].write_func=0x10; device[247].group=26; device[247].type=0;
	device[248].regaddres=0x040C; device[248].func=3; device[248].pub_delay=60; device[248].write_func=0x10; device[248].group=26; device[248].type=0;
	device[249].regaddres=0x040D; device[249].func=3; device[249].pub_delay=60; device[249].write_func=0x10; device[249].group=26; device[249].type=0;
	device[250].regaddres=0x040E; device[250].func=3; device[250].pub_delay=60; device[250].write_func=0x10; device[250].group=26; device[250].type=0;
	device[251].regaddres=0x040F; device[251].func=3; device[251].pub_delay=60; device[251].write_func=0x10; device[251].group=26; device[251].type=0;
	device[252].regaddres=0x0413; device[252].func=3; device[252].pub_delay=60; device[252].write_func=0x10; device[252].group=27; device[252].type=0;
	device[253].regaddres=0x0414; device[253].func=3; device[253].pub_delay=60; device[253].write_func=0x10; device[253].group=27; device[253].type=0;
	device[254].regaddres=0x042B; device[254].func=3; device[254].pub_delay=60; device[254].write_func=0x10; device[254].group=28; device[254].type=0;
	device[255].regaddres=0x042C; device[255].func=3; device[255].pub_delay=60; device[255].write_func=0x10; device[255].group=28; device[255].type=0;
	device[256].regaddres=0x042D; device[256].func=3; device[256].pub_delay=60; device[256].write_func=0x10; device[256].group=28; device[256].type=0;
	device[257].regaddres=0x042E; device[257].func=3; device[257].pub_delay=60; device[257].write_func=0x10; device[257].group=28; device[257].type=0;
	device[258].regaddres=0x042F; device[258].func=3; device[258].pub_delay=60; device[258].write_func=0x10; device[258].group=28; device[258].type=0;
	device[259].regaddres=0x0432; device[259].func=3; device[259].pub_delay=60; device[259].write_func=0x10; device[259].group=29; device[259].type=0;
	device[260].regaddres=0x0433; device[260].func=3; device[260].pub_delay=60; device[260].write_func=0x10; device[260].group=29; device[260].type=0;
	device[261].regaddres=0x0434; device[261].func=3; device[261].pub_delay=60; device[261].write_func=0x10; device[261].group=29; device[261].type=0;
	device[262].regaddres=0x0435; device[262].func=3; device[262].pub_delay=60; device[262].write_func=0x10; device[262].group=29; device[262].type=0;
	device[263].regaddres=0x02FF; device[263].func=3; device[263].pub_delay=60; device[263].write_func=0x10; device[263].group=30; device[263].type=0;
	device[264].regaddres=0x0300; device[264].func=3; device[264].pub_delay=60; device[264].write_func=0x10; device[264].group=30; device[264].type=0;
	device[265].regaddres=0x0301; device[265].func=3; device[265].pub_delay=60; device[265].write_func=0x10; device[265].group=30; device[265].type=0;
	device[266].regaddres=0x0302; device[266].func=3; device[266].pub_delay=60; device[266].write_func=0x10; device[266].group=30; device[266].type=0;
	device[267].regaddres=0x043C; device[267].func=3; device[267].pub_delay=60; device[267].write_func=0x10; device[267].group=31; device[267].type=0;
	device[268].regaddres=0x043D; device[268].func=3; device[268].pub_delay=60; device[268].write_func=0x10; device[268].group=31; device[268].type=0;
	device[269].regaddres=0x043E; device[269].func=3; device[269].pub_delay=60; device[269].write_func=0x10; device[269].group=31; device[269].type=0;
	device[270].regaddres=0x043F; device[270].func=3; device[270].pub_delay=60; device[270].write_func=0x10; device[270].group=31; device[270].type=0;
	device[271].regaddres=0x0440; device[271].func=3; device[271].pub_delay=60; device[271].write_func=0x10; device[271].group=31; device[271].type=0;
	device[272].regaddres=0x0441; device[272].func=3; device[272].pub_delay=60; device[272].write_func=0x10; device[272].group=31; device[272].type=0;
	device[273].regaddres=0x0442; device[273].func=3; device[273].pub_delay=60; device[273].write_func=0x10; device[273].group=31; device[273].type=0;
	device[274].regaddres=0x0443; device[274].func=3; device[274].pub_delay=60; device[274].write_func=0x10; device[274].group=31; device[274].type=0;
	device[275].regaddres=0x0444; device[275].func=3; device[275].pub_delay=60; device[275].write_func=0x10; device[275].group=31; device[275].type=0;
	device[276].regaddres=0x0445; device[276].func=3; device[276].pub_delay=60; device[276].write_func=0x10; device[276].group=31; device[276].type=0;
	device[277].regaddres=0x0446; device[277].func=3; device[277].pub_delay=60; device[277].write_func=0x10; device[277].group=31; device[277].type=0;
	device[278].regaddres=0x0447; device[278].func=3; device[278].pub_delay=60; device[278].write_func=0x10; device[278].group=31; device[278].type=0;
	device[279].regaddres=0x0448; device[279].func=3; device[279].pub_delay=60; device[279].write_func=0x10; device[279].group=31; device[279].type=0;
	device[280].regaddres=0x0449; device[280].func=3; device[280].pub_delay=60; device[280].write_func=0x10; device[280].group=31; device[280].type=0;
	device[281].regaddres=0x044A; device[281].func=3; device[281].pub_delay=60; device[281].write_func=0x10; device[281].group=31; device[281].type=0;
	device[282].regaddres=0x044B; device[282].func=3; device[282].pub_delay=60; device[282].write_func=0x10; device[282].group=31; device[282].type=0;
	device[283].regaddres=0x044C; device[283].func=3; device[283].pub_delay=60; device[283].write_func=0x10; device[283].group=31; device[283].type=0;
	device[284].regaddres=0x044D; device[284].func=3; device[284].pub_delay=60; device[284].write_func=0x10; device[284].group=31; device[284].type=0;
	device[285].regaddres=0x044E; device[285].func=3; device[285].pub_delay=60; device[285].write_func=0x10; device[285].group=31; device[285].type=0;
	device[286].regaddres=0x044F; device[286].func=3; device[286].pub_delay=60; device[286].write_func=0x10; device[286].group=31; device[286].type=0;
	device[287].regaddres=0x0470; device[287].func=3; device[287].pub_delay=60; device[287].write_func=0x10; device[287].group=32; device[287].type=0;
	device[288].regaddres=0x0471; device[288].func=3; device[288].pub_delay=60; device[288].write_func=0x10; device[288].group=32; device[288].type=0;
	device[289].regaddres=0x0472; device[289].func=3; device[289].pub_delay=60; device[289].write_func=0x10; device[289].group=32; device[289].type=0;
	device[290].regaddres=0x0473; device[290].func=3; device[290].pub_delay=60; device[290].write_func=0x10; device[290].group=32; device[290].type=0;
	device[291].regaddres=0x0474; device[291].func=3; device[291].pub_delay=60; device[291].write_func=0x10; device[291].group=32; device[291].type=0;
	device[292].regaddres=0x0475; device[292].func=3; device[292].pub_delay=60; device[292].write_func=0x10; device[292].group=32; device[292].type=0;
	device[293].regaddres=0x0476; device[293].func=3; device[293].pub_delay=60; device[293].write_func=0x10; device[293].group=32; device[293].type=0;
	device[294].regaddres=0x0477; device[294].func=3; device[294].pub_delay=60; device[294].write_func=0x10; device[294].group=32; device[294].type=0;
	device[295].regaddres=0x0478; device[295].func=3; device[295].pub_delay=60; device[295].write_func=0x10; device[295].group=32; device[295].type=0;
	device[296].regaddres=0x0479; device[296].func=3; device[296].pub_delay=60; device[296].write_func=0x10; device[296].group=32; device[296].type=0;
	device[297].regaddres=0x047A; device[297].func=3; device[297].pub_delay=60; device[297].write_func=0x10; device[297].group=32; device[297].type=0;
	device[298].regaddres=0x047B; device[298].func=3; device[298].pub_delay=60; device[298].write_func=0x10; device[298].group=32; device[298].type=0;
	device[299].regaddres=0x047C; device[299].func=3; device[299].pub_delay=60; device[299].write_func=0x10; device[299].group=32; device[299].type=0;
	device[300].regaddres=0x047D; device[300].func=3; device[300].pub_delay=60; device[300].write_func=0x10; device[300].group=32; device[300].type=0;
	device[301].regaddres=0x047E; device[301].func=3; device[301].pub_delay=60; device[301].write_func=0x10; device[301].group=32; device[301].type=0;
	device[302].regaddres=0x047F; device[302].func=3; device[302].pub_delay=60; device[302].write_func=0x10; device[302].group=32; device[302].type=0;
	device[303].regaddres=0x0480; device[303].func=3; device[303].pub_delay=60; device[303].write_func=0x10; device[303].group=32; device[303].type=0;
	device[304].regaddres=0x0481; device[304].func=3; device[304].pub_delay=60; device[304].write_func=0x10; device[304].group=32; device[304].type=0;
	device[305].regaddres=0x0482; device[305].func=3; device[305].pub_delay=60; device[305].write_func=0x10; device[305].group=32; device[305].type=0;
	device[306].regaddres=0x0483; device[306].func=3; device[306].pub_delay=60; device[306].write_func=0x10; device[306].group=32; device[306].type=0;
	device[307].regaddres=0x0484; device[307].func=3; device[307].pub_delay=60; device[307].write_func=0x10; device[307].group=32; device[307].type=0;
	device[308].regaddres=0x0485; device[308].func=3; device[308].pub_delay=60; device[308].write_func=0x10; device[308].group=32; device[308].type=0;
	device[309].regaddres=0x0486; device[309].func=3; device[309].pub_delay=60; device[309].write_func=0x10; device[309].group=32; device[309].type=0;
	device[310].regaddres=0x0487; device[310].func=3; device[310].pub_delay=60; device[310].write_func=0x10; device[310].group=32; device[310].type=0;
	device[311].regaddres=0x0488; device[311].func=3; device[311].pub_delay=60; device[311].write_func=0x10; device[311].group=32; device[311].type=0;
	device[312].regaddres=0x0489; device[312].func=3; device[312].pub_delay=60; device[312].write_func=0x10; device[312].group=32; device[312].type=0;
	device[313].regaddres=0x048A; device[313].func=3; device[313].pub_delay=60; device[313].write_func=0x10; device[313].group=32; device[313].type=0;
	device[314].regaddres=0x048B; device[314].func=3; device[314].pub_delay=60; device[314].write_func=0x10; device[314].group=32; device[314].type=0;
	device[315].regaddres=0x048C; device[315].func=3; device[315].pub_delay=60; device[315].write_func=0x10; device[315].group=32; device[315].type=0;
	device[316].regaddres=0x048D; device[316].func=3; device[316].pub_delay=60; device[316].write_func=0x10; device[316].group=32; device[316].type=0;
	device[317].regaddres=0x048E; device[317].func=3; device[317].pub_delay=60; device[317].write_func=0x10; device[317].group=32; device[317].type=0;
	device[318].regaddres=0x048F; device[318].func=3; device[318].pub_delay=60; device[318].write_func=0x10; device[318].group=32; device[318].type=0;
	device[319].regaddres=0x0490; device[319].func=3; device[319].pub_delay=60; device[319].write_func=0x10; device[319].group=32; device[319].type=0;
	device[320].regaddres=0x0491; device[320].func=3; device[320].pub_delay=60; device[320].write_func=0x10; device[320].group=32; device[320].type=0;
	device[321].regaddres=0x0492; device[321].func=3; device[321].pub_delay=60; device[321].write_func=0x10; device[321].group=32; device[321].type=0;
	device[322].regaddres=0x0493; device[322].func=3; device[322].pub_delay=60; device[322].write_func=0x10; device[322].group=32; device[322].type=0;
	device[323].regaddres=0x0494; device[323].func=3; device[323].pub_delay=60; device[323].write_func=0x10; device[323].group=32; device[323].type=0;
	device[324].regaddres=0x0495; device[324].func=3; device[324].pub_delay=60; device[324].write_func=0x10; device[324].group=32; device[324].type=0;
	device[325].regaddres=0x0496; device[325].func=3; device[325].pub_delay=60; device[325].write_func=0x10; device[325].group=32; device[325].type=0;
	device[326].regaddres=0x0497; device[326].func=3; device[326].pub_delay=60; device[326].write_func=0x10; device[326].group=32; device[326].type=0;
	device[327].regaddres=0x0498; device[327].func=3; device[327].pub_delay=60; device[327].write_func=0x10; device[327].group=32; device[327].type=0;
	device[328].regaddres=0x0499; device[328].func=3; device[328].pub_delay=60; device[328].write_func=0x10; device[328].group=32; device[328].type=0;
	device[329].regaddres=0x049A; device[329].func=3; device[329].pub_delay=60; device[329].write_func=0x10; device[329].group=32; device[329].type=0;
	device[330].regaddres=0x049B; device[330].func=3; device[330].pub_delay=60; device[330].write_func=0x10; device[330].group=32; device[330].type=0;
	device[331].regaddres=0x049C; device[331].func=3; device[331].pub_delay=60; device[331].write_func=0x10; device[331].group=32; device[331].type=0;
	device[332].regaddres=0x049D; device[332].func=3; device[332].pub_delay=60; device[332].write_func=0x10; device[332].group=32; device[332].type=0;
	device[333].regaddres=0x049E; device[333].func=3; device[333].pub_delay=60; device[333].write_func=0x10; device[333].group=32; device[333].type=0;
	device[334].regaddres=0x049F; device[334].func=3; device[334].pub_delay=60; device[334].write_func=0x10; device[334].group=32; device[334].type=0;
	device[335].regaddres=0x04A0; device[335].func=3; device[335].pub_delay=60; device[335].write_func=0x10; device[335].group=32; device[335].type=0;
	device[336].regaddres=0x04A1; device[336].func=3; device[336].pub_delay=60; device[336].write_func=0x10; device[336].group=32; device[336].type=0;
	device[337].regaddres=0x04A2; device[337].func=3; device[337].pub_delay=60; device[337].write_func=0x10; device[337].group=32; device[337].type=0;
	device[338].regaddres=0x04A3; device[338].func=3; device[338].pub_delay=60; device[338].write_func=0x10; device[338].group=32; device[338].type=0;
	device[339].regaddres=0x04A4; device[339].func=3; device[339].pub_delay=60; device[339].write_func=0x10; device[339].group=32; device[339].type=0;
	device[340].regaddres=0x04A5; device[340].func=3; device[340].pub_delay=60; device[340].write_func=0x10; device[340].group=32; device[340].type=0;
	device[341].regaddres=0x04A6; device[341].func=3; device[341].pub_delay=60; device[341].write_func=0x10; device[341].group=32; device[341].type=0;
	device[342].regaddres=0x04A7; device[342].func=3; device[342].pub_delay=60; device[342].write_func=0x10; device[342].group=32; device[342].type=0;
	device[343].regaddres=0x04A8; device[343].func=3; device[343].pub_delay=60; device[343].write_func=0x10; device[343].group=32; device[343].type=0;
	device[344].regaddres=0x04A9; device[344].func=3; device[344].pub_delay=60; device[344].write_func=0x10; device[344].group=32; device[344].type=0;
	device[345].regaddres=0x04AA; device[345].func=3; device[345].pub_delay=60; device[345].write_func=0x10; device[345].group=32; device[345].type=0;
	device[346].regaddres=0x04AB; device[346].func=3; device[346].pub_delay=60; device[346].write_func=0x10; device[346].group=32; device[346].type=0;
	device[347].regaddres=0x04AC; device[347].func=3; device[347].pub_delay=60; device[347].write_func=0x10; device[347].group=32; device[347].type=0;
	device[348].regaddres=0x04AD; device[348].func=3; device[348].pub_delay=60; device[348].write_func=0x10; device[348].group=32; device[348].type=0;
	device[349].regaddres=0x04AE; device[349].func=3; device[349].pub_delay=60; device[349].write_func=0x10; device[349].group=32; device[349].type=0;
	device[350].regaddres=0x04AF; device[350].func=3; device[350].pub_delay=60; device[350].write_func=0x10; device[350].group=32; device[350].type=0;
	device[351].regaddres=0x04B0; device[351].func=3; device[351].pub_delay=60; device[351].write_func=0x10; device[351].group=32; device[351].type=0;
	device[352].regaddres=0x04B1; device[352].func=3; device[352].pub_delay=60; device[352].write_func=0x10; device[352].group=32; device[352].type=0;
	device[353].regaddres=0x04B2; device[353].func=3; device[353].pub_delay=60; device[353].write_func=0x10; device[353].group=32; device[353].type=0;
	device[354].regaddres=0x04B3; device[354].func=3; device[354].pub_delay=60; device[354].write_func=0x10; device[354].group=32; device[354].type=0;
	device[355].regaddres=0x04B4; device[355].func=3; device[355].pub_delay=60; device[355].write_func=0x10; device[355].group=32; device[355].type=0;
	device[356].regaddres=0x04B5; device[356].func=3; device[356].pub_delay=60; device[356].write_func=0x10; device[356].group=32; device[356].type=0;
	device[357].regaddres=0x04B6; device[357].func=3; device[357].pub_delay=60; device[357].write_func=0x10; device[357].group=32; device[357].type=0;
	device[358].regaddres=0x04B7; device[358].func=3; device[358].pub_delay=60; device[358].write_func=0x10; device[358].group=32; device[358].type=0;
	device[359].regaddres=0x04B8; device[359].func=3; device[359].pub_delay=60; device[359].write_func=0x10; device[359].group=32; device[359].type=0;
	device[360].regaddres=0x04B9; device[360].func=3; device[360].pub_delay=60; device[360].write_func=0x10; device[360].group=32; device[360].type=0;
	device[361].regaddres=0x04BA; device[361].func=3; device[361].pub_delay=60; device[361].write_func=0x10; device[361].group=32; device[361].type=0;
	device[362].regaddres=0x04BB; device[362].func=3; device[362].pub_delay=60; device[362].write_func=0x10; device[362].group=32; device[362].type=0;
	device[363].regaddres=0x04BC; device[363].func=3; device[363].pub_delay=60; device[363].write_func=0x10; device[363].group=32; device[363].type=0;
	device[364].regaddres=0x04BD; device[364].func=3; device[364].pub_delay=60; device[364].write_func=0x10; device[364].group=32; device[364].type=0;
	device[365].regaddres=0x04BE; device[365].func=3; device[365].pub_delay=60; device[365].write_func=0x10; device[365].group=32; device[365].type=0;
	device[366].regaddres=0x04BF; device[366].func=3; device[366].pub_delay=60; device[366].write_func=0x10; device[366].group=32; device[366].type=0;
	device[367].regaddres=0x04C0; device[367].func=3; device[367].pub_delay=60; device[367].write_func=0x10; device[367].group=32; device[367].type=0;
	device[368].regaddres=0x04C1; device[368].func=3; device[368].pub_delay=60; device[368].write_func=0x10; device[368].group=32; device[368].type=0;
	device[369].regaddres=0x04C2; device[369].func=3; device[369].pub_delay=60; device[369].write_func=0x10; device[369].group=32; device[369].type=0;
	device[370].regaddres=0x04C3; device[370].func=3; device[370].pub_delay=60; device[370].write_func=0x10; device[370].group=32; device[370].type=0;
	device[371].regaddres=0x04C4; device[371].func=3; device[371].pub_delay=60; device[371].write_func=0x10; device[371].group=32; device[371].type=0;
	device[372].regaddres=0x04C5; device[372].func=3; device[372].pub_delay=60; device[372].write_func=0x10; device[372].group=32; device[372].type=0;
	device[373].regaddres=0x04C6; device[373].func=3; device[373].pub_delay=60; device[373].write_func=0x10; device[373].group=32; device[373].type=0;
	device[374].regaddres=0x04C7; device[374].func=3; device[374].pub_delay=60; device[374].write_func=0x10; device[374].group=32; device[374].type=0;
	device[375].regaddres=0x04C8; device[375].func=3; device[375].pub_delay=60; device[375].write_func=0x10; device[375].group=32; device[375].type=0;
	device[376].regaddres=0x04C9; device[376].func=3; device[376].pub_delay=60; device[376].write_func=0x10; device[376].group=32; device[376].type=0;
	device[377].regaddres=0x04CA; device[377].func=3; device[377].pub_delay=60; device[377].write_func=0x10; device[377].group=32; device[377].type=0;
	device[378].regaddres=0x04CB; device[378].func=3; device[378].pub_delay=60; device[378].write_func=0x10; device[378].group=32; device[378].type=0;
	device[379].regaddres=0x04CC; device[379].func=3; device[379].pub_delay=60; device[379].write_func=0x10; device[379].group=32; device[379].type=0;
	device[380].regaddres=0x04CD; device[380].func=3; device[380].pub_delay=60; device[380].write_func=0x10; device[380].group=32; device[380].type=0;
	device[381].regaddres=0x04CE; device[381].func=3; device[381].pub_delay=60; device[381].write_func=0x10; device[381].group=32; device[381].type=0;
	device[382].regaddres=0x04CF; device[382].func=3; device[382].pub_delay=60; device[382].write_func=0x10; device[382].group=32; device[382].type=0;
	device[383].regaddres=0x04D0; device[383].func=3; device[383].pub_delay=60; device[383].write_func=0x10; device[383].group=32; device[383].type=0;
	device[384].regaddres=0x04D1; device[384].func=3; device[384].pub_delay=60; device[384].write_func=0x10; device[384].group=32; device[384].type=0;
	device[385].regaddres=0x04D2; device[385].func=3; device[385].pub_delay=60; device[385].write_func=0x10; device[385].group=32; device[385].type=0;
	device[386].regaddres=0x04D3; device[386].func=3; device[386].pub_delay=60; device[386].write_func=0x10; device[386].group=32; device[386].type=0;
	device[387].regaddres=0x0614; device[387].func=3; device[387].pub_delay=60; device[387].write_func=0x10; device[387].group=33; device[387].type=0;
	device[388].regaddres=0x0615; device[388].func=3; device[388].pub_delay=60; device[388].write_func=0x10; device[388].group=33; device[388].type=0;
	device[389].regaddres=0x061C; device[389].func=3; device[389].pub_delay=60; device[389].write_func=0x10; device[389].group=34; device[389].type=0;
	device[390].regaddres=0x061F; device[390].func=3; device[390].pub_delay=60; device[390].write_func=0x10; device[390].group=35; device[390].type=0;
	device[391].regaddres=0x0620; device[391].func=3; device[391].pub_delay=60; device[391].write_func=0x10; device[391].group=35; device[391].type=0;
	device[392].regaddres=0x0621; device[392].func=3; device[392].pub_delay=60; device[392].write_func=0x10; device[392].group=35; device[392].type=0;
	device[393].regaddres=0x0622; device[393].func=3; device[393].pub_delay=60; device[393].write_func=0x10; device[393].group=35; device[393].type=0;
	device[394].regaddres=0x0633; device[394].func=3; device[394].pub_delay=60; device[394].write_func=0x10; device[394].group=36; device[394].type=0;
	device[395].regaddres=0x0636; device[395].func=3; device[395].pub_delay=60; device[395].write_func=0x10; device[395].group=37; device[395].type=0;
	device[396].regaddres=0x0637; device[396].func=3; device[396].pub_delay=60; device[396].write_func=0x10; device[396].group=37; device[396].type=0;
	device[397].regaddres=0x0638; device[397].func=3; device[397].pub_delay=60; device[397].write_func=0x10; device[397].group=37; device[397].type=0;
	device[398].regaddres=0x0639; device[398].func=3; device[398].pub_delay=60; device[398].write_func=0x10; device[398].group=37; device[398].type=0;
	device[399].regaddres=0x063A; device[399].func=3; device[399].pub_delay=60; device[399].write_func=0x10; device[399].group=37; device[399].type=0;
	device[400].regaddres=0x063F; device[400].func=3; device[400].pub_delay=60; device[400].write_func=0x10; device[400].group=38; device[400].type=0;
	device[401].regaddres=0x0640; device[401].func=3; device[401].pub_delay=60; device[401].write_func=0x10; device[401].group=38; device[401].type=0;
	device[402].regaddres=0x0641; device[402].func=3; device[402].pub_delay=60; device[402].write_func=0x10; device[402].group=38; device[402].type=0;
	device[403].regaddres=0x0642; device[403].func=3; device[403].pub_delay=60; device[403].write_func=0x10; device[403].group=38; device[403].type=0;
	device[404].regaddres=0x0645; device[404].func=3; device[404].pub_delay=60; device[404].write_func=0x10; device[404].group=39; device[404].type=0;
	device[405].regaddres=0x0646; device[405].func=3; device[405].pub_delay=60; device[405].write_func=0x10; device[405].group=39; device[405].type=0;
	device[406].regaddres=0x0647; device[406].func=3; device[406].pub_delay=60; device[406].write_func=0x10; device[406].group=39; device[406].type=0;
	device[407].regaddres=0x0648; device[407].func=3; device[407].pub_delay=60; device[407].write_func=0x10; device[407].group=39; device[407].type=0;
	device[408].regaddres=0x0649; device[408].func=3; device[408].pub_delay=60; device[408].write_func=0x10; device[408].group=39; device[408].type=0;
	device[409].regaddres=0x064A; device[409].func=3; device[409].pub_delay=60; device[409].write_func=0x10; device[409].group=39; device[409].type=0;
	device[410].regaddres=0x064B; device[410].func=3; device[410].pub_delay=60; device[410].write_func=0x10; device[410].group=39; device[410].type=0;
	device[411].regaddres=0x064C; device[411].func=3; device[411].pub_delay=60; device[411].write_func=0x10; device[411].group=39; device[411].type=0;
	device[412].regaddres=0x064D; device[412].func=3; device[412].pub_delay=60; device[412].write_func=0x10; device[412].group=39; device[412].type=0;
	device[413].regaddres=0x064E; device[413].func=3; device[413].pub_delay=60; device[413].write_func=0x10; device[413].group=39; device[413].type=0;
	device[414].regaddres=0x064F; device[414].func=3; device[414].pub_delay=60; device[414].write_func=0x10; device[414].group=39; device[414].type=0;
	device[415].regaddres=0x0678; device[415].func=3; device[415].pub_delay=60; device[415].write_func=0x10; device[415].group=40; device[415].type=0;
	device[416].regaddres=0x0309; device[416].func=3; device[416].pub_delay=60; device[416].write_func=0x10; device[416].group=41; device[416].type=0;
	device[417].regaddres=0x030A; device[417].func=3; device[417].pub_delay=60; device[417].write_func=0x10; device[417].group=41; device[417].type=0;
	device[418].regaddres=0x030B; device[418].func=3; device[418].pub_delay=60; device[418].write_func=0x10; device[418].group=41; device[418].type=0;
	device[419].regaddres=0x030C; device[419].func=3; device[419].pub_delay=60; device[419].write_func=0x10; device[419].group=41; device[419].type=0;
	device[420].regaddres=0x030D; device[420].func=3; device[420].pub_delay=60; device[420].write_func=0x10; device[420].group=41; device[420].type=0;
	device[421].regaddres=0x030E; device[421].func=3; device[421].pub_delay=60; device[421].write_func=0x10; device[421].group=41; device[421].type=0;
	device[422].regaddres=0x030F; device[422].func=3; device[422].pub_delay=60; device[422].write_func=0x10; device[422].group=41; device[422].type=0;
	device[423].regaddres=0x0310; device[423].func=3; device[423].pub_delay=60; device[423].write_func=0x10; device[423].group=41; device[423].type=0;
	device[424].regaddres=0x0311; device[424].func=3; device[424].pub_delay=60; device[424].write_func=0x10; device[424].group=41; device[424].type=0;
	device[425].regaddres=0x0312; device[425].func=3; device[425].pub_delay=60; device[425].write_func=0x10; device[425].group=41; device[425].type=0;
	device[426].regaddres=0x0313; device[426].func=3; device[426].pub_delay=60; device[426].write_func=0x10; device[426].group=41; device[426].type=0;
	device[427].regaddres=0x0314; device[427].func=3; device[427].pub_delay=60; device[427].write_func=0x10; device[427].group=41; device[427].type=0;
	device[428].regaddres=0x0315; device[428].func=3; device[428].pub_delay=60; device[428].write_func=0x10; device[428].group=41; device[428].type=0;
	device[429].regaddres=0x0316; device[429].func=3; device[429].pub_delay=60; device[429].write_func=0x10; device[429].group=41; device[429].type=0;
	device[430].regaddres=0x0317; device[430].func=3; device[430].pub_delay=60; device[430].write_func=0x10; device[430].group=41; device[430].type=0;
	device[431].regaddres=0x0318; device[431].func=3; device[431].pub_delay=60; device[431].write_func=0x10; device[431].group=41; device[431].type=0;
	device[432].regaddres=0x0319; device[432].func=3; device[432].pub_delay=60; device[432].write_func=0x10; device[432].group=41; device[432].type=0;
	device[433].regaddres=0x031A; device[433].func=3; device[433].pub_delay=60; device[433].write_func=0x10; device[433].group=41; device[433].type=0;
	device[434].regaddres=0x0702; device[434].func=3; device[434].pub_delay=60; device[434].write_func=0x10; device[434].group=42; device[434].type=0;
	device[435].regaddres=0x0703; device[435].func=3; device[435].pub_delay=60; device[435].write_func=0x10; device[435].group=42; device[435].type=0;
	device[436].regaddres=0x0704; device[436].func=3; device[436].pub_delay=60; device[436].write_func=0x10; device[436].group=42; device[436].type=0;
	device[437].regaddres=0x0705; device[437].func=3; device[437].pub_delay=60; device[437].write_func=0x10; device[437].group=42; device[437].type=0;
	device[438].regaddres=0x0706; device[438].func=3; device[438].pub_delay=60; device[438].write_func=0x10; device[438].group=42; device[438].type=0;
	device[439].regaddres=0x0707; device[439].func=3; device[439].pub_delay=60; device[439].write_func=0x10; device[439].group=42; device[439].type=0;
	device[440].regaddres=0x0708; device[440].func=3; device[440].pub_delay=60; device[440].write_func=0x10; device[440].group=42; device[440].type=0;
	device[441].regaddres=0x0709; device[441].func=3; device[441].pub_delay=60; device[441].write_func=0x10; device[441].group=42; device[441].type=0;
	device[442].regaddres=0x1000; device[442].func=3; device[442].pub_delay=60; device[442].write_func=0x10; device[442].group=43; device[442].type=0;
	device[443].regaddres=0x031C; device[443].func=3; device[443].pub_delay=60; device[443].write_func=0x10; device[443].group=44; device[443].type=0;
	device[444].regaddres=0x031D; device[444].func=3; device[444].pub_delay=60; device[444].write_func=0x10; device[444].group=44; device[444].type=0;
	device[445].regaddres=0x031E; device[445].func=3; device[445].pub_delay=60; device[445].write_func=0x10; device[445].group=44; device[445].type=0;
	device[446].regaddres=0x031F; device[446].func=3; device[446].pub_delay=60; device[446].write_func=0x10; device[446].group=44; device[446].type=0;
	device[447].regaddres=0x0320; device[447].func=3; device[447].pub_delay=60; device[447].write_func=0x10; device[447].group=44; device[447].type=0;
	device[448].regaddres=0x0321; device[448].func=3; device[448].pub_delay=60; device[448].write_func=0x10; device[448].group=44; device[448].type=0;
	device[449].regaddres=0x0322; device[449].func=3; device[449].pub_delay=60; device[449].write_func=0x10; device[449].group=44; device[449].type=0;
	device[450].regaddres=0x0323; device[450].func=3; device[450].pub_delay=60; device[450].write_func=0x10; device[450].group=44; device[450].type=0;
	device[451].regaddres=0x0324; device[451].func=3; device[451].pub_delay=60; device[451].write_func=0x10; device[451].group=44; device[451].type=0;
	device[452].regaddres=0x0325; device[452].func=3; device[452].pub_delay=60; device[452].write_func=0x10; device[452].group=44; device[452].type=0;
	device[453].regaddres=0x0326; device[453].func=3; device[453].pub_delay=60; device[453].write_func=0x10; device[453].group=44; device[453].type=0;
	device[454].regaddres=0x0327; device[454].func=3; device[454].pub_delay=60; device[454].write_func=0x10; device[454].group=44; device[454].type=0;
	device[455].regaddres=0x0328; device[455].func=3; device[455].pub_delay=60; device[455].write_func=0x10; device[455].group=44; device[455].type=0;
	device[456].regaddres=0x0329; device[456].func=3; device[456].pub_delay=60; device[456].write_func=0x10; device[456].group=44; device[456].type=0;
	device[457].regaddres=0x032A; device[457].func=3; device[457].pub_delay=60; device[457].write_func=0x10; device[457].group=44; device[457].type=0;
	device[458].regaddres=0x032B; device[458].func=3; device[458].pub_delay=60; device[458].write_func=0x10; device[458].group=44; device[458].type=0;
	device[459].regaddres=0x032C; device[459].func=3; device[459].pub_delay=60; device[459].write_func=0x10; device[459].group=44; device[459].type=0;
	device[460].regaddres=0x032D; device[460].func=3; device[460].pub_delay=60; device[460].write_func=0x10; device[460].group=44; device[460].type=0;
	device[461].regaddres=0x032E; device[461].func=3; device[461].pub_delay=60; device[461].write_func=0x10; device[461].group=44; device[461].type=0;
	device[462].regaddres=0x032F; device[462].func=3; device[462].pub_delay=60; device[462].write_func=0x10; device[462].group=44; device[462].type=0;
	device[463].regaddres=0x0330; device[463].func=3; device[463].pub_delay=60; device[463].write_func=0x10; device[463].group=44; device[463].type=0;
	device[464].regaddres=0x0337; device[464].func=3; device[464].pub_delay=60; device[464].write_func=0x10; device[464].group=45; device[464].type=0;
	device[465].regaddres=0x0338; device[465].func=3; device[465].pub_delay=60; device[465].write_func=0x10; device[465].group=45; device[465].type=0;
	device[466].regaddres=0x0339; device[466].func=3; device[466].pub_delay=60; device[466].write_func=0x10; device[466].group=45; device[466].type=0;
	device[467].regaddres=0x033A; device[467].func=3; device[467].pub_delay=60; device[467].write_func=0x10; device[467].group=45; device[467].type=0;
	device[468].regaddres=0x033B; device[468].func=3; device[468].pub_delay=60; device[468].write_func=0x10; device[468].group=45; device[468].type=0;
	device[469].regaddres=0x033C; device[469].func=3; device[469].pub_delay=60; device[469].write_func=0x10; device[469].group=45; device[469].type=0;
	device[470].regaddres=0x033D; device[470].func=3; device[470].pub_delay=60; device[470].write_func=0x10; device[470].group=45; device[470].type=0;
	device[471].regaddres=0x033E; device[471].func=3; device[471].pub_delay=60; device[471].write_func=0x10; device[471].group=45; device[471].type=0;
	device[472].regaddres=0x0341; device[472].func=3; device[472].pub_delay=60; device[472].write_func=0x10; device[472].group=46; device[472].type=0;
	device[473].regaddres=0x0342; device[473].func=3; device[473].pub_delay=60; device[473].write_func=0x10; device[473].group=46; device[473].type=0;
	device[474].regaddres=0x0343; device[474].func=3; device[474].pub_delay=60; device[474].write_func=0x10; device[474].group=46; device[474].type=0;
	device[475].regaddres=0x0344; device[475].func=3; device[475].pub_delay=60; device[475].write_func=0x10; device[475].group=46; device[475].type=0;
	device[476].regaddres=0x0345; device[476].func=3; device[476].pub_delay=60; device[476].write_func=0x10; device[476].group=46; device[476].type=0;
	device[477].regaddres=0x0346; device[477].func=3; device[477].pub_delay=60; device[477].write_func=0x10; device[477].group=46; device[477].type=0;
	device[478].regaddres=0x0347; device[478].func=3; device[478].pub_delay=60; device[478].write_func=0x10; device[478].group=46; device[478].type=0;
	device[479].regaddres=0x0348; device[479].func=3; device[479].pub_delay=60; device[479].write_func=0x10; device[479].group=46; device[479].type=0;
	device[480].regaddres=0x0349; device[480].func=3; device[480].pub_delay=60; device[480].write_func=0x10; device[480].group=46; device[480].type=0;
	device[481].regaddres=0x034A; device[481].func=3; device[481].pub_delay=60; device[481].write_func=0x10; device[481].group=46; device[481].type=0;
	device[482].regaddres=0x034B; device[482].func=3; device[482].pub_delay=60; device[482].write_func=0x10; device[482].group=46; device[482].type=0;
	device[483].regaddres=0x034C; device[483].func=3; device[483].pub_delay=60; device[483].write_func=0x10; device[483].group=46; device[483].type=0;
	device[484].regaddres=0x034D; device[484].func=3; device[484].pub_delay=60; device[484].write_func=0x10; device[484].group=46; device[484].type=0;
	device[485].regaddres=0x034E; device[485].func=3; device[485].pub_delay=60; device[485].write_func=0x10; device[485].group=46; device[485].type=0;
	device[486].regaddres=0x034F; device[486].func=3; device[486].pub_delay=60; device[486].write_func=0x10; device[486].group=46; device[486].type=0;
	device[487].regaddres=0x0350; device[487].func=3; device[487].pub_delay=60; device[487].write_func=0x10; device[487].group=46; device[487].type=0;
	device[488].regaddres=0x0351; device[488].func=3; device[488].pub_delay=60; device[488].write_func=0x10; device[488].group=46; device[488].type=0;
	device[489].regaddres=0x0352; device[489].func=3; device[489].pub_delay=60; device[489].write_func=0x10; device[489].group=46; device[489].type=0;
	device[490].regaddres=0x0353; device[490].func=3; device[490].pub_delay=60; device[490].write_func=0x10; device[490].group=46; device[490].type=0;
	device[491].regaddres=0x0354; device[491].func=3; device[491].pub_delay=60; device[491].write_func=0x10; device[491].group=46; device[491].type=0;
	device[492].regaddres=0x0355; device[492].func=3; device[492].pub_delay=60; device[492].write_func=0x10; device[492].group=46; device[492].type=0;
	device[493].regaddres=0x0356; device[493].func=3; device[493].pub_delay=60; device[493].write_func=0x10; device[493].group=46; device[493].type=0;
	device[494].regaddres=0x0357; device[494].func=3; device[494].pub_delay=60; device[494].write_func=0x10; device[494].group=46; device[494].type=0;
	device[495].regaddres=0x0358; device[495].func=3; device[495].pub_delay=60; device[495].write_func=0x10; device[495].group=46; device[495].type=0;
	device[496].regaddres=0x0359; device[496].func=3; device[496].pub_delay=60; device[496].write_func=0x10; device[496].group=46; device[496].type=0;
	device[497].regaddres=0x035A; device[497].func=3; device[497].pub_delay=60; device[497].write_func=0x10; device[497].group=46; device[497].type=0;
	device[498].regaddres=0x035B; device[498].func=3; device[498].pub_delay=60; device[498].write_func=0x10; device[498].group=46; device[498].type=0;
	device[499].regaddres=0x035C; device[499].func=3; device[499].pub_delay=60; device[499].write_func=0x10; device[499].group=46; device[499].type=0;
	device[500].regaddres=0x035D; device[500].func=3; device[500].pub_delay=60; device[500].write_func=0x10; device[500].group=46; device[500].type=0;
	device[501].regaddres=0x035E; device[501].func=3; device[501].pub_delay=60; device[501].write_func=0x10; device[501].group=46; device[501].type=0;
	device[502].regaddres=0x035F; device[502].func=3; device[502].pub_delay=60; device[502].write_func=0x10; device[502].group=46; device[502].type=0;
	device[503].regaddres=0x0360; device[503].func=3; device[503].pub_delay=60; device[503].write_func=0x10; device[503].group=46; device[503].type=0;
	device[504].regaddres=0x0361; device[504].func=3; device[504].pub_delay=60; device[504].write_func=0x10; device[504].group=46; device[504].type=0;
	device[505].regaddres=0x0362; device[505].func=3; device[505].pub_delay=60; device[505].write_func=0x10; device[505].group=46; device[505].type=0;
	device[506].regaddres=0x0363; device[506].func=3; device[506].pub_delay=60; device[506].write_func=0x10; device[506].group=46; device[506].type=0;
	device[507].regaddres=0x0364; device[507].func=3; device[507].pub_delay=60; device[507].write_func=0x10; device[507].group=46; device[507].type=0;
	device[508].regaddres=0x0366; device[508].func=3; device[508].pub_delay=60; device[508].write_func=0x10; device[508].group=47; device[508].type=0;
	device[509].regaddres=0x0367; device[509].func=3; device[509].pub_delay=60; device[509].write_func=0x10; device[509].group=47; device[509].type=0;
	device[510].regaddres=0x0368; device[510].func=3; device[510].pub_delay=60; device[510].write_func=0x10; device[510].group=47; device[510].type=0;
	device[511].regaddres=0x036B; device[511].func=3; device[511].pub_delay=60; device[511].write_func=0x10; device[511].group=48; device[511].type=0;
	device[512].regaddres=0x036C; device[512].func=3; device[512].pub_delay=60; device[512].write_func=0x10; device[512].group=48; device[512].type=0;
	device[513].regaddres=0x036D; device[513].func=3; device[513].pub_delay=60; device[513].write_func=0x10; device[513].group=48; device[513].type=0;
	device[514].regaddres=0x036E; device[514].func=3; device[514].pub_delay=60; device[514].write_func=0x10; device[514].group=48; device[514].type=0;
	device[515].regaddres=0x036F; device[515].func=3; device[515].pub_delay=60; device[515].write_func=0x10; device[515].group=48; device[515].type=0;
	device[516].regaddres=0x0370; device[516].func=3; device[516].pub_delay=60; device[516].write_func=0x10; device[516].group=48; device[516].type=0;
	device[517].regaddres=0x0373; device[517].func=3; device[517].pub_delay=60; device[517].write_func=0x10; device[517].group=49; device[517].type=0;
	device[518].regaddres=0x0374; device[518].func=3; device[518].pub_delay=60; device[518].write_func=0x10; device[518].group=49; device[518].type=0;
	device[519].regaddres=0x0375; device[519].func=3; device[519].pub_delay=60; device[519].write_func=0x10; device[519].group=49; device[519].type=0;
	device[520].regaddres=0x0376; device[520].func=3; device[520].pub_delay=60; device[520].write_func=0x10; device[520].group=49; device[520].type=0;
	device[521].regaddres=0x0377; device[521].func=3; device[521].pub_delay=60; device[521].write_func=0x10; device[521].group=49; device[521].type=0;
	device[522].regaddres=0x0378; device[522].func=3; device[522].pub_delay=60; device[522].write_func=0x10; device[522].group=49; device[522].type=0;
	device[523].regaddres=0x0067; device[523].func=4; device[523].pub_delay=60; device[523].write_func=0x10; device[523].group=50; device[523].type=4;
	device[524].regaddres=0x0069; device[524].func=4; device[524].pub_delay=60; device[524].write_func=0x10; device[524].group=50; device[524].type=4;
	device[525].regaddres=0x006B; device[525].func=4; device[525].pub_delay=60; device[525].write_func=0x10; device[525].group=50; device[525].type=4;
	device[526].regaddres=0x006D; device[526].func=4; device[526].pub_delay=60; device[526].write_func=0x10; device[526].group=50; device[526].type=4;
	device[527].regaddres=0x006F; device[527].func=4; device[527].pub_delay=60; device[527].write_func=0x10; device[527].group=50; device[527].type=4;
	device[528].regaddres=0x0071; device[528].func=4; device[528].pub_delay=60; device[528].write_func=0x10; device[528].group=50; device[528].type=4;
	device[529].regaddres=0x0073; device[529].func=4; device[529].pub_delay=60; device[529].write_func=0x10; device[529].group=50; device[529].type=4;
	device[530].regaddres=0x0075; device[530].func=4; device[530].pub_delay=60; device[530].write_func=0x10; device[530].group=50; device[530].type=4;
	device[531].regaddres=0x0077; device[531].func=4; device[531].pub_delay=60; device[531].write_func=0x10; device[531].group=50; device[531].type=4;
	device[532].regaddres=0x0079; device[532].func=4; device[532].pub_delay=60; device[532].write_func=0x10; device[532].group=50; device[532].type=4;
	device[533].regaddres=0x007B; device[533].func=4; device[533].pub_delay=60; device[533].write_func=0x10; device[533].group=50; device[533].type=4;
	device[534].regaddres=0x007D; device[534].func=4; device[534].pub_delay=60; device[534].write_func=0x10; device[534].group=50; device[534].type=4;
	device[535].regaddres=0x007F; device[535].func=4; device[535].pub_delay=60; device[535].write_func=0x10; device[535].group=50; device[535].type=4;
	device[536].regaddres=0x0081; device[536].func=4; device[536].pub_delay=60; device[536].write_func=0x10; device[536].group=50; device[536].type=4;
	device[537].regaddres=0x0083; device[537].func=4; device[537].pub_delay=60; device[537].write_func=0x10; device[537].group=50; device[537].type=4;
	device[538].regaddres=0x0085; device[538].func=4; device[538].pub_delay=60; device[538].write_func=0x10; device[538].group=50; device[538].type=4;
	device[539].regaddres=0x0920; device[539].func=4; device[539].pub_delay=60; device[539].write_func=0x10; device[539].group=51; device[539].type=0;
	device[540].regaddres=0x0921; device[540].func=4; device[540].pub_delay=60; device[540].write_func=0x10; device[540].group=51; device[540].type=0;
	device[541].regaddres=0x0922; device[541].func=4; device[541].pub_delay=60; device[541].write_func=0x10; device[541].group=51; device[541].type=0;
	device[542].regaddres=0x0923; device[542].func=4; device[542].pub_delay=60; device[542].write_func=0x10; device[542].group=51; device[542].type=0;
	device[543].regaddres=0x0924; device[543].func=4; device[543].pub_delay=60; device[543].write_func=0x10; device[543].group=51; device[543].type=0;
	device[544].regaddres=0x0925; device[544].func=4; device[544].pub_delay=60; device[544].write_func=0x10; device[544].group=51; device[544].type=0;
	device[545].regaddres=0x0926; device[545].func=4; device[545].pub_delay=60; device[545].write_func=0x10; device[545].group=51; device[545].type=0;
	device[546].regaddres=0x0927; device[546].func=4; device[546].pub_delay=60; device[546].write_func=0x10; device[546].group=51; device[546].type=0;
	device[547].regaddres=0x0928; device[547].func=4; device[547].pub_delay=60; device[547].write_func=0x10; device[547].group=51; device[547].type=0;
	device[548].regaddres=0x0929; device[548].func=4; device[548].pub_delay=60; device[548].write_func=0x10; device[548].group=51; device[548].type=0;
	device[549].regaddres=0x092A; device[549].func=4; device[549].pub_delay=60; device[549].write_func=0x10; device[549].group=51; device[549].type=0;
	device[550].regaddres=0x092B; device[550].func=4; device[550].pub_delay=60; device[550].write_func=0x10; device[550].group=51; device[550].type=0;
	device[551].regaddres=0x092C; device[551].func=4; device[551].pub_delay=60; device[551].write_func=0x10; device[551].group=51; device[551].type=0;
	device[552].regaddres=0x092D; device[552].func=4; device[552].pub_delay=60; device[552].write_func=0x10; device[552].group=51; device[552].type=0;
	device[553].regaddres=0x092E; device[553].func=4; device[553].pub_delay=60; device[553].write_func=0x10; device[553].group=51; device[553].type=0;
	device[554].regaddres=0x092F; device[554].func=4; device[554].pub_delay=60; device[554].write_func=0x10; device[554].group=51; device[554].type=0;
	device[555].regaddres=0x0930; device[555].func=4; device[555].pub_delay=60; device[555].write_func=0x10; device[555].group=51; device[555].type=0;
	device[556].regaddres=0x0931; device[556].func=4; device[556].pub_delay=60; device[556].write_func=0x10; device[556].group=51; device[556].type=0;
	device[557].regaddres=0x0932; device[557].func=4; device[557].pub_delay=60; device[557].write_func=0x10; device[557].group=51; device[557].type=0;
	device[558].regaddres=0x0933; device[558].func=4; device[558].pub_delay=60; device[558].write_func=0x10; device[558].group=51; device[558].type=0;
	device[559].regaddres=0x0934; device[559].func=4; device[559].pub_delay=60; device[559].write_func=0x10; device[559].group=51; device[559].type=0;
	device[560].regaddres=0x0935; device[560].func=4; device[560].pub_delay=60; device[560].write_func=0x10; device[560].group=51; device[560].type=0;
	device[561].regaddres=0x0936; device[561].func=4; device[561].pub_delay=60; device[561].write_func=0x10; device[561].group=51; device[561].type=0;
	device[562].regaddres=0x0937; device[562].func=4; device[562].pub_delay=60; device[562].write_func=0x10; device[562].group=51; device[562].type=0;
	device[563].regaddres=0x0938; device[563].func=4; device[563].pub_delay=60; device[563].write_func=0x10; device[563].group=51; device[563].type=0;
	device[564].regaddres=0x0939; device[564].func=4; device[564].pub_delay=60; device[564].write_func=0x10; device[564].group=51; device[564].type=0;
	device[565].regaddres=0x093B; device[565].func=4; device[565].pub_delay=60; device[565].write_func=0x10; device[565].group=52; device[565].type=0;
	device[566].regaddres=0x093C; device[566].func=4; device[566].pub_delay=60; device[566].write_func=0x10; device[566].group=52; device[566].type=0;
	device[567].regaddres=0x093D; device[567].func=4; device[567].pub_delay=60; device[567].write_func=0x10; device[567].group=52; device[567].type=0;
	device[568].regaddres=0x095E; device[568].func=4; device[568].pub_delay=60; device[568].write_func=0x10; device[568].group=53; device[568].type=0;
	device[569].regaddres=0x093A; device[569].func=4; device[569].pub_delay=60; device[569].write_func=1; device[569].group=54; device[569].type=0;

	device[570].regaddres=0x0087; device[570].func=4; device[570].pub_delay=10; device[570].write_func=0x10; device[570].group=55; device[570].type=0;
	device[571].regaddres=0x008A; device[571].func=4; device[571].pub_delay=10; device[571].write_func=0x10; device[571].group=56; device[571].type=0;
	device[572].regaddres=0x008B; device[572].func=4; device[572].pub_delay=10; device[572].write_func=0x10; device[572].group=56; device[572].type=1;
	device[573].regaddres=0x008D; device[573].func=4; device[573].pub_delay=10; device[573].write_func=0x10; device[573].group=56; device[573].type=1;
	device[574].regaddres=0x091A; device[574].func=4; device[574].pub_delay=10; device[574].write_func=0x10; device[574].group=57; device[574].type=6;
	device[575].regaddres=0x091D; device[575].func=4; device[575].pub_delay=10; device[575].write_func=0x10; device[575].group=57; device[575].type=6;
	device[576].regaddres=0x00EF; device[576].func=4; device[576].pub_delay=10; device[576].write_func=0x10; device[576].group=58; device[576].type=0;
	device[577].regaddres=0x00F0; device[577].func=4; device[577].pub_delay=10; device[577].write_func=0x10; device[577].group=58; device[577].type=0;
	device[578].regaddres=0x00F1; device[578].func=4; device[578].pub_delay=10; device[578].write_func=0x10; device[578].group=58; device[578].type=0;
	device[579].regaddres=0x00F2; device[579].func=4; device[579].pub_delay=10; device[579].write_func=0x10; device[579].group=58; device[579].type=0;
	device[580].regaddres=0x00F3; device[580].func=4; device[580].pub_delay=10; device[580].write_func=0x10; device[580].group=58; device[580].type=0;
	device[581].regaddres=0x00F4; device[581].func=4; device[581].pub_delay=10; device[581].write_func=0x10; device[581].group=58; device[581].type=0;
	device[582].regaddres=0x00F9; device[582].func=4; device[582].pub_delay=10; device[582].write_func=0x10; device[582].group=59; device[582].type=0;
	device[583].regaddres=0x0300; device[583].func=4; device[583].pub_delay=10; device[583].write_func=0x10; device[583].group=59; device[583].type=6;
	device[584].regaddres=0x00FD; device[584].func=4; device[584].pub_delay=10; device[584].write_func=0x10; device[584].group=59; device[584].type=7;
	device[585].regaddres=0x00FF; device[585].func=4; device[585].pub_delay=10; device[585].write_func=0x10; device[585].group=59; device[585].type=0;
	device[586].regaddres=0x0101; device[586].func=4; device[586].pub_delay=10; device[586].write_func=0x10; device[586].group=60; device[586].type=0;device[586].filtred=true;
	device[587].regaddres=0x0102; device[587].func=4; device[587].pub_delay=10; device[587].write_func=0x10; device[587].group=60; device[587].type=0;device[587].filtred=true;
	device[588].regaddres=0x0103; device[588].func=4; device[588].pub_delay=10; device[588].write_func=0x10; device[588].group=60; device[588].type=0;device[588].filtred=true;
	device[589].regaddres=0x0104; device[589].func=4; device[589].pub_delay=10; device[589].write_func=0x10; device[589].group=60; device[589].type=0;device[589].filtred=true;
	device[590].regaddres=0x0105; device[590].func=4; device[590].pub_delay=10; device[590].write_func=0x10; device[590].group=60; device[590].type=0;device[590].filtred=true;
	device[591].regaddres=0x0106; device[591].func=4; device[591].pub_delay=10; device[591].write_func=0x10; device[591].group=60; device[591].type=0;device[591].filtred=true;
	device[592].regaddres=0x0107; device[592].func=4; device[592].pub_delay=10; device[592].write_func=0x10; device[592].group=60; device[592].type=0;device[592].filtred=true;
	device[593].regaddres=0x0108; device[593].func=4; device[593].pub_delay=10; device[593].write_func=0x10; device[593].group=60; device[593].type=0;device[593].filtred=true;
	device[594].regaddres=0x0109; device[594].func=4; device[594].pub_delay=10; device[594].write_func=0x10; device[594].group=60; device[594].type=0;device[594].filtred=true;
	device[595].regaddres=0x010A; device[595].func=4; device[595].pub_delay=10; device[595].write_func=0x10; device[595].group=60; device[595].type=0;
	device[596].regaddres=0x010B; device[596].func=4; device[596].pub_delay=10; device[596].write_func=0x10; device[596].group=60; device[596].type=0;
	device[597].regaddres=0x010C; device[597].func=4; device[597].pub_delay=10; device[597].write_func=0x10; device[597].group=60; device[597].type=0;
	device[598].regaddres=0x010D; device[598].func=4; device[598].pub_delay=10; device[598].write_func=0x10; device[598].group=60; device[598].type=2;device[598].filtred=true;
	device[599].regaddres=0x010E; device[599].func=4; device[599].pub_delay=10; device[599].write_func=0x10; device[599].group=60; device[599].type=0;
	device[600].regaddres=0x010F; device[600].func=4; device[600].pub_delay=10; device[600].write_func=0x10; device[600].group=60; device[600].type=0;device[600].filtred=true;
	device[601].regaddres=0x0110; device[601].func=4; device[601].pub_delay=10; device[601].write_func=0x10; device[601].group=60; device[601].type=2;
	device[602].regaddres=0x0111; device[602].func=4; device[602].pub_delay=10; device[602].write_func=0x10; device[602].group=60; device[602].type=0;device[602].filtred=true;
	device[603].regaddres=0x0112; device[603].func=4; device[603].pub_delay=10; device[603].write_func=0x10; device[603].group=60; device[603].type=0;device[603].filtred=true;
	device[604].regaddres=0x0113; device[604].func=4; device[604].pub_delay=10; device[604].write_func=0x10; device[604].group=60; device[604].type=0;
	device[605].regaddres=0x0114; device[605].func=4; device[605].pub_delay=10; device[605].write_func=0x10; device[605].group=60; device[605].type=0;device[606].filtred=true;
	device[606].regaddres=0x0115; device[606].func=4; device[606].pub_delay=10; device[606].write_func=0x10; device[606].group=60; device[606].type=0;
	device[607].regaddres=0x0116; device[607].func=4; device[607].pub_delay=10; device[607].write_func=0x10; device[607].group=60; device[607].type=0;device[607].filtred=true;
	device[608].regaddres=0x0117; device[608].func=4; device[608].pub_delay=10; device[608].write_func=0x10; device[608].group=60; device[608].type=0;
	device[609].regaddres=0x0118; device[609].func=4; device[609].pub_delay=10; device[609].write_func=0x10; device[609].group=60; device[609].type=0;device[609].filtred=true;
	device[610].regaddres=0x0119; device[610].func=4; device[610].pub_delay=10; device[610].write_func=0x10; device[610].group=60; device[610].type=0;device[610].filtred=true;
	device[611].regaddres=0x011A; device[611].func=4; device[611].pub_delay=10; device[611].write_func=0x10; device[611].group=60; device[611].type=0;device[611].filtred=true;
	device[612].regaddres=0x011B; device[612].func=4; device[612].pub_delay=10; device[612].write_func=0x10; device[612].group=60; device[612].type=0;device[612].filtred=true;
	device[613].regaddres=0x011C; device[613].func=4; device[613].pub_delay=10; device[613].write_func=0x10; device[613].group=60; device[613].type=0;
	device[614].regaddres=0x0301; device[614].func=4; device[614].pub_delay=10; device[614].write_func=0x10; device[614].group=60; device[614].type=0;
	device[615].regaddres=0x011E; device[615].func=4; device[615].pub_delay=10; device[615].write_func=0x10; device[615].group=60; device[615].type=0;
	device[616].regaddres=0x011F; device[616].func=4; device[616].pub_delay=10; device[616].write_func=0x10; device[616].group=60; device[616].type=0;
	device[617].regaddres=0x0120; device[617].func=4; device[617].pub_delay=10; device[617].write_func=0x10; device[617].group=60; device[617].type=0;device[617].filtred=true;
	device[618].regaddres=0x0121; device[618].func=4; device[618].pub_delay=10; device[618].write_func=0x10; device[618].group=60; device[618].type=0;device[618].filtred=true;
	device[619].regaddres=0x0122; device[619].func=4; device[619].pub_delay=10; device[619].write_func=0x10; device[619].group=60; device[619].type=0;
	device[620].regaddres=0x0123; device[620].func=4; device[620].pub_delay=10; device[620].write_func=0x10; device[620].group=60; device[620].type=0;
	device[621].regaddres=0x0124; device[621].func=4; device[621].pub_delay=10; device[621].write_func=0x10; device[621].group=60; device[621].type=0;
	device[622].regaddres=0x0125; device[622].func=4; device[622].pub_delay=10; device[622].write_func=0x10; device[622].group=60; device[622].type=0;
	device[623].regaddres=0x0126; device[623].func=4; device[623].pub_delay=10; device[623].write_func=0x10; device[623].group=60; device[623].type=0;device[623].filtred=true;
	device[624].regaddres=0x0127; device[624].func=4; device[624].pub_delay=10; device[624].write_func=0x10; device[624].group=60; device[624].type=0;
	device[625].regaddres=0x0128; device[625].func=4; device[625].pub_delay=10; device[625].write_func=0x10; device[625].group=60; device[625].type=0;
	device[626].regaddres=0x0129; device[626].func=4; device[626].pub_delay=10; device[626].write_func=0x10; device[626].group=60; device[626].type=0;device[626].filtred=true;
	device[627].regaddres=0x012A; device[627].func=4; device[627].pub_delay=10; device[627].write_func=0x10; device[627].group=60; device[627].type=0;
	device[628].regaddres=0x012B; device[628].func=4; device[628].pub_delay=10; device[628].write_func=0x10; device[628].group=60; device[628].type=0;
	device[629].regaddres=0x012C; device[629].func=4; device[629].pub_delay=10; device[629].write_func=0x10; device[629].group=60; device[629].type=0;
	device[630].regaddres=0x012D; device[630].func=4; device[630].pub_delay=10; device[630].write_func=0x10; device[630].group=60; device[630].type=0;
	device[631].regaddres=0x012E; device[631].func=4; device[631].pub_delay=10; device[631].write_func=0x10; device[631].group=60; device[631].type=0;
	device[632].regaddres=0x012F; device[632].func=4; device[632].pub_delay=10; device[632].write_func=0x10; device[632].group=60; device[632].type=0;
	device[633].regaddres=0x0130; device[633].func=4; device[633].pub_delay=10; device[633].write_func=0x10; device[633].group=60; device[633].type=0;
	device[634].regaddres=0x0131; device[634].func=4; device[634].pub_delay=10; device[634].write_func=0x10; device[634].group=60; device[634].type=0;
	device[635].regaddres=0x0350; device[635].func=4; device[635].pub_delay=10; device[635].write_func=0x10; device[635].group=61; device[635].type=1;
	device[636].regaddres=0x0352; device[636].func=4; device[636].pub_delay=10; device[636].write_func=0x10; device[636].group=61; device[636].type=0;
	device[637].regaddres=0x0353; device[637].func=4; device[637].pub_delay=10; device[637].write_func=0x10; device[637].group=61; device[637].type=0;
	device[638].regaddres=0x0354; device[638].func=4; device[638].pub_delay=10; device[638].write_func=0x10; device[638].group=61; device[638].type=0;
	device[639].regaddres=0x0355; device[639].func=4; device[639].pub_delay=10; device[639].write_func=0x10; device[639].group=61; device[639].type=0;
	device[640].regaddres=0x0600; device[640].func=4; device[640].pub_delay=10; device[640].write_func=0x10; device[640].group=62; device[640].type=1;
	device[641].regaddres=0x0602; device[641].func=4; device[641].pub_delay=10; device[641].write_func=0x10; device[641].group=62; device[641].type=1;
	device[642].regaddres=0x0604; device[642].func=4; device[642].pub_delay=10; device[642].write_func=0x10; device[642].group=62; device[642].type=1;
	device[643].regaddres=0x0606; device[643].func=4; device[643].pub_delay=10; device[643].write_func=0x10; device[643].group=62; device[643].type=1;
	device[644].regaddres=0x0608; device[644].func=4; device[644].pub_delay=10; device[644].write_func=0x10; device[644].group=62; device[644].type=1;
	device[645].regaddres=0x060A; device[645].func=4; device[645].pub_delay=10; device[645].write_func=0x10; device[645].group=62; device[645].type=1;
	device[646].regaddres=0x060C; device[646].func=4; device[646].pub_delay=10; device[646].write_func=0x10; device[646].group=62; device[646].type=1;
	device[647].regaddres=0x060E; device[647].func=4; device[647].pub_delay=10; device[647].write_func=0x10; device[647].group=62; device[647].type=1;
	device[648].regaddres=0x0610; device[648].func=4; device[648].pub_delay=10; device[648].write_func=0x10; device[648].group=62; device[648].type=1;
	device[649].regaddres=0x0612; device[649].func=4; device[649].pub_delay=10; device[649].write_func=0x10; device[649].group=62; device[649].type=0;
	device[650].regaddres=0x0613; device[650].func=4; device[650].pub_delay=10; device[650].write_func=0x10; device[650].group=62; device[650].type=0;
	device[651].regaddres=0x0614; device[651].func=4; device[651].pub_delay=10; device[651].write_func=0x10; device[651].group=62; device[651].type=0;
	device[652].regaddres=0x0615; device[652].func=4; device[652].pub_delay=10; device[652].write_func=0x10; device[652].group=62; device[652].type=0;
	device[653].regaddres=0x0616; device[653].func=4; device[653].pub_delay=10; device[653].write_func=0x10; device[653].group=62; device[653].type=1;
	device[654].regaddres=0x0618; device[654].func=4; device[654].pub_delay=10; device[654].write_func=0x10; device[654].group=62; device[654].type=1;
	device[655].regaddres=0x061A; device[655].func=4; device[655].pub_delay=10; device[655].write_func=0x10; device[655].group=62; device[655].type=0;
	device[656].regaddres=0x061B; device[656].func=4; device[656].pub_delay=10; device[656].write_func=0x10; device[656].group=62; device[656].type=1;
	device[657].regaddres=0x061D; device[657].func=4; device[657].pub_delay=10; device[657].write_func=0x10; device[657].group=62; device[657].type=1;
	device[658].regaddres=0x061F; device[658].func=4; device[658].pub_delay=10; device[658].write_func=0x10; device[658].group=62; device[658].type=1;
	device[659].regaddres=0x0621; device[659].func=4; device[659].pub_delay=10; device[659].write_func=0x10; device[659].group=62; device[659].type=1;
	device[660].regaddres=0x0623; device[660].func=4; device[660].pub_delay=10; device[660].write_func=0x10; device[660].group=62; device[660].type=1;
	device[661].regaddres=0x0625; device[661].func=4; device[661].pub_delay=10; device[661].write_func=0x10; device[661].group=62; device[661].type=1;
	device[662].regaddres=0x0627; device[662].func=4; device[662].pub_delay=10; device[662].write_func=0x10; device[662].group=62; device[662].type=1;
	device[663].regaddres=0x0629; device[663].func=4; device[663].pub_delay=10; device[663].write_func=0x10; device[663].group=62; device[663].type=0;
	device[664].regaddres=0x062B; device[664].func=4; device[664].pub_delay=10; device[664].write_func=0x10; device[664].group=63; device[664].type=1;
	device[665].regaddres=0x062D; device[665].func=4; device[665].pub_delay=10; device[665].write_func=0x10; device[665].group=63; device[665].type=1;
	device[666].regaddres=0x062F; device[666].func=4; device[666].pub_delay=10; device[666].write_func=0x10; device[666].group=63; device[666].type=1;
	device[667].regaddres=0x0631; device[667].func=4; device[667].pub_delay=10; device[667].write_func=0x10; device[667].group=63; device[667].type=1;
	device[668].regaddres=0x0633; device[668].func=4; device[668].pub_delay=10; device[668].write_func=0x10; device[668].group=63; device[668].type=1;
	device[669].regaddres=0x0635; device[669].func=4; device[669].pub_delay=10; device[669].write_func=0x10; device[669].group=63; device[669].type=1;
	device[670].regaddres=0x0637; device[670].func=4; device[670].pub_delay=10; device[670].write_func=0x10; device[670].group=63; device[670].type=1;
	device[671].regaddres=0x0639; device[671].func=4; device[671].pub_delay=10; device[671].write_func=0x10; device[671].group=63; device[671].type=1;
	device[672].regaddres=0x063B; device[672].func=4; device[672].pub_delay=10; device[672].write_func=0x10; device[672].group=63; device[672].type=1;
	device[673].regaddres=0x063D; device[673].func=4; device[673].pub_delay=10; device[673].write_func=0x10; device[673].group=63; device[673].type=1;
	device[674].regaddres=0x063F; device[674].func=4; device[674].pub_delay=10; device[674].write_func=0x10; device[674].group=63; device[674].type=1;
	device[675].regaddres=0x0641; device[675].func=4; device[675].pub_delay=10; device[675].write_func=0x10; device[675].group=63; device[675].type=1;
	device[676].regaddres=0x0643; device[676].func=4; device[676].pub_delay=10; device[676].write_func=0x10; device[676].group=63; device[676].type=1;
	device[677].regaddres=0x0645; device[677].func=4; device[677].pub_delay=10; device[677].write_func=0x10; device[677].group=63; device[677].type=1;
	device[678].regaddres=0x0647; device[678].func=4; device[678].pub_delay=10; device[678].write_func=0x10; device[678].group=63; device[678].type=1;
	device[679].regaddres=0x0649; device[679].func=4; device[679].pub_delay=10; device[679].write_func=0x10; device[679].group=63; device[679].type=1;
	device[680].regaddres=0x064B; device[680].func=4; device[680].pub_delay=10; device[680].write_func=0x10; device[680].group=63; device[680].type=1;
	device[681].regaddres=0x064D; device[681].func=4; device[681].pub_delay=10; device[681].write_func=0x10; device[681].group=63; device[681].type=1;
	device[682].regaddres=0x064F; device[682].func=4; device[682].pub_delay=10; device[682].write_func=0x10; device[682].group=63; device[682].type=1;
	device[683].regaddres=0x0651; device[683].func=4; device[683].pub_delay=10; device[683].write_func=0x10; device[683].group=63; device[683].type=1;
	device[684].regaddres=0x0653; device[684].func=4; device[684].pub_delay=10; device[684].write_func=0x10; device[684].group=63; device[684].type=1;
	device[685].regaddres=0x0655; device[685].func=4; device[685].pub_delay=10; device[685].write_func=0x10; device[685].group=63; device[685].type=1;
	device[686].regaddres=0x0657; device[686].func=4; device[686].pub_delay=10; device[686].write_func=0x10; device[686].group=63; device[686].type=1;
	device[687].regaddres=0x0659; device[687].func=4; device[687].pub_delay=10; device[687].write_func=0x10; device[687].group=63; device[687].type=0;
	device[688].regaddres=0x065A; device[688].func=4; device[688].pub_delay=10; device[688].write_func=0x10; device[688].group=63; device[688].type=0;
	device[689].regaddres=0x065B; device[689].func=4; device[689].pub_delay=10; device[689].write_func=0x10; device[689].group=63; device[689].type=0;
	device[690].regaddres=0x065C; device[690].func=4; device[690].pub_delay=10; device[690].write_func=0x10; device[690].group=63; device[690].type=0;
	device[691].regaddres=0x065D; device[691].func=4; device[691].pub_delay=10; device[691].write_func=0x10; device[691].group=63; device[691].type=0;
	device[692].regaddres=0x065E; device[692].func=4; device[692].pub_delay=10; device[692].write_func=0x10; device[692].group=63; device[692].type=0;
	device[693].regaddres=0x065F; device[693].func=4; device[693].pub_delay=10; device[693].write_func=0x10; device[693].group=63; device[693].type=0;
	device[694].regaddres=0x0660; device[694].func=4; device[694].pub_delay=10; device[694].write_func=0x10; device[694].group=63; device[694].type=0;
	device[695].regaddres=0x0661; device[695].func=4; device[695].pub_delay=10; device[695].write_func=0x10; device[695].group=63; device[695].type=0;
	device[696].regaddres=0x0662; device[696].func=4; device[696].pub_delay=10; device[696].write_func=0x10; device[696].group=63; device[696].type=0;
	device[697].regaddres=0x0663; device[697].func=4; device[697].pub_delay=10; device[697].write_func=0x10; device[697].group=63; device[697].type=1;
	device[698].regaddres=0x0665; device[698].func=4; device[698].pub_delay=10; device[698].write_func=0x10; device[698].group=63; device[698].type=1;
	device[699].regaddres=0x0667; device[699].func=4; device[699].pub_delay=10; device[699].write_func=0x10; device[699].group=63; device[699].type=1;
	device[700].regaddres=0x0669; device[700].func=4; device[700].pub_delay=10; device[700].write_func=0x10; device[700].group=63; device[700].type=0;
	device[701].regaddres=0x066A; device[701].func=4; device[701].pub_delay=10; device[701].write_func=0x10; device[701].group=63; device[701].type=0;
	device[702].regaddres=0x066B; device[702].func=4; device[702].pub_delay=10; device[702].write_func=0x10; device[702].group=63; device[702].type=4;
	device[703].regaddres=0x08FA; device[703].func=4; device[703].pub_delay=10; device[703].write_func=0x10; device[703].group=64; device[703].type=1;
	device[704].regaddres=0x08FD; device[704].func=4; device[704].pub_delay=10; device[704].write_func=0x10; device[704].group=65; device[704].type=0;
	device[705].regaddres=0x08FE; device[705].func=4; device[705].pub_delay=10; device[705].write_func=0x10; device[705].group=65; device[705].type=0;
	device[706].regaddres=0x08FF; device[706].func=4; device[706].pub_delay=10; device[706].write_func=0x10; device[706].group=65; device[706].type=0;
	device[707].regaddres=0x0900; device[707].func=4; device[707].pub_delay=10; device[707].write_func=0x10; device[707].group=65; device[707].type=0;
	device[708].regaddres=0x0901; device[708].func=4; device[708].pub_delay=10; device[708].write_func=0x10; device[708].group=65; device[708].type=0;
	device[709].regaddres=0x0902; device[709].func=4; device[709].pub_delay=10; device[709].write_func=0x10; device[709].group=65; device[709].type=0;
	device[710].regaddres=0x0903; device[710].func=4; device[710].pub_delay=10; device[710].write_func=0x10; device[710].group=65; device[710].type=0;
	device[711].regaddres=0x0904; device[711].func=4; device[711].pub_delay=10; device[711].write_func=0x10; device[711].group=65; device[711].type=0;
	device[712].regaddres=0x0905; device[712].func=4; device[712].pub_delay=10; device[712].write_func=0x10; device[712].group=65; device[712].type=0;
	device[713].regaddres=0x0906; device[713].func=4; device[713].pub_delay=10; device[713].write_func=0x10; device[713].group=65; device[713].type=0;
	device[714].regaddres=0x0907; device[714].func=4; device[714].pub_delay=10; device[714].write_func=0x10; device[714].group=65; device[714].type=0;
	device[715].regaddres=0x0908; device[715].func=4; device[715].pub_delay=10; device[715].write_func=0x10; device[715].group=65; device[715].type=0;
	device[716].regaddres=0x0909; device[716].func=4; device[716].pub_delay=10; device[716].write_func=0x10; device[716].group=65; device[716].type=0;
	device[717].regaddres=0x090A; device[717].func=4; device[717].pub_delay=10; device[717].write_func=0x10; device[717].group=65; device[717].type=6;
	device[718].regaddres=0x090D; device[718].func=4; device[718].pub_delay=10; device[718].write_func=0x10; device[718].group=65; device[718].type=0;
	device[719].regaddres=0x00FA; device[719].func=4; device[719].pub_delay=10; device[719].write_func=0x10; device[719].group=66; device[719].type=0;
	device[720].regaddres=0x00FB; device[720].func=4; device[720].pub_delay=10; device[720].write_func=0x10; device[720].group=66; device[720].type=0;
	device[721].regaddres=0x00FC; device[721].func=4; device[721].pub_delay=10; device[721].write_func=0x10; device[721].group=66; device[721].type=0;
	device[722].regaddres=0x00FD; device[722].func=4; device[722].pub_delay=10; device[722].write_func=0x10; device[722].group=66; device[722].type=0;
	device[723].regaddres=0x00FE; device[723].func=4; device[723].pub_delay=10; device[723].write_func=0x10; device[723].group=66; device[723].type=0;
	device[724].regaddres=0x039B; device[724].func=3; device[724].pub_delay=10; device[724].write_func=0x10; device[724].group=67; device[724].type=0;device[724].filtred=true;
	device[725].regaddres=0x03A0; device[725].func=3; device[725].pub_delay=10; device[725].write_func=0x10; device[725].group=68; device[725].type=0;device[725].filtred=true;
	device[726].regaddres=0x0338; device[726].func=3; device[726].pub_delay=10; device[726].write_func=0x10; device[726].group=69; device[726].type=0;device[726].filtred=true;
	device[727].regaddres=0x0339; device[727].func=3; device[727].pub_delay=10; device[727].write_func=0x10; device[727].group=69; device[727].type=0;device[727].filtred=true;
	device[728].regaddres=0x033A; device[728].func=3; device[728].pub_delay=10; device[728].write_func=0x10; device[728].group=69; device[728].type=0;device[728].filtred=true;
	device[729].regaddres=0x033B; device[729].func=3; device[729].pub_delay=10; device[729].write_func=0x10; device[729].group=69; device[729].type=0;device[729].filtred=true;
	device[730].regaddres=0x033C; device[730].func=3; device[730].pub_delay=10; device[730].write_func=0x10; device[730].group=69; device[730].type=0;device[730].filtred=true;
	device[731].regaddres=0x0201; device[731].func=3; device[731].pub_delay=1000; device[731].write_func=5; device[731].group=70; device[731].type=0;
	device[732].regaddres=0x0203; device[732].func=3; device[732].pub_delay=1000; device[732].write_func=5; device[732].group=71; device[732].type=0;





	//	uint8_t datatest[2]={0xFF,0xFF};
	//	uint8_t regaddresHI[10]={0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x04,0x04,0x04};
	//	uint8_t regaddresLO[10]={0xFE,0x02,0x03,0x04,0x05,0xFF,0x01,0x0E,0x0D,0x0C};
	uint32_t del=0;
	bool slow_flag=false;
	bool slow_send=false;
	bool modbus_slow=false;
	bool start_f=true;
	string pub_topic;
	/* Infinite loop */


	for(;;)
	{

		//write_modbus(0x01, 0x00, 0x01, 1, datatest);
		//osDelay(500);
		string topic;
		char top[10];

		string sen;
		uint8_t usn[1024]="pub: ";
		uint8_t start[600]="pub: ";
		string sn_top;
		int st_len=5;
		int sn_len=5;
		uint8_t num_device=1;

		//device[read_modbus_count].res=read_modbus(0x01,device[read_modbus_count].regaddresHI,device[read_modbus_count].regaddresLO,device[read_modbus_count].func, 1);
		for(int i=read_modbus_count;i<731;i++)
		{
			if(device[i].group==device[i+1].group)
			{
				num_device++;
			}
			else
			{
				break;
			}
			if(num_device==10)
			{
				break;
			}
		}
		read_modbus(0x01,device[read_modbus_count].regaddresHI,device[read_modbus_count].regaddresLO,device[read_modbus_count].func, num_device);
		num_device=1;
		//		if(device[read_modbus_count].res==device[read_modbus_count].old_res)
		//		{
		//			//device[read_modbus_count].pub=false;
		//		}
		//		else
		//		{
		//			device[read_modbus_count].pub=true;
		//		}
		sprintf(top,"%01Xx%02X%02X",device[pub_count].func,device[pub_count].regaddresHI,device[pub_count].regaddresLO);
		//topic ="1001999/1/";
		topic =imei+"/1/";
		topic+=top;

		if(netw_f&&imei_f)
		{
			pub_topic=imei+"/Modem/IMEI";
			publishMessage(pub_topic, imei);
			int j=0;
			if((st_len+buflen-2)>600)
			{
				break;
			}
			for(int ii=st_len;ii<st_len+buflen-2;ii++)
			{
				start[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			st_len+=buflen-2;
			pub_topic=imei+"/SIM_Operator";
			publishMessage(pub_topic, netw);
			j=0;
			if((st_len+buflen-2)>600)
			{
				break;
			}
			for(int ii=st_len;ii<st_len+buflen-2;ii++)
			{
				start[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			st_len+=buflen-2;
			pub_topic=imei+"/Modem/Model";
			publishMessage(pub_topic, "ARM_Modem");
			j=0;
			if((st_len+buflen-2)>600)
			{
				break;
			}
			for(int ii=st_len;ii<st_len+buflen-2;ii++)
			{
				start[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			st_len+=buflen-2;
			pub_topic=imei+"/Modem/Name";
			publishMessage(pub_topic, "TEST_DESK");
			j=0;
			if((st_len+buflen-2)>600)
			{
				break;
			}
			for(int ii=st_len;ii<st_len+buflen-2;ii++)
			{
				start[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			st_len+=buflen-2;
			pub_topic=imei+"/Modem/Software_version";
			publishMessage(pub_topic, "0.0.1a");
			j=0;
			if((st_len+buflen-2)>600)
			{
				break;
			}
			for(int ii=st_len;ii<st_len+buflen-2;ii++)
			{
				start[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			st_len+=buflen-2;
			start[st_len]='\n';
			start[st_len+1]='\r';
		}
		if(modem_flag)
		{
			//char send_modem[512];
			//mftest=device[pub_count].res;
			//mftest=device[pub_count].res;
			char modem_state[150];
			if(csq_f&&imei_f&&netw_f)
			{
				sprintf(modem_state,"%s,3G, %s, imei %s, v0.9.80, ConfDate 2024-06-14 07:46:46 serv con",netw,csq,imei);
			}
			//			pub_topic=imei+"/1//#";
			//			MQTT_SUB(pub_topic);
			//
			//			//sprintf(send_modem,"sub: %s\n\r",buffer_sub);
			//			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer_sub, sub_len+5);
			//			//			HAL_UART_Transmit(&huart3, (uint8_t*)"To_F0: ", 7,50);
			//			//			HAL_UART_Transmit(&huart3, (uint8_t*)buffer_sub, sub_len+5,200);
			//			//			HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
			//			osDelay(40);
			pub_topic=imei+"/System//#";
			MQTT_SUB(pub_topic);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer_sub, sub_len+5);
			osDelay(40);
			buflen=0;

			if(slow_flag)
			{
				for(int i=slow_counter;i<570;i++)
				{
					if((((device[i].last_pub+device[i].pub_delay*1000)<=HAL_GetTick())&&device[i].res!=device[i].old_res)||((device[i].last_pub+device[i].pub_delay*1000*6)<=HAL_GetTick()))
					{
						//device[i].res=read_modbus(0x01,device[i].regaddresHI,device[i].regaddresLO,device[i].func, 1);
						device[i].pub=true;
						device[i].last_pub=HAL_GetTick();
					}
					if(device[i].pub)
					{
						sn_top=imei+"/1/";
						sprintf(top,"%01Xx%02X%02X",device[i].func,device[i].regaddresHI,device[i].regaddresLO);
						sn_top+=top;
						publishMessage(sn_top, device[i].res);
						device[i].old_res=device[i].res;
						//				device[i].last_pub=
						int j=0;
						if((sn_len+buflen-2)>600)
						{

							slow_counter=i;
							slow_send=true;
							break;
						}
						for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
						{
							usn[ii]=buffer_pub[j];
							j++;
							//sen.push_back((char)buffer_pub[ii-buflen]);
						}
						sn_len+=buflen-2;
					}
					if(i==569)
					{

						slow_counter=0;
						device[i].pub=false;
						break;

					}
					device[i].pub=false;
				}
				slow_send=true;
				if(csq_f&&imei_f&&netw_f)
				{
					int j=0;
					pub_topic=imei+"/Modem/modem_state";
					publishMessage(pub_topic, modem_state);
					for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
					{
						usn[ii]=buffer_pub[j];
						j++;
						//sen.push_back((char)buffer_pub[ii-buflen]);
					}
					sn_len+=buflen-2;
				}
			}

			if(!slow_flag)
			{
				for(int i=counter;i<731;i++)
				{
					if((((device[i].last_pub+device[i].pub_delay*1000)<=HAL_GetTick())&&device[i].res!=device[i].old_res)||((device[i].last_pub+device[i].pub_delay*1000*6)<=HAL_GetTick())||(((device[i].last_pub+device[i].pub_delay*1000)<=HAL_GetTick())&&(i==717||i==575||i==719||i==584||i==722||i==585||i==598||i==587||i==588||i==589)))
					{
						//device[i].res=read_modbus(0x01,device[i].regaddresHI,device[i].regaddresLO,device[i].func, 1);
						device[i].pub=true;
						device[i].last_pub=HAL_GetTick();
					}
					if(device[i].pub)
					{
						sn_top=imei+"/1/";
						sprintf(top,"%01Xx%02X%02X",device[i].func,device[i].regaddresHI,device[i].regaddresLO);
						sn_top+=top;
						publishMessage(sn_top, device[i].res);
						device[i].old_res=device[i].res;
						//				device[i].last_pub=
						int j=0;
						if((sn_len+buflen-2)>500)
						{

							counter=i-1;

							break;
						}
						for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
						{
							usn[ii]=buffer_pub[j];
							j++;
							//sen.push_back((char)buffer_pub[ii-buflen]);
						}
						sn_len+=buflen-2;
					}
					if(i>=730)
					{
						slow_flag=true;
						counter=570;
						device[i].pub=false;
						//publishMessage("999999/Modem/MB_ConnStat", "TX 1 RX 1 RX_ERR 0");
						break;
					}
					device[i].pub=false;
				}
			}
			int j=0;
			char conn_stat[50];
			sprintf(conn_stat,"TX %d RX %d RX_ERR 0 COM %d",tx_counter,rx_counter,tx_count);
			pub_topic=imei+"/Modem/MB_ConnStat";
			publishMessage(pub_topic, conn_stat);
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x00FF";
			publishMessage(pub_topic, device[585].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x00F4";
			publishMessage(pub_topic, device[581].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x00F9";
			publishMessage(pub_topic, device[582].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x010D";
			publishMessage(pub_topic, device[598].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x0105";
			publishMessage(pub_topic, device[590].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x0106";
			publishMessage(pub_topic, device[591].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			pub_topic=imei+"/1/4x0107";
			publishMessage(pub_topic, device[592].res);
			j=0;
			for(int ii=sn_len;ii<sn_len+buflen-2;ii++)
			{
				usn[ii]=buffer_pub[j];
				j++;
				//sen.push_back((char)buffer_pub[ii-buflen]);
			}
			sn_len+=buflen-2;
			usn[sn_len]='\n';
			usn[sn_len+1]='\r';
			if(sn_len>7)
			{
				sendMQTTConnect("","";
				//sprintf(send_modem,"ini: %s\n\r",packet);
				HAL_UART_Transmit_DMA(&huart1, (uint8_t*)packet, packetSize+5);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)"To_F0: ", 7,50);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)packet, packetSize+5,200);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
				osDelay(40);
				//	publishMessage(topic, mftest);
				//	sprintf(send_modem,"pub: %s\n\r",buffer_pub);
				if(start_f&&csq_f&&imei_f&&netw_f)
				{
					HAL_UART_Transmit_DMA(&huart1, start, st_len+5);
					start_f=false;
					osDelay(100);
				}
				tx_counter++;
				for(int z=0;z<1;z++)
				{
					HAL_UART_Transmit_DMA(&huart1, usn, sn_len+5);
					osDelay(100);
					if(slow_send)
					{
						slow_send=false;
						slow_flag=false;
					}
				}
				sn_len=5;

				//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer_pub, buflen+5);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)"To_F0: ", 7,50);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)buffer_pub, buflen+5,200);
				//				HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);


			}


		}
		//		write_modbus(0x01, 0x00, 0x01, 1, datatest);
		//		osDelay(500);
		//		mftest=read_modbus(0x01, 0x00, 0x01, 1);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		modem_flag=false;
		osDelay(10);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
 * @brief Function implementing the myTask02 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
	/* USER CODE BEGIN StartTask02 */
	/* Infinite loop */
	for(;;)
	{
		modem_status();
		osDelay(10);
	}
	/* USER CODE END StartTask02 */
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
