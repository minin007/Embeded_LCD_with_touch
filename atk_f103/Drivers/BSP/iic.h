#ifndef __IIC_H
#define __IIC_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define IIC_SCL_GPIO_PORT           GPIOB
#define IIC_SCL_GPIO_PIN            GPIO_PIN_1
#define IIC_SCL_GPIO_CLK_ENABLE();  do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)

#define IIC_SDA_GPIO_PORT           GPIOF
#define IIC_SDA_GPIO_PIN            GPIO_PIN_9
#define IIC_SDA_GPIO_CLK_ENABLE();  do{__HAL_RCC_GPIOF_CLK_ENABLE();}while(0)

/* IO操作 */
#define IIC_SCL(x)      do{x? \
                            HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)
#define IIC_SDA(x)      do{x? \
                            HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)
#define READ_SDA        HAL_GPIO_ReadPin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN)

/* 函数定义 */
//static void iic_delay(void);
void iic_init(void);
void iic_start(void);
void iic_stop(void);

void iic_ack(void);
void iic_nack(void);
uint8_t iic_wait_ack(void);

void iic_send_byte(uint8_t txd);
uint8_t iic_read_byte(unsigned char ack);

#endif
