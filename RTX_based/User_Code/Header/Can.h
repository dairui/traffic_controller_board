#ifndef __CAN_H__
#define __CAN_H__


/*CAN����֡��ʽ*/
#define IPI						0x20
#define MSG_CONTINUE		0xFD      //CAN����֡���ӱ�־
#define MSG_END					0xFE			//CAN����֡������־



#define MSG_END_2                  	(0x02)              //CAN����֡������־
#define MSG_CONTINUE_2            (0x55)              //CAN����֡���ӱ�־
//extern CanRxMsg CAN_Rx_MSG[4][3];
//extern CanRxMsg CAN_Rx_MSG_Db[4][3];
extern u8 CAN_IT_SET;//can�жϴ�����־
//extern u8 Lamp_Check_Flag;//��״̬У��
//extern u8 Can_DB_Update;//can�������ݿ�

//#define CAN_RX_TIMEOVER		(200)	//��λms
//extern u8 CAN_IT_COUNT;//can ���յ���һ�����ݺ�ʼ��ʱʱ��
//extern u8 CAN_IT_TIMEOVER_Flag;//CAN���ճ�ʱ��ʶ



void CAN_TX(u8 *data);
//void CAN_TX_DATA(CanTxMsg* TxMessage);
void CAN_ERROR_TX(u8 data1,u8 data2);
void CANTX_Lamp_Check(u8 data1,u8 data2, u8 data3);
void CAN_IT_Receive(void);//CAN������λ������
#endif

