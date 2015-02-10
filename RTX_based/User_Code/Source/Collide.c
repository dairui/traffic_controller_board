
#include "Include.h"

/**
  * @brief  8D全表搜索获得上一个时段的控制方式和方案号。u8 Search_8D_All_Prev(u8 Read_Time_Data[])
  *         具体为：
	*					当月当日当前时间起下搜索上一个时段控制方式和方案号，如8D表时段号对应的8E表无效					
	* 				则屏蔽对应的时段号继续搜索8D表最近的时段号  
  * @param  *Read_Time_Data,依次为当前时间星期、日、月
  * @retval 控制方式  方案号
	* @invovled function		search_8e_timeEventID();
	* 											search_8e_timeEventIDNEXT();
	* 											search_8D_timeTbId();
  */
u16 Phase_Collide_Detect(u16 cur_working_phase)
{
	/*	
	数据库操作需要的变量	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*读取97表计数*/
	u8	phase_conflict_count							=	0;
	/*从97表读取的一行冲突相位*/
	u16	phase_conflict_state							= 0;
	/*冲突相位与放行相位与后结果*/
	u16	phase_conflict_temp								= 0;
	
	u8 i;
	
	for(phase_conflict_count = 1;phase_conflict_count <= 16;phase_conflict_count++)
	{	
		//读取97
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
			phase_conflict_state	= 	data_Target.sub_buf[1];	//时段号
			phase_conflict_state <<= 8;
			phase_conflict_state  |= 	data_Target.sub_buf[2];	//时段号
		}	
		//读取97完毕
		
		phase_conflict_temp = phase_conflict_state & cur_working_phase;
		if(phase_conflict_temp > 0)/*大于0的相位有可能是冲突相位，继续判断*/
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
//冲突检测
//输入：阶段放行相位
//此函数需从数据库中读取0xCA表
//return 0,正常;1,告警;2,冲突;//3,未启用相位
//================================
u8 Collide_Detect( u16 phase_bits)
{	
	u8 Flag = 0;
	u8 j = 0, i = 0, k = 0;
	u16 barrier1 = 0, barrier2 = 0;
	u8 phase[16] = {0};
	u8 count_flag = 0;

	Barrier();//分屏障
	for(i = 0; i <= 15; i++)//分为barrier1和barrier2
      {
              if(ring_stutas[i].barrier_flag == 1)
              {
              	barrier1 |= 1 << ring_stutas[i].Phase_num;
              }
      }
	barrier2 = ~barrier1;
	
	for(i = 0; i <= 15; i++)//提取阶段放行相位，存于phase数组
      {
              if(((phase_bits >> i) & 0x01) == 1)
                      phase[j++] = i + 1;
      }
	for(i= 0; i < j; i++)//剔除没启用的放行相位
      {
              if(ring_stutas[phase[i]].Func_ON_OFF != 1)
              {
              	//Flag = 3;//Func_ON_OFF == 0，至少有一个相位没启用
            		phase_bits  &=  (~(1 << (ring_stutas[phase[i]].Phase_num - 1)));
              }
      }
	for(i = 0; i <= 15; i++)//重新提取阶段放行相位，存于phase数组
      {
              if(((phase_bits >> i) & 0x01) == 1)
                      phase[j++] = i + 1;
      }
	if ((phase_bits & barrier1) != 0 && (phase_bits & barrier2))
	{	
	    return Flag = 2;//冲突
	}
	
	for(k = 0; k < j; k++)
		for(i = 0; i < j; i++)
		{
			if(ring_stutas[phase[k]].Ring_num == ring_stutas[phase[i]].Ring_num
                           && ring_stutas[phase[i]].Ring_num != 0
                           && ring_stutas[phase[i]].Ring_num != 0)//存在同环相位
			{
				return Flag = 1;//告警
			}
			if(((1 << (ring_stutas[phase[k]].Phase_num - 1))
				& ring_stutas[phase[i]].SIMUL_Phase) == 0)//phase[k]不是其他放行相位的并发相位
			{
				   count_flag++;
				   if(count_flag == j)
				   {
				   	return Flag = 1;//告警
				   }
			}
			count_flag = 0;
		}
        Flag = 0;
	return Flag;//并发
}

//===========================================
//void Barrier(void)
//相位屏障
//需从数据库中读取0xCA和0xCB表
//编程思想：令相位1的屏障标志为1，再把相位1的并发相位（比如，相位x，y。。。）的屏障标志也置1；
//再由相位1的并发相位（x，y，。。）出发，将其的并发相位也置1，以此类推。。。。知道所有与相位1
//同屏障的相位的屏障标志都为1.
//===========================================
void Barrier(void)
{
// // 	u8 phase_order[17][2] = {0};
// // 	u8 retVal;
// // 	u8 DB_FALSE = 0;
// // 	//u16 phase_simul = 0;//并发相位
// // 	u8 i = 0, j = 0, k = 1;
// // 	u8 flag_end = 0;

// // 	 for(j = 0; j < 50; j++)
// //             Out_Data[j] = 0;
// // 	for(i = 1; i <= 16; i++)
// // 	{
// // 		os_mut_wait(Spi_Mut_lm,0xFFFF);
// // 		retVal = Db_Read(0xCB, i, 0,0);//从数据库中读取0xCB表
// // 		os_mut_release(Spi_Mut_lm);	

// // 		if(retVal != DB_FALSE)
// // 		{
// // 			//memset(&Table_0xCB_read,0,sizeof(Phase_Order_Table));
// // 			Db_MemCopy(0xCB, &Table_0xCB_read,Out_Data,sizeof(Phase_Order_Table));
// // 		}		
// // 		phase_order[i][0] = Table_0xCB_read.Ring_num;//存于phase_order数组
// // 		phase_order[i][1] = Table_0xCB_read.Phase_num;
// // 	}
// // 	for(i = 1; i <= 16; i++)//转存到ring_stutas数组结构体中
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
// // 		//校验,不对再读一遍
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

// // 	ring_stutas[1].barrier_flag = 1;//以与相位1同屏障的相位的屏障标志置1
// // 	k = 1;
// // 	while(flag_end == 0)//直到与相位1同屏障的所有相位的屏障标志都置1
// // 	{
// // 		if(k > 16)
// // 		{
// // 			k = 1;
// // 		}
// //                 for(k = 1; k <= 16; k++)//将所有相位的并发相位提取并存于phase_simul数组
// //                 { 
// //                   j = 0;
// //                   for(i = 0; i <= 15; i++)
// //                   {
// //                           if(((ring_stutas[k].SIMUL_Phase >> i) & 0x01) == 1)
// //                                   ring_stutas[k].phase_simul[j++] = i+1;
// //                   }
// //                 }
// //                 k = 1;
// // 		while(k <= 16)//屏障分割相位
// // 		{	
// //                         j = 0;
// // 			flag_end = 1;
// //                         k++;
// //                         i = k;
// //                         while((ring_stutas[k - 1].phase_simul[j]) != 0 && ring_stutas[k - 1].barrier_flag == 1)
// // 			{//存在并发相位且屏障标志为1
// // 				if(ring_stutas[ring_stutas[k -1].phase_simul[j]].barrier_flag != 1)//将其并发相位的屏障标志置1
// // 				{
// // 					ring_stutas[ring_stutas[k - 1].phase_simul[j]].barrier_flag = 1;
// // 					i = ring_stutas[k - 1].phase_simul[j];
// // 					flag_end = 0;
// // 				}
// // 				j++;
// // 			}
// //                         k = i;//转到并发相位
// // 		}
// // 	}
}
