#include "ws2812.h"

uint8_t Led_Color[LED_NUM][3];
uint8_t Ws2812_Buff[WS2812_BUFF_SIZE];

//*************关闭LED
void Clear_Color(void)
{
	for(unsigned int i = 0;i < WS2812_BUFF_SIZE; i++)
	{
			Ws2812_Buff[i] = 0x60;//Ws2812_Buff[k] = 0xE0;
	}
	HAL_SPI_Transmit_DMA(&hspi1,Ws2812_Buff,WS2812_BUFF_SIZE);
}

//*************设置某点颜色
void Set_Color(uint8_t r,uint8_t g,uint8_t b,uint8_t num)
{
	if(num >= LED_NUM)
		return;
	Led_Color[num][0] = g;
	Led_Color[num][1] = r;
	Led_Color[num][2] = b;
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
					Ws2812_Buff[k] = 0xF0;//Ws2812_Buff[k] = 0xF8;
				else
					Ws2812_Buff[k] = 0x60;//Ws2812_Buff[k] = 0xE0;
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

void HSVtoRGB(uint8_t *r, uint8_t *g, uint8_t *b, uint16_t h, uint16_t s, uint16_t v)
{
	// R,G,B from 0-255, H from 0-360, S,V from 0-100
	int i;
	float RGB_min, RGB_max;
	RGB_max = v*2.55f;
	RGB_min = RGB_max*(100 - s) / 100.0f;

	i = h / 60;
	int difs = h % 60; // factorial part of h

					   // RGB adjustment amount by hue 
	float RGB_Adj = (RGB_max - RGB_min)*difs / 60.0f;

	switch (i) {
	case 0:
		*r = RGB_max;
		*g = RGB_min + RGB_Adj;
		*b = RGB_min;
		break;
	case 1:
		*r = RGB_max - RGB_Adj;
		*g = RGB_max;
		*b = RGB_min;
		break;
	case 2:
		*r = RGB_min;
		*g = RGB_max;
		*b = RGB_min + RGB_Adj;
		break;
	case 3:
		*r = RGB_min;
		*g = RGB_max - RGB_Adj;
		*b = RGB_max;
		break;
	case 4:
		*r = RGB_min + RGB_Adj;
		*g = RGB_min;
		*b = RGB_max;
		break;
	default:		// case 5:
		*r = RGB_max;
		*g = RGB_min;
		*b = RGB_max - RGB_Adj;
		break;
	}
}


unsigned int FourBitsStringBCD_TO_Int(unsigned char *string)
{
	unsigned int Data = 0;
	for(unsigned char i = 0; i < 4; i++)
	{
		switch(string[i])
		{
			case '0':
				break;
			case '1':
				Data += 1 * 0x1000>>(4*i);
				break;
			case '2':
				Data += 2 * 0x1000>>(4*i);
				break;
			case '3':
				Data += 3 * 0x1000>>(4*i);
				break;	
			case '4':
				Data += 4 * 0x1000>>(4*i);
				break;
			case '5':
				Data += 5 * 0x1000>>(4*i);
				break;
			case '6':
				Data += 6 * 0x1000>>(4*i);
				break;
			case '7':
				Data += 7 * 0x1000>>(4*i);
				break;
			case '8':
				Data += 8 * 0x1000>>(4*i);
				break;
			case '9':
				Data += 9 * 0x1000>>(4*i);
				break;
			case 'A':
				Data += 10 * 0x1000>>(4*i);
				break;
			case 'B':
				Data += 11 * 0x1000>>(4*i);
				break;
			case 'C':
				Data += 12 * 0x1000>>(4*i);
				break;
			case 'D':
				Data += 13 * 0x1000>>(4*i);
				break;
			case 'E':
				Data += 14 * 0x1000>>(4*i);
				break;
			case 'F':
				Data += 14 * 0x1000>>(4*i);
				break;
			default:
				break;
		}
	}
	return Data;
}

