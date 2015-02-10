#include "Include.h"

//====================lxblllllllllllllllllll
STAGE_TIMING_TABLE_t 			sche_hold_guand = {0};		//�������ݽṹ�壬�ýṹ���õ���C1��ṹ
sche_optimize_struct			sche_optimize 	= {0};		//�Ż����ݽṹ�壬�ýṹ��Ϊ�Զ���Ľṹ


//===================


u8  sche_change_tpye[][4] =
{
/*
�ѿ��������Ϳ���ȫ��Ҳ����һ�ֿ������ͣ�
���Ʒ�ʽ��Ϊ0�������˿���������Ч����Ϊ0��ִ�иÿ�������		
*/
	
/* ����				��������		������			���Ʒ�ʽ*/
	{0,					0,					0,					0},   /*	��������			*/
	{1,					1,					0,					0},		/*	����ȫ��			*/
	{2,					2,					0,					0},		/*	����	0xBB		*/
	{3,					3,					0,					0},		/*	�ֶ�  0xB5 		*/
	{4,					4,					0,					0},		/*	ϵͳ  0xB6		*/
	{5,					0,					0,					0},		/*	��������			*/
	{6,					0,					0,					0},		/*	��������			*/
	{7,					0,					0,					0},		/*	��ǰ���Ʒ���	*/
};



//====================lxblllllllllllllllllll

/*==============================================================================================
* �﷨��ʽ��    u8 rt_operate_written(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    Э������󣬽�ʵʱ������������д��rt_operate_data_struct�ṹ����
* ����1��       PROTOCOL_DATA *p_Protocol ����Э��������������ݽṹ�嵽�ú���
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
u8 rt_operate_written(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
{
	u8 				i,	j;
	u8 				index;								//��������д�룬index++
	u8 *   msg_to_dispatch = NULL;			//����������Ϣ����
	
 	static u8				to_dispatch_msg[5];		//���͸���������������Ϣ
	
	
	u8 *   p_Char     =NULL;
	u32		 k					=0;	
	
	
	switch (p_Protocol->count_target)
	{
		case 1:								//���������ã�����BB��B5��B6��B7��
			{		
				if (p_Protocol->opobj_data[0].ID  			 == 0xB6 &&
						p_Protocol->opobj_data[0].sub_buf[0] == 0)//B6��,ֻ�ܵ�������Ϊ0����������
				{
					/* �˴�����Ϣ����ʽ���͸�������������
					 * Э��������ֲ������sche_change_tpye����
					 * ������鲿���ڷ�����������ĩβ
					 */
// 					os_mut_wait(RT_Operate_Mut,0xFFFF);   //���ڷ������������޸�
// 					sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[0].sub_buf[0];//��b6��ֵ������
// 					os_mut_release(RT_Operate_Mut);
					/*��������B6��ֻ������Ϊ�ָ�	*/
// // 					to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[0].sub_buf[0];
					to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
					to_dispatch_msg[3]  = 0xb6;
				} 
				else if (p_Protocol->opobj_data[0].ID  == 0xB7)
				{
					/*��������B7��ֻ������ȫ�����	*/
					if ((p_Protocol->opobj_data[0].sub_buf[0] >= YELLOW_WAY) && 
							(p_Protocol->opobj_data[0].sub_buf[0] <= RED_WAY))
					{
						to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
						to_dispatch_msg[3]  = 0xb6;
					}
					else
					{
						return (__FALSE);
					}
					
				}
				else
				{
					return (__FALSE);
				}
			}
			break;
		case 2:								//�����������ã�B7����B6��
			{
				if ((p_Protocol->opobj_data[0].ID  == 0xB7) && (p_Protocol->opobj_data[1].ID  == 0xB6))
				{
					/* �˴�����Ϣ����ʽ���͸�������������
					 * Э��������ֲ������sche_change_tpye����
					 * ������鲿���ڷ�����������ĩβ
					 */
// 					os_mut_wait(RT_Operate_Mut,0xFFFF);   //���ڷ������������޸�
// 					
// 					sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];//��ϵͳ����B6���ÿ��Ʒ�ʽ
// 					sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];			//��b6��ֵ������
// 					
// 					os_mut_release(RT_Operate_Mut);
					if (p_Protocol->opobj_data[0].sub_buf[0] != 0 &&
						  p_Protocol->opobj_data[1].sub_buf[0] != 0)
					{
						to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
					}
					else if(p_Protocol->opobj_data[1].sub_buf[0] == 0)//�ָ���������
					{
						to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[1].sub_buf[0];
					}
					else
					{
						return __FALSE;
					}
				}
				else
				{
					return (__FALSE);
				}
			}
			break;
		case 3:								//�����������ã�����ΪB7��B6��C1����B7��ֵΪ0x04
			{
				if ((p_Protocol->opobj_data[0].ID  == 0xB7) && 
				(p_Protocol->opobj_data[1].ID  == 0xB6) && 
				(p_Protocol->opobj_data[2].ID  == 0xC1)	)
				{/*���ذ���B6��C1�����������ţ�����ȡ�ĸ����ͷ������ȣ��˴�ȡB6��	*/
					if ((p_Protocol->opobj_data[0].sub_buf[0]) == HOLD_WAY)
					{
						/* �˴�����Ϣ����ʽ���͸������������񣬲���俴�����ݽṹ�壬�ýṹ���õ���C1��ṹ
						 * Э��������ֲ������sche_change_tpye���飬������鲿���ڷ�����������ĩβ
						 */
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���ڷ������������޸�
						
// 						sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];//��ϵͳ����B6���ÿ��Ʒ�ʽ
// 						sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];			//��b6��ֵ������
						
						//���ÿ��ؾ��������£����濴�����ݵ��ṹ���У����ñ�־hold_guard_flag
						
						p_Char=(u8*)(&sche_hold_guand);
				
						for(k=0;k<sizeof(STAGE_TIMING_TABLE_t);k++)
						{
							*(p_Char+k)=p_Protocol->opobj_data[2].sub_buf[k];
						}						

						sche_hold_guand.index = sche_change_tpye[NOW_C][SCHE_NUM];
						sche_hold_guand.relPhase = (((sche_hold_guand.relPhase<<8)&0xff00)|((sche_hold_guand.relPhase>>8)&0xff));
						
						
// // 						sche_hold_guand.index 				= p_Protocol->opobj_data[2].sub_buf[0];		//������
// // 						sche_hold_guand.stageId 			= p_Protocol->opobj_data[2].sub_buf[1];		//�׶κ�
// // 						sche_hold_guand.relPhase 			= p_Protocol->opobj_data[2].sub_buf[2];				//��λH
// // 						sche_hold_guand.relPhase 	  	= sche_hold_guand.relPhase <<8;
// // 						sche_hold_guand.relPhase 	  	|= p_Protocol->opobj_data[2].sub_buf[3];			//��λL
// // 						sche_hold_guand.stGreenTime 	= p_Protocol->opobj_data[2].sub_buf[4];			//�̵�ʱ��
// // 						sche_hold_guand.stYellowTime 	= p_Protocol->opobj_data[2].sub_buf[5];			//�Ƶ�ʱ��
// // 						sche_hold_guand.stRedTime  		= p_Protocol->opobj_data[2].sub_buf[6];			//���ʱ��
// // 						sche_hold_guand.stAlterParam 	= p_Protocol->opobj_data[2].sub_buf[7];			//�׶�ѡ�����
// // 					
						
						to_dispatch_msg[MSGA_SCHE_NUM] = sche_hold_guand.index;
						
						os_mut_release(RT_Operate_Mut);	
						
// // 						to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
				
					}
					else
					{
						return (__FALSE);
					}
				}
				else
				{
					return (__FALSE);
				}
			}
			break;
		case 6:								//�����������ã�����ΪB7��B6��B8��B9��BE��BF����B7��ֵΪ0x0C���Ż�ָ������
			{
				if ((p_Protocol->opobj_data[0].ID  == 0xB7) && 
						(p_Protocol->opobj_data[1].ID  == 0xB6) && 
						(p_Protocol->opobj_data[2].ID  == 0xB8) && 
						(p_Protocol->opobj_data[3].ID  == 0xB9) && 
						(p_Protocol->opobj_data[4].ID  == 0xBE) && 
						(p_Protocol->opobj_data[5].ID  == 0xBF)	)
				{
					if ((p_Protocol->opobj_data[0].sub_buf[0]) == OPTIMIZE_WAY &&
							(p_Protocol->opobj_data[1].sub_buf[0]) != YELLOW_SCHE_NUM &&
							(p_Protocol->opobj_data[1].sub_buf[0]) != RED_SCHE_NUM &&
							(p_Protocol->opobj_data[2].sub_buf[0])	> 0 )				//�����Ʒ�ʽΪ�Ż�,���Ż�������Ϊ����ȫ�죬��Period_Time���ô���0����Ч
					{
						/* �˴�����Ϣ����ʽ���͸������������񣬲�����Ż����ݽṹ�壬�ýṹ���õ����Զ���lxb
						 * Э��������ֲ������sche_change_tpye���飬������鲿���ڷ�����������ĩβ
						 */
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���ڷ������������޸�
					
// 						/*��ϵͳ����B6���ÿ��Ʒ�ʽB7	*/
// 						sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
// 						sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						
						/*	����ϵͳ�Ż��£������Ż����ݵ��ṹ����	*/
						sche_optimize.Period_Time		 		= p_Protocol->opobj_data[2].sub_buf[0];//B8��д��
						sche_optimize.Phase_Difference 	= p_Protocol->opobj_data[3].sub_buf[0];//B9��д��
						
						//BE��д���Ż����ݽṹ��
						index = 0;
						for(i = 0; i <= 15; i++)
						{
							sche_optimize.Stage_Time[i] = p_Protocol->opobj_data[4].sub_buf[index++];
						}
						
						//BF��д���Ż����ݽṹ��
						index = 0;
						for(i = 0; i <= 15; i++)
						{
							sche_optimize.Key_Phase_Time[i] = p_Protocol->opobj_data[5].sub_buf[index++];
						}
						os_mut_release(RT_Operate_Mut);		

						to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						to_dispatch_msg[MSGA_CONTROL_WAY] = OPTIMIZE_WAY;
										
					}
					else
					{
						return (__FALSE);
					}
				}	
				else
				{
					return (__FALSE);
				}				
			}
			break;
		default:
			return (__FALSE);
	}
	
	/* ����Ӧ��	*/
	for (i=0; i<p_Protocol->count_target; i++)
	{
		p_To_eth->data[p_To_eth->lenth++]	= p_Protocol->opobj_data[i].ID;
		p_To_eth->data[p_To_eth->lenth++]	=(((p_Protocol->opobj_data[i].count_index)<<6) | 
																				(p_Protocol->opobj_data[i].sub_target) );
		for (j=0;j<(p_Protocol->opobj_data[i].count_index);j++)
		{
			p_To_eth->data[p_To_eth->lenth++]	= p_Protocol->opobj_data[i].sub_index[j];
		}
	}
 
	/* Э���������������������	*/
	to_dispatch_msg[MSGA_TRIGGER_WAY] = 2;
	msg_to_dispatch	= to_dispatch_msg;
	os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
	return __TRUE;
}


/*==============================================================================================
* �﷨��ʽ��    rt_operate_read(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ʵʱ�������ݲ�ѯ����������BB���ֶ�B5��ϵͳ����B6�����Ʒ�ʽB7��������C1���Ż���B8��B9��BE��BF
* ����1��       PROTOCOL_DATA *p_Protocol ����Э��������������ݽṹ�嵽�ú���
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
u8 rt_operate_read(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
{
	u8			i,		j;
	u8			holding_temp 		= 0;			//��ǰϵͳ�Ƿ��ڿ�����
	u8			optimizing_temp = 0;			//��ǰϵͳ�Ƿ����Ż���	

	

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	if ((p_Protocol->opobj_data[0].ID  == 0xB7) && 
			(p_Protocol->opobj_data[1].ID  == 0xB6) && 
			(p_Protocol->opobj_data[2].ID  == 0xC1)	)							//��ѯ��������
	{
		os_mut_wait(RT_Operate_Mut,0xFFFF);   	//���ڷ������������ȡ
		if (sche_change_tpye[NOW_C][SCHE_CON_TYPE] == SYSTEM_C	&&
				sche_change_tpye[NOW_C][SCHE_CON_WAY] 	== 0x05)
		{
			holding_temp = 1;											//��ǰϵͳ���ڿ�����
		}
// // // 		holding_temp = 1;//debug lxb
		os_mut_release(RT_Operate_Mut);
		if (holding_temp)
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
			
			p_To_eth->data[p_To_eth->lenth++]	=	0xB7;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[0].count_index)<<6) | 
																					(p_Protocol->opobj_data[0].sub_target)); 
			p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[NOW_C][SCHE_CON_WAY]; 
			
			p_To_eth->data[p_To_eth->lenth++]	=	0xB6;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[1].count_index)<<6) | 
																					(p_Protocol->opobj_data[1].sub_target));  
			p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[SYSTEM_C][SCHE_NUM]; 

			p_To_eth->data[p_To_eth->lenth++]	=	0xC1;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[2].count_index)<<6) | 
																					(p_Protocol->opobj_data[2].sub_target));  
			for (j=0;j<(p_Protocol->opobj_data[2].count_index);j++)
			{
				p_To_eth->data[p_To_eth->lenth++]	= p_Protocol->opobj_data[2].sub_index[j];
			}
			/*	��ѯӦ�����ֵ	,��ѯC1����		*/
			if (p_Protocol->opobj_data[2].sub_target == 0)
			{
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.index;
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.stageId;
				p_To_eth->data[p_To_eth->lenth++]	= ((sche_hold_guand.relPhase >>8) & 0xFF);
				p_To_eth->data[p_To_eth->lenth++]	= (sche_hold_guand.relPhase & 0xFF);
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.stGreenTime;
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.stYellowTime;
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.stRedTime;
				p_To_eth->data[p_To_eth->lenth++]	= sche_hold_guand.stAlterParam;				
			}
			else
			{
				
			}
				
			os_mut_release(RT_Operate_Mut);
			
		}
		else
		{
			return (__FALSE);
		}
	}
	else if (	(p_Protocol->opobj_data[0].ID  == 0xB7) && 
						(p_Protocol->opobj_data[1].ID  == 0xB6) && 
						(p_Protocol->opobj_data[2].ID  == 0xB8) && 
						(p_Protocol->opobj_data[3].ID  == 0xB9) && 
						(p_Protocol->opobj_data[4].ID  == 0xBE) && 
						(p_Protocol->opobj_data[5].ID  == 0xBF)	)				//��ѯ�Ż�����
	{
		os_mut_wait(RT_Operate_Mut,0xFFFF);   	//���ڷ������������ȡ
		if (sche_change_tpye[NOW_C][SCHE_CON_TYPE] == SYSTEM_C	&&
				sche_change_tpye[NOW_C][SCHE_CON_WAY] 	== 0x0C)
		{
			optimizing_temp = 1;									//��ǰϵͳ�����Ż���	
		}
// // 		optimizing_temp = 1;//debug lxb
		os_mut_release(RT_Operate_Mut);
		if (optimizing_temp)
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
			
			p_To_eth->data[p_To_eth->lenth++]	=	0xB7;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[0].count_index)<<6) | 
																					(p_Protocol->opobj_data[0].sub_target)); 
			p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[NOW_C][SCHE_CON_WAY]; 
			
			p_To_eth->data[p_To_eth->lenth++]	=	0xB8;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[1].count_index)<<6) | 
																					(p_Protocol->opobj_data[1].sub_target));  
			p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Period_Time; 

			p_To_eth->data[p_To_eth->lenth++]	=	0xB9;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[2].count_index)<<6) | 
																					(p_Protocol->opobj_data[2].sub_target));  
			p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Phase_Difference; 
			
			p_To_eth->data[p_To_eth->lenth++]	=	0xBE;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[3].count_index)<<6) | 
																					(p_Protocol->opobj_data[3].sub_target));  
			for(j=0;j<16;j++)
			{
				p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Stage_Time[j]; 
			}

			p_To_eth->data[p_To_eth->lenth++]	=	0xBF;
			p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[4].count_index)<<6) | 
																					(p_Protocol->opobj_data[4].sub_target));  
			for(j=0;j<16;j++)
			{
				p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Key_Phase_Time[j]; 
			}
			
			os_mut_release(RT_Operate_Mut);
			
		}
		else
		{
			return (__FALSE);
		}
	}
	else																											//��ѯ��������
	{
		for (i=0; i<p_Protocol->count_target; i++)
		{
			switch (p_Protocol->opobj_data[i].ID)
			{
				case 0xB5:						//��ѯB5			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB5;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target)); 
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[MANUAL_C][SCHE_NUM]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB6:						//��ѯB6			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB6;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[SYSTEM_C][SCHE_NUM]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB7:						//��ѯB7			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB7;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target)); 

						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[NOW_C][SCHE_CON_WAY]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB8:						//��ѯB8			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB8;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Period_Time; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB9:						//��ѯB9			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB9;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Phase_Difference; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xBE:						//��ѯBE			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xBE;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
							
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						
						for(j=0;j<16;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Stage_Time[j]; 
						}
						
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xBF:						//��ѯBF			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xBF;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //���������������
						for(j=0;j<16;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Key_Phase_Time[j]; 
						}
						os_mut_release(RT_Operate_Mut);
					}
					break;
				default:
					return (__FALSE);
			}
		}
	}

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	return (__TRUE);
}

