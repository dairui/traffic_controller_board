#include "Include.h"

#define TRUE		1
#define FALSE	  0

#define DB_RIGHT __TRUE
#define DB_ERROR __FALSE

/*==============================================================================================
* �﷨��ʽ��    void DB_Read_M(TARGET_DATA *p_Target)
* ʵ�ֹ��ܣ�    �����ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����ֵ��      ���������ֽ���,�������,����__FALSE(=0)
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
	
	if(ID_temp == ROAD_MAP_ID)   ////��ʶ ROAD_MAP_ID ����0X70 //mm20140509)
	{
		if((p_Taget_set->sub_target   == 0) &&
			 (p_Taget_set->count_index  == 0) &&
			 (p_Taget_set->sub_index[0] == 0) &&
			 (p_Taget_set->sub_index[1] == 0)	)  //������� 0x70 ֻ�е�����һ�������ȷ mm20140509)
			{
				Data_addr_temp	= TAB_ROAD_MAP_ID_ADDR;
				for(i=0; i<2; i++)
				{
					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
				}
				ROAD_MAP_bytes_temp = ( ((0x00FF & (p_Taget_set->sub_buf[0]))<<8) 
																| (p_Taget_set->sub_buf[1]) );
					//��ʶ ROAD_MAP_ID �ֽ��� ��λ��ǰ����λ�ں�  
				for(i=0; i<ROAD_MAP_bytes_temp; i++)
				{
					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++);
				}
			}
	}
	else // mm20140509)
	{
		Flag_addr_temp 		= Object_table[ID_temp-0x81].Flag_Addr;     //��ʶ��ַ	
		Data_addr_temp		= Object_table[ID_temp-0x81].Data_Addr;     //���ݵ�ַ	
		
		Tab_OK_flag_temp  = Rd_u8(Flag_addr_temp); 	//������Ч��־	
		if(p_Taget_set->sub_target == 0)
		{
			if(Tab_OK_flag_temp == TAB_OK_FLAG) //����Ч
			{	
				line_bytes_temp  = Object_table[ID_temp-0x81].Line_leng; //ÿ���ֽ���
	//----------------- ����(������,˫����)   --------------------------
				if((p_Taget_set->count_index == 0) && (p_Taget_set->sub_index[0] == 0) 
						&& (p_Taget_set->sub_index[1] == 0))//����
				{
					if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //������
					{
						p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //����һ��Ч����  ����
					}
					else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0)) //˫����
					{
						return(0);
					}
	// // 				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫����
	// // 				{
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //����һ��Ч����  ����
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //��������Ч����
	// // 				}
	// // 				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫����
	// // 				{
	// // 					p_Taget_set->sub_buf[read_count++] = Rd_u8(Data_addr_temp++); //����һ��Ч����  ����
	// // 					Data_addr_temp++;
	// // 				}
					
					line_num_temp 	 = Object_table[ID_temp-0x81].Line_Number;

					for(j=0; j<line_num_temp; j++)
					{
						if(Rd_u8(++Flag_addr_temp) == LINE_OK_FLAG) //����Ч
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
	//----------------- �������� ����һ��ʽ ��ѯ -------------------------------------------------
				else if((p_Taget_set->count_index == 1)																														//һ������
								 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//����һ��Ч
								 &&(Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0) )//��������         ����һ��ʽ		
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
	//----------------- ˫������ ����һ��ʽ ��ѯ -------------------------------------------------
				else if((p_Taget_set->count_index == 2)																													//��������
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//����һ��������Ч
							 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0))//˫������          ����һ��ʽ
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
	//----------------- ˫������ ��������ʽ ��˫������ʽ����ѯ -------------------------------------------------
				else if((p_Taget_set->count_index == 2)																														//��������
								 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//����һ��������Ч
								 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0))//˫������          ˫������ʽ
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
* �﷨��ʽ��    void DB_Write(TARGET_DATA *p_Target)
* ʵ�ֹ��ܣ�    д���ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����ֵ��      ��
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
	u16 ROAD_MAP_bytes_temp = 0;   ////��ʶ ROAD_MAP_ID �����ֽ��� ����0X70 mm20140509)
	
	ID_temp = p_Taget_set->ID;
	
	if(ID_temp == ROAD_MAP_ID)   ////��ʶ ROAD_MAP_ID ����0X70 //mm20140509)	
	{
		if((p_Taget_set->sub_target   == 0) &&
			 (p_Taget_set->count_index  == 0) &&
			 (p_Taget_set->sub_index[0] == 0) &&
			 (p_Taget_set->sub_index[1] == 0)	) //������� 0x70 ֻ�е�����һ�������ȷ mm20140509)
			{
				Data_addr_temp	          = TAB_ROAD_MAP_ID_ADDR;
		// 		p_Taget_set->sub_target   = 0;
		// 		p_Taget_set->count_index  = 0;
		// 		p_Taget_set->sub_index[0] = 0;
		// 		p_Taget_set->sub_index[1] = 0;
				ROAD_MAP_bytes_temp = ( ((0x00FF & (p_Taget_set->sub_buf[0]))<<8) 
																| (p_Taget_set->sub_buf[1]) ); 
							//��ʶ ROAD_MAP_ID �����ֽ��� ��λ��ǰ����λ�ں�  
				ROAD_MAP_bytes_temp += 2; ////��ʶ ROAD_MAP_ID ���ֽ���= �����ֽ��� +2����ԭ�����Ӷ�ʵ��
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
		//�������ݳ���
		if(p_Taget_set->sub_target == 0) //����ʱ�Ӷ���Ϊ0��ֻ֧�����л�����
		{
			Data_addr_old 		= Object_table[ID_temp-0x81].Data_Addr;     //���ݵ�ַ
			Data_addr_temp	 	=	Data_addr_old	;
			Flag_addr_old			= Object_table[ID_temp-0x81].Flag_Addr;     //��ʶ��ַ
			Flag_addr_temp 		= Flag_addr_old ;
			Line_Number_temp 	= Object_table[ID_temp-0X81].Line_Number; //֧������
	//----------------- �������� -------------------------------------------------
			if((p_Taget_set->count_index == 0)
					&&(p_Taget_set->sub_index[0] == 0)
					&&(p_Taget_set->sub_index[1] == 0)
					&&(Object_table[ID_temp-0X81].Index2 == 0))//���� //˫������֧����������
			{
				
				if((Object_table[ID_temp-0X81].Index1 == 0)  && (Object_table[ID_temp-0X81].Index2 == 0))      //������
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //������
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 1;
					Data_addr_temp += 1;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫����
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 2;
					Data_addr_temp += 2;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //˫����0x92��
				{
					Data_leng_temp = Object_table[ID_temp-0X81].Tab_leng - 1;
					Data_addr_temp += 1;
				}


				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;		//д�����ʶ��������ʶ
				for(i=0; i<Line_Number_temp; i++)
				{
					Wr_u8(Flag_addr_temp++,LINE_OK_FLAG) ;	//д���б�ʶ��������ʶ
				}
				
				for(i=0;i<Data_leng_temp;i++)						 //д�������ǰ ȫ����
				{
					Wr_u8(Data_addr_temp++,0) ;	
				}	
				Data_addr_temp	=		Data_addr_old;	

				
				if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //������
				{
					Data_addr_temp ++;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫����
				{
					Data_addr_temp ++;
					Data_addr_temp ++;
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //˫����0x92��
				{
					Data_addr_temp ++;
				}
				
				
				for(i=0;i<Data_leng_temp;i++)						//д��������
				{
					Wr_u8(Data_addr_temp++,(p_Taget_set->sub_buf[write_count++])) ;	
				}
				//========= line ================
				if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //������
				{
					Wr_u8(Data_addr_old , Object_table[ID_temp-0X81].Index1 );
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫����
				{
					Wr_u8(Data_addr_old,  Object_table[ID_temp-0X81].Index1);
					Wr_u8((Data_addr_old + 1), Object_table[ID_temp-0X81].Index2 );
				}
				else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //˫����0x92��
				{
					Wr_u8(Data_addr_old,  Object_table[ID_temp-0X81].Index1);
				}
				//===============================
				
				
			}
	//----------------- �������� ����һ��ʽ ���� -------------------------------------------------
			else if((p_Taget_set->count_index == 1)																														//һ������
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] == 0)							//����һ��Ч
							 &&(Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0) )//��������         ����һ��ʽ
			{
				Data_leng_temp = Object_table[ID_temp-0X81].Line_leng;
				Data_addr_old = Object_table[ID_temp-0x81].Data_Addr; //+ (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng);
																								//����ǰ��N-1�еĵ�ַ��NΪ����1�ĵ�ǰҪд���ֵ
				Data_addr_temp = Data_addr_old + (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng);
				Data_addr_temp += 1;
				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//д�����ʶ��������ʶ
				for(i=1; i<p_Taget_set->sub_index[0]; i++)
				{
					Flag_addr_temp++;
				}
				Wr_u8(Flag_addr_temp,LINE_OK_FLAG) ;		//д���б�ʶ��������ʶ
				

				for(i=0;i<Data_leng_temp;i++)						//д���в���ǰ ȫ����
				{
					Wr_u8(Data_addr_temp++,0) ;	
				}	
				
				Data_addr_temp	=		Data_addr_old + (p_Taget_set->sub_index[0]-1)*(Object_table[ID_temp-0X81].Line_leng) + 1;
				for(i=0;i<Data_leng_temp;i++)						//д��������
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
				Wr_u8(Data_addr_old , Index1_lines_count );  //д����һ��Ч����
				//==================================
				
			}
			
	//----------------- ˫������ ����һ��ʽ ���� -------------------------------------------------
			else if((p_Taget_set->count_index == 2)																														//��������
							 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//����һ��������Ч
							 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0) )//˫������          ����һ��ʽ
			{			
	// //			Data_leng_temp = Object_table[ID_temp-0X81].Line_leng * Object_table[ID_temp-0X81].Index2;
				Data_leng_temp = Object_table[ID_temp-0X81].Line_leng * p_Taget_set->sub_index[1]; //abcdefg
	// // 			Data_addr_old  = Object_table[ID_temp-0x81].Data_Addr 
	// // 											 + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1] - 1)*(Object_table[ID_temp-0X81].Line_leng);
	// // 				//����ǰ��{(N-1)*index2_max +index[2]-1}�еĵ�ַ��NΪ����1�ĵ�ǰҪд���ֵ��MΪ����2�ĵ�ǰҪд���ֵ
				
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
				
				Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//д�����ʶ��������ʶ
				for(i=0; i<((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2); i++)//1-->0 abcdefg
				{
					Flag_addr_temp++;
				}
	// // 			for(i=0; i<Object_table[ID_temp-0X81].Index2; i++)
				for(i=0; i<p_Taget_set->sub_index[1]; i++)
				{
					Wr_u8(Flag_addr_temp++,LINE_OK_FLAG) ;		//д���б�ʶ��������ʶ	
				}
				
				for(i=0;i<Data_leng_temp;i++)						//д���в���ǰ ȫ����
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
				
				
				for(i=0;i<Data_leng_temp;i++)						//д��������
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
				Wr_u8(Data_addr_old , Index1_lines_count );					//д����һ��Ч����
				if(ID_temp != 0x92)
				{
					Wr_u8((Data_addr_old + 1), Index2_lines_count );		//д��������Ч�������ֵ
				}
				//=========================================
				
			}
			
	// // // //----------------- ˫������ ��������ʽ ��˫������ʽ������ -------------------------------------------------
	// // // 		else if((p_Taget_set->count_index == 2)																														//��������
	// // // 						 &&(p_Taget_set->sub_index[0] != 0) 						 && (p_Taget_set->sub_index[1] != 0)							//����һ��������Ч
	// // // 						 &&(Object_table[ID_temp-0X81].Index1 != 0)	 && (Object_table[ID_temp-0X81].Index2 != 0) )//˫������          ˫������ʽ
	// // // 		{			
	// // // 			Data_leng_temp = Object_table[ID_temp-0X81].Line_leng;
	// // // 			Data_addr_old = Object_table[ID_temp-0x81].Data_Addr 
	// // // 											 + ((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2 + p_Taget_set->sub_index[1] - 1)*(Object_table[ID_temp-0X81].Line_leng);
	// // // 				//����ǰ��{(N-1)*index2_max +index[2]-1}�еĵ�ַ��NΪ����1�ĵ�ǰҪд���ֵ��MΪ����2�ĵ�ǰҪд���ֵ
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
	// // // 			Wr_u8(Flag_addr_temp++,TAB_OK_FLAG) ;			//д�����ʶ��������ʶ
	// // // 			for(i=1; i<(((p_Taget_set->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2)+p_Taget_set->sub_index[1]); i++)
	// // // 			{
	// // // 				Flag_addr_temp++;
	// // // 			}
	// // // 			Wr_u8(Flag_addr_temp,LINE_OK_FLAG) ;		//д���б�ʶ��������ʶ
	// // // 			
	// // // 			
	// // // 			for(i=0;i<Data_leng_temp;i++)						//д���в���ǰ ȫ����
	// // // 			{
	// // // 				Wr_u8(Data_addr_temp++,0) ;	
	// // // 			}
	// // // 			Data_addr_temp	=		Data_addr_old;						//д�����ʶ��������ʶ
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
	// // // 			for(i=0;i<Data_leng_temp;i++)						//д��������
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
	// // // 			Wr_u8(Data_addr_old , Index1_lines_count );					//д����һ��Ч����
	// // // 			if(ID_temp != 0x92)
	// // // 			{
	// // // 				Wr_u8((Data_addr_old + 1), Index2_lines_count );		//д��������Ч�������ֵ
	// // // 			}
	// // // 			//==============================================
	// // // 		}
			
		}
	}
	return (write_count);

}






/*==============================================================================================
* �﷨��ʽ��    void DB_Read_Analysis(TARGET_DATA *p_Target)
* ʵ�ֹ��ܣ�    �����ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����ֵ��      ���������ֽ���,�������,����__FALSE(=0)
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
// // // 	u32								No_line_temp				= 0;	//�ڼ���
// // // 	u32								line_num_temp				= 0;	//������
// // // 	u32								line_num_real_temp	= 0;	//������	
// // // 	u8								line_bytes_temp			= 0;	//ÿ���ֽ���
// // // 	u8	 							subobj_bytes_temp   = 0;	//�Ӷ����ֽ���
// // // 	
// // // 	u8 								ID_temp 						= 0;
// // // 	u8 								count_index_temp		= 0;
// // // 	u8 								sub_target_temp			= 0;
// // // 	u8 								Index1_max_temp			= 0;
// // // 	u8 								Index2_max_temp			= 0;
// // // 	
// // // 	
// // // 	u8 								table_OK_flag_temp  = 0;	//����Ч��־
// // // 	u8 								line_num_flag_temp  = 0;	//ʵ��������־
// // // 	
// // // 	u8 								line_OK_flag_temp   = 0;	//����Ч��־

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
// // // 	address_temp		 		= Object_table[ID_temp-0X81].Object_Addr;	//��õ�ַ
// // // 	old_address					=	address_temp;
// // // 	addr_read_count  		=	address_temp;//   debug_lm	
// // // 	line_read_count  		=	0;//   debug_lm	
// // // 	no_line_read_count  =	0;//   debug_lm	
// // // 	
// // // 	
// // // 	table_OK_flag_temp  = Rd_u8(address_temp++); 	//������Ч��־				
// // // 	address_temp++;																//���� 
// // // 	line_num_flag_temp 	= Rd_u8(address_temp++);	//����ʵ������
// // // 	
// // // 	
// // // 	//addr_read_count  =address_temp;
// // // 	
// // // 	if(table_OK_flag_temp == __TRUE)        //������Ч��־ =��Ч
// // // 	{
// // // 		//----------------------    ����������(������,˫����)   --------------------------
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
// // // 				{		//˫����
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
// // // 									//��ӷ���  ����Ḳ�ǵ�
// // // 								}
// // // 							}
// // // 						}
// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 != 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{   //������
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
// // // 								//��ӷ���  ����Ḳ�ǵ�
// // // 							}
// // // 						}
// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 == 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{		//������
// // // 					DB_Read_M(p_Target);
// // // 				}
// // // 				else return (__FALSE);
// // // 			}
// // // 		}
// // // 		//----------------------    ������   --------------------------------------------------
// // // 		else if(count_index_temp 	==1)  			
// // // 		{
// // // 			No_line_temp = p_Target->sub_index[0];						//�к�
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
// // // 												+ p_Target->sub_index[1];	//�к�
// // // 									DB_Read_M(p_Target);
// // // 									//��ӷ���  ����Ḳ�ǵ�
// // // 								}

// // // 				}
// // // 				else if((Object_table[ID_temp-0X81].Index1 != 0)&&(Object_table[ID_temp-0X81].Index2 == 0))
// // // 				{		//������

// // // 						if(sub_target_temp == 0)  												/* ���� */
// // // 						{
// // // 							DB_Read_M(p_Target);			
// // // 							
// // // 						}
// // // 						else  				 																		 /* �����Ӷ��� */  
// // // 						{
// // // 							DB_Read_M(p_Target);
// // // 							
// // // 						}
// // // 				}
// // // 			}
// // // 			
// // // 		}
// // // 		//----------------------    ˫����  --------------------------------------------------
// // // 		else if(count_index_temp 	==2)  			
// // // 		{ 
// // // 			if((p_Target->sub_index[0] > Index1_max_temp)||(p_Target->sub_index[1] > Index2_max_temp))
// // // 			{
// // // 				return (__FALSE);
// // // 			}
// // // 			else
// // // 			{
// // // 				No_line_temp = ((p_Target->sub_index[0]-1)*Object_table[ID_temp-0X81].Index2) 
// // // 												+ p_Target->sub_index[1];	//�к�
// // // 				line_num_temp=1;
// // // 				
// // // 				if(sub_target_temp ==0)  													//����
// // // 				{	

// // // 					if(ID_temp ==0x92)
// // // 					{
// // // 							line_bytes_temp  = Object_table[ID_temp-0X81].Line_Byte_Max;	//������ֽ���

// // // 							address_temp += ((No_line_temp-1)*(2+line_bytes_temp));		    //����ǰ��N-1�еĵ�ַ		
// // // 							
// // // 							addr_read_count =address_temp;
// // // 						
// // // 							line_OK_flag_temp = Rd_u8(address_temp++);					//�������Ч��־λ
// // // 							if((p_Target->sub_index[1] ==1)&&((line_OK_flag_temp !=0)))
// // // 									line_OK_flag_temp=__TRUE;  
// // // 						
// // // 							address_temp++;																			//ÿ��ǰ ������
// // // 							if(line_OK_flag_temp == __TRUE)
// // // 							{
// // // 								for(i=0;i<line_bytes_temp;i++)
// // // 								{					
// // // 									p_Target->sub_buf[read_count++] = Rd_u8(address_temp++);	
// // // 								}
// // // 								return(read_count);																// ���ض�ȡ�������ݵĳ���						
// // // 							}
// // // 							else
// // // 							{//����Ч������
// // // 								Return_Flag = __FALSE;
// // // 							}					
// // // 					}
// // // 					else
// // // 					{
// // // 						line_bytes_temp  = Object_table[ID_temp-0X81].Line_Byte_Max;	//������ֽ���

// // // 						address_temp += ((No_line_temp-1)*(2+line_bytes_temp));		    //����ǰ��N-1�еĵ�ַ		
// // // 						
// // // 						line_OK_flag_temp = Rd_u8(address_temp++);					//�������Ч��־λ
// // // 						address_temp++;																			//ÿ��ǰ ������
// // // 						if(line_OK_flag_temp == __TRUE)
// // // 						{
// // // 							for(i=0;i<line_bytes_temp;i++)
// // // 							{					
// // // 								p_Target->sub_buf[read_count++] = Rd_u8(address_temp++);	
// // // 							}
// // // 							return(read_count);																// ���ض�ȡ�������ݵĳ���						
// // // 						}
// // // 						else
// // // 						{//����Ч������
// // // 							Return_Flag = __FALSE;
// // // 						}
// // // 						
// // // 					}
// // // 					
// // // 					
// // // 				}
// // // 				else   																						//  �����Ӷ���
// // // 				{	 
// // // 					DB_Read_M(p_Target);
// // // 					
// // // 				}
// // // 			}
// // // 						
// // // 		}
// // // 		//----------------------    ������  --------------------------------------------------	
// // // 		else																
// // // 		{
// // // 			/*  ������������  */
// // // 		}	
// // // 	}
// // // 	else
// // // 	{
// // // 		Return_Flag = __FALSE;//����Ч������
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
//	u32								No_line_temp				= 0;	//�ڼ���
//	u32								line_num_temp				= 0;	//������
//	u32								line_num_real_temp	= 0;	//������	
	u8								line_bytes_temp			= 0;	//ÿ���ֽ���
//	u8	 							subobj_bytes_temp   = 0;	//�Ӷ����ֽ���
	
	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;
	
	u32								write_count=0;	
	u32								line_write_count=0;   
//	u32								line_write_count_temp=0;   
	u8								lines_count_temp=0;
	
	u8 								table_OK_flag_temp  = 0;	//����Ч��־
	u8 								line_num_flag_temp  = 0;	//ʵ��������־
	
//	u8 								line_OK_flag_temp   = 0;	//����Ч��־
//	u32								read_count=0;	

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	ID_temp 						= p_Target->ID;
	count_index_temp		= p_Target->count_index;
	sub_target_temp			= p_Target->sub_target;
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;
	//line_num_temp    		= p_Target->sub_index[0];
	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//��õ�ַ
	old_address					=	address_temp;
	
	line_bytes_temp     = Object_table[ID_temp-0X81].Line_leng;
	table_OK_flag_temp  = Rd_u8(address_temp++); 	//������Ч��־				
	address_temp++;																//���� 
	line_num_flag_temp 	= Rd_u8(address_temp++);	//����ʵ������
//  return line_num_flag_temp;
	
	
	if(Object_table[ID_temp-0X81].Index2 != 0)
	{//˫����
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
	{//������
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
	{//����

		return lines_count_temp;
	}
	return 0;  
}






/*==============================================================================================
* �﷨��ʽ��    void DB_Read_92(TARGET_DATA *p_Target)
* ʵ�ֹ��ܣ�    �����ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����ֵ��      ���������ֽ���,�������,����__FALSE(=0)
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
	u32								No_line_temp				= 0;	//�ڼ���
//	u32								line_num_temp				= 0;	//������
	u8								line_bytes_temp			= 0;	//ÿ���ֽ���
//	u8	 							subobj_bytes_temp   = 0;	//�Ӷ����ֽ���
	
	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;
	
	u8 								Index1_temp			= 0;
	u8 								Index2_temp			= 0;
	
	
	u8 								table_OK_flag_temp  = 0;	//����Ч��־
//	u8 								line_num_flag_temp  = 0;	//ʵ��������־
	
	u8 								line_OK_flag_temp   = 0;	//����Ч��־


	

	IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
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

// //	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//��õ�ַ
	Flag_addr_temp 		= Object_table[ID_temp-0x81].Flag_Addr;     //��ʶ��ַ	
	Data_addr_old 		= Object_table[ID_temp-0x81].Data_Addr;     //���ݵ�ַ

	Data_addr_temp    = Data_addr_old;
	
	table_OK_flag_temp  = Rd_u8(Flag_addr_temp++); 	//������Ч��־				
	Data_addr_temp++;																//N����һ��Ч����


	if(table_OK_flag_temp == TAB_OK_FLAG)        //������Ч��־ =��Ч
	{
		line_bytes_temp  = Object_table[ID_temp-0X81].Line_leng;	//������ֽ���	
		No_line_temp = ((Index1_temp-1)*255) + 1; // ��M=1��ʼ��ȡ  
  	
		Flag_addr_temp += (No_line_temp-1);
		Data_addr_temp += ((No_line_temp-1)*(line_bytes_temp));		    //����ǰ��N-1�еĵ�ַ		
		for(j=0;j<255;j++)
		{		 
				    
					line_OK_flag_temp = Rd_u8(Flag_addr_temp++);					//�������Ч��־λ
					if(line_OK_flag_temp == LINE_OK_FLAG)
					{
						for(i=0;i<line_bytes_temp;i++)
						{										
							p_To_eth->data[p_To_eth->lenth++] = Rd_u8(Data_addr_temp++);
							read_count++;
						}
																					
					}
		}
		IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
		GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
		return(read_count);			// ���ض�ȡ�������ݵĳ���
	}	
	else
	{
		IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */   
		GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog		
		Return_Flag = __FALSE;//����Ч������
	}
	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
	return (Return_Flag);
	//----------------------    end   --------------------------------------------------	
}


/*==============================================================================================
* �﷨��ʽ��    void DB_Check_C1_M(TARGET_DATA *p_Target)
* ʵ�ֹ��ܣ�    �����ݿ�
* ����1��       TARGET_DATA *		p_Target  ͨ��ETH���յ�������ָ��(��׼��ʽ)
* ����ֵ��      ���������ֽ���,�������,����__FALSE(=0)
===============================================================================================*/
u32 DB_Check_C1_M(TARGET_DATA *p_Target)
{
	u32 							i										= 0;
	u32 							j										= 0;
//	u8 								Return_Flag 				= __TRUE;
	u8								B0_check_temp[16]	  = {0}; 
	u32 	            address_temp 				= 0;
	u32 	            old_address					= 0;
//	u32								No_line_temp				= 0;	//�ڼ���
	u32								line_num_temp				= 0;	//������
	u8								line_bytes_temp			= 0;	//ÿ���ֽ���
//	u8	 							subobj_bytes_temp   = 0;	//�Ӷ����ֽ���

	u32								line_B0_num_temp		= 0;	//�ڼ���
	u32								line_B0_bytes_temp	= 0;	//������

	u8 								ID_temp 						= 0;
	u8 								count_index_temp		= 0;
	u8 								sub_target_temp			= 0;
	u8 								Index1_max_temp			= 0;
	u8 								Index2_max_temp			= 0;

	u8 								Index1_temp			= 0;
	u8 								Index2_temp			= 0;	
	
	
//	u8 								table_OK_flag_temp  = 0;	//����Ч��־
//	u8 								line_num_flag_temp  = 0;	//ʵ��������־	
//	u8 								line_OK_flag_temp   = 0;	//����Ч��־
	
	
	u16								Open_phase 					= 0;
	u8								Open_channel_temp		= 0;
	u8								Channel_num_temp		= 0;
//	u32								read_count					= 0;
	
	
	u32								count_all						= 0;
	u32								count_odd						= 0;
	u32								count_even					= 0;
	
	u32								read_error_or_true					= 0;
	
	TARGET_DATA 			B0_Target;	

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
	
	ID_temp 						= p_Target->ID;
	count_index_temp		= p_Target->count_index;
	sub_target_temp			= p_Target->sub_target;
	Index1_temp					= p_Target->sub_index[0];
	Index2_temp					= p_Target->sub_index[1];
	
	Index1_max_temp			= Object_table[ID_temp-0X81].Index1;
	Index2_max_temp			= Object_table[ID_temp-0X81].Index2;

	address_temp		 		= Object_table[ID_temp-0X81].Data_Addr;	//��õ�ַ
	old_address					= address_temp;
	
//      u8   ID;                           //���������ʶ 
//      u8   count_index;                  //������������ 
//      u8   sub_index[3];                 //����
//      u8   sub_target;                   //�Ӷ��� 
//      u8   sub_target_type;              //�Ӷ���������� 
//      u8   sub_buf[SUB_TARGET_BUFSIZE];  //��������

	line_num_temp 	 = Object_table[ID_temp-0x81].Line_Number;
	line_bytes_temp  = Object_table[ID_temp-0x81].Line_leng; 
	
	B0_Target.ID							= 0xB0;
	B0_Target.count_index			= 0;
	B0_Target.sub_index[0]		= 0;
	B0_Target.sub_index[1]		= 0;
	B0_Target.sub_target			= 0;
	line_B0_num_temp 	 = Object_table[0xB0-0x81].Line_Number;
	line_B0_bytes_temp  = Object_table[0xB0-0x81].Line_leng; 
	
	//------------------- ���B0����� start ----------------------------------
	
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
	//------------------- ���B0����� end ------------------------------------
	
	//------------------- ���� start -----------------------------------------------
	
	
	Open_phase = (((0x00FF & (p_Target->sub_buf[2]))<<8) | (p_Target->sub_buf[3]));	//��÷�����λ 16bit
	
	for(i=0;i<16;i++)
	{
		Open_channel_temp = 0;
		
		if((Open_phase & (0x0001<<i)) == (0x0001<<i))
		{															//ͨ������
			Open_channel_temp =(i+1);		
			for(j=0;j<16;j++)
			{
				if(Open_channel_temp == B0_check_temp[j] )
				{							
					Channel_num_temp = j+1;
					count_all++;	
					if((Channel_num_temp & 0x01) == 1)
						{
							count_odd++;										//����ͨ���ĸ���
						}
					else
						{
							count_even++;										//ż��ͨ���ĸ���
						}
				}			
			}		
		}
	}	
	//------------------- ���� end -----------------------------------------------

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    


	//------------------- �ж� start -----------------------------------------------	
	if	((count_all ==	count_odd) || (count_all ==	count_even))
		return RIGHT;
	else 
		return WRONG;
	//------------------- �ж� end  -----------------------------------------------	

	//----------------------    end   --------------------------------------------------	
}
//
