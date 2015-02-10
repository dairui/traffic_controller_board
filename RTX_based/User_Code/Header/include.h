
#define RX_BUFSIZE          (400)                      //������ջ�������󳤶�
#define TX_BUFSIZE          (400)                      //���巢�ͻ�������󳤶�,����־��󳤶�


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
#define Channel_Max 		16			//�����λ��
#define Channel_Max_Define (Channel_Max + 1)


// // #define RX_BUFSIZE          (400)                      //������ջ�������󳤶�
// // #define TX_BUFSIZE          (400)                      //���巢�ͻ�������󳤶�,����־��󳤶�


extern u32 TimingDelay;
extern u32	LocalTime; 
//��λ���״̬
// extern u8 Red_Lamp_Status[Channel_Max/2+1];//���״̬
// extern u8 Yellow_Lamp_Status[Channel_Max/2+1];//�Ƶ�״̬
// extern u8 Green_Lamp_Status[Channel_Max/2+1];//�̵�״̬
//extern u16 ADC_result[Channel_Max/2 + 1][7];//��λ�����ֵ
//dbase
 //���ݿ����
//extern u8 Read_Result_A[24];//��ȡ�����Ӷ���ֵ��������������ֽ�24 �ֽ�
//extern u8 Read_Result_B[24][24];//��ȡĳ�д������
//extern u8 Out_Data[2000];//�����ݿ�õ����ݴ�ŵط�

//���ϼ�¼�������ϱ�־
//���ϡ�ʱ���¼
//=============���ϡ��¼���ֱ�ӻ���

#define	Type_97			1			//��λ��ͻ0x97

#define	Type_A7			3			//�źż�����0xA7
#define	Type_A8			4			//���ؿ��Ʒ�ʽ0xA8
#define	Type_A9			5			//��������0xA9
#define	Type_E1			6			//��λ����״̬
#define	Type_E2			7			//��λ��Ƶ�״̬
#define	Type_E3			8			//��λ���̵�״̬
#define	Type_E4			9			//�ɿع�״̬
#define	Type_A6			10	    //������״̬

//#define	Working_Without_Plan		41	//�޷���ִ��
//#define	Lamp_Output_Unmatched	42	//��״̬�����״̬��ƥ��


//==============���ϡ��¼�����������
//#define	Cpu_Commu_Error		61	//����λ��ͨ�Ź���
//#define	Phase_Commu_Error		62	//����λ��ͨ�Ź���
#define	Comm_Warn				13	//ͨ�ű���0xDC
//#define	RTC_Error				81	//ʱ�ӹ���
//#define	Plan_Para_Error			82	//���������쳣
#define	Run_Warn				14	//�źŻ����б���0xDD
//#define	Detector_Unknown_Error	101	//�����δ֪����
//#define	Detector_Set_Error		102	//��������ù���
//#define	Detector_Commu_Error	103	//�����ͨ�Ź���
//#define	Detector_Unstable			104	//��������ȶ�
//#define	Detector_TooLong			105	//���������ʱ�����
//#define	Detector_Inactive			106	//��������
#define	Detect_Warn				11	//���������0xA2
//#define	Detect_Out_Range			121	//��Ӧ�仯������
//#define	Inductance_Unenough		122	//��в���
//#define	Coil_Open				123	//��Ȧ��·
//#define	Watchdog_Fault			124	//Watchdog����
//#define	Coil_Unknown_Error		125	//��Ӧ��Ȧδ֪����
#define	Coil_Warm				12	//��Ӧ��Ȧ����0xA2

//===============�¼�
//#define	Msg_Too_Long			141	//��Ϣ����̫��
//#define	Msg_Type_Error			142	//��Ϣ���ʹ���
//#define	Index_Subobj_Error		143	//�������Ӷ������
//#define	Obj_Set_Out_Range		144	//���õĶ���ֵ������Χ
//#define	Msg_Too_Short			145	//��Ϣ����̫��
//#define	Msg_CANNOT_Decod		146	//������ȷ����
#define	Msg_Analy_Warm			27	//���ݽ�������0xDE
//#define	EEPROM_Read_Error		151	//��EEPROM���ݳ���
//#define	Lamp_Current_Error		152	//�źŵƵ��������쳣
#define	Run_Output_Warn			28	//�źŻ������0xDF
//#define	Unknown_Mode			161	//δ֪ģʽ
//#define	System_Ctrl				162	//ϵͳЭ������
//#define	Host_Ctrl					163	//����Э������
//#define	Manual_Ctrl 				164	//�ֶ�������
//#define	Plan_Period_Ctrl			165	//ʱ�α���ƣ�������ʱ�α����У�
//#define	Cable_Coordination_Ctrl	166	//����Э��
//#define	Guard_Ctrl				167	//����
#define	Sys_Ctrl_Type			21	//ϵͳ���Ʒ�ʽ0xA5
//#define	Induction_Ctrl				168	//��Ӧ
//#define	Sys_Optimize_Ctrl			169	//ϵͳ�Ż�
//#define	Single_Point_Optim_Ctrl	170	//�����Ż�
//#define	Sys_Multiperiod_Ctrl		171	//ϵͳ��ʱ��
//#define	Yel_Flash				172	//����
//#define	Red_All					173	//ȫ��
//#define	Lamp_All_Off				174	//���
//#define	Step_Ctrl					175	//����
#define	Ctrl_Mode				26	//���Ʒ�ʽ0xA5
//#define	Unknown_Flash			181	//δ֪������
//#define	Not_Flash				182	//��ǰ���Ǵ����������״̬
//#define	Auto_Yel_Flash 			183	//�Զ���������
//#define	Local_Manual_Yel_Flash	184	//�����ֶ�����
//#define	Error_Monitor_Yel_Flash	185	//���ϼ�������Ļ���
//#define	Signal_Collide_Yel_Flash	186	//�źų�ͻ����Ļ���
//#define	 System_Start_Yel_Flash	187	//����ʱ���������

//#define	System_Stopping_Run		201	//�źŻ�����ֹͣ����
//#define	Battery_Runtime_Low		202	//��طŵ�ʱ�����
//#define	System_Restart 			203	//�źŻ�����
//#define	System_Start				204	//�źŻ�����
//#define	Cabinet_Door_State		205	//�����ſ���״̬
//#define	System_Landing			206	//ϵͳ��¼
//#define	Plan_Change_Set 			207	//���������޸�

//#define	Coor_Active				211	//������Э������״̬
//#define	Local_Free				212	//���ص������
//#define	Local_Flash				213	//�����������
//#define	MMU_Flash				214	//MMU��������⣬���ҳ���Start_Up����ʱ��

//#define	Emergency_Alarm			221	//��������
//#define	Non_Emergency_Alarm		222	//�ǽ�������
//#define	Detector_Error			223	//���������
//#define	Force_Local_Control		224	//ǿ�Ʊ��ؿ���
//#define	T_F_Flash				225	//T&F Flash



extern Induction_Str Schedule_Struct[Channel_Max_Define];//����ģ��ṹ��
extern   u16 System_Single_Optimize_Set;//ϵͳ�Ż��򵥵��Ż���־��0��û�Ż���1���Ż�
extern  u8 Fangan_Running;//����ִ�з���
//extern u8 Control_Type_8E[16];
extern  u8 mark_Plan;//���Ʒ�ʽ������
////���䴫������
//��cpu�����ж�����
// extern u8 ViceCpu_Data_Flag ;//��CPU�����жϱ�־
// extern u8 ViceCpu_Data[100] ;//���ݻ�����
// extern u8 ViceCpu_Data_Index ;//���ݻ���������
extern u8 Pedestrian_Trigger_Flag ;//���˴�����־
extern u8 Bus_Trigger_Flag ;//����������־
extern u8 Motor_Trigger_Flag ;//������������־
extern u8 Induction_Trigger_Flag ;

extern u8 Usart_Rx_Count;//���ڽ��ռ�������ʱ
extern u8 Vice_Usart_Rx_Count;//���ڽ��ռ�������ʱ


extern u8 END_Yellow_Flicker;//������������

extern u8 Hold_Stage_Num;//���ؽ׶κ�

extern u8 Status_Current_New;//��ǰ��״̬
extern u8 Status_Current_Old;//֮ǰ����״̬

//��������������ר�û���
extern u8 Write_Log_Msg[21];
//extern u8 Mbx_Error_Handle[5];
extern u8 Mbx_DB_Update[5];
extern u16	Test_Count;
extern u8 End_Int_Handle_Flag;//�ж��쳣���Ʊ�־

/****ETH Par******/
extern U8 server_ip[4];            //�����IP
extern u16 server_port;                //�����PORT
extern u16 local_port;            //���ط���˿ں�
extern U8 tcp_soc_server;       //�����TCP
extern u8 tcp_soc_client;       //�ͻ���TCP
extern u8 eth_send_data_flag ;//��̫���������ݱ�־
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

extern OS_MUT Stage_Count_Mut; 			//���ڽ׶���ʱ�����ɫʱ�����ֵ BY lxb1.13


//CAN
//extern CAN_msg CAN_Rx_MSG[4][3];

//�Ż�����
typedef struct {
	//ʲôʱ�򶼿��Ըı�
	u8 Period_Time_1;//�Ż�����ʱ��B7
	u8 Phase_Difference_1;//�Ż���λ��B8
	u8 Stage_Time_1[16];//�Ż��׶�ʱ��BE
	u8 Key_Phase_Time_1[16];//�Ż��׶ιؼ���λ�̵�ʱ��BF
	//ִ���Ż�����ʱ�Ÿı�
	u8 Period_Time;//�Ż�����ʱ��B7
	u8 Phase_Difference;//�Ż���λ��B8
	u8 Stage_Time[16];//�Ż��׶�ʱ��BE
	u8 Key_Phase_Time[16];//�Ż��׶ιؼ���λ�̵�ʱ��BF

	u8 Optimize_Choose;// 1ѡ���Ż���0��ѡ���Ż�
	u16 Optimize_time;//�Ż�����ʱ��
}Optimize_struct;
extern __IO Optimize_struct Optimize_Data;

typedef struct{
	u8	Sche_Num;//������
	u8	Stage_Num;//�׶κ�
	u16	Phase;//������λ
	u16	Time_1;//����
	u16	Time;//����ʱ�䣬����
	//u8	Choose;//ѡ��
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
// #define UART2_RX_SIZE        RX_BUFSIZE   // ����̫��һ֡���ݳ���һ��
// /*  UART2 Receiver buffer  */
// extern	vu8 uart2_rx_buff[UART2_RX_SIZE];
// /*  UART2 Receiver Lenth  */
// extern	vu8 uart2_rx_len;

// /* This count is set on UART2 Receiver time Over */
// extern	vu8 uart2_rx_time_count ;
// /* This flag  is set on UART2 Receiver time Over */
// extern	vu8 uart2_rx_time_over;


/*  UART4 Receiver fifo size */
#define UART4_RX_SIZE        20   // ����̫��һ֡���ݳ���һ��
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


/* ������  xxxxxx */
extern	const object_attribute_struct  Object_table[];

/* �Ӷ���� */
extern	const u8 subobject_table[][SUBOBJ_NUM_MAX+1];

//�·����ݱ�,���ڹ�������ת������
extern	const u8 table_week[12]; //���������ݱ�
//ƽ����·ݱ�
extern	const u8 mon_table[12];


////////////////////////////////////////lxblllllllllllllllllllll


/* �Ż�����,����B8 B9 BE BF */
typedef struct {
	u8 		Period_Time;							//��������ʱ��B8
	u8 		Phase_Difference;					//Э����λ��B9
	u8 		Stage_Time[16];						//�Ż��׶�ʱ��BE
	u8 		Key_Phase_Time[16];				//�Ż��׶ιؼ���λ�̵�ʱ��BF
}sche_optimize_struct;

extern OS_MUT 									RT_Operate_Mut; 			//����ʵʱ�������ݽṹ�� BY lxb
extern u8 											sche_change_tpye[][4];//��������
extern sche_optimize_struct 		sche_optimize;				//�Ż����ݣ�����B8 B9 BE BF
extern STAGE_TIMING_TABLE_t			sche_hold_guand;			//�������ݣ�����C1	
//====================================lxb////



/*	SCHE_CON_WAY���Ʒ�ʽ	*/
#define AUTO_WAY				0				//�������ƣ���ʱ��
#define OFF_WAY							1				//���
#define YELLOW_WAY				2				//��������
#define RED_WAY						3				//��������
#define HOLD_WAY								4				//���ط�ʽ
#define STEP_WAY							5				//������ʽ
#define OPTIMIZE_WAY						12			//�Ż���ʽ

/*	sche_change_tpye��������������	*/
#define STA_YELLOW		0				//��������
#define STA_RED				1				//��������
#define BY_STEP				2				//����     	(	BB	)
#define MANUAL_C			3				//�ֶ�����	(	B5	)
#define SYSTEM_C			4				//ϵͳ����	(	B6	)
#define AUTO_C				5				//��������
#define YELLOW_C			6				//�������� 	(����	)

#define NOW_C						7				//��ǰ����

#define YELLOW_SCHE_NUM 	27		//����������
#define RED_SCHE_NUM 			28		//ȫ�췽����

/*	sche_change_tpye������	*/
#define SCHE_CON_TYPE		1				//��������������
#define SCHE_NUM					2				//������������
#define SCHE_CON_WAY		3				//���Ʒ�ʽ������

/*	��ϢA���ֽ������궨��	*/
#define MSGA_TRIGGER_WAY					0				//��������������
#define MSGA_CONTROL_WAY					1				//������������
#define MSGA_SCHE_NUM							2				//���Ʒ�ʽ������


#define WRITE_LOG_WAIT_TIMES		(0x00FF)				//д��־��Ϣ�ȴ�ʱ��




//#define LIMING_DEBUG_12_23     1


#endif



