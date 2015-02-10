#include "include.h"
// //***********************************************************************
// // �������ƣ�
// // Delay_ms(void)
// //			 
// //˵����ms����ʱ
// //***********************************************************************
// //us����ʱ
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
* �������ܣ�����ͨ�����״̬��
* ��ڲ�����tmp_channel_output: ͨ�����״̬��ṹ��
* ��    �أ���
* ��    ע����
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
//��Ӧ����:�����÷������ж�ʱ����ȫ�������и���Ӧλ��ֵ������=1������=2��������=4��
//����������λʱ���������з�����λ��Ӧλ=10����������������ֵ�����ڳ�ʼ��ʱ
//�������ݿ��б������ݸ�ֵ��
//return 1������ͷִ�д˺���
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
		Schedule_Struct[j].handle_flag = 1;//�����־ȫ��1
		Schedule_Struct[j].induction_control = 1;//��Ӧ���Ʊ�־
		Schedule_Struct[j].control_mode = 1;
		Schedule_Struct[j].stage_opt = 1;
		Schedule_Struct[j].detector_error = 1;
	}
	//=12����+����
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 12)
			{
				//����case1
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					if(Schedule_Struct[j].green_count > Green_Add_Max)//��С��
					{
						Schedule_Struct[j].bus_trigger = 0;//�崥��
					}
					else if((Schedule_Struct[j].green_count <= Green_Add_Max) && (Schedule_Struct[j].green_count >= Green_Add_Min)
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))
					{//�̵��ӳ�
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
						Schedule_Struct[j].bus_trigger = 0;//�崥��
					}
				}
				Schedule_Struct[j].handle_flag = 0;//�����־��0
			}
		}
	}
	//=2����+������
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 2)
			{
				//����case2
				Insert_Phase = 0;//������λ��������==================
				//======================================
				if(Insert_Phase != 0)//���Բ��빫����λ
				{
					Schedule_Struct[j].bus_trigger = 0;//�崥��
					return 1;//������ʼ��
				}
				Schedule_Struct[j].handle_flag = 0;//�����־��0
			}
		}
	}
	//=11����+����
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 11)
			{
				//����case3
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					Bus_prio_flag = 0;
					for(i = 1; i <= Channel_Max; i++)//�Ƿ��й����ȴ�
						Bus_prio_flag += Schedule_Struct[i].bus_trigger;
					if(Schedule_Struct[j].green_count > Green_Add_Max)//��С��
						Schedule_Struct[j].pedestrian_trigger = 0;//�崥��
					else if(Schedule_Struct[j].green_count <= Green_Add_Max 
						&& Schedule_Struct[j].green_count >= Green_Add_Min && Bus_prio_flag == 0
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))
					{//�̵��ӳ�
						Schedule_Struct[j].pedestrian_trigger = 0;//�崥��
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
					}
				}
				Schedule_Struct[j].handle_flag = 0;//�����־��0
			}
		}
	}
	//=1����+������
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 1)
				//����case4
				Schedule_Struct[j].handle_flag = 0;//�����־��0
		}
	}
	//=14����+����
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 14)
			{
				if(Schedule_Struct[j].green_count > 0 && Schedule_Struct[j].max_green_count > 0)
				{
					//����case5
					Bus_prio_flag = 0;
					Pedestrian_prio_flag= 0;
					for(i = 1; i <= Channel_Max; i++)//�Ƿ��й����ȴ�
						Bus_prio_flag += Schedule_Struct[i].bus_trigger;
					for(i = 1; i <= Channel_Max; i++)//�Ƿ������˵ȴ�
						Pedestrian_prio_flag += Schedule_Struct[i].pedestrian_phase;
					if(Schedule_Struct[j].green_count > Green_Add_Max)//��С��
						Schedule_Struct[j].motor_trigger = 0;//�崥��
					else if(Schedule_Struct[j].green_count <= Green_Add_Max && Schedule_Struct[j].green_count >= Green_Add_Min 
						&& Bus_prio_flag == 0 && Pedestrian_prio_flag == 0
						&& ((Phase_Par[j - 1].expandGreen * Int_Time_Per_Second + Schedule_Struct[j].green_count) <= Schedule_Struct[j].max_green_count))//��������//������λ
					{//�̵��ӳ�
						Schedule_Struct[j].green_count += (Phase_Par[j - 1].expandGreen* Int_Time_Per_Second);
						Schedule_Struct[j].motor_trigger = 0;//�崥��
					}
				}
				Schedule_Struct[j].handle_flag = 0;//�����־��0
			}
		}
	}
	//=4����+������
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if( Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 4)
				//����case6
				Schedule_Struct[j].handle_flag = 0;//�����־��0��
		}
	}
	//=10û�д���+����
	Schedule_Struct[4].undetermined_phase = 1;
	//Schedule_Struct[3].pedestrian_phase == 1;
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 10 && Schedule_Struct[j].green_count >= Green_Add_Max)//�ݶ�
			{
				//����case7
				for(i = 1; i <= Channel_Max; i++)
				{
					if((Schedule_Struct[i].undetermined_phase == 1) && ((Phase_Par[i - 1].phaseType & 0x40) == 0x40)
						&&((Phase_Par[i - 1].phaseFunc & 0x04) == 0x04) && ((Phase_Par[i - 1].phaseFunc & 0x10) == 0x10)
						&& ((Phase_Par[j - 1].phaseType & 0x10) == 0x10) && (Follow_Flag == 1))
					{//������λ����λѡ��bit2 = 1
							Follow_Flag = 0;//������ӹ���
							Follow_NO = i;
							Schedule_Struct[i].release_phase = 10;//���д���λ
							Schedule_Struct[i].green_count = Schedule_Struct[j].green_count;//�̵�ʱ��=������λ�̵�ʱ��
							Schedule_Struct[i].max_green_count = Schedule_Struct[j].max_green_count;//����̵�ʱ��=������λ����̵�ʱ��
					}
					if((Schedule_Struct[i].pedestrian_phase == 1) && ((Phase_Par[i - 1].phaseType & 0x04) == 0x04)
						&&(Phase_Par[i - 1].phaseFunc & 0x08 == 0x08)	&& (Phase_Par[j - 1].phaseType & 0x10 == 0x10) && (Follow_Pedestrian_Flag == 1))
					{//������λ����λѡ��bit2 = 1
						Follow_Pedestrian_Flag = 0;
						Follow_Pedestrian_NO = i;
						Schedule_Struct[i].release_phase = 10;//���д���λ
						Schedule_Struct[i].green_count = Schedule_Struct[j].green_count;//�̵�ʱ��=������λ�̵�ʱ��
						Schedule_Struct[i].max_green_count = Schedule_Struct[j].max_green_count;//����̵�ʱ��=������λ����̵�ʱ��
					}
				}
				Schedule_Struct[j].handle_flag = 0;//�����־��0
			}
		}
	}
	//=0û�д���+����
	for(j = 1; j <= Channel_Max; j++)
	{//����+ ����+������+����
		if(Schedule_Struct[j].handle_flag == 1)
		{
			Judge_Value = Schedule_Struct[j].pedestrian_trigger + Schedule_Struct[j].bus_trigger + Schedule_Struct[j].motor_trigger + Schedule_Struct[j].release_phase;
			if(Judge_Value == 0)
				//case0
				Schedule_Struct[j].handle_flag = 0;//�����־��0
		}
	}

	return 1;
}
extern u16 Phase_Collide_Detect(u16 cur_working_phase);
extern OS_SEM eth_client_sche_semaphore;

/*�޲���ʱ��ǰ�Ľ׶��̵Ƽ���ֵ*/
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
	u16 work_phase;//������λ
//	u8 Collide_Data[6] = {0};//�����ϱ�����
//	u16 Time_Current;//��ǰʱ��=min
//	u8 plan_time;//����ִ��ʱ��
	u8 Stage_Current;//��ǰ�׶κ� 

  u8  Plan_Used_Count_temp = 0;//���ɷ���//chen11.29
	Interim_Plan_Model Interim_Scheme_temp[3] = {0};   //���ɷ������ݽṹ��


	//(��������)һ������ζ�ʱ���ж�
	u8	count_per_second = 0;
    //����ģ��(�����ھֲ���)
    //Induction_Str Schedule_Struct_temp[Channel_Max_Define] = {0};//����ģ��ṹ��

	//��������ص���
	//os_mut_wait(Output_Data_Mut,0xFFFF);
	
	//Running_Plan_NO 	= fangan_NO;
	//Running_Stage_NO 	= Stage_NO;
	//�������򿪵���
	//os_mut_release(Output_Data_Mut);	
	os_mut_wait (Mutex_Data_Update,0xFFFF);	//chen11.29
	Plan_Used_Count_temp = Plan_Used_Count;//���ɷ���//chen11.29
	Interim_Scheme_temp[0] = Interim_Scheme[0];
	Interim_Scheme_temp[1] = Interim_Scheme[1];
	Interim_Scheme_temp[2] = Interim_Scheme[2];
	os_mut_release (Mutex_Data_Update);	

	Stage_Current = Stage_NO;
	for(i = 1; i <= Channel_Max; i++)//��0
	{
		Schedule_Struct_temp[i].release_phase 		= 0;
		Schedule_Struct_temp[i].green_count 			= 0;
		Schedule_Struct_temp[i].max_green_count 	= 0;
	}
	
	
	
	if(Plan_Used_Count_temp == 0)//�޹���
	{
		work_phase 		= Plan_Used_temp->Stage_Contex[Stage_Current-1].relPhase;
		work_phase 		= Phase_Follow_Set(work_phase);//������λ

		//��λ����ģ��
		for(i=0;i < Channel_Max;i++)
		{
			if(((work_phase >> i) & 0x01) == 0x01)
				Schedule_Struct_temp[i+1].release_phase = 10;
			else
				Schedule_Struct_temp[i+1].release_phase = 0;
		}

		if(fangan_NO == 27)		//��������500msһ��500msһ��
		{
			count_per_second = Int_Time_Per_Second_yellow;
		}
		else									//��������
		{
			count_per_second = Int_Time_Per_Second;
		}
		for(i = 0; i < Channel_Max; i++)
		{ 
			if(((work_phase >> i) & 0x01) == 0x01)
			{
				Schedule_Struct_temp[i+1].green_count 			= Plan_Used_temp->Stage_Contex[Stage_Current-1].stGreenTime * count_per_second;//��
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

		/*��ȡ��ǰ���ݿ����趨�Ľ׶��̵Ƽ���ֵ*/
		current_stage_green_count = Plan_Used_temp->Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;
		
		if(Stage_Current  >=3)
			i++;
		
		if(Step_En_Flag == 1)//����
		{
			for(i = 1; i <= Channel_Max; i++)
				if(Schedule_Struct_temp[i].green_count > 0)
				{
					Schedule_Struct_temp[i].green_count 			= 0xFFFF;
					Schedule_Struct_temp[i].max_green_count 	= 0xFFFF;
				}
		}
		/*if(Optimize_Data.Optimize_Choose == 1)//�Ż�
		{//�Ż��ؼ���λ�̵�ʱ��
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
		work_phase = Phase_Follow_Set(work_phase);//������λ
			
		//��λ����ģ��
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
				Schedule_Struct_temp[i+1].green_count 			= Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;//��
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
		/*��ȡ��ǰ����ʱ�趨�Ľ׶��̵Ƽ���ֵ*/
		current_stage_green_count = Interim_Scheme_temp[Plan_Used_Count_temp - 1].Stage_Contex[Stage_Current-1].stGreenTime * Int_Time_Per_Second;
		
		if(Step_En_Flag == 1)//���ɷ���   ����
		{
			for(i = 1; i <= Channel_Max; i++)
				if(Schedule_Struct_temp[i].green_count > 0)
				{
					Schedule_Struct_temp[i].green_count 			= 0xFFFF;
					Schedule_Struct_temp[i].max_green_count 	= 0xFFFF;
				}
		}
		
		//�ı���һ��ִ�еĹ��ɷ�����
		if(Stage_Current == Plan_Used_temp->Stage_State)//�÷���ִ���꣬����һ������
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
//������λ���к���
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
	
	//��������ص���
	os_mut_wait(Output_Data_Mut,0xFFFF);
	//Tab_B0  �ֲ���
	for(i = 0; i < Channel_Max_Define; i++)
	{
		Tab_B0_temp[i].chnId 			= Tab_B0[i].chnId;
		Tab_B0_temp[i].signalSrc 	= Tab_B0[i].signalSrc;
		Tab_B0_temp[i].chnStat 		= Tab_B0[i].chnStat;
		Tab_B0_temp[i].chnCtlType = Tab_B0[i].chnCtlType;
	}
	//Tab_C8  �ֲ���
	for(i = 0; i < Channel_Max_Define; i++)
	{//Ŀǰֻ�õ�ǰ��4��
		Tab_C8_temp[i].phaseId 				= Tab_C8[i].phaseId;
		Tab_C8_temp[i].operationType 	= Tab_C8[i].operationType;
		Tab_C8_temp[i].incPhaseLenth	= Tab_C8[i].incPhaseLenth;
		for(j = 0; j < 16; j++)
			Tab_C8_temp[i].incPhase[j]			= Tab_C8[i].incPhase[j];
	}
	//�������򿪵���
	os_mut_release(Output_Data_Mut);	
	
	for(i = 1, j = 0; i <= Channel_Max; i++)
		if(4 == Tab_B0_temp[i].chnCtlType)//������λ
		{
			B0[j++] 	= Tab_B0_temp[i].signalSrc;//��λ
			B0_count 	= j;
		}
	i=0;j=0;
	for(i = 1; i <= 8; i++)
		if(Tab_C8_temp[i].incPhaseLenth > 1)//������λ��
		{
			C8[j++] 	= i;
			C8_count 	= j;
		}

	for(i = 0; i < B0_count; i++)
		for(j = 0; j < C8_count; j++)
			if(Tab_C8_temp[C8[j]].incPhase[0] == B0[i])//�ҵ�������λ
			{
				for(k = 1; k < Tab_C8_temp[C8[j]].incPhaseLenth; k++)
				{
					if((work_phase & (1 << (Tab_C8_temp[C8[j]].incPhase[k]-1))) != 0)//  Schedule_Struct[Tab_C8[C8[j]].incPhase[k]].release_phase == 10)//��λ����
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
// �������ƣ�
// u8 Check_Scheme(u8 scheme_number)
//����:scheme_number��ҪУ��ķ�����
//����:0������δ���ã�1��������Ч
//======================================================================
u8 Check_Scheme(u8 scheme_number)
{
	
		return 1;

}

