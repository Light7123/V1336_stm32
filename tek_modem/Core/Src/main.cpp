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
#include "UnixTime.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

uint8_t rx_data[1600]; //="+init,tcp://test.mosquitto.org:1883,user";
int init_flag;
int i=0;
//message_mqtt_recive mqtt_message;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
bool modem_flag=false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	// передана половина данных
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	// завершена передача всех данных
}
uint32_t unixtime=0;
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
uint32_t GetTimeNTP()
{
	HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CNTP=\"ntp0.ntp-servers.net\",32\r\n",39,100);
	uint32_t timeUnix = unixtime;
	char buffer_time[255];
	HAL_Delay(1000);
	int NTP_wait = 1;
	int i=0;
	int count = 0;
	while(NTP_wait && count < 3)
	{
		count++;
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CCLK?\r\n",12,50);
		HAL_Delay(1000);
		UARTPrint("getTimeNTP\r\n");
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer_time, 100);

		UARTPrint(buffer_time);
		i=0;

		for(;i<sizeof(buffer_time);i++)//ищем время в полученной строке
		{
			if(buffer_time[i]== 'O' && buffer_time[i+1]=='K')
			{
				for(;i<sizeof(buffer_time);i++)
				{
					if(buffer_time[i]== 'L' && buffer_time[i+1]=='K' && buffer_time[i+2]==':')
					{
						NTP_wait = 0;

						char *istr = strstr((char*)buffer_time,"CCLK:"); //1684304269
						if(istr != NULL)
						{
							char timeStr[17];
							memcpy(timeStr,istr+7,17);

							uint16_t yearC =  (timeStr[0]-'0')*10 + (timeStr[1]-'0') + 2000;
							uint8_t monthC = (timeStr[3]-'0')*10 + (timeStr[4]-'0');
							uint8_t dayC = (timeStr[6]-'0')*10 + (timeStr[7]-'0');
							uint8_t hourC = (timeStr[9]-'0')*10 + (timeStr[10]-'0');
							uint8_t minuteC = (timeStr[12]-'0')*10 + (timeStr[13]-'0');
							uint8_t secondC = (timeStr[15]-'0')*10 + (timeStr[16]-'0');

							setDateTime(yearC, monthC, dayC, hourC+4, minuteC, secondC);
							timeUnix = getUnix();
							UARTPrint("timeUnix: %u\n",timeUnix);

							if(timeUnix > UNIXTIME || timeUnix <  UNIXTIME+10000000)
							{
								SetRTC(yearC-2000, monthC, dayC, hourC, minuteC, secondC);
								UARTPrint("set RTC time: %u\n",timeUnix);
								break;
							}
						}
					}
				}
			}
		}
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CNTP=\"ntp0.NL.net\",32\r\n",30,100);
		HAL_Delay(1000);

		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CCLK?\r\n",13,50);
	}

	if(timeUnix <= UNIXTIME || timeUnix >  UNIXTIME+10000000) timeUnix = UNIXTIME;
	return timeUnix;


}
int error_count=0;
uint32_t timer;
void modem_work()
{
	char buffer[500];
	bool ok_flag=false;
	string log_buffer;
	for(int i=0;i<10;i++)
	{


		uint32_t sim_start_Delay = 0;
		//	HAL_UART_Transmit(&huart1, (uint8_t*)"ATE1\r\n",6,100);
		//	HAL_Delay(100);
		//	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer, 100);
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", 4,100);
		HAL_Delay(300);
		//	timer=HAL_GetTick();
		//log_buffer<<"LOG: STM32F0: "<<"AT\r\n";
		//	log_buffer.append("LOG: STM32F0: ").append("AT\r\n");
		//		HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
		//		HAL_Delay(300);
		//	log_buffer.clear();
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) buffer, 100);
		HAL_Delay(1000);
		//		HAL_UART_Receive(&huart1, (uint8_t*)buffer, 500, 1000);
		//		timer=HAL_GetTick();
		//		log_buffer.append("LOG: MODEM: ").append(buffer);
		//		HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
		//		log_buffer.clear();
		for(int i=0;i<499;i++)
		{
			if(buffer[i]=='O'&&buffer[i+1]=='K')
			{
				ok_flag=true;
				break;
			}
		}
		if(strstr(buffer,"SIM not inserted"))
		{
			//UARTPrint("MODEM ERROR\r\n");
			//glcd_int_puts(modem_error, 0, 0);
			//auth_flag=0;
			init_flag=0;
			sub=false;
			memset(buffer,9,500);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
			modem_flag = 0;
		}
		else if(ok_flag)
		{
			//UARTPrint("MODEM READY\r\n");
			//	glcd_int_puts(modem_ok, 0, 0);
			//		HAL_UART_Transmit(&huart1, (uint8_t*)"AT&F\r\n", 8,100);
			//		HAL_Delay(500);
			//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer, 100);
			memset(buffer,9,500);
			error_count=0;
			HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"OK\n\r", 4);
			modem_flag=true;
			break;
		}
		else
		{
			//UARTPrint("MODEM ERROR\r\n");
			//glcd_int_puts(modem_error, 0, 0);
			//auth_flag=0;

			error_count++;
			HAL_Delay(1000);
			if(error_count>5)
			{
				sub=false;
				memset(buffer,9,500);
				init_flag=0;
				error_count=0;
				modem_flag = 0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
			}


		}
	}

}


void CSQ_status()
{
	char ch[30];

	memset(buffer3, 9, sizeof(buffer3));
	HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CSQ\r\n" , 10,100);
	HAL_Delay(100);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 1500);
	HAL_Delay(100);
	for(int i =0;i<1480;i++)
	{
		if(buffer3[i]=='C'&&buffer3[i+1]=='S'&&buffer3[i+2]=='Q'&&buffer3[i+3]==':')
		{
			int j=0;
			for(int ii=i+4;j<6;ii++)
			{
				ch[j]=buffer3[ii];
				j++;
			}
			ch[j]=0;
			char tx_buf[35];
			sprintf(tx_buf,"CSQS:%s\n\r",ch);
			HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, j+7,150);
			//ch[j]=0;
			break;
		}
	}

}

void Get_IMEI()
{
	char ch[30];

	memset(buffer3, 9, sizeof(buffer3));
	HAL_UART_Transmit(&huart1,(uint8_t*)"ATI\r\n" , 10,100);
	HAL_Delay(100);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 1500);
	HAL_Delay(100);
	for(int i =0;i<1480;i++)
	{
		if(buffer3[i]=='I'&&buffer3[i+1]=='M'&&buffer3[i+2]=='E'&&buffer3[i+3]=='I'&&buffer3[i+4]==':')
		{
			int j=0;
			for(int ii=i+5;j<16;ii++)
			{
				ch[j]=buffer3[ii];
				j++;
			}
			ch[j]=0;
			//ch[j]=0;
			char tx_buf[35];
			sprintf(tx_buf,"IMEI:%s\n\r",ch);
			HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf,j+7,150);
			break;
		}

	}

}

void Get_Network_Info()
{
	char ch[30];

	memset(buffer3, 9, sizeof(buffer3));
	HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CSPN?\r\n" , 12,100);
	HAL_Delay(100);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 1500);
	HAL_Delay(100);
	for(int i =0;i<1480;i++)
	{
		if(buffer3[i]=='C'&&buffer3[i+1]=='S'&&buffer3[i+2]=='P'&&buffer3[i+3]=='N'&&buffer3[i+4]==':')
		{
			int j=0;
			for(int ii=i+6;j<11;ii++)
			{
				ch[j]=buffer3[ii];
				j++;
			}
			ch[j]=0;
			char tx_buf[35];
			sprintf(tx_buf,"NETW:%s\n\r",ch);
			HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf,j+7,150);
			break;
		}
	}


}
void modem_status()
{
	char buffer[512];
	char ch[30];
	HAL_UART_Transmit_DMA(&huart1,(uint8_t*)"AT&F\r\n" , 10);
	HAL_Delay(50);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer, 512);
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
//uint32_t message_count=0;
int sub_len;
int pub_len;
int con_len;
char recive_packet_init[256];
char recive_packet_send[1010];
char recive_packet_sub[256];
uint32_t OK_Delay=0;
void modem_wait()
{
	int test=0;
	char ch[20];
	//	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_data, 200);
	bool corect_rec=false;
	for(i=0;i<1;i++)
	{
		if(init_flag==1)
		{
			HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"INIT\n\r", 6);
		}
		else
		{
			//			if(HAL_GetTick()-OK_Delay>10)
			//			{
			//			OK_Delay=HAL_GetTick();
			for(int jk=0;jk<1;jk++)
			{

				HAL_UART_Transmit(&huart2, (uint8_t*)"OK\n\r", 4,40);

			}
			//			}
		}
		HAL_Delay(15);
		HAL_UART_Receive_DMA(&huart2, rx_data, 1600);
		for(int r=0;r<1590;r++)
		{

			if(rx_data[r]=='s'&&rx_data[r+1]=='u'&&rx_data[r+2]=='b'&&rx_data[r+3]==':')
			{
				for(int ii=5+r;ii<1590;ii++)
				{
					if(rx_data[ii]=='\n')
					{
						sub_len=ii-5-r;
						break;
					}
					else
					{
						recive_packet_sub[ii-5-r]=rx_data[ii];
					}
				}
			}
			if(rx_data[r]=='i'&&rx_data[r+1]=='n'&&rx_data[r+2]=='i'&&rx_data[r+3]==':')
			{
				for(int ii=5+r;ii<1590;ii++)
				{
					if(rx_data[ii]=='\n')
					{
						con_len=ii-r-5;

						break;
					}
					else
					{
						recive_packet_init[ii-r-5]=rx_data[ii];
					}
				}
			}
			if(rx_data[r]=='p'&&rx_data[r+1]=='u'&&rx_data[r+2]=='b'&&rx_data[r+3]==':')
			{
				for(int ii=5+r;ii<1590;ii++)
				{
					if(rx_data[ii]=='\n')
					{
						pub_len=ii-r-4;
						memset(rx_data, 9, 1590);
						break;
					}
					else
					{
						recive_packet_send[ii-r-5]=rx_data[ii];
					}

				}
			}
		}

		break;
	}
	if(pub_len>1010||pub_len<18)
	{
		pub_len=0;
	}
	if(sub_len>100||sub_len<16)
	{
		sub_len=0;
	}
	if(con_len>70||con_len<35)
	{
		con_len=0;
	}

	//	if(HAL_GetTick()%10000==0)
	//	{
	//		mqtt_message=Mqtt_Recive();
	//	}
}
//string serv;
//string user;
//string pass;
void log_uart(char* data)
{
	//	HAL_UART_Transmit(&huart2, (uint8_t*)"LOG:", 4,50);
	//	HAL_UART_Transmit(&huart2, (uint8_t*)data, 50,200);
	//	HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2,50);
}
bool init_ready=false;
bool pub_ready=false;
//string topic;
//string mess;
//void modem_init()
//{
//
//	int j=0;
//	while(true)
//	{
//		if(rx_data[i+j+5]!=','&&j<100&&rx_data[i+j+5]!='\t'&&rx_data[i+j+5]!='\n'&&rx_data[i+j+5]!='\r')
//		{
//			topic.push_back(rx_data[i+j+5]);
//		}
//		else
//		{
//			break;
//		}
//		j++;
//	}
//	while(true)
//	{
//		if(rx_data[i+j+6]!=','&&j<200&&rx_data[i+j+6]!='\t'&&rx_data[i+j+6]!='\n'&&rx_data[i+j+6]!='\r')
//		{
//			mess.push_back(rx_data[i+j+6]);
//		}
//		else
//		{
//			break;
//		}
//		j++;
//	}

//	pub_ready=true;
//	init_ready=true;


//	//return 0;
//}
//void modem_send()
//{
//	string topic;
//	int j=0;
//
//
//	while(true)
//	{
//		if(rx_data[i+j+6]!='\n')
//		{
//			topic.push_back(rx_data[i+j+6]);
//		}
//		else
//		{
//			break;
//		}
//		j++;
//	}
//
//
//
//	//Mqtt_Subscribe((char*)topic.c_str());
//}
//void modem_sub()
//{
//	string topic;
//	int j=0;
//
//
//	while(true)
//	{
//		if(rx_data[i+j+6]!='\n')
//		{
//			topic.push_back(rx_data[i+j+6]);
//		}
//		else
//		{
//			break;
//		}
//		j++;
//	}
//
//
//
//	//Mqtt_Subscribe((char*)topic.c_str());
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
bool start=false;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//char data[512];
	uint32_t last=0;
	uint32_t last_pub=0;
	//int count12=0;
	//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	//	HAL_Delay(500);
	//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

	HAL_Delay(7000);
	//	while(1)
	//	{
	//		modem_work();
	//	}

	uint32_t ResDelay=0;
	uint32_t subDelay=0;
	while (1)
	{
		//modem_status();
		//		if(HAL_GetTick()-last>=5000)
		if(!modem_flag&&HAL_GetTick()-last>=5000)
		{
			modem_work();
			last=HAL_GetTick();
		}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

		if(modem_flag)
		{
			//	CSQ_status();
		
			if(init_flag==0)
			{
				char timestr[100];

				

				if(con_len>0&&pub_len>0)
				{
					//CSQ_status();

					Mqtt_Init();
					//						topic.clear();
					//						mess.clear();
					//				
				}

				if(pub)
				{
					if(!start)
					{
	//					unixtime=GetTimeNTP();
	//					sprintf(timestr,"time:%u\n\r",unixtime);
	//					HAL_UART_Transmit(&huart2, (uint8_t*)timestr, 100, 300);
						for(int i=0;i<5;i++)
						{
//							Get_IMEI();
//							Get_Network_Info();
//							CSQ_status();
//							Get_IMEI();
						}
						start=true;
					}
					for(int i=0;i<3;i++)
					{
						Mqtt_Recive();
						if(!pub)
						{
							break;
						}
					}
				}

			}

			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);

		}
		modem_wait();


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  huart2.Init.BaudRate = 460800;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5;
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
