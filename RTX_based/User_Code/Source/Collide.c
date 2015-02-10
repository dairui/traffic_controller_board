
#include "Include.h"

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
u16 Phase_Collide_Detect(u16 cur_working_phase)
{
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*��ȡ97�����*/
	u8	phase_conflict_count							=	0;
	/*��97���ȡ��һ�г�ͻ��λ*/
	u16	phase_conflict_state							= 0;
	/*��ͻ��λ�������λ�����*/
	u16	phase_conflict_temp								= 0;
	
	u8 i;
	
	for(phase_conflict_count = 1;phase_conflict_count <= 16;phase_conflict_count++)
	{	
		//��ȡ97
		read_lenth_temp				=	0;
		data_Target.ID 				= 	0x97;
		data_Target.count_index 		= 	1;
		data_Target.sub_index[0] 		= 	phase_conflict_count;
		data_Target.sub_index[1] 		= 	0;
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
			phase_conflict_state	= 	data_Target.sub_buf[1];	//ʱ�κ�
			phase_conflict_state <<= 8;
			phase_conflict_state  |= 	data_Target.sub_buf[2];	//ʱ�κ�
		}	
		//��ȡ97���
		
		phase_conflict_temp = phase_conflict_state & cur_working_phase;
		if(phase_conflict_temp > 0)/*����0����λ�п����ǳ�ͻ��λ�������ж�*/
		{
			for(i = phase_conflict_count;i<16;i++)
			{
				if((phase_conflict_temp>>i & 1) == 1)
				{
					if((cur_working_phase>>(phase_conflict_count-1) & 1) == 1)
					{
						return cur_working_phase;
					}
				}
			}
		}
		
	}
	return 0;
	
	
}


//================================
//u8 Collide_Detect( u16 phase_bits)
//��ͻ���
//���룺�׶η�����λ
//�˺���������ݿ��ж�ȡ0xCA��
//return 0,����;1,�澯;2,��ͻ;//3,δ������λ
//================================
u8 Collide_Detect( u16 phase_bits)
{	
	u8 Flag = 0;
	u8 j = 0, i = 0, k = 0;
	u16 barrier1 = 0, barrier2 = 0;
	u8 phase[16] = {0};
	u8 count_flag = 0;

	Barrier();//������
	for(i = 0; i <= 15; i++)//��Ϊbarrier1��barrier2
      {
              if(ring_stutas[i].barrier_flag == 1)
              {
              	barrier1 |= 1 << ring_stutas[i].Phase_num;
              }
      }
	barrier2 = ~barrier1;
	
	for(i = 0; i <= 15; i++)//��ȡ�׶η�����λ������phase����
      {
              if(((phase_bits >> i) & 0x01) == 1)
                      phase[j++] = i + 1;
      }
	for(i= 0; i < j; i++)//�޳�û���õķ�����λ
      {
              if(ring_stutas[phase[i]].Func_ON_OFF != 1)
              {
              	//Flag = 3;//Func_ON_OFF == 0��������һ����λû����
            		phase_bits  &=  (~(1 << (ring_stutas[phase[i]].Phase_num - 1)));
              }
      }
	for(i = 0; i <= 15; i++)//������ȡ�׶η�����λ������phase����
      {
              if(((phase_bits >> i) & 0x01) == 1)
                      phase[j++] = i + 1;
      }
	if ((phase_bits & barrier1) != 0 && (phase_bits & barrier2))
	{	
	    return Flag = 2;//��ͻ
	}
	
	for(k = 0; k < j; k++)
		for(i = 0; i < j; i++)
		{
			if(ring_stutas[phase[k]].Ring_num == ring_stutas[phase[i]].Ring_num
                           && ring_stutas[phase[i]].Ring_num != 0
                           && ring_stutas[phase[i]].Ring_num != 0)//����ͬ����λ
			{
				return Flag = 1;//�澯
			}
			if(((1 << (ring_stutas[phase[k]].Phase_num - 1))
				& ring_stutas[phase[i]].SIMUL_Phase) == 0)//phase[k]��������������λ�Ĳ�����λ
			{
				   count_flag++;
				   if(count_flag == j)
				   {
				   	return Flag = 1;//�澯
				   }
			}
			count_flag = 0;
		}
        Flag = 0;
	return Flag;//����
}

//===========================================
//void Barrier(void)
//��λ����
//������ݿ��ж�ȡ0xCA��0xCB��
//���˼�룺����λ1�����ϱ�־Ϊ1���ٰ���λ1�Ĳ�����λ�����磬��λx��y�������������ϱ�־Ҳ��1��
//������λ1�Ĳ�����λ��x��y������������������Ĳ�����λҲ��1���Դ����ơ�������֪����������λ1
//ͬ���ϵ���λ�����ϱ�־��Ϊ1.
//===========================================
void Barrier(void)
{
// // 	u8 phase_order[17][2] = {0};
// // 	u8 retVal;
// // 	u8 DB_FALSE = 0;
// // 	//u16 phase_simul = 0;//������λ
// // 	u8 i = 0, j = 0, k = 1;
// // 	u8 flag_end = 0;

// // 	 for(j = 0; j < 50; j++)
// //             Out_Data[j] = 0;
// // 	for(i = 1; i <= 16; i++)
// // 	{
// // 		os_mut_wait(Spi_Mut_lm,0xFFFF);
// // 		retVal = Db_Read(0xCB, i, 0,0);//�����ݿ��ж�ȡ0xCB��
// // 		os_mut_release(Spi_Mut_lm);	

// // 		if(retVal != DB_FALSE)
// // 		{
// // 			//memset(&Table_0xCB_read,0,sizeof(Phase_Order_Table));
// // 			Db_MemCopy(0xCB, &Table_0xCB_read,Out_Data,sizeof(Phase_Order_Table));
// // 		}		
// // 		phase_order[i][0] = Table_0xCB_read.Ring_num;//����phase_order����
// // 		phase_order[i][1] = Table_0xCB_read.Phase_num;
// // 	}
// // 	for(i = 1; i <= 16; i++)//ת�浽ring_stutas����ṹ����
// // 	{
// // 		
// // 		os_mut_wait(Spi_Mut_lm,0xFFFF);
// // 		retVal = Db_Read(0xCA, phase_order[i][0], phase_order[i][1],0);
// // 		os_mut_release(Spi_Mut_lm);	

// // 		if(retVal != DB_FALSE)
// // 		{
// // 			//memset(&Table_0xCA_read,0,sizeof(Ring_Stutas_Table));
// // 			Db_MemCopy(0xCA, &Table_0xCA_read,Out_Data,sizeof(Ring_Stutas_Table));
// // 		}
// // 		//У��,�����ٶ�һ��
// // 		if(Table_0xCA_read.Ring_num != phase_order[i][0] 
// // 			|| Table_0xCA_read.Phase_num != phase_order[i][1])
// // 		{
// // 			
// // 			os_mut_wait(Spi_Mut_lm,0xFFFF);
// // 			retVal = Db_Read(0xCA, phase_order[i][0], phase_order[i][1],0);
// // 			os_mut_release(Spi_Mut_lm);	

// // 			if(retVal != DB_FALSE)
// // 			{
// // 				//memset(&Table_0xCA_read,0,sizeof(Ring_Stutas_Table));
// // 				Db_MemCopy(0xCA, &Table_0xCA_read,Out_Data,sizeof(Ring_Stutas_Table));
// // 			}
// // 		}
// // 		ring_stutas[phase_order[i][1]].Ring_num	= Table_0xCA_read.Ring_num;
// // 		ring_stutas[phase_order[i][1]].Phase_num = Table_0xCA_read.Phase_num;
// // 		ring_stutas[phase_order[i][1]].SIMUL_Phase = Table_0xCA_read.SIMUL_Phase;
// // 		ring_stutas[phase_order[i][1]].Func_ON_OFF = Table_0xCA_read.Func_ON_OFF;
// // 		
// // 	}

// // 	ring_stutas[1].barrier_flag = 1;//������λ1ͬ���ϵ���λ�����ϱ�־��1
// // 	k = 1;
// // 	while(flag_end == 0)//ֱ������λ1ͬ���ϵ�������λ�����ϱ�־����1
// // 	{
// // 		if(k > 16)
// // 		{
// // 			k = 1;
// // 		}
// //                 for(k = 1; k <= 16; k++)//��������λ�Ĳ�����λ��ȡ������phase_simul����
// //                 { 
// //                   j = 0;
// //                   for(i = 0; i <= 15; i++)
// //                   {
// //                           if(((ring_stutas[k].SIMUL_Phase >> i) & 0x01) == 1)
// //                                   ring_stutas[k].phase_simul[j++] = i+1;
// //                   }
// //                 }
// //                 k = 1;
// // 		while(k <= 16)//���Ϸָ���λ
// // 		{	
// //                         j = 0;
// // 			flag_end = 1;
// //                         k++;
// //                         i = k;
// //                         while((ring_stutas[k - 1].phase_simul[j]) != 0 && ring_stutas[k - 1].barrier_flag == 1)
// // 			{//���ڲ�����λ�����ϱ�־Ϊ1
// // 				if(ring_stutas[ring_stutas[k -1].phase_simul[j]].barrier_flag != 1)//���䲢����λ�����ϱ�־��1
// // 				{
// // 					ring_stutas[ring_stutas[k - 1].phase_simul[j]].barrier_flag = 1;
// // 					i = ring_stutas[k - 1].phase_simul[j];
// // 					flag_end = 0;
// // 				}
// // 				j++;
// // 			}
// //                         k = i;//ת��������λ
// // 		}
// // 	}
}
