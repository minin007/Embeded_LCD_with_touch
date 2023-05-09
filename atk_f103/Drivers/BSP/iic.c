#include "./BSP/iic.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       ����I2C�ٶȵ���ʱ
 * @param       ��
 * @retval      ��
 */
static void iic_delay(void)
{
    delay_us(2);
}
/**
 * @brief       ���ݴ���оƬIIC�ӿڳ�ʼ��
 * @param       ��
 * @retval      ��
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;             /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                     /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;           /* ���� */
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��SCL���� */
    
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;             /* ��©��� */
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��SDA���� */
    
    
    iic_stop();
}
/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);     //������SDA��������SCL
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}
/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);     //SCL�øߵ�ƽʱ���ͷ�SDA
    iic_delay();
}
/**
 * @brief       ����ACKӦ��
 * @param       ��
 * @retval      ��
 */
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     //SCL�õ͵�ƽʱ���ͷ�SDA
    iic_delay();
}
/**
 * @brief       ������ACKӦ��
 * @param       ��
 * @retval      ��
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
 * @brief       �ȴ�Ӧ���źŵ���
 * @param       ��
 * @retval      1������Ӧ��ʧ��
 *              0������Ӧ��ɹ�
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    IIC_SDA(1);     //�����ͷ�SDA
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    
    while(READ_SDA) //�ȴ�Ӧ��
    {
        waittime++;
        if(waittime>250){
            //��ʱֹͣ
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
 * @brief       IIC����һ���ֽ�
 * @param       txd: Ҫ���͵�����
 * @retval      ��
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
    IIC_SDA(1);     //������ɣ��ͷ�SDA
}
/**
 * @brief       IIC����һ���ֽ�
 * @param       ack:  ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
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
