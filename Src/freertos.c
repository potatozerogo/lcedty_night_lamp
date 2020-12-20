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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "wifi.h"
#include "ws2812.h"
#include "sht2x_i2c.h"
#include "key.h"
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

int16_t Test_T_Data,Test_H_Data; 
extern uint8_t Switch_Led_Status;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId WB3SHandle;
osThreadId TH_SensorHandle;
osThreadId KeyHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask_WB3S(void const * argument);
void StartTask_TH_Sensor(void const * argument);
void StartTask_Key(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of WB3S */
  osThreadDef(WB3S, StartTask_WB3S, osPriorityIdle, 0, 128);
  WB3SHandle = osThreadCreate(osThread(WB3S), NULL);

  /* definition and creation of TH_Sensor */
  osThreadDef(TH_Sensor, StartTask_TH_Sensor, osPriorityNormal, 0, 128);
  TH_SensorHandle = osThreadCreate(osThread(TH_Sensor), NULL);

  /* definition and creation of Key */
  osThreadDef(Key, StartTask_Key, osPriorityIdle, 0, 128);
  KeyHandle = osThreadCreate(osThread(Key), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(SYS_LED_GPIO_Port,SYS_LED_Pin);
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask_WB3S */
/**
* @brief Function implementing the WB3S thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_WB3S */
void StartTask_WB3S(void const * argument)
{
  /* USER CODE BEGIN StartTask_WB3S */
  /* Infinite loop */
  for(;;)
  {
		wifi_uart_service();
  }
  /* USER CODE END StartTask_WB3S */
}

/* USER CODE BEGIN Header_StartTask_TH_Sensor */
/**
* @brief Function implementing the TH_Sensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_TH_Sensor */
void StartTask_TH_Sensor(void const * argument)
{
  /* USER CODE BEGIN StartTask_TH_Sensor */
  /* Infinite loop */
  for(;;)
  {
		
//		Test_T_Data = Get_SHT2x_Temp();
//		Test_H_Data = Get_SHT2x_Rh();
    mcu_dp_value_update(DPID_TEMPERATURE_DATA,Get_SHT2x_Temp()/10); //VALUE?????;
    mcu_dp_value_update(DPID_HUMIDITY_DATA,Get_SHT2x_Rh()/10); //VALUE?????;
    osDelay(120000);
  }
  /* USER CODE END StartTask_TH_Sensor */
}

/* USER CODE BEGIN Header_StartTask_Key */
/**
* @brief Function implementing the Key thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Key */
void StartTask_Key(void const * argument)
{
  /* USER CODE BEGIN StartTask_Key */
  /* Infinite loop */
  for(;;)
  {
		switch(Read_Key())
		{
			case 1:
					if(Switch_Led_Status == 1) 
					{
							Switch_Led_Status = 0;
						  mcu_dp_bool_update(DPID_SWITCH_LED,0);//上传开关状态
							Clear_Color();	
					}
					else 
					{
						  Switch_Led_Status = 1;
						  mcu_dp_bool_update(DPID_SWITCH_LED,1);//上传开关状态
							Send_Color();
					}
					break;
			case 2:
					mcu_set_wifi_mode(SMART_CONFIG);//长按5秒，重新设置网络
					break;
			default:
					break;
		}
    osDelay(50);
  }
  /* USER CODE END StartTask_Key */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
