
#include "include.h"
//����

//======================================================================
// �������ƣ�
// USART_Tx(u8 data)
//			 
//˵����USART1���ݷ���
//======================================================================
void USART_Tx_Byte(USART_TypeDef* USARTx, u16 data)
{           
	//USART_SendData(USARTx, data);

// 	Usart_Int_TX[Usart_Int_Send_Length++] = data;
// 	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);  
	//while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET); //�ȴ��������
	//USART_ClearFlag(USARTx, USART_FLAG_TC);

}

//======================================================================
// �������ƣ�
// USART1_Tx_nByte(u8 data)
//			 
//˵����USART1���ݷ��Ͷ������
//======================================================================
void USART_Tx_nByte(USART_TypeDef* USARTx, u8 *address, u16 n)
{    
// 	u16 i = 0;
// 	Usart_Int_Send_Length = 0;
// 	for(i=0; i<n; i++)
// 	{
// 	    Usart_Int_TX[Usart_Int_Send_Length++] = address[i];
// 	}
// 	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);  
}

extern DMA_InitTypeDef    DMA_InitStructure;
extern uint8_t Uart4_Tx_Buffer[10];
extern __IO uint8_t Uart4_Tx_DMA_Ovr;
//======================================================================
// �������ƣ�
// UART4_Tx_nByte(u8 data)
//			 
//˵����UART4���ݷ��Ͷ������
//======================================================================
void UART4_Tx_nByte(u8 *address, u16 n)   //   UART4_LM
{    
// 	uint16_t i=0;
// 	
// 	while(Uart4_Tx_DMA_Ovr !=__TRUE);  //  �ȴ�DMA�������
// 	
// 	for(i=0;i<n;i++)
// 	{
// 		Uart4_Tx_Buffer[i]=address[i];
// 	}
// 	DMA_DeInit(DMA1_Stream4);
// 	DMA_InitStructure.DMA_BufferSize = n;
// 	DMA_Init(DMA1_Stream4, &DMA_InitStructure);  //  for USART4
// 	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
// 	DMA_Cmd(DMA1_Stream4, ENABLE);

// 	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);	
// 	
// 	Uart4_Tx_DMA_Ovr=__FALSE;
}


