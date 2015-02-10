
#include "Include.h"

CONFIG_SCHEME_t Temp_transmit_scheme_diagram1 = {0};
CONFIG_SCHEME_t Temp_transmit_scheme_diagram2 = {0};
CONFIG_SCHEME_t Temp_transmit_scheme_diagram3 = {0};

STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table1 = {0};
STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table2 = {0};
STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table3 = {0};

/*******************************************************************************
  * ��������: û����λ��ʱʹ�øù��ɷ���
  * ��������: ���ݵ�ǰ��������ʱ����·�������ʱ���������ɷ���
  * �������: ��ǰ��������ʱ�䣬�·�������ʱ��
  * �������: ��
*******************************************************************************/

// void Transimt_method_1(u8 new_cycle_time, u8 old_cycle_time)
// {
//     u8 difference = 0;

//     Temp_transmit_scheme_diagram1.schemeId = Transimt_method1_diagram_num1;      //�Լ�����Ĺ��ɷ���������
//     Temp_transmit_scheme_diagram2.schemeId = Transimt_method1_diagram_num2;      //���Ը�����Ҫ�����и���
//     Temp_transmit_scheme_diagram3.schemeId = Transimt_method1_diagram_num3;

//     difference = old_cycle_time - new_cycle_time;

//     if(difference > 0)                                                              //���ɷ��������׶Σ������׶����ڳ��ȵ��㷨
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

//     Temp_transmit_scheme_diagram1.phaseDif = 0;                                 //���ɷ�����λ��0
//     Temp_transmit_scheme_diagram2.phaseDif = 0;
//     Temp_transmit_scheme_diagram3.phaseDif = 0;

//     Temp_transmit_scheme_diagram1.coPhase = 0;                                 //���ɷ���Э����λ0
//     Temp_transmit_scheme_diagram2.coPhase = 0;
//     Temp_transmit_scheme_diagram3.coPhase = 0;

//     Temp_transmit_scheme_diagram1.stageTimeTb = Transimt_method1_stage_table_num1;    //�Լ�����Ĺ��ɷ����׶���ʱ���
//     Temp_transmit_scheme_diagram2.stageTimeTb = Transimt_method1_stage_table_num2;    //���Ը�����Ҫ���и���
//     Temp_transmit_scheme_diagram3.stageTimeTb = Transimt_method1_stage_table_num3;

//     /*  ���ɷ���1�׶εĽ׶���ʱ�� */
//     Temp_transmi_stage_config_table1.index = 1;                                    //�׶���ʱ�������(Ӧ��ԭ������ͬ)
//     Temp_transmi_stage_config_table1.stageId = 1;                                     //�׶κ�(Ӧ��ԭ������ͬ)
//     Temp_transmi_stage_config_table1.relPhase = 0x0003;                               //������λ(Ӧ��ԭ������ͬ)
//     Temp_transmi_stage_config_table1.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);    //�̵�ʱ��
//     Temp_transmi_stage_config_table1.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);    //�Ƶ�ʱ��
//     Temp_transmi_stage_config_table1.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);       //���ʱ��
//     Temp_transmi_stage_config_table1.stAlterParam = 0x00;                                           //�׶�ѡ�����(Ӧ��ԭ������ͬ)
//                     
//     /*  ���ɷ���2�׶εĽ׶���ʱ�� */
//     Temp_transmi_stage_config_table2.index = 1;
//     Temp_transmi_stage_config_table2.stageId = 1;
//     Temp_transmi_stage_config_table2.relPhase = 0x0003;
//     Temp_transmi_stage_config_table2.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table2.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table2.stRedTime = (int)(Temp_transmit_scheme_diagram1.period *0.5);
//     Temp_transmi_stage_config_table2.stAlterParam = 0x00;

//     /*  ���ɷ���3�׶εĽ׶���ʱ�� */
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
  * ��������: ����λ��ʱʹ�øù��ɷ���
  * ��������: ���ݵ�ǰ��������ʱ����·�������ʱ���������ɷ���
  * �������: ��ǰ��������ʱ�䣬�·�������ʱ�䣬��ǰ������λ���һ������λ��
  * �������: ��
*******************************************************************************/

// void Transimt_method_2(u8 new_cycle_time, u8 old_cycle_time, u8 new_phase_differ, u8 old_phase_differ)
// {
//     u16 temp_cycle_time = 0;

//     CONFIG_SCHEME_t Temp_transmit_scheme_diagram1;//,Temp_transmit_scheme_diagram2,Temp_transmit_scheme_diagram3;

//     //STAGE_TIMING_TABLE_t Temp_transmi_stage_config_table1,Temp_transmi_stage_config_table2,Temp_transmi_stage_config_table3;
//     
//     Temp_transmit_scheme_diagram1.schemeId = Transimt_method1_diagram_num1;      //�Լ�����Ĺ��ɷ���������
//     Temp_transmit_scheme_diagram2.schemeId = Transimt_method1_diagram_num2;      //���Ը�����Ҫ�����и���
//     Temp_transmit_scheme_diagram3.schemeId = Transimt_method1_diagram_num3;
// 		
// 		/*��λ�������ʱ��ȡ��*/
// 	  new_phase_differ = new_phase_differ % new_cycle_time;
// 		old_phase_differ = old_phase_differ % old_phase_differ;
// 	
//     temp_cycle_time = new_cycle_time + new_phase_differ - old_phase_differ;         //���ɷ��������׶Σ������׶����ڳ��ȵ��㷨

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

//     /* ���ȷ�� ?   */
//     Temp_transmit_scheme_diagram1.phaseDif = 0;   //���ɷ��������׶β�������λ��
//     Temp_transmit_scheme_diagram2.phaseDif = 0;   //�ൺ�Ƽ���ѧ����P60?
//     Temp_transmit_scheme_diagram3.phaseDif = 0;

//     /*  ���ȷ��?   */
//     Temp_transmit_scheme_diagram1.coPhase = 0; //���ɷ���Э����λ(Ӧ��ԭ������ͬ)
//     Temp_transmit_scheme_diagram2.coPhase = 0;
//     Temp_transmit_scheme_diagram3.coPhase = 0;

//     Temp_transmit_scheme_diagram1.stageTimeTb = Transimt_method1_stage_table_num1;     //�Լ�����Ĺ��ɷ����׶���ʱ���
//     Temp_transmit_scheme_diagram2.stageTimeTb = Transimt_method1_stage_table_num2;     //���Ը�����Ҫ���и���
//     Temp_transmit_scheme_diagram3.stageTimeTb = Transimt_method1_stage_table_num3;

//     /*  ���ɷ���1�Ľ׶���ʱ�� */
//     Temp_transmi_stage_config_table1.index = 1;
//     Temp_transmi_stage_config_table1.stageId = 1;
//     Temp_transmi_stage_config_table1.relPhase = 0x0003;
//     Temp_transmi_stage_config_table1.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table1.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table1.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);
//     Temp_transmi_stage_config_table1.stAlterParam = 0x00;

//     
//     /*  ���ɷ���2�Ľ׶���ʱ�� */
//     Temp_transmi_stage_config_table2.index = 1;
//     Temp_transmi_stage_config_table2.stageId = 1;
//     Temp_transmi_stage_config_table2.relPhase = 0x0003;
//     Temp_transmi_stage_config_table2.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table2.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period * 0.1);
//     Temp_transmi_stage_config_table2.stRedTime = (int)(Temp_transmit_scheme_diagram1.period * 0.5);
//     Temp_transmi_stage_config_table2.stAlterParam = 0x00;

//     
//     /*  ���ɷ���3�Ľ׶���ʱ�� */
//     Temp_transmi_stage_config_table3.index = 1;
//     Temp_transmi_stage_config_table3.stageId = 1;
//     Temp_transmi_stage_config_table3.relPhase = 0x0003;
//     Temp_transmi_stage_config_table3.stGreenTime = (int)(Temp_transmit_scheme_diagram1.period * 0.4);
//     Temp_transmi_stage_config_table3.stYellowTime = (int)(Temp_transmit_scheme_diagram1.period *0.1);
//     Temp_transmi_stage_config_table3.stRedTime = (int)(Temp_transmit_scheme_diagram1.period *0.5);
//     Temp_transmi_stage_config_table3.stAlterParam = 0x00;

// }

/*******************************************************************************
  * ��������: Ҫ���з�������ʱ
  * ��������: �����Ƿ�����λ��������ֹ��ɷ�ʽ
  * �������: ��λ���ֵ
  * �������: ��
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
//���ɷ���    //by_lm
// // __task void Sche_Interim_Task(void)
// // {
// // 	//Interim_Plan_Model Interim_Scheme[3];   //���ɷ������ݽṹ��
// // 	u8 new_cycle_time;                      //������ʱ��(��������Ư��ֵc')
// // 	u8 new_phase_differ;                    //����λ��(��������Ư��ֵc')
// // 	u8 old_phase_differ;                    //����λ��(��������Ư��ֵc')
// // 	float c;
// // 	u8 i,j,m;
// // 	float stage_time[16];                   //�·������׶�ʱ��
// // 	u8 stage_time_temp[3][16];              //���ɷ������׶�ʱ��
// // 	u8 temp;
// // 	u32 time_current;
// // 	//u32 plan_time;
// // 	//vu8 Interim_flag = 0;
// // 	u8 *msg;
// // 	u8 *msg_send;
// // 	u8 sche_number;
// // 	u32 time_differ = 0;//����ʵ��ִ��ʱ����ƻ�ִ��ʱ��֮��
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
// // 		if(Plan_Used.Coordination_Phase != 0)//��Э����λ,����ȫ����Э����λ
// // 		{			
// // 			os_mut_wait(I2c_Mut_lm,0xFFFF);
// // 			I2C_Read_DS3231(Read_Time_Data);			//  I2C_lm   �д��Ķ���	
// // 			os_mut_release(I2c_Mut_lm);			
// // 		
// // 			time_current = Read_Time_Data[2]* 60*60 + Read_Time_Data[1]*60 + Read_Time_Data[0];//ת������
// // 			if(time_current > (Fangan_Hour * 60 * 60 + Fangan_Min * 60))
// // 				time_differ = time_current - (Fangan_Hour * 60 * 60 + Fangan_Min * 60);
// // 			else time_differ = 0;

// // 			//if(Plan_Used.Plane_Execute_Time[0] <= 24)//����ӹ��ɷ���
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
// // 			�����ʱ������
// // 			temp_config_sche[3]
// // 			����ʱ���������
// // 			��λ���Э����λ�������壬��0
// // 			*/     
// // 			//��������ʱ��
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
// // 			���׶���ʱ��
// // 			temp_interim_stage_config[3]
// // 			�׶κš�������λ���·�������ͬ        
// // 			*/		
// // 			i = 0;
// // 			j = 0;
// // 			//��ȡ�ź���
// // 			os_mut_wait (Mutex_Data_Update, 0xffff);
// // 			m = Plan_Used.Stage_State;//�׶�����
// // 			//���������λ��
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				for(j = 0;j < m;j++)
// // 				{
// // 					Interim_Scheme[i].Stage_Contex[j].index = Plan_Used.Stage_Contex[j].index;              //�׶���ʱ������
// // 					Interim_Scheme[i].Stage_Contex[j].stageId = Plan_Used.Stage_Contex[j].stageId;          //�׶κ�
// // 					Interim_Scheme[i].Stage_Contex[j].relPhase = Plan_Used.Stage_Contex[j].relPhase;        //������λ
// // 					Interim_Scheme[i].Stage_Contex[j].stAlterParam = Plan_Used.Stage_Contex[j].stAlterParam;//�׶�ѡ�����
// // 				}
// // 			}
// // 			//�����ɫ��ʱ��(���·������׶�ʱ������������ɷ������׶�ʱ�������һ�׶�ȡʣ��ʱ��)
// // 			for(j = 0;j < (m - 1);j++)
// // 			{
// // 				stage_time[j] = Plan_Used.Stage_Contex[j].stGreenTime + Plan_Used.Stage_Contex[i].stYellowTime + Plan_Used.Stage_Contex[i].stRedTime;
// // 			}
// // 			for(i = 0;i < 3;i++)
// // 			{
// // 				for(j = 0;j < (m - 1);j++)
// // 				{
// // 					stage_time_temp[i][j] = (u8)(stage_time[j] / new_cycle_time * Interim_Scheme[i].Period_Time);
// // 					stage_time_temp[i][m - 1] += stage_time_temp[i][j];//ͳ��ǰ�����׶�ʱ��֮��
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
// // 			Plan_Used_Count = 1;//ָʾ����ִ������:��ǰ��Ҫִ�й��ɷ���
// // 		}
// // 		//�ͷ��ź���
// // 		os_mut_release (Mutex_Data_Update);
// // 		//������ϢC2
// // 		Sche_Execute_MSG[0] = 2;
// // 		Sche_Execute_MSG[1] = sche_number;
// // 		msg_send = Sche_Execute_MSG;
// // 		os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
// // 	}
// // }

