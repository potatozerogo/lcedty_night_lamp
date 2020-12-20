#include "key.h"
#include "main.h"	

uint8_t Key_State = 0;
uint8_t Key_Timer = 0;

uint8_t Read_Key(void)	
{
	uint8_t key_Value = 0;
	switch(Key_State) 
	{
		case 0:
			if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin) == 0 )
				Key_State++;
			break;
		case 1:
			if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin) == 0 )
			{
				Key_State++;
				key_Value = 1;
			}
			else
			{
				Key_State = 0;
			}
			break;
		case 2:
			if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin) == 0 )
			{
				if(++Key_Timer == 100)
				{
					key_Value = 2;
					Key_Timer = 0;
				}
			}
			else
			{
				Key_Timer = 0;
				Key_State = 0;
			}
			break;	
	}
	return (key_Value);
}	

