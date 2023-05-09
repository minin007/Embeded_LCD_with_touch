#include "./BSP/iic.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       控制I2C速度的延时
 * @param       无
 * @retval      无
 */
static void iic_delay(void)
{
    delay_us(2);
}
/**
 * @brief       电容触摸芯片IIC接口初始化
 * @param       无
 * @retval      无
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;             /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;           /* 高速 */
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);  /* 初始化SCL引脚 */
    
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;             /* 开漏输出 */
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);  /* 初始化SDA引脚 */
    
    
    iic_stop();
}
/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);     //先拉低SDA，再拉低SCL
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}
/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);     //SCL置高电平时，释放SDA
    iic_delay();
}
/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     //SCL置低电平时，释放SDA
    iic_delay();
}
/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_nack(void)
{
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}
/**
 * @brief       等待应答信号到来
 * @param       无
 * @retval      1，接收应答失败
 *              0，接收应答成功
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    IIC_SDA(1);     //主机释放SDA
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    
    while(READ_SDA) //等待应答
    {
        waittime++;
        if(waittime>250){
            //超时停止
            iic_stop();
            rack = 1;
            break;
        }
        iic_delay();
    }
    IIC_SCL(0);
    iic_delay();
    return rack;
}
/**
 * @brief       IIC发送一个字节
 * @param       txd: 要发送的数据
 * @retval      无
 */
void iic_send_byte(uint8_t txd)
{
    
    for(uint8_t i = 0; i<8; i++){
        IIC_SDA((txd & 0x80)>>7);
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        txd <<= 1;
    }
    IIC_SDA(1);     //发送完成，释放SDA
}
/**
 * @brief       IIC发送一个字节
 * @param       ack:  ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t iic_read_byte(unsigned char ack)
{
    uint8_t receive = 0;
    for(uint8_t i = 0; i<8; i++){
        receive <<= 1;
        IIC_SCL(1);
        iic_delay();
        
        if(READ_SDA){
            receive++;
        }
        IIC_SCL(0);
        iic_delay();
    }
    if(!ack){
        iic_nack();
    }
    else{
        iic_ack();
    }
    return receive;
}
