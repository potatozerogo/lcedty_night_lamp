#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include <inttypes.h>


#define SHT2X_I2C_WR	0		/* 写控制bit */
#define SHT2X_I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */

#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE() 

#define SHT2X_GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define SHT2X_I2C_SCL_PIN		GPIO_PIN_6			/* 连接到SCL时钟线的GPIO */
#define SHT2X_I2C_SDA_PIN		GPIO_PIN_7			/* 连接到SDA数据线的GPIO */


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define SHT2X_I2C_SCL_1() digitalH(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SCL_PIN)		/* SCL = 1 */
	#define SHT2X_I2C_SCL_0()  digitalL(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SCL_PIN)		/* SCL = 0 */
	
	#define SHT2X_I2C_SDA_1() digitalH(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)		/* SDA = 1 */
	#define SHT2X_I2C_SDA_0()  digitalL(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)		/* SDA = 0 */
	
	#define SHT2X_I2C_SDA_READ()  GPIO_ReadInputDataBit(SHT2X_GPIO_PORT_I2C, SHT2X_I2C_SDA_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define SHT2X_I2C_SCL_1()  SHT2X_GPIO_PORT_I2C->BSRR = SHT2X_I2C_SCL_PIN				/* SCL = 1 */
	#define SHT2X_I2C_SCL_0()  SHT2X_GPIO_PORT_I2C->BRR = SHT2X_I2C_SCL_PIN				/* SCL = 0 */
	
	#define SHT2X_I2C_SDA_1()  SHT2X_GPIO_PORT_I2C->BSRR = SHT2X_I2C_SDA_PIN				/* SDA = 1 */
	#define SHT2X_I2C_SDA_0()  SHT2X_GPIO_PORT_I2C->BRR = SHT2X_I2C_SDA_PIN				/* SDA = 0 */
	
	#define SHT2X_I2C_SDA_READ()  ((SHT2X_GPIO_PORT_I2C->IDR & SHT2X_I2C_SDA_PIN) != 0)	/* 读SDA口线状态 */
#endif


/* 直接操作寄存器的方法控制IO */
#define	digitalH(p,i)			{p->BSRR=i;}			  //设置为高电平		
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//输出低电平






void i2c_CfgGpio(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);


#endif

