#include "include.h"
// //***********************************************************************
// // 函数名称：
// // Delay_ms(void)
// //			 
// //说明：ms级延时
// //***********************************************************************
// //us级延时
// void Delay_us(u16 time)
// {
// 	u32 i;
// 	for(i = 0; i < 72 * time; i++)
// 	{
// 		;
// 	}
// }
// void Delay_ms(u16 time)
// {
// 		u32 i;
//     for(i = 0; i < 1000 * time; i++)
//       Delay_us(1);
//     
// }

/******************************************************************************
* 函数功能：发送通道输出状态表
* 入口参数：tmp_channel_output: 通道输出状态表结构体
* 返    回：无
* 备    注：无
******************************************************************************/
void Send_Channel_Output_Unit(CHANNEL_OUTPUT_STAT_t tmp_channel_output,u8 index)
{
	u8 output_channel_unit[8];
        
	output_channel_unit[0] = 0x20;
	output_channel_unit[1] = 0xB1;
	output_channel_unit[2] = 00;
	output_channel_unit[3] = index;
	output_channel_unit[4] = tmp_channel_output.redStat;
	output_channel_unit[5] = tmp_channel_output.yellowStat;
	output_channel_unit[6] = tmp_channel_output.greenStat;
	output_channel_unit[7] = 0xFE;

	CAN_TX(output_channel_unit);
}

//========================================================================================
//u8 Induction_Schedule(struct_stage_config_tbl_unit *Tab_C1, struct_phase_unit *Tab_95)
//感应方案:配套用法，来中断时，在全局数组中给相应位赋值，行人=1；公交=2；机动车=4；
//产生放行相位时，将数组中放行相位对应位=10；至于数组中其他值，可在初始化时
//根据数据库中表里内容赋值。
//return 1重新重头执行此函数
//========================================================================================
u8 Induction_Schedule(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 Judge_Value = 0;
	u32 Insert_Phase = 0;
	u8 Bus_prio_flag = 0;
	u8 Pedestrian_prio_flag = 0;

	for(j = 1;j <= Channel_Max; j++)
	{	
		Schedule_Struct[j].handle_flag = 1;//处理标志全置1
		Schedule_Struct[j].induction_control = 1;//感应控制标志
		Schedule_Struct[j].control_mode = 1;
		Schedule_Struct[j].stage_opt = 1;
		Schedule_Struct[j].detector_error = 1;
	}
	//=12公交+放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 12)
			{
				//处理case1
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					if(Schedule_Struct[j].green_count > Green_Add_Max)//最小绿
					{
						Schedule_Struct[j].bus_trigger = 0;//清触发
					}
					else if((Schedule_Struct[j].green_count <= Green_Add_Max) && (Schedule_Struct[j].green_count >= Green_Add_Min)
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))
					{//绿灯延长
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
						Schedule_Struct[j].bus_trigger = 0;//清触发
					}
				}
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
			}
		}
	}
	//=2公交+不放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 2)
			{
				//处理case2
				Insert_Phase = 0;//插入相位建立函数==================
				//======================================
				if(Insert_Phase != 0)//可以插入公交相位
				{
					Schedule_Struct[j].bus_trigger = 0;//清触发
					return 1;//到程序开始处
				}
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
			}
		}
	}
	//=11行人+放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 11)
			{
				//处理case3
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					Bus_prio_flag = 0;
					for(i = 1; i <= Channel_Max; i++)//是否有公交等待
						Bus_prio_flag += Schedule_Struct[i].bus_trigger;
					if(Schedule_Struct[j].green_count > Green_Add_Max)//最小绿
						Schedule_Struct[j].pedestrian_trigger = 0;//清触发
					else if(Schedule_Struct[j].green_count <= Green_Add_Max 
						&& Schedule_Struct[j].green_count >= Green_Add_Min && Bus_prio_flag == 0
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))
					{//绿灯延长
						Schedule_Struct[j].pedestrian_trigger = 0;//清触发
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
					}
				}
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
			}
		}
	}
	//=1行人+不放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 1)
				//处理case4
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
		}
	}
	//=14车辆+放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 14)
			{
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					//处理case5
					Bus_prio_flag = 0;
					Pedestrian_prio_flag= 0;
					for(i = 1; i <= Channel_Max; i++)//是否有公交等待
						Bus_prio_flag += Schedule_Struct[i].bus_trigger;
					for(i = 1; i <= Channel_Max; i++)//是否有行人等待
						Pedestrian_prio_flag += Schedule_Struct[i].pedestrian_phase;
					if(Schedule_Struct[j].green_count > Green_Add_Max)//最小绿
						Schedule_Struct[j].motor_trigger = 0;//清触发
					else if(Schedule_Struct[j].green_count <= Green_Add_Max && Schedule_Struct[j].green_count >= Green_Add_Min 
						&& Bus_prio_flag == 0 && Pedestrian_prio_flag == 0
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))//公交优先//行人相位
					{//绿灯延长
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
						Schedule_Struct[j].motor_trigger = 0;//清触发
					}
				}
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
			}
		}
	}
	//=4车辆+不放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if( Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 4)
				//处理case6
				Schedule_Struct[j].handle_flag = 0;//处理标志清0？
		}
	}
	//=10没有触发+放行
	Schedule_Struct[4].undetermined_phase = 1;
	//Schedule_Struct[3].pedestrian_phase == 1;
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 10 && Schedule_Struct[j].green_count >= Green_Add_Max)//暂定
			{
				//处理case7
				for(i = 1; i <= Channel_Max; i++)
				{
					if((Schedule_Struct[i].undetermined_phase == 1) && ((Phase_Par[i - 1].phaseType & 0x40) == 0x40)
						&&((Phase_Par[i - 1].phaseFunc & 0x04) == 0x04) && ((Phase_Par[i - 1].phaseFunc & 0x10) == 0x10)
						&& ((Phase_Par[j - 1].phaseType & 0x10) == 0x10) && (Follow_Flag == 1))
					{//待定相位、相位选项bit2 = 1
							Follow_Flag = 0;//跟随添加过了
							Follow_NO = i;
							Schedule_Struct[i].release_phase = 10;//放行此相位
							Schedule_Struct[i].green_count = Schedule_Struct[j].green_count;//绿灯时间=其他相位绿灯时间
							Schedule_Struct[i].max_green_count = Schedule_Struct[j].max_green_count;//最大绿灯时间=其他相位最大绿灯时间
					}
					if((Schedule_Struct[i].pedestrian_phase == 1) && ((Phase_Par[i - 1].phaseType & 0x04) == 0x04)
						&&(Phase_Par[i - 1].phaseFunc & 0x08 == 0x08)	&& (Phase_Par[j - 1].phaseType & 0x10 == 0x10) && (Follow_Pedestrian_Flag == 1))
					{//待定相位、相位选项bit2 = 1
						Follow_Pedestrian_Flag = 0;
						Follow_Pedestrian_NO = i;
						Schedule_Struct[i].release_phase = 10;//放行此相位
						Schedule_Struct[i].green_count = Schedule_Struct[j].green_count;//绿灯时间=其他相位绿灯时间
						Schedule_Struct[i].max_green_count = Schedule_Struct[j].max_green_count;//最大绿灯时间=其他相位最大绿灯时间
					}
				}
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
			}
		}
	}
	//=0没有触发+放行
	for(j = 1; j <= Channel_Max; j++)
	{//行人+ 公交+机动车+放行
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 0)
				//case0
				Schedule_Struct[j].handle_flag = 0;//处理标志清0
		}
	}

	return 1;
}
extern u16 Phase_Collide_Detect(u16 cur_working_phase);
extern OS_SEM eth_client_sche_semaphore;

/*无步进时当前的阶段绿灯计数值*/
extern u16 current_stage_green_count;

//=========================================================================================
//void Fangan_Model(u8 fangan_NO, u8 Stage_NO, u8 Step_En_Flag)
//input:fangan_NO, Tab_A0
//=========================================================================================
void Fangan_Model(u8 fangan_NO, u8 Stage_NO, u8 Step_En_Flag, 
                 Plan_Model   * Plan_Used_temp, Induction_Str * Schedule_Struct_temp)
{
	u8 	i,j;
//	u8 	flag = 0;
	u16 work_phase;//放行相位
//	u8 Collide_Data[6] = {0};//故障上报数组
//	u16 Time_Current;//当前时间=min
//	u8 plan_time;//方案执行时间
	u8 Stage_Current;//当前阶段号 

  u8  Plan_Used_Count_temp = 0;//过渡方案//chen11.29
	Interim_Plan_Model Interim_Scheme_temp[3] = {0};   //过渡方案数据结构体


	//(黄闪方案)一秒进几次定时器中断
	u8	count_per_second = 0;
    //方案模块(函数内局部用)
    //Induction_Str Schedule_Struct_temp[Channel_Max_Define] = {0};//方案模块结构体

	//加锁，或关调度
	//os_mut_wait(Output_Data_Mut,0xFFFF);
	
	//Running_Plan_NO 	= fangan_NO;
	//Running_Stage_NO 	= Stage_NO;
	//解锁，或开调度
	//os_mut_release(Output_Data_Mut);	
	os_mut_wait (Mutex_Data_Update,0xFFFF);	//chen11.29
	Plan_Used_Count_temp = Plan_Used_Count;//过渡方案//chen11.29
	Interim_Scheme_temp[0] = Interim_Scheme[0];
	Interim_Scheme_temp[1] = Interim_Scheme[1];
	Interim_Scheme_temp[2] = Interim_Scheme[2];
	os_mut_release (Mutex_Data_Update);	

	Stage_Current = Stage_NO;
	for(i = 1; i <= Channel_Max; i++)//清0
	{
		Schedule_Struct_temp[i].release_phase 		= 0;
		Schedule_Struct_temp[i].green_count 			= 0;
		Schedule_Struct_temp[i].max_green_count 	= 0;
	}
	
	
	
	if(Plan_Used_Count_temp == 0)//无过渡
	{
		work_phase 		= Plan_Used_temp->Stage_Contex[Stage_Current-1].relPhase;
		work_phase 		= Phase_Follow_Set(work_phase);//跟随相位

		//相位驱动模块
		for(i=0;i < Channel_Max;i++)
		{
			if(((work_phase >> i) & 0x01) == 0x01)
				Schedule_Struct_temp[i+1].release_phase = 10;
			else
				Schedule_Struct_temp[i+1].release_phase = 0;
		}

		if(fangan_NO == 27)		//黄闪方案500ms一闪500ms一灭
		{
			count_per_second = Int_Time_Per_Second_yellow;
		}
		else									//其他方案
		{
			count_per_second = Int_Time_Per_Second;
		}
		for(i = 0; i < Channel_Max; i++)
		{ 
			if(((work_phase >> i) & 0x01) == 0x01)
			{
				Schedule_Struct_temp[i+1].green_count 			= Plan_Used_temp->Stage_Contex[Stage_Current-1].stGreenTime * count_per_second;//绿
				Schedule_Struct_temp[i+1].max_green_count 	= Plan_Used_temp->Stage_Contex[Stage_Current-1].stGreenTime * count_per_second;
				Schedule_Struct_temp[i+1].yellow_count 			= Plan_Used_temp->Stage_Contex[Stage_Current-1].stYellowTime * count_per_second;
				Schedule_Struct_temp[i+1].red_count 				= Plan_Used_temp->Stage_Contex[Stage_Current-1].stRedTime * count_per_second;
			}
			else
			{
				Schedule_Struct_temp[i+1].green_count 			= 0;
				Schedule_Struct_temp[i+1].max_green_count 	= 0;
				Schedule_Struct_temp[i+1].yellow_count 			= 0;
				Schedule_Struct_temp[i+1].red_count 				= 0;
			}
		} 

		/*获取当前数据库中设定的阶段绿灯计数值*/
		current_stage_green_count = Plan_Used_temp->Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;
		
		if(Stage_Current  >=3)
			i++;
		
		if(Step_En_Flag == 1)//步进
		{
			for(i = 1; i <= Channel_Max; i++)
				if(Schedule_Struct_temp[i].green_count > 0)
				{
					Schedule_Struct_temp[i].green_count 			= 0xFFFF;
					Schedule_Struct_temp[i].max_green_count 	= 0xFFFF;
				}
		}
		/*if(Optimize_Data.Optimize_Choose == 1)//优化
		{//优化关键相位绿灯时间
			for(i = 0; i < Channel_Max; i++)
				if(((Optimize_Data.Key_Phase_Time[i]*Int_Time_Per_Second) > (Schedule_Struct_temp[i+1].red_count + Schedule_Struct_temp[i+1].yellow_count + 5*Int_Time_Per_Second))
					&& (Schedule_Struct_temp[i+1].green_count > 0))
				{
					Schedule_Struct_temp[i+1].green_count 			= Optimize_Data.Key_Phase_Time[i]*Int_Time_Per_Second;
					Schedule_Struct_temp[i+1].max_green_count 	= Optimize_Data.Key_Phase_Time[i]*Int_Time_Per_Second;
				}	
		}*/
	}
	else
	{
		work_phase = Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].relPhase;
		work_phase = Phase_Follow_Set(work_phase);//跟随相位
			
		//相位驱动模块
		for(i=0;i < Channel_Max;i++)
		{
			if((work_phase >> i) & 0x01 == 0x01)
				Schedule_Struct_temp[i+1].release_phase 	= 10;
			else
				Schedule_Struct_temp[i+1].release_phase 	= 0;
		}
		for(i=0;i < Channel_Max;i++)
		{ 
			if(((work_phase >> i) & 0x01) == 0x01)
			{
				Schedule_Struct_temp[i+1].green_count 			= Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;//绿
				Schedule_Struct_temp[i+1].max_green_count 	= Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;
				Schedule_Struct_temp[i+1].yellow_count 		= Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stYellowTime * Int_Time_Per_Second;
				Schedule_Struct_temp[i+1].red_count 				= Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stRedTime * Int_Time_Per_Second;
			}
			else
			{
				Schedule_Struct_temp[i+1].green_count 			= 0;
				Schedule_Struct_temp[i+1].max_green_count 	= 0;
				Schedule_Struct_temp[i+1].yellow_count 		= 0;
				Schedule_Struct_temp[i+1].red_count 				= 1;
			}
		}
		/*获取当前过渡时设定的阶段绿灯计数值*/
		current_stage_green_count = Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;
		
		if(Step_En_Flag == 1)//过渡方案   步进
		{
			for(i = 1; i <= Channel_Max; i++)
				if(Schedule_Struct_temp[i].green_count > 0)
				{
					Schedule_Struct_temp[i].green_count 			= 0xFFFF;
					Schedule_Struct_temp[i].max_green_count 	= 0xFFFF;
				}
		}
		
		//改变下一个执行的过渡方案号
		if(Stage_Current == Plan_Used_temp->Stage_State)//该方案执行完，换下一个方案
		{
			if(Plan_Used_Count_temp >= 3)
			{
				Plan_Used_Count_temp = 0;
			}
			else
				Plan_Used_Count_temp ++;
		}
	}

  
	
	
	os_mut_wait (Mutex_Data_Update,0xFFFF);	
	Plan_Used_Count = Plan_Used_Count_temp;
	os_mut_release (Mutex_Data_Update);	
	
}

//=====================================================================================
//void Phase_Follow_Set(void)
//跟随相位放行函数
//=====================================================================================
u16 Phase_Follow_Set(u16 work_phase)
{
	u8 i,j,k;
	u8 B0[16] = {0};
	u8 B0_count = 0;
	u8 C8[8] = {0};
	u8 C8_count = 0;
	u8 flag = 0;
	CHANNEL_PARAM_t Tab_B0_temp[Channel_Max_Define];
	FOLLOW_PHASE_PARAM_t Tab_C8_temp[Channel_Max_Define];
	
	//加锁，或关调度
	os_mut_wait(Output_Data_Mut,0xFFFF);
	//Tab_B0  局部化
	for(i = 0; i < Channel_Max_Define; i++)
	{
		Tab_B0_temp[i].chnId 			= Tab_B0[i].chnId;
		Tab_B0_temp[i].signalSrc 	= Tab_B0[i].signalSrc;
		Tab_B0_temp[i].chnStat 		= Tab_B0[i].chnStat;
		Tab_B0_temp[i].chnCtlType = Tab_B0[i].chnCtlType;
	}
	//Tab_C8  局部化
	for(i = 0; i < Channel_Max_Define; i++)
	{//目前只用到前面4个
		Tab_C8_temp[i].phaseId 				= Tab_C8[i].phaseId;
		Tab_C8_temp[i].operationType 	= Tab_C8[i].operationType;
		Tab_C8_temp[i].incPhaseLenth	= Tab_C8[i].incPhaseLenth;
		for(j = 0; j < 16; j++)
			Tab_C8_temp[i].incPhase[j]			= Tab_C8[i].incPhase[j];
	}
	//解锁，或开调度
	os_mut_release(Output_Data_Mut);	
	
	for(i = 1, j = 0; i <= Channel_Max; i++)
		if(4 == Tab_B0_temp[i].chnCtlType)//跟随相位
		{
			B0[j++] 	= Tab_B0_temp[i].signalSrc;//相位
			B0_count 	= j;
		}
	i=0;j=0;
	for(i = 1; i <= 8; i++)
		if(Tab_C8_temp[i].incPhaseLenth > 1)//跟随相位表
		{
			C8[j++] 	= i;
			C8_count 	= j;
		}

	for(i = 0; i < B0_count; i++)
		for(j = 0; j < C8_count; j++)
			if(Tab_C8_temp[C8[j]].incPhase[0] == B0[i])//找到跟随相位
			{
				for(k = 1; k < Tab_C8_temp[C8[j]].incPhaseLenth; k++)
				{
					if((work_phase & (1 << (Tab_C8_temp[C8[j]].incPhase[k]-1))) != 0)//  Schedule_Struct[Tab_C8[C8[j]].incPhase[k]].release_phase == 10)//相位放行
					{
						work_phase |= (1 << (Tab_C8_temp[C8[j]].incPhase[0] - 1));//Schedule_Struct[Tab_C8[C8[j]].incPhase[0]].release_phase = 10;
						flag = 1;
					}
				}
				if(flag == 0)
					work_phase &= (~(1 << (Tab_C8_temp[C8[j]].incPhase[0] - 1)));
					
			}
	return work_phase;
}


//======================================================================
// 函数名称：
// u8 Check_Scheme(u8 scheme_number)
//输入:scheme_number需要校验的方案号
//返回:0方案号未配置，1方案号有效
//======================================================================
u8 Check_Scheme(u8 scheme_number)
{
	
		return 1;

}

