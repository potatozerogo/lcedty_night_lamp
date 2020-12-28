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
extern uint8_t Switch_Led_Status;//main.c
extern uint8_t Wb3s_Wifi_Status;//wifi?? main.c
extern unsigned int LED_V_Data,LED_S_Data,LED_H_Data;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId WB3SHandle;
osThreadId TH_SensorHandle;
osThreadId KeyHandle;
osThreadId SHAKE_PROCESSHandle;
osSemaphoreId SHAKEHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask_WB3S(void const * argument);
void StartTask_TH_Sensor(void const * argument);
void StartTask_Key(void const * argument);
void StartTask_SHAKE_PROCESS(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of SHAKE */
  osSemaphoreDef(SHAKE);
  SHAKEHandle = osSemaphoreCreate(osSemaphore(SHAKE), 1);

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
  osThreadDef(Key, StartTask_Key, osPriorityNormal, 0, 128);
  KeyHandle = osThreadCreate(osThread(Key), NULL);

  /* definition and creation of SHAKE_PROCESS */
  osThreadDef(SHAKE_PROCESS, StartTask_SHAKE_PROCESS, osPriorityNormal, 0, 64);
  SHAKE_PROCESSHandle = osThreadCreate(osThread(SHAKE_PROCESS), NULL);

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
	uint16_t Button_Hold_Timer = 0;
	uint8_t First_Start_flag = 1;
  for(;;)
  {
		if(First_Start_flag == 1)//上电长按按键3秒进入wifi重设状态
		{
			while(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin) == 0)
			{
				if(Button_Hold_Timer == 30)
					mcu_set_wifi_mode(SMART_CONFIG);
				if(Button_Hold_Timer > 30)
							HAL_GPIO_TogglePin(SYS_LED_GPIO_Port,SYS_LED_Pin);
				Button_Hold_Timer++;
				osDelay(100);	
			}
			First_Start_flag = 0;
		}
		HAL_GPIO_TogglePin(SYS_LED_GPIO_Port,SYS_LED_Pin);
		osDelay(1000);
//    switch(Wb3s_Wifi_Status) 
//		{
//				case 0:
//					HAL_GPIO_TogglePin(SYS_LED_GPIO_Port,SYS_LED_Pin);
//					osDelay(500);
//				break;
//				
//        case 1:
//					HAL_GPIO_TogglePin(NET_LED_GPIO_Port,NET_LED_Pin);
//					osDelay(100);
//        break;
//        
//        case 2:
//					HAL_GPIO_TogglePin(NET_LED_GPIO_Port,NET_LED_Pin);
//					osDelay(100);
//        break;
//        
//        case 3:
//					HAL_GPIO_WritePin(NET_LED_GPIO_Port,NET_LED_Pin,GPIO_PIN_SET);
//					osDelay(100);
//        break;
//        
//        case 4:
//					HAL_GPIO_WritePin(NET_LED_GPIO_Port,NET_LED_Pin,GPIO_PIN_SET);
//					osDelay(100);
//        break;
//        
//        case 5:
//					HAL_GPIO_TogglePin(NET_LED_GPIO_Port,NET_LED_Pin);
//					osDelay(1000);
//        break;
//      
//        case 6:
//					HAL_GPIO_TogglePin(NET_LED_GPIO_Port,NET_LED_Pin);
//					osDelay(5000);
//				break;
//				
//        case 7:
//					HAL_GPIO_TogglePin(NET_LED_GPIO_Port,NET_LED_Pin);
//					osDelay(100);
//				break;
//			}
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
	unsigned char R_Data,G_Data,B_Data;
	char Adjust_Bright_Direction = 0;	
	
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
				  if(Adjust_Bright_Direction == 1)
					{	
						LED_V_Data += 1;
						if(LED_V_Data >= 100)
							Adjust_Bright_Direction = 0;
					}
					else
					{	
						LED_V_Data -= 1;
						if(LED_V_Data <= 0)
							Adjust_Bright_Direction = 1;
					}
					HSVtoRGB(&R_Data,&G_Data,&B_Data,LED_H_Data,LED_S_Data,LED_V_Data);
					for(unsigned char i = 0; i < 8; i++)
					{
						Set_Color(R_Data,G_Data,B_Data,i);
					} 
					Send_Color();
					break;
			default:
					break;
		}
    osDelay(50);
  }
  /* USER CODE END StartTask_Key */
}

/* USER CODE BEGIN Header_StartTask_SHAKE_PROCESS */
/**
* @brief Function implementing the SHAKE_PROCESS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_SHAKE_PROCESS */
void StartTask_SHAKE_PROCESS(void const * argument)
{
  /* USER CODE BEGIN StartTask_SHAKE_PROCESS */
  /* Infinite loop */
	unsigned char R_Data,G_Data,B_Data;
  for(;;)
  {
		osSemaphoreWait(SHAKEHandle,osWaitForever);
		LED_H_Data += 60; 
		if(LED_H_Data >= 360) 
		{
				LED_H_Data = 0;
				LED_S_Data = 0;
		}
		else
		{
			if(LED_S_Data == 0)
			{	
				LED_H_Data = 0;
				LED_S_Data = 100;
			}
		}
		HSVtoRGB(&R_Data,&G_Data,&B_Data,LED_H_Data,LED_S_Data,LED_V_Data);
		for(unsigned char i = 0; i < 8; i++)
		{
			Set_Color(R_Data,G_Data,B_Data,i);
		} 
		Send_Color();

  }
  /* USER CODE END StartTask_SHAKE_PROCESS */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
