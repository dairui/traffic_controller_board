
#ifndef	_SYSTEM_CONTROL_H
#define	_SYSTEM_CONTROL_H

u8  Find_Now_Hour_Minute(void);

//__task void System_Control_Task(void);
void Find_Today_Fangan(void);
void Find_Today_Fangan_Time(void);
void Order_Today_Fangan_Time(void);//排序一天要执行的方案
void Find_Start_Hour_Minute(void);
u8  Find_Now_Hour_Minute(void);


//extern	u8 	Today_TimeStage_Number;			//今日阶段号
//extern	u8 	Today_Time_Stage_Index[40];		//今日方案号索引
// extern	u8 	Today_Fangan_Hour[16];			//今日方案开始执行小时
// extern	u8 	Today_Fangan_Minute[16];			//今日方案开始执行分钟
// extern	u8 	Start_0xC0_Index;
// extern	u8 	Today_0xC0_Index[32];			//配时方案索引
// extern	u8 	Start_Fangan_Hour_Index;			//第一个执行方案小时的索引
// extern	u8 	Start_Fangan_Minute_Index;		//第一个执行方案分钟的索引
//extern	u8 	Today_Control_Mode[16] ;			//0x8E表第五行控制方式

extern	u8 	Fangan_Hour;
extern	u8	Fangan_Min;
#endif
