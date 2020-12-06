#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include <inttypes.h>


#define SHT2X_I2C_WR	0		/* д����bit */
#define SHT2X_I2C_RD	1		/* ������bit */


/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */

#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE() 

#define SHT2X_GPIO_PORT_I2C	GPIOB			/* GPIO�˿� */
#define SHT2X_I2C_SCL_PIN		GPIO_PIN_6			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define SHT2X_I2C_SDA_PIN		GPIO_PIN_7			/* ���ӵ�SDA�����ߵ�GPIO */


/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#if 0	/* �������룺 1 ѡ��GPIO�Ŀ⺯��ʵ��IO��д */
	#define SHT2X_I2C_SCL_1() digitalH(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SCL_PIN)		/* SCL = 1 */
	#define SHT2X_I2C_SCL_0()  digitalL(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SCL_PIN)		/* SCL = 0 */
	
	#define SHT2X_I2C_SDA_1() digitalH(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)		/* SDA = 1 */
	#define SHT2X_I2C_SDA_0()  digitalL(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)		/* SDA = 0 */
	
	#define SHT2X_I2C_SDA_READ()  GPIO_ReadInputDataBit(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)	/* ��SDA����״̬ */
#else	/* �����֧ѡ��ֱ�ӼĴ�������ʵ��IO��д */
    /*��ע�⣺����д������IAR��߼����Ż�ʱ���ᱻ�����������Ż� */
	#define SHT2X_I2C_SCL_1()  SHT2X_GPIO_PORT_I2C->BSRR = SHT2X_I2C_SCL_PIN				/* SCL = 1 */
	#define SHT2X_I2C_SCL_0()  SHT2X_GPIO_PORT_I2C->BRR = SHT2X_I2C_SCL_PIN				/* SCL = 0 */
	
	#define SHT2X_I2C_SDA_1()  SHT2X_GPIO_PORT_I2C->BSRR = SHT2X_I2C_SDA_PIN				/* SDA = 1 */
	#define SHT2X_I2C_SDA_0()  SHT2X_GPIO_PORT_I2C->BRR = SHT2X_I2C_SDA_PIN				/* SDA = 0 */
	
	#define SHT2X_I2C_SDA_READ()  ((SHT2X_GPIO_PORT_I2C->IDR & SHT2X_I2C_SDA_PIN) != 0)	/* ��SDA����״̬ */
#endif


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalH(p,i)			{p->BSRR=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//����͵�ƽ






void i2c_CfgGpio(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);


#endif

