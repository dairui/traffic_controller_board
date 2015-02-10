#include "include.h"
//=========================================================================
//CAN_TX_DATA(CanTxMsg* TxMessage)
//CAN����Ӧ�ú���
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
	
	���������ʱ���ܵ��ںʹ���125ms��
	ԭ�����£�
	�����������Ķ�ʱ������250ms��
	�������Ҳ��Ҫһ����ʱ�䣬��can�ȴ���ʱ��ӽ�125ms�Ļ���
	���ε�����ͽӽ�250ms������ۻ���ȥ�������������ͻ��ۻ�����δ�������Ϣ
	��˴�����Ϣ����Ŀ���
	*/ 
 	CAN_send(1, &TxMessage, 80);  //  80ms
}

