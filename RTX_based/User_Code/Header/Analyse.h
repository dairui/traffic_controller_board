#ifndef 	_ANALYSE_H
#define	_ANALYSE_H
extern	u8                   Flag_NetTest;             //�����������
extern	u8                   Flag_EthProtocol;             //USARTͨ�Ž�����־
extern	u8			Com_Type;		//ͨѶ���ͣ�1:ETH;2:CAN;3:USART;4:IIC;
//void ETH_Operat(u8 operat_type);
u8 RTC_Get_Week(u16 year, u8 month, u8 day);
void ETH_Send_Time(u8 *rx_buffer, u8 operat_type);
u8 Is_Leap_Year(u16 year);
u32 Read_DS3231_Time(void);

#endif

