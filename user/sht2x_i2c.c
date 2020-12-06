/*
SHT2x ��д
STM32 HAL_I2C
FreeRTos
*/

#include "sht2x_i2c.h"
#include "bsp_i2c_gpio.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"

void chip_temperature(void);
////===========������UART===============
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
* @param *data У������ָ��
* len ���ݸ���
* @retval CRC8���
*/
#define INIT_CRC8 0x00 //��ʼֵ
#define POLY_CRC8 0x31 //����ʽ
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
* @brief �����λSHT2x
* @param 
* 
* @retval 
*/
void SHT2x_SoftReset(void)
{
	uint8_t Sht2x_Command;
	Sht2x_Command = SOFT_RESET;
//	HAL_I2C_Master_Transmit(&hi2c1,0x80,&Sht2x_Command,1,100);
	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(0x80);	/* �˴���дָ�� */
	 
	/* ��3�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4��������Command */
	i2c_SendByte(Sht2x_Command);
	
	/* ��5�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
//	return 1;	/* ִ�гɹ� */

//cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
//	/* ����I2C����ֹͣ�ź� */
//	i2c_Stop();
//	return 0;
}

/**
* @brief ��ʼ��SHT2x
* @param 
* 
* @retval 
*/
void SHT2x_Init(void)
{
	SHT2x_SoftReset();
}


/**
* @brief ��SHT2x����
* @param ��� TRIG_TEMP_MEASUREMENT_POLL 0xF3 //command trig. temp meas. no hold master
				TRIG_HUMI_MEASUREMENT_POLL 0xF5 //command trig. humidity meas. no hold master
* 
* @retval ���ض���������,����0xffff��ʾ��ȡʧ��
*/
uint16_t Read_SHT2x_Data(uint8_t command)
{
//	HAL_StatusTypeDef Ack_Flag;
	uint8_t Ack_Flag;
	uint8_t Time_Limit = 0;
	uint8_t Receive_Buff[3];
	
//	HAL_I2C_Master_Transmit(&hi2c1,0x80,&command,1,0x10);
	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(0x80);	/* �˴���дָ�� */
	 
	/* ��3�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4��������Command */
	i2c_SendByte(command);
	
	/* ��5�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
//		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
//	return 1;	/* ִ�гɹ� */

//cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
//	/* ����I2C����ֹͣ�ź� */
//	i2c_Stop();
//	return 0;
	  
//	Ack_Flag = HAL_BUSY;
	Ack_Flag = 0;
	Time_Limit = 0;
	while(Ack_Flag == 0)
	{
		osDelay(10);
	/* ��1��������I2C���������ź� */
		i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
		i2c_SendByte(0x81);	/* �˴���дָ�� */
	 
	/* ��3�����ȴ�ACK */
		if (i2c_WaitAck() == 0)
		{
			Ack_Flag = 1;	/* AckӦ�� */
			Receive_Buff[0] = i2c_ReadByte();
			i2c_Ack();
			Receive_Buff[1] = i2c_ReadByte();
			i2c_Ack();	
			Receive_Buff[2] = i2c_ReadByte();
			i2c_NAck();	
		}
		
		/* ����I2C����ֹͣ�ź� */		
		i2c_Stop();
		
		if(++Time_Limit >= 10)
		{
			i2c_Stop();
			return (0xffff);//��ʱ����	
		}
	}

	

	if(Receive_Buff[2] == Get_CRC8(Receive_Buff,2))
		return ((Receive_Buff[0] << 8) | Receive_Buff[1]);
	else
		return (0xffff);//crcУ����󷵻�



}


/**
* @brief ���¶�ֵ
* @param 
* 
* @retval �¶�ֵ����λ0.1���϶ȣ�����-999��ʾ��ȡʧ��
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
* @brief ��ʪ��ֵ
* @param 
* 
* @retval ʪ��ֵ����λ0.1 %������-999��ʾ��ȡʧ��
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
	//��ʱ�����³�ʼ��I2C,����Ӳ����ɳ�ʼ��ʧ��
//	HAL_I2C_DeInit(&hi2c1);//�ͷ�IO��ΪGPIO����λ���״̬��־
//	MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //������³�ʼ��I2C������
	osDelay(20);
	SHT2x_Init(); 
	osDelay(20);
	while(1)
	{
		//��ȡ�¶�
		Read_Data = Get_SHT2x_Temp();
		if(Read_Data != -999)
		{
			Read_Temperature_Fail_Counter = 0;
			Actual_Temperature_Data = Read_Data;
//			sprintf((char *)Tx_uart1_buff,"��ǰ�¶�:%d.%d�ȣ�",Read_Data/10,Read_Data%10);//=========���ڵ���============
//			HAL_UART_Transmit(&huart1,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========���ڵ���============
		}
		else
		{
//			sprintf((char *)Tx_uart2_buff,"��ǰ�¶ȶ�ȡ����");//=========���ڵ���============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========���ڵ���============
			Read_Temperature_Fail_Counter++;
			if(Read_Temperature_Fail_Counter >= 10)//���������ȡ�����ۼ�ֵ10�Σ���λI2C�͸�λSHT2x
			{
				Read_Temperature_Fail_Counter = 0;
//				HAL_I2C_DeInit(&hi2c1);        //�ͷ�IO��ΪGPIO����λ���״̬��־
//				MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //������³�ʼ��I2C������
				SHT2x_Init();	
			}
		}
		
		//��ȡʪ��
		Read_Data = Get_SHT2x_Rh();
		if(Read_Data != -999)
		{
			Read_Humidity_Fail_Counter = 0;
			Actual_Humidity_Data = Read_Data;
//			sprintf((char *)Tx_uart2_buff,"ʪ��:%d.%d%%.\r\n",Read_Data/10,Read_Data%10);//=========���ڵ���============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========���ڵ���============
		}
		else
		{
//			sprintf((char *)Tx_uart2_buff,"��ǰʪ�ȶ�ȡ����\r\n");//=========���ڵ���============
//			HAL_UART_Transmit(&huart2,Tx_uart2_buff,strlen((char *)Tx_uart2_buff),10000);//=========���ڵ���============
			Read_Humidity_Fail_Counter++;
			if(Read_Humidity_Fail_Counter >= 10)//���������ȡ�����ۼ�ֵ10�Σ���λI2C�͸�λSHT2x
			{
				Read_Humidity_Fail_Counter = 0;
//				HAL_I2C_DeInit(&hi2c1);        //�ͷ�IO��ΪGPIO����λ���״̬��־
//				MX_I2C1_Init();//HAL_I2C_Init(&hi2c1);          //������³�ʼ��I2C������
				SHT2x_Init();	
			}
		}			
		
		
		osDelay(10000);

	}
}

