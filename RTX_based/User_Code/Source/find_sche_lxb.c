#include "Include.h"

u8 	search_8D_timeTbId(time_struct *current_time, u16 period_valid,time_struct *pre_stage_time);/*	2012-12.29��ʱ�α��	*/
TIME_TABLE_t search_8e_timeEventID(u8 stage_index,time_struct *current_time);
TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,time_struct *current_time);

/**
  * @brief  ���ݵ�ǰʱ�䣬��ǰ����8D��������ʱ�κš�u8 search_8D_timeTbId(u8 *Read_Time_Data, u16 period_valid,time_struct *pre_stage_time)
  *         ����Ϊ��
	*					���ζ���8D��					
	* 				�����в���ð�ݷ��ҳ����ڻ��ǽӽ���ǰ�µ���Ч�� 						                
	* 				�ҵ���Ч�º����ж��Ƿ����ܼƻ�������в���ð�ݷ��ҳ��ӽ���ǰ��x�ļƻ���    
	* 				�ҵ���Ч�º����ж����Ƿ���Ч�������Ч����ð�ݷ��ҳ��ӽ���ǰ���ڵļƻ���  
	* 				�ܼƻ�����   
  * @param  *Read_Time_Data,����Ϊ��ǰʱ�����ڡ��ա���
	*					period_valid		��ǰ������Ч��ʱ�κţ�ÿλ��Ӧһ��ʱ�κ�
	*					pre_stage_time	�ҵ�����Чʱ�κŶ�Ӧ��ʱ��ṹ�壬��Ҫ�������ظ�ʱ�κŶ�Ӧ������
  * @retval ʱ�κ�
	* @invovled function
  */
u8 search_8D_timeTbId(time_struct *current_time, u16 period_valid,time_struct *pre_stage_time)
{
	
	u8					i;
	
	u8 					today_time_stage_index_temp;					//ʱ�α��
	
	/*�洢ʱ��Ķ�����ʱ��ı���*/
	u16					month_temp 					= 	0;
	u32 				day_month_temp 			= 	0;
	u8 					dayWeek_temp 				= 	0;
	
	/*lxb add	���ڷ�ð������8D����ʱ����*/
	u8	count_month_temp;								//�µ�ѭ������
	u8  compare_month = 12;							//��ð��������м�������洢���뵱ǰ��������·�
	
	u8	count_week_temp;								//�ܵ�ѭ������
	u8  compare_week = 7;								//��ð��������м�������洢���뵱ǰ��������·�
	u8	cur_timeTbId_week = 0;					//�����հ��ܵõ���ʱ�α��
	
	u8	count_daymonth_temp;						//�µ�ѭ������
	u8  compare_daymonth = 31;					//��ð��������м�������洢���뵱ǰ��������·�
	u8	cur_timeTbId_month = 0;					//�����հ��µõ���ʱ�α��
	
	u8  curstage_month_temp =0;					//�洢��ǰʱ�κŶ�Ӧ��ʱ����ʱ����
	u8	curstage_week_temp  =0;					//�洢��ǰʱ�κŶ�Ӧ��ʱ����ʱ����
	u8	curstage_day_temp		=0;					//�洢��ǰʱ�κŶ�Ӧ��ʱ����ʱ����
	
	u8	daymonth_max;										//�洢�����գ����£�ʱ������ѭ������
	u8	weekmonth_max;									//�洢�����գ����ܣ�ʱ������ѭ������
	/*	
	��8D����ʱ�洢����
	*/
	TIME_SCHEDULE_t	Table_0x8D_Contect	= 	{0};
	u8 *   					p_Char     					=	NULL;
	u32							k										=	0;
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;	
	
	/*ʱ��ת��Ϊ������	*/
	month_temp 			= 	1 << 	(current_time->month);
	day_month_temp 	= 	1	<<	(current_time->day);
	dayWeek_temp		= 	1	<<	(current_time->week);
	
	today_time_stage_index_temp	= 	0;
	
	for(i = 40; i >= 1; i--)									//�Ӻ���ǰ��
	{
			/*��8D���ݿ�*/
			read_lenth_temp			=	0;
			data_Target.ID 			= 	0x8D;
			data_Target.count_index 	= 	1;
			data_Target.sub_index[0] 	= 	i;
			data_Target.sub_index[1] 	= 	0;
			data_Target.sub_target 	= 	0;
			
			os_mut_wait(Spi_Mut_lm,0xFFFF);
			read_lenth_temp			=	DB_Read_M(&data_Target);	
			os_mut_release(Spi_Mut_lm);	
					
			if(read_lenth_temp == 	0)
			{
				continue;
			}
			else
			{
				if(data_Target.sub_buf[0] != 0)
				{ 
					p_Char=(u8*)(&Table_0x8D_Contect);
					
					for(k=0;k<9;k++)
					{
						*(p_Char+k) = data_Target.sub_buf[k];
					}			
				}	
				else
				{
					continue;
				}
			}		
		/*��8D���ݿ����������Table_0x8D_Contect��*/
			
		/*	����ǰʱ�κ���ȷ����Ч����������ѭ��	*/	
		if((period_valid & (u16)(1 << (Table_0x8D_Contect.timeTbId-1)))	== 0)	
		{		
			continue;
		}			
		
		if(	(Table_0x8D_Contect.month>0)	&&
				((Table_0x8D_Contect.dayWeek>0) || (Table_0x8D_Contect.dayMonth>0))	&&
				(Table_0x8D_Contect.timeTbId>0))																	//8d��ǿ�
		{
			for(count_month_temp=0;count_month_temp <= 12;count_month_temp++)		//��ǰ������Ч��
			{
				/*	
					���������ǵ�ǰ�£���ӵ��տ�ʼ��������count_month_temp==0�����
					���������ǲ��ǵ�ǰ�£�����ǰ��û����������ǰ������������ʱ����31�ջ������տ�ʼ������else���֣���
				*/
				if (count_month_temp==0)
				{ 
					daymonth_max 	= current_time->day;
					weekmonth_max = current_time->week;
				}
				else
				{
					daymonth_max 		= 31;
					weekmonth_max 	= 7;
					day_month_temp 	= 0x80000000;
					dayWeek_temp 		= 0x80;
				}
				
				/*
					�����㷨������Ч�������������գ����£�����Ч�����뵱ǰ������洢ʱ�κ��Լ���ʱ�κŶ�Ӧ��ʱ�����ڣ�
										����Ч�������������գ����ܣ�����Ч�����뵱ǰ������洢ʱ�κ��Լ���ʱ�κŶ�Ӧ��ʱ�����ڣ�
										ֱ���ҵ����ʱ�α�
				*/
				if ((Table_0x8D_Contect.month &	month_temp)	==	month_temp)				//����Ч
				{
					/*	
						��Ϊ12�������Ѿ�ѭ��12��������һȦ�������ҵ���ǰ�£�
						��ʱΪ�˽���if (count_month_temp <= compare_month)�жϣ�
						���뽫count_month_temp = 0;
					*/
					if(count_month_temp == 12)
					{
						count_month_temp = 0;
					}
					
					if (count_month_temp <= compare_month)													//���ӽ���ǰ��
					{
						compare_month = count_month_temp;
						
						/*	�����գ����£�����	*/
						if (Table_0x8D_Contect.dayMonth>0)
						{
							for(count_daymonth_temp=0;count_daymonth_temp < daymonth_max;count_daymonth_temp++)	//��ǰ������Ч��
							{
								if ((Table_0x8D_Contect.dayMonth &	day_month_temp)	==	day_month_temp)	//����Ч
								{
									if (count_daymonth_temp < compare_daymonth)														//���ӽ���ǰ�գ����£�
									{
										compare_daymonth = count_daymonth_temp;
										cur_timeTbId_month = Table_0x8D_Contect.timeTbId;										//�õ�ʱ�κ�
										
										/*	������Ч�����ŵ����ڣ��㷨���£�������	*/
										if (current_time->month >count_month_temp)
										{
											curstage_month_temp = current_time->month - count_month_temp;//����ʱ�κŵ���
										}
										else
										{
											curstage_month_temp = 12 + current_time->month - count_month_temp;	//����ʱ�κŵ���
										}
										curstage_day_temp   = current_time->day - count_daymonth_temp;	//����ʱ�κŵ���
									}	
									count_month_temp =13;			//�ҵ���Ч�£���Ч�գ�Ϊ���˳��µ�forѭ����������ֵ���
									break;										//break�������գ����£�ѭ��	���ҵ�����Ч�����գ����£�
								}
								day_month_temp = (day_month_temp >> 1);																//��λʱ��day_month_temp
							}
							
						}
						
						/*	�����գ����ܣ�����	*/
						else if (Table_0x8D_Contect.dayWeek>0)																			//����Ч�ж���
						{
							for(count_week_temp=0;count_week_temp < weekmonth_max;count_week_temp++)							//��ǰ������Ч��
							{
								if ((Table_0x8D_Contect.dayWeek &	dayWeek_temp)	==	dayWeek_temp)				//����Ч
								{
									if (count_week_temp < compare_week)																		//���ӽ���ǰ�գ����ܣ�
									{
										compare_week = count_week_temp;
										cur_timeTbId_week = Table_0x8D_Contect.timeTbId;
										/*	������Ч�����ŵ����ڣ��㷨���£�������	*/
										if (current_time->month >count_month_temp)
										{
											curstage_month_temp = current_time->month - count_month_temp;				//����ʱ�κŵ���
										}
										else
										{
											curstage_month_temp = 12 + current_time->month - count_month_temp;	//����ʱ�κŵ���
										}
										curstage_week_temp  = current_time->week - count_daymonth_temp;				//����ʱ�κŵ��¡�����
									
									}
									count_month_temp =13;			//�ҵ���Ч�£���Ч�գ�Ϊ���˳��µ�forѭ����������ֵ���
									break;										//break�������գ����ܣ�ѭ��																															//�ҵ���Ч�����գ����ܣ�
								}
								dayWeek_temp = (dayWeek_temp >> 1);
							}
							
						}
						else
						{
						}
						
					}
				}
				
				if(month_temp == 0x02)												//����1�·���ϣ����ж�12�·�
				{
					month_temp = 0x1000;
				}
				else
				{
					month_temp = (month_temp >> 1);
				}
			}
			
		}
		else
		{
			continue;
		}
	}
	
	/*	�������գ����£����ҵ�ʱ�α����0,�Ҹ�ʱ�κ�Ϊ���յ�ʱ�κţ��򷵻ظ�ʱ�α�ţ��Լ���ʱ�κŶ�Ӧ�����ڣ�
			�����������գ����ܣ����ҵ�ʱ�α����0���򷵻ظ�ʱ�α��	���Լ���ʱ�κŶ�Ӧ�����ڣ�
			�����������գ����£����ҵ�ʱ�α����0,�Ҹ�ʱ�κŲ�Ϊ���յ�ʱ�κţ��򷵻ظ�ʱ�α�ţ��Լ���ʱ�κŶ�Ӧ�����ڣ�
			���򣬽���
	*/
	if ((cur_timeTbId_week > 0) || (cur_timeTbId_month > 0))
	{
		if ((cur_timeTbId_month > 0) && (compare_daymonth == 0))
		{
			today_time_stage_index_temp	=	cur_timeTbId_month;	
			
			pre_stage_time->month = curstage_month_temp;			//����ʱ�κŵ��¡�����
			pre_stage_time->day   = curstage_day_temp;				//����ʱ�κŵ��¡�����
		}
		else if (cur_timeTbId_week > 0)
		{
			today_time_stage_index_temp	=	cur_timeTbId_week;	
			
			pre_stage_time->month = curstage_month_temp;			//����ʱ�κŵ��¡�����
			pre_stage_time->week  = curstage_week_temp;				//����ʱ�κŵ��¡�����
									
		}
		else
		{
			today_time_stage_index_temp	=	cur_timeTbId_month;	
			
			pre_stage_time->month = curstage_month_temp;			//����ʱ�κŵ��¡�����
			pre_stage_time->day   = curstage_day_temp;				//����ʱ�κŵ��¡�����
		}

		return today_time_stage_index_temp;									//����ʱ�κ���Ч
	}
	else
	{
		return 0;									//û�ҵ�����������
	}
	
}



/**
  * @brief  ���ݵ�ǰʱ�䣬��ǰ����8E�����õ�����������Ʒ�ʽ
  *         TIME_TABLE_t search_8e_timeEventID(u8 stage_index,u8 *Read_Time_Data)
  * @param  ʱ�κţ�stage_index��
						��ǰʱ�䣺Read_Time_Data��
  * @retval TIME_TABLE_t������8e��Ľṹ�壬�������Ʒ�ʽ�������ŵ�����
						TIME_TABLE_t.schemeId==0������8e��Ϊ�գ�
						TIME_TABLE_t.schemeId==0xAA������8e�ǿգ�������������ǰʱ�Σ�����ǰһ������
						TIME_TABLE_t.schemeId==1������8e�ǿգ��������˷���������Ʒ�ʽ���������
	* @invovled function
  */
TIME_TABLE_t search_8e_timeEventID(u8 stage_index,time_struct *current_time)
{
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*	
	��8E�����ݿ���ʱ����
	*/
	TIME_TABLE_t		Table_0x8E_read 			= 	{0};
	
	u8 					today_time_stage_index_temp;			//ʱ�α��
	u8 					now_scheme_index_temp;						//��ǰʱ������

	u8 					Scheme_Num;												//������
	u8 					Control_Type;											//���Ʒ�ʽ
	
	u8 					i 							= 	0;
	/*	
	8E���ж�Ӧ����Ч�¼�����Ŀ
	*/
	u8 					index 						= 	0;
	
	/*lxb add	//ʱ����ð��������м�������洢���뵱ǰʱ�����ʱ	��*/
	u16  				compare_time_temp = 24*60+60;							
	/*����8e�����ʱ�ṹ�壬�������Ʒ�ʽ�������ŵ�����*/
	TIME_TABLE_t table_0x8E_return = {0};
	
	today_time_stage_index_temp = stage_index;
	
	/*
	���Ҹ�ʱ�α��������Ч���¼��ţ�
	����¼�¼���ʼ��
	���㡢
	���֡�
	�������͡�
	��ʱ������	
	*/
	index		= 		0;													//��Ч���¼���������
	now_scheme_index_temp = 0;								//��ǰʱ����������
	for(	i = 1;		i <= 48;	i++)
	{	
		//��ȡ8e
		read_lenth_temp				=	0;
		data_Target.ID 				= 	0x8E;
		data_Target.count_index 		= 	2;
		data_Target.sub_index[0] 		= 	today_time_stage_index_temp;
		data_Target.sub_index[1] 		= 	i;
		data_Target.sub_target 		= 	0;
		
		os_mut_wait(Spi_Mut_lm,0xFFFF);
	  read_lenth_temp				=	DB_Read_M(&data_Target);	
		os_mut_release(Spi_Mut_lm);	
		
		
		if(read_lenth_temp 			== 	0	)
		{
			continue;
		}
		else
		{
			Table_0x8E_read.timeId 			= 	data_Target.sub_buf[0];	//ʱ�κ�
			Table_0x8E_read.eventId 		= 	data_Target.sub_buf[1];	//ʱ���¼���
			Table_0x8E_read.hour 				= 	data_Target.sub_buf[2];	//��ʼʱ
			Table_0x8E_read.minute 			= 	data_Target.sub_buf[3];	//��ʼ��
			Table_0x8E_read.ctlMode 		= 	data_Target.sub_buf[4];	//���Ʒ�ʽ
			Table_0x8E_read.schemeId 		= 	data_Target.sub_buf[5];	//������
			Table_0x8E_read.auOutput 		= 	data_Target.sub_buf[6];
			Table_0x8E_read.spOutput 		= 	data_Target.sub_buf[7];
			
			index++;																//��Ч�¼�������++
		}	
		//��ȡ8e���
		
		/*	
			�㷨������ʱ�����ð�������ҵ����뵱ǰ�������compare_time_temp��С���ķ���	
		*/
		if((Table_0x8E_read.timeId > 0) &&
			 (Table_0x8E_read.eventId > 0) &&
			 (Table_0x8E_read.schemeId > 0))
		{																						//��8e����Ч
			if((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 	
			<=  (current_time->hour * 60 + current_time->minute ))
			{																					//����ǰʱ����ڶ�ȡ������ֵ�ʱ��
				if(
					((current_time->hour * 60 + current_time->minute)
					-(Table_0x8E_read.hour * 60 + Table_0x8E_read.minute)) 
					< compare_time_temp
					)																			//����ǰʱ��С�ڴ洢��ʱ��ʱ��
				{
					compare_time_temp = (current_time->hour * 60 + current_time->minute)
														- (Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) ;					
					now_scheme_index_temp = i;							 //�洢�ҵ����¼�������
					
					Scheme_Num		=	Table_0x8E_read.schemeId;//��ȡ������
					Control_Type	= Table_0x8E_read.ctlMode; //�洢���Ʒ�ʽ
				}
			}
		}
		else
		{
			continue;
		}
		
		
	}
	
	if((index == 0))		 //����8eΪ��
	{
		table_0x8E_return.schemeId		= 0;		 //8e����Ч
	}
	else
	{
		if (now_scheme_index_temp == 0)				
		/*
			index��Ϊ0��now_scheme_index_tempΪ0��
			����ʱ�κ���Ч������ǰʱ��û�䷽����Ӧ���ҵ�ǰһ������һ��ʱ�εķ���
		*/
		{
			table_0x8E_return.schemeId	= 0xFF;
		}
		else																		//����������ҵ����ʵķ���������Ʒ�ʽ��
		{
			table_0x8E_return.schemeId		= Scheme_Num;
			table_0x8E_return.ctlMode 		= Control_Type;
			table_0x8E_return.auOutput 		= table_0x8E_return.auOutput;
			table_0x8E_return.spOutput 		= table_0x8E_return.spOutput;
		}
	}
	return table_0x8E_return;
}

/**
  * @brief  ���ݵ�ǰʱ�䣬��ǰ����8E�����õ�����������Ʒ�ʽ
  *         TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,u8 *Read_Time_Data)
  * @param  ʱ�κţ�stage_index��
						��ǰʱ�䣺Read_Time_Data��
  * @retval TIME_TABLE_t������8e��Ľṹ�壬�������ӵ�ʱ���
						TIME_TABLE_t.hour==0��TIME_TABLE_t.minute==0,����8e��Ϊ�գ�
						�������8e�ǿգ������������ӵ�ʱ��֣��������
	* @invovled function
  */
TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,time_struct *current_time)
{
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*	
	��8E�����ݿ���ʱ����
	*/
	TIME_TABLE_t		Table_0x8E_read 			= 	{0};
	u8 					today_time_stage_index_temp;			//ÿ��ֻ��Ӧһ��ʱ�α��jialuyao
	u8 					now_scheme_index_temp;						//��ǰʱ������

	u8 					cur_hour;												//ʱ�м����
	u8 					cur_minute;											//���м����
	/*	
	ԭFind_Today_Fangan()����
	*/
	u8 					i 							= 	0;
	/*	
	8E���ж�Ӧ����Ч�¼�����Ŀ
	*/
	u8 					index 						= 	0;
	
	/*lxb add	//ʱ����ð��������м�������洢���뵱ǰʱ�����ʱ	��*/
	u16  				compare_time_temp = 24*60+60;							
	
	/*����8e�����ʱ�ṹ�壬�������Ʒ�ʽ�������ŵ�����*/
	TIME_TABLE_t table_0x8E_return = {0};
	
	today_time_stage_index_temp = stage_index;
	
	/*
	���Ҹ�ʱ�α��������Ч���¼��ţ�
	����¼�¼���ʼ��
	���㡢
	���֡�
	�������͡�
	��ʱ������	
	*/
	index		= 		0;													//��Ч���¼���������
	now_scheme_index_temp = 0;
	for(	i = 1;		i <= 48;	i++)
	{	
		//��ȡ8e
		read_lenth_temp				=	0;
		data_Target.ID 				= 	0x8E;
		data_Target.count_index 		= 	2;
		data_Target.sub_index[0] 		= 	today_time_stage_index_temp;
		data_Target.sub_index[1] 		= 	i;
		data_Target.sub_target 		= 	0;
		
		os_mut_wait(Spi_Mut_lm,0xFFFF);
	  read_lenth_temp				=	DB_Read_M(&data_Target);	
		os_mut_release(Spi_Mut_lm);	
		
		
		if(read_lenth_temp 			== 	0	)
		{
			continue;
		}
		else
		{
			Table_0x8E_read.timeId 			= 	data_Target.sub_buf[0];	//ʱ�κ�
			Table_0x8E_read.eventId 		= 	data_Target.sub_buf[1];	//ʱ���¼���
			Table_0x8E_read.hour 				= 	data_Target.sub_buf[2];	//��ʼʱ
			Table_0x8E_read.minute 			= 	data_Target.sub_buf[3];	//��ʼ��
			Table_0x8E_read.ctlMode 		= 	data_Target.sub_buf[4];	//���Ʒ�ʽ
			Table_0x8E_read.schemeId 		= 	data_Target.sub_buf[5];	//������
			Table_0x8E_read.auOutput 		= 	data_Target.sub_buf[6];
			Table_0x8E_read.spOutput 		= 	data_Target.sub_buf[7];
			
			index++;
		}	
		//��ȡ8e���
		
		/*	
			�㷨������ʱ�����ð���������������ҵ����뵱ǰ�������compare_time_temp��С���ķ���	
		*/
		if((Table_0x8E_read.timeId > 0) &&
			 (Table_0x8E_read.eventId > 0) &&
			 (Table_0x8E_read.schemeId > 0))
		{																						//��8e����Ч		
			if((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 	
			> (current_time->hour * 60 + current_time->minute ))
			{																					//����ǰʱ��С�ڶ�ȡ������ֵ�ʱ��
				if(
					((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 
					-(current_time->hour * 60 + current_time->minute)) 
					< compare_time_temp
					)																			//����ǰʱ��С�ڴ洢��ʱ��ʱ��
				{
					compare_time_temp = ((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 
															-(current_time->hour * 60 + current_time->minute));					
					now_scheme_index_temp = i;
					
					cur_hour		=	Table_0x8E_read.hour;
					cur_minute	=	Table_0x8E_read.minute;
				}
			}

			
		}
		else
		{
			continue;
		}
		
		
	}
	
	if(now_scheme_index_temp == 0)		 //������ǰʱ��Ϊ���һ��ʱ��
	{
		table_0x8E_return.hour			= 0;		 //8e����Ч
		table_0x8E_return.minute		= 0;		 //8e����Ч
	}
	else
	{
		table_0x8E_return.minute	= cur_minute;
		table_0x8E_return.hour 		= cur_hour;
	}
	return table_0x8E_return;
	
}
/**
  * @brief  8Dȫ�����������һ��ʱ�εĿ��Ʒ�ʽ�ͷ����š�u8 Search_8D_All_Prev(u8 Read_Time_Data[])
  *         ����Ϊ��
	*					���µ��յ�ǰʱ������������һ��ʱ�ο��Ʒ�ʽ�ͷ����ţ���8D��ʱ�κŶ�Ӧ��8E����Ч					
	* 				�����ζ�Ӧ��ʱ�κż�������8D�������ʱ�κ�  
  * @param  *Read_Time_Data,����Ϊ��ǰʱ�����ڡ��ա���
  * @retval ���Ʒ�ʽ  ������
	* @invovled function		search_8e_timeEventID();
	* 											search_8e_timeEventIDNEXT();
	* 											search_8D_timeTbId();
  */
u8 search_8D_all_timeTbId (time_struct *current_time,TIME_TABLE_t *table_0x8E_return)
{
	u8 							time_stage_index;							//ʱ�α��
	u16 						period_valid	=	0xffff;   		//��ֵʱ��ȫ����Ч
	u8 							period_valid_count;  					//ʱ�α������
	TIME_TABLE_t 		table_0x8E_str = {0};					//8e����ʱ�ṹ�壬��������8e��õĿ��Ʒ�ʽ��
	time_struct			pre_stage_time = {0};					//ʱ��ṹ�壬��ǰ�����Ĵ洢�ṹ��
	time_struct			orig_time_data = {0};					//�洢��ʱ��оƬ�����ĳ�ʼʱ��
	
	/*	�洢��ʱ��оƬ������ʱ��,��Ϊԭʼʱ�䣨current_time���ܻ��ڳ���ִ�й����и��ģ�	*/
		orig_time_data.second = current_time->second;
		orig_time_data.minute = current_time->minute;
		orig_time_data.hour		= current_time->hour;
		orig_time_data.week 	= current_time->week;
		orig_time_data.day		= current_time->day;
		orig_time_data.month  = current_time->month;
		orig_time_data.year   = current_time->year;
	
	for(period_valid_count=1;period_valid_count <= 16;period_valid_count++) // ���16��ʱ�α�
	{
		/*�ӵ�ǰ����ǰ������ֱ����������Чʱ�κţ��������ʱ�κŵ����ڵ�pre_stage_time*/
		time_stage_index = search_8D_timeTbId (current_time,period_valid,&pre_stage_time); 
		if(time_stage_index == 0) 			//��Чʱ�α�
		{
			return  (__FALSE);						//����
		}
		else 								    				//ʱ�α���Ч
		{
			/*�ӵ�ǰʱ����ǰ����8e�����ؿ��Ʒ�ʽ�ͷ�����*/	
			table_0x8E_str = search_8e_timeEventID(time_stage_index, current_time);
			
			/*������Ʒ�ʽ�ͷ����ŵ�*/	
			table_0x8E_return->schemeId = table_0x8E_str.schemeId; 
			table_0x8E_return->ctlMode 	= table_0x8E_str.ctlMode; 
			table_0x8E_return->auOutput = table_0x8E_str.auOutput; 
			table_0x8E_return->spOutput = table_0x8E_str.spOutput; 
			
			/*���صĿ��Ʒ�ʽ�ͷ�������Ч�����ʱ�κ���Ч�����θ�ʱ�κţ�����ѭ������*/	
			if ((table_0x8E_return->schemeId) == 0)    								//8E���Ƿ���Ч
			{
					period_valid &= (u16)(~(1 << (time_stage_index-1)));  //8E����Ч ����ǰʱ�α�ţ�λ��0��������
			}
			/*���صĿ��Ʒ�ʽ�ͷ�����0xff�������ʱ�κ���Ч��
				����ʱ���û�䷽����������ǰһ�����ʱ�εķ����źͿ��Ʒ�ʽ*/
			else if((table_0x8E_return->schemeId) == 0xFF)		//��Ѱǰһ�����ʱ�εĿ��Ʒ�ʽ
			{
				/*	�޸�д��ʱ��Ϊǰһ�����ʱ�̣��Զ���ǰһ�����ʱ��	*/
				current_time->hour   = 23;									//ʱ
				current_time->minute = 59;									//��
				current_time->second = 59;									//��
				if(pre_stage_time.week == 1)
				{
					current_time->week   = 7;									//����
				}
				else
				{
					current_time->week   = pre_stage_time.week - 1;			//����
				}
				if(pre_stage_time.day == 1)
				{
					current_time->day = 31;								//��
					//pre_stage_timeΪ����Чʱ�κŶ�Ӧ������ʱ��ṹ��
					if(pre_stage_time.month == 1)
					{
						current_time->month = 12;							//��
					}
					else
					{
						current_time->month = pre_stage_time.month - 1;//��
					}
				}
				else
				{
					current_time->day = pre_stage_time.day - 1;		//��
				}
			}
			
			/*
				���صĿ��Ʒ�ʽ�ͷ�������Ч������������8eʱ����Ϊ��һ������
				��û���ѵ�ʱ�䣬�򷵻�2��������һ����дΪ��һ���0����Ϊ����
			*/
			else  
			{
					table_0x8E_str = search_8e_timeEventIDNEXT(time_stage_index, &orig_time_data);
				
					table_0x8E_return->hour 	= table_0x8E_str.hour; 
					table_0x8E_return->minute = table_0x8E_str.minute;
					return  (__TRUE);    //2���ֽڸ��ֽ�Ϊ���Ʒ�ʽ�����ֽ�Ϊ������
			}
		}
	}
	return  (__FALSE);						//û���ѵ����򽵼�
}
