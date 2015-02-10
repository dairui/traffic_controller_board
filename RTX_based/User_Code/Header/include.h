
#define RX_BUFSIZE          (400)                      //定义接收缓冲区最大长度
#define TX_BUFSIZE          (400)                      //定义发送缓冲区最大长度,单日志最大长度


#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include "RTL.h"				  /* RTX kernel functions & defines      */
#include "RTX_CAN.h"                  /* CAN Generic functions & defines     */
//#include "ucos_ii.h"
#include <Net_Config.h>
#include "ETH_STM32F4xx.h"
#include "Device_Init.h"
#include "Usart.h"
#include "Can.h"
#include "spi.h"
#include "flash.h"
#include "dBase.h"
#include "I2C.h"
#include "Task.h"
#include "Schedule.h"
#include "First_Start.h"
#include "System_Control.h"
#include "Analyse.h"
#include "Guodu.h"
#include "Collide.h"
#include "led.h"
#include "marix_keyboard.h"


/*

#include "Global.h"

#include	"Find_Scheme_Number.h"

#include "DB_Update_Task.h"
#include "Execute_Task.h"
#include	"Hand_Control_Task.h"
#include "Yellow_Flicker.h"
#include	"Optimize_Control_Task.h"


#include "main.h"
#include "Analysis.h"


#include "stm32_eth.h"
#include "netconf.h"
#include "helloworld.h"
#include "httpd.h"
#include "tftpserver.h"
#include "ethernet_add.h"
#include "tcp.h"
*/

#include "math.h"
#include "string.h"
#include "stdio.h"




#define CLI()      __set_PRIMASK(1)  
#define SEI()      __set_PRIMASK(0)  






extern u32 Flag_Login;

#define STM3240G_EVAL_Self                   
#define Channel_Max 		16			//最大相位数
#define Channel_Max_Define (Channel_Max + 1)


// // #define RX_BUFSIZE          (400)                      //定义接收缓冲区最大长度
// // #define TX_BUFSIZE          (400)                      //定义发送缓冲区最大长度,单日志最大长度


extern u32 TimingDelay;
extern u32	LocalTime; 
//相位板灯状态
// extern u8 Red_Lamp_Status[Channel_Max/2+1];//红灯状态
// extern u8 Yellow_Lamp_Status[Channel_Max/2+1];//黄灯状态
// extern u8 Green_Lamp_Status[Channel_Max/2+1];//绿灯状态
//extern u16 ADC_result[Channel_Max/2 + 1][7];//相位板电流值
//dbase
 //数据库参数
//extern u8 Read_Result_A[24];//读取单个子对象值，单个属性最大字节24 字节
//extern u8 Read_Result_B[24][24];//读取某行存放数组
//extern u8 Out_Data[2000];//读数据库得到数据存放地方

//故障记录——故障标志
//故障、时间记录
//=============故障、事件，直接黄闪

#define	Type_97			1			//相位冲突0x97

#define	Type_A7			3			//信号及报警0xA7
#define	Type_A8			4			//本地控制方式0xA8
#define	Type_A9			5			//紧急报警0xA9
#define	Type_E1			6			//相位板红灯状态
#define	Type_E2			7			//相位板黄灯状态
#define	Type_E3			8			//相位板绿灯状态
#define	Type_E4			9			//可控硅状态
#define	Type_A6			10	    //黄闪板状态

//#define	Working_Without_Plan		41	//无方案执行
//#define	Lamp_Output_Unmatched	42	//灯状态与输出状态表不匹配


//==============故障、事件，降级运行
//#define	Cpu_Commu_Error		61	//和上位机通信故障
//#define	Phase_Commu_Error		62	//和相位板通信故障
#define	Comm_Warn				13	//通信报警0xDC
//#define	RTC_Error				81	//时钟故障
//#define	Plan_Para_Error			82	//方案参数异常
#define	Run_Warn				14	//信号机运行报警0xDD
//#define	Detector_Unknown_Error	101	//检测器未知故障
//#define	Detector_Set_Error		102	//检测器配置故障
//#define	Detector_Commu_Error	103	//检测器通信故障
//#define	Detector_Unstable			104	//检测器不稳定
//#define	Detector_TooLong			105	//检测器存在时间过长
//#define	Detector_Inactive			106	//检测器不活动
#define	Detect_Warn				11	//检测器报警0xA2
//#define	Detect_Out_Range			121	//感应变化量超限
//#define	Inductance_Unenough		122	//电感不足
//#define	Coil_Open				123	//线圈开路
//#define	Watchdog_Fault			124	//Watchdog故障
//#define	Coil_Unknown_Error		125	//感应线圈未知故障
#define	Coil_Warm				12	//感应线圈报警0xA2

//===============事件
//#define	Msg_Too_Long			141	//消息长度太长
//#define	Msg_Type_Error			142	//消息类型错误
//#define	Index_Subobj_Error		143	//索引和子对象错误
//#define	Obj_Set_Out_Range		144	//设置的对象值超出范围
//#define	Msg_Too_Short			145	//消息长度太短
//#define	Msg_CANNOT_Decod		146	//不能正确解码
#define	Msg_Analy_Warm			27	//数据解析报警0xDE
//#define	EEPROM_Read_Error		151	//读EEPROM数据出错
//#define	Lamp_Current_Error		152	//信号灯电流波动异常
#define	Run_Output_Warn			28	//信号机轻故障0xDF
//#define	Unknown_Mode			161	//未知模式
//#define	System_Ctrl				162	//系统协调控制
//#define	Host_Ctrl					163	//主机协调控制
//#define	Manual_Ctrl 				164	//手动面板控制
//#define	Plan_Period_Ctrl			165	//时段表控制（按本地时段表运行）
//#define	Cable_Coordination_Ctrl	166	//有缆协调
//#define	Guard_Ctrl				167	//看守
#define	Sys_Ctrl_Type			21	//系统控制方式0xA5
//#define	Induction_Ctrl				168	//感应
//#define	Sys_Optimize_Ctrl			169	//系统优化
//#define	Single_Point_Optim_Ctrl	170	//单点优化
//#define	Sys_Multiperiod_Ctrl		171	//系统多时段
//#define	Yel_Flash				172	//黄闪
//#define	Red_All					173	//全红
//#define	Lamp_All_Off				174	//灭灯
//#define	Step_Ctrl					175	//步进
#define	Ctrl_Mode				26	//控制方式0xA5
//#define	Unknown_Flash			181	//未知的闪光
//#define	Not_Flash				182	//当前不是处于闪光控制状态
//#define	Auto_Yel_Flash 			183	//自动黄闪控制
//#define	Local_Manual_Yel_Flash	184	//本地手动黄闪
//#define	Error_Monitor_Yel_Flash	185	//故障监视引起的黄闪
//#define	Signal_Collide_Yel_Flash	186	//信号冲突引起的黄闪
//#define	 System_Start_Yel_Flash	187	//启动时的闪光控制

//#define	System_Stopping_Run		201	//信号机即将停止运行
//#define	Battery_Runtime_Low		202	//电池放电时间过低
//#define	System_Restart 			203	//信号机重启
//#define	System_Start				204	//信号机开机
//#define	Cabinet_Door_State		205	//机柜门开关状态
//#define	System_Landing			206	//系统登录
//#define	Plan_Change_Set 			207	//方案设置修改

//#define	Coor_Active				211	//正处于协调控制状态
//#define	Local_Free				212	//本地单点控制
//#define	Local_Flash				213	//本地闪光控制
//#define	MMU_Flash				214	//MMU引起的闪光，并且长于Start_Up闪光时间

//#define	Emergency_Alarm			221	//紧急报警
//#define	Non_Emergency_Alarm		222	//非紧急报警
//#define	Detector_Error			223	//监测器故障
//#define	Force_Local_Control		224	//强制本地控制
//#define	T_F_Flash				225	//T&F Flash



extern Induction_Str Schedule_Struct[Channel_Max_Define];//方案模块结构体
extern   u16 System_Single_Optimize_Set;//系统优化或单点优化标志，0，没优化，1，优化
extern  u8 Fangan_Running;//正在执行方案
//extern u8 Control_Type_8E[16];
extern  u8 mark_Plan;//控制方式数组标号
////邮箱传送数组
//副cpu串口中断数据
// extern u8 ViceCpu_Data_Flag ;//副CPU数据中断标志
// extern u8 ViceCpu_Data[100] ;//数据缓冲区
// extern u8 ViceCpu_Data_Index ;//数据缓冲区索引
extern u8 Pedestrian_Trigger_Flag ;//行人触发标志
extern u8 Bus_Trigger_Flag ;//公交触发标志
extern u8 Motor_Trigger_Flag ;//机动车触发标志
extern u8 Induction_Trigger_Flag ;

extern u8 Usart_Rx_Count;//串口接收计数器计时
extern u8 Vice_Usart_Rx_Count;//串口接收计数器计时


extern u8 END_Yellow_Flicker;//黄闪结束设置

extern u8 Hold_Stage_Num;//看守阶段号

extern u8 Status_Current_New;//当前新状态
extern u8 Status_Current_Old;//之前所处状态

//邮箱任务发送数据专用缓存
extern u8 Write_Log_Msg[21];
//extern u8 Mbx_Error_Handle[5];
extern u8 Mbx_DB_Update[5];
extern u16	Test_Count;
extern u8 End_Int_Handle_Flag;//中断异常控制标志

/****ETH Par******/
extern U8 server_ip[4];            //服务端IP
extern u16 server_port;                //服务端PORT
extern u16 local_port;            //本地服务端口号
extern U8 tcp_soc_server;       //服务端TCP
extern u8 tcp_soc_client;       //客户端TCP
extern u8 eth_send_data_flag ;//以太网发送数据标志
extern u8 client_data_type;
extern u8 cli_connect_ser_flag;
extern u8 cli_connect_ser_state;    //by_lm
extern u8 cli_connect_wait_ack; 


#define RTX_KERNEL 1
#define DHCP_TOUT   50  
extern volatile unsigned char RTH_link_ok_lm;
extern BOOL tick;
extern U32  dhcp_tout;

extern OS_SEM eth_send_data_semaphore;   //by_lm


extern U8* ETH_Protocol_Control_Data;
extern vu32 lenth;
extern vu32 lenth_client;

extern OS_MUT I2c_Mut_lm;
extern OS_MUT Spi_Mut_lm;

extern OS_MUT Stage_Count_Mut; 			//用于阶段配时表各灯色时间计数值 BY lxb1.13


//CAN
//extern CAN_msg CAN_Rx_MSG[4][3];

//优化数据
typedef struct {
	//什么时候都可以改变
	u8 Period_Time_1;//优化周期时长B7
	u8 Phase_Difference_1;//优化相位差B8
	u8 Stage_Time_1[16];//优化阶段时长BE
	u8 Key_Phase_Time_1[16];//优化阶段关键相位绿灯时间BF
	//执行优化方案时才改变
	u8 Period_Time;//优化周期时长B7
	u8 Phase_Difference;//优化相位差B8
	u8 Stage_Time[16];//优化阶段时长BE
	u8 Key_Phase_Time[16];//优化阶段关键相位绿灯时间BF

	u8 Optimize_Choose;// 1选择优化，0不选择优化
	u16 Optimize_time;//优化数据时间
}Optimize_struct;
extern __IO Optimize_struct Optimize_Data;

typedef struct{
	u8	Sche_Num;//方案号
	u8	Stage_Num;//阶段号
	u16	Phase;//看守相位
	u16	Time_1;//接收
	u16	Time;//看守时间，秒数
	//u8	Choose;//选择
}Hold_struct;
extern	Hold_struct Hold_Data;






/* Global variables ----------------------------------------------------------*/

/* Ethernet Receiver Fifo size*/
#define RX_FIFO_SIZE        6000

/*  Ethernet Receiver Data Fifo */
extern	vu8 eth_rx_fifo[RX_FIFO_SIZE];
/*  Ethernet Receiver Lenth Fifo */
extern	vu32 eth_rx_len ;

extern	vu32 rx_wr_index,rx_rd_index,rx_counter;

/* This flag is set on Ethernet Receiver Fifo Overflow */
extern	vu8 rx_buffer_overflow;


/* Ethernet Send Fifo size*/
#define TX_FIFO_SIZE        9000
/*  Ethernet Receiver Data Fifo */
extern	vu8 eth_tx_fifo[TX_FIFO_SIZE];
/*  Ethernet Receiver Lenth Fifo */
#define TX_LEN_SIZE        1000
extern  vu32 eth_tx_len[TX_LEN_SIZE] ;

extern	vu32 tx_wr_index,tx_rd_index,tx_counter;
extern	vu32 tx_len_wr_index,tx_len_rd_index,tx_len_counter;

/* This flag is set on Ethernet Receiver Fifo Overflow */
extern	vu8 tx_buffer_overflow;
/* This flag is set on Ethernet Lenth Fifo Overflow */
extern	vu8 tx_len_overflow;



/*  Write Client FIFO size */
#define WRITE_Client_SIZE        20  
/*  Write Client FIFO  */
extern	vu8 write_Client_fifo[WRITE_Client_SIZE][12];

extern	vu32 write_Client_wr_index,write_Client_rd_index,write_Client_counter;
/* This flag is set on Write Client  Fifo Overflow */
extern	vu8 write_Client_buffer_overflow;




#define UART_RX_OVER_TIMES      12   //  12ms

// /*  UART2 Receiver buffer size */
// #define UART2_RX_SIZE        RX_BUFSIZE   // 跟以太网一帧数据长度一样
// /*  UART2 Receiver buffer  */
// extern	vu8 uart2_rx_buff[UART2_RX_SIZE];
// /*  UART2 Receiver Lenth  */
// extern	vu8 uart2_rx_len;

// /* This count is set on UART2 Receiver time Over */
// extern	vu8 uart2_rx_time_count ;
// /* This flag  is set on UART2 Receiver time Over */
// extern	vu8 uart2_rx_time_over;


/*  UART4 Receiver fifo size */
#define UART4_RX_SIZE        20   // 跟以太网一帧数据长度一样
/*  UART4 Receiver Fifo size */
extern	vu8 uart4_rx_fifo[UART4_RX_SIZE];
/*  UART4 Receiver Lenth  */
extern	vu8 uart4_rx_len;
extern	vu32 uart4_rx_wr_index,uart4_rx_rd_index,uart4_rx_counter;
/* This flag is set on Ethernet Receiver Fifo Overflow */
extern	vu8 uart4_rx_buffer_overflow;

/* This count is set on UART4 Receiver time Over */
extern	vu8 uart4_rx_time_count ;
/* This flag  is set on UART4 Receiver time Over */
extern	vu8 uart4_rx_time_over;



/*  Write Log FIFO size */
#define WRITE_LOG_SIZE        10  
/*  Write Log FIFO  */
extern	vu8 write_log_wr_fifo[WRITE_LOG_SIZE][21];

extern	vu32 write_log_wr_index,write_log_rd_index,write_log_counter;
/* This flag is set on Write Log  Fifo Overflow */
extern	vu8 write_log_buffer_overflow;
extern __IO uint8_t change_8D_8E_C0_C1_flag ;


/* 参数表  xxxxxx */
extern	const object_attribute_struct  Object_table[];

/* 子对象表 */
extern	const u8 subobject_table[][SUBOBJ_NUM_MAX+1];

//月份数据表,用于公历日期转换星期
extern	const u8 table_week[12]; //月修正数据表
//平年的月份表
extern	const u8 mon_table[12];


////////////////////////////////////////lxblllllllllllllllllllll


/* 优化数据,包括B8 B9 BE BF */
typedef struct {
	u8 		Period_Time;							//公共周期时长B8
	u8 		Phase_Difference;					//协调相位差B9
	u8 		Stage_Time[16];						//优化阶段时长BE
	u8 		Key_Phase_Time[16];				//优化阶段关键相位绿灯时间BF
}sche_optimize_struct;

extern OS_MUT 									RT_Operate_Mut; 			//用于实时操作数据结构体 BY lxb
extern u8 											sche_change_tpye[][4];//方案数组
extern sche_optimize_struct 		sche_optimize;				//优化数据，包括B8 B9 BE BF
extern STAGE_TIMING_TABLE_t			sche_hold_guand;			//看守数据，包括C1	
//====================================lxb////



/*	SCHE_CON_WAY控制方式	*/
#define AUTO_WAY				0				//自主控制，多时段
#define OFF_WAY							1				//灭灯
#define YELLOW_WAY				2				//开机黄闪
#define RED_WAY						3				//开机红闪
#define HOLD_WAY								4				//看守方式
#define STEP_WAY							5				//步进方式
#define OPTIMIZE_WAY						12			//优化方式

/*	sche_change_tpye控制类型行索引	*/
#define STA_YELLOW		0				//开机黄闪
#define STA_RED				1				//开机红闪
#define BY_STEP				2				//步进     	(	BB	)
#define MANUAL_C			3				//手动控制	(	B5	)
#define SYSTEM_C			4				//系统控制	(	B6	)
#define AUTO_C				5				//自主控制
#define YELLOW_C			6				//黄闪控制 	(降级	)

#define NOW_C						7				//当前控制

#define YELLOW_SCHE_NUM 	27		//黄闪方案号
#define RED_SCHE_NUM 			28		//全红方案号

/*	sche_change_tpye列索引	*/
#define SCHE_CON_TYPE		1				//控制类型列索引
#define SCHE_NUM					2				//方案号列索引
#define SCHE_CON_WAY		3				//控制方式列索引

/*	消息A各字节索引宏定义	*/
#define MSGA_TRIGGER_WAY					0				//控制类型列索引
#define MSGA_CONTROL_WAY					1				//方案号列索引
#define MSGA_SCHE_NUM							2				//控制方式列索引


#define WRITE_LOG_WAIT_TIMES		(0x00FF)				//写日志消息等待时长




//#define LIMING_DEBUG_12_23     1


#endif



