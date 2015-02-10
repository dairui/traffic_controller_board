#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

// #define CTLR_TYPE1_LED_ON(void) 	(GPIO_SetBits(GPIOE, GPIO_Pin_2))
// #define CTLR_TYPE2_LED_ON(void) 	(GPIO_SetBits(GPIOE, GPIO_Pin_3))
// #define SEND_LED_ON(void) 			(GPIO_SetBits(GPIOE, GPIO_Pin_4))
// #define RECEIVE_LED_ON(void) 		(GPIO_SetBits(GPIOE, GPIO_Pin_5))
// #define RUN_LED_ON(void) 			(GPIO_SetBits(GPIOE, GPIO_Pin_6))
// #define ERROR_LED_ON(void) 			(GPIO_SetBits(GPIOF, GPIO_Pin_6))
// #define ERROR1_LED_ON(void) 		(GPIO_SetBits(GPIOF, GPIO_Pin_7))
// #define ERROR2_LED_ON(void) 		(GPIO_SetBits(GPIOF, GPIO_Pin_8))

// #define CTLR_TYPE1_LED_OFF(void) 	(GPIO_ResetBits(GPIOE, GPIO_Pin_2))
// #define CTLR_TYPE2_LED_OFF(void) 	(GPIO_ResetBits(GPIOE, GPIO_Pin_3))
// #define SEND_LED_OFF(void) 			(GPIO_ResetBits(GPIOE, GPIO_Pin_4))
// #define RECEIVE_LED_OFF(void) 		(GPIO_ResetBits(GPIOE, GPIO_Pin_5))
// #define RUN_LED_OFF(void) 			(GPIO_ResetBits(GPIOE, GPIO_Pin_6))
// #define ERROR_LED_OFF(void) 		(GPIO_ResetBits(GPIOF, GPIO_Pin_6))
// #define ERROR1_LED_OFF(void) 		(GPIO_ResetBits(GPIOF, GPIO_Pin_7))
// #define ERROR2_LED_OFF(void) 		(GPIO_ResetBits(GPIOF, GPIO_Pin_8))

#define Threshold_Value   1//��ֵ
#define Channel_Max 16  //�����λ��
#define Channel_Max_Define    (Channel_Max + 1)//����ʱʹ��
#define Channel_Type        3//·��ͨ�����ͣ��������͵�����������գ�ֱ�У����˵ȡ�����

#define Green_Min  		(1) //ÿ�η����̵���С2��
#define Yellow_Min  		(1) //ÿ�η��лƵ���С1��
#define Red_Min  			(1) //ÿ�η��к����С1��

#define Int_Time_Per_Second 4 				//һ������ζ�ʱ���ж�
#define Int_Time_Per_Second_yellow 2  //��������һ������ζ�ʱ���ж�
#define Green_Add_Max  (3 * Int_Time_Per_Second) //���̵ƽ���ǰ3��
#define Green_Add_Min   (1 * Int_Time_Per_Second)//���̵ƽ���ǰ1��
extern STAGE_TIMING_TABLE_t Tab_C1[Channel_Max_Define];
extern PHASE_PARAM_t Tab_95[Channel_Max_Define];
extern CHANNEL_PARAM_t Tab_B0[Channel_Max_Define];
extern CHANNEL_OUTPUT_STAT_t Tab_B1[Channel_Max_Define];
extern PHASE_OUTPUT_STAT_t Tab_96[Channel_Max_Define];
extern FOLLOW_PHASE_PARAM_t Tab_C8[9];
extern u8 Stage_End_Flag;//�׶ν�����־
extern u8 Plan_End_Flag;//����������־
extern u8 Old_Plan;//��ǰ�ķ���
extern u8 Yellow_End_Flag;//�Ƶƽ�����־

#define Invalid_Scheme_Num    128
//======
extern u16 Tim2_Int_Count;//����timer 2�жϴ���
//extern u8 Yel_Flick_Time;//0xA3����ʱ�������ʱ��
//extern u8 Red_All_Time;//0xA4����ʱ��ȫ�����ʱ��
//extern u8 Manual_Control_Scheme;//0xB5�ֶ����Ʒ���
//extern u8 System_Control_Scheme;//0xB6ϵͳ���Ʒ���
//extern u8 Control_Mode;//0xB7���Ʒ�ʽ
//extern u8 Scheme_Num;//�ⲿ���Ʒ�����
//extern u8 Control_Type;//���Ʒ�ʽ
//extern u8 Extern_Scheme_Num;//������
//extern u8 Today_Control_Mode[16] ;//0x8E������п��Ʒ�ʽ
#define Invalid_Scheme_Num    128
#define Single_Optimize_Scheme_Num 31 //�����Ż�������
#define System_Optimize_Scheme_Num 32 //ϵͳ�Ż�������
extern u8 Circulate_Times;
//extern u8 ADC_Usart[4][12];
typedef struct {
	u8 Plan_Num;//������
	u8 Last_Plan_Num;//�ϴεķ����ţ����û�У���ΪInvalid_Scheme_Num
	u8 Period_Time;//����ʱ��-�Ż�0xB8������0xC0
	u8 Phase_Difference;//��λ��-�Ż�0XB9������0XC0
	u8 Last_Phase_Difference;//��һ����������λ��
	u8 Coordination_Phase;//Э����λ0xC0
	u8 Stage_State;//�׶�״̬-�Ż�0xBA������0xC1
	u8 Stage_Time[16];//�׶�ʱ��-�Ż�0xBE������0xC1
	u8 Key_Phase_Time[16];//�׶ιؼ���λ�̵�ʱ��-�Ż�0xBE������0x95
	STAGE_TIMING_TABLE_t Stage_Contex[16];//�׶���ʱ�����
	u8 Plane_Execute_Time[3];//����ִ��ʱ��:��һ�ֽ�-���㡢�ڶ��ֽ�-����0x8E
}Plan_Model;//����ģ��ṹ��

//schedule
//32��λ��Ӧ�����ṹ��
typedef struct {
	u8 induction_control;//��Ӧ����-��Ӧ����
	u8 control_mode;//���Ʒ���-��Ӧ����
	u8 stage_opt;//�׶�ѡ��-Ϊ��Ӧ�׶�
	u8 detector_error;//���������-���������Ϊ1
	
	u8 system_control;//ϵͳ����-Ϊϵͳ���Ʒ�ʽ
	u8 system_scheme;//ϵͳ����-Ϊϵͳ���Ʒ���
	
	u8 pedestrian_trigger;//���˴���
	u8 bus_trigger;//��������
	u8 motor_trigger;//����������
	u8 handle_flag;//�����־
	
	u8 fixed_phase;//�̶���λ
	u8 undetermined_phase;//������λ
	u8 flexible_phase;//������λ
	u8 key_phase;//�ؼ���λ
	u8 reserve_phase;//������λ
	u8 pedestrian_phase;//������λ
	u8 bicycle_phase;//���г���λ
	u8 motor_phase;//��������λ
	
	u8 follow_phase_opt;//������λѡ��
	u8 pedestrian_phase_opt;//������λѡ��
	u8 undetermined_phase_opt;//������λѡ��
	u8 twice_cross_opt;//���ι���ѡ��
	u8 phase_enable_flag;//��λ���ñ�־
	u8 release_phase;//������λ
	
	u8 phase_release_flag;//��λ���б�־
	u16 green_count;//�̵Ƽ�����
	u16 max_green_count;//����̵Ƽ�����
	u8 yellow_count;//�ƵƼ�����
	u8 red_count;//��Ƽ�����
}Induction_Str; 


extern Plan_Model Plan_Used;//Ӧ��ģ��
extern u8 Plan_Used_Count;//���ɷ���ѡ��
//extern u8 Now_Scheme_Index;//��ǰʱ��ķ������������������ڷ�����
extern PHASE_PARAM_t Phase_Par[16];//��λ������
//extern u8 Tab_BB_Stepping_Control;//��������ʱ�Ľ׶κ�
extern vu8 Channel_Output_Int_Flag;//ͨ��״̬����жϱ�־
//extern u8 Stage_Current;//��ǰ�׶κ� 
//extern u8 Stage_Current_Reset;//��ǰ�׶κ� ����
//extern vu8 Count_allows;//�Ƿ��������
//extern u8 Lock_Count_allows;//�Ƿ��������lock�׶�ר��
//extern u8 Guard_Flag;//������λ��־
//extern u8 Step_Control_Number;//���벽���׶κ�
//extern u8 Lock_Flag;//���ر�־
//extern u8 Step_Jump_Flag;//������Ծ��־
//extern u8 Single_Step_Green_start;//���������¸��׶��̵ƿ�ʼ
//extern u8 Prio_Control_Mode;//�������ȼ�����ģʽ
//extern u8 Prio_Control_End;//�������ȼ����ƽ���
//extern u8 Working_Phase[Channel_Max_Define];//��ǰ���ڷ�����λ
//extern u8 First_Single_Step;//������һ��
//extern u8 Exit_Keyboard_Control;//�˳��ֶ�������=1
extern u8 Follow_Flag;//�����־
extern u8 Follow_NO;//������λ
extern u8 Follow_Pedestrian_NO;//���˸�����λ
extern u8 Follow_Pedestrian_Flag;//���˸����־

extern u8 Er_Hdl_Flag;//�쳣�����־

extern u16 Bus_Trig;//������Ӧ
extern u16 Ped_Trig;//���˸�Ӧ
extern u16 Car_Trig;//��������Ӧ
//����ִ�е�����
extern u8   Running_Plan_NO;
extern u8   Running_Stage_NO;
extern u16 Running_Phase;



void Send_Channel_Output_Unit(CHANNEL_OUTPUT_STAT_t tmp_channel_output,u8 index);

void Fangan_Model(u8 fangan_NO, u8 Stage_NO, u8 Step_En_Flag, 
                 Plan_Model   * Plan_Used_temp, Induction_Str * Schedule_Struct_temp);

//u8 Phase_Output_Int(void);

u8 Induction_Schedule(void);



u16 Phase_Follow_Set(u16 work_phase);

u8 Check_Scheme(u8 scheme_number);

__task void Find_Scheme_Task(void);
#endif

