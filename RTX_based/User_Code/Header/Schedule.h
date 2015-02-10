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

#define Threshold_Value   1//阀值
#define Channel_Max 16  //最大相位数
#define Channel_Max_Define    (Channel_Max + 1)//定义时使用
#define Channel_Type        3//路口通道类型，放行类型灯组数，如左拐，直行，行人等。。。

#define Green_Min  		(1) //每次放行绿灯最小2秒
#define Yellow_Min  		(1) //每次放行黄灯最小1秒
#define Red_Min  			(1) //每次放行红灯最小1秒

#define Int_Time_Per_Second 4 				//一秒进几次定时器中断
#define Int_Time_Per_Second_yellow 2  //黄闪方案一秒进几次定时器中断
#define Green_Add_Max  (3 * Int_Time_Per_Second) //距绿灯结束前3秒
#define Green_Add_Min   (1 * Int_Time_Per_Second)//距绿灯结束前1秒
extern STAGE_TIMING_TABLE_t Tab_C1[Channel_Max_Define];
extern PHASE_PARAM_t Tab_95[Channel_Max_Define];
extern CHANNEL_PARAM_t Tab_B0[Channel_Max_Define];
extern CHANNEL_OUTPUT_STAT_t Tab_B1[Channel_Max_Define];
extern PHASE_OUTPUT_STAT_t Tab_96[Channel_Max_Define];
extern FOLLOW_PHASE_PARAM_t Tab_C8[9];
extern u8 Stage_End_Flag;//阶段结束标志
extern u8 Plan_End_Flag;//方案结束标志
extern u8 Old_Plan;//以前的方案
extern u8 Yellow_End_Flag;//黄灯结束标志

#define Invalid_Scheme_Num    128
//======
extern u16 Tim2_Int_Count;//进入timer 2中断次数
//extern u8 Yel_Flick_Time;//0xA3启动时闪光控制时间
//extern u8 Red_All_Time;//0xA4启动时的全红控制时间
//extern u8 Manual_Control_Scheme;//0xB5手动控制方案
//extern u8 System_Control_Scheme;//0xB6系统控制方案
//extern u8 Control_Mode;//0xB7控制方式
//extern u8 Scheme_Num;//外部控制方案号
//extern u8 Control_Type;//控制方式
//extern u8 Extern_Scheme_Num;//方案号
//extern u8 Today_Control_Mode[16] ;//0x8E表第五行控制方式
#define Invalid_Scheme_Num    128
#define Single_Optimize_Scheme_Num 31 //单点优化方案号
#define System_Optimize_Scheme_Num 32 //系统优化方案号
extern u8 Circulate_Times;
//extern u8 ADC_Usart[4][12];
typedef struct {
	u8 Plan_Num;//方案号
	u8 Last_Plan_Num;//上次的方案号，如果没有，则为Invalid_Scheme_Num
	u8 Period_Time;//周期时长-优化0xB8，其他0xC0
	u8 Phase_Difference;//相位差-优化0XB9，其他0XC0
	u8 Last_Phase_Difference;//上一个方案的相位差
	u8 Coordination_Phase;//协调相位0xC0
	u8 Stage_State;//阶段状态-优化0xBA，其他0xC1
	u8 Stage_Time[16];//阶段时长-优化0xBE，其他0xC1
	u8 Key_Phase_Time[16];//阶段关键相位绿灯时间-优化0xBE，其他0x95
	STAGE_TIMING_TABLE_t Stage_Contex[16];//阶段配时表参数
	u8 Plane_Execute_Time[3];//方案执行时间:第一字节-整点、第二字节-整分0x8E
}Plan_Model;//方案模块结构体

//schedule
//32相位感应方案结构体
typedef struct {
	u8 induction_control;//感应控制-感应允许
	u8 control_mode;//控制方案-感应方案
	u8 stage_opt;//阶段选项-为感应阶段
	u8 detector_error;//检测器故障-检测器正常为1
	
	u8 system_control;//系统控制-为系统控制方式
	u8 system_scheme;//系统方案-为系统控制方案
	
	u8 pedestrian_trigger;//行人触发
	u8 bus_trigger;//公交触发
	u8 motor_trigger;//机动车触发
	u8 handle_flag;//处理标志
	
	u8 fixed_phase;//固定相位
	u8 undetermined_phase;//待定相位
	u8 flexible_phase;//弹性相位
	u8 key_phase;//关键相位
	u8 reserve_phase;//保留相位
	u8 pedestrian_phase;//行人相位
	u8 bicycle_phase;//自行车相位
	u8 motor_phase;//机动车相位
	
	u8 follow_phase_opt;//跟随相位选项
	u8 pedestrian_phase_opt;//行人相位选项
	u8 undetermined_phase_opt;//待定相位选项
	u8 twice_cross_opt;//二次过街选项
	u8 phase_enable_flag;//相位启用标志
	u8 release_phase;//放行相位
	
	u8 phase_release_flag;//相位放行标志
	u16 green_count;//绿灯计数器
	u16 max_green_count;//最大绿灯计数器
	u8 yellow_count;//黄灯计数器
	u8 red_count;//红灯计数器
}Induction_Str; 


extern Plan_Model Plan_Used;//应用模块
extern u8 Plan_Used_Count;//过渡方案选择
//extern u8 Now_Scheme_Index;//当前时间的方案号索引，并不等于方案号
extern PHASE_PARAM_t Phase_Par[16];//相位参数表
//extern u8 Tab_BB_Stepping_Control;//步进控制时的阶段号
extern vu8 Channel_Output_Int_Flag;//通道状态输出中断标志
//extern u8 Stage_Current;//当前阶段号 
//extern u8 Stage_Current_Reset;//当前阶段号 清零
//extern vu8 Count_allows;//是否允许计数
//extern u8 Lock_Count_allows;//是否允许计数lock阶段专用
//extern u8 Guard_Flag;//看守相位标志
//extern u8 Step_Control_Number;//给入步进阶段号
//extern u8 Lock_Flag;//看守标志
//extern u8 Step_Jump_Flag;//步进跳跃标志
//extern u8 Single_Step_Green_start;//单步步进下个阶段绿灯开始
//extern u8 Prio_Control_Mode;//更高优先级控制模式
//extern u8 Prio_Control_End;//更高优先级控制结束
//extern u8 Working_Phase[Channel_Max_Define];//当前正在放行相位
//extern u8 First_Single_Step;//单步第一步
//extern u8 Exit_Keyboard_Control;//退出手动面板控制=1
extern u8 Follow_Flag;//跟随标志
extern u8 Follow_NO;//跟随相位
extern u8 Follow_Pedestrian_NO;//行人跟随相位
extern u8 Follow_Pedestrian_Flag;//行人跟随标志

extern u8 Er_Hdl_Flag;//异常处理标志

extern u16 Bus_Trig;//公交感应
extern u16 Ped_Trig;//行人感应
extern u16 Car_Trig;//机动车感应
//正在执行的数据
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

