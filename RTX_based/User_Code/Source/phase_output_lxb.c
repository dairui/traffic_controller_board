#include "Include.h"

extern STAGE_TIMING_COUNT_t		stage_timing_count;//�׶���ʱ�����ɫʱ��ת���ļ���ֵ
extern u8		Flashing_pulse_500ms; //500ms״ֵ̬��500ms��תһ��
extern u8		Flashing_pulse_1s;		//1000ms״ֵ̬��1000ms��תһ��
// // /*
// // ��λ��������	id=0x95
// // 16*12=192B

// // typedef struct PHASE_PARAM_S
// // {
// // 	US8 		phaseId;		//��λ��
// // 	US8 		greenSec;		//���˹����̵Ƶ���ɢ
// // 	US8 		cleanTime;	//�������ʱ��
// // 	US8		minGreenTime;//��С�̵�ʱ��
// // 	US8 		expandGreen;	//��λ�̵��ӳ�ʱ��
// // 	US8		maxGreen_1;	////����̵�ʱ��1
// // 	US8		maxGreen_2;	//����̵�ʱ��2
// // 	US8 		stadyGreen;		//������λ�̶��̵�ʱ��
// // 	US8		greenShine;		//����ʱ��
// // 	US8		phaseType;		//��λ����
// // 	US8		phaseFunc;		//��λѡ���
// // 	US8		reserved;			//��չ�ֶ�
// // }PHASE_PARAM_t;
// // */

// // /*
// // ��λ���״̬������ id=0x96
// // 4*2 = 8B
// // typedef struct PHASE_OUTPUT_STAT_S
// // {
// // 	US8		statId;				//��λ״̬����к�
// // 	US8		redStat;				//��λ�ĺ�����״̬
// // 	US8		yellowStat;		//��λ�ĻƵ����״̬
// // 	US8		greenStat;			//��λ���̵����״̬
// // }PHASE_OUTPUT_STAT_t;

// // */

// // /*
// // ��λ��ͻϮ����id=0x97
// // 16*3=48B
// // typedef struct PHASE_CONFLICT_S
// // {
// // 	US8		index;					//�к�
// // 	US16	phase;				//��ͻ��λ	
// // }PHASE_CONFLICT_t;
// // */
// // /*
// // ͨ�����״̬˥����id=0xb1
// // 2*4=8B
// // typedef struct CHANNEL_OUTPUT_STAT_S
// // {
// // 	US8		index;						//	�к�
// // 	US8		redStat;					//ͨ���ĺ�����״̬
// // 	US8		yellowStat;			//ͨ���ĻƵ����״̬
// // 	US8		greenStat;				//ͨ�����̵����״̬
// // }CHANNEL_OUTPUT_STAT_t;
// // */
// // /*
// // ͨ������˥����id=0xB0
// // 16*4=64B
// // typedef struct CHANNEL_PARAM_S
// // {
// // 	US8		chnId;					//	1 ͨ����
// // 	US8		signalSrc;				// 2 ͨ�����Ƶ��ź�Դ
// // 	US8		chnStat;					// 3 �� �� Automatic Flash����ģʽʱ��ͨ��״̬
// // 	US8		chnCtlType;			// 4 ͨ����������
// // }CHANNEL_PARAM_t;
// // */
// // /*
// // �׶���ʱ˥����id=0xC1
// // 16*16*8 = 2K
// // typedef struct STAGE_TIMING_TABLE_S
// // {
// // 	US8		index;					//	�׶���ʱ������
// // 	US8		stageId;					//�׶κ�
// // 	US16	relPhase;				//������λ2B
// // 	US8		stGreenTime;		//�׶��̵�ʱͬ
// // 	US8		stYellowTime;		//�׶λƵ�ʱ��
// // 	US8		stRedTime;			//�׶κ��ʱ��
// // 	US8		stAlterParam;		//�׶�ѡ�����
// // }STAGE_TIMING_TABLE_t; 
// // */

// // /*
// // 16ͨ�����״̬���id=0xb1�������ֵ��;
// // */
// // typedef struct CHANNEL_OUTPUT_TOB1_S
// // {
// // 	US8			index;							//	�к�
// // 	US16		redStat;					//16ͨ���ĺ�����״̬
// // 	US16		yellowStat;				//16ͨ���ĻƵ����״̬
// // 	US16		greenStat;				//16ͨ�����̵����״̬
// // }CHANNEL_OUTPUT_TOB1_T;
// // /*
// // �׶���ʱ��������id=0xC1ת������
// // 16*16*8 = 2K
// // */
// // typedef struct STAGE_TIMING_COUNT_S
// // {
// // 	US8			index;					//	�׶���ʱ������
// // 	US8			stageId;					//�׶κ�
// // 	US16		relPhase;				//������λ2B
// // 	US16		stGreenCount[16];		//�׶�16ͨ���̵Ƽ���
// // 	US16		stYellowCount[16];	//�׶�16ͨ���ƵƼ���
// // 	US16		stRedCount[16];			//�׶�16ͨ����Ƽ���
// // 	US8			stAlterParam;		//�׶�ѡ�����
// // }STAGE_TIMING_COUNT_t; 
// // /*
// // 16��λ���״̬������ id=0x96
// // 4*2 = 8B
// // */
// // typedef struct PHASE_OUTPUT_16STAT_S
// // {
// // 	US8		  statId;				//��λ״̬����к�
// // 	US16		redStat;				//��λ�ĺ�����״̬
// // 	US16		yellowStat;		//��λ�ĻƵ����״̬
// // 	US16		greenStat;			//��λ���̵����״̬
// // }PHASE_OUTPUT_16STAT_t;
/**
  * @brief  ͨ�������������   ������
  * @param  passage	ͨ����
  * @retval b1��16λͨ��״̬��������ͨ����״̬
	* @invovled function
  */
void  Passage_Red_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //�ص���������=0
{
		channel_output_temp->greenStat  &= ~(u16)(1<<(passage-1));
		channel_output_temp->yellowStat &= ~(u16)(1<<(passage-1));
		channel_output_temp->redStat    |=  (u16)(1<<(passage-1));
}
/**
  * @brief  ͨ�������������   ����Ƶ�
  * @param  passage	ͨ����
  * @retval b1��16λͨ��״̬��������ͨ����״̬
	* @invovled function
  */
void  Passage_Yellow_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //�ص���������=0
{
		channel_output_temp->greenStat  &= ~((u16)(1<<(passage-1)));
		channel_output_temp->yellowStat |=  (u16)(1<<(passage-1));
		channel_output_temp->redStat    &= ~((u16)(1<<(passage-1)));
}
/**
  * @brief  ͨ�������������   ����̵�
  * @param  passage	ͨ����
  * @retval b1��16λͨ��״̬��������ͨ����״̬
	* @invovled function
  */
void  Passage_Green_0pen_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //�ص���������=0
{
		channel_output_temp->greenStat  |=  (u16)(1<<(passage-1));
		channel_output_temp->yellowStat &= ~((u16)(1<<(passage-1)));
		channel_output_temp->redStat    &= ~((u16)(1<<(passage-1)));
}
/**
  * @brief  ͨ�������������   ������
  * @param  passage	ͨ����
  * @retval b1��16λͨ��״̬��������ͨ����״̬
	* @invovled function
  */
void  Passage_All_Close_Out(u8 passage,CHANNEL_OUTPUT_TOB1_t *channel_output_temp)  //�ص���������=0
{
		channel_output_temp->greenStat  &= (~(u16)(1<<(passage-1)));
		channel_output_temp->yellowStat &= (~(u16)(1<<(passage-1)));
		channel_output_temp->redStat    &= (~(u16)(1<<(passage-1)));
}

/**
  * @brief  ���ͨ�����״̬��CHANNEL_OUTPUT_TOB1_T Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1)
  *					ͨ������c1�������λ���ҵ�������е�ͨ������д��b1��
	*
	* @param  channel_output_C1 ʱ��ת��Ϊ����ֵ���c1��
  * @retval CHANNEL_OUTPUT_TOB1_T��b1��16λͨ��״̬��������ͨ����״̬
	* @invovled function Passage_All_Close_Out���ĸ�����
  */

U16	type2_follow_phase = 0;//���������͵ĸ�����λ
CHANNEL_OUTPUT_TOB1_t Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1,u16* channel_ability,u16* passage_walker)
{
			/*	
		���ݿ������Ҫ�ı���	
		*/
		u32										read_lenth_temp		=		0;	
		TARGET_DATA 					data_Target;	
				
		u8						i = 0;
		u8						B0_error_count	= 0;
	
		CHANNEL_PARAM_t 				tab_0xb0_temp[Channel_Max] = {0};				//b0��
		CHANNEL_OUTPUT_TOB1_t		channel_status_tob1		= {0};						//�����b1��
		PHASE_PARAM_t 					phase_par_temp;													//95��
		/*
		��0xB0��	
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
		/*	��ȡ��ͨ��ʹ��״̬ 3.24	*/
		for(i=0;i < Channel_Max;i++)
		{
			if(tab_0xb0_temp[i].signalSrc > 0)		/*	��ͨ����Ӧ����λ>0�����ͨ������	*/
			{
				*channel_ability |= (1<<(tab_0xb0_temp[i].chnId-1));
			}
			else																	/*	����ͨ��������	*/
			{
				*channel_ability &= ~(1<<(tab_0xb0_temp[i].chnId-1));
			}
			if(tab_0xb0_temp[i].chnCtlType==3)    //����ͨ�����淢����λ��
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
			if (tab_0xb0_temp[i].signalSrc == 0)							//ͨ��δ����,����
			{
				 if(Flashing_pulse_1s) 
					 Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  					//����Ϊ�ߣ��Ƶ������
				 else 
					 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);             //����Ϊ�ͣ��Ƶ������
			}
			else if((tab_0xb0_temp[i].chnStat & 0x06) >0)									//ͨ������ģʽ������
			{
				if((tab_0xb0_temp[i].chnStat & 0x04) >0)											//����ͨ������
				{
					 if(Flashing_pulse_1s) 
						 Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  					//����Ϊ�ߣ���������
					 else 
						 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1) ;          //����Ϊ�ͣ���������
				}					 
				else
				{
					 if(Flashing_pulse_1s) 
						 Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);  				//����Ϊ�ߣ��Ƶ������
					 else 
						 Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1) ;          //����Ϊ�ͣ��Ƶ������					
				}
			}
			else																														//ͨ������
			{
				if (((channel_output_C1->relPhase & (u16)(1<<(tab_0xb0_temp[i].signalSrc-1))) >0)//ͨ���ڷ��н׶�
				||(((type2_follow_phase>>i)&1)==1))
				{
					/*
					��0x95��	
					*/
					read_lenth_temp						=		0;
					data_Target.ID 						= 	0x95;
					data_Target.count_index 	= 	1;
					data_Target.sub_index[0] 	= 	i+1;			//��i+1����λ��Ӧ��95��
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
								&& (Flashing_pulse_500ms==0))      //�Ǹ�Ӧ�׶���������,�̵���
							{
								Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(	 ((channel_output_C1->stAlterParam & 0x01) == 0)
											&& ((channel_output_C1->stAlterParam & 0xe0) != 0) 
											&& (tab_0xb0_temp[i].chnCtlType!=3) 
											&& (((channel_output_C1->stAlterParam & 0xe0)>>3) >= channel_output_C1->stGreenCount[i])
											&& (Flashing_pulse_500ms==0))      //�Ǹ�Ӧ�׶λ���������,�̵���
							{
								Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(	 ((channel_output_C1->stAlterParam & 0x01) == 1)
											&& ( phase_par_temp.stadyGreen != 0) 
											&& (phase_par_temp.stadyGreen > channel_output_C1->stGreenCount[i])
											&& (Flashing_pulse_1s==0))     //��Ӧ�׶�����,�̵���
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
							if(tab_0xb0_temp[i].chnCtlType!=3)    			//���������Ƶ��������
							{
								Passage_Yellow_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
							else if(tab_0xb0_temp[i].chnCtlType==3)    //���˳����Ƶ��ڼ�������
							{
								Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
							}
					}
					else if(channel_output_C1->stRedCount[i] != 0)		 //������
					{
						Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
					}
					else																					//��Ϊ0�������
					{
						if(channel_output_C1->index != 27)
							Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
						else
							Passage_All_Close_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
					}
				}
				else   //ͨ�����ڷ��н׶�,ͨ��������
				{
					Passage_Red_0pen_Out(tab_0xb0_temp[i].chnId,&channel_status_tob1);
				}
			}
					
		}
		return channel_status_tob1;
}
/**
  * @brief  ���ͨ�����״̬��u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp)
  *					ͨ������c1�������λ,��д��16��λ��96��
	*
	* @param  channel_output_C1 ʱ��ת��Ϊ����ֵ���c1��
  * @retval phase_output_temp��96���16λͨ��״̬����������λ״̬;
	*					Phase_Run,ִ�������ֵ--����Ȼ���е���λ����Ϊ0�����ʾ�׶ν���
	* @invovled function 
  */
u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp)
{

	u8			phase_count = 0;	
	u16			Phase_Run   = 0xFFFF;

	for(phase_count=0;phase_count<16;phase_count++)  //ͨ��״̬ת��Ϊ��λ״̬��
	{
		if (1)//(channel_output_C1->relPhase & (u16)(1<<phase_count)) >0)//ͨ���ڷ��н׶�
		{
			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
			{
				phase_output_temp->greenStat |= ((u16)(1<<phase_count));  //�̵�״̬
			}			
			else if (channel_output_C1->stYellowCount[phase_count] > 1)
			{
				phase_output_temp->yellowStat |= ((u16)(1<<phase_count));  //�Ƶ�״̬
			}
			else if(channel_output_C1->stRedCount[phase_count] > 1)
			{
				phase_output_temp->redStat |= ((u16)(1<<phase_count));  		//���״̬
			}
			else
			{
				Phase_Run &= (~(u16)(1<<phase_count));
			}
		}
		else 
		{
			phase_output_temp->redStat |= ((u16)(1<<phase_count));  		  //�Ƿ�����λ���״̬
			Phase_Run &= (~(u16)(1<<phase_count));
		}
	}
	return Phase_Run;  //������λ���״̬��0x96	
}

u16 Phase_run_step(STAGE_TIMING_COUNT_t *channel_output_C1,U16 type2_follow)
{

	u8			phase_count = 0;	
	u16			Phase_Run   = 0xFFFF;

// 	if(type2_follow == 0)
	{
		for(phase_count=0;phase_count<16;phase_count++)  //ͨ��״̬ת��Ϊ��λ״̬��
		{
			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
			{
				;  //�̵�״̬
			}		
			else
			{
				Phase_Run &= (~(u16)(1<<phase_count));
			}
		}
	}
// 	else
// 	{
// 		for(phase_count=0;phase_count<16;phase_count++)  //ͨ��״̬ת��Ϊ��λ״̬��
// 		{
// 			if (channel_output_C1->stGreenCount[phase_count] > 1) 	  	
// 			{
// 				;  //�̵�״̬
// 			}			
// 			else if (channel_output_C1->stYellowCount[phase_count] > 1)
// 			{
// 				;  //�Ƶ�״̬
// 			}
// 			else if(channel_output_C1->stRedCount[phase_count] > 1)
// 			{
// 				;  		//���״̬
// 			}
// 			else
// 			{
// 				Phase_Run &= (~(u16)(1<<phase_count));
// 			}
// 		}
// 	}
	return Phase_Run;  //������λ���״̬��0x96	
}

FOLLOW_PHASE_WORKING_t follow_phase_para[C8_LINE]={0};
TARGET_DATA 		data_Target_follow;
U16 stage_follow_phase(STAGE_TIMING_COUNT_t *channel_output_C1)
{
	u16 stage_followrelPhase=0;    //���׶εĸ�����λ�ż���
	u16 mainphase=0;             //������λ������λ�ż���
	u8 i,follow_count,j;  //k,l,m;	
	
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32							read_lenth_temp				=	0;	
// 	TARGET_DATA 		data_Target;		
	
	for(follow_count=0;follow_count<8;follow_count++)    //follow_count=8��������λ��.
	{		
		/*
		������λ����˥����id=0xC8
		8*39 = 312B
		typedef struct FOLLOW_PHASE_PARAM_S
		{
			US8		phaseId;				//������λ���
			US8		operationType;	//��������
			US8		incPhaseLenth;	//������λ����
			US8		incPhase[16];		//������λ
			US8		fixPhaseLenth;		//������λ����
			US8		fixPhase[16];		//������λ
			US8		tailGreen;				//β���̵�
			US8		tailYellow;				//β���Ƶ�
			US8		tailRed;					//β�����
		}FOLLOW_PHASE_PARAM_t;
		*/

		//��ȡc8
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
			follow_phase_para[follow_count].phaseId   = 	data_Target_follow.sub_buf[0];				//������λ���
			follow_phase_para[follow_count].operationType   = 	data_Target_follow.sub_buf[1];	//��������
			follow_phase_para[follow_count].incPhaseLenth   = 	data_Target_follow.sub_buf[2];	//������λ����
			for(j=0;j<follow_phase_para[follow_count].incPhaseLenth;j++)
			{
				if(data_Target_follow.sub_buf[3+j]>0)
				{
					follow_phase_para[follow_count].incPhase[j]   = 	data_Target_follow.sub_buf[3+j]-1;		//������λ
				}
				else
				{
					follow_phase_para[follow_count].incPhase[j]   = 	data_Target_follow.sub_buf[3+j];		//������λ
				}
			}
// // 			follow_phase_para[follow_count].fixPhaseLenth	 	= 	data_Target_follow.sub_buf[19];		//������λ����
// // 			for(j=0;j<follow_phase_para[follow_count].fixPhaseLenth;j++)
// // 				follow_phase_para[follow_count].fixPhase[16]   = 	data_Target_follow.sub_buf[20+j];		//������λ
			follow_phase_para[follow_count].tailGreenCnt   = 	4*data_Target_follow.sub_buf[36];				//β���̵�
			follow_phase_para[follow_count].tailYellowCnt   = 	4*data_Target_follow.sub_buf[37];				//β���Ƶ�
			follow_phase_para[follow_count].tailRedCnt   = 	4*data_Target_follow.sub_buf[38];					//β�����
		}	
		if(follow_phase_para[follow_count].incPhaseLenth!=0)   //���ж��Ƿ����ø�����λ��
		{
			mainphase=0;
			for(i=0;i<follow_phase_para[follow_count].incPhaseLenth;i++)                     //8_3=����λ����
			{
				 mainphase |= (U16)((U16)1<< follow_phase_para[follow_count].incPhase[i]);             //c8_4[i] ������λ������λתΪ2���ֽڵ�λ��ʾ��
			}
			if((channel_output_C1->relPhase & mainphase)!=0 )        //��ǰ������λ���и�����λ������λ��
				stage_followrelPhase|=(U16)((U16)1<<(follow_count));    //�������׶θ�����λ�ţ�Ҫȫ������8�Ÿ�����λ��������Ч�ĸ�����λ
		}			
	}
	//��ȡc8���,�����˱��׶θ�����λ��
	return stage_followrelPhase;
}
void follow_phase_c8(STAGE_TIMING_COUNT_t *channel_output_C1,u16* stage_followrelPhase)
{
	u16 mainphase=0;             //������λ������λ�ż���
	u8 i,k,l;//m;	j,
	
	//**********************************************************//
	/*1���жϱ��׶��Ƿ��и�����λ*/
	/*2�������Ӷ�����������жϸ�����λ���ӳ����л����ӳٷ���*/
	/*3���ӳٷ��е�Ҫ�ȵ��ӳ�ʱ��������ڿ�ʼ�����̵����׶��̵�ͬʱ�������ӳ�ʱ���Ӷ���β���̵�ʱ��*/
	/*4���̵ƽ���ǰ��������Ҫ����16���׶���λ�������ֿ��ж�ʱ�䣡*/
	/*����˺�������250ms��ʱ�����ļ�����--��*/	
	if( *stage_followrelPhase!=0 )      //�и�����λ    
	{
		for(i = 0; i < C8_LINE; i++)    //8��������λ��������1-8����ȫ��16����λ��8����λ�У�������λ�ͷ�����λ�ںϣ����ÿ���ͨ����������ͨ���źͿ��Ƶ���λ
		{
			if((*stage_followrelPhase>>i )==1)			//���׶εĸ�����λ 2B
			{
				mainphase = 0;
					//c8_7count��ֵ��Ҫע�⸳ֵλ�ã����ǵ�ʱ���������							
				if((follow_phase_para[i].operationType==1) &&(follow_phase_para[i].tailGreenCnt>0)) 
				{
					follow_phase_para[i].tailGreenCnt--;                //=1��������λ�ӳٷ��У�ͬʱ������=0������λͬʱ��ʼ���ӳ�������
				}
				else if((follow_phase_para[i].operationType==1)
					&&(follow_phase_para[i].tailGreenCnt==0))
				{
														
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //8_3=����λ����
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] ������λ������λתΪ2���ֽڵ�λ��ʾ��
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //��ȡ������λ��Ӧ������λ��				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //������λ�ϵ����׶η�����λ��
						//ע�����и�+1�ǲο�ԭ�����ϵģ�����λ��ǰ������ʱ�丳ֵ������λ
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k] ;  	//������λ�̵Ƹ�ֵ
						channel_output_C1->stYellowCount[i] = channel_output_C1->stYellowCount[k] ;		        //������λ�ƵƸ�ֵ
						channel_output_C1->stRedCount[i] 		= channel_output_C1->stRedCount[k];			         //������λ��Ƹ�ֵ
						
					}
					*stage_followrelPhase &= ~(u16)(1<<i);         //����ɸ�����λ����
				}
				else if(follow_phase_para[i].operationType==0)                            //=0������λͬʱ��ʼ���ӳ�������
				{							
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //8_3=����λ����
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] ������λ������λתΪ2���ֽڵ�λ��ʾ��
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //��ȡ������λ��Ӧ������λ��				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //������λ�ϵ����׶η�����λ��
						//ע�����и�+1�ǲο�ԭ�����ϵģ�����λ��ǰ������ʱ�丳ֵ������λ
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k]+follow_phase_para[i].tailGreenCnt ;  	//������λ�̵Ƹ�ֵ
						channel_output_C1->stYellowCount[i] =  follow_phase_para[i].tailYellowCnt ;		        //������λ�ƵƸ�ֵ
						channel_output_C1->stRedCount[i] 		=  follow_phase_para[i].tailRedCnt;			         //������λ��Ƹ�ֵ
					}
					*stage_followrelPhase &= ~(u16)(1<<i);         //����ɸ�����λ����	
				}						
				else if(follow_phase_para[i].operationType==2)                            //=2������λͬʱ��ʼ���ӳ�������
				{							
					for(l=0;l<follow_phase_para[i].incPhaseLenth;l++)                     //��0��ʼ��Ҫ-1��8_3=����λ����
					{
						 mainphase |= (U16)((U16)1<< follow_phase_para[i].incPhase[l]);             //c8_4[i] ������λ������λתΪ2���ֽڵ�λ��ʾ��
					}
					mainphase &= channel_output_C1->relPhase;
					for(k=0;k<16;k++)
					{                          
						if(((mainphase>>k) & 1)==1 ) 
							break;             //��ȡ������λ��Ӧ������λ��				
					}
					if(k!=16)
					{
						channel_output_C1->relPhase |= (u16) (1<<i );            //������λ�ϵ����׶η�����λ��
						//ע�����и�+1�ǲο�ԭ�����ϵģ�����λ��ǰ������ʱ�丳ֵ������λ
						channel_output_C1->stGreenCount[i] 	= channel_output_C1->stGreenCount[k]+follow_phase_para[i].tailGreenCnt ;  	//������λ�̵Ƹ�ֵ
						channel_output_C1->stYellowCount[i] =  follow_phase_para[i].tailYellowCnt ;		        //������λ�ƵƸ�ֵ
						channel_output_C1->stRedCount[i] 		=  follow_phase_para[i].tailRedCnt;			         //������λ��Ƹ�ֵ
					}
					type2_follow_phase |= (u16) (1<<i );
					
					*stage_followrelPhase &= ~(u16)(1<<i);         //����ɸ�����λ����	
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
	/*�ӳٷ���*/
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
	static u8 dog_tt_cont;  //���Ź����������ӳ����Ź�ʱ�� 
	
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

	/*1.���ڷǶ�ʱ���񣬼���־����̫��Ӧ��tcp���񣬵���������ʱ�������ź�xx_xx_run��
			���������������ڴ�������ź�xx_xx_over�����������ź���ȣ�˵��������������ι����
			����Щ����û���У���run��over�źž�Ϊ0�����ι��
		2.����ADC����ѧϰ���񣬵�Դ�������power_detect_task��������ʱ����������
			ֻ��Ҫ�����źż���
	*/
// 	if(	((tsk_tcp_run^tsk_tcp_over) ==(__FALSE))//tcp_tsk
	if(	((tsk_log_run^tsk_log_over) ==(__FALSE))//log_write_task
		&&((tsk_DB_run^tsk_DB_over) ==(__FALSE))//eth_usart_communication_task
		&&((tsk_dispatch_run^tsk_dispatch_over) ==(__FALSE))//log_write_task
		&&((tsk_data_run^tsk_data_over) ==(__FALSE))//eth_usart_communication_task
		&&((tsk_execute_run^tsk_execute_over) ==(__FALSE))//log_write_task
		&&((tsk_CAN_run^tsk_CAN_over) ==(__FALSE))//eth_usart_communication_task
		&&(tsk_output_run ==(__TRUE)))     //ADC_task��ʼ����
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
