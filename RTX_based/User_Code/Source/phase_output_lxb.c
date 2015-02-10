#include "Include.h"

extern STAGE_TIMING_COUNT_t		stage_timing_count;//阶段配时表各灯色时间转换的计数值
extern u8		Flashing_pulse_500ms; //500ms状态值，500ms翻转一次
extern u8		Flashing_pulse_1s;		//1000ms状态值，1000ms翻转一次
// // /*
// // 相位参数行数	id=0x95
// // 16*12=192B

// // typedef struct PHASE_PARAM_S
// // {
// // 	US8 		phaseId;		//相位号
// // 	US8 		greenSec;		//行人过街绿灯的秒散
// // 	US8 		cleanTime;	//行人清空时间
// // 	US8		minGreenTime;//最小绿灯时间
// // 	US8 		expandGreen;	//单位绿灯延长时间
// // 	US8		maxGreen_1;	////最大绿灯时问1
// // 	US8		maxGreen_2;	//最大绿灯时问2
// // 	US8 		stadyGreen;		//弹性相位固定绿灯时间
// // 	US8		greenShine;		//绿闪时间
// // 	US8		phaseType;		//相位类型
// // 	US8		phaseFunc;		//相位选项功能
// // 	US8		reserved;			//扩展字段
// // }PHASE_PARAM_t;
// // */

// // /*
// // 相位输出状态表行数 id=0x96
// // 4*2 = 8B
// // typedef struct PHASE_OUTPUT_STAT_S
// // {
// // 	US8		statId;				//相位状态组的行号
// // 	US8		redStat;				//相位的红灯输出状态
// // 	US8		yellowStat;		//相位的黄灯输出状态
// // 	US8		greenStat;			//相位的绿灯输出状态
// // }PHASE_OUTPUT_STAT_t;

// // */

// // /*
// // 相位冲突袭行数id=0x97
// // 16*3=48B
// // typedef struct PHASE_CONFLICT_S
// // {
// // 	US8		index;					//行号
// // 	US16	phase;				//冲突相位	
// // }PHASE_CONFLICT_t;
// // */
// // /*
// // 通道输出状态衰行数id=0xb1
// // 2*4=8B
// // typedef struct CHANNEL_OUTPUT_STAT_S
// // {
// // 	US8		index;						//	行号
// // 	US8		redStat;					//通道的红灯输出状态
// // 	US8		yellowStat;			//通道的黄灯输出状态
// // 	US8		greenStat;				//通道的绿灯输出状态
// // }CHANNEL_OUTPUT_STAT_t;
// // */
// // /*
// // 通道参数衰行数id=0xB0
// // 16*4=64B
// // typedef struct CHANNEL_PARAM_S
// // {
// // 	US8		chnId;					//	1 通道号
// // 	US8		signalSrc;				// 2 通道控制的信号源
// // 	US8		chnStat;					// 3 定 义 Automatic Flash控制模式时的通道状态
// // 	US8		chnCtlType;			// 4 通道控制类型
// // }CHANNEL_PARAM_t;
// // */
// // /*
// // 阶段配时衰行敛id=0xC1
// // 16*16*8 = 2K
// // typedef struct STAGE_TIMING_TABLE_S
// // {
// // 	US8		index;					//	阶段配时方案号
// // 	US8		stageId;					//阶段号
// // 	US16	relPhase;				//放行相位2B
// // 	US8		stGreenTime;		//阶段绿灯时同
// // 	US8		stYellowTime;		//阶段黄灯时间
// // 	US8		stRedTime;			//阶段红灯时间
// // 	US8		stAlterParam;		//阶段选项参数
// // }STAGE_TIMING_TABLE_t; 
// // */

// // /*
// // 16通道输出状态表给id=0xb1的输出表赋值用途
// // */
// // typedef struct CHANNEL_OUTPUT_TOB1_S
// // {
// // 	US8			index;							//	行号
// // 	US16		redStat;					//16通道的红灯输出状态
// // 	US16		yellowStat;				//16通道的黄灯输出状态
// // 	US16		greenStat;				//16通道的绿灯输出状态
// // }CHANNEL_OUTPUT_TOB1_T;
// // /*
// // 阶段配时计数表，由id=0xC1转换而来
// // 16*16*8 = 2K
// // */
// // typedef struct STAGE_TIMING_COUNT_S
// // {
// // 	US8			index;					//	阶段配时方案号
// // 	US8			stageId;					//阶段号
// // 	US16		relPhase;				//放行相位2B
// // 	US16		stGreenCount[16];		//阶段16通道绿灯计数
// // 	US16		stYellowCount[16];	//阶段16通道黄灯计数
// // 	US16		stRedCount[16];			//阶段16通道红灯计数
// // 	US8			stAlterParam;		//阶段选项参数
// // }STAGE_TIMING_COUNT_t; 
// // /*
// // 16相位输出状态表行数 id=0x96
// // 4*2 = 8B
// // */
// // typedef struct PHASE_OUTPUT_16STAT_S
// // {
// // 	US8		  statId;				//相位状态组的行号
// // 	US16		redStat;				//相位的红灯输出状态
// // 	US16		yellowStat;		//相位的黄灯输出状态
// // 	US16		greenStat;			//相位的绿灯输出状态
// // }PHASE_OUTPUT_16STAT_t;
/**
  * @brief  通道互锁输出函数   输出红灯
  * @param  passage	通道号
  * @retval b1的16位通道状态表，包含各通道的状态
	* @invovled function
  */
void  Passage_Red_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //关灯用于闪光=0
{
		channel_output_temp->greenStat  &= ~(u16)(1<<(passage-1));
		channel_output_temp->yellowStat &= ~(u16)(1<<(passage-1));
		channel_output_temp->redStat    |=  (u16)(1<<(passage-1));
}
/**
  * @brief  通道互锁输出函数   输出黄灯
  * @param  passage	通道号
  * @retval b1的16位通道状态表，包含各通道的状态
	* @invovled function
  */
void  Passage_Yellow_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //关灯用于闪光=0
{
		channel_output_temp->greenStat  &= ~((u16)(1<<(passage-1)));
		channel_output_temp->yellowStat |=  (u16)(1<<(passage-1));
		channel_output_temp->redStat    &= ~((u16)(1<<(passage-1)));
}
/**
  * @brief  通道互锁输出函数   输出绿灯
  * @param  passage	通道号
  * @retval b1的16位通道状态表，包含各通道的状态
	* @invovled function
  */
void  Passage_Green_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //关灯用于闪光=0
{
		channel_output_temp->greenStat  |=  (u16)(1<<(passage-1));
		channel_output_temp->yellowStat &= ~((u16)(1<<(passage-1)));
		channel_output_temp->redStat    &= ~((u16)(1<<(passage-1)));
}
/**
  * @brief  通道互锁输出函数   输出灭灯
  * @param  passage	通道号
  * @retval b1的16位通道状态表，包含各通道的状态
	* @invovled function
  */
void  Passage_All_Close_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //关灯用于闪光=0
{
		channel_output_temp->greenStat  &= (~(u16)(1<<(passage-1)));
		channel_output_temp->yellowStat &= (~(u16)(1<<(passage-1)));
		channel_output_temp->redStat    &= (~(u16)(1<<(passage-1)));
}

/**
  * @brief  输出通道输出状态表CHANNEL_OUTPUT_TOB1_T Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1)
  *					通过根据c1表放行相位与找到输出放行的通道，并写入b1表
	*
	* @param  channel_output_C1 时间转换为计数值后的c1表
  * @retval CHANNEL_OUTPUT_TOB1_T，b1的16位通道状态表，包含各通道的状态
	* @invovled function Passage_All_Close_Out等四个函数
  */

U16	type2_follow_phase = 0;//第三种类型的跟随相位
CHANNEL_OUTPUT_TOB1_t Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1,u16* channel_ability,u16* passage_walker)
{
			/*	
		数据库操作需要的变量	
		*/
		u32										read_lenth_temp		=		0;	
		TARGET_DATA 					data_Target;	
				
		u8						i = 0;
		u8						B0_error_count	= 0;
	
		CHANNEL_PARAM_t 				tab_0xb0_temp[Channel_Max] = {0};				//b0表
		CHANNEL_OUTPUT_TOB1_t		channel_status_tob1		= {0};						//输出的b1表
		PHASE_PARAM_t 					phase_par_temp;													//95表
		/*
		读0xB0表	
		*/
		for(i=0;i < Channel_Max;i++)
		{
			read_lenth_temp						=		0;
			data_Target.ID 						= 	0xB0;
			data_Target.count_index 	= 	1;
			data_Target.sub_index[0] 	= 	i+1;
			data_Target.sub_index[1] 	= 	0;
			data_Target.sub_target 		= 	0;

			os_mut_wait(Spi_Mut_lm,0xFFFF);
			read_lenth_temp				=	DB_Read_M(&data_Target);	
			os_mut_release(Spi_Mut_lm);				
			
			if(read_lenth_temp == 	0)
			{
				B0_error_count++;
			}
			else
			{
				tab_0xb0_temp[i].chnId			= data_Target.sub_buf[0];
				tab_0xb0_temp[i].signalSrc		= data_Target.sub_buf[1];
				tab_0xb0_temp[i].chnStat		= data_Target.sub_buf[2];
				tab_0xb0_temp[i].chnCtlType     = data_Target.sub_buf[3];			
			}
		}
		/*	获取各通道使能状态 3.24	*/
		for(i=0;i < Channel_Max;i++)
		{
			if(tab_0xb0_temp[i].signalSrc > 0)		/*	该通道对应的相位>0，则该通道启用	*/
			{
				*channel_ability |= (1<<(tab_0xb0_temp[i].chnId-1));
			}
			else																	/*	否则通道不启用	*/
			{
				*channel_ability &= ~(1<<(tab_0xb0_temp[i].chnId-1));
			}
			if(tab_0xb0_temp[i].chnCtlType==3)    //行人通道保存发给相位板
			{
				*passage_walker |= (u16)(1<<i);
			}
			else
			{
				*passage_walker &= ~((u16)(1<<i));
			}
		}
		if(((channel_output_C1->stAlterParam & 0xe0)>>5)<3)
			{
					channel_output_C1->stAlterParam |= 0x60;
			}	
		for(i=0;i < Channel_Max;i++)
		{
			if (tab_0xb0_temp[i].signalSrc == 0)							//通道未启用,黄闪
			{
				 if(Flashing_pulse_1s) 
					 Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  					//脉冲为高，黄灯输出高
				 else 
					 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);             //脉冲为低，黄灯输出低
			}
			else if((tab_0xb0_temp[i].chnStat & 0x06) >0)									//通道控制模式有设置
			{
				if((tab_0xb0_temp[i].chnStat & 0x04) >0)											//设置通道红闪
				{
					 if(Flashing_pulse_1s) 
						 Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  					//脉冲为高，红灯输出高
					 else 
						 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1) ;          //脉冲为低，红灯输出低
				}					 
				else
				{
					 if(Flashing_pulse_1s) 
						 Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  				//脉冲为高，黄灯输出高
					 else 
						 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1) ;          //脉冲为低，黄灯输出低					
				}
			}
			else																														//通道启用
			{
				if (((channel_output_C1->relPhase & (u16)(1<<(tab_0xb0_temp[i].signalSrc-1))) >0)//通道在放行阶段
				||(((type2_follow_phase>>i)&1)==1))
				{
					/*
					读0x95表	
					*/
					read_lenth_temp						=		0;
					data_Target.ID 						= 	0x95;
					data_Target.count_index 	= 	1;
					data_Target.sub_index[0] 	= 	i+1;			//第i+1个相位对应的95表
					data_Target.sub_index[1] 	= 	0;
					data_Target.sub_target 		= 	0;
					
					os_mut_wait(Spi_Mut_lm,0xFFFF);
					read_lenth_temp				=	DB_Read_M(&data_Target);	
					os_mut_release(Spi_Mut_lm);				
					
					if(read_lenth_temp == 	0)
					{
						phase_par_temp.cleanTime 		= 	0;
						phase_par_temp.expandGreen 	= 	0;
						phase_par_temp.greenSec 		= 	0;
						phase_par_temp.greenShine 	= 	0;
						phase_par_temp.maxGreen_1 	= 	0;
						phase_par_temp.maxGreen_2 	= 	0;
						phase_par_temp.minGreenTime = 	0;
						phase_par_temp.phaseFunc 		= 	0;
						phase_par_temp.phaseId 			= 	0;
						phase_par_temp.phaseType 		= 	0;
						phase_par_temp.reserved 		= 	0;
						phase_par_temp.stadyGreen 	= 	0;
					}
					else
					{
						phase_par_temp.phaseId 			= 	data_Target.sub_buf[0];
						phase_par_temp.greenSec 		= 	data_Target.sub_buf[1];
						phase_par_temp.cleanTime 		= 	data_Target.sub_buf[2];
						phase_par_temp.minGreenTime = 	data_Target.sub_buf[3];
						phase_par_temp.expandGreen 	= 	data_Target.sub_buf[4];
						phase_par_temp.maxGreen_1		= 	data_Target.sub_buf[5];
						phase_par_temp.maxGreen_2		= 	data_Target.sub_buf[6];
						phase_par_temp.stadyGreen 	= 	data_Target.sub_buf[7];
						phase_par_temp.greenShine 	= 	data_Target.sub_buf[8];
						phase_par_temp.phaseType 		= 	data_Target.sub_buf[9];
						phase_par_temp.phaseFunc 		= 	data_Target.sub_buf[10];
						phase_par_temp.reserved 		= 	data_Target.sub_buf[11];			
					}
					
										
					if (channel_output_C1->stGreenCount[i] != 0)
					{
							if(	 ((channel_output_C1->stAlterParam & 0x01) == 0)
								&& ((channel_output_C1->stAlterParam & 0xe0) != 0)
								&& (tab_0xb0_temp[i].chnCtlType==3) 
								&& (((channel_output_C1->stAlterParam & 0xe0)>>2) >= channel_output_C1->stGreenCount[i])
								&& (Flashing_pulse_500ms==0))      //非感应阶段行人绿闪,绿灯灭
							{
								Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(	 ((channel_output_C1->stAlterParam & 0x01) == 0)
											&& ((channel_output_C1->stAlterParam & 0xe0) != 0) 
											&& (tab_0xb0_temp[i].chnCtlType!=3) 
											&& (((channel_output_C1->stAlterParam & 0xe0)>>3) >= channel_output_C1->stGreenCount[i])
											&& (Flashing_pulse_500ms==0))      //非感应阶段机动车绿闪,绿灯灭
							{
								Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(	 ((channel_output_C1->stAlterParam & 0x01) == 1)
											&& ( phase_par_temp.stadyGreen != 0) 
											&& (phase_par_temp.stadyGreen > channel_output_C1->stGreenCount[i])
											&& (Flashing_pulse_1s==0))     //感应阶段绿闪,绿灯灭
							{
								Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else
							{
								Passage_Green_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
					}
					else if(channel_output_C1->stYellowCount[i] != 0)
					{
							if(tab_0xb0_temp[i].chnCtlType!=3)    			//机动车道黄灯正常输出
							{
								Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(tab_0xb0_temp[i].chnCtlType==3)    //行人车道黄灯期间输出红灯
							{
								Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
					}
					else if(channel_output_C1->stRedCount[i] != 0)		 //输出红灯
					{
						Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
					}
					else																					//都为0，则灭灯
					{
						if(channel_output_C1->index != 27)
							Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
						else
							Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
					}
				}
				else   //通道不在放行阶段,通道输出红灯
				{
					Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
				}
			}
					
		}
		return channel_status_tob1;
}
/**
  * @brief  输出通道输出状态表u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp)
  *					通过根据c1表放行相位,并写入16相位的96表
	*
	* @param  channel_output_C1 时间转换为计数值后的c1表
  * @retval phase_output_temp，96表的16位通道状态表，包含各相位状态;
	*					Phase_Run,执行完计数值--后，仍然放行的相位，若为0，则表示阶段结束
	* @invovled function 
  */
u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp)
{

	u8			phase_count = 0;	
	u16			Phase_Run   = 0xFFFF;

	for(phase_count=0;phase_count<16;phase_count++)  //通道状态转换为相位状态表
	{
		if (1)//(channel_output_C1->relPhase & (u16)(1<<phase_count)) >0)//通道在放行阶段
		{
			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
			{
				phase_output_temp->greenStat |= ((u16)(1<<phase_count));  //绿灯状态
			}			
			else if (channel_output_C1->stYellowCount[phase_count] > 1)
			{
				phase_output_temp->yellowStat |= ((u16)(1<<phase_count));  //黄灯状态
			}
			else if(channel_output_C1->stRedCount[phase_count] > 1)
			{
				phase_output_temp->redStat |= ((u16)(1<<phase_count));  		//红灯状态
			}
			else
			{
				Phase_Run &= (~(u16)(1<<phase_count));
			}
		}
		else 
		{
			phase_output_temp->redStat |= ((u16)(1<<phase_count));  		  //非放行相位红灯状态
			Phase_Run &= (~(u16)(1<<phase_count));
		}
	}
	return Phase_Run;  //返回相位输出状态表0x96	
}

u16 Phase_run_step(STAGE_TIMING_COUNT_t *channel_output_C1,U16 type2_follow)
{

	u8			phase_count = 0;	
	u16			Phase_Run   = 0xFFFF;

// 	if(type2_follow == 0)
	{
		for(phase_count=0;phase_count<16;phase_count++)  //通道状态转换为相位状态表
		{
			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
			{
				;  //绿灯状态
			}		
			else
			{
				Phase_Run &= (~(u16)(1<<phase_count));
			}
		}
	}
// 	else
// 	{
// 		for(phase_count=0;phase_count<16;phase_count++)  //通道状态转换为相位状态表
// 		{
// 			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
// 			{
// 				;  //绿灯状态
// 			}			
// 			else if (channel_output_C1->stYellowCount[phase_count] > 1)
// 			{
// 				;  //黄灯状态
// 			}
// 			else if(channel_output_C1->stRedCount[phase_count] > 1)
// 			{
// 				;  		//红灯状态
// 			}
// 			else
// 			{
// 				Phase_Run &= (~(u16)(1<<phase_count));
// 			}
// 		}
// 	}
	return Phase_Run;  //返回相位输出状态表0x96	
}

FOLLOW_PHASE_WORKING_t follow_phase_para[C8_LINE]={0};
TARGET_DATA 		data_Target_follow;
U16 stage_follow_phase(STAGE_TIMING_COUNT_t *channel_output_C1)
{
	u16 stage_followrelPhase=0;    //本阶段的跟随相位号集合
	u16 mainphase=0;             //跟随相位的主相位号集合
	u8 i,follow_count,j;  //k,l,m;	
	
	/*	
	数据库操作需要的变量	
	*/
	u32							read_lenth_temp				=	0;	
// 	TARGET_DATA 		data_Target;		
	
	for(follow_count=0;follow_count<8;follow_count++)    //follow_count=8个跟随相位表.
	{		
		/*
		跟随相位参数衰行数id=0xC8
		8*39 = 312B
		typedef struct FOLLOW_PHASE_PARAM_S
		{
			US8		phaseId;				//跟随相位表号
			US8		operationType;	//操作类型
			US8		incPhaseLenth;	//包古相位长度
			US8		incPhase[16];		//包含相位
			US8		fixPhaseLenth;		//修正相位长度
			US8		fixPhase[16];		//修正相位
			US8		tailGreen;				//尾部绿灯
			US8		tailYellow;				//尾部黄灯
			US8		tailRed;					//尾部红灯
		}FOLLOW_PHASE_PARAM_t;
		*/

		//读取c8
		read_lenth_temp				=	0;
		data_Target_follow.ID 				= 	0xC8;
		data_Target_follow.count_index 		= 	1;
		data_Target_follow.sub_index[0] 		= 	follow_count+1;
		data_Target_follow.sub_index[1] 		= 	0;
		data_Target_follow.sub_target 		= 	0;
		
		os_mut_wait(Spi_Mut_lm,0xFFFF);
	  read_lenth_temp				=	DB_Read_M(&data_Target_follow);	
		os_mut_release(Spi_Mut_lm);	
			
		if(read_lenth_temp 			== 	0	)
		{
			continue;
		}
		else
		{
			follow_phase_para[follow_count].phaseId   = 	data_Target_follow.sub_buf[0];				//跟随相位表号
			follow_phase_para[follow_count].operationType   = 	data_Target_follow.sub_buf[1];	//操作类型
			follow_phase_para[follow_count].incPhaseLenth   = 	data_Target_follow.sub_buf[2];	//包古相位长度
			for(j=0;j<follow_phase_para[follow_count].incPhaseLenth;j++)
			{
				if(data_Target_follow.sub_buf[3+j]>0)
				{
					follow_phase_para[follow_count].incPhase[j]   = 	data_Target_follow.sub_buf[3+j]-1;		//包含相位
				}
				else
				{
					follow_phase_para[follow_count].incPhase[j]   = 	data_Target_follow.sub_buf[3+j];		//包含相位
				}
			}
// // 			follow_phase_para[follow_count].fixPhaseLenth	 	= 	data_Target_follow.sub_buf[19];		//修正相位长度
// // 			for(j=0;j<follow_phase_para[follow_count].fixPhaseLenth;j++)
// // 				follow_phase_para[follow_count].fixPhase[16]   = 	data_Target_follow.sub_buf[20+j];		//修正相位
			follow_phase_para[follow_count].tailGreenCnt   = 	4*data_Target_follow.sub_buf[36];				//尾部绿灯
			follow_phase_para[follow_count].tailYellowCnt   = 	4*data_Target_follow.sub_buf[37];				//尾部黄灯
			follow_phase_para[follow_count].tailRedCnt   = 	4*data_Target_follow.sub_buf[38];					//尾部红灯
		}	
		if(follow_phase_para[follow_count].incPhaseLenth!=0)   //先判断是否设置跟随相位。
		{
			mainphase=0;
			for(i=0;i<follow_phase_para[follow_count].incPhaseLenth;i++)                     //8_3=主相位个数
			{
				 mainphase |= (U16)((U16)1<< follow_phase_para[follow_count].incPhase[i]);             //c8_4[i] 跟随相位的主相位转为2个字节的位表示！
			}
			if((channel_output_C1->relPhase & mainphase)!=0 )        //当前放行相位中有跟随相位的主相位。
				stage_followrelPhase|=(U16)((U16)1<<(follow_count));    //保留本阶段跟随相位号，要全部搜索8张跟随相位表，保留有效的跟随相位
		}			
	}
	//读取c8完毕,计算了本阶段跟随相位号
	return stage_followrelPhase;
}
void follow_phase_c8(STAGE_TIMING_COUNT_t *channel_output_C1,u16* stage_followrelPhase)
{
	u16 mainphase=0;             //跟随相位的主相位号集合
	u8 i,k,l;//m;	j,
	
	//**********************************************************//
	/*1、判断本阶段是否有跟随相位*/
	/*2、根据子对象操作内型判断跟随相位是延长放行还是延迟放行*/
	/*3、延迟放行的要先等延迟时间结束，在开始放行绿灯最后阶段绿灯同时结束，延迟时间子对象尾部绿灯时间*/
	/*4、绿灯结束前黄闪，需要根据16个阶段相位计数器分开判断时间！*/
	/*建议此函数放在250ms定时触发的计数器--内*/	
	if( *stage_followrelPhase!=0 )      //有跟随相位    
	{
		for(i = 0; i < C8_LINE; i++)    //8个跟随相位，依次是1-8，在全部16个相位底8个相位中，跟随相位和放行相位融合，不用考虑通道参数表中通道号和控制的相位
		{
			if((*stage_followrelPhase>>i )==1)			//本阶段的跟随相位 2B
			{
				mainphase = 0;
					//c8_7count赋值需要注意赋值位置，考虑到时间误差问题							
				if((follow_phase_para[i].operationType==1) &&(follow_phase_para[i].tailGreenCnt>0)) 
				{
					follow_phase_para[i].tailGreenCnt--;                //=1：跟随相位延迟放行，同时结束，=0跟随相位同时开始，延长结束。
				}
				else if((follow_phase_para[i].operationType==1)
					&&(follow_phase_para[i].tailGreenCnt==0))
				{
														
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //8_3=主相位个数
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] 跟随相位的主相位转为2个字节的位表示！
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //获取跟随相位对应的主相位号				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //跟随相位合到本阶段放行相位中
						//注意这有个+1是参考原程序上的，主相位当前计数器时间赋值跟随相位
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k] ;  	//跟随相位绿灯赋值
						channel_output_C1->stYellowCount[i] = channel_output_C1->stYellowCount[k] ;		        //跟随相位黄灯赋值
						channel_output_C1->stRedCount[i] 		= channel_output_C1->stRedCount[k];			         //跟随相位红灯赋值
						
					}
					*stage_followrelPhase &= ~(u16)(1<<i);         //已完成跟随相位处理
				}
				else if(follow_phase_para[i].operationType==0)                            //=0跟随相位同时开始，延长结束。
				{							
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //8_3=主相位个数
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] 跟随相位的主相位转为2个字节的位表示！
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //获取跟随相位对应的主相位号				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //跟随相位合到本阶段放行相位中
						//注意这有个+1是参考原程序上的，主相位当前计数器时间赋值跟随相位
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k]+follow_phase_para[i].tailGreenCnt ;  	//跟随相位绿灯赋值
						channel_output_C1->stYellowCount[i] =  follow_phase_para[i].tailYellowCnt ;		        //跟随相位黄灯赋值
						channel_output_C1->stRedCount[i] 		=  follow_phase_para[i].tailRedCnt;			         //跟随相位红灯赋值
					}
					*stage_followrelPhase &= ~(u16)(1<<i);         //已完成跟随相位处理	
				}						
				else if(follow_phase_para[i].operationType==2)                            //=2跟随相位同时开始，延长结束。
				{							
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //从0开始需要-1，8_3=主相位个数
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] 跟随相位的主相位转为2个字节的位表示！
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //获取跟随相位对应的主相位号				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //跟随相位合到本阶段放行相位中
						//注意这有个+1是参考原程序上的，主相位当前计数器时间赋值跟随相位
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k]+follow_phase_para[i].tailGreenCnt ;  	//跟随相位绿灯赋值
						channel_output_C1->stYellowCount[i] =  follow_phase_para[i].tailYellowCnt ;		        //跟随相位黄灯赋值
						channel_output_C1->stRedCount[i] 		=  follow_phase_para[i].tailRedCnt;			         //跟随相位红灯赋值
					}
					type2_follow_phase |= (u16) (1<<i );
					
					*stage_followrelPhase &= ~(u16)(1<<i);         //已完成跟随相位处理	
				}						
			
			}			
		}
	}	
}

U8 find_current_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U16 current_phase_run)
{
	U8 i;//j;
	
	for(i=0;i<PHASET_ARR_NO;i++)
	{
		if(current_phase_run == phase_order_t[i][1])
			return phase_order_t[i][0];
	}
	return 0xff;
}

U8 find_next_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO)
{
	if(current_phaseNO == PHASET_ARR_NO-1) return 0;
	if(phase_order_t[current_phaseNO+1][1] == 0) return 0;
	return (current_phaseNO+1);
}
extern u8 walker_green_flicker;
U8 is_current_phase_over(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO,STAGE_TIMING_COUNT_t *channel_output_C1)
{
	U8 next_phaseNO = 0U;
	U16 current_over_phase = 0U;
	U8 i;
	
	next_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
	/*延迟放行*/
	if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[current_phaseNO][1])
	{
		current_over_phase = phase_order_t[current_phaseNO][1];
		for(i=0;i<Channel_Max;i++)
		{
			if(((current_over_phase>>i)&1)==1)
			{
				if(channel_output_C1->stGreenCount[i] > walker_green_flicker*Int_Time_Per_Second)
				{
					return (__FALSE);
				}
			}
		}
		return (__TRUE);
	}
	else
	{
		current_over_phase = (phase_order_t[current_phaseNO][1] & (~phase_order_t[next_phaseNO][1]));
		
		for(i=0;i<Channel_Max;i++)
		{
			if(((current_over_phase>>i)&1)==1)
			{
				if((channel_output_C1->stGreenCount[i] > 1)
					||(channel_output_C1->stYellowCount[i] > 1)
				||(channel_output_C1->stRedCount[i] > 1))
				{
					return (__FALSE);
				}
			}
		}
		return (__TRUE);
	}
}

void tsk_watchdog_reload()
{
	static U8 tcp_cnt = 100;
	static u8 dog_tt_cont;  //看门狗计数器，延长看门狗时间 
	
		if(tsk_log_run == __TRUE)
		{
			if (tsk_log_over == __FALSE)
			{
				if(tcp_cnt >0)
				{
					tcp_cnt--;
				}
				else
				{
					printf("\ngfagadsgsdd");
					while(1);
				}
			}
			else    //__TRUE
			{
				tcp_cnt = 100;
				tsk_tcp_run=(__FALSE);
				tsk_tcp_over=(__FALSE);
			}
		}

	/*1.对于非定时任务，即日志、以太网应答、tcp任务，当进入任务时给运行信号xx_xx_run，
			当完成任务（任务出口处给完成信号xx_xx_over，若这两个信号相等，说明任务正常，可喂狗。
			若这些任务没运行，则run与over信号均为0，亦可喂狗
		2.对于ADC任务，学习任务，电源检测任务power_detect_task这三个定时启动的任务，
			只需要运行信号即可
	*/
// 	if(	((tsk_tcp_run^tsk_tcp_over) ==(__FALSE))//tcp_tsk
	if(	((tsk_log_run^tsk_log_over) ==(__FALSE))//log_write_task
		&&((tsk_DB_run^tsk_DB_over) ==(__FALSE))//eth_usart_communication_task
		&&((tsk_dispatch_run^tsk_dispatch_over) ==(__FALSE))//log_write_task
		&&((tsk_data_run^tsk_data_over) ==(__FALSE))//eth_usart_communication_task
		&&((tsk_execute_run^tsk_execute_over) ==(__FALSE))//log_write_task
		&&((tsk_CAN_run^tsk_CAN_over) ==(__FALSE))//eth_usart_communication_task
		&&(tsk_output_run ==(__TRUE)))     //ADC_task开始运行
		{
			dog_tt_cont=0;
			IWDG_ReloadCounter();
			GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	// 		tsk_tcp_run=(__FALSE);
	// 		tsk_tcp_over=(__FALSE);
			tsk_log_run=(__FALSE);
			tsk_log_over=(__FALSE);
			tsk_DB_run=(__FALSE);
			tsk_DB_over=(__FALSE);
			tsk_dispatch_run=(__FALSE);
			tsk_dispatch_over=(__FALSE);
			tsk_data_run=(__FALSE);
			tsk_data_over=(__FALSE);
			tsk_execute_run=(__FALSE);
			tsk_execute_over=(__FALSE);
			tsk_CAN_run=(__FALSE);
			tsk_CAN_over=(__FALSE);
			tsk_output_run=(__FALSE);
		}
		else if(dog_tt_cont<10)
		{
			dog_tt_cont++;
			IWDG_ReloadCounter();
			GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
		}
	
}
