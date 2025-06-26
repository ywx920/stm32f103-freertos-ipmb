#include "soft_i2c.h"
// #include "delay.h"  // 延时函数：delay_1us()



void delay_1us(uint32_t us)
{
    // uint32_t i,  j;
    for( uint32_t i = 0; i < 500; i++)
    {
    }
}


// static void SDA_OUT(void)
// {
//     GPIO_InitTypeDef gpio;
//     gpio.GPIO_Pin = I2C_SDA_PIN;
//     gpio.GPIO_Mode = GPIO_Mode_Out_OD;
//     gpio.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(I2C_SDA_GPIO, &gpio);
// }

// static void SDA_IN(void)
// {
//     GPIO_InitTypeDef gpio;
//     gpio.GPIO_Pin = I2C_SDA_PIN;
//     gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     gpio.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(I2C_SDA_GPIO, &gpio);
// }

void Soft_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    IIC_SDA_1();
    IIC_SCL_1();
}

void I2C_Start(void)
{
    // printf("I2C_Start\r\n");
    //SDA_OUT();
    IIC_SDA_1();
    IIC_SCL_1();
    delay_1us(1);
    IIC_SDA_0();
    delay_1us(1);
    IIC_SCL_0();
    delay_1us(1);
}

void I2C_Stop(void)
{
    // printf("I2C_Stop\r\n");
    //SDA_OUT();
    IIC_SCL_0();
    IIC_SCL_1();
    delay_1us(1);
    IIC_SDA_1();
    delay_1us(1);
}

void I2C_SendByte(uint8_t byte)
{
    uint8_t t;   
    for(t=0;t<8;t++)
    {
        if(byte&0x80)
        {
            IIC_SDA_1();
        }
        else
        {
            IIC_SDA_0();
        }
		delay_1us(1);   
		IIC_SCL_1();
		delay_1us(1); 
		IIC_SCL_0();	
		delay_1us(1);
		if(t == 7)
        {
            IIC_SDA_1();    //释放总线
        }
        byte<<=1;
		delay_1us(1);  
   }

}

uint8_t IIC_Read_Byte(void)
{
    uint8_t i, byte = 0;

    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        IIC_SCL_1();
        delay_1us(1);
        if (READ_SDA)
            byte |= 1;
        IIC_SCL_0();
        delay_1us(1);
    }
    return byte;
}

uint8_t I2C_WaitACK(void)
{
    uint8_t timeout = 250;

    IIC_SDA_1();
    delay_1us(1);
    IIC_SCL_1();
    delay_1us(1);

    while (READ_SDA)
    {
        timeout--;
        if (timeout == 0)
        {
            I2C_Stop();
            return 1;
        }
    }

    IIC_SCL_0();
    delay_1us(1);
    return 0;
}

//产生ACK应答
void IIC_Ack(void)
{
	IIC_SDA_0();
	delay_1us(1);
	IIC_SCL_1();
	delay_1us(1);
	IIC_SCL_0();
	delay_1us(1);
	IIC_SDA_1();
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SDA_1();
	delay_1us(1);
	IIC_SCL_1();
	delay_1us(1);
	IIC_SCL_0();
	delay_1us(1);
}	

void I2C_WriteByte(uint8_t addr,uint8_t data,uint8_t device_addr)
{
	I2C_Start();  
	I2C_SendByte(device_addr);	//发器件地址
	I2C_WaitACK(); 
	I2C_SendByte(addr);   //发送寄存器地址
	I2C_WaitACK(); 
	I2C_SendByte(data);     //发送字节							   
    I2C_WaitACK();  		    	   
    I2C_Stop();//产生一个停止条件 
    delay_1us(1);
}
 
uint16_t I2C_ReadByte(uint8_t addr,uint8_t device_addr,uint8_t ByteNumToRead)  //读寄存器或读数据
{	
		uint16_t data;
		I2C_Start();  
		I2C_SendByte(device_addr);	//发器件地址
		I2C_WaitACK();
		I2C_SendByte(addr);   //发送寄存器地址
		I2C_WaitACK(); 
		I2C_Start();  	
		I2C_SendByte(device_addr+1);	//发器件地址
		I2C_WaitACK();
		data=IIC_Read_Byte();
	  IIC_NAck();
		I2C_Stop();//产生一个停止条件	    
		return data;
}
