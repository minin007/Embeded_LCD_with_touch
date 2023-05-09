
#include <stdio.h>
#include "./SYSTEM/delay/delay.h"
#include ".\BSP\touch.h"
#include "./BSP/iic.h"
#include <string.h>
#include "./BSP/tflcd.h"

uint8_t tp_num = 5;     //触摸点数量

/* GT9XXX 10个触摸点(最多) 对应的寄存器表 */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};
/**
 * @brief       初始化gt9xxx触摸屏
 * @param       无
 * @retval      0, 初始化成功; 1, 初始化失败;
 */
uint8_t gt9xxx_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    GT9XXX_RST_GPIO_CLK_ENABLE();   /* RST引脚时钟使能 */
    GT9XXX_INT_GPIO_CLK_ENABLE();   /* INT引脚时钟使能 */
    
    gpio_init_struct.Pin = GT9XXX_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GT9XXX_RST_GPIO_PORT, &gpio_init_struct); /* 初始化RST引脚 */

    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct); /* 初始化INT引脚 */
    
    iic_init();
    /* 设定使用通信地址为0x28/29 0xba/bb方式 */
    HAL_GPIO_WritePin(GT9XXX_INT_GPIO_PORT,GT9XXX_INT_GPIO_PIN,GPIO_PIN_SET);
    GT9XXX_RST(0);
    delay_ms(10);
    GT9XXX_RST(1);
    delay_ms(10);
    
    /* INT引脚模式重新设置, 输入模式, 浮空输入 */
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 不带上下拉，浮空模式 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct); /* 初始化INT引脚 */
    
    delay_ms(100);
    uint8_t ID[5];
    gt9xxx_rd_reg(GT9XXX_PID_REG, ID, 4);
    ID[4] = 0;
    printf("CTP ID: %s\r\n", ID);
    
    if(strcmp((char *)ID, "9271") == 0){
        tp_num = 10;
    }
    uint8_t temp = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &temp, 1);   //软复位
    delay_ms(10);
    temp = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &temp, 1);   //复位结束
    
    return 0;
}
/**
 * @brief       向gt9xxx写入一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 写数据长度
 * @retval      0, 成功; 1, 失败;
 */
uint8_t gt9xxx_wr_reg(uint16_t wreg, uint8_t *wbuf, uint8_t wlen)
{
    uint8_t ret = 0;
    iic_start();
    iic_send_byte(GT9XXX_CMD_WR);
    iic_wait_ack();
    iic_send_byte(wreg >> 8);    //高八位
    iic_wait_ack();
    iic_send_byte(wreg & 0xff);  //低八位
    iic_wait_ack();
    
    for(uint8_t i = 0; i<wlen; i++){
        iic_send_byte(wbuf[i]);
        ret = iic_wait_ack();
        if(ret) break;
    }
    iic_stop();
    return ret;
}
/**
 * @brief       从gt9xxx读出一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 读数据长度
 * @retval      无
 */
void gt9xxx_rd_reg(uint16_t rreg, uint8_t *rbuf, uint8_t rlen)
{
    iic_start();
    iic_send_byte(GT9XXX_CMD_WR);
    iic_wait_ack();
    iic_send_byte(rreg >> 8);    //高八位
    iic_wait_ack();
    iic_send_byte(rreg & 0xff);  //低八位
    iic_wait_ack();
    iic_start();
    iic_send_byte(GT9XXX_CMD_RD);
    iic_wait_ack();
    
    for(uint8_t i = 0; i<rlen; i++){
        rbuf[i] = iic_read_byte(i == (rlen-1) ? 0 : 1);
    }
    
    iic_stop();
    
}
/**
 * @brief       扫描触摸屏(采用查询方式)
 * @param       无
 * @retval      当前触屏状态
 *   @arg       0, 触屏无触摸; 
 *   @arg       1, 触屏有触摸;
 */
void gt9xxx_scan(void)
{
    uint8_t buf[4];
    //uint8_t res = 0;
    //uint16_t temp, tempsta;
    uint8_t mode = 0;
    uint16_t x,y;
    //static uint8_t t = 0;   //查询次数，用于控制间隔
    //t++;
    
    //if((t%10) == 0 || t<10)
    //{
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);
        if((mode & 0x80) && (mode & 0xf)<=5){
            uint8_t t = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &t, 1);  //清除标志
        }
        if((mode & 0xf) && (mode & 0xf)<=5){
            gt9xxx_rd_reg(GT9XXX_TP1_REG, buf, 4);  //
            x = ((uint16_t)buf[1]<<8) + buf[0];
            y = ((uint16_t)buf[3]<<8) + buf[2];
            printf("x:%d y:%d\r\n", x, y);
            lcd_fill_circle(x, y, 10, WHITE);
        }
    //}
    //return 0;
}

