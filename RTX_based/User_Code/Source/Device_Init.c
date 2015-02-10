#include "include.h"
extern void I2C_Init_DS3231_Lm(void);
/*******************************************************************************
* Function Name  	:	 DeviceInit
* Description    		:	板级初始化: I/O，定时器，AD，CAN总线等
* Output         			: 	None
* Return         			: 	None
*******************************************************************************/
void DeviceInit(void)
{    
	RCC_configuration();
	PWR_BackupAccessCmd(ENABLE);
	GPIO_configuration();

	I2C_Init_DS3231_Lm();
	
//	Exti_Configuration();
//	NVIC_configuration();
	USART_configuration();

//	TIM2_configuration(1);
	TIM3_configuration(1);
	//TIM4_configuration(1);//500MS
	TIM5_configuration(1);
	SRAM_Init();
	led_init();
	marix_keyboard_init();
	
}

/**********************************************************************************************************
* Function Name  	:	 RCC_Configuration
* Description    		:	系统时钟初始化，系统主时钟为72MHZ
* Output         			: 	None
* Return         			: 	None
***********************************************************************************************************/
void RCC_configuration(void)
{	
	u32				PLL_M;	 	
	u32				PLL_N;
	u32				PLL_P;
	u32				PLL_Q;
	//RCC_ClocksTypeDef RCC_Clocks;
	//配置前将所有RCC重置为初始值
	RCC_DeInit();

	//这里选择 外部晶振（HSE）作为 时钟源，因此首先打开外部晶振
	RCC_HSEConfig(RCC_HSE_ON);
	//等待外部晶振进入稳定状态
	while( RCC_WaitForHSEStartUp() != SUCCESS );

	//我们要选择PLL时钟作为系统时钟，因此这里先要对PLL时钟进行配置
	//选择外部晶振作为PLL的时钟源
	// 到这一步为止，已有 HSE_VALUE = 25 MHz.
	//PLL_VCO input clock = (HSE_VALUE or HSI_VALUE / PLL_M)，
	//根据文档，这个值被建议在 1~2MHz，因此我们令 PLL_M = 25，
	//即 PLL_VCO input clock = 1MHz 
	PLL_M		= 	25;	

	// 到这一步为止，已有 PLL_VCO input clock = 1 MHz.
	//PLL_VCO output clock = (PLL_VCO input clock) * PLL_N,
	//这个值要用来计算系统时钟，我们 令 PLL_N = 336,
	//即 PLL_VCO output clock = 336 MHz.		
	PLL_N	    = 	336;

	// 到这一步为止，已有 PLL_VCO output clock = 168 MHz.
	//System Clock = (PLL_VCO output clock)/PLL_P ,
	//因为我们要 SystemClock = 168 Mhz，因此令 PLL_P = 2.
	
	PLL_P	   	=	2;

	//这个系数用来配置SD卡读写，USB等功能，暂时不用，根据文档，暂时先设为6
	PLL_Q	   	= 	7;

	// 配置PLL并将其使能，获得 168Mhz 的 System Clock 时钟
	RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q);
	RCC_PLLCmd(ENABLE);

	//到了这一步，我们已经配置好了PLL时钟。下面我们配置Syetem Clock
	//选择PLL时钟作为系统时钟源
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	//到了这一步，我们已经配置好了系统时钟，频率为 168MHz. 下面我们可以对 AHB，APB，外设等的	时钟进行配置
	//时钟的结构请参考用户手册

	//首先配置 AHB时钟（HCLK）. 为了获得较高的频率，我们对 SYSCLK 1分频，得到HCLK
	RCC_HCLKConfig(RCC_HCLK_Div1);

	//APBx时钟（PCLK）由AHB时钟（HCLK）分频得到，下面我们配置 PCLK

	//APB1时钟配置. 4分频，即 PCLK1 = 42 MHz
	RCC_PCLK1Config(RCC_HCLK_Div4);

	//APB2时钟配置. 2分频，即 PCLK2 = 84 MHz
	RCC_PCLK2Config(RCC_HCLK_Div2);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
	//函数结束*****
	//RCC_GetClocksFreq(&RCC_Clocks);
	//SysTick_Config(RCC_Clocks.SYSCLK_Frequency/100);

	NVIC_SetPriority(SysTick_IRQn, 1);
	 // GPIOG Periph clock enable 
	RCC_AHB1PeriphClockCmd(   RCC_AHB1Periph_GPIOA
							| RCC_AHB1Periph_GPIOB
							| RCC_AHB1Periph_GPIOC
							| RCC_AHB1Periph_GPIOD
							| RCC_AHB1Periph_GPIOE
							| RCC_AHB1Periph_DMA1
							, ENABLE);
	
	RCC_APB1PeriphClockCmd(   RCC_APB1Periph_USART2
							| RCC_APB1Periph_UART4
							| RCC_APB1Periph_SPI3
							//| RCC_APB1Periph_CAN1
							| RCC_APB1Periph_TIM2
							| RCC_APB1Periph_TIM3
							| RCC_APB1Periph_TIM4
							| RCC_APB1Periph_TIM5
							,  ENABLE);

	RCC_APB2PeriphClockCmd(  RCC_APB2Periph_SYSCFG
							, ENABLE);

	//以上函数可以大体上说明这些库函数的作用
}

/**********************************************************************************************************
* Function Name  	:	 RCC_Configuration
* Description    		:	系统时钟初始化，系统主时钟为72MHZ
* Output         			: 	None
* Return         			: 	None
***********************************************************************************************************/
/*void RCC_configuration(void)
{	
	RCC_ClocksTypeDef RCC_Clocks;
	SystemInit();

	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.SYSCLK_Frequency/100);

	NVIC_SetPriority(SysTick_IRQn, 1);
	 // GPIOG Periph clock enable 
	RCC_AHB1PeriphClockCmd(   RCC_AHB1Periph_GPIOD
							| RCC_AHB1Periph_GPIOA
							, ENABLE);

	RCC_APB1PeriphClockCmd(   RCC_APB1Periph_USART2
							| RCC_APB1Periph_CAN1, ENABLE);
	//以上函数可以大体上说明这些库函数的作用
}
*/

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None
  * @retval None
  */
void SRAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 +                       SRAM pins assignment                                   +
 +-------------------+--------------------+------------------+------------------+
 | PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0 <-> FSMC_A10 | 
 | PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1 <-> FSMC_A11 | 
 | PD4  <-> FSMC_NOE | PE3  <-> FSMC_A19  | PF2  <-> FSMC_A2 | PG2 <-> FSMC_A12 | 
 | PD5  <-> FSMC_NWE | PE4  <-> FSMC_A20  | PF3  <-> FSMC_A3 | PG3 <-> FSMC_A13 | 
 | PD8  <-> FSMC_D13 | PE7  <-> FSMC_D4   | PF4  <-> FSMC_A4 | PG4 <-> FSMC_A14 | 
 | PD9  <-> FSMC_D14 | PE8  <-> FSMC_D5   | PF5  <-> FSMC_A5 | PG5 <-> FSMC_A15 | 
 | PD10 <-> FSMC_D15 | PE9  <-> FSMC_D6   | PF12 <-> FSMC_A6 | 
 | PD11 <-> FSMC_A16 | PE10 <-> FSMC_D7   | PF13 <-> FSMC_A7 |------------------+
 | PD12 <-> FSMC_A17 | PE11 <-> FSMC_D8   | PF14 <-> FSMC_A8 | 
 | PD13 <-> FSMC_A18 | PE12 <-> FSMC_D9   | PF15 <-> FSMC_A9 | 
 | PD14 <-> FSMC_D0  | PE13 <-> FSMC_D10  |------------------+
 | PD15 <-> FSMC_D1  | PE14 <-> FSMC_D11  |
 |                   | PE15 <-> FSMC_D12  |
 +-------------------+--------------------+
 
	PD7 <-> FSMC_NE1 | 
*/



  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | 
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 |
																
																GPIO_Pin_7 ;
																
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* GPIOE configuration */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3 | GPIO_Pin_4 |
                                GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);


  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  | 
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;      

  GPIO_Init(GPIOF, &GPIO_InitStructure);


  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
 // GPIO_PinAFConfig(GPIOG, GPIO_PinSource10 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5 ; //|GPIO_Pin_10;      

  GPIO_Init(GPIOG, &GPIO_InitStructure);

/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 3;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 6;
	
// 	p.FSMC_AddressSetupTime = 1;
//   p.FSMC_AddressHoldTime = 0;
//   p.FSMC_DataSetupTime = 2;
	
  p.FSMC_BusTurnAroundDuration = 1;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_C;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM1 Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

}

//======================================================================
// 函数名称：
// GPIO_configuration(void)
//			 
//说明：端口配置
//======================================================================
void GPIO_configuration(void)
{
	GPIO_InitTypeDef GPIO_init;       
	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;

	//硬件看门狗--喂狗
	/* Configure Pg7 in output pushpull mode */
	  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_init.GPIO_Pin = GPIO_Pin_7;
	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init.GPIO_OType = GPIO_OType_PP;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_init);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	GPIO_init.GPIO_OType = GPIO_OType_PP;
	GPIO_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_init.GPIO_Mode = GPIO_Mode_AF;
		
	GPIO_init.GPIO_Pin = GPIO_Pin_10;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_init);
	
	/* Configure USART4 Rx as alternate function  */
	GPIO_init.GPIO_Mode = GPIO_Mode_AF;
	GPIO_init.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_init);

	/***********************I2C***************************/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
	GPIO_init.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init.GPIO_OType = GPIO_OType_OD;  
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_init);   


	//ALARM INTERUPT
	/***********************EXTI**************************/
	//改为PB5
	GPIO_init.GPIO_Pin = GPIO_Pin_5;
	GPIO_init.GPIO_Mode = GPIO_Mode_IN;
  GPIO_init.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_init);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	
	//=====指示灯输出
// // 	GPIO_init.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6;
// // 	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
// // 	GPIO_init.GPIO_OType = GPIO_OType_PP;  
// // 	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
// // 	GPIO_Init(GPIOE, &GPIO_init);   
	
// // 	GPIO_init.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
// // 	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
// // 	GPIO_init.GPIO_OType = GPIO_OType_PP;  
// // 	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
// // 	GPIO_Init(GPIOF, &GPIO_init);   
}
void Exti_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_DeInit();
	
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	EXTI_ClearITPendingBit(EXTI_Line5); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
//======================================================================
// 函数名称：
// NVIC_configuration(void)
//			 
// 说明：中断配置
//======================================================================
void NVIC_configuration(void)
{
		//初始化变量定义
	NVIC_InitTypeDef nvic_init;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
//#define VECT_TAB_RAM
	//选择向量表位置，宏在编译器内定义或自己定义在文件中，以便切换编程模式
#ifdef  VECT_TAB_RAM        
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else        
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
// // 	//优先级选定
	
//=================================	//by_lm	
	/*UART4中断配置*/
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 						//嵌套优先级分组为 1    //by_lm   1:3
	
	

	NVIC_ClearPendingIRQ(UART4_IRQn);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 						//嵌套通道为UART4_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	//抢占优先级为 0        //by_lm   0:0   拥有最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 					//响应优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						//通道中断使能
	NVIC_Init(&NVIC_InitStructure);


	NVIC_ClearPendingIRQ(DMA1_Stream4_IRQn);
	/*DMA中断配置*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 						//嵌套优先级分组为 1      
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn; 		//嵌套通道为DMA1_Stream4_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 	//抢占优先级为 1       //by_lm   1:0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 					//响应优先级为 0      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						//通道中断使能
	NVIC_Init(&NVIC_InitStructure);		

//================================= //by_lm

	//Alarm exti
// // 	I2C_Clear_Alarm_Register();		  //  I2C_lm   有待改动的			
// // 	
// // 	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
// // 	nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn;
// // 	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:2
// // 	nvic_init.NVIC_IRQChannelSubPriority = 2;
// // 	nvic_init.NVIC_IRQChannelCmd = ENABLE;
// // 	NVIC_Init(&nvic_init); 


	//TIM3中断
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	nvic_init.NVIC_IRQChannel = TIM3_IRQn;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:4
	nvic_init.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&nvic_init);

	//TIM4中断
	NVIC_ClearPendingIRQ(TIM4_IRQn);
	nvic_init.NVIC_IRQChannel = TIM4_IRQn;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:5
	nvic_init.NVIC_IRQChannelSubPriority = 5;
	NVIC_Init(&nvic_init);
	
	//TIM5中断
	NVIC_ClearPendingIRQ(TIM5_IRQn);
	nvic_init.NVIC_IRQChannel = TIM5_IRQn;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:6
	nvic_init.NVIC_IRQChannelSubPriority = 6;
	NVIC_Init(&nvic_init); 
	

}

void NVIC_disable_configuration(void)
{
		//初始化变量定义
	NVIC_InitTypeDef nvic_init;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 						//嵌套优先级分组为 1    //by_lm   1:3
		
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 						//嵌套通道为UART4_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	//抢占优先级为 0        //by_lm   0:0   拥有最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 					//响应优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; 						//通道中断使能
	NVIC_Init(&NVIC_InitStructure);

	/*DMA中断配置*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 						//嵌套优先级分组为 1      
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn; 		//嵌套通道为DMA1_Stream4_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 	//抢占优先级为 1       //by_lm   1:0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 					//响应优先级为 0      
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; 						//通道中断使能
	NVIC_Init(&NVIC_InitStructure);		

	//Alarm exti

	
	nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:2
	nvic_init.NVIC_IRQChannelSubPriority = 2;
	nvic_init.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&nvic_init); 

	//TIM2中断
	nvic_init.NVIC_IRQChannel = TIM2_IRQn;
	nvic_init.NVIC_IRQChannelCmd = DISABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:3
	nvic_init.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_init);

	//TIM3中断
	nvic_init.NVIC_IRQChannel = TIM3_IRQn;
	nvic_init.NVIC_IRQChannelCmd = DISABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:4
	nvic_init.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&nvic_init);

	//TIM4中断
	nvic_init.NVIC_IRQChannel = TIM4_IRQn;
	nvic_init.NVIC_IRQChannelCmd = DISABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:5
	nvic_init.NVIC_IRQChannelSubPriority = 5;
	NVIC_Init(&nvic_init);
	
	//TIM5中断
	nvic_init.NVIC_IRQChannel = TIM5_IRQn;
	nvic_init.NVIC_IRQChannelCmd = DISABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:6
	nvic_init.NVIC_IRQChannelSubPriority = 6;
	NVIC_Init(&nvic_init); 
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define UART4_DR_Addr 0x40004C04   
DMA_InitTypeDef    DMA_InitStructure;
uint8_t Uart4_Tx_Buffer[10] = {0};

void DMA_Config(void)
{
  /* Enable DMA1 clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  /* DMA1 Stream4 channe4 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)UART4_DR_Addr;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Uart4_Tx_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 10;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA1_Stream4, &DMA_InitStructure);  //  for USART4
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE); //DMA 通道4 传输完成中断

}

/*******************************************************************************
* Function Name  : DMAReConfig_TX
* Description    : 重新允许DMA
*******************************************************************************/
void DMAReConfig_TX(uint8_t *pTx_Data,uint16_t  DMA_lenth)
{
// // 	uint32_t i=0;
// // 	
// // 	for(i=0;i<DMA_lenth;i++)
// // 	{
// // 		Uart4_Tx_Buffer[i]=pTx_Data[i];
// // 	}

// // 	DMA_DeInit(DMA1_Stream4);
// // 	DMA_InitStructure.DMA_BufferSize = DMA_lenth;
// // 	DMA_Init(DMA1_Stream4, &DMA_InitStructure);  //  for USART4
// // 	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
// // 	DMA_Cmd(DMA1_Stream4, ENABLE);

// // 	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);	
	
}


//======================================================================
// 函数名称：
// USART_configuration(void)
//			 
// 说明：串口设置
//======================================================================
void USART_configuration(void)
{
	USART_InitTypeDef usart2_init;
	USART_InitTypeDef usart4_init;
	
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
//   usart4_init.USART_BaudRate = 115200;
//   usart4_init.USART_WordLength = USART_WordLength_8b;
//   usart4_init.USART_StopBits = USART_StopBits_1;
//   usart4_init.USART_Parity = USART_Parity_No;
//   usart4_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//   usart4_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//   /* USART configuration */
//   USART_Init(UART4, &usart4_init); 
// 	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);     
//   /* Enable USART */
//   USART_Cmd(UART4, ENABLE);
// 	
// 	DMA_Config();

  usart4_init.USART_BaudRate = 115200;
  usart4_init.USART_WordLength = USART_WordLength_8b;
  usart4_init.USART_StopBits = USART_StopBits_1;
  usart4_init.USART_Parity = USART_Parity_No;
  usart4_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart4_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(UART4, &usart4_init);   
  /* Enable USART */
  USART_Cmd(UART4, ENABLE);

  usart2_init.USART_BaudRate = 115200;
  usart2_init.USART_WordLength = USART_WordLength_8b;
  usart2_init.USART_StopBits = USART_StopBits_1;
  usart2_init.USART_Parity = USART_Parity_No;
  usart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart2_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(USART2, &usart2_init);   
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);

}

//   debug for UART4   end
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//======================================================================
// 函数名称：
// SPI1_configuration(void)
//			 
// 说明：SPI1配置
//======================================================================
void SPI3_configuration(void)
{
    SPI_InitTypeDef spi3_init;

    SPI_I2S_DeInit(SPI3);

    spi3_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //双线全双工 
    spi3_init.SPI_Mode = SPI_Mode_Master;                        //主模式
    spi3_init.SPI_NSS = SPI_NSS_Soft;                            //硬件配置
    spi3_init.SPI_FirstBit = SPI_FirstBit_MSB;                   //低位在先
    spi3_init.SPI_DataSize = SPI_DataSize_8b;                    //字节发送
    spi3_init.SPI_CPOL = SPI_CPOL_Low;                           //低电平空闲
    spi3_init.SPI_CPHA = SPI_CPHA_1Edge;                         //第一个沿采样
    spi3_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //预分频
    spi3_init.SPI_CRCPolynomial = 7;                             //多项式次数

    SPI_Init(SPI3, &spi3_init);                                  //初始化

    SPI_SSOutputCmd(SPI3, ENABLE);                               //NSS引脚设置，正常I/O口 

    GPIO_SetBits(GPIOE, GPIO_Pin_8);                            //NSS初始化为高电平

    SPI_Cmd(SPI3, ENABLE);                                       //使能
}
//======================================================================
// 函数名称：
// TIM2_configuration
//			 
// 说明：定时设置，0.25秒的倍数
//======================================================================
void TIM2_configuration(u8 second)
{
    TIM_TimeBaseInitTypeDef time_init;

    //初始化
    TIM_DeInit(TIM2);
		second =4;
    time_init.TIM_CounterMode = TIM_CounterMode_Up;         //计数方向
    time_init.TIM_Period = second * 499;               			//计数周期
    time_init.TIM_Prescaler = 41999;                        //预分频,计算时PSC=N+1
    time_init.TIM_ClockDivision = TIM_CKD_DIV4;             //采样周期

    TIM_TimeBaseInit(TIM2, &time_init);
    
    //清除由初始化产生的更新标志，防止进入中断
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    //直接更新寄存器
    TIM_ARRPreloadConfig(TIM2, DISABLE);

    //中断设定
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);    

    //关定时器
    TIM_Cmd(TIM2, DISABLE);
	
}
//======================================================================
// 函数名称：
// TIM5_configuration
//			 
// 说明：定时设置，1ms的倍数   //  by_lm
//    time_init.TIM_Period = second * 19;               			//计数周期
//    time_init.TIM_Prescaler = 4199;                        //预分频,计算时PSC=N+1

//======================================================================

void TIM3_configuration(u8 second)
{
    TIM_TimeBaseInitTypeDef time_init;

    //初始化
    TIM_DeInit(TIM3);
	
    time_init.TIM_CounterMode = TIM_CounterMode_Up;         //计数方向
    time_init.TIM_Period = second * 19;               			//计数周期
    time_init.TIM_Prescaler = 4199;                        //预分频,计算时PSC=N+1
    time_init.TIM_ClockDivision = TIM_CKD_DIV1;             //采样周期	
	
	

    TIM_TimeBaseInit(TIM3, &time_init);
    
    //清除由初始化产生的更新标志，防止进入中断
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    
    //直接更新寄存器
    TIM_ARRPreloadConfig(TIM3, DISABLE);



    //关定时器
    TIM_Cmd(TIM3, DISABLE);
	    //中断设定
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	
	
}
//======================================================================
// 函数名称：
// TIM4_configuration
//			 
// 说明：定时设置，500MS
//======================================================================
void TIM4_configuration(u8 second)
{
    TIM_TimeBaseInitTypeDef time_init;

    //初始化
    TIM_DeInit(TIM4);

    time_init.TIM_CounterMode = TIM_CounterMode_Up;         //计数方向
    time_init.TIM_Period = second * 399;               	//计数周期
    time_init.TIM_Prescaler = 41999;                        //预分频,计算时PSC=N+1
    time_init.TIM_ClockDivision = TIM_CKD_DIV4;             //采样周期

    TIM_TimeBaseInit(TIM4, &time_init);
    
    //清除由初始化产生的更新标志，防止进入中断
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    
    //直接更新寄存器
    TIM_ARRPreloadConfig(TIM4, DISABLE);



    //关定时器
    TIM_Cmd(TIM4, DISABLE);
		
		    //中断设定
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    
	
}
//======================================================================
// 函数名称：
// TIM5_configuration
//			 
// 说明：定时设置，1S的倍数
//======================================================================

void TIM5_configuration(u8 second)
{
    TIM_TimeBaseInitTypeDef time_init;

    //初始化
    TIM_DeInit(TIM5);

    time_init.TIM_CounterMode = TIM_CounterMode_Up;         //计数方向
    time_init.TIM_Period = second * 1999;               	//计数周期
    time_init.TIM_Prescaler = 41999;                        //预分频,计算时PSC=N+1
    time_init.TIM_ClockDivision = TIM_CKD_DIV1;             //采样周期

    TIM_TimeBaseInit(TIM5, &time_init);
    
    //清除由初始化产生的更新标志，防止进入中断
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    //直接更新寄存器
    TIM_ARRPreloadConfig(TIM5, DISABLE);

    //中断设定
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);    

    //关定时器
    TIM_Cmd(TIM5, DISABLE);
	
}
/*******************************************************************************
* Function Name  : GPIO_PinReverse
* Description    : GPIO位取反
* Input          : GPIO口：GPIO_TypeDef* GPIOx
* Output         : GPIO位：uint16_t GPIO_Pin
* Return         : None
*******************************************************************************/
void GPIO_PinReverse(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    
    GPIOx->ODR ^= GPIO_Pin;
}


