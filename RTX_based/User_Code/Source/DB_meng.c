#include "Include.h"

#define TRUE		1
#define FALSE	  0

#define DB_RIGHT __TRUE
#define DB_ERROR __FALSE

/*==============================================================================================
* 语法格式：    void DB_Read_M(TARGET_DATA *p_Target)
* 实现功能：    读数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 返回值：      所读到的字节数,如果错误,返回__FALSE(=0)
===============================================================================================*/
u32 DB_Read_M(TARGET_DATA * p_Taget_set)
{
	u8  ID_temp = 0;
	u32 read_count  = 0;
	u32 Flag_addr_temp = 0;
	u32 Data_addr_temp = 0;
	u8 Tab_OK_flag_temp = 0;
	u32 line_bytes_temp = 0;
	u32 line_OK_count =0;
	u32 line_num_temp = 0;
	u32 i = 0;
	u32 j = 0;
	u16 ROAD_MAP_bytes_temp = 0;
	
	
	ID_temp = p_Taget_set->ID;
	
	if(ID_temp == ROAD_MAP_ID)   ////标识 ROAD_MAP_ID 暂用0X70 //mm20140509)
	{
		if((p_Taget_set->sub_target   == 0) &&
			 (p_Taget_set->count_index  == 0) &&
			 (p_Taget_set->sub_index[0] == 0) &&
			 (p_Taget_set->sub_index[1] == 0)	)  //错误检验 0x70 只有单对象一种情况正确 mm20140509)
			{
				Data_addr_temp	= TAB_ROAD_MAP_ID_ADDR;
				for(i=0; i<2; i++)
				{
					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
				}
				ROAD_MAP_bytes_temp = ( ((0x00FF & (p_Taget_set->sub_buf[0]))<<8) 
																| (p_Taget_set->sub_buf[1]) );
					//标识 ROAD_MAP_ID 字节数 高位在前，低位在后  
				for(i=0; i<ROAD_MAP_bytes_temp; i++)
				{
					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
				}
			}
	}
	else // mm20140509)
	{
		Flag_addr_temp 		= Object_table[ID_temp-0x81].Flag_Addr;     //标识地址	
		Data_addr_temp		= Object_table[ID_temp-0x81].Data_Addr;     //数据地址	
		
		Tab_OK_flag_temp  = Rd_u8(Flag_addr_temp); 	//整表有效标志	
		if(p_Taget_set->sub_target == 0)
		{
			if(Tab_OK_flag_temp == TAB_OK_FLAG) //表有效
			{	
				line_bytes_temp  = Object_table[ID_temp-0x81].Line_leng; //每行字节数
	//----------------- 整表(单索引,双索引)   --------------------------
				if((p_Taget_set->count_index == 0) && (p_Taget_set->sub_index[0] == 0) 
						&& (p_Taget_set->sub_index[1] == 0))//整表
				{
					if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //单索引
					{
						p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //索引一有效个数  行数
					}
					else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0)) //双索引
					{
						return(0);
					}
	// // 				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引
	// // 				{
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //索引一有效个数  行数
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //索引二有效个数
	// // 				}
	// // 				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引
	// // 				{
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //索引一有效个数  行数
	// // 					Data_addr_temp++;
	// // 				}
					
					line_num_temp 	 = Object_table[ID_temp-0x81].Line_Number;

					for(j=0; j<line_num_temp; j++)
					{
						if(Rd_u8(++Flag_addr_temp) == LINE_OK_FLAG) //行有效
						{
							line_OK_count++;
							for(i=0; i<line_bytes_temp; i++)
							{
								p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
							}
						}
						else
						{
							Data_addr_temp += Object_table[ID_temp-0x81].Line_leng; 
						}
					}
				}
	//----------------- 单索引表 索引一方式 查询 -------------------------------------------------
				else if((p_Taget_set->count_index == 1)																														//一个索引
								 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//索引一有效
								 &&(Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0) )//单索引表         索引一方式		
				{
					
					Flag_addr_temp += p_Taget_set->sub_index[0];
					Data_addr_temp += 1;
					Data_addr_temp += (p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0x81].Line_leng;
					
					if(Rd_u8(Flag_addr_temp) == LINE_OK_FLAG)
					{
							for(i=0; i<line_bytes_temp; i++)
							{
								p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
							}
					}
					else
					{
						Data_addr_temp += Object_table[ID_temp-0x81].Line_leng; 
					}
					
				}
	//----------------- 双索引表 索引一方式 查询 -------------------------------------------------
				else if((p_Taget_set->count_index == 2)																													//两个索引
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//索引一索引二有效
							 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0))//双索引表          索引一方式
				{
					Flag_addr_temp += (p_Taget_set->sub_index[0]-1 )* Object_table[ID_temp-0X81].Index2 + 1;// // abcdefg //
					Data_addr_temp += 2;
					if( ID_temp == 0x92)
					{
						Data_addr_temp -= 1;
					}
					Data_addr_temp += (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0x81].Line_leng)*(Object_table[ID_temp-0X81].Index2);
					for(j=0; j<Object_table[ID_temp-0X81].Index2; j++)
					{
						if(Rd_u8(Flag_addr_temp++) == LINE_OK_FLAG)
						{
								for(i=0; i<line_bytes_temp ; i++)
								{
									p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
								}
						}
						else
						{
							Data_addr_temp += line_bytes_temp;
						}
					}
				
				}
	//----------------- 双索引表 索引二方式 （双索引方式）查询 -------------------------------------------------
				else if((p_Taget_set->count_index == 2)																														//两个索引
								 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//索引一索引二有效
								 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0))//双索引表          双索引方式
				{	
					Flag_addr_temp += (p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1];
					Data_addr_temp += 2;
					if( ID_temp == 0x92)
					{
						Data_addr_temp -= 1;
					}
					Data_addr_temp += ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1] - 1 ) *Object_table[ID_temp-0x81].Line_leng;
					if(Rd_u8(Flag_addr_temp) == LINE_OK_FLAG)
					{				
							for(i=0; i<line_bytes_temp; i++)
							{
								p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
							}	
					}						
				}
				
				
			}
		}
	}
	return(read_count);
}





/*==============================================================================================
* 语法格式：    void DB_Write(TARGET_DATA *p_Target)
* 实现功能：    写数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 返回值：      无
===============================================================================================*/
u32 DB_Write_M(TARGET_DATA * p_Taget_set)
{
	u8  ID_temp =0;
	u8  Index1_lines_count = 0;
	u8  Index2_lines_count = 0;
	u8  Index2_lines_count_temp = 0;
	u32 Data_leng_temp;
	u32 Data_addr_temp;
	u32 Data_addr_old;
	u32 Flag_addr_temp;
	u32 Flag_addr_old;
	u32 Line_Number_temp;
	u32 i=0;
	u32 j=0;
	u32	write_count=0;
	u16 ROAD_MAP_bytes_temp = 0;   ////标识 ROAD_MAP_ID 数据字节数 暂用0X70 mm20140509)
	
	ID_temp = p_Taget_set->ID;
	
	if(ID_temp == ROAD_MAP_ID)   ////标识 ROAD_MAP_ID 暂用0X70 //mm20140509)	
	{
		if((p_Taget_set->sub_target   == 0) &&
			 (p_Taget_set->count_index  == 0) &&
			 (p_Taget_set->sub_index[0] == 0) &&
			 (p_Taget_set->sub_index[1] == 0)	) //错误检验 0x70 只有单对象一种情况正确 mm20140509)
			{
				Data_addr_temp	          = TAB_ROAD_MAP_ID_ADDR;
		// 		p_Taget_set->sub_target   = 0;
		// 		p_Taget_set->count_index  = 0;
		// 		p_Taget_set->sub_index[0] = 0;
		// 		p_Taget_set->sub_index[1] = 0;
				ROAD_MAP_bytes_temp = ( ((0x00FF & (p_Taget_set->sub_buf[0]))<<8) 
																| (p_Taget_set->sub_buf[1]) ); 
							//标识 ROAD_MAP_ID 数据字节数 高位在前，低位在后  
				ROAD_MAP_bytes_temp += 2; ////标识 ROAD_MAP_ID 总字节数= 数据字节数 +2，用原变量加二实现
		//	for(i=0; i<2; i++)  
		//	{Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;}	
				for(i=0; i<ROAD_MAP_bytes_temp; i++)
				{
					Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
				}
			}
			else return (0);
	}
else  //mm20140509)
{ 	
		//计算数据长度
		if(p_Taget_set->sub_target == 0) //设置时子对象为0，只支持整行或整表
		{
			Data_addr_old 		= Object_table[ID_temp-0x81].Data_Addr;     //数据地址
			Data_addr_temp	 	=	Data_addr_old	;
			Flag_addr_old			= Object_table[ID_temp-0x81].Flag_Addr;     //标识地址
			Flag_addr_temp 		= Flag_addr_old ;
			Line_Number_temp 	= Object_table[ID_temp-0X81].Line_Number; //支持行数
	//----------------- 整表设置 -------------------------------------------------
			if((p_Taget_set->count_index == 0)
					&&(p_Taget_set->sub_index[0] == 0)
					&&(p_Taget_set->sub_index[1] == 0)
					&&(Object_table[ID_temp-0X81].Index2 == 0))//整表 //双索引不支持整表设置
			{
				
				if((Object_table[ID_temp-0X81].Index1 == 0)  && (Object_table[ID_temp-0X81].Index2 == 0))      //单对象
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //单索引
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 1;
					Data_addr_temp += 1;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 2;
					Data_addr_temp += 2;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //双索引0x92表
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 1;
					Data_addr_temp += 1;
				}


				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;		//写整表标识，索引标识
				for(i=0; i<Line_Number_temp; i++)
				{
					Wr_u8(Flag_addr_temp++,LINE_OK_FLAG) ;	//写整行标识，索引标识
				}
				
				for(i=0;i<Data_leng_temp;i++)						 //写整表操作前 全清零
				{
					Wr_u8(Data_addr_temp++,0) ;	
				}	
				Data_addr_temp	=		Data_addr_old;	

				
				if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //单索引
				{
					Data_addr_temp ++;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引
				{
					Data_addr_temp ++;
					Data_addr_temp ++;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //双索引0x92表
				{
					Data_addr_temp ++;
				}
				
				
				for(i=0;i<Data_leng_temp;i++)						//写整表数据
				{
					Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
				}
				//========= line ================
				if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //单索引
				{
					Wr_u8(Data_addr_old , Object_table[ID_temp-0X81].Index1 );
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引
				{
					Wr_u8(Data_addr_old,  Object_table[ID_temp-0X81].Index1);
					Wr_u8((Data_addr_old + 1), Object_table[ID_temp-0X81].Index2 );
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //双索引0x92表
				{
					Wr_u8(Data_addr_old,  Object_table[ID_temp-0X81].Index1);
				}
				//===============================
				
				
			}
	//----------------- 单索引表 索引一方式 设置 -------------------------------------------------
			else if((p_Taget_set->count_index == 1)																														//一个索引
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//索引一有效
							 &&(Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0) )//单索引表         索引一方式
			{
				Data_leng_temp = Object_table[ID_temp-0X81].Line_leng;
				Data_addr_old = Object_table[ID_temp-0x81].Data_Addr; //+ (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng);
																								//跳过前面N-1行的地址，N为索引1的当前要写入的值
				Data_addr_temp = Data_addr_old + (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng);
				Data_addr_temp += 1;
				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//写整表标识，索引标识
				for(i=1; i<p_Taget_set->sub_index[0]; i++)
				{
					Flag_addr_temp++;
				}
				Wr_u8(Flag_addr_temp,LINE_OK_FLAG) ;		//写整行标识，索引标识
				

				for(i=0;i<Data_leng_temp;i++)						//写整行操作前 全清零
				{
					Wr_u8(Data_addr_temp++,0) ;	
				}	
				
				Data_addr_temp	=		Data_addr_old + (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng) + 1;
				for(i=0;i<Data_leng_temp;i++)						//写整行数据
				{
					Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
				}
				//=========line=====================
				Index1_lines_count = 0;
				for(i=0 ; i< Object_table[ID_temp-0X81].Index1 ; i++)
					{
						if(Rd_u8(Flag_addr_old + 1 + i) == LINE_OK_FLAG)
							Index1_lines_count++;
					}
				Wr_u8(Data_addr_old , Index1_lines_count );  //写索引一有效行数
				//==================================
				
			}
			
	//----------------- 双索引表 索引一方式 设置 -------------------------------------------------
			else if((p_Taget_set->count_index == 2)																														//两个索引
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//索引一索引二有效
							 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0) )//双索引表          索引一方式
			{			
	// //			Data_leng_temp = Object_table[ID_temp-0X81].Line_leng * Object_table[ID_temp-0X81].Index2;
				Data_leng_temp = Object_table[ID_temp-0X81].Line_leng * p_Taget_set->sub_index[1]; //abcdefg
	// // 			Data_addr_old  = Object_table[ID_temp-0x81].Data_Addr 
	// // 											 + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1] - 1)*(Object_table[ID_temp-0X81].Line_leng);
	// // 				//跳过前面{(N-1)*index2_max +index[2]-1}行的地址，N为索引1的当前要写入的值，M为索引2的当前要写入的值
				
				Data_addr_old  = Object_table[ID_temp-0x81].Data_Addr ;
				
				Data_addr_temp = Data_addr_old + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 )*(Object_table[ID_temp-0X81].Line_leng);//abcdefg20130730;
				if(ID_temp != 0x92)
				{
					Data_addr_temp += 2;
				}
				else
				{
					Data_addr_temp += 1;
				}
				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//写整表标识，索引标识
				for(i=0; i<((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2); i++)//1-->0 abcdefg
				{
					Flag_addr_temp++;
				}
	// // 			for(i=0; i<Object_table[ID_temp-0X81].Index2; i++)
				for(i=0; i<p_Taget_set->sub_index[1]; i++)
				{
					Wr_u8(Flag_addr_temp++,LINE_OK_FLAG) ;		//写整行标识，索引标识	
				}
				
				for(i=0;i<Data_leng_temp;i++)						//写整行操作前 全清零
				{
					Wr_u8(Data_addr_temp++,0) ;	
				}
				
				Data_addr_temp	=	Data_addr_old + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 )*(Object_table[ID_temp-0X81].Line_leng);//abcdefg20130730;
				if(ID_temp != 0x92)
				{
					Data_addr_temp ++;
					Data_addr_temp ++;
				}
				else
				{
					Data_addr_temp ++;
				}
				
				
				for(i=0;i<Data_leng_temp;i++)						//写整行数据
				{
					Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
				}
				//============line=========================
				Index1_lines_count = 0;
				Index2_lines_count = 0;
				
				for(j=0 ; j< Object_table[ID_temp-0X81].Index1 ; j++)
				{
					for(i=0 ; i< Object_table[ID_temp-0X81].Index2 ; i++)
					{
						if(Rd_u8(Flag_addr_old + 1 + i + j*(Object_table[ID_temp-0X81].Index2)) == LINE_OK_FLAG)
						{
							Index2_lines_count_temp++;
						}
					}
					if(Index2_lines_count_temp != 0)
					{
						Index1_lines_count++;
					}
					if(Index2_lines_count < Index2_lines_count_temp)
					{
						Index2_lines_count = Index2_lines_count_temp;
					}
					Index2_lines_count_temp = 0;
				}
				Wr_u8(Data_addr_old , Index1_lines_count );					//写索引一有效行数
				if(ID_temp != 0x92)
				{
					Wr_u8((Data_addr_old + 1), Index2_lines_count );		//写索引二有效行数最大值
				}
				//=========================================
				
			}
			
	// // // //----------------- 双索引表 索引二方式 （双索引方式）设置 -------------------------------------------------
	// // // 		else if((p_Taget_set->count_index == 2)																														//两个索引
	// // // 						 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//索引一索引二有效
	// // // 						 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0) )//双索引表          双索引方式
	// // // 		{			
	// // // 			Data_leng_temp = Object_table[ID_temp-0X81].Line_leng;
	// // // 			Data_addr_old = Object_table[ID_temp-0x81].Data_Addr 
	// // // 											 + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1] - 1)*(Object_table[ID_temp-0X81].Line_leng);
	// // // 				//跳过前面{(N-1)*index2_max +index[2]-1}行的地址，N为索引1的当前要写入的值，M为索引2的当前要写入的值
	// // // 			Data_addr_temp =	Data_addr_old	;
	// // // 			if(ID_temp != 0x92)
	// // // 			{
	// // // 				Data_addr_temp += 2;
	// // // 			}
	// // // 			else
	// // // 			{
	// // // 				Data_addr_temp += 1;
	// // // 			}
	// // // 			
	// // // 			Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//写整表标识，索引标识
	// // // 			for(i=1; i<(((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2)+p_Taget_set->sub_index[1]); i++)
	// // // 			{
	// // // 				Flag_addr_temp++;
	// // // 			}
	// // // 			Wr_u8(Flag_addr_temp,LINE_OK_FLAG) ;		//写整行标识，索引标识
	// // // 			
	// // // 			
	// // // 			for(i=0;i<Data_leng_temp;i++)						//写整行操作前 全清零
	// // // 			{
	// // // 				Wr_u8(Data_addr_temp++,0) ;	
	// // // 			}
	// // // 			Data_addr_temp	=		Data_addr_old;						//写整表标识，索引标识
	// // // 			
	// // // 			if(ID_temp != 0x92)
	// // // 			{
	// // // 				Data_addr_temp ++;
	// // // 				Data_addr_temp ++;
	// // // 			}
	// // // 			else
	// // // 			{
	// // // 				Data_addr_temp ++;
	// // // 			}
	// // // 			
	// // // 			for(i=0;i<Data_leng_temp;i++)						//写整行数据
	// // // 			{
	// // // 				Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
	// // // 			}
	// // // 			//==========line================================
	// // // 			Index1_lines_count = 0;
	// // // 			Index2_lines_count = 0;
	// // // 			for(j=0 ; j< Object_table[ID_temp-0X81].Index1 ; j++)
	// // // 			{
	// // // 			  for(i=0 ; i< Object_table[ID_temp-0X81].Index2 ; i++)
	// // // 				{
	// // // 					if(Rd_u8(Flag_addr_old + 1 + i + j*(Object_table[ID_temp-0X81].Index2)) == LINE_OK_FLAG)
	// // // 					{
	// // // 						Index2_lines_count_temp++;
	// // // 					}
	// // // 				}
	// // // 				if(Index2_lines_count_temp != 0)
	// // // 				{
	// // // 					Index1_lines_count++;
	// // // 				}
	// // // 				if(Index2_lines_count < Index2_lines_count_temp)
	// // // 				{
	// // // 					Index2_lines_count = Index2_lines_count_temp;					
	// // // 				}
	// // // 				Index2_lines_count_temp = 0;
	// // // 			}
	// // // 			Wr_u8(Data_addr_old , Index1_lines_count );					//写索引一有效行数
	// // // 			if(ID_temp != 0x92)
	// // // 			{
	// // // 				Wr_u8((Data_addr_old + 1), Index2_lines_count );		//写索引二有效行数最大值
	// // // 			}
	// // // 			//==============================================
	// // // 		}
			
		}
	}
	return (write_count);

}






/*==============================================================================================
* 语法格式：    void DB_Read_Analysis(TARGET_DATA *p_Target)
* 实现功能：    读数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 返回值：      所读到的字节数,如果错误,返回__FALSE(=0)
===============================================================================================*/
// // // u32 DB_Read_Analysis(TARGET_DATA *p_Target)
// // // {
// // // 	u32 							i										= 0;
// // // 	u32 							j										= 0;
// // // 	u32 							n										= 0;
// // // 	u8 								Return_Flag 				= __TRUE;
// // // 	
// // // 	u32 	            address_temp 				= 0;
// // // 	u32 	            old_address 				= 0;
// // // 	u32								No_line_temp				= 0;	//第几行
// // // 	u32								line_num_temp				= 0;	//总行数
// // // 	u32								line_num_real_temp	= 0;	//总行数	
// // // 	u8								line_bytes_temp			= 0;	//每行字节数
// // // 	u8	 							subobj_bytes_temp   = 0;	//子对象字节数
// // // 	
// // // 	u8 								ID_temp 						= 0;
// // // 	u8 								count_index_temp		= 0;
// // // 	u8 								sub_target_temp			= 0;
// // // 	u8 								Index1_max_temp			= 0;
// // // 	u8 								Index2_max_temp			= 0;
// // // 	
// // // 	
// // // 	u8 								table_OK_flag_temp  = 0;	//表有效标志
// // // 	u8 								line_num_flag_temp  = 0;	//实际行数标志
// // // 	
// // // 	u8 								line_OK_flag_temp   = 0;	//行有效标志

// // // 	u32								read_count=0;
// // // 	
// // // 	
// // // 	static u32								addr_read_count=0;   //   debug_lm	
// // // 	static u32								line_read_count=0;   //   debug_lm	
// // // 	static u32								no_line_read_count=0;   //   debug_lm	
// // // 	
// // // 	ID_temp 						= p_Target->ID;
// // // 	count_index_temp		= p_Target->count_index;
// // // 	sub_target_temp			= p_Target->sub_target;
// // // 	
// // // 	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
// // // 	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;
// // // 	//line_num_temp    		= p_Target->sub_index[0];
// // // 	address_temp		 		= Object_table[ID_temp-0X81].Object_Addr;	//获得地址
// // // 	old_address					=	address_temp;
// // // 	addr_read_count  		=	address_temp;//   debug_lm	
// // // 	line_read_count  		=	0;//   debug_lm	
// // // 	no_line_read_count  =	0;//   debug_lm	
// // // 	
// // // 	
// // // 	table_OK_flag_temp  = Rd_u8(address_temp++); 	//整表有效标志				
// // // 	address_temp++;																//保留 
// // // 	line_num_flag_temp 	= Rd_u8(address_temp++);	//整表实际行数
// // // 	
// // // 	
// // // 	//addr_read_count  =address_temp;
// // // 	
// // // 	if(table_OK_flag_temp == __TRUE)        //整表有效标志 =有效
// // // 	{
// // // 		//----------------------    单对象、整表(单索引,双索引)   --------------------------
// // // 		if(count_index_temp 			==0)					
// // // 		{
// // // 			if((p_Target->sub_index[0] > 0)||(p_Target->sub_index[1] > 0))
// // // 			{
// // // 				return (__FALSE);
// // // 			}
// // // 			else
// // // 			{
// // // 				line_num_real_temp 	 = Rd_u8(old_address + 2);
// // // 				line_num_temp			= Object_table[ID_temp-0x81].Line_Number;
// // // 				line_bytes_temp  = Object_table[ID_temp-0x81].Line_Byte_Max; 
// // // 				
// // // 				if(Object_table[ID_temp-0X81].Index2 != 0)
// // // 				{		//双索引
// // // 						if((line_num_real_temp*line_bytes_temp) <= SUB_TARGET_BUFSIZE)
// // // 						{
// // // 							DB_Read_M(p_Target);						
// // // 						}
// // // 						else
// // // 						{
// // // 							p_Target->count_index=2;
// // // 							for(i=1;i<Index1_max_temp;i++)
// // // 							{	
// // // 								p_Target->sub_index[0]=i;
// // // 								for(j=1;j<Index2_max_temp;j++)
// // // 								{
// // // 									p_Target->sub_index[1]=j;
// // // 									DB_Read_M(p_Target);
// // // 									//添加发送  否则会覆盖掉
// // // 								}
// // // 							}
// // // 						}
// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 != 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{   //单索引
// // // 						if((line_num_real_temp*line_bytes_temp) <= SUB_TARGET_BUFSIZE)
// // // 						{
// // // 							DB_Read_M(p_Target);
// // // 						
// // // 						}
// // // 						else
// // // 						{
// // // 							p_Target->count_index=1;
// // // 							for(i=1;i<Index1_max_temp;i++)
// // // 							{
// // // 								p_Target->sub_index[0]=i;
// // // 								DB_Read_M(p_Target);
// // // 								//添加发送  否则会覆盖掉
// // // 							}
// // // 						}
// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 == 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{		//单对象
// // // 					DB_Read_M(p_Target);
// // // 				}
// // // 				else return (__FALSE);
// // // 			}
// // // 		}
// // // 		//----------------------    单索引   --------------------------------------------------
// // // 		else if(count_index_temp 	==1)  			
// // // 		{
// // // 			No_line_temp = p_Target->sub_index[0];						//行号
// // // 			
// // // 			if((No_line_temp > Index1_max_temp)||(p_Target->sub_index[1] != 0))
// // // 			{
// // // 				return (__FALSE);
// // // 			}
// // // 			else
// // // 			{
// // // 				line_num_temp=1;
// // // 				if(Object_table[ID_temp-0X81].Index2 != 0)
// // // 				{
// // // 					p_Target->count_index=2;	
// // // 								p_Target->sub_index[0]=No_line_temp;
// // // 								for(j=1;j<Index2_max_temp;j++)
// // // 								{
// // // 									p_Target->sub_index[1] = j;
// // // 									No_line_temp = ((p_Target->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2) 
// // // 												+ p_Target->sub_index[1];	//行号
// // // 									DB_Read_M(p_Target);
// // // 									//添加发送  否则会覆盖掉
// // // 								}

// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 != 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{		//单索引

// // // 						if(sub_target_temp == 0)  												/* 整行 */
// // // 						{
// // // 							DB_Read_M(p_Target);			
// // // 							
// // // 						}
// // // 						else  				 																		 /* 单个子对象 */  
// // // 						{
// // // 							DB_Read_M(p_Target);
// // // 							
// // // 						}
// // // 				}
// // // 			}
// // // 			
// // // 		}
// // // 		//----------------------    双索引  --------------------------------------------------
// // // 		else if(count_index_temp 	==2)  			
// // // 		{ 
// // // 			if((p_Target->sub_index[0] > Index1_max_temp)||(p_Target->sub_index[1] > Index2_max_temp))
// // // 			{
// // // 				return (__FALSE);
// // // 			}
// // // 			else
// // // 			{
// // // 				No_line_temp = ((p_Target->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2) 
// // // 												+ p_Target->sub_index[1];	//行号
// // // 				line_num_temp=1;
// // // 				
// // // 				if(sub_target_temp ==0)  													//整行
// // // 				{	

// // // 					if(ID_temp ==0x92)
// // // 					{
// // // 							line_bytes_temp  = Object_table[ID_temp-0X81].Line_Byte_Max;	//获得行字节数

// // // 							address_temp += ((No_line_temp-1)*(2+line_bytes_temp));		    //跳过前面N-1行的地址		
// // // 							
// // // 							addr_read_count =address_temp;
// // // 						
// // // 							line_OK_flag_temp = Rd_u8(address_temp++);					//获得行有效标志位
// // // 							if((p_Target->sub_index[1] ==1)&&((line_OK_flag_temp !=0)))
// // // 									line_OK_flag_temp=__TRUE;  
// // // 						
// // // 							address_temp++;																			//每行前 保留字
// // // 							if(line_OK_flag_temp == __TRUE)
// // // 							{
// // // 								for(i=0;i<line_bytes_temp;i++)
// // // 								{					
// // // 									p_Target->sub_buf[read_count++] = Rd_u8(address_temp++);	
// // // 								}
// // // 								return(read_count);																// 返回读取到的数据的长度						
// // // 							}
// // // 							else
// // // 							{//行无效，返回
// // // 								Return_Flag = __FALSE;
// // // 							}					
// // // 					}
// // // 					else
// // // 					{
// // // 						line_bytes_temp  = Object_table[ID_temp-0X81].Line_Byte_Max;	//获得行字节数

// // // 						address_temp += ((No_line_temp-1)*(2+line_bytes_temp));		    //跳过前面N-1行的地址		
// // // 						
// // // 						line_OK_flag_temp = Rd_u8(address_temp++);					//获得行有效标志位
// // // 						address_temp++;																			//每行前 保留字
// // // 						if(line_OK_flag_temp == __TRUE)
// // // 						{
// // // 							for(i=0;i<line_bytes_temp;i++)
// // // 							{					
// // // 								p_Target->sub_buf[read_count++] = Rd_u8(address_temp++);	
// // // 							}
// // // 							return(read_count);																// 返回读取到的数据的长度						
// // // 						}
// // // 						else
// // // 						{//行无效，返回
// // // 							Return_Flag = __FALSE;
// // // 						}
// // // 						
// // // 					}
// // // 					
// // // 					
// // // 				}
// // // 				else   																						//  单个子对象
// // // 				{	 
// // // 					DB_Read_M(p_Target);
// // // 					
// // // 				}
// // // 			}
// // // 						
// // // 		}
// // // 		//----------------------    三索引  --------------------------------------------------	
// // // 		else																
// // // 		{
// // // 			/*  三索引表，保留  */
// // // 		}	
// // // 	}
// // // 	else
// // // 	{
// // // 		Return_Flag = __FALSE;//表无效，返回
// // // 	}
// // // 	//return (Return_Flag);
// // // 	//----------------------    end   --------------------------------------------------	
// // // }


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

u8 DB_Read_Real_Line(TARGET_DATA *p_Target)
{
	u32 							i										= 0;
// 	u32 							j										= 0;
// 	u32 							n										= 0;
//	u8 								Return_Flag 				= __TRUE;
	
	u32 	            address_temp 				= 0;
	u32 	            old_address 				= 0;
//	u32								No_line_temp				= 0;	//第几行
//	u32								line_num_temp				= 0;	//总行数
//	u32								line_num_real_temp	= 0;	//总行数	
	u8								line_bytes_temp			= 0;	//每行字节数
//	u8	 							subobj_bytes_temp   = 0;	//子对象字节数
	
	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;
	
	u32								write_count=0;	
	u32								line_write_count=0;   
//	u32								line_write_count_temp=0;   
	u8								lines_count_temp=0;
	
	u8 								table_OK_flag_temp  = 0;	//表有效标志
	u8 								line_num_flag_temp  = 0;	//实际行数标志
	
//	u8 								line_OK_flag_temp   = 0;	//行有效标志
//	u32								read_count=0;	

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	ID_temp 						= p_Target->ID;
	count_index_temp		= p_Target->count_index;
	sub_target_temp			= p_Target->sub_target;
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;
	//line_num_temp    		= p_Target->sub_index[0];
	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//获得地址
	old_address					=	address_temp;
	
	line_bytes_temp     = Object_table[ID_temp-0X81].Line_leng;
	table_OK_flag_temp  = Rd_u8(address_temp++); 	//整表有效标志				
	address_temp++;																//保留 
	line_num_flag_temp 	= Rd_u8(address_temp++);	//整表实际行数
//  return line_num_flag_temp;
	
	
	if(Object_table[ID_temp-0X81].Index2 != 0)
	{//双索引
		for(i=0;i<Index1_max_temp;i++)
				{
					if(Rd_u8(old_address+3 + (i*Index2_max_temp*(2+line_bytes_temp))) == __TRUE)
						line_write_count++;
				}
				
				if(line_write_count>255)
				{
					return 0;
				}
				else
				{
					lines_count_temp = (u8) line_write_count;
				}
				return lines_count_temp;
	}
	else if((Object_table[ID_temp-0X81].Index1 != 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
	{//单索引
		for(i=0;i<Index1_max_temp;i++)
				{
					if(Rd_u8(old_address+3 + (i*(2+line_bytes_temp))) == __TRUE)
						line_write_count++;
				}
				
				if(line_write_count>255)
				{
					return 0;
				}
				else
				{
					lines_count_temp = (u8) line_write_count;
				}
				return lines_count_temp;
	}
	else if((Object_table[ID_temp-0X81].Index1 == 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
	{//整表

		return lines_count_temp;
	}
	return 0;  
}






/*==============================================================================================
* 语法格式：    void DB_Read_92(TARGET_DATA *p_Target)
* 实现功能：    读数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 返回值：      所读到的字节数,如果错误,返回__FALSE(=0)
===============================================================================================*/


u32 DB_Read_92(TARGET_DATA *p_Target,to_eth_struct * p_To_eth)
{
	u32 							i										= 0;
	u32 							j										= 0;
	u8 								Return_Flag 				= __TRUE;
	u32								read_count=0;
	u32 	            	Flag_addr_temp 				= 0;
	u32 	            	Data_addr_old					= 0;
	u32 	            	Data_addr_temp 				= 0;
	u32								No_line_temp				= 0;	//第几行
//	u32								line_num_temp				= 0;	//总行数
	u8								line_bytes_temp			= 0;	//每行字节数
//	u8	 							subobj_bytes_temp   = 0;	//子对象字节数
	
	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;
	
	u8 								Index1_temp			= 0;
	u8 								Index2_temp			= 0;
	
	
	u8 								table_OK_flag_temp  = 0;	//表有效标志
//	u8 								line_num_flag_temp  = 0;	//实际行数标志
	
	u8 								line_OK_flag_temp   = 0;	//行有效标志


	

	IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	
	ID_temp 						= p_Target->ID;
	count_index_temp		= p_Target->count_index;
	sub_target_temp			= p_Target->sub_target;
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;
	
	Index1_temp					= p_Target->sub_index[0];
	Index2_temp					= p_Target->sub_index[1];
	
	

	p_To_eth->data[p_To_eth->lenth++]	= ID_temp;
	p_To_eth->data[p_To_eth->lenth++]	= (((count_index_temp)<<6) | (sub_target_temp) );
	p_To_eth->data[p_To_eth->lenth++]	= Index1_temp;

// //	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//获得地址
	Flag_addr_temp 		= Object_table[ID_temp-0x81].Flag_Addr;     //标识地址	
	Data_addr_old 		= Object_table[ID_temp-0x81].Data_Addr;     //数据地址

	Data_addr_temp    = Data_addr_old;
	
	table_OK_flag_temp  = Rd_u8(Flag_addr_temp++); 	//整表有效标志				
	Data_addr_temp++;																//N索引一有效个数


	if(table_OK_flag_temp == TAB_OK_FLAG)        //整表有效标志 =有效
	{
		line_bytes_temp  = Object_table[ID_temp-0X81].Line_leng;	//获得行字节数	
		No_line_temp = ((Index1_temp-1)*255) + 1; // 从M=1开始读取  
  	
		Flag_addr_temp += (No_line_temp-1);
		Data_addr_temp += ((No_line_temp-1)*(line_bytes_temp));		    //跳过前面N-1行的地址		
		for(j=0;j<255;j++)
		{		 
				    
					line_OK_flag_temp = Rd_u8(Flag_addr_temp++);					//获得行有效标志位
					if(line_OK_flag_temp == LINE_OK_FLAG)
					{
						for(i=0;i<line_bytes_temp;i++)
						{										
							p_To_eth->data[p_To_eth->lenth++] = Rd_u8(Data_addr_temp++);
							read_count++;
						}
																					
					}
		}
		IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
		GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
		return(read_count);			// 返回读取到的数据的长度
	}	
	else
	{
		IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */   
		GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog		
		Return_Flag = __FALSE;//表无效，返回
	}
	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	return (Return_Flag);
	//----------------------    end   --------------------------------------------------	
}


/*==============================================================================================
* 语法格式：    void DB_Check_C1_M(TARGET_DATA *p_Target)
* 实现功能：    读数据库
* 参数1：       TARGET_DATA *		p_Target  通过ETH接收到的数据指针(标准格式)
* 返回值：      所读到的字节数,如果错误,返回__FALSE(=0)
===============================================================================================*/
u32 DB_Check_C1_M(TARGET_DATA *p_Target)
{
	u32 							i										= 0;
	u32 							j										= 0;
//	u8 								Return_Flag 				= __TRUE;
	u8								B0_check_temp[16]	  = {0}; 
	u32 	            address_temp 				= 0;
	u32 	            old_address					= 0;
//	u32								No_line_temp				= 0;	//第几行
	u32								line_num_temp				= 0;	//总行数
	u8								line_bytes_temp			= 0;	//每行字节数
//	u8	 							subobj_bytes_temp   = 0;	//子对象字节数

	u32								line_B0_num_temp		= 0;	//第几行
	u32								line_B0_bytes_temp	= 0;	//总行数

	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;

	u8 								Index1_temp			= 0;
	u8 								Index2_temp			= 0;	
	
	
//	u8 								table_OK_flag_temp  = 0;	//表有效标志
//	u8 								line_num_flag_temp  = 0;	//实际行数标志	
//	u8 								line_OK_flag_temp   = 0;	//行有效标志
	
	
	u16								Open_phase 					= 0;
	u8								Open_channel_temp		= 0;
	u8								Channel_num_temp		= 0;
//	u32								read_count					= 0;
	
	
	u32								count_all						= 0;
	u32								count_odd						= 0;
	u32								count_even					= 0;
	
	u32								read_error_or_true					= 0;
	
	TARGET_DATA 			B0_Target;	

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	
	ID_temp 						= p_Target->ID;
	count_index_temp		= p_Target->count_index;
	sub_target_temp			= p_Target->sub_target;
	Index1_temp					= p_Target->sub_index[0];
	Index2_temp					= p_Target->sub_index[1];
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;

	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//获得地址
	old_address					= address_temp;
	
//      u8   ID;                           //操作对象标识 
//      u8   count_index;                  //操作索引个数 
//      u8   sub_index[3];                 //索引
//      u8   sub_target;                   //子对象 
//      u8   sub_target_type;              //子对象操作类型 
//      u8   sub_buf[SUB_TARGET_BUFSIZE];  //操作数据

	line_num_temp 	 = Object_table[ID_temp-0x81].Line_Number;
	line_bytes_temp  = Object_table[ID_temp-0x81].Line_leng; 
	
	B0_Target.ID							= 0xB0;
	B0_Target.count_index			= 0;
	B0_Target.sub_index[0]		= 0;
	B0_Target.sub_index[1]		= 0;
	B0_Target.sub_target			= 0;
	line_B0_num_temp 	 = Object_table[0xB0-0x81].Line_Number;
	line_B0_bytes_temp  = Object_table[0xB0-0x81].Line_leng; 
	
	//------------------- 获得B0表参数 start ----------------------------------
	
	os_mut_wait(Spi_Mut_lm,0xFFFF);
	read_error_or_true	=DB_Read_M(&B0_Target);
	os_mut_release(Spi_Mut_lm);	
	
	
	if( read_error_or_true != 0)
	{

			for(j=0;j<16;j++)
			{
				if(B0_Target.sub_buf[j*line_B0_bytes_temp] == (j+1))
				{
					B0_check_temp[j] = B0_Target.sub_buf[j*line_B0_bytes_temp+1];
				}
			}		
	}
	else
	{
		return WRONG;
	}
	//------------------- 获得B0表参数 end ------------------------------------
	
	//------------------- 遍历 start -----------------------------------------------
	
	
	Open_phase = (((0x00FF & (p_Target->sub_buf[2]))<<8) | (p_Target->sub_buf[3]));	//获得放行相位 16bit
	
	for(i=0;i<16;i++)
	{
		Open_channel_temp = 0;
		
		if((Open_phase & (0x0001<<i)) == (0x0001<<i))
		{															//通道总数
			Open_channel_temp =(i+1);		
			for(j=0;j<16;j++)
			{
				if(Open_channel_temp == B0_check_temp[j] )
				{							
					Channel_num_temp = j+1;
					count_all++;	
					if((Channel_num_temp & 0x01) == 1)
						{
							count_odd++;										//奇数通道的个数
						}
					else
						{
							count_even++;										//偶数通道的个数
						}
				}			
			}		
		}
	}	
	//------------------- 遍历 end -----------------------------------------------

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    


	//------------------- 判断 start -----------------------------------------------	
	if	((count_all ==	count_odd) || (count_all ==	count_even))
		return RIGHT;
	else 
		return WRONG;
	//------------------- 判断 end  -----------------------------------------------	

	//----------------------    end   --------------------------------------------------	
}
//
