/**
  ******************************************************************************
  * @file    led.c
  * @author  liming
  * @version V1.0.0
  * @date    13-8-2013
  * @brief  
  ******************************************************************************
	*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "led.h"
#include "RTL.h"


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void led_init(void)
{
	/* Private typedef -----------------------------------------------------------*/
	GPIO_InitTypeDef  GPIO_InitStructure;

  /* GPIOG Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* Configure PG6 and PG8 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* ÷∂Ø√Ê∞Âled*/
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}
void output_595(uint8_t dat)
{
	
 uint8_t i=0; 
	
 for(i=0;i<8;i++)    
	{
		if(dat&0x80)
			DS_1(); 
		else 
			DS_0();
		
		dat<<=1;
		
		SH_0();
		SH_1();
		SH_0();
	}
	ST_0();  
	ST_1();
	ST_0();
}
/**
  * @brief  OutPut SD_LED stats.
  * @arg 		dat:  stats
  * @retval None
  */
void SD_output_595(uint16_t dat)
{
	u8 i=0; 
 for(i=0;i<16;i++)    
	{
		if(dat&0x8000)
			SD_DS_1(); 
		else 
			SD_DS_0();
		
		dat<<=1;
		
		SD_SH_0();
		SD_SH_1();
		SD_SH_0();
		
	}	
	SD_ST_0();  
	SD_ST_1();  //  enable out
	SD_ST_0(); 	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
