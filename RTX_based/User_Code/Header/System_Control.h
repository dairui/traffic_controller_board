
#ifndef	_SYSTEM_CONTROL_H
#define	_SYSTEM_CONTROL_H

u8  Find_Now_Hour_Minute(void);

//__task void System_Control_Task(void);
void Find_Today_Fangan(void);
void Find_Today_Fangan_Time(void);
void Order_Today_Fangan_Time(void);//����һ��Ҫִ�еķ���
void Find_Start_Hour_Minute(void);
u8  Find_Now_Hour_Minute(void);


//extern	u8 	Today_TimeStage_Number;			//���ս׶κ�
//extern	u8 	Today_Time_Stage_Index[40];		//���շ���������
// extern	u8 	Today_Fangan_Hour[16];			//���շ�����ʼִ��Сʱ
// extern	u8 	Today_Fangan_Minute[16];			//���շ�����ʼִ�з���
// extern	u8 	Start_0xC0_Index;
// extern	u8 	Today_0xC0_Index[32];			//��ʱ��������
// extern	u8 	Start_Fangan_Hour_Index;			//��һ��ִ�з���Сʱ������
// extern	u8 	Start_Fangan_Minute_Index;		//��һ��ִ�з������ӵ�����
//extern	u8 	Today_Control_Mode[16] ;			//0x8E������п��Ʒ�ʽ

extern	u8 	Fangan_Hour;
extern	u8	Fangan_Min;
#endif
