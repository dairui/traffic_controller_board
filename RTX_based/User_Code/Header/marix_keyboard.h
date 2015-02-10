/**
  ******************************************************************************
  * @file    marix_keyboard.h
  * @author  liming
  * @version V1.0.0
  * @date    13-8-2013
  * @brief  
  ******************************************************************************
	*/
#ifndef __MATRIX_KEYBOARD_H
#define __MATRIX_KEYBOARD_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/*ROW1~4 OUT*/
#define  SET_ROW_OUT1   	GPIO_SetBits(GPIOG, GPIO_Pin_9)
#define  SET_ROW_OUT2   	GPIO_SetBits(GPIOG, GPIO_Pin_10) 
#define  SET_ROW_OUT3   	GPIO_SetBits(GPIOG, GPIO_Pin_11) 
#define  SET_ROW_OUT4   	GPIO_SetBits(GPIOG, GPIO_Pin_12)
#define  RESET_ROW_OUT1   GPIO_ResetBits(GPIOG, GPIO_Pin_9)
#define  RESET_ROW_OUT2   GPIO_ResetBits(GPIOG, GPIO_Pin_10) 
#define  RESET_ROW_OUT3   GPIO_ResetBits(GPIOG, GPIO_Pin_11) 
#define  RESET_ROW_OUT4   GPIO_ResetBits(GPIOG, GPIO_Pin_12)

/*line1~4 in*/
#define  READ_LINE_IN4  (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_13))
#define  READ_LINE_IN3  (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_14))
#define  READ_LINE_IN2  (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))
#define  READ_LINE_IN1  (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5))

/*row output control--select which row to output*/
#define  ROWALL_OUTPUT()	SET_ROW_OUT1;\
													SET_ROW_OUT2;\
													SET_ROW_OUT3;\
													SET_ROW_OUT4
#define  ROWALL_CLOSE()		RESET_ROW_OUT1;\
													RESET_ROW_OUT2;\
													RESET_ROW_OUT3;\
													RESET_ROW_OUT4													
#define  ROW1_OUTPUT()		SET_ROW_OUT1;\
													RESET_ROW_OUT2;\
													RESET_ROW_OUT3;\
													RESET_ROW_OUT4
#define  ROW2_OUTPUT()		RESET_ROW_OUT1;\
													SET_ROW_OUT2;\
													RESET_ROW_OUT3;\
													RESET_ROW_OUT4
#define  ROW3_OUTPUT()		RESET_ROW_OUT1;\
													RESET_ROW_OUT2;\
													SET_ROW_OUT3;\
													RESET_ROW_OUT4
#define  ROW4_OUTPUT()		RESET_ROW_OUT1;\
													RESET_ROW_OUT2;\
													RESET_ROW_OUT3;\
													SET_ROW_OUT4

/*read line input state*/
#define  LINE1_INPUT		((READ_LINE_IN1) & (~READ_LINE_IN2) &\
												(~READ_LINE_IN3) & (~READ_LINE_IN4))
#define  LINE2_INPUT		((~READ_LINE_IN1) & (READ_LINE_IN2) &\
												(~READ_LINE_IN3) & (~READ_LINE_IN4))
#define  LINE3_INPUT		((~READ_LINE_IN1) & (~READ_LINE_IN2) &\
												(READ_LINE_IN3) & (~READ_LINE_IN4))
#define  LINE4_INPUT		((~READ_LINE_IN1) & (~READ_LINE_IN2) &\
												(~READ_LINE_IN3) & (READ_LINE_IN4))


extern void marix_keyboard_init(void);
extern unsigned char key_ReadInputNumber(void);
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
