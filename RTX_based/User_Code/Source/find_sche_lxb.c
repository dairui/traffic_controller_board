#include "Include.h"

u8 	search_8D_timeTbId(time_struct *current_time, u16 period_valid,time_struct *pre_stage_time);/*	2012-12.29找时段表号	*/
TIME_TABLE_t search_8e_timeEventID(u8 stage_index,time_struct *current_time);
TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,time_struct *current_time);

/**
  * @brief  根据当前时间，往前搜索8D整表，返回时段号。u8 search_8D_timeTbId(u8 *Read_Time_Data, u16 period_valid,time_struct *pre_stage_time)
  *         具体为：
	*					依次读出8D表					
	* 				单表中采用冒泡法找出等于或是接近当前月的有效月 						                
	* 				找到有效月后先判断是否有周计划，如果有采用冒泡法找出接近当前周x的计划号    
	* 				找到有效月后再判断日是否有效，如果有效采用冒泡法找出接近当前日期的计划号  
	* 				周计划优先   
  * @param  *Read_Time_Data,依次为当前时间星期、日、月
	*					period_valid		当前所有有效的时段号，每位对应一个时段号
	*					pre_stage_time	找到的有效时段号对应的时间结构体，主要用来返回该时段号对应的日期
  * @retval 时段号
	* @invovled function
  */
u8 search_8D_timeTbId(time_struct *current_time, u16 period_valid,time_struct *pre_stage_time)
{
	
	u8					i;
	
	u8 					today_time_stage_index_temp;					//时段表号
	
	/*存储时间的二进制时间的变量*/
	u16					month_temp 					= 	0;
	u32 				day_month_temp 			= 	0;
	u8 					dayWeek_temp 				= 	0;
	
	/*lxb add	用于反冒泡排序8D表临时变量*/
	u8	count_month_temp;								//月的循环变量
	u8  compare_month = 12;							//月冒泡排序的中间变量，存储距离当前月最近的月份
	
	u8	count_week_temp;								//周的循环变量
	u8  compare_week = 7;								//周冒泡排序的中间变量，存储距离当前月最近的月份
	u8	cur_timeTbId_week = 0;					//调度日按周得到的时段表号
	
	u8	count_daymonth_temp;						//月的循环变量
	u8  compare_daymonth = 31;					//月冒泡排序的中间变量，存储距离当前月最近的月份
	u8	cur_timeTbId_month = 0;					//调度日按月得到的时段表号
	
	u8  curstage_month_temp =0;					//存储当前时段号对应的时间临时变量
	u8	curstage_week_temp  =0;					//存储当前时段号对应的时间临时变量
	u8	curstage_day_temp		=0;					//存储当前时段号对应的时间临时变量
	
	u8	daymonth_max;										//存储调度日（按月）时的搜索循环次数
	u8	weekmonth_max;									//存储调度日（按周）时的搜索循环次数
	/*	
	读8D表临时存储变量
	*/
	TIME_SCHEDULE_t	Table_0x8D_Contect	= 	{0};
	u8 *   					p_Char     					=	NULL;
	u32							k										=	0;
	/*	
	数据库操作需要的变量	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;	
	
	/*时间转化为二进制	*/
	month_temp 			= 	1 << 	(current_time->month);
	day_month_temp 	= 	1	<<	(current_time->day);
	dayWeek_temp		= 	1	<<	(current_time->week);
	
	today_time_stage_index_temp	= 	0;
	
	for(i = 40; i >= 1; i--)									//从后往前找
	{
			/*读8D数据库*/
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
		/*读8D数据库结束，存在Table_0x8D_Contect中*/
			
		/*	若当前时段号已确认无效，则舍弃该循环	*/	
		if((period_valid & (u16)(1 << (Table_0x8D_Contect.timeTbId-1)))	== 0)	
		{		
			continue;
		}			
		
		if(	(Table_0x8D_Contect.month>0)	&&
				((Table_0x8D_Contect.dayWeek>0) || (Table_0x8D_Contect.dayMonth>0))	&&
				(Table_0x8D_Contect.timeTbId>0))																	//8d表非空
		{
			for(count_month_temp=0;count_month_temp <= 12;count_month_temp++)		//向前搜索有效月
			{
				/*	
					若搜索的是当前月，则从当日开始搜索，即count_month_temp==0情况；
					若搜索的是不是当前月，即当前月没搜索到，往前搜索到其他月时，从31日或者周日开始搜索（else部分）；
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
					搜索算法：月有效，则搜索调度日（按月），有效，距离当前近，则存储时段号以及该时段号对应的时间日期；
										若无效，则搜索调度日（按周），有效，距离当前近，则存储时段号以及该时段号对应的时间日期；
										直到找到最近时段表。
				*/
				if ((Table_0x8D_Contect.month &	month_temp)	==	month_temp)				//月有效
				{
					/*	
						若为12，表明已经循环12个月找了一圈，重新找到当前月，
						此时为了进入if (count_month_temp <= compare_month)判断，
						则须将count_month_temp = 0;
					*/
					if(count_month_temp == 12)
					{
						count_month_temp = 0;
					}
					
					if (count_month_temp <= compare_month)													//更接近当前月
					{
						compare_month = count_month_temp;
						
						/*	调度日（按月）查找	*/
						if (Table_0x8D_Contect.dayMonth>0)
						{
							for(count_daymonth_temp=0;count_daymonth_temp < daymonth_max;count_daymonth_temp++)	//向前搜索有效周
							{
								if ((Table_0x8D_Contect.dayMonth &	day_month_temp)	==	day_month_temp)	//日有效
								{
									if (count_daymonth_temp < compare_daymonth)														//更接近当前日（按月）
									{
										compare_daymonth = count_daymonth_temp;
										cur_timeTbId_month = Table_0x8D_Contect.timeTbId;										//得到时段号
										
										/*	保存有效方案号的日期，算法如下，不详述	*/
										if (current_time->month >count_month_temp)
										{
											curstage_month_temp = current_time->month - count_month_temp;//保存时段号的月
										}
										else
										{
											curstage_month_temp = 12 + current_time->month - count_month_temp;	//保存时段号的月
										}
										curstage_day_temp   = current_time->day - count_daymonth_temp;	//保存时段号的日
									}	
									count_month_temp =13;			//找到有效月，有效日，为了退出月的for循环，计数赋值最大
									break;										//break掉调度日（按月）循环	，找到了有效调度日（按月）
								}
								day_month_temp = (day_month_temp >> 1);																//移位时间day_month_temp
							}
							
						}
						
						/*	调度日（按周）查找	*/
						else if (Table_0x8D_Contect.dayWeek>0)																			//月有效判断周
						{
							for(count_week_temp=0;count_week_temp < weekmonth_max;count_week_temp++)							//向前搜索有效周
							{
								if ((Table_0x8D_Contect.dayWeek &	dayWeek_temp)	==	dayWeek_temp)				//周有效
								{
									if (count_week_temp < compare_week)																		//更接近当前日（按周）
									{
										compare_week = count_week_temp;
										cur_timeTbId_week = Table_0x8D_Contect.timeTbId;
										/*	保存有效方案号的日期，算法如下，不详述	*/
										if (current_time->month >count_month_temp)
										{
											curstage_month_temp = current_time->month - count_month_temp;				//保存时段号的月
										}
										else
										{
											curstage_month_temp = 12 + current_time->month - count_month_temp;	//保存时段号的月
										}
										curstage_week_temp  = current_time->week - count_daymonth_temp;				//保存时段号的月、星期
									
									}
									count_month_temp =13;			//找到有效月，有效日，为了退出月的for循环，计数赋值最大
									break;										//break掉调度日（按周）循环																															//找到有效调度日（按周）
								}
								dayWeek_temp = (dayWeek_temp >> 1);
							}
							
						}
						else
						{
						}
						
					}
				}
				
				if(month_temp == 0x02)												//若到1月份完毕，则判断12月份
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
	
	/*	若调度日（按月）查找的时段表大于0,且该时段号为今日的时段号，则返回该时段表号，以及该时段号对应的日期；
			否则若调度日（按周）查找的时段表大于0，则返回该时段表号	，以及该时段号对应的日期；
			否则，若调度日（按月）查找的时段表大于0,且该时段号不为今日的时段号，则返回该时段表号，以及该时段号对应的日期；
			否则，降级
	*/
	if ((cur_timeTbId_week > 0) || (cur_timeTbId_month > 0))
	{
		if ((cur_timeTbId_month > 0) && (compare_daymonth == 0))
		{
			today_time_stage_index_temp	=	cur_timeTbId_month;	
			
			pre_stage_time->month = curstage_month_temp;			//保存时段号的月、星期
			pre_stage_time->day   = curstage_day_temp;				//保存时段号的月、星期
		}
		else if (cur_timeTbId_week > 0)
		{
			today_time_stage_index_temp	=	cur_timeTbId_week;	
			
			pre_stage_time->month = curstage_month_temp;			//保存时段号的月、星期
			pre_stage_time->week  = curstage_week_temp;				//保存时段号的月、星期
									
		}
		else
		{
			today_time_stage_index_temp	=	cur_timeTbId_month;	
			
			pre_stage_time->month = curstage_month_temp;			//保存时段号的月、星期
			pre_stage_time->day   = curstage_day_temp;				//保存时段号的月、星期
		}

		return today_time_stage_index_temp;									//返回时段号有效
	}
	else
	{
		return 0;									//没找到，降级！！
	}
	
}



/**
  * @brief  根据当前时间，往前搜索8E整表，得到方案号与控制方式
  *         TIME_TABLE_t search_8e_timeEventID(u8 stage_index,u8 *Read_Time_Data)
  * @param  时段号：stage_index；
						当前时间：Read_Time_Data；
  * @retval TIME_TABLE_t：返回8e表的结构体，包括控制方式、方案号等四项
						TIME_TABLE_t.schemeId==0，表明8e表为空；
						TIME_TABLE_t.schemeId==0xAA，表明8e非空，但搜索不到当前时段，需往前一天搜索
						TIME_TABLE_t.schemeId==1，表明8e非空，搜索到了方案号与控制方式，正常情况
	* @invovled function
  */
TIME_TABLE_t search_8e_timeEventID(u8 stage_index,time_struct *current_time)
{
	/*	
	数据库操作需要的变量	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*	
	读8E表数据库临时变量
	*/
	TIME_TABLE_t		Table_0x8E_read 			= 	{0};
	
	u8 					today_time_stage_index_temp;			//时段标号
	u8 					now_scheme_index_temp;						//当前时段索引

	u8 					Scheme_Num;												//方案号
	u8 					Control_Type;											//控制方式
	
	u8 					i 							= 	0;
	/*	
	8E表中对应的有效事件号数目
	*/
	u8 					index 						= 	0;
	
	/*lxb add	//时分钟冒泡排序的中间变量，存储距离当前时最近的时	分*/
	u16  				compare_time_temp = 24*60+60;							
	/*返回8e表的临时结构体，包括控制方式、方案号等四项*/
	TIME_TABLE_t table_0x8E_return = {0};
	
	today_time_stage_index_temp = stage_index;
	
	/*
	查找该时段表号所有有效的事件号，
	并记录事件开始的
	整点、
	整分、
	控制类型、
	配时方案号	
	*/
	index		= 		0;													//有效地事件号数清零
	now_scheme_index_temp = 0;								//当前时段索引清零
	for(	i = 1;		i <= 48;	i++)
	{	
		//读取8e
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
			Table_0x8E_read.timeId 			= 	data_Target.sub_buf[0];	//时段号
			Table_0x8E_read.eventId 		= 	data_Target.sub_buf[1];	//时段事件号
			Table_0x8E_read.hour 				= 	data_Target.sub_buf[2];	//开始时
			Table_0x8E_read.minute 			= 	data_Target.sub_buf[3];	//开始分
			Table_0x8E_read.ctlMode 		= 	data_Target.sub_buf[4];	//控制方式
			Table_0x8E_read.schemeId 		= 	data_Target.sub_buf[5];	//方案号
			Table_0x8E_read.auOutput 		= 	data_Target.sub_buf[6];
			Table_0x8E_read.spOutput 		= 	data_Target.sub_buf[7];
			
			index++;																//有效事件号数量++
		}	
		//读取8e完毕
		
		/*	
			算法：根据时间进行冒泡排序，找到距离当前最近（即compare_time_temp最小）的方案	
		*/
		if((Table_0x8E_read.timeId > 0) &&
			 (Table_0x8E_read.eventId > 0) &&
			 (Table_0x8E_read.schemeId > 0))
		{																						//若8e表有效
			if((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 	
			<=  (current_time->hour * 60 + current_time->minute ))
			{																					//若当前时间大于读取的整点分的时间
				if(
					((current_time->hour * 60 + current_time->minute)
					-(Table_0x8E_read.hour * 60 + Table_0x8E_read.minute)) 
					< compare_time_temp
					)																			//若当前时间小于存储的时分时间
				{
					compare_time_temp = (current_time->hour * 60 + current_time->minute)
														- (Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) ;					
					now_scheme_index_temp = i;							 //存储找到的事件号索引
					
					Scheme_Num		=	Table_0x8E_read.schemeId;//读取方案号
					Control_Type	= Table_0x8E_read.ctlMode; //存储控制方式
				}
			}
		}
		else
		{
			continue;
		}
		
		
	}
	
	if((index == 0))		 //表明8e为空
	{
		table_0x8E_return.schemeId		= 0;		 //8e表无效
	}
	else
	{
		if (now_scheme_index_temp == 0)				
		/*
			index不为0而now_scheme_index_temp为0，
			表明时段号有效，但当前时间没配方案，应该找到前一天的最后一个时段的方案
		*/
		{
			table_0x8E_return.schemeId	= 0xFF;
		}
		else																		//正常情况，找到合适的方案号与控制方式等
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
  * @brief  根据当前时间，往前搜索8E整表，得到方案号与控制方式
  *         TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,u8 *Read_Time_Data)
  * @param  时段号：stage_index；
						当前时间：Read_Time_Data；
  * @retval TIME_TABLE_t：返回8e表的结构体，包括闹钟的时与分
						TIME_TABLE_t.hour==0，TIME_TABLE_t.minute==0,表明8e表为空；
						否则表明8e非空，搜索到了闹钟的时与分，正常情况
	* @invovled function
  */
TIME_TABLE_t search_8e_timeEventIDNEXT(u8 stage_index,time_struct *current_time)
{
	/*	
	数据库操作需要的变量	
	*/
	u32							read_lenth_temp				=	0;	
	TARGET_DATA 		data_Target;		
	
	/*	
	读8E表数据库临时变量
	*/
	TIME_TABLE_t		Table_0x8E_read 			= 	{0};
	u8 					today_time_stage_index_temp;			//每天只对应一个时段表号jialuyao
	u8 					now_scheme_index_temp;						//当前时段索引

	u8 					cur_hour;												//时中间变量
	u8 					cur_minute;											//分中间变量
	/*	
	原Find_Today_Fangan()变量
	*/
	u8 					i 							= 	0;
	/*	
	8E表中对应的有效事件号数目
	*/
	u8 					index 						= 	0;
	
	/*lxb add	//时分钟冒泡排序的中间变量，存储距离当前时最近的时	分*/
	u16  				compare_time_temp = 24*60+60;							
	
	/*返回8e表的临时结构体，包括控制方式、方案号等四项*/
	TIME_TABLE_t table_0x8E_return = {0};
	
	today_time_stage_index_temp = stage_index;
	
	/*
	查找该时段表号所有有效的事件号，
	并记录事件开始的
	整点、
	整分、
	控制类型、
	配时方案号	
	*/
	index		= 		0;													//有效地事件号数清零
	now_scheme_index_temp = 0;
	for(	i = 1;		i <= 48;	i++)
	{	
		//读取8e
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
			Table_0x8E_read.timeId 			= 	data_Target.sub_buf[0];	//时段号
			Table_0x8E_read.eventId 		= 	data_Target.sub_buf[1];	//时段事件号
			Table_0x8E_read.hour 				= 	data_Target.sub_buf[2];	//开始时
			Table_0x8E_read.minute 			= 	data_Target.sub_buf[3];	//开始分
			Table_0x8E_read.ctlMode 		= 	data_Target.sub_buf[4];	//控制方式
			Table_0x8E_read.schemeId 		= 	data_Target.sub_buf[5];	//方案号
			Table_0x8E_read.auOutput 		= 	data_Target.sub_buf[6];
			Table_0x8E_read.spOutput 		= 	data_Target.sub_buf[7];
			
			index++;
		}	
		//读取8e完毕
		
		/*	
			算法：根据时间进行冒泡排序，往后搜索找到距离当前最近（即compare_time_temp最小）的方案	
		*/
		if((Table_0x8E_read.timeId > 0) &&
			 (Table_0x8E_read.eventId > 0) &&
			 (Table_0x8E_read.schemeId > 0))
		{																						//若8e表有效		
			if((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 	
			> (current_time->hour * 60 + current_time->minute ))
			{																					//若当前时间小于读取的整点分的时间
				if(
					((Table_0x8E_read.hour * 60 + Table_0x8E_read.minute) 
					-(current_time->hour * 60 + current_time->minute)) 
					< compare_time_temp
					)																			//若当前时间小于存储的时分时间
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
	
	if(now_scheme_index_temp == 0)		 //表明当前时间为最后一个时段
	{
		table_0x8E_return.hour			= 0;		 //8e表无效
		table_0x8E_return.minute		= 0;		 //8e表无效
	}
	else
	{
		table_0x8E_return.minute	= cur_minute;
		table_0x8E_return.hour 		= cur_hour;
	}
	return table_0x8E_return;
	
}
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
u8 search_8D_all_timeTbId (time_struct *current_time,TIME_TABLE_t *table_0x8E_return)
{
	u8 							time_stage_index;							//时段表号
	u16 						period_valid	=	0xffff;   		//赋值时段全部有效
	u8 							period_valid_count;  					//时段表计数器
	TIME_TABLE_t 		table_0x8E_str = {0};					//8e表临时结构体，返回搜索8e表得的控制方式等
	time_struct			pre_stage_time = {0};					//时间结构体，往前搜索的存储结构体
	time_struct			orig_time_data = {0};					//存储从时钟芯片读出的初始时间
	
	/*	存储从时钟芯片读出的时间,作为原始时间（current_time可能会在程序执行过程中更改）	*/
		orig_time_data.second = current_time->second;
		orig_time_data.minute = current_time->minute;
		orig_time_data.hour		= current_time->hour;
		orig_time_data.week 	= current_time->week;
		orig_time_data.day		= current_time->day;
		orig_time_data.month  = current_time->month;
		orig_time_data.year   = current_time->year;
	
	for(period_valid_count=1;period_valid_count <= 16;period_valid_count++) // 最大16个时段表
	{
		/*从当前月往前搜索，直到搜索到有效时段号，并保存该时段号的日期到pre_stage_time*/
		time_stage_index = search_8D_timeTbId (current_time,period_valid,&pre_stage_time); 
		if(time_stage_index == 0) 			//无效时段表
		{
			return  (__FALSE);						//降级
		}
		else 								    				//时段表有效
		{
			/*从当前时间往前搜索8e表，返回控制方式和方案号*/	
			table_0x8E_str = search_8e_timeEventID(time_stage_index, current_time);
			
			/*保存控制方式和方案号等*/	
			table_0x8E_return->schemeId = table_0x8E_str.schemeId; 
			table_0x8E_return->ctlMode 	= table_0x8E_str.ctlMode; 
			table_0x8E_return->auOutput = table_0x8E_str.auOutput; 
			table_0x8E_return->spOutput = table_0x8E_str.spOutput; 
			
			/*返回的控制方式和方案号无效，则该时段号无效，屏蔽该时段号，重新循环搜索*/	
			if ((table_0x8E_return->schemeId) == 0)    								//8E表是否有效
			{
					period_valid &= (u16)(~(1 << (time_stage_index-1)));  //8E表无效 ，当前时段表号，位置0继续搜索
			}
			/*返回的控制方式和方案号0xff，则表明时段号有效，
				但该时间段没配方案，需搜索前一天最后时段的方案号和控制方式*/
			else if((table_0x8E_return->schemeId) == 0xFF)		//找寻前一天最后时段的控制方式
			{
				/*	修改写入时间为前一天最后时刻，以读出前一天最后时段	*/
				current_time->hour   = 23;									//时
				current_time->minute = 59;									//分
				current_time->second = 59;									//秒
				if(pre_stage_time.week == 1)
				{
					current_time->week   = 7;									//星期
				}
				else
				{
					current_time->week   = pre_stage_time.week - 1;			//星期
				}
				if(pre_stage_time.day == 1)
				{
					current_time->day = 31;								//日
					//pre_stage_time为该有效时段号对应的日期时间结构体
					if(pre_stage_time.month == 1)
					{
						current_time->month = 12;							//月
					}
					else
					{
						current_time->month = pre_stage_time.month - 1;//月
					}
				}
				else
				{
					current_time->day = pre_stage_time.day - 1;		//日
				}
			}
			
			/*
				返回的控制方式和方案号有效，则往下搜索8e时间作为下一个闹钟
				若没有搜到时间，则返回2，表明下一闹钟写为下一天的0点作为闹钟
			*/
			else  
			{
					table_0x8E_str = search_8e_timeEventIDNEXT(time_stage_index, &orig_time_data);
				
					table_0x8E_return->hour 	= table_0x8E_str.hour; 
					table_0x8E_return->minute = table_0x8E_str.minute;
					return  (__TRUE);    //2个字节高字节为控制方式，低字节为方案号
			}
		}
	}
	return  (__FALSE);						//没有搜到，则降级
}
