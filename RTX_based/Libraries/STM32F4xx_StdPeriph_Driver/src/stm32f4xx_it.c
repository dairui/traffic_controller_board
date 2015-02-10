/**
  ******************************************************************************
  * @file    CAN/LoopBack/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "Include.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CAN_LoopBack
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t ret;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
//	#ifdef LIMING_DEBUG_12_23
		printf("\nxxxxxxxxxxx		Hard Fault 		xxxxxxxxxxxx\n");
//	#endif		
  while (1)
  {
  }
}
// hard fault handler in C,  
// with stack location as input parameter  
void hard_fault_handler_c(unsigned int * hardfault_args)  
{  
unsigned int stacked_r0;  
unsigned int stacked_r1;  
unsigned int stacked_r2;  
unsigned int stacked_r3;  
unsigned int stacked_r12;  
unsigned int stacked_lr;  
unsigned int stacked_pc;  
unsigned int stacked_psr;  

stacked_r0 = ((unsigned long) hardfault_args[0]);  
stacked_r1 = ((unsigned long) hardfault_args[1]);  
stacked_r2 = ((unsigned long) hardfault_args[2]);  
stacked_r3 = ((unsigned long) hardfault_args[3]);  

stacked_r12 = ((unsigned long) hardfault_args[4]);  
stacked_lr = ((unsigned long) hardfault_args[5]);  
stacked_pc = ((unsigned long) hardfault_args[6]);  
stacked_psr = ((unsigned long) hardfault_args[7]);  
  
printf ("[Hard fault handler]\n");  
printf ("R0 = %x\n", stacked_r0);  
printf ("R1 = %x\n", stacked_r1);  
printf ("R2 = %x\n", stacked_r2);  
printf ("R3 = %x\n", stacked_r3);  
printf ("R12 = %x\n", stacked_r12);  
printf ("LR = %x\n", stacked_lr);  
printf ("PC = %x\n", stacked_pc);  
printf ("PSR = %x\n", stacked_psr);  
printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));  
printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));  
printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));  
printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));  
printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));  

    
while(1) 
{ 
        ;; 
} 
  
}  

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
//	#ifdef LIMING_DEBUG_12_23
		printf("\nxxxxxxxxxxx		Memory Manage 		xxxxxxxxxxxx\n");
//	#endif	
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */

//	#ifdef LIMING_DEBUG_12_23
		printf("\nxxxxxxxxxxx		Bus Fault 		xxxxxxxxxxxx\n");
//	#endif
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
//	#ifdef LIMING_DEBUG_12_23
		printf("\nxxxxxxxxxxx		Usage Fault		xxxxxxxxxxxx\n");
//	#endif

  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*void SVC_Handler(void)
{
}*/

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*void PendSV_Handler(void)
{
}*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
	u8 i = 0;
	i++;
}*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void PPP_IRQHandler(void)
{
}

/*串口2接收中断函数*/
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
	//接收中断
   	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
   	{ 
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
// // 			Eth_ControlBlk.eth_rx[Eth_ControlBlk.rx_count++] = USART_ReceiveData(USART2);
// // 			
// // 			uart2_rx_time_count = 0;
			
// // 			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
// // 			uart2_rx_buff[uart2_rx_len++] = USART_ReceiveData(USART2);			
// // 			uart2_rx_time_count = 0;
			
   	}
	//发送中断
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
// // 		USART_SendData(USART2, Usart_Int_TX[Usart_Int_TX_Count++]);
// // 		if (Usart_Int_TX_Count >= Usart_Int_Send_Length)
// // 		{
// // 			//发送字节结束
// // 			Usart_Int_Send_Length = 0;
// // 			Usart_Int_TX_Count = 0;
// // 			USART_ClearITPendingBit(USART2,USART_IT_TXE);
// // 			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
// // 			USART_ITConfig(USART2, USART_IT_TC, ENABLE);
// // 		}
	}
	//发送完成
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_TC);
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	}
	
	//USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	NVIC_ClearPendingIRQ(USART2_IRQn);
}


/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void CAN1_RX0_IRQHandler(void)
//{
	
//}


/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern OS_TID t_matrix_keyboard;                        	/* assigned task id of task: matix_keyboard */
void EXTI9_5_IRQHandler(void)
{
	u8 *msg;
	static u8 Sche_Dispatch_MSG=3;
	
	if(EXTI_GetFlagStatus(EXTI_Line5) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
		Wether_Change_Fangan_Alarm = 1;

		//Sche_Dispatch_MSG[0] = 3;
		msg = &Sche_Dispatch_MSG;

 		isr_mbx_send(Sche_Dispatch_Mbox, msg);
		
		//isr_mbx_send(Sche_Dispatch_Mbox, msg);
	}

	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);       
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern OS_TID t_task_send_CAN;
extern os_mbx_declare(Sche_Output_Mbox, 		20);	

extern	vu32 optimize_time_count ;     		//优化时长计数器   //debug_lm
extern	vu32 optimize_time_flag;
#define  OPTIMIZE_TIMES   (4*60*15)   //   250ms时基，优化时长为15m。
extern vu32 start_time_count;
void TIM2_IRQHandler(void)
{	

	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) 
	{
		 TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
// // 		 start_time_count++;
// // 		 if(start_time_count >=255)start_time_count=0;
	}
	
}



/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

/*开机黄闪全红标志，时间*/
extern U8 start_yellow_flag;
extern U8 start_red_flag;
extern U8 start_yellow_time;
extern U8 start_red_time;

U32 heart_cnt = 0;
void TIM3_IRQHandler(void)
{	
	u8 *msg;

	static u8 kk = 3;
	

	static u16 time_250ms_count = 0;
	static u16 time_100ms_count = 0;
	
	static u32  ddd=0;
	u32 *msg_to_sche_out;
	
	u8 *msg_to_dispatch =NULL;
	static u8 to_sche_dispatch_MSG[2]={2,0};	
	
	static U16 yellow_cnt = 0;
	static U16 red_cnt = 0;

	u8 *msg_start;
	static u8 Sche_Dispatch_MSG=3;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update)) 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		
		//heart
		if(heart_cnt<=ETH_CLIENT_TIC_TIMES)
		{
			heart_cnt++;
		}
		else
		{
			heart_cnt = 0;
			Eth_client_tic_time_ovr = 1;
		}
		//开机黄闪全红计数
		if(start_yellow_flag)
		{
			if(yellow_cnt<=(start_yellow_time*1000))
				yellow_cnt++;
			else
			{
				start_yellow_flag = (__FALSE);
				msg_start = &Sche_Dispatch_MSG;

				isr_mbx_send(Sche_Dispatch_Mbox, msg_start);
			}
				
		}
		if(start_red_flag)
		{
			if(red_cnt<=(start_red_time*1000))
				red_cnt++;
			else
			{
				start_red_flag = (__FALSE);
				msg_start = &Sche_Dispatch_MSG;

				isr_mbx_send(Sche_Dispatch_Mbox, msg_start);
			}
		}
		
		if(uart4_rx_time_count == UART_RX_OVER_TIMES)  //  now  (UART_RX_OVER_TIMES = 12ms)
		{
			uart4_rx_time_count++;
			uart4_rx_time_count++;  // 此后 uart4_rx_time_count > 2 + UART_RX_OVER_TIMES 
															// 确保了  isr_mbx_send()   只执行一次
			uart4_rx_time_over=1;
			
			msg = &kk;
			isr_mbx_send(DB_Update_Mbox,(void*) msg);
	  }
		else if(uart4_rx_time_count<UART_RX_OVER_TIMES)
						uart4_rx_time_count++;		
	
		if(time_250ms_count > 250)
		{
			time_250ms_count=0;
			//=============================  以前TIM2的部分拿这来用

			Channel_Output_Int_Flag++;
			ddd=0;			
			msg_to_sche_out= &ddd;
			isr_mbx_send(Sche_Output_Mbox,msg_to_sche_out);   

			//LXB
			if (optimize_time_flag == 1)
			{
				optimize_time_count ++;
				if(optimize_time_count == OPTIMIZE_TIMES)
					{
						optimize_time_flag = 0;
						optimize_time_count = 0;
						
						to_sche_dispatch_MSG[0]=2;
						to_sche_dispatch_MSG[1]=0;
						msg_to_dispatch = to_sche_dispatch_MSG;
						isr_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch);
					}
			}

		//=============================
		}
		else
		{
			time_250ms_count++;

		}	

		/*led*/
		if(time_100ms_count>100)
		{
			time_100ms_count=0;
			ddd = 0;
			msg_to_sche_out= &ddd;
			isr_mbx_send(mailbox_led,msg_to_sche_out);   		
		}
		else
		{
			time_100ms_count++;
		}		
	}
	
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update)) 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
	TIM_ClearITPendingBit(TIM4, TIM4_IRQn);  
	NVIC_ClearPendingIRQ(TIM4_IRQn);
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
	u8 i;
	static u8 send_temp[2]={0};
	//u8 *msg_send;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update)) 
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		
		if(Optimize_Data.Optimize_time > 0)
			Optimize_Data.Optimize_time --;
		if(Optimize_Data.Optimize_time == 1)//优化时间到
		{
// 			CTLR_TYPE1_LED_OFF();
			send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED序号
			send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
			isr_mbx_send(mailbox_led,(void*)send_temp);
			
			Optimize_Data.Period_Time_1 = 0;
			Optimize_Data.Phase_Difference_1 = 0;
			for(i = 0; i <= 15; i++)
				Optimize_Data.Stage_Time_1[i] = 0;
			for(i = 0; i <= 15; i++)
				Optimize_Data.Key_Phase_Time_1[i] = 0;
		}
	}
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ClearITPendingBit(TIM5, TIM5_IRQn);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
