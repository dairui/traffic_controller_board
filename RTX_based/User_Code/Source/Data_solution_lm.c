#include "Include.h"

extern u32 DB_Read_M(TARGET_DATA *p_Target);
extern u32 DB_Write_M(TARGET_DATA *p_Target);

/*==============================================================================================
* 语法格式：    void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    以太网查询时间
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
===============================================================================================*/
void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
	u32  time_count=0;

	os_mut_wait(I2c_Mut_lm,0xFFFF);
	time_count = Read_DS3231_Time();//查询当前时间，4字节秒数  
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
* 语法格式：    void eth_set_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    以太网设置时间
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
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
	u8 		Write_Time_Data[7] 					={0};  /*  写时间缓存  */



	if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))
	{
		
			timecount = p_Target->sub_buf[0];//接收时间---转化成4字节秒数
			timecount <<= 8;
			timecount |= p_Target->sub_buf[1];
			timecount <<= 8;
			timecount |= p_Target->sub_buf[2];
			timecount <<= 8;
			timecount |= p_Target->sub_buf[3];
			
			temp = timecount / 86400;  //得到天数(秒钟数对应的)
			if(daycnt != temp)//超过一天了
			{   
				daycnt = temp;
				temp1 = 1970;   //从1970年开始
				while(temp >= 365)
				{
					if(Is_Leap_Year(temp1))  //是闰年
					{
						if(temp >= 366)
						{
							temp -= 366;  //闰年的秒钟数
						}
						else 
						{
							temp1++;
							break;
						}
					} 
					else
					{
						temp -= 365; //平年
					}
					temp1++;
				}
				Write_Time_Data[6] = temp1 - 1970;  //得到年份
				temp1 = 0;
				while(temp >= 28) //超过了一个月
				{
					if(Is_Leap_Year(Write_Time_Data[6] + 1970) && (temp1 == 1))//当年是不是闰年/2月份
					{
						if(temp >= 29)
							temp -= 29;  //闰年的秒钟数
						else 
							break;
					}
					else
					{
						if(temp >= mon_table[temp1])
							temp -= mon_table[temp1];  //平年
						else 
							break;
					}
					temp1++;
				}
				Write_Time_Data[5] = temp1 + 1;  //得到月份
				Write_Time_Data[4] = temp + 1;    //得到日期
			}
			temp = timecount % 86400; //得到秒钟数
			Write_Time_Data[2] = temp / 3600; //小时
			Write_Time_Data[1] = (temp % 3600) / 60; //分钟
			Write_Time_Data[0] = (temp % 3600) % 60; //秒钟
			Write_Time_Data[3] = RTC_Get_Week((u16)(Write_Time_Data[6] + 1970),Write_Time_Data[5],Write_Time_Data[4]);//获取星期
			Write_Time_Data[0] = Write_Time_Data[0] / 10 *16 + Write_Time_Data[0] %10;
			Write_Time_Data[1] = Write_Time_Data[1] / 10 *16 + Write_Time_Data[1] %10;
			Write_Time_Data[2] = Write_Time_Data[2] / 10 *16 + Write_Time_Data[2] %10;
			Write_Time_Data[3] = Write_Time_Data[3] / 10 *16 + Write_Time_Data[3] %10;
			Write_Time_Data[4] = Write_Time_Data[4] / 10 *16 + Write_Time_Data[4] %10;
			Write_Time_Data[5] = Write_Time_Data[5] / 10 *16 + Write_Time_Data[5] %10;
                        
			if(Write_Time_Data[6] < 30)//2000年之前的日期不能设置，错误应答
			{
				p_To_eth->error										=__TRUE;
				p_To_eth->error_type 							=ERROR_OTHER;   
		 
			}
			else
			{
				Write_Time_Data[6] = (Write_Time_Data[6] - 30) / 10 *16 + Write_Time_Data[6] %10;//2000年后
			
				os_mut_wait(I2c_Mut_lm,0xFFFF);   //  I2C_lm
				I2C_Write_DS3231(Write_Time_Data);  //  I2C_lm   有待改动的		
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
// 	u8 		write_time_data[7] 					={0};  /*  写时间缓存  */


// 	u32 time_set_day = 0;//设置时间,天
// 	u32 time_set_sec = 0;//设置时间，秒
// 	u32 time_init = 0;//设置秒数基准时间

// 	if((p_Target->ID  == 0x86) || (p_Target->ID == 0x88))
// 	{
// 		time_count=((u32)(p_Target->sub_buf[0]<<24))|	//接收时间---转化成4字节秒数
// 							 ((u32)(p_Target->sub_buf[1]<<16))|
// 							 ((u32)(p_Target->sub_buf[2]<<8))	|
// 							 ((u32) p_Target->sub_buf[3]);	

// 		time_set_day = time_count / 86400;  //得到天数(秒钟数对应的)
// 		if(time_set_day > 0)//超过一天了
// 		{   
// 			time_init = 1970;   //从1970年开始
// 			while(time_set_day >= 365)
// 			{
// 				if(Is_Leap_Year(time_init))  //是闰年
// 				{
// 					if(time_set_day >= 366)
// 					{
// 						time_set_day -= 366;  //闰年的天数
// 					}
// 					else 
// 					{
// 						time_init++;
// 						break;
// 					}
// 				} 
// 				else
// 				{
// 					time_set_day -= 365; //平年
// 				}
// 				time_init++;
// 			}
// 			//========
// 			write_time_data[6] = time_init - 1970;  //得到年份

// 			time_init = 0;
// 			while(time_set_day >= 28) //超过了一个月
// 			{
// 				if(Is_Leap_Year(write_time_data[6] + 1970) && (time_init == 1))//当年是不是闰年/2月份
// 				{
// 					if(time_set_day >= 29)
// 						time_set_day -= 29;  //月天数
// 					else 
// 						break;
// 				}
// 				else
// 				{
// 					if(time_set_day >= mon_table[time_init])
// 						time_set_day -= mon_table[time_init];  //平年
// 					else 
// 						break;
// 				}
// 				time_init++;
// 			}
// 			//========
// 			write_time_data[5] = time_init + 1;  //得到月份
// 			write_time_data[4] = time_set_day + 1;    //得到日期
// 		}

// 		time_set_sec = time_count % 86400; //得到一天内的秒钟数
// 		write_time_data[0] = (time_set_day % 3600) % 60; //秒钟
// 		write_time_data[1] = (time_set_day % 3600) / 60; //分钟
// 		write_time_data[2] = time_set_day / 3600; //小时
// 		write_time_data[3] = RTC_Get_Week((u16)(write_time_data[6] + 2000),write_time_data[5],write_time_data[4]);//获取星期
// 		//进制转换
// 		write_time_data[0] = write_time_data[0] / 10 *16 + write_time_data[0] %10;
// 		write_time_data[1] = write_time_data[1] / 10 *16 + write_time_data[1] %10;
// 		write_time_data[2] = write_time_data[2] / 10 *16 + write_time_data[2] %10;
// 		write_time_data[3] = write_time_data[3] / 10 *16 + write_time_data[3] %10;
// 		write_time_data[4] = write_time_data[4] / 10 *16 + write_time_data[4] %10;
// 		write_time_data[5] = write_time_data[5] / 10 *16 + write_time_data[5] %10;
// 	//---------------------
// 		if(write_time_data[6] < 30)//2000年之前的日期不能设置，错误应答	
// 		{
// 			p_To_eth->error										=__TRUE;
// 			p_To_eth->error_type 							=ERROR_OTHER;   	
// 		}
// 		else   //正确的，需要设置
// 		{
// 			write_time_data[6] = (write_time_data[6] - 30) / 10 *16 + write_time_data[6] %10;//2000年后
// 		
// 			os_mut_wait(I2c_Mut_lm,0xFFFF);   //  I2C_lm
// 			I2C_Write_DS3231(write_time_data);  //  I2C_lm   有待改动的		
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
* 语法格式：    void eth_read_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    以太网读取数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
===============================================================================================*/

OS_SEM						src_Db_update_task;   //  用于读写数据库   //debug_lm
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
	

	

 	if(lenth !=0)  //没出错
 	{
		p_To_eth->data[p_To_eth->lenth++]	= ID_temp;
		p_To_eth->data[p_To_eth->lenth++]	= (((count_index_temp)<<6) | (sub_target_temp) );

		
		
		os_mut_wait(Spi_Mut_lm,0xFFFF);
// // // 		read_error_or_true	=DB_Read_Real_Line(p_Target) ;
		
// // // 		if((read_error_or_true != 0) &&	(count_index_temp == 0))//返回行数，整表查询时，该位置写入行数
// // // 		{
// // // 			p_To_eth->data[p_To_eth->lenth++]	= DB_Read_Real_Line(p_Target);
// // // 		}
		os_mut_release(Spi_Mut_lm);	
		
		
		
// // // 		if((Index1_max_temp != 0) && (Index2_max_temp != 0) && (count_index_temp == 0))
// // // 		{ //双索引整表查询，该位置写入索引2的最大值
// // // 			p_To_eth->data[p_To_eth->lenth++]	= Index2_max_temp;
// // // 		}
		
		
		if(count_index_temp 			==0)				// 单对象、整表(单索引,双索引)
		{
				/*
				由于所查的表中"单对象、整表"长已经包含了"对象标识"所占用一个字节数,
				以及"索引个数+子对象号"所占用一个字节数
				因此,此处要减去 2.
				*/
// // 				line_temp=Object_table[ID_temp-0x81].Line_Number; 		//得到当前ID对应的最大行数 (line_max)
// // 				tx_data_lenth=(Object_table[ID_temp-0x81].Object_Number - BAOLIU*line_temp -2-1); //所有的参数的长度
// // 				for(j=0;j<tx_data_lenth;j++)
// // 				{
// // 					p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
// // 				}			


				for(j=0;j<lenth;j++)
				{
					p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
				}		

				
		}
		else if((count_index_temp 	==1)&&(Index1_max_temp != 0)&&(Index2_max_temp == 0))  			// 单索引
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
			
				if(sub_target_temp ==0)  	//整行
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//每行的长度
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  单个子对象
				{	 
						tx_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//单个子对象的大小
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}			
				}
			
		}
		else if((count_index_temp ==2)&&(Index1_max_temp != 0)&&(Index2_max_temp != 0)
			&&(Index1_temp != 0)&&(Index2_temp == 0))  			// 双索引的单索引方式
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
// // 				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];
			  p_To_eth->data[p_To_eth->lenth++]	=lenth / Object_table[ID_temp-0X81].Line_leng; // // abcdefg //
				if(sub_target_temp ==0)  	//整行
				{
						tx_data_lenth=lenth;//(Object_table[ID_temp-0x81].Line_Byte_Max * Index2_max_temp);			//每行的长度 // abcdefg //
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  单个子对象
				{	 
							p_To_eth->error										=__TRUE;
							p_To_eth->error_type 							=ERROR_OTHER;  
	
				}
			
		}
		else if((count_index_temp 	==2)&&(Index1_max_temp != 0)
			&&(Index2_max_temp != 0)&&(Index1_temp != 0)&&(Index2_temp != 0))   			// 双索引
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];
				if(sub_target_temp ==0)  	//整行
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//每行的长度
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  单个子对象
				{	 
						tx_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//单个子对象的大小
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}			
				}
		}
		else																	// 三索引表
		{
			/*  三索引表，保留  */
			p_To_eth->error										=__TRUE;
			p_To_eth->error_type 							=ERROR_OTHER;  
		}
			

		
		
		
		
		/*   单对象、单索引、双索引的回复不一样 */
	}
	else 														//出错了
	{
		p_To_eth->error										=__TRUE;
		p_To_eth->error_type 							=ERROR_OTHER;   		
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
/*==============================================================================================
* 语法格式：    void eth_read_line_only_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    以太网读取数据库,只支持单索引的 整行  读取操作
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
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

	
	
	if(lenth  !=0)  //没出错
	{
		p_To_eth->data[p_To_eth->lenth++]	=ID_temp;
		p_To_eth->data[p_To_eth->lenth++]	=(((count_index_temp)<<6) | (sub_target_temp) );   			

		
		if(count_index_temp 	==1)  			// 单索引
		{
				p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];
			
				if(sub_target_temp ==0)  	//整行
				{
						tx_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//每行的长度
						for(j=0;j<tx_data_lenth;j++)
						{
							p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_buf[j];
						}				
				}
				else  				 						//  单个子对象
				{	 
					p_To_eth->error										=__TRUE;
					p_To_eth->error_type 							=ERROR_OTHER; /*  三索引表，保留  */			
				}
		}	
		else																	// 三索引表
		{
			p_To_eth->error										=__TRUE;
			p_To_eth->error_type 							=ERROR_OTHER; /*  三索引表，保留  */
		}	
		/*   单对象、单索引、双索引的回复不一样 */
	}
		else 														//出错了
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
// 												+ p_Target->sub_index[1];	//行号
// 									DB_Read_M(p_Target);
// 									//添加发送  否则会覆盖掉
// 								}
								



/*==============================================================================================
* 语法格式：    void eth_set_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
* 实现功能：    以太网设置数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 参数2：       to_eth_struct *	p_To_eth	发往ETH的数据指针
* 返回值：      无
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

	
	if(db_error_or_right != 0 ) //没出错
	{
		if(count_index_temp 			==0)				// 单对象、整表(单索引,双索引)
		{

			p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
			p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																					(p_Target->sub_target) );   	
		}
		else if(count_index_temp 	==1)  			// 单索引
		{
				if(sub_target_temp ==0)  	//整行
				{		
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];		

				}
				else  				 						//  单个子对象
				{	 
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
				}
			
		}
		else if(count_index_temp 	==2)  			// 双索引
		{
				if(sub_target_temp ==0)  	//整行
				{
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];	
					
				}
				else  				 						//  单个子对象
				{	 
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->ID;
						p_To_eth->data[p_To_eth->lenth++]	=(	((p_Target->count_index)<<6) | 
																								(p_Target->sub_target) );
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[0];	
						p_To_eth->data[p_To_eth->lenth++]	=p_Target->sub_index[1];	
				}
		}
		else																	// 三索引表
		{
			/*  三索引表，保留  */
		}
			

		
		
		
		
		/*   单对象、单索引、双索引的回复不一样 */
	}
	else 														//出错了
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

	

// 	IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

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
		/*  发送的信息的条数大于最大值(now =1000)  */
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
	有待进一步处理
	else if(len_overflow_temp	==__TRUE)     
	{
		len_overflow_temp			=__FALSE;
	}
	*/
	tsk_unlock (); 			//  enable  task switching 	


// 	IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	if(buff_overflow_temp	==__TRUE) 
		return (1);
	else 
		return (0);
}








