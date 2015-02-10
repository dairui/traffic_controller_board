#include "Include.h"

extern u32 DB_Read_M(TARGET_DATA *p_Target);
extern u32 DB_Write_M(TARGET_DATA *p_Target);

/*==============================================================================================
* �﷨��ʽ��    void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ��̫����ѯʱ��
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
	u32  time_count=0;

	os_mut_wait(I2c_Mut_lm,0xFFFF);
	time_count = Read_DS3231_Time();//��ѯ��ǰʱ�䣬4�ֽ�����  
	os_mut_release(I2c_Mut_lm);
	
	if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))
	{
		p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
		p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																				(p_Target->sub_target) );   			
		p_To_eth->data[p_To_eth->lenth++]	=(u8)((time_count & 0xFF000000) >> 24);
		p_To_eth->data[p_To_eth->lenth++]	=(u8)((time_count & 0x00FF0000) >> 16);
		p_To_eth->data[p_To_eth->lenth++]	=(u8)((time_count & 0x0000FF00) >> 8);
		p_To_eth->data[p_To_eth->lenth++]	=(u8)(time_count & 0x000000FF);
	}
	else //if(p_Target->ID  == 0x87)     
	{													
		p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
		p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																				(p_Target->sub_target) );   			
		p_To_eth->data[p_To_eth->lenth++]	=0;  
		p_To_eth->data[p_To_eth->lenth++]	=0;  
		p_To_eth->data[p_To_eth->lenth++]	=0;  
		p_To_eth->data[p_To_eth->lenth++]	=0;  
	}

}
/*==============================================================================================
* �﷨��ʽ��    void eth_set_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ��̫������ʱ��
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
void eth_set_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{

//	u8 ID = 0;
	u16  daycnt = 0;
	u16  temp1 = 0;
	u32  timecount=0;
	u32 temp=0;
//	u32 i=0;
	
////	u32  	time_count									=0;
	u8 		Write_Time_Data[7] 					={0};  /*  дʱ�仺��  */



	if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))
	{
		
			timecount = p_Target->sub_buf[0];//����ʱ��---ת����4�ֽ�����
			timecount <<= 8;
			timecount |= p_Target->sub_buf[1];
			timecount <<= 8;
			timecount |= p_Target->sub_buf[2];
			timecount <<= 8;
			timecount |= p_Target->sub_buf[3];
			
			temp = timecount / 86400;  //�õ�����(��������Ӧ��)
			if(daycnt != temp)//����һ����
			{   
				daycnt = temp;
				temp1 = 1970;   //��1970�꿪ʼ
				while(temp >= 365)
				{
					if(Is_Leap_Year(temp1))  //������
					{
						if(temp >= 366)
						{
							temp -= 366;  //�����������
						}
						else 
						{
							temp1++;
							break;
						}
					} 
					else
					{
						temp -= 365; //ƽ��
					}
					temp1++;
				}
				Write_Time_Data[6] = temp1 - 1970;  //�õ����
				temp1 = 0;
				while(temp >= 28) //������һ����
				{
					if(Is_Leap_Year(Write_Time_Data[6] + 1970) && (temp1 == 1))//�����ǲ�������/2�·�
					{
						if(temp >= 29)
							temp -= 29;  //�����������
						else 
							break;
					}
					else
					{
						if(temp >= mon_table[temp1])
							temp -= mon_table[temp1];  //ƽ��
						else 
							break;
					}
					temp1++;
				}
				Write_Time_Data[5] = temp1 + 1;  //�õ��·�
				Write_Time_Data[4] = temp + 1;    //�õ�����
			}
			temp = timecount % 86400; //�õ�������
			Write_Time_Data[2] = temp / 3600; //Сʱ
			Write_Time_Data[1] = (temp % 3600) / 60; //����
			Write_Time_Data[0] = (temp % 3600) % 60; //����
			Write_Time_Data[3] = RTC_Get_Week((u16)(Write_Time_Data[6] + 1970),Write_Time_Data[5],Write_Time_Data[4]);//��ȡ����
			Write_Time_Data[0] = Write_Time_Data[0] / 10 *16 + Write_Time_Data[0] %10;
			Write_Time_Data[1] = Write_Time_Data[1] / 10 *16 + Write_Time_Data[1] %10;
			Write_Time_Data[2] = Write_Time_Data[2] / 10 *16 + Write_Time_Data[2] %10;
			Write_Time_Data[3] = Write_Time_Data[3] / 10 *16 + Write_Time_Data[3] %10;
			Write_Time_Data[4] = Write_Time_Data[4] / 10 *16 + Write_Time_Data[4] %10;
			Write_Time_Data[5] = Write_Time_Data[5] / 10 *16 + Write_Time_Data[5] %10;
                        
			if(Write_Time_Data[6] < 30)//2000��֮ǰ�����ڲ������ã�����Ӧ��
			{
				p_To_eth->error										=__TRUE;
				p_To_eth->error_type 							=ERROR_OTHER;   
		 
			}
			else
			{
				Write_Time_Data[6] = (Write_Time_Data[6] - 30) / 10 *16 + Write_Time_Data[6] %10;//2000���
			
				os_mut_wait(I2c_Mut_lm,0xFFFF);   //  I2C_lm
				I2C_Write_DS3231(Write_Time_Data);  //  I2C_lm   �д��Ķ���		
				os_mut_release(I2c_Mut_lm);	
				
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
				p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																					(p_Target->sub_target) ); 
			}
		}
		 /* end of "if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))"   */
	else //if(p_Target->ID  == 0x87)     
	{		
		p_To_eth->error										=__TRUE;
		p_To_eth->error_type 							=ERROR_OTHER;   
	}
}

// void eth_set_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
// {

// 	u32  	time_count									=0;
// 	u8 		write_time_data[7] 					={0};  /*  дʱ�仺��  */


// 	u32 time_set_day = 0;//����ʱ��,��
// 	u32 time_set_sec = 0;//����ʱ�䣬��
// 	u32 time_init = 0;//����������׼ʱ��

// 	if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))
// 	{
// 		time_count=((u32)(p_Target->sub_buf[0]<<24))|	//����ʱ��---ת����4�ֽ�����
// 							 ((u32)(p_Target->sub_buf[1]<<16))|
// 							 ((u32)(p_Target->sub_buf[2]<<8))	|
// 							 ((u32) p_Target->sub_buf[3]);	

// 		time_set_day = time_count / 86400;  //�õ�����(��������Ӧ��)
// 		if(time_set_day > 0)//����һ����
// 		{   
// 			time_init = 1970;   //��1970�꿪ʼ
// 			while(time_set_day >= 365)
// 			{
// 				if(Is_Leap_Year(time_init))  //������
// 				{
// 					if(time_set_day >= 366)
// 					{
// 						time_set_day -= 366;  //���������
// 					}
// 					else 
// 					{
// 						time_init++;
// 						break;
// 					}
// 				} 
// 				else
// 				{
// 					time_set_day -= 365; //ƽ��
// 				}
// 				time_init++;
// 			}
// 			//========
// 			write_time_data[6] = time_init - 1970;  //�õ����

// 			time_init = 0;
// 			while(time_set_day >= 28) //������һ����
// 			{
// 				if(Is_Leap_Year(write_time_data[6] + 1970) && (time_init == 1))//�����ǲ�������/2�·�
// 				{
// 					if(time_set_day >= 29)
// 						time_set_day -= 29;  //������
// 					else 
// 						break;
// 				}
// 				else
// 				{
// 					if(time_set_day >= mon_table[time_init])
// 						time_set_day -= mon_table[time_init];  //ƽ��
// 					else 
// 						break;
// 				}
// 				time_init++;
// 			}
// 			//========
// 			write_time_data[5] = time_init + 1;  //�õ��·�
// 			write_time_data[4] = time_set_day + 1;    //�õ�����
// 		}

// 		time_set_sec = time_count % 86400; //�õ�һ���ڵ�������
// 		write_time_data[0] = (time_set_day % 3600) % 60; //����
// 		write_time_data[1] = (time_set_day % 3600) / 60; //����
// 		write_time_data[2] = time_set_day / 3600; //Сʱ
// 		write_time_data[3] = RTC_Get_Week((u16)(write_time_data[6] + 2000),write_time_data[5],write_time_data[4]);//��ȡ����
// 		//����ת��
// 		write_time_data[0] = write_time_data[0] / 10 *16 + write_time_data[0] %10;
// 		write_time_data[1] = write_time_data[1] / 10 *16 + write_time_data[1] %10;
// 		write_time_data[2] = write_time_data[2] / 10 *16 + write_time_data[2] %10;
// 		write_time_data[3] = write_time_data[3] / 10 *16 + write_time_data[3] %10;
// 		write_time_data[4] = write_time_data[4] / 10 *16 + write_time_data[4] %10;
// 		write_time_data[5] = write_time_data[5] / 10 *16 + write_time_data[5] %10;
// 	//---------------------
// 		if(write_time_data[6] < 30)//2000��֮ǰ�����ڲ������ã�����Ӧ��	
// 		{
// 			p_To_eth->error										=__TRUE;
// 			p_To_eth->error_type 							=ERROR_OTHER;   	
// 		}
// 		else   //��ȷ�ģ���Ҫ����
// 		{
// 			write_time_data[6] = (write_time_data[6] - 30) / 10 *16 + write_time_data[6] %10;//2000���
// 		
// 			os_mut_wait(I2c_Mut_lm,0xFFFF);   //  I2C_lm
// 			I2C_Write_DS3231(write_time_data);  //  I2C_lm   �д��Ķ���		
// 			os_mut_release(I2c_Mut_lm);				

// 			p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
// 			p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
// 																					(p_Target->sub_target) );   			
// 			p_To_eth->data[p_To_eth->lenth++]	=0;  
// 			p_To_eth->data[p_To_eth->lenth++]	=0;  
// 			p_To_eth->data[p_To_eth->lenth++]	=0;  
// 			p_To_eth->data[p_To_eth->lenth++]	=0;  			
// 	 }
// 	}  /* end of "if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))"   */
// 	else //if(p_Target->ID  == 0x87)     
// 	{		
// 		p_To_eth->error										=__TRUE;
// 		p_To_eth->error_type 							=ERROR_OTHER;   
// 	}
// }

/*==============================================================================================
* �﷨��ʽ��    void eth_read_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ��̫����ȡ���ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/

OS_SEM						src_Db_update_task;   //  ���ڶ�д���ݿ�   //debug_lm
 //db_wr_re_struct 	db_wr_re						={ {0}, {NULL}, {NULL}, {NULL} };  //debug_lm
#define DB_ACTION_READ  	1    //  debug_lm
#define DB_ACTION_WRITE  	2    //  debug_lm 
extern u8 DB_Read_Real_Line(TARGET_DATA *p_Target);

void eth_read_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
//	u32 							i										=0;
	u32 							j										=0;


	u8 								ID_temp 						=0;
	u8 								count_index_temp		=0;
	u8 								sub_target_temp			=0;


	u32								lenth								=0;	
	u32								tx_data_lenth      	=0;
	
	u8 								Index1_temp					= 0;
	u8 								Index2_temp					= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;
	
	
//	u8								read_error_or_true  =0;
	
	ID_temp 						=p_Target->ID;
	count_index_temp		=p_Target->count_index;
	sub_target_temp			=p_Target->sub_target;
	
	Index1_temp			= p_Target->sub_index[0];
	Index2_temp			= p_Target->sub_index[1];
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;

	os_mut_wait(Spi_Mut_lm,0xFFFF);
	lenth =DB_Read_M(p_Target);
	os_mut_release(Spi_Mut_lm);	
	

	

 	if(lenth !=0)  //û����
 	{
		p_To_eth->data[p_To_eth->lenth++]	= ID_temp;
		p_To_eth->data[p_To_eth->lenth++]	= (((count_index_temp)<<6) | (sub_target_temp) );

		
		
		os_mut_wait(Spi_Mut_lm,0xFFFF);
// // // 		read_error_or_true	=DB_Read_Real_Line(p_Target) ;
		
// // // 		if((read_error_or_true != 0) &&	(count_index_temp == 0))//���������������ѯʱ����λ��д������
// // // 		{
// // // 			p_To_eth->data[p_To_eth->lenth++]	= DB_Read_Real_Line(p_Target);
// // // 		}
		os_mut_release(Spi_Mut_lm);	
		
		
		
// // // 		if((Index1_max_temp != 0) && (Index2_max_temp != 0) && (count_index_temp == 0))
// // // 		{ //˫���������ѯ����λ��д������2�����ֵ
// // // 			p_To_eth->data[p_To_eth->lenth++]	= Index2_max_temp;
// // // 		}
		
		
		if(count_index_temp 			==0)				// ����������(������,˫����)
		{
				/*
				��������ı���"����������"���Ѿ�������"�����ʶ"��ռ��һ���ֽ���,
				�Լ�"��������+�Ӷ����"��ռ��һ���ֽ���
				���,�˴�Ҫ��ȥ 2.
				*/
// // 				line_temp=Object_table[ID_temp-0x81].Line_Number; 		//�õ���ǰID��Ӧ��������� (line_max)
// // 				tx_data_lenth=(Object_table[ID_temp-0x81].Object_Number - BAOLIU*line_temp -2-1); //���еĲ����ĳ���
// // 				for(j=0;j<tx_data_lenth;j++)
// // 				{
// // 					p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
// // 				}			


				for(j=0;j<lenth;j++)
				{
					p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
				}		

				
		}
		else if((count_index_temp 	==1)&&(Index1_max_temp != 0)&&(Index2_max_temp == 0))  			// ������
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
			
				if(sub_target_temp ==0)  	//����
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//ÿ�еĳ���
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  �����Ӷ���
				{	 
						tx_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//�����Ӷ���Ĵ�С
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}			
				}
			
		}
		else if((count_index_temp ==2)&&(Index1_max_temp != 0)&&(Index2_max_temp != 0)
			&&(Index1_temp != 0)&&(Index2_temp == 0))  			// ˫�����ĵ�������ʽ
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
// // 				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];
			  p_To_eth->data[p_To_eth->lenth++]	=lenth / Object_table[ID_temp-0X81].Line_leng; // // abcdefg //
				if(sub_target_temp ==0)  	//����
				{
						tx_data_lenth=lenth;//(Object_table[ID_temp-0x81].Line_Byte_Max * Index2_max_temp);			//ÿ�еĳ��� // abcdefg //
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  �����Ӷ���
				{	 
							p_To_eth->error										=__TRUE;
							p_To_eth->error_type 							=ERROR_OTHER;  
	
				}
			
		}
		else if((count_index_temp 	==2)&&(Index1_max_temp != 0)
			&&(Index2_max_temp != 0)&&(Index1_temp != 0)&&(Index2_temp != 0))   			// ˫����
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];
				if(sub_target_temp ==0)  	//����
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//ÿ�еĳ���
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  �����Ӷ���
				{	 
						tx_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//�����Ӷ���Ĵ�С
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}			
				}
		}
		else																	// ��������
		{
			/*  ������������  */
			p_To_eth->error										=__TRUE;
			p_To_eth->error_type 							=ERROR_OTHER;  
		}
			

		
		
		
		
		/*   �����󡢵�������˫�����Ļظ���һ�� */
	}
	else 														//������
	{
		p_To_eth->error										=__TRUE;
		p_To_eth->error_type 							=ERROR_OTHER;   		
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
/*==============================================================================================
* �﷨��ʽ��    void eth_read_line_only_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ��̫����ȡ���ݿ�,ֻ֧�ֵ������� ����  ��ȡ����
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
void eth_read_line_only_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
//	u32 							i										=0;
	u32 							j										=0;

	u32								tx_data_lenth      	=0;
	u8 								ID_temp 						=0;
	u8 								count_index_temp		=0;
	u8 								sub_target_temp			=0;


	u32								lenth								=0;	


	ID_temp 						=p_Target->ID;
	count_index_temp		=p_Target->count_index;
	sub_target_temp			=p_Target->sub_target;


	os_mut_wait(Spi_Mut_lm,0xFFFF);
	lenth =DB_Read_M(p_Target);
	os_mut_release(Spi_Mut_lm);	

	
	
	if(lenth  !=0)  //û����
	{
		p_To_eth->data[p_To_eth->lenth++]	=ID_temp;
		p_To_eth->data[p_To_eth->lenth++]	=(((count_index_temp)<<6) | (sub_target_temp) );   			

		
		if(count_index_temp 	==1)  			// ������
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
			
				if(sub_target_temp ==0)  	//����
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//ÿ�еĳ���
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  �����Ӷ���
				{	 
					p_To_eth->error										=__TRUE;
					p_To_eth->error_type 							=ERROR_OTHER; /*  ������������  */			
				}
		}	
		else																	// ��������
		{
			p_To_eth->error										=__TRUE;
			p_To_eth->error_type 							=ERROR_OTHER; /*  ������������  */
		}	
		/*   �����󡢵�������˫�����Ļظ���һ�� */
	}
		else 														//������
		{
			p_To_eth->error										=__TRUE;
			p_To_eth->error_type 							=ERROR_OTHER;   		
		}
	
}



// 								p_Target->count_index=2;	
// 								p_Target->sub_index[0]=No_line_temp;
// 								for(j=1;j<Index2_max_temp;j++)
// 								{
// 									p_Target->sub_index[1] = j;
// 									No_line_temp = ((p_Target->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2) 
// 												+ p_Target->sub_index[1];	//�к�
// 									DB_Read_M(p_Target);
// 									//��ӷ���  ����Ḳ�ǵ�
// 								}
								



/*==============================================================================================
* �﷨��ʽ��    void eth_set_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* ʵ�ֹ��ܣ�    ��̫���������ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����2��       to_eth_struct *	p_To_eth	����ETH������ָ��
* ����ֵ��      ��
===============================================================================================*/
void eth_set_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
//	u32 							i										=0;

	u8 								ID_temp 						=0;
	u8 								count_index_temp		=0;
	u8 								sub_target_temp			=0;

	u32 								db_error_or_right			=0;

	ID_temp 						=p_Target->ID;
	count_index_temp		=p_Target->count_index;
	sub_target_temp			=p_Target->sub_target;

	os_mut_wait(Spi_Mut_lm,0xFFFF);
	db_error_or_right	=DB_Write_M(p_Target);
	os_mut_release(Spi_Mut_lm);	

	
	if(db_error_or_right != 0 ) //û����
	{
		if(count_index_temp 			==0)				// ����������(������,˫����)
		{

			p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
			p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																					(p_Target->sub_target) );   	
		}
		else if(count_index_temp 	==1)  			// ������
		{
				if(sub_target_temp ==0)  	//����
				{		
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];		

				}
				else  				 						//  �����Ӷ���
				{	 
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
				}
			
		}
		else if(count_index_temp 	==2)  			// ˫����
		{
				if(sub_target_temp ==0)  	//����
				{
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];	
					
				}
				else  				 						//  �����Ӷ���
				{	 
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];	
				}
		}
		else																	// ��������
		{
			/*  ������������  */
		}
			

		
		
		
		
		/*   �����󡢵�������˫�����Ļظ���һ�� */
	}
	else 														//������
	{
		p_To_eth->error										=__TRUE;
		p_To_eth->error_type 							=ERROR_OTHER;   		
	}
	


}
//================================



u8  copy_to_eth(u8 *p_Data,u32 lenth)
{
	u32 			i    		 								=0;
	
	u32 			old_tx_wr_index					=0;
//	u32 			old_tx_counter					=0;
	
	u32 			old_tx_len_wr_index			=0;
	u32 			old_tx_len_counter			=0;	
	
	u8  			buff_overflow_temp      =0;
	u8  			len_overflow_temp     	=0;

	

// 	IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	tsk_lock ();  			//  disables task switching 
	
	old_tx_wr_index 			=tx_wr_index;
	old_tx_wr_index 			=tx_counter;
	
	old_tx_len_wr_index 	=tx_wr_index;
	old_tx_len_counter 		=tx_counter;
	
	for(i = 0;i < lenth;i++)
	{
			eth_tx_fifo[tx_wr_index] = p_Data[i];
			if (++tx_wr_index == TX_FIFO_SIZE) tx_wr_index=0;
			if (++tx_counter  == TX_FIFO_SIZE)
			{
				//tx_counter=1;
				//tx_buffer_overflow=__TRUE;
				buff_overflow_temp	=__TRUE;
			};												
	}	

	eth_tx_len[tx_len_wr_index] = lenth;
	if (++tx_len_wr_index == TX_LEN_SIZE) tx_len_wr_index=0;
	if (++tx_len_counter  == TX_LEN_SIZE)
	{
		/*  ���͵���Ϣ�������������ֵ(now =1000)  */
		//tx_len_counter=1;
		//tx_len_overflow=__TRUE;   
		len_overflow_temp	=__TRUE;
};
	
	if(buff_overflow_temp	==__TRUE)
	{
		tx_wr_index  	=old_tx_wr_index;
		tx_counter		=old_tx_wr_index;
		tx_wr_index		=old_tx_len_wr_index;
		tx_counter		=old_tx_len_counter;
	}
	/*
	�д���һ������
	else if(len_overflow_temp	==__TRUE)     
	{
		len_overflow_temp			=__FALSE;
	}
	*/
	tsk_unlock (); 			//  enable  task switching 	


// 	IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	if(buff_overflow_temp	==__TRUE) 
		return (1);
	else 
		return (0);
}








