#ifndef Guo_du_H
#define Guo_du_H

#define  Transimt_method1_diagram_num1 40
#define  Transimt_method1_diagram_num2 41
#define  Transimt_method1_diagram_num3 42

#define  Transimt_method1_stage_table_num1 200
#define  Transimt_method1_stage_table_num2 201
#define  Transimt_method1_stage_table_num3 202

//extern CONFIG_SCHEME_t Temp_transmit_scheme_diagram1,Temp_transmit_scheme_diagram2,Temp_transmit_scheme_diagram3;

extern STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table1,Temp_transmi_stage_config_table2,Temp_transmi_stage_config_table3;

void Transimt_process(u8 old_cycle_time, u8 new_cycle_time, u8 old_phase_differ, u8 new_phase_differ);


typedef struct {
	u8 Period_Time;                         //周期时长
	STAGE_TIMING_TABLE_t Stage_Contex[16];  //阶段配时表参数
}Interim_Plan_Model;                   //过渡方案模块结构体
extern    Interim_Plan_Model Interim_Scheme[3];   //过渡方案数据结构体
#endif
