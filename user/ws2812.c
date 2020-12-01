#include "ws2812.h"

uint8_t Led_Color[LED_NUM][3];
uint8_t Ws2812_Buff[WS2812_BUFF_SIZE];

//*************关闭LED
void Clear_Color(void)
{
	uint32_t c;
	uint32_t l;
	for(l=0;l<LED_NUM;l++)
	{
		for(c=0;c<3;c++)
		{
			Led_Color[l][c] = 0;
		}
	}
	Send_Color();
}

//*************设置某点颜色
void Set_Color(uint8_t r,uint8_t g,uint8_t b,uint8_t num)
{
	if(num > LED_NUM || num == 0)
		return;
	Led_Color[num-1][0] = g;
	Led_Color[num-1][1] = r;
	Led_Color[num-1][2] = b;
}

//*************发送指令到WS2812
void Send_Color(void)
{
	uint32_t i;
	uint32_t c;
	uint32_t k;	
	uint32_t l;	

	k = 0;
	for(l=0;l<LED_NUM;l++)
	{
		for(c=0;c<3;c++)
		{
			for(i=0;i<8;i++)
			{
				if(Led_Color[l][c] & 1<<(7-i))
					Ws2812_Buff[k] = 0xF8;
				else
					Ws2812_Buff[k] = 0xE0;
				k++;
			}
		}	
	}
	
	HAL_SPI_Transmit_DMA(&hspi1,Ws2812_Buff,WS2812_BUFF_SIZE);
//	DMA_Cmd (SPI1_TX_DMA_CHANNEL,DISABLE);
//	SPIx_DMA_Config();
////	SPI_FLASH_CS_LOW();
////	Delay(100);
//	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
}


