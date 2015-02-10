#include "include.h"


//I2Cʱ��
u8 Wether_Change_Fangan_Alarm = 0;


//schedule//����ģ��
Induction_Str Schedule_Struct[Channel_Max_Define] = {0};//����ģ��ṹ��
STAGE_TIMING_TABLE_t Tab_C1[Channel_Max_Define];
PHASE_PARAM_t Tab_95[Channel_Max_Define];
CHANNEL_PARAM_t Tab_B0[Channel_Max_Define];
CHANNEL_OUTPUT_STAT_t Tab_B1[Channel_Max_Define];
PHASE_OUTPUT_STAT_t Tab_96[Channel_Max_Define];
FOLLOW_PHASE_PARAM_t Tab_C8[9];
u8 Stage_End_Flag = 1;//�׶ν�����־
u8 Plan_End_Flag = 1;//����������־
u8 Old_Plan= 0x80;//��ǰ�ķ���
u8 Yellow_End_Flag = 0;//�Ƶƽ�����־

//u8 Tab_BB_Stepping_Control = 0;//��������ʱ�Ľ׶κ�


Plan_Model Plan_Used;//Ӧ��ģ��
u8 Plan_Used_Count = 0;//���ɷ���ѡ��
  Interim_Plan_Model Interim_Scheme[3] = {0};   //���ɷ������ݽṹ��
u8 Circulate_Times = 0;//ѭ�����������ڱ�ʾ��һ�����ɷ���ʱ��Last_Plane_Num��ȡֵ
 //u8 Now_Scheme_Index;//��ǰʱ��ķ������������������ڷ�����
PHASE_PARAM_t Phase_Par[16] = {0};//��λ������
vu8 Channel_Output_Int_Flag = 0;//ͨ��״̬����жϱ�־


u8 Follow_Flag;//�����־
u8 Follow_NO = 0;//������λ
u8 Follow_Pedestrian_NO = 0;//���˸�����λ
u8 Follow_Pedestrian_Flag = 0;//���˸����־
u8 mark_Plan = 0;//���Ʒ�ʽ������
u8 Out_Drive = 1;//�������ѡ��


u8 Yellow_Flicker_Number = 0;

u8 Pedestrian_Trigger_Flag = 0;//���˴�����־
u8 Bus_Trigger_Flag = 0;//����������־
u8 Motor_Trigger_Flag = 0;//������������־
u8 Induction_Trigger_Flag = 0;

u16 Bus_Trig;//������Ӧ
u16 Ped_Trig;//���˸�Ӧ
u16 Car_Trig;//��������Ӧ

u8   Running_Plan_NO;
u8   Running_Stage_NO;
u16 Running_Phase;

u8 Usart_Rx_Count = 0;
u8 Vice_Usart_Rx_Count = 0;


u8 END_Yellow_Flicker = 0;//������������

u8 Er_Hdl_Flag = 0;//�쳣�����־
//collide
//��״̬
Ring_Stutas ring_stutas[17] = {0};
Ring_Stutas_Table phase_8[17] = {0};

Ring_Stutas_Table 	      Table_0xCA_read = {0};
Phase_Order_Table             Table_0xCB_read = {0};

u8 Hold_Stage_Num = 0xff;//���ؽ׶κ�

u8 Status_Current_New = 0;//��ǰ��״̬
u8 Status_Current_Old = 0;//֮ǰ����״̬

//��������������ר�û���
u8 Write_Log_Msg[21] = {0};
u8 Mbx_DB_Update[5] = {0};
u16	Test_Count   = 0; 
u8 End_Int_Handle_Flag = 0;//�ж��쳣���Ʊ�־

/****ETH Par******/
U8 server_ip[4];            //�����IP
u16 server_port;                //�����PORT
u16 local_port = 90;            //���ط���˿ں�
U8 tcp_soc_server;              //�����TCP
u8 tcp_soc_client;              //�ͻ���TCP
u8 eth_send_data_flag = 0;      //��̫���������ݱ�־
u8 client_data_type= 0;         //�����ϱ��������� 1--���� 2-��������    //no_liming
u8 cli_connect_ser_flag = 0;    //�ͻ������ӷ�������־�����������ϱ�����
u8 cli_connect_ser_state= 0;    //by_lm
u8 cli_connect_wait_ack=__TRUE; 

BOOL tick;
U32  dhcp_tout;


//�Ż�����
//Optimize_struct Optimize_Data = {0};
__IO Optimize_struct Optimize_Data = {0};
Hold_struct Hold_Data = {0};



/*  Write Client FIFO size */
#define WRITE_Client_SIZE        20  
/*  Write Client FIFO  */
vu8 write_Client_fifo[WRITE_Client_SIZE][12];

vu32 write_Client_wr_index,write_Client_rd_index,write_Client_counter;
/* This flag is set on Write Client  Fifo Overflow */
vu8 write_Client_buffer_overflow;



/* Global variables ----------------------------------------------------------*/

/* Ethernet Receiver Fifo size*/
#define RX_FIFO_SIZE        6000
/*  Ethernet Receiver Data Fifo */
vu8 eth_rx_fifo[RX_FIFO_SIZE];
/*  Ethernet Receiver Lenth Fifo */
vu32 eth_rx_len=0 ;

vu32 rx_wr_index,rx_rd_index,rx_counter;

/* This flag is set on Ethernet Receiver Fifo Overflow */
vu8 rx_buffer_overflow;

/* Ethernet Send Fifo size*/
#define TX_FIFO_SIZE        9000
/*  Ethernet Receiver Data Fifo */
vu8 eth_tx_fifo[TX_FIFO_SIZE];
/*  Ethernet Receiver Lenth Fifo */
#define TX_LEN_SIZE        1000
vu32 eth_tx_len[TX_LEN_SIZE] ;

vu32 tx_wr_index,			tx_rd_index,			tx_counter;
vu32 tx_len_wr_index,	tx_len_rd_index,	tx_len_counter;

/* This flag is set on Ethernet Receiver Fifo Overflow */
vu8 tx_buffer_overflow;
/* This flag is set on Ethernet Lenth Fifo Overflow */
vu8 tx_len_overflow;


#define UART_RX_OVER_TIMES      12   //  12ms


/*  UART4 Receiver buffer size */
#define UART4_RX_SIZE        20   // ����̫��һ֡���ݳ���һ��
/*  UART4 Receiver Fifo  */
vu8 uart4_rx_fifo[UART4_RX_SIZE];
/*  UART4 Receiver Lenth  */
vu8 uart4_rx_len=0;
vu32 uart4_rx_wr_index,uart4_rx_rd_index,uart4_rx_counter;
/* This flag is set on Ethernet Receiver Fifo Overflow */
vu8 uart4_rx_buffer_overflow;


/* This count is set on UART4 Receiver time Over */
vu8 uart4_rx_time_count = 1+UART_RX_OVER_TIMES;
/* This flag  is set on UART4 Receiver time Over */
vu8 uart4_rx_time_over=0;


/*  Write Log FIFO size */
#define WRITE_LOG_SIZE        10  
/*  Write Log FIFO  */
vu8 write_log_wr_fifo[WRITE_LOG_SIZE][21];

vu32 write_log_wr_index,write_log_rd_index,write_log_counter;
/* This flag is set on Write Log  Fifo Overflow */
vu8 write_log_buffer_overflow;

/*	8D_8E_C0_C1������±�־	*/
__IO uint8_t change_8D_8E_C0_C1_flag =0;  


///BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
#define  MIN_ID   0X81
#define  MAN_ID   0XC9     //Ŀǰ�������Ϊ0XC9�����ʶ
#define  BAOLIU   2



/* �����ʶ���ͱ� */
#define ID_ALL 						0			/*  �����Բ���  */
#define ID_DB							1			/*  ���ݿ����  */
#define ID_NEVER_SET 			2			/*  ��Զ�������ã����ñ������Բ�ѯ */
#define ID_NO_SET 				3			/*  �������ã������ñ������Բ�ѯ  */
#define ID_TIME		 				4			/*  ʱ�����ݲ�д�����ݱ�  */
#define ID_REAL_TIME			5			/*  ʵʱ�������ݲ�д�����ݱ� */
#define ID_PARAMETER			6			/*  �������ñ���ʱ�������ã��ɲ�ѯ������һ����֧ */
#define ID_REPORT					7			/*  д�뱨����־������,�������ã������ñ����ܲ�ѯ */
#define ID_92_NEVER_SET	  8


// 	0  ID;			  	        /* �����ʶ					*/
// 	1	 Data_Addr;					  /* ���ݵ�ַ					*/
// 	2	 Flag_Addr;					  /* ��־��ַ					*/
// 	3	 Line_Number;			    /* ֧������					*/
// 	4	 Subobj_Number;				/* �Ӷ�����				  */
// 	5  Line_leng;						/* ÿ���ֽ���		    */
// 	6  Tab_leng;				    /* ������ֽ���			*/
// 	7  Index1;							/* ����1 						*/
// 	8  Index2;							/* ����2 						*/
// 	9  ID_type;							/* ��־���� 				*/
/* ������  xxxxxx */
const object_attribute_struct  Object_table[] = 
{
/* 																					 B����bytes���ֽ���			*/
/* 		0					 1							2				        3				  4 			       5			        6		   			 7					8      9  */
/* �����ʶ;  ���ݵ�ַ  ;   ��־��ַ       ;  ֧������;    �Ӷ�����;   ÿ��B;       �����B;     ����1��; ����2��; ��־���� */
  {	0X81	,	TAB_81_ADDR	,	TAB_81_FLAG_ADDR , TAB_81_LINES ,			1	, LINE_81_BYTES ,	TAB_81_BYTES	,		0 	, 	0 , ID_DB          },
  {	0X82	,	TAB_82_ADDR	,	TAB_82_FLAG_ADDR , TAB_82_LINES ,			1	, LINE_82_BYTES ,	TAB_82_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X83	,	TAB_83_ADDR	,	TAB_83_FLAG_ADDR , TAB_83_LINES ,			1	, LINE_83_BYTES ,	TAB_83_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X84	,	TAB_84_ADDR	,	TAB_84_FLAG_ADDR , TAB_84_LINES ,			1	, LINE_84_BYTES ,	TAB_84_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X85	,	TAB_85_ADDR	,	TAB_85_FLAG_ADDR , TAB_85_LINES ,		 10 , LINE_85_BYTES ,	TAB_85_BYTES	,	 16 	, 	0 , ID_NEVER_SET   },
  {	0X86	,	TAB_86_ADDR	,	TAB_86_FLAG_ADDR , TAB_86_LINES ,			1	, LINE_86_BYTES ,	TAB_86_BYTES	,		0 	, 	0 , ID_TIME        },
  {	0X87	,	TAB_87_ADDR	,	TAB_87_FLAG_ADDR , TAB_87_LINES ,			1	, LINE_87_BYTES ,	TAB_87_BYTES	,		0 	, 	0 , ID_TIME        },
  {	0X88	,	TAB_88_ADDR	,	TAB_88_FLAG_ADDR , TAB_88_LINES ,			1	, LINE_88_BYTES ,	TAB_88_BYTES	,		0 	, 	0 , ID_TIME        },
  {	0X89	,	TAB_89_ADDR	,	TAB_89_FLAG_ADDR , TAB_89_LINES ,			1	, LINE_89_BYTES ,	TAB_89_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X8A	,	TAB_8A_ADDR	,	TAB_8A_FLAG_ADDR , TAB_8A_LINES ,			1	, LINE_8A_BYTES ,	TAB_8A_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X8B	,	TAB_8B_ADDR	,	TAB_8B_FLAG_ADDR , TAB_8B_LINES ,			1	, LINE_8B_BYTES ,	TAB_8B_BYTES	,		0 	, 	0 , ID_PARAMETER   },
  {	0X8C	,	TAB_8C_ADDR	,	TAB_8C_FLAG_ADDR , TAB_8C_LINES ,			1	, LINE_8C_BYTES ,	TAB_8C_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X8D	,	TAB_8D_ADDR	,	TAB_8D_FLAG_ADDR , TAB_8D_LINES ,		  5	, LINE_8D_BYTES ,	TAB_8D_BYTES	,	 40 	, 	0 ,	ID_DB					 },
  {	0X8E	,	TAB_8E_ADDR	,	TAB_8E_FLAG_ADDR , TAB_8E_LINES ,	    8	, LINE_8E_BYTES ,	TAB_8E_BYTES	,	 16 	,  48 ,	ID_DB					 },
  {	0X8F	,	TAB_8F_ADDR	,	TAB_8F_FLAG_ADDR , TAB_8F_LINES ,			1	, LINE_8F_BYTES ,	TAB_8F_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X90	,	TAB_90_ADDR	,	TAB_90_FLAG_ADDR , TAB_90_LINES ,			1	, LINE_90_BYTES ,	TAB_90_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X91	,	TAB_91_ADDR	,	TAB_91_FLAG_ADDR , TAB_91_LINES ,		  5	, LINE_91_BYTES ,	TAB_91_BYTES	,	 64 	, 	0 ,	ID_NEVER_SET	 },
  {	0X92	,	TAB_92_ADDR	,	TAB_92_FLAG_ADDR , TAB_92_LINES ,     4	, LINE_92_BYTES ,	TAB_92_BYTES  ,	 64 	, 255 ,	ID_92_NEVER_SET},
  {	0X93	,	TAB_93_ADDR	,	TAB_93_FLAG_ADDR , TAB_93_LINES ,			1	, LINE_93_BYTES ,	TAB_93_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X94	,	TAB_94_ADDR	,	TAB_94_FLAG_ADDR , TAB_94_LINES ,			1	, LINE_94_BYTES ,	TAB_94_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X95	,	TAB_95_ADDR	,	TAB_95_FLAG_ADDR , TAB_95_LINES ,		 12	, LINE_95_BYTES ,	TAB_95_BYTES	,	 16 	, 	0 ,	ID_DB					 },
  {	0X96	,	TAB_96_ADDR	,	TAB_96_FLAG_ADDR , TAB_96_LINES ,			4	, LINE_96_BYTES ,	TAB_96_BYTES	,		2 	, 	0 ,	ID_DB					 },
  {	0X97	,	TAB_97_ADDR	,	TAB_97_FLAG_ADDR , TAB_97_LINES ,			2	, LINE_97_BYTES ,	TAB_97_BYTES	,	 16 	, 	0 ,	ID_REPORT			 },
  {	0X98	,	TAB_98_ADDR	,	TAB_98_FLAG_ADDR , TAB_98_LINES ,			1	, LINE_98_BYTES ,	TAB_98_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X99	,	TAB_99_ADDR	,	TAB_99_FLAG_ADDR , TAB_99_LINES ,			1	, LINE_99_BYTES ,	TAB_99_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0X9A	,	TAB_9A_ADDR	,	TAB_9A_FLAG_ADDR , TAB_9A_LINES ,			1	, LINE_9A_BYTES ,	TAB_9A_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X9B	,	TAB_9B_ADDR	,	TAB_9B_FLAG_ADDR , TAB_9B_LINES ,			1	, LINE_9B_BYTES ,	TAB_9B_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X9C	,	TAB_9C_ADDR	,	TAB_9C_FLAG_ADDR , TAB_9C_LINES ,			1	, LINE_9C_BYTES ,	TAB_9C_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X9D	,	TAB_9D_ADDR	,	TAB_9D_FLAG_ADDR , TAB_9D_LINES ,			1	, LINE_9D_BYTES ,	TAB_9D_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X9E	,	TAB_9E_ADDR	,	TAB_9E_FLAG_ADDR , TAB_9E_LINES ,			1	, LINE_9E_BYTES ,	TAB_9E_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0X9F	,	TAB_9F_ADDR	,	TAB_9F_FLAG_ADDR , TAB_9F_LINES ,		  8	, LINE_9F_BYTES ,	TAB_9F_BYTES	,	 48 	, 	0 ,	ID_DB					 },
  {	0XA0	,	TAB_A0_ADDR	,	TAB_A0_FLAG_ADDR , TAB_A0_LINES ,			3	, LINE_A0_BYTES ,	TAB_A0_BYTES	,		8 	, 	0 ,	ID_NO_SET			 },
  {	0XA1	,	TAB_A1_ADDR	,	TAB_A1_FLAG_ADDR , TAB_A1_LINES ,		  7	, LINE_A1_BYTES ,	TAB_A1_BYTES	,	 48 	, 	0 ,	ID_NO_SET			 },
  {	0XA2	,	TAB_A2_ADDR	,	TAB_A2_FLAG_ADDR , TAB_A2_LINES ,		  3	, LINE_A2_BYTES ,	TAB_A2_BYTES	,	 48 	, 	0 ,	ID_REPORT			 },
  {	0XA3	,	TAB_A3_ADDR	,	TAB_A3_FLAG_ADDR , TAB_A3_LINES ,			1	, LINE_A3_BYTES ,	TAB_A3_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XA4	,	TAB_A4_ADDR	,	TAB_A4_FLAG_ADDR , TAB_A4_LINES ,			1	, LINE_A4_BYTES ,	TAB_A4_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XA5	,	TAB_A5_ADDR	,	TAB_A5_FLAG_ADDR , TAB_A5_LINES ,			1	, LINE_A5_BYTES ,	TAB_A5_BYTES	,		0 	, 	0 ,	ID_REPORT			 },
  {	0XA6	,	TAB_A6_ADDR	,	TAB_A6_FLAG_ADDR , TAB_A6_LINES ,			1	, LINE_A6_BYTES ,	TAB_A6_BYTES	,		0 	, 	0 ,	ID_REPORT			 },
  {	0XA7	,	TAB_A7_ADDR	,	TAB_A7_FLAG_ADDR , TAB_A7_LINES ,			1	, LINE_A7_BYTES ,	TAB_A7_BYTES	,		0 	, 	0 ,	ID_REPORT			 },
  {	0XA8	,	TAB_A8_ADDR	,	TAB_A8_FLAG_ADDR , TAB_A8_LINES ,			1	, LINE_A8_BYTES ,	TAB_A8_BYTES	,		0 	, 	0 ,	ID_REPORT			 },
  {	0XA9	,	TAB_A9_ADDR	,	TAB_A9_FLAG_ADDR , TAB_A9_LINES ,			1	, LINE_A9_BYTES ,	TAB_A9_BYTES	,		0 	, 	0 ,	ID_REPORT   	 },
  {	0XAA	,	TAB_AA_ADDR	,	TAB_AA_FLAG_ADDR , TAB_AA_LINES ,			1	, LINE_AA_BYTES ,	TAB_AA_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XAB	,	TAB_AB_ADDR ,	TAB_AB_FLAG_ADDR , TAB_AB_LINES ,			1	, LINE_AB_BYTES ,	TAB_AB_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XAC	,	TAB_AC_ADDR ,	TAB_AC_FLAG_ADDR , TAB_AC_LINES ,			1	, LINE_AC_BYTES ,	TAB_AC_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XAD	,	TAB_AD_ADDR ,	TAB_AD_FLAG_ADDR , TAB_AD_LINES ,			1	, LINE_AD_BYTES ,	TAB_AD_BYTES	,		0 	, 	0 ,	ID_DB	         },
  {	0XAE	,	TAB_AE_ADDR	,	TAB_AE_FLAG_ADDR , TAB_AE_LINES ,			1	, LINE_AE_BYTES ,	TAB_AE_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XAF	,	TAB_AF_ADDR	,	TAB_AF_FLAG_ADDR , TAB_AF_LINES ,			1	, LINE_AF_BYTES ,	TAB_AF_BYTES	,		0 	, 	0 ,	ID_PARAMETER   },
  {	0XB0	,	TAB_B0_ADDR	,	TAB_B0_FLAG_ADDR , TAB_B0_LINES ,		  4	, LINE_B0_BYTES ,	TAB_B0_BYTES	,	 16 	, 	0 ,	ID_DB			     },
  {	0XB1	,	TAB_B1_ADDR	,	TAB_B1_FLAG_ADDR , TAB_B1_LINES ,			4	, LINE_B1_BYTES ,	TAB_B1_BYTES	,		2 	, 	0 ,	ID_NO_SET	     },
  {	0XB2	,	TAB_B2_ADDR	,	TAB_B2_FLAG_ADDR , TAB_B2_LINES ,			1	, LINE_B2_BYTES ,	TAB_B2_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XB3	,	TAB_B3_ADDR	,	TAB_B3_FLAG_ADDR , TAB_B3_LINES ,			1	, LINE_B3_BYTES ,	TAB_B3_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XB4	,	TAB_B4_ADDR	,	TAB_B4_FLAG_ADDR , TAB_B4_LINES ,			1	, LINE_B4_BYTES ,	TAB_B4_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XB5	,	TAB_B5_ADDR	,	TAB_B5_FLAG_ADDR , TAB_B5_LINES ,			1	, LINE_B5_BYTES ,	TAB_B5_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XB6	,	TAB_B6_ADDR	,	TAB_B6_FLAG_ADDR , TAB_B6_LINES ,			1	, LINE_B6_BYTES ,	TAB_B6_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XB7	,	TAB_B7_ADDR	,	TAB_B7_FLAG_ADDR , TAB_B7_LINES ,			1	, LINE_B7_BYTES ,	TAB_B7_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XB8	,	TAB_B8_ADDR	,	TAB_B8_FLAG_ADDR , TAB_B8_LINES ,			1	, LINE_B8_BYTES ,	TAB_B8_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XB9	,	TAB_B9_ADDR	,	TAB_B9_FLAG_ADDR , TAB_B9_LINES ,			1	, LINE_B9_BYTES ,	TAB_B9_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XBA	,	TAB_BA_ADDR ,	TAB_BA_FLAG_ADDR , TAB_BA_LINES ,			1	, LINE_BA_BYTES ,	TAB_BA_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XBB	,	TAB_BB_ADDR ,	TAB_BB_FLAG_ADDR , TAB_BB_LINES ,			1	, LINE_BB_BYTES ,	TAB_BB_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XBC	,	TAB_BC_ADDR ,	TAB_BC_FLAG_ADDR , TAB_BC_LINES ,			1	, LINE_BC_BYTES ,	TAB_BC_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XBD	,	TAB_BD_ADDR ,	TAB_BD_FLAG_ADDR , TAB_BD_LINES ,		  1 , LINE_BD_BYTES ,	TAB_BD_BYTES	,		0 	, 	0 , ID_DB       	 },
  {	0XBE	,	TAB_BE_ADDR ,	TAB_BE_FLAG_ADDR , TAB_BE_LINES ,		  1 , LINE_BE_BYTES ,	TAB_BE_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XBF	,	TAB_BF_ADDR ,	TAB_BF_FLAG_ADDR , TAB_BF_LINES ,		  1 , LINE_BF_BYTES ,	TAB_BF_BYTES	,		0 	, 	0 ,	ID_REAL_TIME	 },
  {	0XC0	,	TAB_C0_ADDR	,	TAB_C0_FLAG_ADDR , TAB_C0_LINES ,			5	, LINE_C0_BYTES ,	TAB_C0_BYTES	,	 32 	, 	0 ,	ID_DB					 },
  {	0XC1	,	TAB_C1_ADDR	,	TAB_C1_FLAG_ADDR , TAB_C1_LINES ,			7	, LINE_C1_BYTES ,	TAB_C1_BYTES	,	 16 	,  16 ,	ID_ALL				 },
  {	0XC2	,	TAB_C2_ADDR	,	TAB_C2_FLAG_ADDR , TAB_C2_LINES ,			1	, LINE_C2_BYTES ,	TAB_C2_BYTES	,		0 	, 	0 ,	ID_DB				   },
  {	0XC3	,	TAB_C3_ADDR	,	TAB_C3_FLAG_ADDR , TAB_C3_LINES ,			1	, LINE_C3_BYTES ,	TAB_C3_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XC4	,	TAB_C4_ADDR	,	TAB_C4_FLAG_ADDR , TAB_C4_LINES ,			1	, LINE_C4_BYTES ,	TAB_C4_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XC5	,	TAB_C5_ADDR	,	TAB_C5_FLAG_ADDR , TAB_C5_LINES ,			1	, LINE_C5_BYTES ,	TAB_C5_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XC6	,	TAB_C6_ADDR	,	TAB_C6_FLAG_ADDR , TAB_C6_LINES ,			1	, LINE_C6_BYTES ,	TAB_C6_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XC7	,	TAB_C7_ADDR	,	TAB_C7_FLAG_ADDR , TAB_C7_LINES ,			1	, LINE_C7_BYTES ,	TAB_C7_BYTES	,		0 	, 	0 ,	ID_PARAMETER	 },
  {	0XC8	,	TAB_C8_ADDR	,	TAB_C8_FLAG_ADDR , TAB_C8_LINES ,		  9 , LINE_C8_BYTES ,	TAB_C8_BYTES	,		8 	, 	0 ,	ID_DB					 },
  {	0XC9	,	TAB_C9_ADDR	,	TAB_C9_FLAG_ADDR , TAB_C9_LINES ,			4	, LINE_C9_BYTES ,	TAB_C9_BYTES	,		1 	, 	0 ,	ID_DB					 },
  {	0XCA	,	TAB_CA_ADDR ,	TAB_CA_FLAG_ADDR , TAB_CA_LINES ,			4	, LINE_CA_BYTES ,	TAB_CA_BYTES	,		4 	,  16 ,	ID_DB					 },
  {	0XCB	,	TAB_CB_ADDR ,	TAB_CB_FLAG_ADDR , TAB_CB_LINES ,			3	, LINE_CB_BYTES ,	TAB_CB_BYTES	,	 16 	, 	0 ,	ID_DB					 },
  {	0XCC	,	TAB_CC_ADDR ,	TAB_CC_FLAG_ADDR , TAB_CC_LINES ,			4	, LINE_CC_BYTES ,	TAB_CC_BYTES	,	 16 	, 	0 ,	ID_DB					 },
  {	0XCD	,	TAB_CD_ADDR ,	TAB_CD_FLAG_ADDR , TAB_CD_LINES ,		 10 , LINE_CD_BYTES ,	TAB_CD_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XCE	,	TAB_CE_ADDR ,	TAB_CE_FLAG_ADDR , TAB_CE_LINES ,			2	, LINE_CE_BYTES ,	TAB_CE_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XCF	,	TAB_CF_ADDR ,	TAB_CF_FLAG_ADDR , TAB_CF_LINES ,			4	, LINE_CF_BYTES ,	TAB_CF_BYTES	,		0 	, 	0 ,	ID_DB					 },
  {	0XD0	,	TAB_D0_ADDR	,	TAB_D0_FLAG_ADDR , TAB_D0_LINES ,			2	, LINE_D0_BYTES ,	TAB_D0_BYTES	,		0 	, 	0 ,	ID_DB					 },
};

// // 	0 	Object_ID;			  	/* �����ʶ					*/
// // 	1	Object_Addr;					/* ��ַ							*/
// // 	2	Line_Number;					/* ֧������					*/
// // 	3	Subobject_Number;			/* �Ӷ�����					*/
// // 	4	Line_Byte_Max;				/* ÿ���ֽ���				*/
// // 	5  Line_Byte_Number;		/* ÿ��ʵ���ֽ���		*/
// // 	6 Object_Number;				/* ������ֽ���			*/
// // 	7  Index1;							/* ����1 						*/
// // 	8  Index2;							/* ����2 						*/
// /* ������  xxxxxx */
// const object_attribute_struct  Object_table[] = 
// {
// /* 																					 B����bytes���ֽ���			*/
// /* 		0					1							2				3				4 			5			6		   					7					8 */
// /* �����ʶ;  ��ַ     ;   ����;�Ӷ�����;ÿ��B;ʵ����B;   		�����B;   ����1����; ����2����*/
//   {	0X81	,	TAB_81_ADDR	,			1	,			1	,			2	,			4	,		 TAB_81_BYTES	,		0 	, 	0 },
//   {	0X82	,	TAB_82_ADDR	,			1	,			1	,			1	,			3	,		 TAB_82_BYTES	,		0 	, 	0 },
//   {	0X83	,	TAB_83_ADDR	,			1	,			1	,			2	,			4	,		 TAB_83_BYTES	,		0 	, 	0 },
//   {	0X84	,	TAB_84_ADDR	,			1	,			1	,			2	,			4	,		 TAB_84_BYTES	,		0 	, 	0 },
//   {	0X85	,	TAB_85_ADDR	,		 16 ,		 10 ,		102 ,		104	,		 TAB_85_BYTES	,		16 	, 	0 },
//   {	0X86	,	TAB_86_ADDR	,			1	,			1	,			4	,			6	,		 TAB_86_BYTES	,		0 	, 	0 },
//   {	0X87	,	TAB_87_ADDR	,			1	,			1	,			4	,			6	,		 TAB_87_BYTES	,		0 	, 	0 },
//   {	0X88	,	TAB_88_ADDR	,			1	,			1	,			4	,			6	,		 TAB_88_BYTES	,		0 	, 	0 },
//   {	0X89	,	TAB_89_ADDR	,			1	,			1	,			1	,			3	,		 TAB_89_BYTES	,		0 	, 	0 },
//   {	0X8A	,	TAB_8A_ADDR	,			1	,			1	,			1	,			3	,		 TAB_8A_BYTES	,		0 	, 	0 },
//   {	0X8B	,	TAB_8B_ADDR	,			1	,			1	,			1	,			3	,		 TAB_8B_BYTES	,		0 	, 	0 },
//   {	0X8C	,	TAB_8C_ADDR	,			1	,			1	,			1	,			3	,		 TAB_8C_BYTES	,		0 	, 	0 },
//   {	0X8D	,	TAB_8D_ADDR	,		 40 ,			5	,			9	,		 11	,	 	 TAB_8D_BYTES	,		40 	, 	0 },
//   {	0X8E	,	TAB_8E_ADDR	,		768	,			8	,			8	,		 10	,	 	 TAB_8E_BYTES	,		16 	, 	48 },
//   {	0X8F	,	TAB_8F_ADDR	,			1	,			1	,			1	,			3	,		 TAB_8F_BYTES	,		0 	, 	0 },
//   {	0X90	,	TAB_90_ADDR	,			1	,			1	,			1	,			3	,		 TAB_90_BYTES	,		0 	, 	0 },
//   {	0X91	,	TAB_91_ADDR	,		 64 ,			5	,			8	,		 10	,	 	 TAB_91_BYTES	,		64 	, 	0 },
//   {	0X92	,	TAB_92_ADDR	,	16320	,		  4	,		 10	,		 12	,		 TAB_92_BYTES ,		64 	, 	255 },
//   {	0X93	,	TAB_93_ADDR	,			1	,			1	,			1	,			3	,		 TAB_93_BYTES	,		0 	, 	0 },
//   {	0X94	,	TAB_94_ADDR	,			1	,			1	,			1	,			3	,		 TAB_94_BYTES	,		0 	, 	0 },
//   {	0X95	,	TAB_95_ADDR	,		 16 ,		 12 ,		 12	,		 14	,	 	 TAB_95_BYTES	,		16 	, 	0 },
//   {	0X96	,	TAB_96_ADDR	,			2	,			4	,			4	,			6	,		 TAB_96_BYTES	,		2 	, 	0 },
//   {	0X97	,	TAB_97_ADDR	,		 16 ,			2	,			3	,			5	,		 TAB_97_BYTES	,		16 	, 	0 },
//   {	0X98	,	TAB_98_ADDR	,			1	,			1	,			1	,			3	,		 TAB_98_BYTES	,		0 	, 	0 },
//   {	0X99	,	TAB_99_ADDR	,			1	,			1	,			1	,			3	,		 TAB_99_BYTES	,		0 	, 	0 },
//   {	0X9A	,	TAB_9A_ADDR	,			1	,			1	,			1	,			3	,		 TAB_9A_BYTES	,		0 	, 	0 },
//   {	0X9B	,	TAB_9B_ADDR	,			1	,			1	,			1	,			3	,		 TAB_9B_BYTES	,		0 	, 	0 },
//   {	0X9C	,	TAB_9C_ADDR	,			1	,			1	,			1	,			3	,		 TAB_9C_BYTES	,		0 	, 	0 },
//   {	0X9D	,	TAB_9D_ADDR	,			1	,			1	,			1	,			3	,		 TAB_9D_BYTES	,		0 	, 	0 },
//   {	0X9E	,	TAB_9E_ADDR	,			1	,			1	,			1	,			3	,		 TAB_9E_BYTES	,		0 	, 	0 },
//   {	0X9F	,	TAB_9F_ADDR	,		 48 ,			8	,			9	,		 11	,	 	 TAB_9F_BYTES	,		48 	, 	0 },
//   {	0XA0	,	TAB_A0_ADDR	,		  8 ,			3	,			3	,			5	,		 TAB_A0_BYTES	,		8 	, 	0 },
//   {	0XA1	,	TAB_A1_ADDR	,		 48 ,			7	,			7	,			9	,	 	 TAB_A1_BYTES	,		48 	, 	0 },
//   {	0XA2	,	TAB_A2_ADDR	,		 48 ,			3	,			3	,			5	,	 	 TAB_A2_BYTES	,		48 	, 	0 },
//   {	0XA3	,	TAB_A3_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A3_BYTES	,		0 	, 	0 },
//   {	0XA4	,	TAB_A4_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A4_BYTES	,		0 	, 	0 },
//   {	0XA5	,	TAB_A5_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A5_BYTES	,		0 	, 	0 },
//   {	0XA6	,	TAB_A6_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A6_BYTES	,		0 	, 	0 },
//   {	0XA7	,	TAB_A7_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A7_BYTES	,		0 	, 	0 },
//   {	0XA8	,	TAB_A8_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A8_BYTES	,		0 	, 	0 },
//   {	0XA9	,	TAB_A9_ADDR	,			1	,			1	,			1	,			3	,		 TAB_A9_BYTES	,		0 	, 	0 },
//   {	0XAA	,	TAB_AA_ADDR	,			1	,			1	,			1	,			3	,		 TAB_AA_BYTES	,		0 	, 	0 },
//   {	0XAB	,	TAB_AB_ADDR ,			1	,			1	,			1	,			3	,		 TAB_AB_BYTES	,		0 	, 	0 },
//   {	0XAC	,	TAB_AC_ADDR ,			1	,			1	,			4	,			6	,		 TAB_AC_BYTES	,		0 	, 	0 },
//   {	0XAD	,	TAB_AD_ADDR ,			1	,			1	,			4	,			6	,		 TAB_AD_BYTES	,		0 	, 	0 },
//   {	0XAE	,	TAB_AE_ADDR	,			1	,			1	,			1	,			3	,		 TAB_AE_BYTES	,		0 	, 	0 },
//   {	0XAF	,	TAB_AF_ADDR	,			1	,			1	,			1	,			3	,		 TAB_AF_BYTES	,		0 	, 	0 },
//   {	0XB0	,	TAB_B0_ADDR	,		 16 ,			4	,			4	,			6	,		 TAB_B0_BYTES	,		16 	, 	0 },
//   {	0XB1	,	TAB_B1_ADDR	,			2	,			4	,			4	,			6	,		 TAB_B1_BYTES	,		2 	, 	0 },
//   {	0XB2	,	TAB_B2_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B2_BYTES	,		0 	, 	0 },
//   {	0XB3	,	TAB_B3_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B3_BYTES	,		0 	, 	0 },
//   {	0XB4	,	TAB_B4_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B4_BYTES	,		0 	, 	0 },
//   {	0XB5	,	TAB_B5_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B5_BYTES	,		0 	, 	0 },
//   {	0XB6	,	TAB_B6_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B6_BYTES	,		0 	, 	0 },
//   {	0XB7	,	TAB_B7_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B7_BYTES	,		0 	, 	0 },
//   {	0XB8	,	TAB_B8_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B8_BYTES	,		0 	, 	0 },
//   {	0XB9	,	TAB_B9_ADDR	,			1	,			1	,			1	,			3	,		 TAB_B9_BYTES	,		0 	, 	0 },
//   {	0XBA	,	TAB_BA_ADDR ,			1	,			1	,			1	,			3	,		 TAB_BA_BYTES	,		0 	, 	0 },
//   {	0XBB	,	TAB_BB_ADDR ,			1	,			1	,			1	,			3	,		 TAB_BB_BYTES	,		0 	, 	0 },
//   {	0XBC	,	TAB_BC_ADDR ,			1	,			1	,			1	,			3	,		 TAB_BC_BYTES	,		0 	, 	0 },
//   {	0XBD	,	TAB_BD_ADDR ,			1	,			1	,		 14 ,		 16	,		 TAB_BD_BYTES	,		0 	, 	0 },
//   {	0XBE	,	TAB_BE_ADDR ,			1	,			1	,		 16 ,		 18	,		 TAB_BE_BYTES	,		0 	, 	0 },
//   {	0XBF	,	TAB_BF_ADDR ,			1	,			1	,		 16 ,		 18	,		 TAB_BF_BYTES	,		0 	, 	0 },
//   {	0XC0	,	TAB_C0_ADDR	,		 32	,			5	,			5	,			7	,	 	 TAB_C0_BYTES	,		32 	, 	0 },
//   {	0XC1	,	TAB_C1_ADDR	,		256	,			7	,			8	,		 10	,		 TAB_C1_BYTES	,		16 	, 	16 },
//   {	0XC2	,	TAB_C2_ADDR	,			1	,			1	,			1	,			3	,		 TAB_C2_BYTES	,		0 	, 	0 },
//   {	0XC3	,	TAB_C3_ADDR	,			1	,			1	,			1	,			3	,		 TAB_C3_BYTES	,		0 	, 	0 },
//   {	0XC4	,	TAB_C4_ADDR	,			1	,			1	,			2	,			4	,		 TAB_C4_BYTES	,		0 	, 	0 },
//   {	0XC5	,	TAB_C5_ADDR	,			1	,			1	,			1	,			3	,		 TAB_C5_BYTES	,		0 	, 	0 },
//   {	0XC6	,	TAB_C6_ADDR	,			1	,			1	,			1	,			3	,		 TAB_C6_BYTES	,		0 	, 	0 },
//   {	0XC7	,	TAB_C7_ADDR	,			1	,			1	,			1	,			3	,		 TAB_C7_BYTES	,		0 	, 	0 },
//   {	0XC8	,	TAB_C8_ADDR	,		  8 ,			9	,		 39 ,		 41	,	 	 TAB_C8_BYTES	,		8 	, 	0 },
//   {	0XC9	,	TAB_C9_ADDR	,			1	,			4	,			4	,			6	,		 TAB_C9_BYTES	,		1 	, 	0 },
//   {	0XCA	,	TAB_CA_ADDR ,		224	,			4	,			2	,			4	,	 	 TAB_CA_BYTES	,		0 	, 	0 },
//   {	0XCB	,	TAB_CB_ADDR ,		 16 ,			3	,			3	,			4	,		 TAB_CB_BYTES	,		0 	, 	0 },
//   {	0XCC	,	TAB_CC_ADDR ,		 16 ,			4	,			2	,			4	,		 TAB_CC_BYTES	,		0 	, 	0 },
//   {	0XCD	,	TAB_CD_ADDR ,			1	,		 10 ,		 12 ,		 14	,		 TAB_CD_BYTES	,		0 	, 	0 },
//   {	0XCE	,	TAB_CE_ADDR ,			1	,			1	,			2	,			4	,		 TAB_CE_BYTES	,		0 	, 	0 },
//   {	0XCF	,	TAB_CF_ADDR ,			1	,			1	,			4	,			6	,		 TAB_CF_BYTES	,		0 	, 	0 },
//   {	0XD0	,	TAB_D0_ADDR	,		 17 ,			4	,			2	,			4	,		 TAB_D0_BYTES	,		0 	, 	0 },
// };


#define SUBOBJ_NUM_MAX   12
/* �Ӷ���� */
const u8 subobject_table[][SUBOBJ_NUM_MAX+1] = 
{
	/*ID   	1		2		 3		 4		 5		 6		 7		8		 9		10   11	 12	 */ 
	{0X81,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X82,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X83,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X84,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X85,	1	,	1	,	 24 ,	 1	,	24	,	 1	,	24	,	1	,	24	,	 1	,	0	,	0	},	/* 0X85 10���Ӷ��� */
	{0X86,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X87,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X88,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X89,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X8A,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X8B,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X8C,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X8D,	1	,	2	,		1	,	 4	,	 1	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0X8D  5���Ӷ��� */
	{0X8E,	1	,	1	,		1	,	 1	,	 1	,	 1	,	 1	,	1	,	 0	,	 0	,	0	,	0	},	/* 0X8E  8���Ӷ��� ˫���� */
	{0X8F,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X90,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X91,	1	,	4	,		1	,	 1	,	 1	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0X91  5���Ӷ��� */
	{0X92,	1	,	1	,		4	,	 4	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0X92  4���Ӷ��� */
	{0X93,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X94,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X95,	1	,	1	,		1	,	 1	,	 1	,	 1	,	 1	,	1	,	 1	,	 1	,	1	,	1	},	/* 0X95 12���Ӷ��� */
	{0X96,	1	,	1	,		1	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0X96  4���Ӷ��� */
	{0X97,	1	,	2	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0X97  2���Ӷ��� */
	{0X98,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X99,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9A,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9B,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9C,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9D,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9E,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0X9F,	1	,	1	,		1	,	 1	,	 1	,	 1	,	 2	,	1	,	 0	,	 0	,	0	,	0	},	/* 0X9F  8���Ӷ��� */
	{0XA0,	1	,	1	,		1	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XA0  3���Ӷ��� */
	{0XA1,	1	,	1	,		1	,	 1	,	 1	,	 1	,	 1	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XA1  7���Ӷ��� */
	{0XA2,	1	,	1	,		1	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XA2  3���Ӷ��� */
	{0XA3,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA4,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA5,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA6,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA7,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA8,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XA9,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAA,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAB,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAC,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAD,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAE,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XAF,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB0,	1	,	1	,		1	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XB0  4���Ӷ��� */
	{0XB1,	1	,	1	,		1	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XB1  4���Ӷ��� */
	{0XB2,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB3,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB4,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB5,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB6,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB7,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB8,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XB9,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBA,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBB,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBC,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBD,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBE,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XBF,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC0,	1	,	1	,		1	,	 1	,	 1	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XC0  5���Ӷ��� */
	{0XC1,	1	,	1	,		2	,	 1	,	 1	,	 1	,	 1	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XC1  7���Ӷ��� ˫���� */
	{0XC2,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC3,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC4,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC5,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC6,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC7,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XC8,	1	,	1	,		1	,	16	,	 1	,	16	,	 1	,	1	,	 1	,	 0	,	0	,	0	},	/* 0XC8  9���Ӷ��� */
	{0XC9,	1	,	1	,		1	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XC9  4���Ӷ��� */
	{0XCA,	1	,	1	,		2	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XCA  4���Ӷ��� */
	{0XCB,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XCC,	1	,	2	,		2	,	 2	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	},	/* 0XCC  4���Ӷ��� */
	{0XCD,	1	,	4	,		4	,	 4	,	 4	,	 4	,	 2	,	6	,	12	,	12	,	0	,	0	},	/* 0XCD 10���Ӷ��� */
	{0XCE,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XCF,	0	,	0	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	, 0 },	/*  0���Ӷ��� */
	{0XD0,	1	,	2	,		2	,	 2	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	,	0	,	0	} 	/* 0XD0  4���Ӷ��� */
};




// // // /* ������󳤶ȣ�����ÿ�еĳ���  */
// // // const u32 table_lenth[][2] = 
// // // {
// // // /*  ÿ��B;           �����B; 					 �����ʶ;  */
// // //   {	4 -BAOLIU		,		 7 -BAOLIU					}, /* 0X81 */
// // //   {	4 -BAOLIU		,		 6 -BAOLIU					}, /* 0X82 */
// // //   {	3 -BAOLIU		,		 7 -BAOLIU					}, /* 0X83 */
// // //   {	4 -BAOLIU		,		 7 -BAOLIU					}, /* 0X84*/
// // //   {	104 -BAOLIU	,	1667 -BAOLIU*16				}, /* 0X85 */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0X86 */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0X87 */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0X88 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X89 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X8A */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X8B */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X8C */
// // //   {	11 -BAOLIU	,	 443 -BAOLIU*40				}, /* 0X8D */
// // //   {	10 -BAOLIU	,	7683 -BAOLIU*768			}, /* 0X8E */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X8F */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X90 */
// // //   {	10 -BAOLIU	,	 643 -BAOLIU*64				}, /* 0X91 */
// // //   {	12 -BAOLIU	,	195843 -BAOLIU*16320	}, /* 0X92 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X93 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X94 */
// // //   {	14 -BAOLIU	,	 227 -BAOLIU*16				}, /* 0X95 */
// // //   {	6 -BAOLIU		,		15 -BAOLIU*2				}, /* 0X96 */
// // //   {	5 -BAOLIU		,		83 -BAOLIU*16				}, /* 0X97 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X98 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X99 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X9A */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X9B */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X9C */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X9D */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0X9E */
// // //   {	11 -BAOLIU	,	 531 -BAOLIU*48				}, /* 0X9F */
// // //   {	5 -BAOLIU		,		43 -BAOLIU*8				}, /* 0XA0 */
// // //   {	9 -BAOLIU		,	 435 -BAOLIU*48				}, /* 0XA1 */
// // //   {	5 -BAOLIU		,	 243 -BAOLIU*48				}, /* 0XA2 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA3 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA4 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA5 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA6 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA7 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA8 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XA9 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XAA */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XAB */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0XAC */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0XAD */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XAE */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XAF */
// // //   {	6 -BAOLIU		,		99 -BAOLIU*16				}, /* 0XB0 */
// // //   {	6 -BAOLIU		,		15 -BAOLIU*2				}, /* 0XB1 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB2 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB3 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB4 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB5 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB6 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB7 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB8 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XB9 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XBA */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XBB */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XBC */
// // //   {	16 -BAOLIU	,		19 -BAOLIU					}, /* 0XBD */
// // //   {	18 -BAOLIU	,		21 -BAOLIU					}, /* 0XBE */
// // //   {	18 -BAOLIU	,		21 -BAOLIU					}, /* 0XBF */
// // //   {	7 -BAOLIU		,	 227 -BAOLIU*32				}, /* 0XC0 */
// // //   {	10 -BAOLIU	,	2563 -BAOLIU*256			}, /* 0XC1 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XC2 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XC3 */
// // //   {	4 -BAOLIU		,		 7 -BAOLIU					}, /* 0XC4 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XC5 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XC6 */
// // //   {	3 -BAOLIU		,		 6 -BAOLIU					}, /* 0XC7 */
// // //   {	4 -BAOLIU		,	 331 -BAOLIU					}, /* 0XC8 */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0XC9 */
// // //   {	4 -BAOLIU		,	 899 -BAOLIU*224			}, /* 0XCA */
// // //   {	4 -BAOLIU		,		67 -BAOLIU*16				}, /* 0XCB */
// // //   {	4 -BAOLIU		,		67 -BAOLIU*16				}, /* 0XCC */
// // //   {	14 -BAOLIU	,		17 -BAOLIU					}, /* 0XCD */
// // //   {	4 -BAOLIU		,		 7 -BAOLIU					}, /* 0XCE */
// // //   {	6 -BAOLIU		,		 9 -BAOLIU					}, /* 0XCF */
// // //   {	4 -BAOLIU		,		71 -BAOLIU*17				}, /* 0XD0 */
// // // 	
// // // };

/* �����ʶ���ͱ� */
#define ID_ALL 						0			/*  �����Բ���  */
#define ID_DB							1			/*  ���ݿ����  */
#define ID_NEVER_SET 			2			/*  ��Զ�������ã����ñ������Բ�ѯ */
#define ID_NO_SET 				3			/*  �������ã������ñ������Բ�ѯ  */
#define ID_TIME		 				4			/*  ʱ�����ݲ�д�����ݱ�  */
#define ID_REAL_TIME			5			/*  ʵʱ�������ݲ�д�����ݱ� */
#define ID_PARAMETER			6			/*  �������ñ���ʱ�������ã��ɲ�ѯ������һ����֧ */
#define ID_REPORT					7			/*  д�뱨����־������,�������ã������ñ����ܲ�ѯ */
#define ID_92_NEVER_SET	  8

// const u8 ID_type[][2] = 
// {
// 	/*ID   	1		 */ 
// 	{0X81,	ID_DB					},	/*  ���ݿ���� */
// 	{0X82,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X83,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X84,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X85,	ID_NEVER_SET	},	/*  ��Զ�������ã����ñ������Բ� */
// 	{0X86,	ID_TIME				},	/*  ʱ������ */
// 	{0X87,	ID_TIME				},	/*  ʱ������ */
// 	{0X88,	ID_TIME				},	/*  ʱ������ */
// 	{0X89,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X8A,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X8B,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X8C,	ID_DB					},	/*  ���ݿ���� */
// 	{0X8D,	ID_DB					},	/*  ���ݿ���� */
// 	{0X8E,	ID_DB					},	/*  ���ݿ���� */
// 	{0X8F,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X90,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X91,	ID_NEVER_SET	},	/*  ��Զ�������ã����ñ������Բ� */
// 	{0X92,	ID_92_NEVER_SET	},	/*  ��Զ�������ã����ñ������Բ� */
// 	{0X93,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X94,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X95,	ID_DB					},	/*  ���ݿ���� */
// 	{0X96,	ID_DB					},	/*  ���ݿ���� */
// 	{0X97,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0X98,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X99,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0X9A,	ID_DB					},	/*  ���ݿ���� */
// 	{0X9B,	ID_DB					},	/*  ���ݿ���� */
// 	{0X9C,	ID_DB					},	/*  ���ݿ���� */
// 	{0X9D,	ID_DB					},	/*  ���ݿ���� */
// 	{0X9E,	ID_DB					},	/*  ���ݿ���� */
// 	{0X9F,	ID_DB					},	/*  ���ݿ���� */
// 	{0XA0,	ID_NO_SET			},	/*  �������ã������ñ������Բ�ѯ */
// 	{0XA1,	ID_NO_SET			},	/*  �������ã������ñ������Բ�ѯ */
// 	{0XA2,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XA3,	ID_DB					},	/*  ���ݿ���� */
// 	{0XA4,	ID_DB					},	/*  ���ݿ���� */
// 	{0XA5,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XA6,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XA7,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XA8,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XA9,	ID_REPORT			},	/*  д�뱨����־ */
// 	{0XAA,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XAB,	ID_DB					},	/*  ���ݿ���� */
// 	{0XAC,	ID_DB					},	/*  ���ݿ���� */
// 	{0XAD,	ID_DB					},	/*  ���ݿ���� */
// 	{0XAE,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XAF,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XB0,	ID_DB					},	/*  ���ݿ���� */
// 	{0XB1,	ID_NO_SET			},	/*  �������ã������ñ������Բ�ѯ */
// 	{0XB2,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XB3,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XB4,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XB5,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XB6,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XB7,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XB8,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XB9,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XBA,	ID_DB					},	/*  ���ݿ���� */
// 	{0XBB,	ID_DB					},	/*  ���ݿ���� */
// 	{0XBC,	ID_DB					},	/*  ���ݿ���� */
// 	{0XBD,	ID_DB					},	/*  ���ݿ���� */
// 	{0XBE,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XBF,	ID_REAL_TIME	},	/*  ʵʱ���� */
// 	{0XC0,	ID_DB					},	/*  ���ݿ���� */
// 	{0XC1,	ID_ALL				},	/*  �����Բ��� */
// 	{0XC2,	ID_DB					},	/*  ���ݿ���� */
// 	{0XC3,	ID_DB					},	/*  ���ݿ���� */
// 	{0XC4,	ID_DB					},	/*  ���ݿ���� */
// 	{0XC5,	ID_DB					},	/*  ���ݿ����*/
// 	{0XC6,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XC7,	ID_PARAMETER	},	/*  �������ñ� */
// 	{0XC8,	ID_DB					},	/*  ���ݿ���� */
// 	{0XC9,	ID_DB					},	/*  ���ݿ���� */
// 	{0XCA,	ID_DB					},	/*  ���ݿ���� */
// 	{0XCB,	ID_DB 				},	/*  ���ݿ���� */
// 	{0XCC,	ID_DB					},	/*  ���ݿ���� */
// 	{0XCD,	ID_DB					},	/*  ���ݿ���� */
// 	{0XCE,	ID_DB 				},	/*  ���ݿ���� */
// 	{0XCF,	ID_DB					},	/*  ���ݿ���� */
// 	{0XD0,	ID_DB					} 	/*  ���ݿ���� */
// };



	/*	�����׶�����-ֻ��	*///��������500msһ��500msһ��
	const STAGE_TIMING_TABLE_t Yel_Stage_Time_Table[16] = {	{4,1,0xFFFF,0,1,0,0},
																													{4,2,0xFFFF,0,0,0,0},
																													{4,3,0xFFFF,0,0,0,0},
																													{4,4,0xFFFF,0,1,0,0},
																													{4,5,0xFFFF,0,0,0,0},
																													{4,6,0xFFFF,0,0,0,0},
																													{4,7,0xFFFF,0,1,0,0},
																													{4,8,0xFFFF,0,0,0,0},
																													{4,9,0xFFFF,0,0,0,0},
																													{4,10,0xFFFF,0,1,0,0},
																													{4,11,0xFFFF,0,0,0,0},
																													{4,12,0xFFFF,0,0,0,0},
																													{4,13,0xFFFF,0,1,0,0},
																													{4,14,0xFFFF,0,0,0,0},
																													{4,15,0xFFFF,0,0,0,0},
// 																													{4,16,0xFFFF,0,0,0,0}
																													};
	/*	ȫ��׶�����-ֻ��	*/
	const STAGE_TIMING_TABLE_t Red_Stage_Time_Table[16] = {	{5,1,0xFFFF,0,0,1,0},
																													{5,2,0xFFFF,0,0,1,0},
																													{5,3,0xFFFF,0,0,1,0},
																													{5,4,0xFFFF,0,0,1,0},
																													{0}									};

BIT_BAND_FLAG bit_band_flag_dog;  //  λ����־λ������ ���Ź�
