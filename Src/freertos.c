/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "rtc.h"
#include "usart.h"
#include "string.h"
#include "cJSON.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#define RXBUFFERSIZE 256
RTC_DateTypeDef GetData;  //获取日期结构体

RTC_TimeTypeDef GetTime;   //获取时间结构体
extern char RxBuffer[RXBUFFERSIZE];
extern uint8_t aRxBuffer;			//接收中断缓冲
extern uint8_t Uart2_Rx_Cnt;		//接收缓冲计数
char Commend[]="page0.t0.txt=\"00：00：00\"";
extern char Commend2[];

extern cJSON *json;
extern cJSON *Array_obj1;
extern cJSON *Array_obj2;
extern cJSON *Array_obj3;
extern cJSON *Array_obj4;
extern cJSON *Array;
extern uint8_t my_re_buf[];
extern uint16_t pt;
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Task_usartHandle;
osThreadId Task_hc05Handle;
osThreadId Task_ledHandle;
osSemaphoreId myBinarySem01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartTask01(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
extern char TX_Data[];
extern char Data[];
extern char Recive[5];
extern char Recive2[5];
extern uint8_t aRxBuffer2;			//接收中断缓冲2

extern void HMISends(char *buf1);		  //字符串发送函数
extern void HMISendb(uint8_t k);		         //字节发送函数
extern void FreeRTOS_command(char Command_AT[]);
extern void FreeROTS_ESP8266_Init(void);

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

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
  /* definition and creation of Task_usart */
  osThreadDef(Task_usart, StartTask01, osPriorityNormal, 0, 1500);
  Task_usartHandle = osThreadCreate(osThread(Task_usart), NULL);

  /* definition and creation of Task_hc05 */
  osThreadDef(Task_hc05, StartTask02, osPriorityIdle, 0, 128);
  Task_hc05Handle = osThreadCreate(osThread(Task_hc05), NULL);

  /* definition and creation of Task_led */
  osThreadDef(Task_led, StartTask03, osPriorityIdle, 0, 128);
  Task_ledHandle = osThreadCreate(osThread(Task_led), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTask01 */
/**
  * @brief  Function implementing the Task_usart thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartTask01 */
void StartTask01(void const * argument)
{
  /* USER CODE BEGIN StartTask01 */
  /* Infinite loop */
  for(;;)
  {
		cJSON_Hooks hooks;
		hooks.malloc_fn = pvPortMalloc;
		hooks.free_fn = vPortFree;
		cJSON_InitHooks(&hooks); 
		osDelay(3500);
		FreeROTS_ESP8266_Init();

		FreeRTOS_command(TX_Data);		//发送指令
		osDelay(10000);
		
		json = cJSON_Parse((const char *) my_re_buf );
		Array_obj1=cJSON_GetObjectItem(json,"results");
		
		int size=cJSON_GetArraySize(Array_obj1);
		printf("\n%d",size);
		
		if(json == NULL)
			printf("\njson fmt error:%s\n.", cJSON_GetErrorPtr());
		else
		{
			Array=cJSON_GetArrayItem(Array_obj1,0);
			Array_obj2=cJSON_GetObjectItem(Array,"now");
			Array_obj3=cJSON_GetObjectItem(Array_obj2,"code");
			osDelay(1000);
			Array_obj4=cJSON_GetObjectItem(Array_obj2,"temperature");
			printf("n\r%s\n\r",Array_obj3->valuestring);
			printf("n\r%s\n\r",Array_obj4->valuestring);
			strcpy(Recive,Array_obj3->valuestring);
			strcpy(Recive2,Array_obj4->valuestring);
			
			sprintf(Commend2,"page0.t4.txt=\"%s\"",Recive2);
			HMISends(Commend2);
			HMISendb(0xff);
			memset(Recive2,0x00,sizeof(Recive2)); //清空数组
			
			if(Recive[0]==1&&(Recive[1]=='3'||Recive[1]=='4'||Recive[1]=='5'))
			{
				printf("\n天气：雨\n");
				HMISends("page0.t1.txt=\"雨\"");
				HMISendb(0xff);	
			}
			
			else if(Recive[0]=='0'||Recive[0]=='1')
			{	
				printf("\n天气：晴\n");
				HMISends("page0.t1.txt=\"晴\"");
				HMISendb(0xff);
			}
			else if(Recive[0]=='4'||Recive[0]=='5'||Recive[0]=='6')
			{	
				printf("\n天气：多云\n");
				HMISends("page0.t1.txt=\"多云\"");
				HMISendb(0xff);
			}
			else if(Recive[0]=='9')
			{
				printf("\n天气：阴\n");
				HMISends("page0.t1.txt=\"阴\"");
				HMISendb(0xff);
			}
			Recive[0]='x';
			Recive[1]='x';
		}
		
		memset(my_re_buf,0x00,2000);
		pt=0;

//		cJSON_free(my_re_buf);
		cJSON_Delete(json);

		osDelay(3500);
  }
  /* USER CODE END StartTask01 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task_hc05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);
		HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
	  HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
		sprintf(Commend,"page0.t0.txt=\"%02d：%02d：%02d\"",GetTime.Hours,GetTime.Minutes,GetTime.Seconds);
		HMISends(Commend);
		HMISendb(0xff);
		osDelay(1000);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task_led thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(myBinarySem01Handle,osWaitForever);
		HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart2_Rx_Cnt,0xFFFF); //将收到的信息发送出去
		while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
		Uart2_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
