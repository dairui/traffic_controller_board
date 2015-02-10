#include "include.h"
//=========================================================================
//CAN_TX_DATA(CanTxMsg* TxMessage)
//CAN发送应用函数
//=======================================================================
void CAN_TX(u8 *data)
{
 	CAN_msg TxMessage;
 	TxMessage.id = 0x00;
 	TxMessage.format = STANDARD_FORMAT;
 	TxMessage.type = DATA_FRAME;
 	TxMessage.len = 0x08;
 	TxMessage.data[0] = data[0];
 	TxMessage.data[1] = data[1];
 	TxMessage.data[2] = data[2];
 	TxMessage.data[3] = data[3];
 	TxMessage.data[4] = data[4];
 	TxMessage.data[5] = data[5];
 	TxMessage.data[6] = data[6];
	TxMessage.data[7] = data[7];
 	TxMessage.ch = 1;

	/* 	
	
	下面这个延时不能等于和大于125ms，
	原因如下：
	方案输出任务的定时周期是250ms，
	方案输出也需要一定的时间，而can等待的时间接近125ms的话，
	两次的输出就接近250ms，如此累积下去，方案输出任务就会累积大量未处理的消息
	因此存在消息溢出的可能
	*/ 
 	CAN_send(1, &TxMessage, 80);  //  80ms
}

