/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "cJSON.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
uint8_t my_re_buf[2000];
uint16_t pt=0;
uint8_t aRxBuffer2;			//接收中断缓冲2
char RST_Start[]="AT+RST\r\n";
char MODE_Start[]="AT+CWMODE=1\r\n";
char WIFI_Start[]="AT+CWJAP=\"HUAWEI-3AHFVD\",\"13550695909\"\r\n";
char Connected_Start[]="AT+CIPMUX=0\r\n";
char TCP_Start[]="AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n";
char TX_Start[]="AT+CIPMODE=1\r\n";
char Data[]="AT+CIPSEND\r\n";
char TX_Data[]="GET https://api.seniverse.com/v3/weather/now.json?key=SG_ckZ26xPfp8E2EK&location=chengdu&language=zh-Hans&unit=c\r\n";
char Recive[5];
char Recive2[5];
char Commend2[]="page0.t4.txt=\"0\"";

#define RXBUFFERSIZE 256
char RxBuffer[RXBUFFERSIZE];
uint8_t aRxBuffer;			//接收中断缓冲
uint8_t Uart2_Rx_Cnt = 0;		//接收缓冲计数
extern osSemaphoreId myBinarySem01Handle;

cJSON *json;
cJSON *Array_obj1;
cJSON *Array_obj2;
cJSON *Array_obj3;
cJSON *Array_obj4;
cJSON *Array;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void HMISends(char *buf1)		  //字符串发送函数
{
	uint8_t i=0;
	while(1)
	{
	 if(buf1[i]!=0)
	 	{
			HAL_UART_Transmit(&huart3, (uint8_t *)&buf1[i], 1,0xFFFF); //将收到的信息发送出去
			while(HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
			i++;
		}
	 else 
	 return ;

		}
}

void HMISendb(uint8_t k)		         //字节发送函数
{		 
	uint8_t i;
	 for(i=0;i<3;i++)
	 {
	 if(k!=0)
	 	{
			HAL_UART_Transmit(&huart3, (uint8_t *)&k, 1,0xFFFF); //将收到的信息发送出去
			while(HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
		}
	 else 
	 return ;

	 } 
} 

void HMISendstart(void)
{
	 	HAL_Delay(200);
		HMISendb(0xff);
		HAL_Delay(200);
}

void command(char Command_AT[])
{
	int Lenth=strlen(Command_AT);
	HAL_UART_Transmit(&huart2,(uint8_t*)Command_AT,Lenth,0xff);
	printf("%s",Command_AT);
	HAL_Delay(3500);
}

void FreeRTOS_command(char Command_AT[])
{
	int Lenth=strlen(Command_AT);
	HAL_UART_Transmit(&huart2,(uint8_t*)Command_AT,Lenth,0xff);
	printf("%s",Command_AT);
	osDelay(3500);
}

void FreeROTS_ESP8266_Init(void)
{
	FreeRTOS_command(MODE_Start);
	FreeRTOS_command(RST_Start);
	FreeRTOS_command(WIFI_Start);
	FreeRTOS_command(Connected_Start);
	FreeRTOS_command(TCP_Start);
	FreeRTOS_command(TX_Start);
	FreeRTOS_command(Data);
}

void ESP8266_Init(void)
{
	command(MODE_Start);
	command(RST_Start);
	command(WIFI_Start);
	command(Connected_Start);
	command(TCP_Start);
	command(TX_Start);
	command(Data);
}
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
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);
	HMISendstart();
	HMISends("page0.t0.txt=\"00：00：00\"");
	HMISendb(0xff);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
	
  /* USER CODE END 2 */
  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init(); 
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);

	if(huart==&huart2)
	{
		my_re_buf[pt++]=aRxBuffer2;
		
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
	}
	
	else if(huart==&huart3)
	{	
		RxBuffer[Uart2_Rx_Cnt++] = aRxBuffer;
		if((RxBuffer[Uart2_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart2_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart2_Rx_Cnt,0xFFFF); //将收到的信息发送出去
			while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
			printf("您所收到的数据是：");
			osSemaphoreRelease(myBinarySem01Handle);
		}
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
	}
}
/* USER CODE END 4 */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
