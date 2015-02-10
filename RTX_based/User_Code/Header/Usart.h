#ifndef USART_H
#define USART_H
void USART_Tx_Byte(USART_TypeDef* USARTx, u16 data);
void USART_Tx_nByte(USART_TypeDef* USARTx, u8 *address, u16 n);
void UART4_Tx_nByte(u8 *address, u16 n);
void UART4_Tx_Byte(u16 data);

#endif

