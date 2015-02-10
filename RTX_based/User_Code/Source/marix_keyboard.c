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
#include "marix_keyboard.h"
#include <RTL.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void marix_keyboard_init(void)
{
	/* Private typedef -----------------------------------------------------------*/
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
  /* GPIOG Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  /* Configure PG9 ~ PG12 in output pull_UP mode as keyboard row--key_out1~key_out4*/
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* Configure PG13 ~ PG14 PA4~PA5 in input pull_UP mode as keyboard line--key_out5~key_out8*/
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_DOWN;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

}


/*read input line number,return line number*/
unsigned char line_ReadInputNumber()
{
	
	if(LINE1_INPUT)		//line 1 have key pressing
	{
		return 0;
	}
	else if(LINE2_INPUT)
	{
		return 1;
	}
	else if(LINE3_INPUT)
	{
		return 2;
	}
	else if(LINE4_INPUT)
	{
		return 3;
	}
	return 0;//no key pressing
}
/*read input key number,return key number*/
unsigned char key_ReadInputNumber()
{
	unsigned char line_number = 0;
	/*第1/2/3/4行输出低，判断列是否有低电平，有即有按键按下*/
	ROW1_OUTPUT();
	os_dly_wait(2);
	line_number = line_ReadInputNumber();
	if(line_number)
	{
		return line_number+3*(1-1);
	}
	
	ROW2_OUTPUT();
	os_dly_wait(2);
	line_number = line_ReadInputNumber();
	if(line_number)
	{
		return line_number+3*(2-1);
	}
	
	ROW3_OUTPUT();
	os_dly_wait(2);
	line_number = line_ReadInputNumber();
	if(line_number)
	{
		return line_number+3*(3-1);
	}
	
	ROW4_OUTPUT();
	os_dly_wait(2);
	line_number = line_ReadInputNumber();
	if(line_number)
	{
		return line_number+3*(4-1);
	}
	return 0;//no key pressing
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
