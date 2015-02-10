/**
  ******************************************************************************
  * @file    led.h
  * @author  liming
  * @version V1.0.0
  * @date    13-8-2013
  * @brief  
  ******************************************************************************
	*/
#ifndef __LED_H
#define __LED_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define DS_0()   GPIOC->BSRRH = GPIO_Pin_8
#define DS_1()   GPIOC->BSRRL = GPIO_Pin_8

#define ST_0()   GPIOC->BSRRH = GPIO_Pin_6
#define ST_1()   GPIOC->BSRRL = GPIO_Pin_6

#define SH_0()   GPIOC->BSRRH = GPIO_Pin_7
#define SH_1()   GPIOC->BSRRL = GPIO_Pin_7

/*手动面板led*/
#define SD_DS_0()   GPIOA->BSRRL = GPIO_Pin_9
#define SD_DS_1()   GPIOA->BSRRH = GPIO_Pin_9

#define SD_ST_0()   GPIOC->BSRRH = GPIO_Pin_9
#define SD_ST_1()   GPIOC->BSRRL = GPIO_Pin_9

#define SD_SH_0()   GPIOA->BSRRH = GPIO_Pin_8
#define SD_SH_1()   GPIOA->BSRRL = GPIO_Pin_8

#define LED_NUM 		8   			//led的总数
#define HAND_LED_NUM 		12    //手动led的总数
typedef enum 
{
	LED_NONE = 0, //   无变化
  LED1 = 1,
  LED2 = 2,
  LED3 = 3,
  LED4 = 4,
	LED5 = 5,
	LED6 = 6,
	LED7 = 7,
	LED8 = 8,      
	SD_LED1 = 9,		//手动面板LED
  SD_LED2 = 10,
  SD_LED3 = 11,
  SD_LED4 = 12,
	SD_LED5 = 13,
	SD_LED6 = 14,
	SD_LED7 = 15,
	SD_LED8 = 16,      
	SD_LED9 = 17,
	SD_LED10 = 18,
	SD_LED11 = 19,
	SD_LED12 = 20,
	
	LED_ALL = 255      
} Led_TypeDef;

/*前端led 序号与物理意义对应关系*/
#define CTLR_TYPE1_LED	LED4 
#define CTLR_TYPE2_LED	LED5 

#define SEND_LED				LED1 
#define RECEIVE_LED			LED6 

#define RUN_LED					LED2 
#define ERROR_LED				LED7 
#define ERROR1_LED			LED3 
#define ERROR2_LED			LED8 

/*手动面板led 序号与物理意义对应关系*/
#define HAND_CTRL_LED						SD_LED1 
#define SYSTEM_CTRL_LED					SD_LED2 
#define ATOU_CTRL_LED						SD_LED3 
#define BUS_CTRL_LED						SD_LED4 
#define OPTIMIZE_CTRL_LED				SD_LED5 
#define INDUCTION_CTRL_LED			SD_LED6 
#define PERIOD_CTRL_LED					SD_LED7 
#define HOLD_CTRL_LED						SD_LED8 
#define STEP_CTRL_LED						SD_LED9 
#define RED_CTRL_LED						SD_LED10 
#define YELLOW_CTRL_LED					SD_LED11 
#define NORMAL_CTRL_LED					SD_LED12 

typedef enum 
{
  LED_OFF = 1,
  LED_ON,
 	LED_RUN,									// 500 ms 闪烁
 	LED_FAST,									// 100 ms 闪烁
 	LED_FAST_STOP							// 100 ms 闪烁 4次  停1s
} Led_Run_TypeDef;

#define	LED_TIME_500ms	 	5
#define	LED_TIME_1s 			10

#define	FAST_NUM 					4*2
#define	STOP_NUM 					5*2

extern void led_init(void);
extern void output_595(uint8_t dat);
extern void SD_output_595(uint16_t dat);
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
