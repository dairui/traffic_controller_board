
#include "Include.h"

CONFIG_SCHEME_t Temp_transmit_scheme_diagram1 = {0};
CONFIG_SCHEME_t Temp_transmit_scheme_diagram2 = {0};
CONFIG_SCHEME_t Temp_transmit_scheme_diagram3 = {0};

STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table1 = {0};
STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table2 = {0};
STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table3 = {0};

/*******************************************************************************
  * 调用条件: 没有相位差时使用该过渡方案
  * 函数功能: 根据当前方案周期时间和下方案周期时间计算出过渡方案
  * 输入参数: 当前方案周期时间，下方案周期时间
  * 输出参数: 无
*******************************************************************************/

// void Transimt_method_1(u8 new_cycle_time, u8 old_cycle_time)
// {
//     u8 difference = 0;

//     Temp_transmit_scheme_diagram1.schemeId = Transimt_method1_diagram_num1;      //自己定义的过渡方案方案号
//     Temp_transmit_scheme_diagram2.schemeId = Transimt_method1_diagram_num2;      //可以根据需要在自行更改
//     Temp_transmit_scheme_diagram3.schemeId = Transimt_method1_diagram_num3;

//     difference = old_cycle_time - new_cycle_time;

//     if(difference > 0)                                                              //过渡方案三个阶段，各个阶段周期长度的算法
//     {
//         Temp_transmit_scheme_diagram1.period = (int)((7/15)*old_cycle_time + (8/15)*new_cycle_time);
//         Temp_transmit_scheme_diagram2.period = (int)((3/15)*old_cycle_time + (12/15)*new_cycle_time);
//         Temp_transmit_scheme_diagram3.period = (int)((1/15)*old_cycle_time + (14/15)*new_cycle_time);
//     }
//     else
//     {      
//         Temp_transmit_scheme_diagram1.period = (int)((1/15)*new_cycle_time + (14/15)*old_cycle_time);
//         Temp_transmit_scheme_diagram2.period = (int)((3/15)*new_cycle_time + (12/15)*old_cycle_time);
//         Temp_transmit_scheme_diagram3.period = (int)((7/15)*new_cycle_time + (8/15)*old_cycle_time);
//     }

//     Temp_transmit_scheme_diagram1.phaseDif = 0;                                 //过渡方案相位差0
//     Temp_transmit_scheme_diagram2.phaseDif = 0;
//     Temp_transmit_scheme_diagram3.phaseDif = 0;

//     Temp_transmit_scheme_diagram1.coPhase = 0;                                 //过渡方案协调相位0
//     Temp_transmit_scheme_diagram2.coPhase = 0;
//     Temp_transmit_scheme_diagram3.coPhase = 0;

//     Temp_transmit_scheme_diagram1.stageTimeTb = Transimt_method1_stage_table_num1;    //自己定义的过渡方案阶段配时表号
//     Temp_transmit_scheme_diagram2.stageTimeTb = Transimt_method1_stage_table_num2;    //可以根据需要进行更改
//     Temp_transmit_scheme_diagram3.stageTimeTb = Transimt_method1_stage_table_num3;

//     /*  过渡方案1阶段的阶段配时表 */
//     Temp_transmi_stage_config_table1.index = 1;                                    //阶段配时方案表号(应与原方案相同)
//     Temp_transmi_stage_config_table1.stageId = 1;                                     //阶段号(应与原方案相同)
//     Temp_transmi_stage_config_table1.relPhase = 0x0003;                               //工作相位(应与原方案相同)
//     Temp_transmi_stage_config_table1.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);    //绿灯时间
//     Temp_transmi_stage_config_table1.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);    //黄灯时间
//     Temp_transmi_stage_config_table1.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);       //红灯时间
//     Temp_transmi_stage_config_table1.stAlterParam = 0x00;                                           //阶段选项参数(应与原方案相同)
//                     
//     /*  过渡方案2阶段的阶段配时表 */
//     Temp_transmi_stage_config_table2.index = 1;
//     Temp_transmi_stage_config_table2.stageId = 1;
//     Temp_transmi_stage_config_table2.relPhase = 0x0003;
//     Temp_transmi_stage_config_table2.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table2.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table2.stRedTime = (int)(Temp_transmit_scheme_diagram1.period *0.5);
//     Temp_transmi_stage_config_table2.stAlterParam = 0x00;

//     /*  过渡方案3阶段的阶段配时表 */
//     Temp_transmi_stage_config_table3.index = 1;
//     Temp_transmi_stage_config_table3.stageId = 1;
//     Temp_transmi_stage_config_table3.relPhase = 0x0003;
//     Temp_transmi_stage_config_table3.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table3.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table3.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);
//     Temp_transmi_stage_config_table3.stAlterParam = 0x00;
//    
// }


/*******************************************************************************
  * 调用条件: 有相位差时使用该过渡方案
  * 函数功能: 根据当前方案周期时间和下方案周期时间计算出过渡方案
  * 输入参数: 当前方案周期时间，下方案周期时间，当前方案相位差，下一方案相位差
  * 输出参数: 无
*******************************************************************************/

// void Transimt_method_2(u8 new_cycle_time, u8 old_cycle_time, u8 new_phase_differ, u8 old_phase_differ)
// {
//     u16 temp_cycle_time = 0;

//     CONFIG_SCHEME_t Temp_transmit_scheme_diagram1;//,Temp_transmit_scheme_diagram2,Temp_transmit_scheme_diagram3;

//     //STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table1,Temp_transmi_stage_config_table2,Temp_transmi_stage_config_table3;
//     
//     Temp_transmit_scheme_diagram1.schemeId = Transimt_method1_diagram_num1;      //自己定义的过渡方案方案号
//     Temp_transmit_scheme_diagram2.schemeId = Transimt_method1_diagram_num2;      //可以根据需要在自行更改
//     Temp_transmit_scheme_diagram3.schemeId = Transimt_method1_diagram_num3;
// 		
// 		/*相位差对周期时长取余*/
// 	  new_phase_differ = new_phase_differ % new_cycle_time;
// 		old_phase_differ = old_phase_differ % old_phase_differ;
// 	
//     temp_cycle_time = new_cycle_time + new_phase_differ - old_phase_differ;         //过渡方案三个阶段，各个阶段周期长度的算法

//     if(temp_cycle_time < 0.01)
//     {
//         temp_cycle_time = temp_cycle_time + new_cycle_time;
//     }
//     else if(temp_cycle_time < 0.5*new_cycle_time)
//     {
//         
//         Temp_transmit_scheme_diagram1.period = new_cycle_time + (u8)(temp_cycle_time/3);
//         Temp_transmit_scheme_diagram2.period = new_cycle_time + (u8)(temp_cycle_time/3);
//         Temp_transmit_scheme_diagram3.period = new_cycle_time + temp_cycle_time - 2*(u8)(temp_cycle_time/3);

//     }
//     else if(temp_cycle_time < 1.5*new_cycle_time)
//     {
//         Temp_transmit_scheme_diagram1.period = (u8)((2*new_cycle_time + temp_cycle_time)/3);
//         Temp_transmit_scheme_diagram2.period = (u8)((2*new_cycle_time + temp_cycle_time)/3);
//         Temp_transmit_scheme_diagram3.period = 2*new_cycle_time + temp_cycle_time + 2*(u8)((2*new_cycle_time + temp_cycle_time)/3);

//     }
//     else if(temp_cycle_time < 2*new_cycle_time)
//     {
//         
//         Temp_transmit_scheme_diagram1.period = (u8)((new_cycle_time + temp_cycle_time)/3);
//         Temp_transmit_scheme_diagram2.period = (u8)((new_cycle_time + temp_cycle_time)/3);
//         Temp_transmit_scheme_diagram3.period = new_cycle_time + temp_cycle_time - 2*(u8)((new_cycle_time + temp_cycle_time)/3);

//     }

//     /* 如何确定 ?   */
//     Temp_transmit_scheme_diagram1.phaseDif = 0;   //过渡方案三个阶段不再有相位差
//     Temp_transmit_scheme_diagram2.phaseDif = 0;   //青岛科技大学论文P60?
//     Temp_transmit_scheme_diagram3.phaseDif = 0;

//     /*  如何确定?   */
//     Temp_transmit_scheme_diagram1.coPhase = 0; //过渡方案协调相位(应与原方案相同)
//     Temp_transmit_scheme_diagram2.coPhase = 0;
//     Temp_transmit_scheme_diagram3.coPhase = 0;

//     Temp_transmit_scheme_diagram1.stageTimeTb = Transimt_method1_stage_table_num1;     //自己定义的过渡方案阶段配时表号
//     Temp_transmit_scheme_diagram2.stageTimeTb = Transimt_method1_stage_table_num2;     //可以根据需要进行更改
//     Temp_transmit_scheme_diagram3.stageTimeTb = Transimt_method1_stage_table_num3;

//     /*  过渡方案1的阶段配时表 */
//     Temp_transmi_stage_config_table1.index = 1;
//     Temp_transmi_stage_config_table1.stageId = 1;
//     Temp_transmi_stage_config_table1.relPhase = 0x0003;
//     Temp_transmi_stage_config_table1.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table1.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table1.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);
//     Temp_transmi_stage_config_table1.stAlterParam = 0x00;

//     
//     /*  过渡方案2的阶段配时表 */
//     Temp_transmi_stage_config_table2.index = 1;
//     Temp_transmi_stage_config_table2.stageId = 1;
//     Temp_transmi_stage_config_table2.relPhase = 0x0003;
//     Temp_transmi_stage_config_table2.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table2.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table2.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);
//     Temp_transmi_stage_config_table2.stAlterParam = 0x00;

//     
//     /*  过渡方案3的阶段配时表 */
//     Temp_transmi_stage_config_table3.index = 1;
//     Temp_transmi_stage_config_table3.stageId = 1;
//     Temp_transmi_stage_config_table3.relPhase = 0x0003;
//     Temp_transmi_stage_config_table3.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table3.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period *0.1);
//     Temp_transmi_stage_config_table3.stRedTime = (int)(Temp_transmit_scheme_diagram1.period *0.5);
//     Temp_transmi_stage_config_table3.stAlterParam = 0x00;

// }

/*******************************************************************************
  * 调用条件: 要进行方案过渡时
  * 函数功能: 根据是否有相位差调用哪种过渡方式
  * 输入参数: 相位差的值
  * 输出参数: 无
*******************************************************************************/

// void Transimt_process(u8 old_cycle_time, u8 new_cycle_time, u8 old_phase_differ, u8 new_phase_differ )
// {
//     if((old_phase_differ == 0) && (new_phase_differ == 0))
//     {
//         Transimt_method_1(new_cycle_time, old_cycle_time);
//     }
//     else
//     {
//         Transimt_method_2(new_cycle_time, old_cycle_time, new_phase_differ, old_phase_differ);
//     }

// }
//过渡方案    //by_lm
// // __task void Sche_Interim_Task(void)
// // {
// // 	//Interim_Plan_Model Interim_Scheme[3];   //过渡方案数据结构体
// // 	u8 new_cycle_time;                      //新周期时长(计算周期漂移值c')
// // 	u8 new_phase_differ;                    //新相位差(计算周期漂移值c')
// // 	u8 old_phase_differ;                    //旧相位差(计算周期漂移值c')
// // 	float c;
// // 	u8 i,j,m;
// // 	float stage_time[16];                   //新方案各阶段时长
// // 	u8 stage_time_temp[3][16];              //过渡方案各阶段时长
// // 	u8 temp;
// // 	u32 time_current;
// // 	//u32 plan_time;
// // 	//vu8 Interim_flag = 0;
// // 	u8 *msg;
// // 	u8 *msg_send;
// // 	u8 sche_number;
// // 	u32 time_differ = 0;//方案实际执行时间与计划执行时间之差
// // 	
// // 	u8 Read_Time_Data[7] = {0};
// // 	
// // 	
// // 	for(;;)
// // 	{
// // 		os_mbx_wait(Sche_Interim_Mbox, (void *)&msg, 0xffff);
// // 		sche_number = *msg;

// // 		for(i = 0;i < 3;i++)
// // 		{
// // 			for(j = 0;j < 16;j++)
// // 			{
// // 				stage_time_temp[i][j] = 0;
// // 			}
// // 		}
// // 		if(Plan_Used.Coordination_Phase != 0)//有协调相位,黄闪全红无协调相位
// // 		{			
// // 			os_mut_wait(I2c_Mut_lm,0xFFFF);
// // 			I2C_Read_DS3231(Read_Time_Data);			//  I2C_lm   有待改动的	
// // 			os_mut_release(I2c_Mut_lm);			
// // 		
// // 			time_current = Read_Time_Data[2]* 60*60 + Read_Time_Data[1]*60 + Read_Time_Data[0];//转成秒数
// // 			if(time_current > (Fangan_Hour * 60 * 60 + Fangan_Min * 60))
// // 				time_differ = time_current - (Fangan_Hour * 60 * 60 + Fangan_Min * 60);
// // 			else time_differ = 0;

// // 			//if(Plan_Used.Plane_Execute_Time[0] <= 24)//可添加过渡方案
// // 			//{
// // 			//	plan_time = Plan_Used.Plane_Execute_Time[0] * 60*60 + Plan_Used.Plane_Execute_Time[1]*60 + Plan_Used.Plane_Execute_Time[2];
// // 			//	if((plan_time > 0) && (time_current > plan_time)
// // 			//		&& ((time_current - plan_time) <= Plan_Used.Phase_Difference))
// // 			//	{
// // 			//		Interim_flag = 1;
// // 			//	}
// // 			//}
// // 		//}
// // 		//if(Interim_flag == 1)
// // 		//{
// // 		
// // 			new_cycle_time = Plan_Used.Period_Time;
// // 			new_phase_differ = Plan_Used.Phase_Difference;

// // 			if(new_phase_differ >= time_differ)
// // 				new_phase_differ = (new_phase_differ - time_differ)%new_cycle_time;
// // 			else 
// // 			{
// // 				new_phase_differ = new_cycle_time - (time_differ - new_phase_differ)%new_cycle_time;
// // 			}
// // 			
// // 			//old_phase_differ = Plan_Used.Last_Phase_Difference;
// // 			old_phase_differ = 0;
// // 			//new_phase_differ = new_phase_differ % new_cycle_time;
// // 			c = (float)(new_cycle_time + new_phase_differ - old_phase_differ);

// // 			/*
// // 			填充配时方案表
// // 			temp_config_sche[3]
// // 			周期时长计算得来
// // 			相位差和协调相位均无意义，赋0
// // 			*/     
// // 			//计算周期时长
// // 			if(c < 0)
// // 			{
// // 				c = c + new_cycle_time;
// // 			}
// // 			if((c < (new_cycle_time /2))&&(c > 0.0000001))
// // 			{
// // 				Interim_Scheme[0].Period_Time= new_cycle_time + (u8)(c/3);
// // 				Interim_Scheme[1].Period_Time= new_cycle_time + (u8)(c/3);
// // 				Interim_Scheme[2].Period_Time = new_cycle_time + c - 2*(u8)(c/3);
// // 			}
// // 			else if(c < (new_cycle_time * 3 / 2))
// // 			{
// // 				Interim_Scheme[0].Period_Time = (u8)((2*new_cycle_time + c)/3);
// // 				Interim_Scheme[1].Period_Time = (u8)((2*new_cycle_time + c)/3);
// // 				Interim_Scheme[2].Period_Time = 2*new_cycle_time + c - 2*(u8)((2*new_cycle_time + c)/3);
// // 			}
// // 			else if(c < 2*new_cycle_time)
// // 			{
// // 				Interim_Scheme[0].Period_Time = (u8)((new_cycle_time + c)/3);
// // 				Interim_Scheme[1].Period_Time = (u8)((new_cycle_time + c)/3);
// // 				Interim_Scheme[2].Period_Time = new_cycle_time + c - 2*(u8)((new_cycle_time + c)/3);
// // 			}

// // 			/*
// // 			填充阶段配时表
// // 			temp_interim_stage_config[3]
// // 			阶段号、放行相位与新方案的相同        
// // 			*/		
// // 			i = 0;
// // 			j = 0;
// // 			//获取信号量
// // 			os_mut_wait (Mutex_Data_Update, 0xffff);
// // 			m = Plan_Used.Stage_State;//阶段总数
// // 			//计算放行相位等
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				for(j = 0;j < m;j++)
// // 				{
// // 					Interim_Scheme[i].Stage_Contex[j].index = Plan_Used.Stage_Contex[j].index;              //阶段配时方案号
// // 					Interim_Scheme[i].Stage_Contex[j].stageId = Plan_Used.Stage_Contex[j].stageId;          //阶段号
// // 					Interim_Scheme[i].Stage_Contex[j].relPhase = Plan_Used.Stage_Contex[j].relPhase;        //放行相位
// // 					Interim_Scheme[i].Stage_Contex[j].stAlterParam = Plan_Used.Stage_Contex[j].stAlterParam;//阶段选项参数
// // 				}
// // 			}
// // 			//计算各色灯时间(按新方案个阶段时长比例分配过渡方案各阶段时长，最后一阶段取剩余时间)
// // 			for(j = 0;j < (m - 1);j++)
// // 			{
// // 				stage_time[j] = Plan_Used.Stage_Contex[j].stGreenTime + Plan_Used.Stage_Contex[i].stYellowTime + Plan_Used.Stage_Contex[i].stRedTime;
// // 			}
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				for(j = 0;j < (m - 1);j++)
// // 				{
// // 					stage_time_temp[i][j] = (u8)(stage_time[j] / new_cycle_time * Interim_Scheme[i].Period_Time);
// // 					stage_time_temp[i][m - 1] += stage_time_temp[i][j];//统计前几个阶段时长之和
// // 				}
// // 			}
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				temp = stage_time_temp[i][m - 1];
// // 				stage_time_temp[i][m - 1] = Interim_Scheme[i].Period_Time - temp;
// // 			}
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				for(j = 0;j < m;j++)
// // 				{
// // 					Interim_Scheme[i].Stage_Contex[j].stGreenTime = stage_time_temp[i][j] - 6;
// // 					Interim_Scheme[i].Stage_Contex[j].stYellowTime = 4;
// // 					Interim_Scheme[i].Stage_Contex[j].stRedTime = 2;
// // 				}
// // 			}
// // 			Plan_Used_Count = 1;//指示方案执行任务:当前需要执行过渡方案
// // 		}
// // 		//释放信号量
// // 		os_mut_release (Mutex_Data_Update);
// // 		//发送消息C2
// // 		Sche_Execute_MSG[0] = 2;
// // 		Sche_Execute_MSG[1] = sche_number;
// // 		msg_send = Sche_Execute_MSG;
// // 		os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
// // 	}
// // }

