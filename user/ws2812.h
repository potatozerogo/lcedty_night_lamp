#ifndef __WS2812_H
#define	__WS2812_H


#define LED_NUM 8   //WS2812µÄÊýÁ¿
#define WS2812_BUFF_SIZE LED_NUM*24

#include "stm32f1xx_hal.h"
#include "spi.h"


void Clear_Color(void);
void Set_Color(uint8_t r,uint8_t g,uint8_t b,uint8_t num);
void Send_Color(void);

unsigned int FourBitsStringBCD_TO_Int(unsigned char *string);
void HSVtoRGB(uint8_t *r, uint8_t *g, uint8_t *b, uint16_t h, uint16_t s, uint16_t v);



#endif /* __WS2812_H */

