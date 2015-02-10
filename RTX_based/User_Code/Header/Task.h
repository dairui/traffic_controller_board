#ifndef _TASK_H
#define _TASK_H

#define PHASET_ARR_NO 48
#define ETH_CLIENT_TIC_TIMES  (1000*60*5)//   1min 心跳时间
/*		lxb2112.12.29	*/
#define NOW_SECOND		0
#define NOW_MINUTE		1
#define NOW_HOUR			2
#define NOW_WEEK			3
#define NOW_DAY				4
#define NOW_MONTH			5
#define NOW_YEAR			6

/*		lxb2112.12.29	*/


/////lxb2013 1.3///////////////////
/*
日期时间结构体
*/
typedef struct TIME_S
{
	US8 	second;
	US8 	minute;
	US8 	hour;
	US8 	week;
	US8	  day;
	US8		month;
	US8		year;
}time_struct;
///////////////////////////lxb1.3
typedef struct
{
    uint32_t bit0:1;
    uint32_t bit1:1;
    uint32_t bit2:1;
    uint32_t bit3:1;
    uint32_t bit4:1;
    uint32_t bit5:1;
    uint32_t bit6:1;
    uint32_t bit7:1;
    uint32_t bit8:1;
    uint32_t bit9:1;
    uint32_t bit10:1;
    uint32_t bit11:1;
    uint32_t bit12:1;
    uint32_t bit13:1;
    uint32_t bit14:1;
    uint32_t bit15:1;
    uint32_t bit16:1;
    uint32_t bit17:1;
    uint32_t bit18:1;
    uint32_t bit19:1;
    uint32_t bit20:1;
    uint32_t bit21:1;
    uint32_t bit22:1;
    uint32_t bit23:1;
    uint32_t bit24:1;
    uint32_t bit25:1;
    uint32_t bit26:1;
    uint32_t bit27:1;
    uint32_t bit28:1;
    uint32_t bit29:1;
    uint32_t bit30:1;
    uint32_t bit31:1;
}BIT_BAND_FLAG __attribute__((bitband));

extern BIT_BAND_FLAG bit_band_flag_dog;  //  位带标志位定义区 看门狗

//================   任务运行与运行结束标志标志   start  ==============
#define tsk_tcp_run      			(bit_band_flag_dog.bit0)   	// tcp_tsk运行
#define tsk_log_run       		(bit_band_flag_dog.bit1) 	  // log_write_task运行
#define tsk_DB_run    				(bit_band_flag_dog.bit2)   	// eth_usart_communication_task运行 	
#define tsk_dispatch_run    	(bit_band_flag_dog.bit3)   	// ADC_task运行 
#define tsk_data_run    			(bit_band_flag_dog.bit4)   	// learn_detect_task运行  
#define tsk_execute_run					(bit_band_flag_dog.bit5)   	// power_detect_task运行 
#define tsk_CAN_run					(bit_band_flag_dog.bit6)   	// power_detect_task运行 
#define tsk_output_run					(bit_band_flag_dog.bit7)   	// power_detect_task运行 

#define tsk_tcp_over    			(bit_band_flag_dog.bit8)   	// tcp_tsk运行 完成 
#define tsk_log_over    			(bit_band_flag_dog.bit9)   	// log_write_task运行	完成
#define tsk_DB_over    				(bit_band_flag_dog.bit10)   	// eth_usart_communication_task运行	完成
#define tsk_dispatch_over    	(bit_band_flag_dog.bit11)   	// ADC_task运行 
#define tsk_data_over   			(bit_band_flag_dog.bit12)   	// learn_detect_task运行  
#define tsk_execute_over			(bit_band_flag_dog.bit13)   	// power_detect_task运行 
#define tsk_CAN_over					(bit_band_flag_dog.bit14)   	// power_detect_task运行 
//================   任务运行与运行结束标志标志    end  ==============
#define bFeed_Extern_Dog			(bit_band_flag_dog.bit15)   	// 外部看门狗喂狗标识

///////////////////////////lxb1.13
/*
16通道输出状态表给id=0xb1的输出表赋值用途
*/
typedef struct CHANNEL_OUTPUT_TOB1_S
{
	US8			index;							//	行号
	US16		redStat;					//16通道的红灯输出状态
	US16		yellowStat;				//16通道的黄灯输出状态
	US16		greenStat;				//16通道的绿灯输出状态
}CHANNEL_OUTPUT_TOB1_t;
/*
16相位输出状态表行数 id=0x96
4*2 = 8B
*/
typedef struct PHASE_OUTPUT_16STAT_S
{
	US8		  statId;				//相位状态组的行号
	US16		redStat;				//相位的红灯输出状态
	US16		yellowStat;		//相位的黄灯输出状态
	US16		greenStat;			//相位的绿灯输出状态
}PHASE_OUTPUT_16STAT_t;
/*
阶段配时计数表，由id=0xC1转换而来
16*16*8 = 2K
*/
typedef struct STAGE_TIMING_COUNT_S
{
	US8			index;					//	阶段配时方案号
	US8			stageId;					//阶段号
	US16		relPhase;				//放行相位2B
	US16		stGreenCount[16];		//阶段16通道绿灯计数
	US16		stYellowCount[16];	//阶段16通道黄灯计数
	US16		stRedCount[16];			//阶段16通道红灯计数
	US8			stAlterParam;		//阶段选项参数
	US8			control_way;			//控制方式
}STAGE_TIMING_COUNT_t; 

/*跟随相位c8*/
/*
跟随相位参数衰行数id=0xC8
8*39 = 312B
*/
typedef struct FOLLOW_PHASE_WORKING_S
{
	US8		phaseId;				//跟随相位表号
	US8		operationType;	//操作类型
	US8		incPhaseLenth;	//包古相位长度
	US8		incPhase[16];		//包含相位
	US8		fixPhaseLenth;		//修正相位长度
	US8		fixPhase[16];		//修正相位
	US16		tailGreenCnt;				//尾部绿灯计数
	US16		tailYellowCnt;				//尾部黄灯计算
	US16		tailRedCnt;					//尾部红灯计数
}FOLLOW_PHASE_WORKING_t;

#define C8_LINE 8
///////////////////////////lxb1.13

extern	u8	Sche_Dispatch_MSG[5];			//方案调度任务消息
extern	u8	Sche_Execute_MSG[5];			//方案执行任务消息
extern	u8	Sche_Data_Update_MSG[5];	//方案数据任务
extern	u8	Sche_Interim_MSG[5];			//过渡方案
extern	u8	Degrade_MSG	[5];					//降级任务消息

extern OS_ID  tmr1_yellow_flicker;

extern	OS_SEM Induction_Sem;
extern	OS_SEM Bus_Induction_Sem;
extern	OS_SEM Ped_Induction_Sem;

extern	OS_MUT	Mutex_Data_Update;
extern	OS_MUT	Tab_B1_Mutex;


extern	os_mbx_declare(DB_Update_Mbox, 		20);
extern	os_mbx_declare(mailbox_led, 		20);
extern	os_mbx_declare(Write_Log_Mbox, 		20);
extern	os_mbx_declare(Sche_Dispatch_Mbox,20);//方案调度任务
extern	os_mbx_declare(Sche_Data_Update_Mbox, 20);//方案数据任务
extern	os_mbx_declare(Sche_Execute_Mbox, 20);//方案执行任务
//extern	os_mbx_declare(Sche_Interim_Mbox, 20);//过渡方案任务

//extern	os_mbx_declare(Degrade_Mbox, 			20);						//降级	任务
//------------------------------------------------
//extern	os_mbx_declare(Db_wr_re_mbox, 				20);						//数据库 任务

//------------------------------------------------
extern	OS_MUT	Output_Data_Mut;


extern	OS_TID	t_maintask;
extern	OS_TID	t_DB_Update_Task;
extern	OS_TID	t_Write_Log_Task;
extern	OS_TID	t_Induction_Task;//感应任务
// extern	OS_TID	t_Bus_Induction_Task;//公交感应任务  //no_liming
// extern	OS_TID	t_Ped_Induction_Task;//行人感应任务//no_liming
extern	OS_TID	t_Sche_Dispatch_Task;//方案调度任务
extern	OS_TID	t_Sche_Data_Update_Task;//方案数据任务
extern	OS_TID	t_Sche_Execute_Task;//方案执行任务
// extern	OS_TID	t_Stagecount_Input_Task;//(计数器任务)
extern	OS_TID	t_Sche_Output_Task;//方案输出(计数器任务)
//extern	OS_TID 	t_Sche_Interim_Task;//过渡方案

extern	OS_TID	t_Degrade_Task;							//降级任务

extern OS_TID t_tick_timer; 
extern OS_TID t_tcp_task; 

extern vu8 Eth_client_tic_time_ovr;





#define	Light_Off		5
#define	Yellow_Flick	6
#define	All_Red		7
__task void init (void);
__task void maintask (void);
__task void DB_Update_Task(void);
__task void Write_Log_Task(void);
__task void Induction_Task(void);
__task void Bus_Induction_Task(void);
__task void Ped_Induction_Task(void);
__task void Sche_Dispatch_Task(void);//2方案调度任务
__task void Sche_Data_Update_Task(void);//3方案数据任务
__task void Sche_Execute_Task(void);//5方案执行任务
// __task void Stagecount_Input_Task(void);//计数器任务
__task void Sche_Output_Task(void);//6方案输出任务(计数器任务)
//__task void Sche_Interim_Task(void);//过渡方案任务
__task void led (void) ;
__task void Degrade_Task(void);//降级任务
__task void Db_read_or_write_task(void);
__task void matrix_keyboard (void);

__task void tcp_task(void);
__task void tick_timer(void);

__task void task_send_CAN(void);

__task void task_rece_CAN (void)  ;

void Phase_Output_Int(u8 in_type);
TIME_TABLE_t Find_Scheme(time_struct *current_time);
void System_Optimize(void);
void ETH_Operat(u8 operat_type);

#endif

