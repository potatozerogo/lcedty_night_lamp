#ifndef __SHT2x_I2C_H
#define __SHT2x_I2C_H

#include "stm32f1xx_hal.h"

#define    TRIG_TEMP_MEASUREMENT_HM   0xE3 //command trig. temp meas. hold master
#define    TRIG_HUMI_MEASUREMENT_HM   0xE5 //command trig. humidity meas. hold master
#define    TRIG_TEMP_MEASUREMENT_POLL 0xF3 //command trig. temp meas. no hold master
#define    TRIG_HUMI_MEASUREMENT_POLL 0xF5 //command trig. humidity meas. no hold master
#define    USER_REG_W                 0xE6 //command writing user register
#define    USER_REG_R                 0xE7 //command reading user register
#define    SOFT_RESET                 0xFE  //command soft reset


void SHT2x_Init(void);
void SHT2x_SoftReset(void);
int16_t Get_SHT2x_Temp(void);
int16_t Get_SHT2x_Rh(void);
void Task_TH_Sensor(void);

#endif //#endif __SHT2x_I2C_H

