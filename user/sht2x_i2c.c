/*
SHT2x 读写
STM32 HAL_I2C
FreeRTos
*/

#include "sht2x_i2c.h"
#include "bsp_i2c_gpio.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"

void chip_temperature(void);
////===========调试用UART===============
//uint8_t String_Statr[] = "=====T&H Sensor=====\r\n";
//uint8_t Rx_uart1_buff[32];
//uint8_t Tx_uart1_buff[32];
//extern uint8_t Rx_uart2_buff[32];
extern uint8_t Tx_uart2_buff[32];
////uint8_t UART1_Transmit_Flag = 0;
//extern uint8_t UART2_Transmit_Flag;
//extern uint8_t pp;
////===========UART===============

int16_t Actual_Temperature_Data; 
int16_t Actual_Humidity_Data;


/**
* @brief CRC8
* @param *data 校验数据指针
* len 数据个数
* @retval CRC8结果
*/
#define INIT_CRC8 0x00 //初始值
#define POLY_CRC8 0x31 //多项式
uint8_t Get_CRC8(uint8_t *data, uint16_t len)
{
     const uint8_t POLYNOMIAL = POLY_CRC8;
     uint8_t crc = INIT_CRC8;
     int i, j;
   
	 for (i=0; i<len; ++i) 
	 {
     	crc ^= *data++;
 
     	for (j=0; j<8; ++j) 
		{
        	crc = ( crc & 0x80 )? (crc << 1) ^ POLYNOMIAL: (crc << 1);
       	}
  	 }
	 return (crc);
}

/**
* @brief 软件复位SHT2x
* @param 
* 
* @retval 
*/
void SHT2x_SoftReset(void)
{
	uint8_t Sht2x_Command;
	Sht2x_Command = SOFT_RESET;
//	HAL_I2C_Master_Transmit(&hi2c1,0x80,&Sht2x_Command,1,100);
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(0x80);	/* 此处是写指令 */
	 
	/* 第3步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送Command */
	i2c_SendByte(Sht2x_Command);
	
	/* 第5步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 发送I2C总线停止信号 */
	i2c_Stop();
//	return 1;	/* 执行成功 */

//cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
//	/* 发送I2C总线停止信号 */
//	i2c_Stop();
//	return 0;
}

/**
* @brief 初始化SHT2x
* @param 
* 
* @retval 
*/
void SHT2x_Init(void)
{
	SHT2x_SoftReset();
}


/**
* @brief 读SHT2x数据
* @param 命令： TRIG_TEMP_MEASUREMENT_POLL 0xF3 //command trig. temp meas. no hold master
				TRIG_HUMI_MEASUREMENT_POLL 0xF5 //command trig. humidity meas. no hold master
* 
* @retval 返回读到的数据,返回0xffff表示读取失败
*/
uint16_t Read_SHT2x_Data(uint8_t command)
{
//	HAL_StatusTypeDef Ack_Flag;
	uint8_t Ack_Flag;
	uint8_t Time_Limit = 0;
	uint8_t Receive_Buff[3];
	
//	HAL_I2C_Master_Transmit(&hi2c1,0x80,&command,1,0x10);
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(0x80);	/* 此处是写指令 */
	 
	/* 第3步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送Command */
	i2c_SendByte(command);
	
	/* 第5步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 发送I2C总线停止信号 */
	i2c_Stop();
//	return 1;	/* 执行成功 */

//cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
//	/* 发送I2C总线停止信号 */
//	i2c_Stop();
//	return 0;
	  
//	Ack_Flag = HAL_BUSY;
	Ack_Flag = 0;
	Time_Limit = 0;
	while(Ack_Flag == 0)
	{
		osDelay(10);
	/* 第1步：发起I2C总线启动信号 */
		i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
		i2c_SendByte(0x81);	/* 此处是写指令 */
	 
	/* 第3步：等待ACK */
		if (i2c_WaitAck() == 0)
		{
			Ack_Flag = 1;	/* Ack应答 */
			Receive_Buff[0] = i2c_ReadByte();
			i2c_Ack();
			Receive_Buff[1] = i2c_ReadByte();
			i2c_Ack();	
			Receive_Buff[2] = i2c_ReadByte();
			i2c_NAck();	
		}
		
		/* 发送I2C总线停止信号 */		
		i2c_Stop();
		
		if(++Time_Limit >= 10)
		{
			i2c_Stop();
			return (0xffff);//超时返回	
		}
	}

	

	if(Receive_Buff[2] == Get_CRC8(Receive_Buff,2))
		return ((Receive_Buff[0] << 8) | Receive_Buff[1]);
	else
		return (0xffff);//crc校验错误返回



}


/**
* @brief 读温度值
* @param 
* 
* @retval 温度值，单位0.1摄氏度，返回-999表示读取失败
*/
int16_t Get_SHT2x_Temp(void)
{
    uint16_t SHT2x_Temp_Data;
	int32_t Temp;
	
    SHT2x_Temp_Data = Read_SHT2x_Data(TRIG_TEMP_MEASUREMENT_POLL);
	if(SHT2x_Temp_Data == 0xffff)
		return (-999);
    Temp = SHT2x_Temp_Data & ~0x0003;
	Temp = Temp * 1757;
	Temp = Temp >> 16;
	Temp = Temp - 468;
    return ((int16_t) Temp);
}


/**
* @brief 读湿度值
* @param 
* 
* @retval 湿度值，单位0.1 %，返回-999表示读取失败
*/
int16_t Get_SHT2x_Rh(void)
{
    uint16_t SHT2x_Rh_Data;
	int32_t Rh;
	
    SHT2x_Rh_Data = Read_SHT2x_Data(TRIG_HUMI_MEASUREMENT_POLL);
	if(SHT2x_Rh_Data == 0xffff)
		return (-999);
    Rh = SHT2x_Rh_Data & ~0x000f;
	Rh = Rh * 1250;
	Rh = Rh >> 16;
	Rh = Rh - 60;
    return ((int16_t) Rh);
}



void Task_TH_Sensor(void)
{
	int16_t  Read_Data;
	uint8_t Read_Temperature_Fail_Counter = 0;
	uint8_t Read_Humidity_Fail_Counter = 0;
	i2c_CfgGpio();
	//延时后重新初始化I2C,避免硬件造成初始化失败
//	HAL_I2C_DeInit(&hi2c1);//释放IO口为GPIO，复位句柄状态标志
//	MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //这句重新初始化I2C控制器
	osDelay(20);
	SHT2x_Init(); 
	osDelay(20);
	while(1)
	{
		//读取温度
		Read_Data = Get_SHT2x_Temp();
		if(Read_Data != -999)
		{
			Read_Temperature_Fail_Counter = 0;
			Actual_Temperature_Data = Read_Data;
//			sprintf((char *)Tx_uart1_buff,"当前温度:%d.%d度，",Read_Data/10,Read_Data%10);//=========串口调试============
//			HAL_UART_Transmit(&huart1,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========串口调试============
		}
		else
		{
//			sprintf((char *)Tx_uart2_buff,"当前温度读取错误，");//=========串口调试============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========串口调试============
			Read_Temperature_Fail_Counter++;
			if(Read_Temperature_Fail_Counter >= 10)//如果连续读取错误累加值10次，复位I2C和复位SHT2x
			{
				Read_Temperature_Fail_Counter = 0;
//				HAL_I2C_DeInit(&hi2c1);        //释放IO口为GPIO，复位句柄状态标志
//				MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //这句重新初始化I2C控制器
				SHT2x_Init();	
			}
		}
		
		//读取湿度
		Read_Data = Get_SHT2x_Rh();
		if(Read_Data != -999)
		{
			Read_Humidity_Fail_Counter = 0;
			Actual_Humidity_Data = Read_Data;
//			sprintf((char *)Tx_uart2_buff,"湿度:%d.%d%%.\r\n",Read_Data/10,Read_Data%10);//=========串口调试============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========串口调试============
		}
		else
		{
//			sprintf((char *)Tx_uart2_buff,"当前湿度读取错误。\r\n");//=========串口调试============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========串口调试============
			Read_Humidity_Fail_Counter++;
			if(Read_Humidity_Fail_Counter >= 10)//如果连续读取错误累加值10次，复位I2C和复位SHT2x
			{
				Read_Humidity_Fail_Counter = 0;
//				HAL_I2C_DeInit(&hi2c1);        //释放IO口为GPIO，复位句柄状态标志
//				MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //这句重新初始化I2C控制器
				SHT2x_Init();	
			}
		}			
		
		
		osDelay(10000);

	}
}

