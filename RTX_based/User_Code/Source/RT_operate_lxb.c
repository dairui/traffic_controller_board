#include "Include.h"

//====================lxblllllllllllllllllll
STAGE_TIMING_TABLE_t 			sche_hold_guand = {0};		//看守数据结构体，该结构体用的是C1表结构
sche_optimize_struct			sche_optimize 	= {0};		//优化数据结构体，该结构体为自定义的结构


//===================


u8  sche_change_tpye[][4] =
{
/*
把开机黄闪和开机全红也当作一种控制类型，
控制方式项为0，表明此控制类型无效，不为0则执行该控制类型		
*/
	
/* 索引				控制类型		方案号			控制方式*/
	{0,					0,					0,					0},   /*	开机黄闪			*/
	{1,					1,					0,					0},		/*	开机全红			*/
	{2,					2,					0,					0},		/*	步进	0xBB		*/
	{3,					3,					0,					0},		/*	手动  0xB5 		*/
	{4,					4,					0,					0},		/*	系统  0xB6		*/
	{5,					0,					0,					0},		/*	自主控制			*/
	{6,					0,					0,					0},		/*	黄闪控制			*/
	{7,					0,					0,					0},		/*	当前控制方案	*/
};



//====================lxblllllllllllllllllll

/*==============================================================================================
* 语法格式：    u8 rt_operate_written(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
* 实现功能：    协议解析后，将实时操作部分数据写入rt_operate_data_struct结构体中
* 参数1：       PROTOCOL_DATA *p_Protocol 传入协议解析得来的数据结构体到该函数
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
===============================================================================================*/
u8 rt_operate_written(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
{
	u8 				i,	j;
	u8 				index;								//用于数组写入，index++
	u8 *   msg_to_dispatch = NULL;			//发送邮箱消息变量
	
 	static u8				to_dispatch_msg[5];		//发送给方案调度任务消息
	
	
	u8 *   p_Char     =NULL;
	u32		 k					=0;	
	
	
	switch (p_Protocol->count_target)
	{
		case 1:								//单对象设置，包括BB表、B5表、B6表、B7表
			{		
				if (p_Protocol->opobj_data[0].ID  			 == 0xB6 &&
						p_Protocol->opobj_data[0].sub_buf[0] == 0)//B6表,只能单独设置为0，自主控制
				{
					/* 此处以消息的形式发送给方案调度任务，
					 * 协议解析部分不用填充sche_change_tpye数组
					 * 填充数组部分在方案调度任务末尾
					 */
// 					os_mut_wait(RT_Operate_Mut,0xFFFF);   //用于方案数据数组修改
// 					sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[0].sub_buf[0];//给b6表赋值方案号
// 					os_mut_release(RT_Operate_Mut);
					/*单独设置B6表，只能设置为恢复	*/
// // 					to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[0].sub_buf[0];
					to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
					to_dispatch_msg[3]  = 0xb6;
				} 
				else if (p_Protocol->opobj_data[0].ID  == 0xB7)
				{
					/*单独设置B7表，只能设置全红黄闪	*/
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
		case 2:								//两个对象设置，B7表与B6表
			{
				if ((p_Protocol->opobj_data[0].ID  == 0xB7) && (p_Protocol->opobj_data[1].ID  == 0xB6))
				{
					/* 此处以消息的形式发送给方案调度任务，
					 * 协议解析部分不用填充sche_change_tpye数组
					 * 填充数组部分在方案调度任务末尾
					 */
// 					os_mut_wait(RT_Operate_Mut,0xFFFF);   //用于方案数据数组修改
// 					
// 					sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];//给系统控制B6设置控制方式
// 					sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];			//给b6表赋值方案号
// 					
// 					os_mut_release(RT_Operate_Mut);
					if (p_Protocol->opobj_data[0].sub_buf[0] != 0 &&
						  p_Protocol->opobj_data[1].sub_buf[0] != 0)
					{
						to_dispatch_msg[MSGA_SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						to_dispatch_msg[MSGA_CONTROL_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
					}
					else if(p_Protocol->opobj_data[1].sub_buf[0] == 0)//恢复自主控制
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
		case 3:								//三个对象设置，依次为B7表、B6表、C1表，且B7表值为0x04
			{
				if ((p_Protocol->opobj_data[0].ID  == 0xB7) && 
				(p_Protocol->opobj_data[1].ID  == 0xB6) && 
				(p_Protocol->opobj_data[2].ID  == 0xC1)	)
				{/*看守包括B6，C1，有两方案号，具体取哪个发送方案调度，此处取B6表	*/
					if ((p_Protocol->opobj_data[0].sub_buf[0]) == HOLD_WAY)
					{
						/* 此处以消息的形式发送给方案调度任务，并填充看守数据结构体，该结构体用的是C1表结构
						 * 协议解析部分不用填充sche_change_tpye数组，填充数组部分在方案调度任务末尾
						 */
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //用于方案数据数组修改
						
// 						sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];//给系统控制B6设置控制方式
// 						sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];			//给b6表赋值方案号
						
						//设置看守警卫任务下，保存看守数据到结构体中，并置标志hold_guard_flag
						
						p_Char=(u8*)(&sche_hold_guand);
				
						for(k=0;k<sizeof(STAGE_TIMING_TABLE_t);k++)
						{
							*(p_Char+k)=p_Protocol->opobj_data[2].sub_buf[k];
						}						

						sche_hold_guand.index = sche_change_tpye[NOW_C][SCHE_NUM];
						sche_hold_guand.relPhase = (((sche_hold_guand.relPhase<<8)&0xff00)|((sche_hold_guand.relPhase>>8)&0xff));
						
						
// // 						sche_hold_guand.index 				= p_Protocol->opobj_data[2].sub_buf[0];		//方案号
// // 						sche_hold_guand.stageId 			= p_Protocol->opobj_data[2].sub_buf[1];		//阶段号
// // 						sche_hold_guand.relPhase 			= p_Protocol->opobj_data[2].sub_buf[2];				//相位H
// // 						sche_hold_guand.relPhase 	  	= sche_hold_guand.relPhase <<8;
// // 						sche_hold_guand.relPhase 	  	|= p_Protocol->opobj_data[2].sub_buf[3];			//相位L
// // 						sche_hold_guand.stGreenTime 	= p_Protocol->opobj_data[2].sub_buf[4];			//绿灯时间
// // 						sche_hold_guand.stYellowTime 	= p_Protocol->opobj_data[2].sub_buf[5];			//黄灯时间
// // 						sche_hold_guand.stRedTime  		= p_Protocol->opobj_data[2].sub_buf[6];			//红灯时间
// // 						sche_hold_guand.stAlterParam 	= p_Protocol->opobj_data[2].sub_buf[7];			//阶段选项参数
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
		case 6:								//六个对象设置，依次为B7表、B6、B8表、B9表、BE表、BF表，且B7表值为0x0C，优化指定方案
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
							(p_Protocol->opobj_data[2].sub_buf[0])	> 0 )				//若控制方式为优化,且优化方案不为黄闪全红，且Period_Time设置大于0才有效
					{
						/* 此处以消息的形式发送给方案调度任务，并填充优化数据结构体，该结构体用的是自定义lxb
						 * 协议解析部分不用填充sche_change_tpye数组，填充数组部分在方案调度任务末尾
						 */
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //用于方案数据数组修改
					
// 						/*给系统控制B6设置控制方式B7	*/
// 						sche_change_tpye[SYSTEM_C][SCHE_CON_WAY] = p_Protocol->opobj_data[0].sub_buf[0];
// 						sche_change_tpye[SYSTEM_C][SCHE_NUM] = p_Protocol->opobj_data[1].sub_buf[0];
						
						/*	设置系统优化下，保存优化数据到结构体中	*/
						sche_optimize.Period_Time		 		= p_Protocol->opobj_data[2].sub_buf[0];//B8表写入
						sche_optimize.Phase_Difference 	= p_Protocol->opobj_data[3].sub_buf[0];//B9表写入
						
						//BE表写入优化数据结构体
						index = 0;
						for(i = 0; i <= 15; i++)
						{
							sche_optimize.Stage_Time[i] = p_Protocol->opobj_data[4].sub_buf[index++];
						}
						
						//BF表写入优化数据结构体
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
	
	/* 设置应答	*/
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
 
	/* 协议解析触发方案调度任务	*/
	to_dispatch_msg[MSGA_TRIGGER_WAY] = 2;
	msg_to_dispatch	= to_dispatch_msg;
	os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
	return __TRUE;
}


/*==============================================================================================
* 语法格式：    rt_operate_read(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    实时操作数据查询，包括步进BB，手动B5，系统控制B6，控制方式B7，看守下C1，优化下B8，B9，BE，BF
* 参数1：       PROTOCOL_DATA *p_Protocol 传入协议解析得来的数据结构体到该函数
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
===============================================================================================*/
u8 rt_operate_read(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth)
{
	u8			i,		j;
	u8			holding_temp 		= 0;			//当前系统是否处于看守中
	u8			optimizing_temp = 0;			//当前系统是否处于优化中	

	

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	if ((p_Protocol->opobj_data[0].ID  == 0xB7) && 
			(p_Protocol->opobj_data[1].ID  == 0xB6) && 
			(p_Protocol->opobj_data[2].ID  == 0xC1)	)							//查询看守数据
	{
		os_mut_wait(RT_Operate_Mut,0xFFFF);   	//用于方案数据数组读取
		if (sche_change_tpye[NOW_C][SCHE_CON_TYPE] == SYSTEM_C	&&
				sche_change_tpye[NOW_C][SCHE_CON_WAY] 	== 0x05)
		{
			holding_temp = 1;											//当前系统处于看守中
		}
// // // 		holding_temp = 1;//debug lxb
		os_mut_release(RT_Operate_Mut);
		if (holding_temp)
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
			
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
			/*	查询应答对象值	,查询C1整行		*/
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
						(p_Protocol->opobj_data[5].ID  == 0xBF)	)				//查询优化数据
	{
		os_mut_wait(RT_Operate_Mut,0xFFFF);   	//用于方案数据数组读取
		if (sche_change_tpye[NOW_C][SCHE_CON_TYPE] == SYSTEM_C	&&
				sche_change_tpye[NOW_C][SCHE_CON_WAY] 	== 0x0C)
		{
			optimizing_temp = 1;									//当前系统处于优化中	
		}
// // 		optimizing_temp = 1;//debug lxb
		os_mut_release(RT_Operate_Mut);
		if (optimizing_temp)
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
			
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
	else																											//查询其他数据
	{
		for (i=0; i<p_Protocol->count_target; i++)
		{
			switch (p_Protocol->opobj_data[i].ID)
			{
				case 0xB5:						//查询B5			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB5;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target)); 
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[MANUAL_C][SCHE_NUM]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB6:						//查询B6			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB6;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[SYSTEM_C][SCHE_NUM]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB7:						//查询B7			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB7;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target)); 

						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						p_To_eth->data[p_To_eth->lenth++]	=	sche_change_tpye[NOW_C][SCHE_CON_WAY]; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB8:						//查询B8			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB8;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Period_Time; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xB9:						//查询B9			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xB9;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Phase_Difference; 
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xBE:						//查询BE			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xBE;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
							
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
						
						for(j=0;j<16;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=	sche_optimize.Stage_Time[j]; 
						}
						
						os_mut_release(RT_Operate_Mut);
					}
					break;
				case 0xBF:						//查询BF			
					{
						p_To_eth->data[p_To_eth->lenth++]	=	0xBF;
						p_To_eth->data[p_To_eth->lenth++]	=	(((p_Protocol->opobj_data[i].count_index)<<6) | 
																								(p_Protocol->opobj_data[i].sub_target));  
						
						os_mut_wait(RT_Operate_Mut,0xFFFF);   //方案数据数组读出
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

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	return (__TRUE);
}

