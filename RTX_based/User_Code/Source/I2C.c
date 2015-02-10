
#include "Include.h" 

/* I2C peripheral configuration defines (control interface of the audio codec) */
#define CODEC_I2C                      I2C1
#define CODEC_I2C_CLK                  RCC_APB1Periph_I2C1
#define CODEC_I2C_GPIO_CLOCK           RCC_AHB1Periph_GPIOB
#define CODEC_I2C_GPIO_AF              GPIO_AF_I2C1
#define CODEC_I2C_GPIO                 GPIOB
#define CODEC_I2C_SCL_PIN              GPIO_Pin_6
#define CODEC_I2C_SDA_PIN              GPIO_Pin_7
#define CODEC_I2S_SCL_PINSRC           GPIO_PinSource6
#define CODEC_I2S_SDA_PINSRC           GPIO_PinSource7

#ifndef I2C_SPEED
 #define I2C_SPEED                        200000
#endif /* I2C_SPEED */

/**
  * @brief Initializes IOs used by the Audio Codec (on the control and audio 
  *        interfaces).
  * @param  None
  * @retval None
  */
static void Codec_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable I2S and I2C GPIO clocks */
  RCC_AHB1PeriphClockCmd(CODEC_I2C_GPIO_CLOCK , ENABLE);

  /* CODEC_I2C SCL and SDA pins configuration -------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = CODEC_I2C_SCL_PIN | CODEC_I2C_SDA_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_I2C_GPIO, &GPIO_InitStructure);     
  /* Connect pins to I2C peripheral */
  GPIO_PinAFConfig(CODEC_I2C_GPIO, CODEC_I2S_SCL_PINSRC, CODEC_I2C_GPIO_AF);  
  GPIO_PinAFConfig(CODEC_I2C_GPIO, CODEC_I2S_SDA_PINSRC, CODEC_I2C_GPIO_AF);  

	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_9);//ds3231复位
}

/**
  * @brief  Initializes the Audio Codec control interface (I2C).
  * @param  None
  * @retval None
  */
static void Codec_CtrlInterface_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  
  /* Enable the CODEC_I2C peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_I2C_CLK, ENABLE);
  
  /* CODEC_I2C peripheral configuration */
  I2C_DeInit(CODEC_I2C);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x33;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  /* Enable the I2C peripheral */
  I2C_Cmd(CODEC_I2C, ENABLE);  
  I2C_Init(CODEC_I2C, &I2C_InitStructure);
}
void I2C_Init_DS3231_Lm(void)
{

	Codec_GPIO_Init();
	Codec_CtrlInterface_Init();
	
	//I2C_Clear_Alarm_Register();		  
}
//=================
/* Uncomment this line to enable verifying data sent to codec after each write 
  operation */
#define VERIFY_WRITTENDATA 
#define CODEC_FLAG_TIMEOUT             ((uint32_t)0x1000)
#define CODEC_LONG_TIMEOUT             ((uint32_t)(300 * CODEC_FLAG_TIMEOUT))
__IO uint32_t  CODECTimeout = CODEC_LONG_TIMEOUT;   
/* The 7 bits Codec address (sent through I2C interface) */
#define CODEC_ADDRESS                   0xD0  

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static uint32_t Codec_TIMEOUT_UserCallback(void)
{   
  return (0);
}
/**
  * @brief  Reads and returns the value of an audio codec register through the
  *         control interface (I2C).
  * @param  RegisterAddr: Address of the register to be read.
  * @retval Value of the register to be read or dummy value if the communication
  *         fails.
  */
static uint32_t Codec_ReadRegister(uint8_t RegisterAddr)
{
  uint32_t result = 0;

  /*!< While the bus is busy */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /* Start the config sequence */
  I2C_GenerateSTART(CODEC_I2C, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS+1, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the register address to be read */
  I2C_SendData(CODEC_I2C, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /*!< Send START condition a second time */  
  I2C_GenerateSTART(CODEC_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  } 
  
  /*!< Send Codec address for read */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS, I2C_Direction_Receiver);  
  
  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_ADDR) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }     
  
  /*!< Disable Acknowledgment */
  I2C_AcknowledgeConfig(CODEC_I2C, DISABLE);   
  
  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)CODEC_I2C->SR2;
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);
  
  /* Wait for the byte to be received */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_RXNE) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /*!< Read the byte received from the Codec */
  result = I2C_ReceiveData(CODEC_I2C);
  
  /* Wait to make sure that STOP flag has been cleared */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(CODEC_I2C->CR1 & I2C_CR1_STOP)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }  
  
  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(CODEC_I2C, ENABLE);  
  
  /* Clear AF flag for next communication */
  I2C_ClearFlag(CODEC_I2C, I2C_FLAG_AF); 
  
  /* Return the byte read from Codec */
  return result;
}
/**
  * @brief  Writes a Byte to a given register into the audio codec through the 
            control interface (I2C)
  * @param  RegisterAddr: The address (location) of the register to be written.
  * @param  RegisterValue: the Byte value to be written into destination register.
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue)
{
  uint32_t result = 0;

  /*!< While the bus is busy */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /* Start the config sequence */
  I2C_GenerateSTART(CODEC_I2C, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the first address for write operation */
  I2C_SendData(CODEC_I2C, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }
  
  /* Prepare the register value to be sent */
  I2C_SendData(CODEC_I2C, RegisterValue);
  
  /*!< Wait till all data have been physically transferred on the bus */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }
  
  /* End the configuration sequence */
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);  
  
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */  
  result = (Codec_ReadRegister(RegisterAddr) == RegisterValue)? 0:1;
#endif /* VERIFY_WRITTENDATA */

  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;  
}

//======================================================================
// 函数名称：
// u8 I2C_Read_DS3231(u8 adress, u8 *tempdata)
//
// 说明：向指定地址读取数据   MSB年。。。秒LSB
//======================================================================
u8 I2C_Read_DS3231(u8 *tempdata)
{
    u8 flag = 0, i = 0;    


		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	
		for (i=0; i<7; i++)                 //7字节数据
		{
			//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */  
			tempdata[i]=Codec_ReadRegister(i);
		}

	tempdata[6] = tempdata[6]/16 * 10 + tempdata[6] % 16;//年"16进制转成10进制"
	tempdata[5] = tempdata[5]/16 * 10 + tempdata[5] % 16;//月
	tempdata[4] = tempdata[4]/16 * 10 + tempdata[4] % 16;//日
	tempdata[2] = tempdata[2]/16 * 10 + tempdata[2] % 16;//时
	tempdata[1] = tempdata[1]/16 * 10 + tempdata[1] % 16;//分
	tempdata[0] = tempdata[0]/16 * 10 + tempdata[0] % 16;//秒

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

    return flag;
}
//======================================================================
// 函数名称：
// I2C_Write_DS3231(u8 adress, u8 *tempdata)
//
// 说明：向指定地址写入数据 MSB年。。。秒LSB
//======================================================================
u8 I2C_Write_DS3231(u8 *tempdata)
{

    u8 flag = 0;

		u8 i = 0;

	
		GPIO_SetBits(GPIOB, GPIO_Pin_9);
	
	
		for (i=0; i<7; i++)                 //7字节数据
		{
			//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */  
			Codec_WriteRegister(i,tempdata[i]);
		}
    return flag;
}
u8 I2C_Write_Alarm(u8 *tempdata)		//设定闹钟的秒、分、时、日期
{
	
		
//	u8 tx_count = 0;
	u8 flag = 0;
//	u8 jump = 0;
	u8 i = 0;

	u8 Date,Hour,Minute,Seconds,Temp_Data[4] = {0};


	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	Seconds = tempdata[0];
	Seconds &= 0x7F;

	Minute = tempdata[1];
	Minute &= 0x7F;

	Hour = tempdata[2];
	Hour &= 0x7F;
	Hour &= 0xBF;

	Date = tempdata[3];
	Date |= 0x80;
	Date &= 0xBF;

	Temp_Data[0] = Seconds;
	Temp_Data[1] = Minute;
	Temp_Data[2] = Hour;
	Temp_Data[3] = Date;
	
	I2C_Clear_Alarm_Register();		 
	
	for (i=0; i<4; i++)                 //4字节数据
		{
			//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */  
			Codec_WriteRegister(i+0x07,tempdata[i]);
		}	

    return flag;

}

void I2C_Write_Alarm_Register(u8 data)
{

		u8 i = 0;

		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	
		for (i=0; i<1; i++)                 //1字节数据
		{
			//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */  
			Codec_WriteRegister(i+0x0E,data);
		}
   

}

void I2C_Clear_Alarm_Register(void)
{
		u8 i = 0;

		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	
		for (i=0; i<1; i++)                 //1字节数据
		{
			//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */  
			Codec_WriteRegister(i+0x0F,0);
		}

}



//===========================================================
//u32 I2C_Get_Temperature(void)
//读DS3231温度
//===========================================================
float I2C_Get_Temperature(void)     /*     此函数不用   */    /*   by_lm   */ 
{
// // 	u32 temperature;
	float temp;
// //       
// //       // 等待控制寄存器的CONV位为0
// //       while( I2C_Read_DS3231_Reg(0x0E) & 0x20 );
// //       
// //       temperature = I2C_Read_DS3231_Reg(0X11);
// //       temperature <<= 5;
// //       temperature |= (I2C_Read_DS3231_Reg(0X12)>>3);
// //       if( temperature&0x1000 )      // 如果符号位为1，表示是一个负数
// //       {
// //             temperature |= 0xE000;  // 进行符号扩展
// //       }
// // 	temp = ((float)(((u32)(temperature *1000)) * 0.03125)) / 1000;

      return temp;
}

//======================================================================
// 函数名称：
// u8 I2C_Read_DS3231(u8 adress, u8 *tempdata)
//
// 说明：向指定寄存器读取数据  
//======================================================================

u8 I2C_Read_DS3231_Reg(u8 reg)
{
// //     u8 tx_count = 0;
    u8 data = 0;    

// //     //错误重发三次
// //     while(tx_count < 3)       
// //     {
// //         I2C_start_DS3231();

// //         I2C_Write_Byte_DS3231(0xD0);            //器件地址+写
// //         if (I2C_Receive_Ack_DS3231() == 1)           
// //         {   
// //             tx_count++;
// //             continue;
// //         }        

// //         I2C_Write_Byte_DS3231(reg);            //改访问地址指针
// //         if (I2C_Receive_Ack_DS3231() == 1)           
// //         {   
// //             tx_count++;
// //             continue;
// //         }

// //         I2C_Stop_DS3231();        
// //     
// //         I2C_start_DS3231();                     

// //         I2C_Write_Byte_DS3231(0xD1);            //器件地址+读
// //         if (I2C_Receive_Ack_DS3231() == 1)           
// //         {   
// //             tx_count++;
// //             continue;
// //         }

// //         data = I2C_Read_Byte_DS3231();
// //             
// //         I2C_Stop_DS3231();
// //         
// //         break;
// //     }
    return data;
}

