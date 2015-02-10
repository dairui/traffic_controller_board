#ifndef __CAN_H__
#define __CAN_H__


/*CAN发送帧格式*/
#define IPI						0x20
#define MSG_CONTINUE		0xFD      //CAN数据帧连接标志
#define MSG_END					0xFE			//CAN数据帧结束标志



#define MSG_END_2                  	(0x02)              //CAN数据帧结束标志
#define MSG_CONTINUE_2            (0x55)              //CAN数据帧连接标志
//extern CanRxMsg CAN_Rx_MSG[4][3];
//extern CanRxMsg CAN_Rx_MSG_Db[4][3];
extern u8 CAN_IT_SET;//can中断触发标志
//extern u8 Lamp_Check_Flag;//灯状态校验
//extern u8 Can_DB_Update;//can更新数据库

//#define CAN_RX_TIMEOVER		(200)	//单位ms
//extern u8 CAN_IT_COUNT;//can 接收到第一个数据后开始记时时间
//extern u8 CAN_IT_TIMEOVER_Flag;//CAN接收超时标识



void CAN_TX(u8 *data);
//void CAN_TX_DATA(CanTxMsg* TxMessage);
void CAN_ERROR_TX(u8 data1,u8 data2);
void CANTX_Lamp_Check(u8 data1,u8 data2, u8 data3);
void CAN_IT_Receive(void);//CAN接收相位板数据
#endif

