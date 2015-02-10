#include "Include.h"

u8	Sche_Dispatch_MSG	[5] = {0};					//��������������Ϣ
u8	Sche_Execute_MSG	[5] = {0};					//����ִ��������Ϣ
u8	Sche_Data_Update_MSG[5] = {0};				//������������
u8	Sche_Interim_MSG[5] = {0};						//���ɷ���

u8	Degrade_MSG	[5] = {0};								//����������Ϣ

OS_ID  	tmr1_yellow_flicker;
OS_MUT	Mutex_Data_Update;
OS_MUT	Tab_B1_Mutex;
OS_MUT	Output_Data_Mut;
OS_MUT 	RT_Operate_Mut;
OS_MUT 	I2c_Mut_lm;
OS_MUT 	Spi_Mut_lm;
OS_MUT 	Stage_Count_Mut; 			//���ڽ׶���ʱ�����ɫʱ�����ֵ BY lxb1.13

OS_SEM eth_client_sche_semaphore;
OS_SEM eth_send_data_semaphore;

os_mbx_declare(Sche_Output_Mbox, 			20);	
os_mbx_declare(Sche_Dispatch_Mbox, 		20);						//���� ����	����
os_mbx_declare(Sche_Data_Update_Mbox, 20);						//���� ���� ����
os_mbx_declare(Sche_Execute_Mbox, 		20);						//���� ִ�� ����
os_mbx_declare(Write_Log_Mbox, 				20);
os_mbx_declare(DB_Update_Mbox, 				20);
os_mbx_declare (mailbox_led, 20);//led_task

OS_TID t_maintask;									//��ʼ������
OS_TID t_DB_Update_Task;						//���ݿ��������
OS_TID t_Write_Log_Task;						//��¼�¼���־����
OS_TID t_Sche_Dispatch_Task;				//������������
OS_TID t_Sche_Data_Update_Task;			//������������
OS_TID t_Sche_Execute_Task;					//����ִ������
OS_TID t_Sche_Output_Task;					//�������(����������)  //��ǰ������  //by_lm   
// OS_TID t_Stagecount_Input_Task;			//����ֵ��ֵ����lxb
OS_TID t_tick_timer; 
OS_TID t_tcp_task; 
OS_TID t_task_rece_CAN;
OS_TID t_led;                           /* assigned task id of task: led     */
OS_TID t_matrix_keyboard;               /* assigned task id of task: matix_keyboard */

//  ջ������8�ֽ�Ϊ�߽���룬��U64��������(�޷��ŵĳ�����)��    
U64 DB_Update_Stack									[1000];
U64 Write_Log_Stack									[300];
U64 tcp_stack												[200];
U64 Sche_Dispatch_Stack							[400];			//���� ���� ����
U64 Sche_Data_Update_Stack					[600];			//���� ���� ����
U64 Sche_Execute_Stack							[400];			//���� ִ�� ����
U64 Sche_Output_Stack								[400];			//���� ִ�� ����
// U64 Stagecount_Input_Stack					[50];				//����������
U64 CAN_Rece_Stack									[200];			//���� ִ�� ����
U64 Led_Stack												[50];			//���� ִ�� ����
U8 soc_state_lm;

 U8 TT_cont1;    //���������ڷ�ֹ�����ط�
 U8 TT_cont2;
 U8 TT_cont3;
 U8 TT_cont4;



vu8 		Eth_client_tic_time_ovr=1; 

extern U16 read_PHY (U32 PhyReg); 
extern u8 DB_Read_M(TARGET_DATA *p_Target);  //  debug_lm
extern u8 DB_Write_M(TARGET_DATA *p_Target); //  debug_lm

u8 server_wait_ack =__TRUE; 
/*************callback function************/
U16 tcp_callback_server (U8 soc, U8 event, U8 *ptr, U16 par)   //This function is called on TCP event 
{
	u32 i = 0;
	//u32 receive_index = 0;

	switch (event) 
	{
	  case TCP_EVT_DATA:
      /* TCP data frame has arrived, data is located at *par1, */
					if(par > 0)
					{
							tsk_lock ();  			//  disables task switching 
							/* Read one Frame from the TCP Ethernet receive   */
							for(i = 0;i < par;i++)
							{
									eth_rx_fifo[rx_wr_index] = ptr[i];
									if (++rx_wr_index == RX_FIFO_SIZE) rx_wr_index=0;
							
									if (++rx_counter  == RX_FIFO_SIZE)
									{
										rx_counter=1;
										rx_buffer_overflow=__TRUE;
									};												
							}			
							soc_state_lm=__TRUE;

							tsk_unlock (); 			//  enable  task switching 	

					}					
      return (1);	
				
    case TCP_EVT_CONREQ:
      /* Remote peer requested connect, accept it */
      return (1);

    case TCP_EVT_CONNECT:
      /* The TCP socket is connected */
      return (1);	
    case TCP_EVT_ACK: 
				server_wait_ack	=__FALSE;
            break;		
		
	}  
	return (0);
}



U16 tcp_callback_client (U8 soc, U8 event, U8 *ptr, U16 par)   //This function is called on TCP event
{   
    switch (event) 
    {
        case TCP_EVT_CONNECT:
             cli_connect_ser_flag = 1;
				
            break;
        case TCP_EVT_ACK: 
						cli_connect_wait_ack=__FALSE;
            break;
        case TCP_EVT_CLOSE:
            break;
    }

    return (0);
}


extern  uint32_t	 IWDGRST_flag;		//	

#define SCHE_NUM_BKPRAM_ADDR  	(BKPSRAM_BASE)				/*	������������ ��BKPRAM�е���ʼ��ַ	*/  //	BKPRAM_LM
#define NOW_STAGE_BKPRAM_ADDR  	(BKPSRAM_BASE+50)    	/*	��ǰ�׶� ��BKPRAM�е���ʼ��ַ	*/			//	BKPRAM_LM
extern  void BKPRAM_Write(u8*p_Char,u8 lenth,uint32_t addr);				//	BKPRAM_LM
extern  void BKPRAM_Read(u8*p_Char,u8 lenth,uint32_t addr);					//	BKPRAM_LM

__task void maintask (void)
{
	u8 *msg_send;
	u8 send_temp[2];
	u8 send_temp1[2];

	os_mut_init (Mutex_Data_Update);
	os_mut_init (Tab_B1_Mutex);
	os_mut_init (Output_Data_Mut);
	os_mut_init (RT_Operate_Mut);
	os_mut_init (Stage_Count_Mut); 	
	os_mut_init (I2c_Mut_lm);  
	os_mut_init	(Spi_Mut_lm);	
	
	os_mbx_init (Sche_Output_Mbox, 					sizeof(Sche_Output_Mbox)					);
	os_mbx_init (Sche_Dispatch_Mbox, 				sizeof(Sche_Dispatch_Mbox)				);
	os_mbx_init (Sche_Data_Update_Mbox, 		sizeof(Sche_Data_Update_Mbox)			);
	os_mbx_init (Sche_Execute_Mbox, 				sizeof(Sche_Execute_Mbox)					);
	os_mbx_init (Write_Log_Mbox, 						sizeof(Write_Log_Mbox)						);
	os_mbx_init (DB_Update_Mbox, 						sizeof(DB_Update_Mbox)						);
	os_mbx_init (&mailbox_led, 							sizeof(mailbox_led));
	
//========================================
	os_sem_init	(eth_client_sche_semaphore,						0	);
	os_sem_init	(eth_send_data_semaphore,							0	);

	os_tsk_prio(t_maintask, 50);	
	
	t_Sche_Dispatch_Task 		= os_tsk_create_user(Sche_Dispatch_Task, 		10, &Sche_Dispatch_Stack, 		sizeof(Sche_Dispatch_Stack));
	t_Sche_Data_Update_Task = os_tsk_create_user(Sche_Data_Update_Task, 12, &Sche_Data_Update_Stack, 	sizeof(Sche_Data_Update_Stack));
	t_Sche_Execute_Task 		= os_tsk_create_user(Sche_Execute_Task, 		11, &Sche_Execute_Stack, 			sizeof(Sche_Execute_Stack));
// 	t_Stagecount_Input_Task = os_tsk_create_user(Stagecount_Input_Task, 12, &Stagecount_Input_Stack, 	sizeof(Stagecount_Input_Stack));
	t_Sche_Output_Task 			= os_tsk_create_user(Sche_Output_Task, 			20, &Sche_Output_Stack, 			sizeof(Sche_Output_Stack));
	t_DB_Update_Task 				= os_tsk_create_user(DB_Update_Task, 				19, &DB_Update_Stack, 				sizeof(DB_Update_Stack));
	t_Write_Log_Task 				= os_tsk_create_user(Write_Log_Task, 				8,  &Write_Log_Stack, 				sizeof(Write_Log_Stack));

	t_task_rece_CAN 				= os_tsk_create_user(task_rece_CAN, 				30,  &CAN_Rece_Stack, 				sizeof(CAN_Rece_Stack));
	
	t_led    								= os_tsk_create_user (led, 									29,  &Led_Stack, 				sizeof(Led_Stack));     	/* start task lcd                   */
	t_matrix_keyboard  			= os_tsk_create (matrix_keyboard, 28);     /* start task matrix_keyboard       */
	t_tick_timer 						= os_tsk_create(tick_timer, 								32	);  
	t_tcp_task 							= os_tsk_create_user (tcp_task, 						0, &tcp_stack, 								sizeof(tcp_stack));
	
	NVIC_configuration();

// // 	tmr1_yellow_flicker = os_tmr_create (ETH_CLIENT_TIC_TIMES, 1);    // ����һ����ʱ��

	//���ذ�С�ƿ���
	send_temp[0]=(uint8_t)RUN_LED;          			// LED���
	send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
	os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
	send_temp1[0]=(uint8_t)ERROR_LED;          			// LED���
	send_temp1[1]=(uint8_t)LED_OFF;												// ����״̬			
	os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

/*
  �������д������������ʹ�õ� os_mbx_send()����������ֻ�ǰ���Ϣ��ָ�����ʽ���͹�ȥ��
	Ҳ����˵����Ϣָ����ָ������ݾ�����Ϣ������������Ϣ�ĵ�ַ�������Ļ������ֻ�ܷ���4byte����Ϣ��

	����һ�����   �������������У��ȴ�Sche_Dispatch_Mbox�����Sche_Dispatch_Task()�����ȵõ�ִ�еġ�
	
*/
	if(IWDGRST_flag ==__TRUE)  			/*  ���Ź���λ	*/
	{
		Sche_Dispatch_MSG[0] = 4; 		/*	�������������еġ��������ڽ�����	*/
	}
	else 
	{
		Sche_Dispatch_MSG[0] = 1;			/*	�������������еġ��ϵ������	*/
	}

	msg_send = Sche_Dispatch_MSG;
	os_mbx_send(Sche_Dispatch_Mbox, (void *)msg_send, 0x10);

	os_tsk_delete_self ();
}
extern void tsk_watchdog_reload(void);
__task void tick_timer (void) //Timer tick every 100 ms
{
	os_itv_set (50); 
	while (1) 
	{
		
		os_itv_wait ();
	
		if(TT_cont1>0) TT_cont1--;   //���������� ��֤���ݶ�ʱ�����ظ�����
		if(TT_cont2>0) TT_cont2--;
		if(TT_cont3>0) TT_cont3--;
		if(TT_cont4>0) TT_cont4--;
		led_init();//��������PA8 pc9
		tsk_watchdog_reload();
		bFeed_Extern_Dog = __TRUE;
		timer_tick ();
	}
}

extern U8 own_hw_adr[];
extern U8 lhost_name[];
extern LOCALM localm[];

extern LOCALM  ip_config;

extern U8 mac_adr[];

__task void tcp_task (void)//main thread of the telnet,we send data here as a client terminal
{
	u32 i = 0;//,q;
	int regv=0;
	u8 *msg;

	u32 semaphore_one_ok=0;	
	u32 semaphore_thr_ok=0;
	
	U8 *sendbuf;  
	u32 sendbuf_len; //by csj
	u32 maxlen;	
	
	u32 tx_lenth_temp;	
	
	u8 send_temp[2] = {0};
	
  dhcp_tout = DHCP_TOUT;
	

	mem_copy (own_hw_adr, mac_adr, 6);

	
	init_TcpNet();
	if(RTH_link_ok_lm)
	{
		tcp_soc_server = tcp_get_socket (TCP_TYPE_SERVER, 0, 3, tcp_callback_server);	
		if (tcp_soc_server != 0) 
				tcp_listen (tcp_soc_server, local_port);
		
		tcp_soc_client = tcp_get_socket (TCP_TYPE_CLIENT,0,120,tcp_callback_client);
	}
	else 
	{
		while (1) 
			{
				regv = read_PHY (PHY_REG_STS);
				if (regv & 0x0001) 
				{
					/* Link is on. */
					RTH_link_ok_lm=1;
					init_TcpNet ();
					tcp_soc_server = tcp_get_socket (TCP_TYPE_SERVER, 0, 10, tcp_callback_server);
					if (tcp_soc_server != 0) 
							tcp_listen (tcp_soc_server, local_port);
					
					tcp_soc_client = tcp_get_socket (TCP_TYPE_CLIENT,0,120,tcp_callback_client);
					break;
				}
				os_sem_wait(eth_send_data_semaphore,0);  /*  ��������˸������ź�  */
				os_sem_wait(eth_client_sche_semaphore,0);
				os_dly_wait(100);
			}
	}

	dhcp_disable ();
	mem_copy (&localm[NETIF_ETH], &ip_config, sizeof(ip_config));
	
	
	while (1) 
	{
	
// 		#ifdef LIMING_DEBUG_12_23
// 			printf("-tcp-\n");
// 		#endif		
		tsk_tcp_run = (__TRUE);//ι��
		
		main_TcpNet();
		
	
		if(soc_state_lm == __TRUE)
		{
			soc_state_lm = __FALSE;
			
			Mbx_DB_Update[0] = 1;
			msg = Mbx_DB_Update;   
			os_mbx_send(DB_Update_Mbox, msg, 0x10);
		}		

		if(tx_buffer_overflow == __TRUE)
			tx_buffer_overflow == __FALSE;

		if(os_sem_wait(eth_send_data_semaphore,0) ==OS_R_OK )semaphore_one_ok +=1;
		if(semaphore_one_ok>0 )//�������˿���Ӧ��������
		{
			if (tcp_check_send (tcp_soc_server)) 
			{
				semaphore_one_ok  -=1;
				
// 				RECEIVE_LED_ON();   //liming
				send_temp[0]=(uint8_t)RECEIVE_LED;          			// LED���
				send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
				
				tsk_lock();           	 //  disables task switching 
				if(tx_len_counter !=0)    																 //�õ��������ݵĳ���
				{
					 tx_lenth_temp=eth_tx_len[tx_len_rd_index];
					 if (++tx_len_rd_index == TX_LEN_SIZE) tx_len_rd_index=0;
					 --tx_len_counter;
				}
				tsk_unlock();            //  enable  task switching 
		
				maxlen = tcp_max_dsize (tcp_soc_server);
				sendbuf = tcp_get_buf (maxlen);
				sendbuf[0]=0x21;		
				
				if(tx_lenth_temp > (maxlen -1))
				{
						tsk_lock();           	 //  disables task switching 
						if(tx_counter !=0)
						{
							for(i=0;i<(maxlen-1);i++)    
							{
								sendbuf[1+i] = eth_tx_fifo[tx_rd_index];
								if (++tx_rd_index == TX_FIFO_SIZE) tx_rd_index=0;
							}
							tx_counter -= (maxlen-1);   /*   ���  ETH_FIFO  */
						}
						tsk_unlock();            //  enable  task switching 				
						
						sendbuf_len = maxlen;
						tcp_send (tcp_soc_server,sendbuf,sendbuf_len);
						
						tx_lenth_temp -=(maxlen-1);
				}	
				else
				{
						tsk_lock();           	 //  disables task switching 
						if(tx_counter !=0)
						{
							for(i=0;i<tx_lenth_temp;i++)    
							{
								sendbuf[1+i] = eth_tx_fifo[tx_rd_index];
								if (++tx_rd_index == TX_FIFO_SIZE) tx_rd_index=0;
							}
							tx_counter -= tx_lenth_temp;   /*   ���  ETH_FIFO  */
						}
						tsk_unlock();            //  enable  task switching 				
						
						sendbuf_len = tx_lenth_temp+1;
						tcp_send (tcp_soc_server,sendbuf,sendbuf_len);
						
						tx_lenth_temp =0;
				}
				
				do
				{
					main_TcpNet();
					if (tcp_check_send (tcp_soc_server)) 
					{

						if(tx_lenth_temp > (maxlen-1) )
						{
								maxlen = tcp_max_dsize (tcp_soc_server);
								sendbuf = tcp_get_buf (maxlen);							
							
								tsk_lock();           	 //  disables task switching 
								if(tx_counter !=0)
								{
									for(i=0;i<(maxlen-1);i++)    
									{
										sendbuf[i] = eth_tx_fifo[tx_rd_index];
										if (++tx_rd_index == TX_FIFO_SIZE) tx_rd_index=0;
									}
									tx_counter -= (maxlen-1);   /*   ���  ETH_FIFO  */
								}
								tsk_unlock();            //  enable  task switching 				
								
								sendbuf_len = maxlen-1;
								tcp_send (tcp_soc_server,sendbuf,sendbuf_len);
								
								tx_lenth_temp -=(maxlen-1);
						}
						else if(tx_lenth_temp >0)
						{
								maxlen = tcp_max_dsize (tcp_soc_server);
								sendbuf = tcp_get_buf (maxlen);							
								tsk_lock();           	 //  disables task switching 
								if(tx_counter !=0)
								{
									for(i=0;i<tx_lenth_temp;i++)   
									{
										sendbuf[i] = eth_tx_fifo[tx_rd_index];
										if (++tx_rd_index == TX_FIFO_SIZE) tx_rd_index=0;
									}
									tx_counter -= tx_lenth_temp;   /*   ���  ETH_FIFO  */
								}
								tsk_unlock();            //  enable  task switching 				
								
								sendbuf_len = tx_lenth_temp;
								tcp_send (tcp_soc_server,sendbuf,sendbuf_len);
								
								tx_lenth_temp =0;
						}
					
				}
				

			}while(tx_lenth_temp !=0);
				
// 					RECEIVE_LED_OFF();    //  liming
				send_temp[0]=(uint8_t)RECEIVE_LED;          			// LED���
				send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			}
		}	
		
		//  �������������ݰ����Ͳ���-- ������ʱ��������  						
		if(Eth_client_tic_time_ovr==1)  
		{
			switch (tcp_get_state (tcp_soc_client)) //client launch a connet to the server 
			{
				case TCP_STATE_FREE:
				case TCP_STATE_CLOSED:
						 tcp_connect(tcp_soc_client, server_ip, server_port, 0);         
				case TCP_STATE_CONNECT:
								if (tcp_check_send (tcp_soc_client))
								{
									maxlen = tcp_max_dsize (tcp_soc_client);
									sendbuf = tcp_get_buf (maxlen);	
									sendbuf[0] = 0x21;									
									sendbuf[1] = 0x83;
									sendbuf[2] = 0x79;
									sendbuf[3] = 0;
									sendbuf[4] = 0;
									tcp_send (tcp_soc_client,sendbuf,5);

									Eth_client_tic_time_ovr=0;
// // // 									tcp_close(tcp_soc_client);	
			
								}
								//IWDG_ReloadCounter();   /* ι�� */  // IWDG_LM
						 break;
			}	//end of switch			

		}  //end of "if(Eth_client_tic_time_ovr ==1)"
		
		
		if(os_sem_wait(eth_client_sche_semaphore,0) ==OS_R_OK )semaphore_thr_ok +=1; 	//�ȴ��źŵĵ���
		if(semaphore_thr_ok>0 )	
		{
			switch (tcp_get_state (tcp_soc_client)) //client launch a connet to the server 
			{
				case TCP_STATE_FREE:
				case TCP_STATE_CLOSED:
						 tcp_connect(tcp_soc_client, server_ip, server_port, 0);         
				case TCP_STATE_CONNECT:
						if (tcp_check_send (tcp_soc_client))
						{
							maxlen = tcp_max_dsize (tcp_soc_client);
							sendbuf = tcp_get_buf (maxlen);

							sendbuf[0]=0x21;
							
							tsk_lock();           	 //  disables task switching 
							if(write_Client_counter !=0)//����ֻ��ֵһ��??
							{
								for(i=0;i<12;i++)    //  ���յ������е����ݿ�����sendbuf���棬
								{
									sendbuf[i+1] = write_Client_fifo[write_Client_rd_index][i];
								}
								if (++write_Client_rd_index == WRITE_Client_SIZE) write_Client_rd_index=0;
																
								--write_Client_counter;										
							}
							tsk_unlock();            //  enable  task switching 		
							
							tcp_send (tcp_soc_client,sendbuf,12+1);
							semaphore_thr_ok -=1;
			
// // // 							tcp_close(tcp_soc_client);
						}
						 break;
			}	//end of switch	
		
			if(semaphore_thr_ok>=WRITE_Client_SIZE)  //   ��̫��û�������ϣ���Ҫ�ϴ���������fifo���ˡ�
			{
				semaphore_thr_ok=1;
				write_Client_buffer_overflow=__FALSE;
			}
		}				
		tsk_tcp_over = (__TRUE);//ι��
		os_tsk_pass();				
  }
}


__task void led (void) 
{
	static Led_TypeDef 			leds =LED_NONE;  					//���ڽ�����Ϣ
	static Led_Run_TypeDef 	rec_run_type =LED_OFF;		//���ڽ�����Ϣ
	
	static Led_Run_TypeDef 	run_type[HAND_LED_NUM+LED_NUM];				//��˸����
//	static BOOL  						run_stats[HAND_LED_NUM+LED_NUM];      	//������
	
	u8 LED_Status_time = 0;
	u8 LED_Mark = 0;
	u8 Hand_LED_Mark = 0;
	
	uint8_t led_stats=0;	/* ���ڱ���ǰ��LED״̬ */
	uint16_t hand_led_stats=0;	/* �����ֶ����led��״̬ */
	
	uint16_t sd2_sd11_status = 0;//SDLED2~SDLED11�Ƶ����״̬(������ƻ����ԣ����������״̬)
	
	uint32_t   	i=0;
	uint8_t * 	receive=(uint8_t *)i;
	
  while(1)
	{
		os_mbx_wait (&mailbox_led, (void*)(&receive), 0xffff);
		// LED �����ʱ����100ms
		if(*receive>LED_NONE)
		{
			leds					=	(Led_TypeDef)			(receive[0]);
			rec_run_type	=	(Led_Run_TypeDef)	(receive[1]);
			
			if(leds ==LED_ALL)
			{
				for(i=0; i<LED_NUM; i++)
					run_type[i]	=rec_run_type;
			}
			else if((leds>SD_LED1)&&(leds<SD_LED12))
				//����SD_LED2~SD_LED11,������ϵ�״̬,����ֵ��hand_led_stats��Ӧλ
			{
				sd2_sd11_status = (uint16_t)(receive[2]|(receive[3]<<8));
				for(i=1;i<11;i++)
				{
					if(( (sd2_sd11_status>>i) & 1)==1) 
						run_type[LED_NUM+i-1+1]	=LED_ON;
					else
						run_type[LED_NUM+i-1+1]	=LED_OFF;
				}
			}				
			else
			{
				run_type[leds-1]	=rec_run_type;
			}
		}
		else
		{
			if (LED_Status_time<6) LED_Status_time++;  //��˸������
			else LED_Status_time=0;
			
			//------------------ǰ��led��ʾ----------------
			run_type[0] = LED_RUN;
			run_type[1] = LED_OFF;//�ݹر�
			run_type[2] = LED_OFF;//�ݹر�
			run_type[3] = LED_OFF;//�ݹر�
			run_type[4] = LED_OFF;//�ݹر�
			run_type[5] = LED_OFF;//�ݹر�
			run_type[7] = LED_OFF;//�ݹر�
			for(i=0; i<LED_NUM; i++)  	 
			{
				switch (run_type[i])
				{
					case LED_ON:	
						if((i==6)||(i==7)) led_stats |= (0x01<<i);
						else led_stats &= ~(0x01<<i);
						break;
					case LED_OFF:	
						if((i==6)||(i==7)) led_stats &= ~(0x01<<i);
						else led_stats |= (0x01<<i);
						break;		
					case LED_RUN:
							if(LED_Status_time==0) 
							{
								if((led_stats>>i &1)== 0)
								{
									led_stats |= (0x01<<i);
								}
								else 
								{
									led_stats &= ~(0x01<<i);
								}
							}
						break;
					case LED_FAST:
							if(LED_Status_time%2==0) 
							{
								led_stats |= (0x01<<i);
								
							}
							else 
							{
								led_stats &= ~(0x01<<i);
							}
						break;
					case LED_FAST_STOP:
						if (LED_Status_time==1)  LED_Mark=~LED_Mark; //��־ȡ��
						if(LED_Mark==0)
						{
							if(LED_Status_time%2==0) 
							{
								led_stats |= (0x01<<i);
								
							}
							else 
							{
								led_stats &= ~(0x01<<i);
							}
						}
						else if(LED_Mark==1)
						{//LED��
								led_stats &= ~(0x01<<i);
						}
						break;							
					default:
						break;
				}			

			}	
			output_595(led_stats);	
			
			//------------�ֶ����led��ʾ-----------------------
			for(i=0; i<HAND_LED_NUM; i++)  	 
			{
				switch (run_type[i+LED_NUM])
				{
					case LED_ON:	hand_led_stats |= (0x01<<i);
						break;
					case LED_OFF:	hand_led_stats &= ~(0x01<<i);
						break;		
					case LED_RUN:
							if(LED_Status_time==0) 
							{
								if((hand_led_stats>>i &1)== 0)
								{
									hand_led_stats |= (0x01<<i);
								}
								else 
								{
									hand_led_stats &= ~(0x01<<i);
								}
							}
						break;
					case LED_FAST:
							if(LED_Status_time%2==0) 
							{
								hand_led_stats |= (0x01<<i);
								
							}
							else 
							{
								hand_led_stats &= ~(0x01<<i);
							}
						break;
					case LED_FAST_STOP:
						if (LED_Status_time==1)  Hand_LED_Mark=~Hand_LED_Mark; //��־ȡ��
						if(Hand_LED_Mark==0)
						{
							if(LED_Status_time%2==0) 
							{
								hand_led_stats |= (0x01<<i);
								
							}
							else 
							{
								hand_led_stats &= ~(0x01<<i);
							}
						}
						else if(Hand_LED_Mark==1)
						{//LED��
								hand_led_stats &= ~(0x01<<i);
						}
						break;							
					default:
						break;
				}			

			}	
			SD_output_595(hand_led_stats);	
		}
  }
}

__task void matrix_keyboard (void) 
{
//	OS_RESULT result;
	unsigned char key_number = 0;
	
	static uint8_t send_temp[4];
	uint8_t the_key_number = 0;
	uint8_t count = 0;
	
	uint8_t  Enable_Keyboard_flag	= 0;	//ʹ���ֶ�����־��Ϊ1��ʾ��F8���������������Ч����֮��Ч
	
	u8 *msg_send;
	static u8 Uart_to_Sche_Dispatch_MSG[8] = {0};

	while(1)
	{
	//	ROWALL_OUTPUT();//��������У��ȴ��жϴ���
// // 		result = os_evt_wait_and (0x0001, 0xffff);
// // 		if (result == OS_R_TMO) {
// // 			printf("matrix_keyboard Event wait timeout.\n");
// // 		}
// // 		else 
		{
			ROWALL_CLOSE();
			os_dly_wait(10);	//10ms as base to scan keyboard
			key_number = key_ReadInputNumber();
			if(key_number > 0)
			{
				
				the_key_number = key_number;				
				while(1)
				{					
					os_dly_wait(10);	//10ms as base to scan keyboard
					key_number = key_ReadInputNumber();
					if(key_number == the_key_number)
					{
						count++;
						if(count >=4)	//ȥ��10+4*10 = 50ms
						{
							count = 0;
							
							if((the_key_number == 7)&&(Enable_Keyboard_flag==0))//ʹ�ܼ�F8���£�����
							{
								send_temp[0]=(uint8_t)HAND_CTRL_LED;          			// �ֶ����LED���
								send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							if(the_key_number == 3)//���������£�����
							{
								send_temp[0]=(uint8_t)NORMAL_CTRL_LED;          			// �ֶ����LED���
								send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							while(key_ReadInputNumber() == the_key_number)
							{
								os_dly_wait(10);//�ȴ�����
							}
							
							if((the_key_number == 7)&&(Enable_Keyboard_flag==0))//ʹ�ܼ�F8�������
							{
								send_temp[0]=(uint8_t)HAND_CTRL_LED;          			// �ֶ����LED���
								send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							if(the_key_number == 3)//�������������
							{
								send_temp[0]=(uint8_t)NORMAL_CTRL_LED;          			// �ֶ����LED���
								send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							//key pressed
							if(the_key_number == 7)						//ʹ�ܼ�F8���£����ʹ�ܼ��̱�־Enable_Keyboard_flag��1
							{
								Enable_Keyboard_flag = 1;//�ֶ�
							}
							else
							{
									if(Enable_Keyboard_flag == 1)		//ʹ�ܼ��̱�־Enable_Keyboard_flag=1����ʾ���̿ɲ���
									{
										switch(the_key_number)
										{
// // 											case 2:
// // 											{
// // 													break;
// // 											}
											case 1://ȫ��
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 3;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
												
// 												printf("ȫ��\n");
												break;
											}
// // 											case 4:
// // 											{
// // 													break;
// // 											}
// // 											case 5:
// // 											{
// // 													break;
// // 											}
											case 2://����
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 2;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
// 												printf("����\n");
												break;
											}
// // 											case 7:
// // 											{
// // 													break;
// // 											}
											case 6://����
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
												printf("����\n");
												break;
											}
											case 3://����
											{
												Enable_Keyboard_flag = 0;//ʹ�ܼ��̱�־Enable_Keyboard_flag��0
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 0;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
																	
												send_temp[0]=(uint8_t)HAND_CTRL_LED;  // �ֶ����LED���
												send_temp[1]=(uint8_t)LED_OFF;					// ����״̬			
												os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
												
// 												printf("����\n");
												break;
												//������ʱ��
											}
											default:
												break;
										}  
									}
							}							
							os_dly_wait(1000);
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				;//����
			}		
		}

	}

}

extern u8 	search_8D_all_timeTbId (time_struct *current_time,TIME_TABLE_t *table_0x8E_return);

/**
  * @brief  �ѷ�������Ʒ�ʽ����д����u8 Find_Scheme(u8 *sche_num_type)
  * @param  *sche_num_type,����Ϊ����������Ʒ�ʽ
  * @retval ���ص�u8����
	* @invovled function
	*					search_8D_all_timeTbId (Read_Time_Data,&table_0x8E_return)
	*					I2C_Read_DS3231 (Read_Time_Data)
	*					I2C_Write_Alarm(Alram_Time_Data)
  */

TIME_TABLE_t Find_Scheme(time_struct *current_time)
{
	u8					read_pre_flag;												//��ǰ�����õ����Ʒ�ʽ�������š����ӵı�־

	//���ڱ����8d8eȫ��������õ��Ŀ��Ʒ�ʽ���������Լ����ӵ�ʱ���
	TIME_TABLE_t table_0x8E_return = {0};	

	/*
	��8d8eȫ�����뵱ǰʱ�䣬�Լ����صĽṹ��table_0x8E_return�������ϼ����壩
	���ر�־˵����
	read_pre_flag == 0������û���ҵ��κη���������
	read_pre_flag == 1���������ҵ��˷����뵱�������
	read_pre_flag == 2���������ҵ��˷��������÷���Ϊ�������ʱ�εķ�������������Ӧ��дΪ��һ���0��
	*/
	read_pre_flag = search_8D_all_timeTbId (current_time,&table_0x8E_return);

	if(read_pre_flag == 0)
	{
	//��������
	table_0x8E_return.schemeId	= 	27;
	table_0x8E_return.ctlMode 	= 	0;
	return table_0x8E_return;
	}
	else 
	{
	//����������Ʒ�ʽ����
	table_0x8E_return.schemeId	= 	table_0x8E_return.schemeId;
	table_0x8E_return.ctlMode		= 	table_0x8E_return.ctlMode;
	//д����
	table_0x8E_return.minute		= 	table_0x8E_return.minute;
	table_0x8E_return.hour			= 	table_0x8E_return.hour;					
	return table_0x8E_return;
	}
}

/*˵��
*����������ĺ������ݼ�sche_change_temp[][]����Ĳ�������Ϊ�����֣�
*ǰ����Ϊ��sche_change_temp[][]������ݣ�Э�����������������䣬�����д�ͳһ������
 ��֪ͨ�������ݸ�������������ݣ�
*�󲿷�Ϊɨ��sche_change_temp[][]������䵱ǰ����sche_change_temp[NOW_C]��
 ��֪ͨ��CPUִ����ز����Լ�д��־��д��־Ҳ�ɷ���ǰ�벿�֣�
*�������Ϊ����Ϣ������֪ͨ�������ݸ��»���ִ������
*/
vu32 optimize_time_count = 0;     		//�Ż�ʱ��������
vu32 optimize_time_flag = 0;

vu8	red_to_auto						= __FALSE;		//����ȫ����л����������ֶο��ƣ�ֻ��һ��

vu8 start_yellow_red_flag     = __TRUE;			//��������ȫ���ʶ��ȫ�����0

/*���й��ϣ���һֱ����*/
u8 degrade_always_flag = 0;


/*��������ȫ���־��ʱ��*/
U8 start_yellow_flag = 0U;
U8 start_red_flag = 0U;
U8 start_yellow_time = 0U;
U8 start_red_time = 0U;

u8 phaseT_make = 0;

/* ����� */
U16 phase_order_t[PHASET_ARR_NO][5]= 
{
	/* �����     ������λ   ����ʱ��   ��Ӧ�׶κ�  ����ʱ�� */
	0
};
__task void Sche_Dispatch_Task(void)
{
	
	//���ڱ����8d8eȫ��������õ��Ŀ��Ʒ�ʽ���������Լ����ӵ�ʱ���
	TIME_TABLE_t table_0x8E_return = {0};	
	//��ǰʱ��ṹ��
	time_struct			current_time = {0};

	u8  i,j;
//	u8	k = 0;
	u8	*msg								= NULL;												//����ָ��
	u8	*msg_send						= NULL;
	u8	Message_Type 				= 0;		//��ʾ�����͵���������ͣ��緽�����»򷽰�ִ������// // 	u32	Sche_Index 					= 0;		//��Ϣ����
	u8	Flash_Time 					= 0;		//����ʱ��
	u8	Red_Time 						= 0;		//ȫ��ʱ��
	u8	Change_Flag 				= 0;		//�������ڽ������ı䷽����־�����ı䣬����Ϣ��������������
	u8 	Read_Time_Data[7] = {0};
	u8 	Alram_Time_Data[4] = {0};

	u32 									read_lenth_temp 	=		0;	
	TARGET_DATA 					data_Target;	

//  u8 Vice_Led_Ctrl[20] 					= {0};

/*��ʱ�������飬����������Ϣ���飬��ȫ�ֱ���sche_change_tpye[][]��Ӧ*/
	u8  sche_change_temp[][4] =
	{
	/* ����				��������		������			���Ʒ�ʽ*/
		{0,					0,					0,					0},   /*	��������			*/
		{1,					1,					0,					0},		/*	����ȫ��			*/
		
		{2,					2,					0,					0},		/*	����	0xBB		*/
		{3,					3,					0,					0},		/*	�ֶ�  0xB5 		*/
		{4,					4,					0,					0},		/*	ϵͳ  0xB6		*/
		{5,					5,					0,					0},		/*	��������			*/
		{6,					6,					0,					0},		/*	��������	*/
		{7,					0,					0,					0},		/*	��ǰ���Ʒ���	*/
	};

/*�������ͣ�1Ϊ������������	
	�������ͣ�2ΪЭ���������	
	�������ͣ�3Ϊ���Ӵ���	
	�������ͣ�4Ϊ�������ڽ�������	*/
	u8	trigger_type 					= 0;
/*���Ʒ�ʽ��1Ϊ������������	
	���Ʒ�ʽ��2ΪЭ���������	
	���Ʒ�ʽ��3Ϊ���Ӵ���	
	���Ʒ�ʽ��4Ϊ�������ڽ�������	*/	
	u8	rx_control_way 				= 0;
/*�����ţ�1Ϊ������������	
	�����ţ�2ΪЭ���������	
	�����ţ�3Ϊ���Ӵ���	
	�����ţ�4Ϊ�������ڽ�������	*/	
	u8	rx_sche_number 				= 0;	
	u8	start_red_enable			= 0;		//����ȫ��ʹ�ܱ�־��������������1�����Ӵ���ȫ�����0
	u8  sche_old_temp[4] 			=	{0};	//����ǰһ�����п��Ʒ������������
	
	u8 to_execute_temp[3];							//����ִ���������ݴ洢��ʱ����
	u8 to_update_temp[3]; 							//�������ݸ����������ݴ洢��ʱ����
//	u8 from_find_scheme[3];							//�����У��ҷ����õ��ķ���������0�Ϳ��Ʒ�ʽ����1

	u8	rx_con_type =0;
	
	u8  send_temp[4] = {0};
	u8  send_temp2[4] = {0};
	
	
	
	NVIC_InitTypeDef nvic_init;
	
	
	for(;;)
	{
		/*�ȴ���Ϣ������1Ϊ������������	,2ΪЭ�����������
										3Ϊ���Ӵ���,4Ϊ�������ڽ�������	*/
		os_mbx_wait(Sche_Dispatch_Mbox, (void *)&msg, 0xffff);
		#ifdef LIMING_DEBUG_12_23
			printf("-Dispatch-\n");
		#endif		
		
		/*��Ϣ��ʽ����һ�ֽ�Ϊ�������ͣ�2Ϊ���Ʒ�ʽ��3Ϊ������	*/
		trigger_type 	 = *msg;
		rx_control_way = *(++msg);
		rx_sche_number = *(++msg);
		rx_con_type    = *(++msg);

		tsk_dispatch_run= (__TRUE);//ι��
		
		/*���й��ϣ���һֱ����*/
		if(degrade_always_flag == 1)
		{
			trigger_type = 5;
		}
		
		/*�ñ���Ϊ0����ʾ�跢��Ϣ������ִ������Ϊ1��ʾ�����������ݸ�������*/
 		Message_Type = 0;
		/*����ϢԴ���������sche_change_temp[][]����	*/				
		switch (trigger_type)
		{
//========================================================================== (1)				
				case 1:										//�ϵ����
				{		
					
					read_lenth_temp 					=	0;
					data_Target.ID						=	0xA3;
					data_Target.count_index 	=	0;
					data_Target.sub_index[0]	=	0;
					data_Target.sub_index[1]	=	0;
					data_Target.sub_target		=	0;
					
					os_mut_wait(Spi_Mut_lm,0xFFFF);
					read_lenth_temp 					=		DB_Read_M(&data_Target);
					os_mut_release(Spi_Mut_lm);	
					
					if( read_lenth_temp 			!=	0	)	 //right
					{
						Flash_Time = data_Target.sub_buf[0]%10 + (data_Target.sub_buf[0]/10 << 4);
					}		
			
					if(Flash_Time < 0x0A)//���10��
					{
						Flash_Time = 0x0a;
					}
					start_yellow_flag = (__TRUE);
					start_yellow_time = Flash_Time;
					
					start_red_enable 		= 1;					//��������ȫ��
					sche_change_temp[STA_YELLOW][SCHE_CON_WAY]  = YELLOW_WAY;
					sche_change_temp[STA_YELLOW][SCHE_NUM] = 27;//��������������27
					sche_change_temp[NOW_C][SCHE_CON_WAY]  = sche_change_temp[STA_YELLOW][SCHE_CON_WAY];
					sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[STA_YELLOW][SCHE_NUM];
						
					/*���ͷ������ݸ�������	*/
					Message_Type = 1;//
					to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
					to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];
					/*֪ͨ��CPU	*/

					/*������־��*/
					tsk_lock ();  			//  disables task switching 
					write_log_wr_fifo[write_log_wr_index][0] = Type_A7;		
					write_log_wr_fifo[write_log_wr_index][1] = 0xA7;
					write_log_wr_fifo[write_log_wr_index][2] = 0;
					write_log_wr_fifo[write_log_wr_index][3] = 0;
					write_log_wr_fifo[write_log_wr_index][4] = 0x80;
					
					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;								
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);

					write_log_wr_fifo[write_log_wr_index][0] = Type_A6;
					write_log_wr_fifo[write_log_wr_index][1] = 0xA6;
					write_log_wr_fifo[write_log_wr_index][2] = 0x00;
					write_log_wr_fifo[write_log_wr_index][3] = 0;
					write_log_wr_fifo[write_log_wr_index][4] = 0x07;	
					
				if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;								
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
				}
					break;
//==========================================================================	(2)	
				case 2:										//Э�����
				{
					
						if(rx_control_way == 0)													//�ָ�����
						{
							if((sche_change_temp[NOW_C][SCHE_CON_WAY] >= OFF_WAY) && 
								 (sche_change_temp[NOW_C][SCHE_CON_WAY] <= RED_WAY))
							{//����ȫ����ƻָ�����
									switch (rx_con_type)
									{
										case 0xb5:
												{
													sche_change_temp[MANUAL_C][SCHE_CON_WAY] = 0;
													sche_change_temp[MANUAL_C][SCHE_NUM] = 0;
													sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;
													sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;

												}
											break;
										case 0xb6:
												{
													sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;
													sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;
												}
											break;
										default:
											break;
									}

								
								Message_Type = 2;
								to_execute_temp[0] = 6;//��Ϣֵ
								to_execute_temp[1] = 1;
							}
							else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == HOLD_WAY)//���ػָ�����
							{
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//����
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//���ط���
								
								Message_Type = 2;
								to_execute_temp[0] = 6;//��Ϣֵ
								to_execute_temp[1] = 2;
							}
							else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY)//�����ָ�����
							{
								sche_change_temp[BY_STEP][SCHE_CON_WAY] = 0;//����
								sche_change_temp[BY_STEP][SCHE_NUM] = 0;//��������
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//����
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//��������
								
								sche_change_temp[NOW_C][SCHE_CON_WAY] = 0;	//���㣬�Ա���һ�η�������
								sche_change_temp[NOW_C][SCHE_NUM] = 0;			
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = AUTO_C;

								send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // �ֶ����LED���
								send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
								send_temp[2]=(uint8_t)0x44;         	// SDLED2~SDLED11��8bit-���� �����ڵ�
								send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11��8bit-
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								
								Message_Type = 2;
								to_execute_temp[0] = 6;//��Ϣֵ
								to_execute_temp[1] = 3;
							}
							else if((sche_change_temp[NOW_C][SCHE_CON_WAY] == 8) || 
											(sche_change_temp[NOW_C][SCHE_CON_WAY] == OPTIMIZE_WAY))
							{
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//�Ż�
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//�Ż�����

								send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED���
								send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
							}
							else
							{
								
							}
							//д��־
							if(TT_cont1==0)
							{
								TT_cont1=5;
								tsk_lock ();  			//  disables task switching 
								write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
								write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
								write_log_wr_fifo[write_log_wr_index][2] = 0x00;
								write_log_wr_fifo[write_log_wr_index][3] = 0;
								write_log_wr_fifo[write_log_wr_index][4] = 0x05;							
								if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
															
								if (++write_log_counter  > WRITE_LOG_SIZE)
								{
									write_log_counter=WRITE_LOG_SIZE;
									write_log_buffer_overflow=__TRUE;
								};
								tsk_unlock (); 			//  enable  task switching 			
								os_mbx_send (Write_Log_Mbox, msg_send, 0xFFFF);
							}
							else TT_cont1=5;
						}
					//----------------------------------------------------------------
						else if((rx_control_way >= YELLOW_WAY) && 
										(rx_control_way <= RED_WAY))						//	���	����	ȫ�죨��ƶ���ȡ����
						{
							/*�˴�����ȫ����Դ����������Ϊ��̫������ϵͳ������������USART�ֶ�����
							*����ݲ�����
							*��ʱ��ϵͳ���������д�������lxb
							*/
							
							/*	ȫ�����������������ִ�в���ʱ�����κδ���  	*/
							if(sche_change_temp[NOW_C][SCHE_CON_WAY] != STEP_WAY)
							{
								switch (rx_con_type)
								{
									case 0xb5:
										{
												if(rx_control_way == YELLOW_WAY)//����
												{
													sche_change_temp[MANUAL_C][SCHE_CON_WAY] = rx_control_way;
													sche_change_temp[MANUAL_C][SCHE_NUM] = 27;						 

													//д��־
													tsk_lock ();  			//  disables task switching 		
													write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
													write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
													write_log_wr_fifo[write_log_wr_index][2] = 0x00;//
													write_log_wr_fifo[write_log_wr_index][3] = 0x04;//�ֶ������� 
													write_log_wr_fifo[write_log_wr_index][4] = 12;
				

												}
												if(rx_control_way == RED_WAY)//ȫ��
												{
													sche_change_temp[MANUAL_C][SCHE_CON_WAY] = rx_control_way;
													sche_change_temp[MANUAL_C][SCHE_NUM] = 28;
													//д��־
													tsk_lock ();  			//  disables task switching 		
													write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
													write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
													write_log_wr_fifo[write_log_wr_index][2] = 0x00;
													write_log_wr_fifo[write_log_wr_index][3] = 0x04;//�ֶ������� 
													write_log_wr_fifo[write_log_wr_index][4] = 13;
												}
										}										
										break;
									case 0xb6:
										{
											 if((sche_change_temp[MANUAL_C][SCHE_CON_WAY] ==YELLOW_WAY) ||
												  (sche_change_temp[MANUAL_C][SCHE_CON_WAY] ==RED_WAY)	) ;
											 else
											 {
														if(rx_control_way == YELLOW_WAY)//����
														{
															sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;
															sche_change_temp[SYSTEM_C][SCHE_NUM] = 27;						 

															//д��־
															tsk_lock ();  			//  disables task switching 
															write_log_wr_fifo[write_log_wr_index][0] =Sys_Ctrl_Type;		
															write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
															write_log_wr_fifo[write_log_wr_index][2] = 0;
															write_log_wr_fifo[write_log_wr_index][3] = 0x02;
															write_log_wr_fifo[write_log_wr_index][4] = 12;

														}
														if(rx_control_way == RED_WAY)//ȫ��
														{
															sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;
															sche_change_temp[SYSTEM_C][SCHE_NUM] = 28;
															//д��־
															tsk_lock ();  			//  disables task switching 
															write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;		
															write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
															write_log_wr_fifo[write_log_wr_index][2] = 0;
															write_log_wr_fifo[write_log_wr_index][3] = 0x02;
															write_log_wr_fifo[write_log_wr_index][4] = 13;//ȫ��

														}												 
												}
												
										}
										break;
									default:
										break;
								}

									
									Message_Type = 2;
									to_execute_temp[0] = 4;//��Ϣֵ
						
									if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
									if (++write_log_counter  > WRITE_LOG_SIZE)
									{
										write_log_counter=WRITE_LOG_SIZE;
										write_log_buffer_overflow=__TRUE;
									};
									tsk_unlock (); 			//  enable  task switching 			
									os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);

							}   //end of 
			
						}
					//----------------------------------------------------------------				
						else if(rx_control_way == HOLD_WAY)							// ����
						{
							if(	(sche_change_temp[NOW_C][SCHE_CON_WAY] == YELLOW_WAY) 	||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == RED_WAY) 			||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY) 		);
							else
							{
									sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;			//����
									
									if(rx_sche_number ==0)
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];					
									}
									else
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = rx_sche_number;					//��������lxb
									}

									
									Message_Type = 2;
									to_execute_temp[0] = 4;//��Ϣֵ

									//д��־
									tsk_lock ();  			//  disables task switching 			
									write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
									write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
									write_log_wr_fifo[write_log_wr_index][2] = 0x00;
									write_log_wr_fifo[write_log_wr_index][3] = 0x02;
									write_log_wr_fifo[write_log_wr_index][4] = 0x07;

												
									if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;							
									if (++write_log_counter  > WRITE_LOG_SIZE)
									{
										write_log_counter=WRITE_LOG_SIZE;
										write_log_buffer_overflow=__TRUE;
									};
									tsk_unlock (); 			//  enable  task switching 			
									os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
							}								
						
						}
					//----------------------------------------------------------------					
						else if(rx_control_way == STEP_WAY)							// ����
						{
							/*	������������lxbȫ��������ܲ���*/
							if((sche_change_temp[NOW_C][SCHE_CON_WAY] == 0) ||
								 (sche_change_temp[NOW_C][SCHE_CON_WAY] > 3))
							{
								sche_change_temp[BY_STEP][SCHE_CON_WAY] = rx_control_way;//����
								sche_change_temp[BY_STEP][SCHE_NUM] 		= sche_change_temp[NOW_C][SCHE_NUM];
								/*	
									����"����"û�������Ľ׶�����(�׶�ʱ��Ϊ���޳�)���ڴ�֮��Ҳ�Ͳ���
									���뵽"else if(trigger_type == 4)								//�������ڽ���"
									��ˣ��ڴ˰�"����"�����Ϳ��Ʒ�ʽ��ֵ��ͬʱ��
									ҲӦ�øı�"��ǰ"�������ͣ� "��ǰ"���Ʒ�ʽ�ͷ����Ÿ�ֵ

								*/
								sche_change_temp[NOW_C][SCHE_CON_TYPE]  	= BY_STEP ;
								//sche_change_temp[NOW_C][SCHE_NUM] 				= sche_change_temp[BY_STEP][SCHE_NUM];  //�����Բ���
								sche_change_temp[NOW_C][SCHE_CON_WAY] 		= sche_change_temp[BY_STEP][SCHE_CON_WAY];							
								
								send_temp[0]=(uint8_t)STEP_CTRL_LED;  // �ֶ����LED���
								send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
								send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11��8bit-
								send_temp[3]=(uint8_t)0x01;						// SDLED2~SDLED11��8bit-������
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // �ֶ����LED���
								send_temp2[1]=(uint8_t)LED_ON;					// ����״̬			
								os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
								
								Message_Type = 2;
								to_execute_temp[0] = 5;//��Ϣֵ
								

								//д��־
								tsk_lock ();  			//  disables task switching 		
								write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
								write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
								write_log_wr_fifo[write_log_wr_index][2] = 0x00;
								write_log_wr_fifo[write_log_wr_index][3] = 0x04;
								write_log_wr_fifo[write_log_wr_index][4] = 15;

											
								if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
															
								if (++write_log_counter  > WRITE_LOG_SIZE)
								{
									write_log_counter=WRITE_LOG_SIZE;
									write_log_buffer_overflow=__TRUE;
								};
								tsk_unlock (); 			//  enable  task switching 			
								os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);	
							}					
						}
					//----------------------------------------------------------------					
						else if((rx_control_way == 8) ||                //  liming_?   Ϊʲô�������������
										(rx_control_way == OPTIMIZE_WAY))				// �Ż��������Ż�û����
						{
							if(	(sche_change_temp[NOW_C][SCHE_CON_WAY] == YELLOW_WAY) 	||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == RED_WAY) 			||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == HOLD_WAY) 		||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY) 		);
							else 
							{
									sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;//�Ż�
									if (rx_sche_number != 0)					//�Ż�ָ������
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = rx_sche_number;
									}
									else															//�Ż��޷�������ֵ��ǰ����
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
									}
// // 									optimize_time_count = 0;   //������ʱ��
									
							}
																	
							//д��־
							tsk_lock ();  			//  disables task switching 			
							write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
							write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
							write_log_wr_fifo[write_log_wr_index][2] = 0x00;
							write_log_wr_fifo[write_log_wr_index][3] = 0x02;
							write_log_wr_fifo[write_log_wr_index][4] = 0x09;	
										
							if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
																
							if (++write_log_counter  > WRITE_LOG_SIZE)
							{
								write_log_counter=WRITE_LOG_SIZE;
								write_log_buffer_overflow=__TRUE;
							};
							tsk_unlock (); 			//  enable  task switching 			
							os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);

						}
				}
					break;
//==========================================================================	(3)		
				case 3:										//����
				{
					
					if(start_red_enable == 1)				//����ȫ�죬����������ʹ��ȫ�죬���Ӵ���
					{
						start_red_enable = 0;					//�رտ���ȫ��

						start_yellow_flag = (__FALSE);//�رտ�������
						
						read_lenth_temp 					=	0;
						data_Target.ID						=	0xA4;
						data_Target.count_index 	=	0;
						data_Target.sub_index[0]	=	0;
						data_Target.sub_index[1]	=	0;
						data_Target.sub_target		=	0;
						
						os_mut_wait(Spi_Mut_lm,0xFFFF);
						read_lenth_temp 					=		DB_Read_M(&data_Target);
						os_mut_release(Spi_Mut_lm);	
						
						if( read_lenth_temp 			!=	0	)	 //right
						{
							Red_Time = data_Target.sub_buf[0]%10 + (data_Target.sub_buf[0]/10 << 4);
						}		

						if(Red_Time < 0x05)//���6��
						{
							Red_Time = 0x05;
						}
						start_red_flag = (__TRUE);
						start_red_time = Red_Time;
										

						 /*  ��������������ķ���  */
						sche_change_temp[STA_YELLOW][SCHE_CON_WAY]  = YELLOW_WAY;
						sche_change_temp[STA_YELLOW][SCHE_NUM] = 0;//��������������27
						
						//��������ȫ�췽��
						sche_change_temp[STA_RED][SCHE_CON_WAY]  = RED_WAY;
						sche_change_temp[STA_RED][SCHE_NUM] = 28;//ȫ�췽����					
						sche_change_temp[NOW_C][SCHE_CON_WAY]  = sche_change_temp[STA_RED][SCHE_CON_WAY];
						sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[STA_RED][SCHE_NUM];
				
						//֪ͨ�������ݸ�������
						Message_Type = 1;
						to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];			
		// // // 				to_update_temp[2] = 88;			//Ӧ�ò���Ҫ���
						//д��־
						tsk_lock ();  			//  disables task switching	
						write_log_wr_fifo[write_log_wr_index][0] = Ctrl_Mode;
						write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
						write_log_wr_fifo[write_log_wr_index][2] = 0;
						write_log_wr_fifo[write_log_wr_index][3] = 0;
						write_log_wr_fifo[write_log_wr_index][4] = 13;			
									
						if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;													 
						if (++write_log_counter  > WRITE_LOG_SIZE)
						{
							write_log_counter=WRITE_LOG_SIZE;
							write_log_buffer_overflow=__TRUE;
						};
						tsk_unlock (); 			//  enable  task switching 			
						os_mbx_send (Write_Log_Mbox, msg, WRITE_LOG_WAIT_TIMES);
					}
					else														//���Ӵ���������ʱ�䵽�������·���
					{
						//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

						I2C_Clear_Alarm_Register();
						
						Exti_Configuration();
						
						NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
						nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn;
						nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:2
						nvic_init.NVIC_IRQChannelSubPriority = 2;
						nvic_init.NVIC_IRQChannelCmd = ENABLE;
						NVIC_Init(&nvic_init); 
	
						if(red_to_auto == __FALSE)//����ȫ����������Ӵ�����ʱ��
						{
							red_to_auto = __TRUE;						//���θ�if
							start_yellow_red_flag = (__FALSE);	//����ȫ�����
							 /*  ���������ȫ��ķ���  */
							sche_change_temp[STA_RED][SCHE_CON_WAY]  = RED_WAY;
							sche_change_temp[STA_RED][SCHE_NUM] = 0;//ȫ�췽����									
							
							/*	Ϊ����������׼��	*/
							sche_change_temp[BY_STEP][SCHE_CON_WAY] = 0;
							sche_change_temp[BY_STEP][SCHE_NUM] = 0;
							sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;
							sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;
							sche_change_temp[MANUAL_C][SCHE_CON_WAY] = 0;
							sche_change_temp[MANUAL_C][SCHE_NUM] = 0;		

						}
						
						/*����ö���Ҫ��׼�����ֲ�������	lxb*/
						//����ִ��ʱ����д
						os_mut_wait(I2c_Mut_lm,0xFFFF);
						I2C_Read_DS3231(Read_Time_Data);			//  I2C_lm   �д��Ķ���	
						os_mut_release(I2c_Mut_lm);	
						
						os_mut_wait (Mutex_Data_Update, 0xffff);				
						Plan_Used.Plane_Execute_Time[0] = Read_Time_Data[2];//Сʱ
						Plan_Used.Plane_Execute_Time[1] = Read_Time_Data[1];//����
						Plan_Used.Plane_Execute_Time[2] = Read_Time_Data[0];//����
						os_mut_release (Mutex_Data_Update);	  
						
						
						os_mut_wait(I2c_Mut_lm,0xFFFF);
						I2C_Read_DS3231(Read_Time_Data);
						os_mut_release(I2c_Mut_lm);	
						
						//���沿�֣��������������Ժ�ı��ʱ�Ӻ�������
						current_time.second =	Read_Time_Data[0];
						current_time.minute =	Read_Time_Data[1];
						current_time.hour 	=	Read_Time_Data[2];
						current_time.week	  =	Read_Time_Data[3];
						current_time.day 		=	Read_Time_Data[4];
						current_time.month 	=	Read_Time_Data[5];
						current_time.year 	=	Read_Time_Data[6];

						
						table_0x8E_return = Find_Scheme(&current_time);//�鵽��ǰ���Ʒ�ʽ�ͷ����ţ���д���¸�ʱ�ε�����
						/*��ֵ��һ�׶η����źͿ��Ʒ�ʽ���������ƣ�lxb	*/
						sche_change_temp[AUTO_C][SCHE_CON_WAY] = table_0x8E_return.ctlMode;
						sche_change_temp[AUTO_C][SCHE_NUM] = table_0x8E_return.schemeId;
						//д����
						Alram_Time_Data[0] 			= 	0;
						Alram_Time_Data[1] 			= 	table_0x8E_return.minute/10*16 + table_0x8E_return.minute%10;
						Alram_Time_Data[2] 			= 	table_0x8E_return.hour/10*16 + table_0x8E_return.hour%10;
						Alram_Time_Data[3] 			= 	0x80;

						os_mut_wait(I2c_Mut_lm,0xFFFF);   	//  I2C_lm
						I2C_Write_Alarm(Alram_Time_Data);		//����ʱ��д��ʱ��оƬ���ӼĴ���   
						I2C_Write_Alarm_Register(0x05);			//ʹ��ʱ��оƬ�����ж����		 
						os_mut_release(I2c_Mut_lm);	


						send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED���
						send_temp[1]=(uint8_t)LED_RUN;												// ����״̬
						
						os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
					} 

				}  /*  end of "else if(Trigger_Type == 3)" */
					break;
//==========================================================================	(4)			
				case 4:										//�������ڽ���
				{
					
					if(IWDGRST_flag ==__TRUE)  			/*  ���Ź���λ	*/
					{
						/*  �Ӻ����ж�����������	*/
						BKPRAM_Read((u8*)sche_change_temp,sizeof(sche_change_temp),SCHE_NUM_BKPRAM_ADDR);

						//�Ѳ�Ϊ0�ķ�����������ͼ����Ʒ�ʽ������ǰ����
						sche_change_temp[NOW_C][SCHE_CON_TYPE] 		= 0;
						sche_change_temp[NOW_C][SCHE_NUM] 				= 0;
						sche_change_temp[NOW_C][SCHE_CON_WAY] 		= 0;
						
						for(i=0;i<8;i++)
						{
							printf ( "\n" ); 
							printf ( "%d," ,sche_change_temp[i][SCHE_CON_TYPE]); 
							printf ( "%d," ,sche_change_temp[i][SCHE_NUM]);
							printf ( "%d;" ,sche_change_temp[i][SCHE_CON_WAY]);
							
						}
						printf ( "\n=======================\n" );
					
						
// // SCHE_CON_TYPE		1				//��������������
// // SCHE_NUM					2				//������������
// // SCHE_CON_WAY		3				//���Ʒ�ʽ������
					}			
					/*   ɨ������ sche_change_temp���������͵ķ�����   */
					for (i =STA_YELLOW;i<= AUTO_C;i++)   /*    0 ----- 5   */
					{
						if (i == BY_STEP)									//����ɨ����Ʒ�ʽ
						{
							if (sche_change_temp[i][SCHE_CON_WAY] == STEP_WAY)
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//�Ѳ�Ϊ0�ķ�����������ͼ����Ʒ�ʽ������ǰ����
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = i;
								sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
											
								break;
							}
						}
						else																	//��������ɨ�跽����
						{
							if (sche_change_temp[i][SCHE_NUM] != 0)
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//�Ѳ�Ϊ0�ķ�����������ͼ����Ʒ�ʽ������ǰ����
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = i;
								sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
											
								break;
							}
							else if (i == AUTO_C)						//�������ֶ���ϵͳ���ƶ�Ϊ�գ���Ϊ��������
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//�Ѳ�Ϊ0�ķ�����������ͼ����Ʒ�ʽ������ǰ����
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = i;
								sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];					
							}
							else
							{
							}
						}
					}
					switch (sche_change_temp[NOW_C][SCHE_CON_TYPE])
					{
						case STA_YELLOW:				//�Ƿ�Ϊ����
							{
								Change_Flag=0;							//���������������·�������
							}
							break;	
						case STA_RED:				//�Ƿ�Ϊȫ��
							{
								Change_Flag=0;							//����ȫ�죬�����·�������
							}
							break;									
						case BY_STEP:						//�Ƿ�Ϊ����
							{
								Change_Flag=0;						//���������������·�������
							}
							break;
						case MANUAL_C:					//�Ƿ�Ϊ�ֶ����ƣ��ֶ�����ȫ����ƻ���
							{
								if(sche_old_temp[SCHE_CON_WAY] != 
									 sche_change_temp[NOW_C][SCHE_CON_WAY])
								{//���������еķ�������һ������һ�£�������Ϣ���������ݸ�������
									Change_Flag=1;
								}
								else
								{//���������еķ�������һ����һ�£���ֱ��֪ͨ����ִ������
									Change_Flag=0;
								}
							}
							break;
						case SYSTEM_C:					//�Ƿ�Ϊϵͳ����
							{
								switch (sche_change_temp[SYSTEM_C][SCHE_CON_WAY])
								{
									case YELLOW_WAY:
									case RED_WAY:
										{
											/*���������еķ�������һ������һ�£�������Ϣ���������ݸ�������
											һ����֪ͨ����ִ������*/
											if(sche_old_temp[SCHE_CON_WAY] != 
												 sche_change_temp[NOW_C][SCHE_CON_WAY])
											{//����ȫ��
												Change_Flag=1;
											}
											else
											{
												Change_Flag=0;
											}
										}
										break;
									case HOLD_WAY:
										{
											//Change_Flag=0;
											sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;  //by_liming
											sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;


												Change_Flag=1;	

										}
										break;
									case OPTIMIZE_WAY:
										{									//�Ż�
											optimize_time_flag = 1;
//											optimize_time_count = 0;   //������ʱ��
											Change_Flag=1;
										}			
								
										break;
									default:
										break;
								}
							}
							break;
						case AUTO_C:						//�Ƿ�Ϊ��������/*ϵͳ���ơ��ֶ�����û����Ч���ݣ�����Ҷ�ʱ������*/
							{
								Change_Flag = 0;
								if(change_8D_8E_C0_C1_flag ==1)
								{
									change_8D_8E_C0_C1_flag=0;
									phaseT_make = __TRUE;
									Change_Flag = 1;
									break;
								}
								
								if((sche_old_temp[SCHE_NUM] != 
										sche_change_temp[NOW_C][SCHE_NUM]) 
									&& (sche_change_temp[NOW_C][SCHE_NUM] != 0))//�����б�
								{
									Change_Flag = 1;
								}
								if((sche_old_temp[SCHE_CON_WAY] != 
										sche_change_temp[NOW_C][SCHE_CON_WAY]) && 
									 (sche_change_temp[NOW_C][SCHE_CON_WAY] != 0))//���Ʒ�ʽ�б�
								{
									Change_Flag = 1;
								}
								if((sche_old_temp[SCHE_CON_TYPE] != 
										sche_change_temp[NOW_C][SCHE_CON_TYPE]) )//���������б�
								{
									Change_Flag = 1;
								}	

							}
							break;
						default:
							break;
					}
					
					if(Change_Flag == 0)								//֪ͨ����ִ������ִ�е�ǰ����
					{
						Message_Type = 2;
						to_execute_temp[0] = 3;//��ϢֵΪ3
						to_execute_temp[1] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_execute_temp[2] = 0;
						if(sche_change_temp[NOW_C][SCHE_CON_TYPE] == HOLD_WAY)
						{
							/*
							Ŀǰ�ķ���ִ�����񲢲����շ�����,
							�����to_execute_temp[2]�������       //liming_?
							*/
							os_mut_wait(RT_Operate_Mut,0xFFFF);   
							to_execute_temp[2] = sche_hold_guand.index;//��Ҫ���������ǽ׶���ʱ������ 
							os_mut_release(RT_Operate_Mut);		
						}
					}
					else																//֪ͨ�������ݸ������񣬸��·�������
					{
						if((sche_change_temp[NOW_C][SCHE_NUM]!=27)&&(sche_change_temp[NOW_C][SCHE_NUM]!=28))
						{
							if(sche_change_temp[NOW_C][SCHE_CON_WAY] == AUTO_WAY)
							{
								phaseT_make = __TRUE;
								for(i=0;i<PHASET_ARR_NO;i++)
								{
									for(j=0;j<5;j++)
									phase_order_t[i][j]=0;
								}
							}
						}
						
						Message_Type = 1;//
						to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];
					}
					
					//����led����--������SD2~SD11
					if(((sche_change_temp[NOW_C][SCHE_CON_WAY] != 1) && (sche_change_temp[NOW_C][SCHE_CON_WAY] <= 6)) 
						|| (sche_change_temp[NOW_C][SCHE_CON_WAY] == 8) || (sche_change_temp[NOW_C][SCHE_CON_WAY] == 12))
					{	
						
						switch (sche_change_temp[NOW_C][SCHE_CON_TYPE])
						{
							case STA_YELLOW:						//��������
								{	
									send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // �ֶ����LED���
									send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
									send_temp[2]=(uint8_t)0x04;         	// SDLED2~SDLED11��8bit-������
									send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11��8bit-�����
									os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								}
								break;
							case STA_RED:						//�Ƿ�Ϊȫ��
								{
									send_temp[0]=(uint8_t)RED_CTRL_LED;  // �ֶ����LED���
									send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
									send_temp[2]=(uint8_t)0x04;         	// SDLED2~SDLED11��8bit-������
									send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11��8bit-�����
									os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								}
								break;
// // // 							case BY_STEP:						//�Ƿ�Ϊ����
// // // 								{
// // // 									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 5)//����
// // // 									{					
// // // 										send_temp[0]=(uint8_t)STEP_CTRL_LED;  // �ֶ����LED���
// // // 										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
// // // 										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11��8bit-
// // // 										send_temp[3]=(uint8_t)0x01;						// SDLED2~SDLED11��8bit-������
// // // 										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
// // // 										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // �ֶ����LED���
// // // 										send_temp2[1]=(uint8_t)LED_ON;					// ����״̬			
// // // 										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
// // // 									}	
// // // 									else
// // // 									{
// // // 									}
// // // 								}
// // // 								break;
							case MANUAL_C:					//�Ƿ�Ϊ�ֶ�����
								{
									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 2)//�ֶ�����
									{				
										send_temp[0]=(uint8_t)YELLOW_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11��8bit-
										send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11��8bit-������
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // �ֶ����LED���
										send_temp2[1]=(uint8_t)LED_ON;					// ����״̬			
										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 3)//�ֶ�ȫ��
									{							
										send_temp[0]=(uint8_t)RED_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11��8bit-
										send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11��8bit-������
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // �ֶ����LED���
										send_temp2[1]=(uint8_t)LED_ON;					// ����״̬			
										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
									}
									else
									{
									}
								}
								break;
							case SYSTEM_C:					//�Ƿ�Ϊϵͳ����
								{
									if(sche_change_temp[NOW_C][SCHE_NUM] == 27)//ϵͳ����
									{						
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x02;         	// SDLED2~SDLED11��8bit-ϵͳ��
										send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11��8bit-�����
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_NUM] == 28)//ϵͳȫ��
									{				
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x02;         	// SDLED2~SDLED11��8bit-ϵͳ��
										send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11��8bit-ȫ���
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 4)//ϵͳ����
									{	
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x82;         	// SDLED2~SDLED11��8bit-ϵͳ�� ���ص�
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11��8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 12)//ϵͳ�Ż�
									{		
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x12;         	// SDLED2~SDLED11��8bit-ϵͳ�� �Ż���
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11��8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else
									{
									}
								}
								break;
							case AUTO_C:						//�Ƿ�Ϊ��������/*ϵͳ���ơ��ֶ�����û����Ч���ݣ�����Ҷ�ʱ������*/
								{
									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 6)//������Ӧ
									{	
										send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x24;         	// SDLED2~SDLED11��8bit-������ ��Ӧ��
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11��8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else //��������
									{			
										send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // �ֶ����LED���
										send_temp[1]=(uint8_t)LED_ON;					// ����״̬			
										send_temp[2]=(uint8_t)0x44;         	// SDLED2~SDLED11��8bit-������ �����ڵ�
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11��8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);	
									  if(TT_cont2==0)
										{
											TT_cont2=5;
											tsk_lock ();  			//  disables task switching 
											write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
											write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
											write_log_wr_fifo[write_log_wr_index][2] = 0x00;
											write_log_wr_fifo[write_log_wr_index][3] = 0;
											write_log_wr_fifo[write_log_wr_index][4] = 0x05;		  //������ʱ�ο���		
											if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;																	
											if (++write_log_counter  > WRITE_LOG_SIZE)
											{
												write_log_counter=WRITE_LOG_SIZE;
												write_log_buffer_overflow=__TRUE;
											};
											tsk_unlock (); 			//  enable  task switching 			
											os_mbx_send (Write_Log_Mbox, msg_send, 0xFFFF);
										}
										else (TT_cont2=5);
									}
								}
								break;
							default:
								break;
						}
					}	
					
					/*  �ѷ�������д�����	*/
					BKPRAM_Write((u8*)sche_change_temp,sizeof(sche_change_temp),SCHE_NUM_BKPRAM_ADDR);
				}   /*  end of "else if(Trigger_Type == 4)" */
				
					break;
//==========================================================================	(5)			
				case 5:										//����
				{
					switch (sche_change_temp[NOW_C][SCHE_CON_TYPE])
					{
						case STA_YELLOW:		
							{
								Change_Flag=0;							//����������������
							}
							break;	
						case STA_RED:						
						case BY_STEP:					
						case MANUAL_C:							
						case SYSTEM_C:								
							{
								/*   
									�����"ȫ��"��"����"��"�ֶ�"��"ϵͳ"�����ڼ���Ҫ����,
									�������Ӧ�ķ����� ��
									����Ϊ�������ơ�
								*/
								for (i =STA_YELLOW;i< AUTO_C;i++)   /*    0 ----- 4   */
								{						
									sche_change_temp[i][SCHE_NUM] = 0;
								}
								
								sche_change_temp[NOW_C][SCHE_CON_TYPE]	=sche_change_temp[AUTO_C][SCHE_CON_TYPE];
								sche_change_temp[NOW_C][SCHE_NUM]				=sche_change_temp[AUTO_C][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY]		=sche_change_temp[AUTO_C][SCHE_CON_WAY];

								Change_Flag=1;							//���·�������
							}
							break;		
						case AUTO_C:	
						case YELLOW_C:					
							{
// 								/*   
// 									�����"��������"�ڼ䷢���˽�����
// 									1�������Ų�Ϊ0���򽵼�Ϊ0�ŷ��������Ʒ�ʽ����
// 									2��������Ϊ0	���򽵼�Ϊ��������
// 								*/						
// 								if(sche_change_temp[NOW_C][SCHE_NUM] !=0)
// 								{
// 									sche_change_temp[AUTO_C][SCHE_CON_TYPE]	=AUTO_C;
// 									sche_change_temp[AUTO_C][SCHE_NUM]			=0; 
// 									//sche_change_temp[AUTO_C][SCHE_CON_WAY]	=sche_old_temp[SCHE_CON_WAY];		//�����Բ���д					
// 									
// 									sche_change_temp[NOW_C][SCHE_CON_TYPE]	=AUTO_C;
// 									sche_change_temp[NOW_C][SCHE_NUM]				=0; 
// 									sche_change_temp[NOW_C][SCHE_CON_WAY]		=sche_change_temp[AUTO_C][SCHE_CON_WAY];
// 									Change_Flag = 1;
// 									
// 									
// 									GPIO_SetBits(GPIOD,GPIO_Pin_15);//   debug_lm
// 									
// 								}
// 								else 
// 								{
// 									sche_change_temp[NOW_C][SCHE_CON_TYPE]	=YELLOW_C;
// 									sche_change_temp[NOW_C][SCHE_NUM]				=27; 
// 									sche_change_temp[NOW_C][SCHE_CON_WAY]		=YELLOW_WAY;
// 									Change_Flag = 1;
// 								}
									sche_change_temp[NOW_C][SCHE_CON_TYPE]	=YELLOW_C;
									sche_change_temp[NOW_C][SCHE_NUM]				=27; 
									sche_change_temp[NOW_C][SCHE_CON_WAY]		=YELLOW_WAY;
									Change_Flag = 1;								
								
												
							}
							break;
						default:
							
						
							break;
					}
					
					if(Change_Flag == 0)								//֪ͨ����ִ������ִ�е�ǰ����
					{
						Message_Type = 2;
						to_execute_temp[0] = 3;						//��ϢֵΪ3
						to_execute_temp[1] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_execute_temp[2] = 0;
					}
					else																//֪ͨ�������ݸ������񣬸��·�������
					{
						Message_Type = 1;	
						to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];
					}
				}  // end of "else if(trigger_type ==5)	"								//����
					break;
//==========================================================================	(6)			
				default:
					break;
//==========================================================================		
		}		
		/*���ȫ�ֱ�������sche_change_tpye[][4]���Ա���̫����ѯ�������;
		* �˴������sche_change_tpye����Э��������ֲ������sche_change_tpye����
		*/
		os_mut_wait(RT_Operate_Mut,0xFFFF);   //���ڷ������������޸�
		for ( i=0;i<=NOW_C;i++)
		{
			sche_change_tpye[i][SCHE_CON_TYPE] = sche_change_temp[i][SCHE_CON_TYPE];
			sche_change_tpye[i][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
			sche_change_tpye[i][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
		}
		os_mut_release(RT_Operate_Mut);
		
		tsk_dispatch_over= (__TRUE);//ι��
		
		if(Message_Type == 1)//������������   //  to  Sche_Data_Update_Task()  //by_lm
		{
			msg_send = to_update_temp;
			os_mbx_send(Sche_Data_Update_Mbox, msg_send, 0x10);    
		}
		else if(Message_Type == 2)//����ִ������
		{
			msg_send = to_execute_temp;
			os_mbx_send(Sche_Execute_Mbox, msg_send, 0x10);  
		}
	}
}
		
/*	�����׶�����-ֻ��	*/
extern const STAGE_TIMING_TABLE_t Yel_Stage_Time_Table[16];
/*	ȫ��׶�����-ֻ��	*/
extern const STAGE_TIMING_TABLE_t Red_Stage_Time_Table[16];
__task void Sche_Data_Update_Task(void)
{
	u8										*msg;
	u8 										*msg_send;
	u32 									i,j;
	u8 										time_start_flag;//��ʱ��������־-ȷ����ʱ��ֻ����һ��
	
	


	/*
	�������ݶ�ȡ�������
	*/

	u8 										degrade_flag;	//������־
	u8 										scheme_num_temp;
	u8 										control_type_temp;
	CONFIG_SCHEME_t 			check_0xc0_temp;
	STAGE_TIMING_TABLE_t 	check_0xc1_temp[16];
	PHASE_PARAM_t 				phase_par_temp[16];
	FOLLOW_PHASE_PARAM_t 	tab_0xc8_temp[9];
	CHANNEL_PARAM_t 			tab_0xb0_temp[Channel_Max_Define];
	/*
	���ɷ��������������
	*/
	u32 			temp;	
	u8				Stage_State_temp;								//�׶�����
	float 		stage_time[16];              		//�·������׶�ʱ��
	u32 			stage_time_temp[3][16];   			//���ɷ������׶�ʱ��
	float 		c;															//����Ư��ֵc'
	u32 			time_differ_temp = 0;						//����ʵ��ִ��ʱ����ƻ�ִ��ʱ��֮��
	u32 			time_current_temp;							//DS3231ʱ��-ת���ɵ�����
	u8 				time_data_temp[7] = {0};				//DS3231ʱ��-����
	u8				new_cycle_time_temp;      			//������ʱ��(��������Ư��ֵc')
	u8				new_phase_differ_temp;       		//����λ��(��������Ư��ֵc')
	u8				old_phase_differ_temp;    			//����λ��(��������Ư��ֵc')
	u8 				fangan_hour_temp;								//�·�����ʼִ��ʱ��-Сʱ
	u8 				fangan_min_temp;								//�·�����ʼִ��ʱ��-����
	u32				fangan_time_temp;								//�·�����ʼִ��ʱ��-ת��������
	
	
	u8  			read_index=0;
	u8  			hold_stageId=0;
	
	u32  			B0_error_count=0;
	
	//========================  debug_lm

	//=========================
	
	//================================3.23
	u16 work_phase;//������λ
		/*�ϱ�����*/
	u8 Phase_conflict_answer_Data[7] = {0};
	u16 phase_conflict;//��ͻ������λ
	
	u8 send_temp[2]={0};
	u8 send_temp1[2]={0};
	
	/*	
	���ݿ������Ҫ�ı���	
	*/
	u32										read_lenth_temp		=		0;	
	TARGET_DATA 					data_Target;	
		
	control_type_temp++;
	
	for(;;)
	{
		
		tsk_data_run= (__TRUE);//ι��
		//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
		
		os_mbx_wait(Sche_Data_Update_Mbox, (void *)&msg, 0xffff);
		control_type_temp	= *msg;  
		scheme_num_temp 	= *(++msg);

		
	
		
		#ifdef LIMING_DEBUG_12_23
			printf("-Data_Up-\n");
		#endif		

		
		/*
		��ʼ��temp����---�������ݶ�ȡ�������ȫ����0
		*/
		time_start_flag 							= 	0;
		degrade_flag 									=		0;
		check_0xc0_temp.coPhase 			= 	0;
		check_0xc0_temp.period 				= 	0;
		check_0xc0_temp.phaseDif 			= 	0;
		check_0xc0_temp.schemeId 			= 	0;
		check_0xc0_temp.stageTimeTb 	= 	0;	
		for(i = 0;i < 16;i++)
		{
			check_0xc1_temp[i].index 					= 	0;
			check_0xc1_temp[i].relPhase 			= 	0;
			check_0xc1_temp[i].stageId				=		0;
			check_0xc1_temp[i].stAlterParam 	= 	0;
			check_0xc1_temp[i].stGreenTime 		= 	0;
			check_0xc1_temp[i].stRedTime 			= 	0;
			check_0xc1_temp[i].stYellowTime 	= 	0;
		}
		for(i = 0;i < 16;i++)
		{
			phase_par_temp[i].cleanTime 		= 	0;
			phase_par_temp[i].expandGreen 	= 	0;
			phase_par_temp[i].greenSec 			= 	0;
			phase_par_temp[i].greenShine 		= 	0;
			phase_par_temp[i].maxGreen_1 		= 	0;
			phase_par_temp[i].maxGreen_2 		= 	0;
			phase_par_temp[i].minGreenTime 	= 	0;
			phase_par_temp[i].phaseFunc 		= 	0;
			phase_par_temp[i].phaseId 			= 	0;
			phase_par_temp[i].phaseType 		= 	0;
			phase_par_temp[i].reserved 			= 	0;
			phase_par_temp[i].stadyGreen 		= 	0;
		}	
		for(i = 0;i < 9;i++)
		{
			for(j = 0;j < 16;j++)
			{
				tab_0xc8_temp[i].fixPhase[j] 	= 	0;
				tab_0xc8_temp[i].incPhase[j] 	= 	0;
			}
			tab_0xc8_temp[i].fixPhaseLenth 	= 	0;
			tab_0xc8_temp[i].incPhaseLenth 	= 	0;
			tab_0xc8_temp[i].operationType 	= 	0;
			tab_0xc8_temp[i].phaseId 				= 	0;
			tab_0xc8_temp[i].tailGreen 			= 	0;
			tab_0xc8_temp[i].tailRed 				= 	0;
			tab_0xc8_temp[i].tailYellow			= 	0;	
		}	
		for(i = 0;i < Channel_Max_Define;i++)
		{
			tab_0xb0_temp[i].chnCtlType 	= 	0;
				tab_0xb0_temp[i].chnId 			= 	0;
				tab_0xb0_temp[i].chnStat 		= 	0;
				tab_0xb0_temp[i].signalSrc 	= 	0;
		}
		/*
		��ʼ��temp����---���ɷ��������������ȫ����0
		*/
		temp 							= 	0;	
		Stage_State_temp 	= 	0;
		for(i = 0;i < 16;i++)
			stage_time[i] 	= 	0; 
		for(i = 0;i < 3;i++)
			for(j = 0;j < 16;j++)
				stage_time_temp[i][j] = 0;
	
		c 											= 	0;						
		time_differ_temp 				= 	0;
		time_current_temp 			= 	0;
		for(i = 0;i < 7;i++)
			time_data_temp[i] 		= 	0;	
		new_cycle_time_temp 		= 	0;
		new_phase_differ_temp 	= 	0;
		old_phase_differ_temp 	= 	0;
		fangan_hour_temp 				= 	0;
		fangan_min_temp 				= 	0;	
		fangan_time_temp 				= 	0;
		hold_stageId						=   0;

		//=================================================
		/*=======�����ݿ���� ���õ������ݷ���temp��=======*/
		
		/*
		��0xC0��
		*/	
		read_lenth_temp						=		0;
		data_Target.ID 						= 	0xC0;
		data_Target.count_index 	= 	1;
		data_Target.sub_index[0] 	= 	scheme_num_temp;
		data_Target.sub_index[1] 	= 	0;
		data_Target.sub_target 		= 	0;


		os_mut_wait(Spi_Mut_lm,0xFFFF);
	  read_lenth_temp				=	DB_Read_M(&data_Target);	
		os_mut_release(Spi_Mut_lm);	

		
		if(	read_lenth_temp 			== 	0	)	//error  ��0
		{
			check_0xc0_temp.period				= 	0;
			check_0xc0_temp.phaseDif			= 	0;
			check_0xc0_temp.coPhase 			= 	0;
			check_0xc0_temp.stageTimeTb 	= 	0;
		}
		else 
		{
			check_0xc0_temp.period				= 	data_Target.sub_buf[1];
			check_0xc0_temp.phaseDif			= 	data_Target.sub_buf[2];
			check_0xc0_temp.coPhase 			= 	data_Target.sub_buf[3];
			check_0xc0_temp.stageTimeTb 	= 	data_Target.sub_buf[4];
		}
	
		/*	
		sche_optimize	
		typedef struct {
			u8 		Period_Time;			//��������ʱ��B8	=====�滻C0��
			u8 		Phase_Difference;		//Э����λ��B9		=====�滻C0��
			u8 		Stage_Time[16];		//�Ż��׶�ʱ��BE	=====�滻C1��
			u8 		Key_Phase_Time[16];	//�Ż��׶ιؼ���λ�̵�ʱ��BF
		}sche_optimize_struct;
		*/
		//////////==========youhua
		if(	control_type_temp	== OPTIMIZE_WAY)  //�Ż�
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF); //����
			check_0xc0_temp.period		= 	sche_optimize.Period_Time;
			check_0xc0_temp.phaseDif	= 	sche_optimize.Phase_Difference;
			os_mut_release(RT_Operate_Mut);	  
		}

		
		/*
		��0xC1��
		*/
		read_index = 0;
		if	(scheme_num_temp == 27)		//����
		{
				for(i = 0;i < 16;i++)
				{
					check_0xc1_temp[i] = Yel_Stage_Time_Table[i];
				}
				read_index=15;
		}
		else if	(scheme_num_temp == 28)		//ȫ��
		{
				for(i = 0;i < 16;i++)
				{
					check_0xc1_temp[i] = Red_Stage_Time_Table[i];
				}		
				read_index=4;
		}
		else 
		{
			for(i = 0; i < 16; i++)
			{
				read_lenth_temp					=		0;
				data_Target.ID 						= 	0xC1;
				data_Target.count_index 		= 	2;
				data_Target.sub_index[0] 	= 	check_0xc0_temp.stageTimeTb;
				data_Target.sub_index[1] 	= 	i+1;
				data_Target.sub_target 		= 	0;

				
				os_mut_wait(Spi_Mut_lm,0xFFFF);
				read_lenth_temp				=	DB_Read_M(&data_Target);	
				os_mut_release(Spi_Mut_lm);	
				//read_lenth_temp					=		DB_Read_M(&data_Target);
				
				if(	read_lenth_temp 		!= 	0	)	//error  ���ܸ�0
				{
					check_0xc1_temp[read_index].index 					= 	scheme_num_temp;
					check_0xc1_temp[read_index].stageId 				= 	data_Target.sub_buf[1];
					check_0xc1_temp[read_index].relPhase 			= 	data_Target.sub_buf[2];
					check_0xc1_temp[read_index].relPhase 			<<= 8;
					check_0xc1_temp[read_index].relPhase 			+= 	data_Target.sub_buf[3];
					check_0xc1_temp[read_index].stGreenTime 		= 	data_Target.sub_buf[4];
					check_0xc1_temp[read_index].stYellowTime 	= 	data_Target.sub_buf[5];
					check_0xc1_temp[read_index].stRedTime 			= 	data_Target.sub_buf[6];
					check_0xc1_temp[read_index].stAlterParam 	= 	data_Target.sub_buf[7];
					read_index++;
				}
			}	
		}

		if(	control_type_temp	== HOLD_WAY)  //����              //debug_lm
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);  										 			//����
			hold_stageId			=sche_hold_guand.stageId;
			check_0xc1_temp[hold_stageId-1]	=sche_hold_guand;							//  �˴���i���ڶ���   //liming_?
			os_mut_release(RT_Operate_Mut);	     													//����
		}
	
/*	
sche_optimize	
typedef struct {
	u8 		Period_Time;			//��������ʱ��B8	=====�滻C0��
	u8 		Phase_Difference;		//Э����λ��B9		=====�滻C0��
	u8 		Stage_Time[16];		//�Ż��׶�ʱ��BE	=====�滻C1��
	u8 		Key_Phase_Time[16];	//�Ż��׶ιؼ���λ�̵�ʱ��BF
}sche_optimize_struct;
*/
		//////////==========youhua
		if(	control_type_temp	== OPTIMIZE_WAY)  //�Ż�
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF); //����
			/*
			�׶��̵�ʱ���滻
			�����ж��Ƿ�Ϊ�ؼ���λ
			��Ϊ�ؼ���λ����׶��̵�ʱ���滻ΪKey_Phase_Time[]
			����Ϊ�ؼ���λ������Stage_Time[]�滻
			*/
			for(i = 0;i < read_index;i++)
			{
				if(((check_0xc1_temp[i].stAlterParam) & 0x10) != 0 )//�ؼ���λ
				{
					if(sche_optimize.Key_Phase_Time[i] >= Green_Min)
					{    
						check_0xc1_temp[i].stGreenTime = sche_optimize.Key_Phase_Time[i];
					}
         }
				else//�ǹؼ���λ
				{
					if(sche_optimize.Stage_Time[i]  >= (Green_Min +check_0xc1_temp[i].stYellowTime +check_0xc1_temp[i].stRedTime))
          {     
					   check_0xc1_temp[i].stGreenTime = sche_optimize.Stage_Time[i] 
																							-check_0xc1_temp[i].stYellowTime
																							-check_0xc1_temp[i].stRedTime;
          }
				}

			}
			os_mut_release(RT_Operate_Mut);	  
		}

		/*
		��0x95��
		*/
		for(i = 1;i <= Channel_Max;i++)
		{		
			read_lenth_temp						=		0;
			data_Target.ID 						= 	0x95;
			data_Target.count_index 	= 	1;
			data_Target.sub_index[0] 	= 	i;
			data_Target.sub_index[1] 	= 	0;
			data_Target.sub_target 		= 	0;

			
			os_mut_wait(Spi_Mut_lm,0xFFFF);
			read_lenth_temp				=	DB_Read_M(&data_Target);	
			os_mut_release(Spi_Mut_lm);	
			
			//read_lenth_temp						=		DB_Read_M(&data_Target);
			
			if(read_lenth_temp == 	0)//error	��0
			{		
				phase_par_temp[i-1].phaseId 			= 	0;
				phase_par_temp[i-1].greenSec 			= 	0;
				phase_par_temp[i-1].cleanTime 		= 	0;
				phase_par_temp[i-1].minGreenTime 	= 	0;
				phase_par_temp[i-1].expandGreen 	= 	0;
				phase_par_temp[i-1].maxGreen_1		= 	0;
				phase_par_temp[i-1].maxGreen_2		= 	0;
				phase_par_temp[i-1].stadyGreen 		= 	0;
				phase_par_temp[i-1].greenShine 		= 	0;
				phase_par_temp[i-1].phaseType 		= 	0;
				phase_par_temp[i-1].phaseFunc 		= 	0;
				phase_par_temp[i-1].reserved 			= 	0;
			}
			else
			{
				phase_par_temp[i-1].phaseId 			= 	data_Target.sub_buf[0];
				phase_par_temp[i-1].greenSec 			= 	data_Target.sub_buf[1];
				phase_par_temp[i-1].cleanTime 		= 	data_Target.sub_buf[2];
				phase_par_temp[i-1].minGreenTime 	= 	data_Target.sub_buf[3];
				phase_par_temp[i-1].expandGreen 	= 	data_Target.sub_buf[4];
				phase_par_temp[i-1].maxGreen_1		= 	data_Target.sub_buf[5];
				phase_par_temp[i-1].maxGreen_2		= 	data_Target.sub_buf[6];
				phase_par_temp[i-1].stadyGreen 		= 	data_Target.sub_buf[7];
				phase_par_temp[i-1].greenShine 		= 	data_Target.sub_buf[8];
				phase_par_temp[i-1].phaseType 		= 	data_Target.sub_buf[9];
				phase_par_temp[i-1].phaseFunc 		= 	data_Target.sub_buf[10];
				phase_par_temp[i-1].reserved 			= 	data_Target.sub_buf[11];
			}		
		}		

		/*
		��0xC8��	
		*/
		for(i = 1;i <= 8;i++)
		{				
			read_lenth_temp						=		0;
			data_Target.ID 						= 	0xC8;
			data_Target.count_index 	= 	1;
			data_Target.sub_index[0] 	= 	i;
			data_Target.sub_index[1] 	= 	0;
			data_Target.sub_target 		= 	0;
			

			os_mut_wait(Spi_Mut_lm,0xFFFF);
			read_lenth_temp				=	DB_Read_M(&data_Target);	
			os_mut_release(Spi_Mut_lm);	
			
			//read_lenth_temp						=		DB_Read_M(&data_Target);
			
			if(read_lenth_temp == 	0)
			{
			}
			else
			{	
				tab_0xc8_temp[i].phaseId 				= data_Target.sub_buf[0];
				tab_0xc8_temp[i].incPhaseLenth 	= data_Target.sub_buf[2];
				for(j = 0; j < data_Target.sub_buf[2]; j++)
					tab_0xc8_temp[i].incPhase[j] 	= data_Target.sub_buf[3+j];
			}
		}
		
		/*
		��0xB0��	
		*/
		for(i=1;i <= Channel_Max;i++)
		{
			read_lenth_temp						=		0;
			data_Target.ID 						= 	0xB0;
			data_Target.count_index 	= 	1;
			data_Target.sub_index[0] 	= 	i;
			data_Target.sub_index[1] 	= 	0;
			data_Target.sub_target 		= 	0;

			os_mut_wait(Spi_Mut_lm,0xFFFF);
			read_lenth_temp				=	DB_Read_M(&data_Target);	
			os_mut_release(Spi_Mut_lm);				
			//read_lenth_temp						=		DB_Read_M(&data_Target);
			
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
				//Db_MemCopy(0xB0, &tab_0xb0_temp[i],data_Target.sub_buf,sizeof(CHANNEL_PARAM_t));//liming_?
		}

		if( B0_error_count >=Channel_Max)
		{
				for(i = 0;i < 16;i++)
				{
					check_0xc1_temp[i].index 					= 	0;
					check_0xc1_temp[i].relPhase 			= 	0;
					check_0xc1_temp[i].stageId				=		0;
					check_0xc1_temp[i].stAlterParam 	= 	0;
					check_0xc1_temp[i].stGreenTime 		= 	0;
					check_0xc1_temp[i].stRedTime 			= 	0;
					check_0xc1_temp[i].stYellowTime 	= 	0;
				}
				degrade_flag = 1;
		}
		
		
		/*	
		У�鷽���Ƿ�����	
		����1�������Ŷ�Ӧһ��
		�׶κ�,ĿǰΪPlan_Used.Stage_State
		����ʱ������
		*/
		if(read_index>=1)
		{

			for(i=0;	i		< check_0xc1_temp[read_index - 1].stageId; 	i++)
			{
				if(	check_0xc1_temp[i].stageId 				== 0		)																								//����û��������������
					degrade_flag	=	1;																																//����������
				if(	(check_0xc1_temp[i].stGreenTime 	< Green_Min)		&&	(scheme_num_temp < 27)	)	//�̵�ʱ��С����Сֵ
					degrade_flag	=	1;
				if(	(check_0xc1_temp[i].stYellowTime 	< Yellow_Min)		&& 	(scheme_num_temp < 27)	)	//�Ƶ�ʱ��С����Сֵ
					degrade_flag	=	1;
				if(	(check_0xc1_temp[i].stRedTime 		< Red_Min)			&& 	(scheme_num_temp < 27)	)	//���ʱ��С����Сֵ
					degrade_flag	=	1;
			}
		}
		else
		{
			
			degrade_flag = 1;
		}
			
		/*
		����У�������д���
		�������򽵼�
		�����򿽱���������������
		*/
		if(degrade_flag == 1)
		{
			/*	
			��������	
			*/
// 			RUN_LED_OFF();
// 			ERROR_LED_ON();
			send_temp[0]=(uint8_t)RUN_LED;          			// LED���
			send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			send_temp1[0]=(uint8_t)ERROR_LED;          			// LED���
			send_temp1[1]=(uint8_t)LED_ON;												// ����״̬			
			os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

			/*��������*/
			Degrade_MSG[0]	= 	5;
			msg_send				= 	Degrade_MSG;
			os_mbx_send(Sche_Dispatch_Mbox,msg_send,0x10);				
			/*	
			д��־	
			*/

//				if(TT_cont3==0)    //  Downgrade_report==0
//				{
//					TT_cont3=5;
//					tsk_lock ();		
//					write_log_wr_fifo[write_log_wr_index][0] = Type_A8;
//					write_log_wr_fifo[write_log_wr_index][1] = 0xA8;
//					write_log_wr_fifo[write_log_wr_index][2] = 0x00;
//					write_log_wr_fifo[write_log_wr_index][3] = 0;
//					write_log_wr_fifo[write_log_wr_index][4] = 0x20;							
//					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;										
//					if (++write_log_counter  > WRITE_LOG_SIZE)
//					{
//						write_log_counter						=	WRITE_LOG_SIZE;
//						write_log_buffer_overflow		=	__TRUE;
//					}
//					tsk_unlock ();	
//					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);

//				}
//				else TT_cont3=5;

		}
		else
		{
		//============================================
		/*=======��temp�е�����ת������������=======*/
// 			ERROR_LED_OFF();
// 			RUN_LED_ON();

		send_temp[0]=(uint8_t)RUN_LED;          			// LED���
		send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
		os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
		send_temp1[0]=(uint8_t)ERROR_LED;          			// LED���
		send_temp1[1]=(uint8_t)LED_OFF;												// ����״̬			
		os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

			/*
			�ȴ������������ʷ�������
			*/
			os_mut_wait (Mutex_Data_Update, 0xffff);						
			
			Plan_Used.Plan_Num 			= 	scheme_num_temp;
			Plan_Used.Stage_State 	= 	check_0xc1_temp[read_index - 1].stageId;
			/*
			д0xC0����������	
			*/
			Plan_Used.Period_Time 				= 	check_0xc0_temp.period;
			Plan_Used.Phase_Difference 		= 	check_0xc0_temp.phaseDif;
			Plan_Used.Coordination_Phase	= 	check_0xc0_temp.coPhase;			
			/*	
			д0xC1����������	
			*/
			for(i = 0; i < 16; i++)
			{
				Plan_Used.Stage_Contex[i].index 				= 	check_0xc1_temp[i].index;
				Plan_Used.Stage_Contex[i].stageId 			= 	check_0xc1_temp[i].stageId;
				Plan_Used.Stage_Contex[i].relPhase 			= 	check_0xc1_temp[i].relPhase;
				Plan_Used.Stage_Contex[i].stGreenTime 	= 	check_0xc1_temp[i].stGreenTime;
				Plan_Used.Stage_Contex[i].stYellowTime 	= 	check_0xc1_temp[i].stYellowTime;
				Plan_Used.Stage_Contex[i].stRedTime 		= 	check_0xc1_temp[i].stRedTime;
				Plan_Used.Stage_Contex[i].stAlterParam 	= 	check_0xc1_temp[i].stAlterParam;
			}
			//================================3.23

			//�ж���λ����
			if(scheme_num_temp != 27 && scheme_num_temp != 28)
			{
				for(i = 0; i < 16; i++)
				{
					work_phase = Plan_Used.Stage_Contex[i].relPhase;
					phase_conflict = Phase_Collide_Detect(work_phase);
					if(phase_conflict > 0)
					{
							//�ϱ���λ��ͻ����������,��ͻ
							Phase_conflict_answer_Data[0] = 0x83;
							Phase_conflict_answer_Data[1] = 0x97;
							Phase_conflict_answer_Data[2] = 0x00;
							Phase_conflict_answer_Data[3] = i+1;//�кŴ�ʱ���ڽ׶κ�?
							Phase_conflict_answer_Data[4] = (u8)((work_phase & 0xFF00) >> 8);//��ͻ��λ;
							Phase_conflict_answer_Data[5] = (u8)(work_phase & 0x00FF);
				 
							tsk_lock ();  			//  disables task switching 
							for(j = 0;j < 6;j++)          //8B
								write_Client_fifo[write_Client_wr_index][j] = Phase_conflict_answer_Data[j];

							if (++write_Client_wr_index == WRITE_Client_SIZE) write_Client_wr_index=0;

							if (++write_Client_counter  == WRITE_Client_SIZE)
							{
								write_Client_counter=1;
								write_Client_buffer_overflow=__TRUE;
							};
							tsk_unlock (); 			//  enable  task switching 	
							os_sem_send(eth_client_sche_semaphore);    
							
// 							/*����λ����ֹ��ϣ��򲻷������ݸ���λ�������*/
// 							unsend_to_phase_flag = 1;	
// 							break;
							/*	��������		*/
							Degrade_MSG[0]	= 	5;
							msg_send				= 	Degrade_MSG;
							os_mbx_send(Sche_Dispatch_Mbox,msg_send,0x10);	
					}
// 					else
// 					{
// 						/*����λ����ֹ��ϣ��򲻷������ݸ���λ�������*/
// 							unsend_to_phase_flag = 0;
// 					}						
				}
				
				//��λ����ģ��
			}
			
			//================================3.23
			/*	
			д0x95����������	
			*/
			for(i = 0;i < Channel_Max;i++)
			{
				Phase_Par[i].phaseId 				= 	phase_par_temp[i].phaseId;
				Phase_Par[i].greenSec 			= 	phase_par_temp[i].greenSec;
				Phase_Par[i].cleanTime			= 	phase_par_temp[i].cleanTime;
				Phase_Par[i].minGreenTime		= 	phase_par_temp[i].minGreenTime;
				Phase_Par[i].expandGreen 		= 	phase_par_temp[i].expandGreen;
				Phase_Par[i].maxGreen_1			= 	phase_par_temp[i].maxGreen_1;
				Phase_Par[i].maxGreen_2			= 	phase_par_temp[i].maxGreen_2;
				Phase_Par[i].stadyGreen 		= 	phase_par_temp[i].stadyGreen;
				Phase_Par[i].greenShine 		= 	phase_par_temp[i].greenShine;
				Phase_Par[i].phaseType			= 	phase_par_temp[i].phaseType;
				Phase_Par[i].phaseFunc 			= 	phase_par_temp[i].phaseFunc;
				Phase_Par[i].reserved				= 	phase_par_temp[i].reserved;		
			}	
			/*	
			�ؼ���λ�̵�ʱ��д����������	
			*/
			for(i = 0; i < 16;i++)
				Plan_Used.Key_Phase_Time[i] 	= 	Phase_Par[i].minGreenTime;
			/*		
			д0xC8����������	
			*/
			for(i = 1;i <= 8;i++)
			{
				Tab_C8[i].phaseId 				= 	tab_0xc8_temp[i].phaseId ;
				Tab_C8[i].incPhaseLenth		= 	tab_0xc8_temp[i].incPhaseLenth;
				for(j = 0; j < 16; j++)
					Tab_C8[i].incPhase[j] 	= 	tab_0xc8_temp[i].incPhase[j];
			}
			/*	
			д0xB0����������	
			*/
			for(i=1;i <= Channel_Max;i++)
			{
				Tab_B0[i].chnCtlType 		= 	tab_0xb0_temp[i].chnCtlType;
				Tab_B0[i].chnId 				= 	tab_0xb0_temp[i].chnId;
				Tab_B0[i].chnStat 			= 	tab_0xb0_temp[i].chnStat;
				Tab_B0[i].signalSrc 		= 	tab_0xb0_temp[i].signalSrc;
			}

			/*
			�ж��Ƿ���Ҫִ�й��ɷ���
			����·����ǻ�����27����ȫ�죨28�����򲻲���Ҫִ�кͼ���
			�����ǣ�����Ҫִ�кͼ���
			*/
			if(	(scheme_num_temp == 27)		||	(scheme_num_temp == 28)	
				||(	control_type_temp	== HOLD_WAY)
				||(	control_type_temp	== OPTIMIZE_WAY)) 	
			{
				/*
				�ͷ��ź������˳��������ݷ���
				*/
				//os_mut_release (Mutex_Data_Update);	
				Plan_Used_Count = 0;//���ɷ���   1�ŷ���
			}
			else
			{		
				//==================================
				/*=======���ɷ������㲢��ֵ=======*/
						
				fangan_hour_temp 			= 	Plan_Used.Plane_Execute_Time[0];	//�·�����ʼִ��ʱ��-Сʱ
				fangan_min_temp 			= 	Plan_Used.Plane_Execute_Time[1];	//�·�����ʼִ��ʱ��-����		
				new_cycle_time_temp		= 	Plan_Used.Period_Time;						//�·�������ʱ��
				new_phase_differ_temp	= 	Plan_Used.Phase_Difference;				//�·�����λ��
				Stage_State_temp 			= 	Plan_Used.Stage_State;						//�׶�����
				
				fangan_time_temp 			= 	fangan_hour_temp * 60 * 60 	+ 	fangan_min_temp * 60;

				os_mut_wait(I2c_Mut_lm,0xFFFF);
				I2C_Read_DS3231(time_data_temp);														//  I2C_lm   �д��Ķ���	
				os_mut_release(I2c_Mut_lm);		
				time_current_temp 		= 	time_data_temp[2]* 60*60 		+ 	time_data_temp[1]*60 	+ 	time_data_temp[0];

				if(time_current_temp 	> fangan_time_temp		)
					time_differ_temp 		= time_current_temp 	- 	fangan_time_temp;
				else 
					time_differ_temp 		=	0;

				if(	new_phase_differ_temp 	>= 	time_differ_temp	)
					new_phase_differ_temp			=		(	new_phase_differ_temp	- time_differ_temp	)			%		new_cycle_time_temp;
				else 
					new_phase_differ_temp 		=		new_cycle_time_temp 		- (	time_differ_temp - new_phase_differ_temp	)		%		new_cycle_time_temp;

				//old_phase_differ_temp 		= 	Plan_Used.Last_Phase_Difference;
				old_phase_differ_temp 			= 	0;
				//new_phase_differ_temp 		= 	new_phase_differ_temp % new_cycle_time_temp;
				c = (float)(	new_cycle_time_temp 	+ 	new_phase_differ_temp 	- 	old_phase_differ_temp);

				/*
				�����ʱ������
				����ʱ���������
				��λ���Э����λ�������壬��0

				��������ʱ��	
				*/
				if(c < 0)
				{
					c = 	c 	+ 	new_cycle_time_temp;
				}
				
				if(		(	c	<	(new_cycle_time_temp/2)	)		&&	(c > 0.0000001)	)
				{
					Interim_Scheme[0].Period_Time		= 	new_cycle_time_temp		+		(u8)(c/3);
					Interim_Scheme[1].Period_Time		= 	new_cycle_time_temp		+		(u8)(c/3);
					Interim_Scheme[2].Period_Time 	= 	new_cycle_time_temp 	+		c 	- 	2	*	(u8)(c/3)	;
				}
				else if(	c < 	(	new_cycle_time_temp * 3 / 2	)		)
				{
					Interim_Scheme[0].Period_Time 	= 	(u8)(		(2*new_cycle_time_temp + c)	/3	);
					Interim_Scheme[1].Period_Time 	= 	(u8)(		(2*new_cycle_time_temp + c)	/3	);
					Interim_Scheme[2].Period_Time 	= 	2	*	new_cycle_time_temp 	+ 	c - 	2*(u8)(	(2*new_cycle_time_temp + c)	/3	);
				}
				else if(	c < 	2*new_cycle_time_temp	)
				{
					Interim_Scheme[0].Period_Time 	= 	(u8)(		(new_cycle_time_temp + c)	/3	);
					Interim_Scheme[1].Period_Time 	= 	(u8)(		(new_cycle_time_temp + c)	/3	);
					Interim_Scheme[2].Period_Time 	= 	new_cycle_time_temp 	+ 	c 	- 	2*(u8)(	(new_cycle_time_temp + c)	/3	);
				}
				else//�����ϲ�����ִ����
				{

				}

				/*
				���׶���ʱ��
				�׶κš�������λ���·�������ͬ        
				*/		

				/*	���������λ	*/
				for(i = 0;i < 3;i++)
				{
					for(j = 0;j < Stage_State_temp;j++)
					{
						Interim_Scheme[i].Stage_Contex[j].index 				= Plan_Used.Stage_Contex[j].index;          //�׶���ʱ������
						Interim_Scheme[i].Stage_Contex[j].stageId 			= Plan_Used.Stage_Contex[j].stageId;        //�׶κ�
						Interim_Scheme[i].Stage_Contex[j].relPhase 			= Plan_Used.Stage_Contex[j].relPhase;       //������λ
						Interim_Scheme[i].Stage_Contex[j].stAlterParam 	= Plan_Used.Stage_Contex[j].stAlterParam;		//�׶�ѡ�����
					}
				}
				
				/*			
				�����ɫ��ʱ��
				(���·������׶�ʱ������
				������ɷ������׶�ʱ����
				���һ�׶�ȡʣ��ʱ��)		
				*/
				for(j = 0;	j < (Stage_State_temp - 1);	j++)
				{
					stage_time[j]		= 	Plan_Used.Stage_Contex[j].stGreenTime 	+ 	Plan_Used.Stage_Contex[j].stYellowTime		+ 	Plan_Used.Stage_Contex[j].stRedTime;
				}
				for(i = 0;i < 3;i++)
				{
					for(j = 0;j < (Stage_State_temp - 1);	j++)
					{
						
						stage_time_temp[i][j] 										= 	(u8)(		stage_time[j] 	/ 	new_cycle_time_temp 	* 	Interim_Scheme[i].Period_Time		);
						stage_time_temp[i][Stage_State_temp - 1] 	+= 	stage_time_temp[i][j];					//ͳ��ǰ�����׶�ʱ��֮��
					}
				}
				for(i = 0;i < 3;i++)
				{
					temp																			= 		stage_time_temp[i][Stage_State_temp - 1];
					stage_time_temp[i][Stage_State_temp - 1] 	= 		Interim_Scheme[i].Period_Time - temp;
				}
				for(i = 0;i < 3;i++)
				{
					for(j = 0;j < Stage_State_temp;j++)
					{
						Interim_Scheme[i].Stage_Contex[j].stGreenTime 	= 	stage_time_temp[i][j] - 6;	//�׶��̵�ʱ��
						Interim_Scheme[i].Stage_Contex[j].stYellowTime 	= 	4;													//�׶λƵ�ʱ��
						Interim_Scheme[i].Stage_Contex[j].stRedTime 		= 	2;													//�׶κ��ʱ��
					}
				}
				Plan_Used_Count = 1;//���ɷ���   1�ŷ���
			}
			
			/*
			�ͷ��ź������˳��������ݷ���
			*/
			os_mut_release (Mutex_Data_Update);	  
			/*
			������ʱ����
			ִֻ��һ��
			*/
			if(time_start_flag == 0)
			{
				time_start_flag = 1;

				TIM_Cmd(TIM3, ENABLE);
				//TIM_Cmd(TIM4, ENABLE);
				TIM_Cmd(TIM5, ENABLE);
			}
			
			
			
			
			//========�д�����====================================
			//====================================================		
			/*
			��Ϣֵ�д��������������ȷ��
			*/
			
// 			if(Plan_Used.Coordination_Phase != 0)//��Э����λ,����ȫ����Э����λ
// 			{
// 				//���ɷ���
// 				Sche_Interim_MSG[0] = scheme_num_temp;
// 				msg_send 			= Sche_Interim_MSG;
// 				os_mbx_send(Sche_Interim_Mbox,msg_send,0x10);
// 			}
// 			else
			//{//������ɷ���
				//������ϢC2
				Sche_Execute_MSG[0] = 2;
				Sche_Execute_MSG[1] = scheme_num_temp;
				Sche_Execute_MSG[2] =	hold_stageId;
				msg_send = Sche_Execute_MSG;
				os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
			//}	
		}

		tsk_data_over= (__TRUE);//ι��
		//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	}
}
//����ִ������
//�׶���ʱ�����ɫʱ��ת���ļ���ֵ��ȫ�ֱ������ݱ���/*������*/
STAGE_TIMING_COUNT_t		stage_timing_count = {0};
/*�޲���ʱ��ǰ�Ľ׶��̵Ƽ���ֵ*//*������*/
u16 current_stage_green_count = 0;
/*����ǰ�̵��������У���һ�ΰ�����ʱ����Ҫ��0xffff��������ֵ���ñ�־��ʾ��һ�ν��벽��*//*������*/
u8 first_keep_current_step = 0;
/*���ݿ��趨����������ʱ��*//*������*/
u8 walker_green_flicker = 6;

u8 sche_cnt = 0;
u8 sche_end = 0;
__task void Sche_Execute_Task(void)
{

	u32 i, p;
	u8  fangan_NO, MSG_Send_Flag;
	
	u8  *msg;
	u8  *msg_send;
	u8  msg_rx_type = 0;			//��Ϣ����
	u8  Stage_Total = 0;			//�ܵĽ׶���
	u8 	Step_En_Flag = 0;			//������ʶ
	u8 	Back_Type 	= 0;			//�ָ�����֮ǰ����
	u8	current_stage_no = 0;	//��ǰ�׶κ�
	u8	to_Sche_Dispatch_MSG_temp[5] = {0};		//����������������������

//	u8	to_Sche_Output_Mbox_temp=0;

	u8  hold_stageId=0;

	Plan_Model                Plan_Used_temp;	//Ӧ��ģ��(�ֲ�)
	static Induction_Str 	Schedule_Struct_exe_temp[Channel_Max_Define] 	= {0};//����ģ��ṹ��

	/*	lxb��λ����½ӿ�1.13		*/
	STAGE_TIMING_COUNT_t		stage_count_temp 		= {0};//�׶���ʱ�����ɫʱ��ת���ļ���ֵ�ֲ��ṹ�壬��������������������
	u32 msg_to_output = 0;
	u8	step_flag =0;
	/*������*/
	STAGE_TIMING_COUNT_t stage_count_feedback_temp = {0};//��ǰ�����������ֵ�ṹ���ɷ�����������������������ֵ
	
	U8 step_stage = 0;//==5,������ʼ;==0xf0,��������;==0xf1������������
	
	/*����ɵ��̵Ƽ���ֵcompleted_green_count*/
	u16 completed_green_count= 0;/*������*/
	/*	lxb��λ����½ӿ�1.13		*/
			
	U8 send_temp[2] = {0};
	
	for(;;)
	{
		os_mbx_wait(Sche_Execute_Mbox, (void *)&msg, 0xffff);
		
		#ifdef LIMING_DEBUG_12_23
			printf("-Execute-\n");
		#endif		
		
		tsk_execute_run= (__TRUE);//ι��  

		msg_rx_type 		= *msg;
		Back_Type 			= *(++msg);//�ָ�����֮ǰ���ͣ�msg_rx_type == 6ʱʹ��
		hold_stageId		= *(++msg);
		
		MSG_Send_Flag 	= 0;

	/*	lxb��λ����½ӿ�1.13		*/		
		stage_count_temp.index 				= 0;								//	�׶���ʱ������			
		stage_count_temp.stageId 			= 0;								//�׶κ�	
		stage_count_temp.stAlterParam = 0;	//�׶�ѡ�����
			stage_count_temp.relPhase = 0;					//������λ2B
		for(i = 0; i < Channel_Max; i++)
		{
			stage_count_temp.stGreenCount[i] 	= 0;//�׶�16ͨ���̵Ƽ���
			stage_count_temp.stYellowCount[i] = 0;//�׶�16ͨ���ƵƼ���
			stage_count_temp.stRedCount[i] 		= 0;//�׶�16ͨ����Ƽ���
		}
	/*	lxb��λ����½ӿ�1.13		*/
			
		os_mut_wait (Mutex_Data_Update,0xFFFF);	
		Plan_Used_temp	=Plan_Used;
		os_mut_release (Mutex_Data_Update);	
		
		os_mut_wait(Output_Data_Mut,0xFFFF);			//��������ص���
		for(i = 1; i < Channel_Max_Define; i++)
		{
			Schedule_Struct_exe_temp[i] = Schedule_Struct[i];
		}
		os_mut_release(Output_Data_Mut); 				//�������򿪵���	
			
// 		if((Plan_Used_temp.Plan_Num!=27)&&(Plan_Used_temp.Plan_Num!=28))
// 		{
// 			if(sche_cnt == 0)
// 			{
// 				phaseT_make = __TRUE;
// 				sche_cnt = 0xf0;
// 			}
// 		}
//================================================================(1)			
		/*   ���Է���������� (�׶ν���)  */
		if(msg_rx_type == 1)
		{
			step_flag = 0;//�׶ν���������ʱ�رղ�����ʽ�����ڷ����������ƺ����ֵ��ֵ/*������*/
			
			if(sche_end ==1) 
			{
				current_stage_no = Stage_Total;
				sche_end = 0;
			}
			
			if( current_stage_no < Stage_Total)//����δ����
			{
				current_stage_no ++;//�׶��Լӣ���һ�׶�
				Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
			}
			else// ��������
			{
				to_Sche_Dispatch_MSG_temp[0] 	= 4;
				//==========������Ϣ
				MSG_Send_Flag 								= 1;
				
			}
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB	
				/*  �ѵ�ǰ�׶κ�д������	*/
				BKPRAM_Write(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);		
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB					
			
		}
//================================================================
//================================================================(2)	
		else if(msg_rx_type == 2)//������   ���Է������ݸ�������
		{
			/*			*/
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB					
			if(IWDGRST_flag ==__TRUE)  			/*  ���Ź���λ	*/
			{
						IWDGRST_flag =__FALSE;			/*   ���һ��ʵ���ˣ��������־��  */
						
						/*  ������ǰ�׶κ�	*/
						BKPRAM_Read(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);
				
						if(hold_stageId !=0)  
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//������
							Stage_Total 			= Plan_Used_temp.Stage_State;	//�׶�����
							//current_stage_no  =	hold_stageId;
							hold_stageId			=0;	   //  �Ա���һ��ʹ�� 
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
							for(p = 0; p <= Channel_Max; p++)
							{
								if(Schedule_Struct_exe_temp[p].release_phase == 0x0A)//��λ���ط���
								{
									Schedule_Struct_exe_temp[p].green_count 			= Schedule_Struct_exe_temp[p].green_count * 60;//����ת������
									Schedule_Struct_exe_temp[p].max_green_count 	= Schedule_Struct_exe_temp[p].green_count * 60;
								}
							}
						}
						else 
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//������
							Stage_Total 			= Plan_Used_temp.Stage_State;	//�׶�����
							//current_stage_no 	= 1;					//��ǰ�׶κ� = 1
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
						}								
			}	/*   end of "if(IWDGRST_flag ==__TRUE) "  */
			else 							/*  û�п��Ź���λ	*/
			{
						if(hold_stageId !=0)  
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//������
							Stage_Total 			= Plan_Used_temp.Stage_State;	//�׶�����
							current_stage_no  =	hold_stageId;
							hold_stageId			=0;	   //  �Ա���һ��ʹ�� 
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
							for(p = 0; p <= Channel_Max; p++)
							{
								if(Schedule_Struct_exe_temp[p].release_phase == 0x0A)//��λ���ط���
								{
									Schedule_Struct_exe_temp[p].max_green_count 	= Schedule_Struct_exe_temp[p].green_count * 60;
									Schedule_Struct_exe_temp[p].green_count 			= Schedule_Struct_exe_temp[p].green_count * 60;//����ת������
								}
							}
						}
						else 
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//������
							Stage_Total 			= Plan_Used_temp.Stage_State;	//�׶�����
							current_stage_no 	= 1;					//��ǰ�׶κ� = 1
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
						}
			}	
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB

			
		}
//================================================================
//================================================================(3)			
		else if(msg_rx_type == 3)//�����ڣ�����δ���£���������ֱ�ӹ���
		{
				fangan_NO 				= Plan_Used_temp.Plan_Num;	//������
				Stage_Total 			= Plan_Used_temp.Stage_State;	//�׶�����
				current_stage_no 	= 1;					//��ǰ�׶κ� = 1
				Fangan_Model(fangan_NO, 1,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);//����������ֵ			
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB	
				/*  �ѵ�ǰ�׶κ�д������	*/
				BKPRAM_Write(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);		
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB		
		}
		
//================================================================
//================================================================(4)			
		else if(msg_rx_type == 4)//����ȫ����ƿ���
		{//��ǰ������ǰ����		
			
			send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED���
			send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			for(p = 0; p <= Channel_Max; p++)
			{
				if(Schedule_Struct_exe_temp[p].green_count > 1)
				{
					Schedule_Struct_exe_temp[p].green_count 			= 0;
					Schedule_Struct_exe_temp[p].max_green_count 	= 0;
				}
			}
			Stage_Total 			= 0;//�ܽ׶�������
			os_mut_wait (Mutex_Data_Update,0xFFFF);	
  		Plan_Used_Count = 0;//���ɷ���//chen11.29
  		os_mut_release (Mutex_Data_Update);	
		}
//================================================================
//================================================================(5)			
		else if(msg_rx_type == 5)//����/*������*/
		{
			
			send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED���
			send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			step_flag = 1;//������ʶ
	
			/*
				�׶���ʱ�����ɫʱ������ֲ���
			*/
			os_mut_wait(Stage_Count_Mut,0xFFFF);
			stage_count_feedback_temp = stage_timing_count;				
			os_mut_release(Stage_Count_Mut);
			
			os_mut_wait (Mutex_Data_Update,0xFFFF);	
  		Plan_Used_Count = 0;//���ɷ���//chen11.29
  		os_mut_release (Mutex_Data_Update);	
			
			if(Step_En_Flag == 0)//������һ��
			{
				step_stage = 5;
				for(p = 0; p <= Channel_Max; p++)
				{
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{
						Schedule_Struct_exe_temp[p].green_count 			= 0xFFFF;
						Schedule_Struct_exe_temp[p].max_green_count 	= 0xFFFF;
						
						/*������ɵ��̵Ƽ���ֵ����completed_green_count*/
						completed_green_count = current_stage_green_count-stage_count_feedback_temp.stGreenCount[p-1];
					}
				}
				Step_En_Flag = 1;//������ʶ��λ
				
			}
			else//�����ڶ�����
			{
				step_stage = 0xf0;
				for(p = 0; p <= Channel_Max; p++)
				{
// 					/*
// 						�׶���ʱ�����ɫʱ������ֲ���
// 					*/
// 					os_mut_wait(Stage_Count_Mut,0xFFFF);
// 					stage_count_dispatch_temp = stage_timing_count;				
// 					os_mut_release(Stage_Count_Mut);
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{
						Schedule_Struct_exe_temp[p].green_count 			= walker_green_flicker*Int_Time_Per_Second;
						Schedule_Struct_exe_temp[p].max_green_count 	= walker_green_flicker*Int_Time_Per_Second;
						
						/*����ɵ��̵Ƽ���ֵ����*/
						completed_green_count = 0;
					}
				}
			}
		}
//================================================================
//================================================================(6)			
		else if(msg_rx_type == 6)//�ָ�����
		{
			if(Back_Type == 1)//��ɽȫ�����
			{
				Stage_Total = 0;
// 				CTLR_TYPE2_LED_OFF();
				send_temp[0]=(uint8_t)CTLR_TYPE2_LED;          			// LED���
				send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			}
			else if(Back_Type == 2)//����
			{
				Stage_Total = 0;
				for(p = 0; p <= Channel_Max; p++)
				{
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{
						Schedule_Struct_exe_temp[p].green_count 			= 0;
						Schedule_Struct_exe_temp[p].max_green_count 	= 0;
					}
				}

			}
			else if(Back_Type == 3)//����
			{
				step_stage = 0xf1;
// 				CTLR_TYPE2_LED_OFF();
				send_temp[0]=(uint8_t)CTLR_TYPE2_LED;          			// LED���
				send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
				
				step_flag = 0;//������ʶ
				
				/*����ǰ�̵��������У���һ�ΰ�����ʱ����Ҫ��0xffff��������ֵ��
				�ñ�־��ʾ��һ�ν��벽��*/
				first_keep_current_step = 0;
				
				/*
					�׶���ʱ�����ɫʱ������ֲ���������
				*/
				os_mut_wait(Stage_Count_Mut,0xFFFF);
				stage_count_feedback_temp = stage_timing_count;				
				os_mut_release(Stage_Count_Mut);
				
				/*
					�ֶ�����л����Զ�ʱ���������ִ������̵�ʱ�䣬����ִ�е��̵�ʱ��������ʱ��6s����
				��ǰ�׶α���Ҫִ�е�ʱ�䣬��ֵ����ʱ�伴�ɣ�����ֵ���ֵ��������
				*/
				for(p = 1; p <= Channel_Max; p++)
				{
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{		
						if(stage_count_feedback_temp.stGreenCount[p-1]>0)
						{	
							if(current_stage_green_count
								<=(0xffff-stage_count_feedback_temp.stGreenCount[p-1]+walker_green_flicker*Int_Time_Per_Second+completed_green_count))
							{
								Schedule_Struct_exe_temp[p].green_count 		= walker_green_flicker*Int_Time_Per_Second;
								Schedule_Struct_exe_temp[p].max_green_count = walker_green_flicker*Int_Time_Per_Second;							
							}
							else
							{
								Schedule_Struct_exe_temp[p].green_count 		= 
								current_stage_green_count -(0xffff-stage_count_feedback_temp.stGreenCount[p-1]+completed_green_count);
								Schedule_Struct_exe_temp[p].max_green_count = Schedule_Struct_exe_temp[p].green_count;							
							}
						}
						else
						{
							Schedule_Struct_exe_temp[p].green_count 		= 0;
							Schedule_Struct_exe_temp[p].max_green_count =	0;
								
							Schedule_Struct_exe_temp[p].yellow_count 		= stage_count_feedback_temp.stYellowCount[p-1];
	
						}
						
					}
				}
// 				Step_En_Flag = 0;//������ʶ��λ
			}
			/*else if(Back_Type == 4)//�Ż�
			{
				//�Ż��������
				Optimize_Data.Period_Time_1 = 0;
				Optimize_Data.Phase_Difference_1 = 0;
				for(i = 0; i <= 15; i++)
				{
					Optimize_Data.Stage_Time_1[i] = 0;
					Optimize_Data.Key_Phase_Time_1[i] = 0;
				}
				Optimize_Data.Optimize_time = 0;
			}*/
		}
//================================================================

			os_mut_wait(Output_Data_Mut,0xFFFF);
			for(i = 1; i < Channel_Max_Define; i++)
			{
				Schedule_Struct[i] =Schedule_Struct_exe_temp[i];
			}
			
			/*	lxb��λ����½ӿ�1.13		*/
			if(Plan_Used.Plan_Num>0)
			{
				stage_count_temp.index 				= Plan_Used.Plan_Num;								//	�׶���ʱ������
			}
			else
			{
				stage_count_temp.index = 0xff;
			}
			stage_count_temp.stageId 			= current_stage_no;								//�׶κ�
			
			os_mut_release(Output_Data_Mut);
			if(Step_En_Flag == 1)
			{
				stage_count_temp.control_way 	= step_stage;
				if(step_stage == 0xf1) Step_En_Flag = 0;
			}
			else
			{
				stage_count_temp.control_way 	= AUTO_WAY;
			}
			stage_count_temp.stAlterParam = Plan_Used_temp.Stage_Contex[current_stage_no-1].stAlterParam;//�׶�ѡ�����
			/*���ݿ��趨����������ʱ��*/
			walker_green_flicker = ((stage_count_temp.stAlterParam & 0xe0)>>4);
			if(walker_green_flicker<6) walker_green_flicker = 6;
			for(i = 0; i < Channel_Max; i++)
			{
				if(Schedule_Struct_exe_temp[i+1].release_phase == 0x0A)
				{
					stage_count_temp.relPhase |= (1<<i);					//������λ2B
				}
				stage_count_temp.stGreenCount[i] 	= Schedule_Struct_exe_temp[i+1].green_count;		//�׶�16ͨ���̵Ƽ���
				stage_count_temp.stYellowCount[i] = Schedule_Struct_exe_temp[i+1].yellow_count;		//�׶�16ͨ���ƵƼ���
				stage_count_temp.stRedCount[i] 		= Schedule_Struct_exe_temp[i+1].red_count;			//�׶�16ͨ����Ƽ���
			}
			/*	lxb��λ����½ӿ�1.13		*/
				
			tsk_execute_over= (__TRUE);//ι��
			
			if(MSG_Send_Flag == 1)//������Ϣ
			{
				msg_send = to_Sche_Dispatch_MSG_temp;
				os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
			}
			else 
			{
				msg_to_output = (u32)(&stage_count_temp);
				os_mbx_send(Sche_Output_Mbox, &msg_to_output, 0x10);  
			}		

	}
}

//������񡪡�����������

//������񡪡�����������
#define 	Msg_From_Execute_Task	(1)	//����ִ�����񴥷�
#define 	Msg_From_Tim4_Int			(2)	//��ʱ�жϴ���

extern CHANNEL_OUTPUT_TOB1_t Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1,u16* channel_ability,u16* passage_walker);
extern u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp);
u8		Flashing_pulse_500ms		= 0;//500ms״ֵ̬��500ms��תһ��
u8		Flashing_pulse_1s				= 0;//1000ms״ֵ̬��1000ms��תһ��

extern U16	type2_follow_phase;//���������͵ĸ�����λ

extern U8 find_current_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U16 current_phase_run);
extern U8 find_next_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO);
extern U8 is_current_phase_over(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO,STAGE_TIMING_COUNT_t *channel_output_C1);

extern void follow_phase_c8(STAGE_TIMING_COUNT_t *channel_output_C1,u16* stage_followrelPhase);
extern FOLLOW_PHASE_WORKING_t follow_phase_para[C8_LINE];
__task void Sche_Output_Task(void)
{
	/*�׶���ʱ�����ɫʱ��ת���ļ���ֵ�ֲ��ṹ��*/
	STAGE_TIMING_COUNT_t		*pstage_count_temp 	= NULL;
	/*�׶���ʱ�����ɫʱ��ת���ļ���ֵ�ֲ��ṹ��*/
	STAGE_TIMING_COUNT_t		stage_count_temp 	  = {0};
	/*16��λ����λ״̬������ת����96��*/
	PHASE_OUTPUT_16STAT_t		phase_16status_temp	= {0};
	/*16ͨ����ͨ��״̬������ת����B1��*/
	CHANNEL_OUTPUT_TOB1_t		channel_toB1_temp		= {0};
	/*16ͨ����ͨ��ʹ�ܱ�ĳλΪ1����ʾ��ͨ�����ã��ñ���B0����*/
	u16 channel_ability													=		0;
	/*16ͨ��������ͨ����ĳλΪ1����ʾ��ͨ��Ϊ����ͨ�����ñ���B0����*/
	u16	passage_walker 													=		0;
	u8						Flashing_pulse_Time = 0;		//500ms��1s״̬��תֵ����
	u8						phase_count = 0;						//��λ������
	u8						i = 0,j=0;
	u32						*msg;
	
	u8 send_num = 0;
	/*����ִ������֪ͨ�׶ν���	*/
	u8 						to_Sche_Execute_MSG_temp[3] = {0};
	u8						*msg_send	= NULL;

	u8 to_Sche_Dispatch_MSG_temp[3]={0};
	u8 *msg_dispatch = NULL;
	u16						current_phase_run = 0xFFFF;		//ִ�������ֵ--����Ȼ���е���λ����Ϊ0�����ʾ�׶ν���
	
	u8 B0_toPhase[8]={0};//����λ�巢��B0�����Ϣ
	
	U16 stage_followrelPhase = 0;
	
	/*��������*/
	U16 phase_order_num,last_phase_run = 0;
	
	/*�����������*/
	U8 step_stat = 0U;
	U8 step_flag = 0;
	U8 current_phaseNO = 0U;
	U8 next_phaseNO = 0U;
	U16 current_over_phase = 0U;
//	U16 current_stage_phase = 0U;
	U8  step_ending = 0U;
//	U8  first_make_phaseT =0;
	U16 phase_cnt = 0;
	U16 current_continue_phase = 0;
//	U8  type2_calculate = 0;
//	U8  type2_phaseT = 0;
	
	U16 actual_phase = 0;//ʵ�ʷ�����λ
	u16 phase_conflict = 0;
	u8 Phase_conflict_answer_Data[7] = {0};
	
	u8 phase_conflict_times = 0;
	
	for (;;) 
	{   
 	  os_mbx_wait(Sche_Output_Mbox, (void *)&msg, 0xffff);   //  256ms
 		
		#ifdef LIMING_DEBUG_12_23
			printf("-Output-\n");
		printf("%d\n",Plan_Used.Plan_Num);
		#endif	
		
		tsk_output_run= (__TRUE);//ι��   
		
		if(*msg > 0)									//����ִ�д���
		{
			pstage_count_temp = (STAGE_TIMING_COUNT_t	*)*msg;
			stage_count_temp.control_way  = pstage_count_temp->control_way;				//���Ʒ�ʽ
			if(step_stat==0)
			{
				stage_count_temp.index 				= pstage_count_temp->index;				//	�׶���ʱ������
				stage_count_temp.stageId 			= pstage_count_temp->stageId;	//�׶κ�
				stage_count_temp.stAlterParam = pstage_count_temp->stAlterParam;//�׶�ѡ�����
				stage_count_temp.relPhase 		= pstage_count_temp->relPhase;					//������λ2B
			}
			if(1)
			{
				if(stage_count_temp.control_way == STEP_WAY) 
				{
					step_stat = 1U;
					step_flag = __TRUE;
				}
				else if(stage_count_temp.control_way == 0xf0) 
				{
					step_stat = 2U;
					step_flag = __TRUE;
				}
				else if(stage_count_temp.control_way == 0xf1) 
				{
					step_stat = 3U;
// 					step_flag = __FALSE;
				}
				else
				{
				
				}
			
			}
			if(step_stat>0)
			{
				switch(step_stat)
				{
					case 1U://step start
					{
						step_stat = 0;
						current_phaseNO = find_current_phaseNO(phase_order_t,current_phase_run);
						if(current_phaseNO < 0xFF)
						{
							stage_count_temp.relPhase = phase_order_t[current_phaseNO][1];
							for(i=0;i<Channel_Max;i++)
							{
								if(((phase_order_t[current_phaseNO][1]>>i)&1)==1)
								{
									if(stage_count_temp.stGreenCount[i] >= walker_green_flicker*Int_Time_Per_Second)
									{
										stage_count_temp.stGreenCount[i] = 0xFFFF;
										stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
										stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
									}
								}
							}
						}
					}
						break;
					case 2U://step handle ������ǰ�̵Ʋ���
					{
						step_stat = 0;
						next_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
						
// // // 						if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[next_phaseNO][1])
// // // 						{//�Ӻ����
// // // 							current_over_phase = (phase_order_t[current_phaseNO][1] & (~phase_order_t[next_phaseNO][1]));
// // // 						}
// // // 						else
// // // 							current_over_phase = phase_order_t[current_phaseNO][1];
						if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[current_phaseNO][1])
						{//�Ӻ����
							current_over_phase = phase_order_t[current_phaseNO][1];	
						}
						else
							current_over_phase = (phase_order_t[current_phaseNO][1] & (~phase_order_t[next_phaseNO][1]));				
						
							stage_count_temp.relPhase = phase_order_t[current_phaseNO][1];
						for(i=0;i<Channel_Max;i++)
						{
							if(((current_over_phase>>i)&1)==1)
							{
								if(stage_count_temp.stGreenCount[i] >= walker_green_flicker*Int_Time_Per_Second)
								{
									if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[current_phaseNO][1])
									{
										stage_count_temp.stGreenCount[i] = walker_green_flicker*Int_Time_Per_Second;
									}
									else
										stage_count_temp.stGreenCount[i] = walker_green_flicker*Int_Time_Per_Second;
									stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
									stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
								}
							}
						}
					}
						break;
					case 3U://step over
					{
						step_stat = 0;
						step_ending = __TRUE;
						/*end current stage step*/
						next_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
						current_over_phase = (phase_order_t[current_phaseNO][1] & (~phase_order_t[next_phaseNO][1]));
						current_continue_phase = (phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]);
						stage_count_temp.relPhase = phase_order_t[current_phaseNO][1];
						for(i=0;i<Channel_Max;i++)
						{
							if(((current_over_phase>>i)&1)==1)
							{
								if(stage_count_temp.stGreenCount[i] >= walker_green_flicker*Int_Time_Per_Second)
								{
									stage_count_temp.stGreenCount[i] = phase_order_t[current_phaseNO][4];
									stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
									stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
								}
							}
							if(((current_continue_phase>>i)&1)==1)
							{
								if(stage_count_temp.stGreenCount[i] >= walker_green_flicker*Int_Time_Per_Second)
								{
									stage_count_temp.stGreenCount[i] = phase_order_t[current_phaseNO][4]+8*Int_Time_Per_Second+walker_green_flicker*Int_Time_Per_Second;
									stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
									stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
								}
							}	
						}
					}
						break;
					
					default:
						break;
				}
				
			}
			else
			{
				for(i = 0; i < Channel_Max; i++)
				{
					if(((type2_follow_phase>>i)&1)==1)
					{
						
					}
					else
					{
						stage_count_temp.stGreenCount[i] 		= pstage_count_temp->stGreenCount[i];//�׶�16ͨ���̵Ƽ���
						stage_count_temp.stYellowCount[i] 	= pstage_count_temp->stYellowCount[i];//�׶�16ͨ���ƵƼ���
						stage_count_temp.stRedCount[i] 		  = pstage_count_temp->stRedCount[i];//�׶�16ͨ����Ƽ���
					}
				}
			}
			
			if((stage_count_temp.index != 27)&&
				(stage_count_temp.index != 28)){					
				stage_followrelPhase = stage_follow_phase(&stage_count_temp);
				}
				
			if((phaseT_make == __TRUE)
				&&(stage_count_temp.index != 27)&&
				(stage_count_temp.index != 28))
			{
				do
				{
					if((stage_count_temp.index != 27)&&
					(stage_count_temp.index != 28)){	
						follow_phase_c8(&stage_count_temp,&stage_followrelPhase);
					}
					
// // // // // // // // 					current_phase_run = Phase_run_step(&stage_count_temp,type2_follow_phase);		//�õ�16��λ״̬��
// // 					current_stage_phase = Phase_Out_96(&stage_count_temp,&phase_16status_temp);
					current_phase_run = Phase_Out_96(&stage_count_temp,&phase_16status_temp);
// 					current_phase_run |= type2_follow_phase;
					for(phase_count=0;phase_count<16;phase_count++)
					{
						if(stage_count_temp.stGreenCount[phase_count] > 0)
						{
							stage_count_temp.stGreenCount[phase_count]--;											//�̵Ƽ�����
						}
						else if(stage_count_temp.stYellowCount[phase_count] > 0)
						{
							stage_count_temp.stYellowCount[phase_count]--;										//�ƵƼ�����
						}
						else if(stage_count_temp.stRedCount[phase_count] > 0)
						{
							stage_count_temp.stRedCount[phase_count]--;												//��Ƽ�����
						}
						
						if(((type2_follow_phase>>phase_count)&1)==1)
						{
							if(stage_count_temp.stRedCount[phase_count] == 0)
							{
								type2_follow_phase &= ~(u16)(1<<phase_count);
							}
						}
					}
				
					if((current_phase_run != last_phase_run)//&&(current_phase_run>0))
						&&((current_phase_run&(~type2_follow_phase)) != 0))
					{
						last_phase_run = current_phase_run;
						
						if((current_phase_run == phase_order_t[0][1])&&(phase_order_num>0))
						{
							if(phase_order_num>0)
							{
// // // 								if((type2_calculate == __TRUE)&&(phase_order_num>1))
// // // 								{
// // // 									type2_calculate == __FALSE;
// // // 									phase_order_t[phase_order_num-2][4] = phase_cnt;
// // // 								}
// // // 								else
// // // 								{
									phase_order_t[phase_order_num-1][4] = phase_cnt;
// // // 								}
							}
							phase_cnt=0;
							
							phaseT_make = __FALSE;
							
							last_phase_run = 0;
//							first_make_phaseT = 0;
							phase_order_num = 0;
//							type2_phaseT = 0;
							
							to_Sche_Dispatch_MSG_temp[0]=4;
							msg_dispatch = to_Sche_Dispatch_MSG_temp;
							os_mbx_send(Sche_Dispatch_Mbox, msg_dispatch, 0x10);
							
							break;
						}
						else
						{
							phase_order_t[phase_order_num][0] = phase_order_num;
							phase_order_t[phase_order_num][1] = current_phase_run;
							phase_order_t[phase_order_num][2] = 0;
							phase_order_t[phase_order_num][3] = stage_count_temp.stageId;
							if(phase_order_num>0)
							{

									phase_order_t[phase_order_num-1][4] = phase_cnt;

							}
							phase_cnt=0;
						}
						if(phase_order_num<PHASET_ARR_NO-1) 
						{
							phase_order_num++;
						}
						else
						{
							phaseT_make = __FALSE;
						}
					}
					else
						phase_cnt++;
					
				}while((current_phase_run&(~type2_follow_phase)) != 0);
				
				if((current_phase_run&(~type2_follow_phase)) == 0)					//�׶ν�����֪ͨ����ִ������
				{
					to_Sche_Execute_MSG_temp[0] = 1;//MSG_Type
					to_Sche_Execute_MSG_temp[1] = pstage_count_temp->index;
					to_Sche_Execute_MSG_temp[2] = pstage_count_temp->stageId;
					msg_send = to_Sche_Execute_MSG_temp;
					os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
				}
			}
		}
		else
		{		
			if((phaseT_make == __TRUE)
			 &&(stage_count_temp.index != 27)
			 &&(stage_count_temp.index != 28))
			{
				
			}
			else
			{
				/* ��ȡ500ms��1s״̬��תֵ */    
				if(Flashing_pulse_Time == 2)
				{
					Flashing_pulse_500ms = ~Flashing_pulse_500ms;//1s�������ڣ���500ms����500ms
				}
				else if(Flashing_pulse_Time == 4)
				{
					Flashing_pulse_Time = 0;
					Flashing_pulse_500ms = ~Flashing_pulse_500ms;
					Flashing_pulse_1s = ~Flashing_pulse_1s;	//2s�������ڣ���500ms����500ms
				}
				else
				{
				}
				Flashing_pulse_Time++;	
				
				/*���������ѭ��*/
				if(step_flag == __TRUE)
				{
					if(step_ending == __TRUE)
					{	
						if(is_current_phase_over(phase_order_t,current_phaseNO,&stage_count_temp) == __TRUE)
						{
							current_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
							if(current_phaseNO == 0)					//�׶ν�����֪ͨ����ִ������
							{
								step_ending = __FALSE;
								step_flag = __FALSE;
								sche_end = __TRUE;
								to_Sche_Execute_MSG_temp[0] = 1;//MSG_Type
								to_Sche_Execute_MSG_temp[1] = pstage_count_temp->index;
								to_Sche_Execute_MSG_temp[2] = pstage_count_temp->stageId;
								msg_send = to_Sche_Execute_MSG_temp;
								os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
							}
							else
							{
								next_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
								current_continue_phase = (phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]);
								stage_count_temp.relPhase = phase_order_t[current_phaseNO][1];
								for(i=0;i<Channel_Max;i++)
								{
									if(((phase_order_t[current_phaseNO][1]>>i)&1)==1)
									{
										stage_count_temp.stGreenCount[i] = phase_order_t[current_phaseNO][4];
										stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
										stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
									}
									if(((current_continue_phase>>i)&1)==1)
									{

											stage_count_temp.stGreenCount[i] += (8*Int_Time_Per_Second+walker_green_flicker*Int_Time_Per_Second);
									}	
								}
							}
						}
					
					}	
					else
					{
						if(is_current_phase_over(phase_order_t,current_phaseNO,&stage_count_temp) == __TRUE)
						{
							current_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
							stage_count_temp.relPhase = phase_order_t[current_phaseNO][1];
							for(i=0;i<Channel_Max;i++)
							{
								if(((phase_order_t[current_phaseNO][1]>>i)&1)==1)
								{
									stage_count_temp.stGreenCount[i] = 0xFFFF;
									stage_count_temp.stYellowCount[i] = 4*Int_Time_Per_Second;
									stage_count_temp.stRedCount[i] = 4*Int_Time_Per_Second;
								}
							}
						}
					}
					
				}
				else
				{
					if((stage_count_temp.index != 27)&&
							(stage_count_temp.index != 28)){	
							follow_phase_c8(&stage_count_temp,&stage_followrelPhase);
						}
				
				}
				/*
					���16��λ״̬��96��
					16��λ���״̬������ id=0x96
					4*2 = 8B
					typedef struct PHASE_OUTPUT_16STAT_S
					{
						US8		  statId;				//��λ״̬����к�
						US16		redStat;				//��λ�ĺ�����״̬
						US16		yellowStat;		//��λ�ĻƵ����״̬
						US16		greenStat;			//��λ���̵����״̬
					}PHASE_OUTPUT_16STAT_t;
				*/
				current_phase_run = Phase_Out_96(&stage_count_temp,&phase_16status_temp);		//�õ�16��λ״̬��
				if((current_phase_run != stage_count_temp.relPhase)||(current_phase_run==0)) 
				{
					//�׶ν������Ǹ�Ӧ�׶���λ����
				}
				/*
					16��λ״̬��96��ת����8��λ״̬��96��
					��λ���״̬������ id=0x96
					4*2 = 8B
					typedef struct PHASE_OUTPUT_STAT_S
					{
						US8		statId;				//��λ״̬����к�
						US8		redStat;				//��λ�ĺ�����״̬
						US8		yellowStat;		//��λ�ĻƵ����״̬
						US8		greenStat;			//��λ���̵����״̬
					}PHASE_OUTPUT_STAT_t;
				*/
				
				
				/*
					���16ͨ��״̬��B1��
					typedef struct CHANNEL_OUTPUT_TOB1_S
					{
						US8			index;							//	�к�
						US16		redStat;					//16ͨ���ĺ�����״̬
						US16		yellowStat;				//16ͨ���ĻƵ����״̬
						US16		greenStat;				//16ͨ�����̵����״̬
					}CHANNEL_OUTPUT_TOB1_T;
				*/
				channel_toB1_temp = Passage_Out_B1(&stage_count_temp,&channel_ability,&passage_walker);		//�õ�16ͨ��״̬��
				
				if(stage_count_temp.index != 27 && stage_count_temp.index != 28)
				{
					actual_phase = 0;
					for(phase_count=0;phase_count<16;phase_count++)
					{
						if(stage_count_temp.stGreenCount[phase_count] > 1)
						{
							actual_phase |=(U16)(1<<phase_count);											
						}
					}
					phase_conflict = Phase_Collide_Detect(actual_phase);
					if((phase_conflict > 0) && (phase_conflict_times >=4))
					{
						type2_follow_phase = 0;
						
						if(step_flag == __TRUE)
						{
							/*  �����λ */
							NVIC_SystemReset();	 
						}
						//�ϱ���λ��ͻ����������,��ͻ
						Phase_conflict_answer_Data[0] = 0x83;
						Phase_conflict_answer_Data[1] = 0x97;
						Phase_conflict_answer_Data[2] = 0x00;
						Phase_conflict_answer_Data[3] = stage_count_temp.stageId+1;//�кŴ�ʱ���ڽ׶κ�?
						Phase_conflict_answer_Data[4] = (u8)((actual_phase & 0xFF00) >> 8);//��ͻ��λ;
						Phase_conflict_answer_Data[5] = (u8)(actual_phase & 0x00FF);
			 
						tsk_lock ();  			//  disables task switching 
						for(j = 0;j < 6;j++)          //8B
							write_Client_fifo[write_Client_wr_index][j] = Phase_conflict_answer_Data[j];

						if (++write_Client_wr_index == WRITE_Client_SIZE) write_Client_wr_index=0;

						if (++write_Client_counter  == WRITE_Client_SIZE)
						{
							write_Client_counter=1;
							write_Client_buffer_overflow=__TRUE;
						};
						tsk_unlock (); 			//  enable  task switching 	
						os_sem_send(eth_client_sche_semaphore);    
						
						/*	��������		*/
						Degrade_MSG[0]	= 	5;
						msg_send				= 	Degrade_MSG;
						os_mbx_send(Sche_Dispatch_Mbox,msg_send,0x10);	
					}
					else if (phase_conflict > 0)
					{
						phase_conflict_times++;
					}
					else
					{
						phase_conflict_times = 0;
					}
				}
				/*
					16ͨ��״̬��B1��ת�������8ͨ��״̬��B1��
					typedef struct CHANNEL_OUTPUT_TOB1_S
					{
						US8			index;					//	�к�
						US8		redStat;					//8ͨ���ĺ�����״̬
						US8		yellowStat;				//8ͨ���ĻƵ����״̬
						US8		greenStat;				//8ͨ�����̵����״̬
					}CHANNEL_OUTPUT_TOB1_T;
				*/
				//����λ�巢�͵�״̬��Ϣ
				if(os_mut_wait(Tab_B1_Mutex,40) == OS_R_OK)//�ȴ�������
				{
					Tab_B1[1].index = 1;
					Tab_B1[1].redStat 		= (u8)(channel_toB1_temp.redStat 		& 0xff);
					Tab_B1[1].yellowStat  = (u8)(channel_toB1_temp.yellowStat & 0xff);
					Tab_B1[1].greenStat 	= (u8)(channel_toB1_temp.greenStat 	& 0xff);
					Tab_B1[2].index = 2;
					Tab_B1[2].redStat 		= (u8)((channel_toB1_temp.redStat>>8) 	 & 0xff);
					Tab_B1[2].yellowStat  = (u8)((channel_toB1_temp.yellowStat>>8) & 0xff);
					Tab_B1[2].greenStat 	= (u8)((channel_toB1_temp.greenStat>>8)  & 0xff);
					
					/*����ͨ��ʹ�ܱ�by lxb3.24�����ø�b1���ͽṹ*/
					B0_toPhase[3] 			= (u8)(channel_ability& 0xff);
					B0_toPhase[4] 		= (u8)((channel_ability>>8) & 0xff);
					
					/*����16ͨ��������ͨ����ĳλΪ1����ʾ��ͨ��Ϊ����ͨ�����ñ���B0����*/
					B0_toPhase[5]  = (u8)(passage_walker& 0xff);
					B0_toPhase[6] 	= (u8)((passage_walker>>8) & 0xff);
					
					B0_toPhase[0] = 0x20;
					B0_toPhase[1] = 0xD1;
					B0_toPhase[2] = 00;
					B0_toPhase[7] = 0xFE;
					
					send_num	 = 1;					
					Send_Channel_Output_Unit(Tab_B1[1],send_num);
					send_num	 = 2;		
					Send_Channel_Output_Unit(Tab_B1[2],send_num);
							
					CAN_TX(B0_toPhase);
				}
				os_mut_release (Tab_B1_Mutex);//�ͷŻ�����
				
				/*
					�׶���ʱ�����ɫʱ�������1
					typedef struct STAGE_TIMING_COUNT_S
					{
						US8			index;					//	�׶���ʱ������
						US8			stageId;					//�׶κ�
						US16		relPhase;				//������λ2B
						US16		stGreenCount[16];		//�׶�16ͨ���̵Ƽ���
						US16		stYellowCount[16];	//�׶�16ͨ���ƵƼ���
						US16		stRedCount[16];			//�׶�16ͨ����Ƽ���
						US8			stAlterParam;		//�׶�ѡ�����
					}STAGE_TIMING_COUNT_t; 
				*/
				for(phase_count=0;phase_count<16;phase_count++)
				{
					if(stage_count_temp.stGreenCount[phase_count] > 0)
					{
						stage_count_temp.stGreenCount[phase_count]--;											//�̵Ƽ�����
					}
					else if(stage_count_temp.stYellowCount[phase_count] > 0)
					{
						stage_count_temp.stYellowCount[phase_count]--;										//�ƵƼ�����
					}
					else if(stage_count_temp.stRedCount[phase_count] > 0)
					{
						stage_count_temp.stRedCount[phase_count]--;												//��Ƽ�����
					}
					
					if(((type2_follow_phase>>phase_count)&1)==1)
					{
						if(stage_count_temp.stRedCount[phase_count] == 0)
						{
							type2_follow_phase &= ~(u16)(1<<phase_count);
						}
					}
				}
				
				/*
					�׶���ʱ�����ɫʱ�����ȫ�ָ���
				*/
				os_mut_wait(Stage_Count_Mut,0xFFFF);
				stage_timing_count = stage_count_temp;				
				os_mut_release(Stage_Count_Mut);
			
				if(step_flag == __FALSE)
				{
					if((current_phase_run&(~type2_follow_phase)) == 0)					//�׶ν�����֪ͨ����ִ������
					{
						to_Sche_Execute_MSG_temp[0] = 1;//MSG_Type
						to_Sche_Execute_MSG_temp[1] = pstage_count_temp->index;
						to_Sche_Execute_MSG_temp[2] = pstage_count_temp->stageId;
						msg_send = to_Sche_Execute_MSG_temp;
						os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
					}		
				}				
			}				
		}
	} 
}



#define ETH_TEMP_SIZE  2000

u8   	 eth_temp_data[ETH_TEMP_SIZE]={0};    //  ���ڱ����������ݰ� ��temp			//by_lm
u32    eth_temp_lenth=0;				//  ���ڱ����������ݰ��ĳ���		  
u32    eth_temp_current=0;			//  ���ڱ��浫ǰ����ĳ���	 eth_temp_current <= eth_temp_lenth	

#define MES_MAX_SIZE   	484
#define MES_IPI   			0x21
#define MES_OPTION_MIN  0x80

extern const u32 table_lenth[][2];

PROTOCOL_DATA 		Proto_Blk;   //   debug_lm

u8 proto_explain_check(u8 *p_Rx_buf,u32 *p_Rx_index,u32 rx_count)
{
 	u32   i              = 0;
	u32   j              = 0;
	u32  	index          = 0;

	//PROTOCOL_DATA 		Proto_Blk;
	u8 								ID_temp 						=0;
	u8 								count_index_temp		=0;
	u8 								sub_target_temp			=0;

	u8 								line_temp						=0;  	//   ����
//	u8 								cor_temp						=0;		//   ����
	u32                msg_data_lenth  		=0;
	
	/*
		���������IPI �� ��������
		�����һ������IPI,�ڶ���������ȷ�Ĳ�������,��"*p_Rx_index += 1;"���ᱻִ��.
		��֮,���ҵ����һ���ֽ�Ϊֹ
	*/
	*p_Rx_index =0;

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

	for(i = 0; i < (rx_count-1); i++)  
	{
		/*  ������ͷ(IPI�Ͳ�������)  */
		if((p_Rx_buf[i] == MES_IPI) && ((p_Rx_buf[i+1] >= MES_OPTION_MIN)))
			break;	
		else
			*p_Rx_index += 1;
	}	

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

		
	/*
			�����������ݶ����� 
			û���ҵ�һ����ȷ������ͷ(IPI�Ͳ�������)
	*/
	if(*p_Rx_index ==rx_count-1)  
	{
		*p_Rx_index = rx_count;
		return (ERROR_OTHER);
	}
	
	/*
			�����������ǰ��� ="*p_Rx_index"�ֽ����ǲ��Ե�
			֮���ҵ�һ����ȷ������ͷ(IPI�Ͳ�������)
	*/		
	else if(*p_Rx_index !=0) 
	{
		return (ERROR_OTHER);	
	}
	else 
	{		
		if(rx_count<4)   //   ���յ�������̫��
		{
			*p_Rx_index = rx_count;
			return (ERROR_TOOSHORT);	
		}			

		Proto_Blk.count_target   = (u8)(( (p_Rx_buf[1] & 0x70) >> 4) + 1);  //�������
		Proto_Blk.operation_type = (u8)(   p_Rx_buf[1] & 0x0F);             //��������
		
		*p_Rx_index=2;		// IPIռ��1B����������ռ��1B
		index = *p_Rx_index;
    if(Proto_Blk.operation_type ==0)   						//��ѯ����
		{
				for (i=0; i<Proto_Blk.count_target; i++)
				{
						Proto_Blk.opobj_data[i].ID          =  p_Rx_buf[index++];                 //�����ʶ
						Proto_Blk.opobj_data[i].count_index = (p_Rx_buf[index	 ]&0xC0) >> 6;			//��������
						Proto_Blk.opobj_data[i].sub_target  =  p_Rx_buf[index++]&0x3F;       			//�Ӷ����(�ڼ����Ӷ���,0��ʾ���е��Ӷ�������)
						
						*p_Rx_index +=1;		//   "�����ʶ"ռ��һ���ֽ�
						*p_Rx_index +=1;		//   "��������+�Ӷ��ź�"ռ��һ���ֽ�
						
						ID_temp 						=Proto_Blk.opobj_data[i].ID;
						count_index_temp		=Proto_Blk.opobj_data[i].count_index;
						sub_target_temp			=Proto_Blk.opobj_data[i].sub_target;
						

						//-----------------------------------------
						/* 	�����ʶ���� */
////						if((ID_temp 	<	MIN_ID)||    
////							 ( ID_temp	>	MAX_ID)	)
								if((ID_temp 	<	ROAD_MAP_ID)||
									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
							     ( ID_temp	>	MAX_ID)	) //mm20140509)
						  
						{
							/*   ����ID��ռ�õ�λ���п��ܾ�����һ��IPI,��˴��±�Ϊ2��ʼ����  */
							*p_Rx_index = 2;
							if(rx_count <=5)
							{
									*p_Rx_index =rx_count;    //С�ڵ���5�����ݵİ��������ܴ���������ͷ֮��������������
							}
							else 
							{
								for(i = 2; i < (rx_count-2-1); i++) 
								{
									/*  ����һ������ͷ(IPI�Ͳ�������)  */
									if((p_Rx_buf[i] == MES_IPI) && ((p_Rx_buf[i+1] >= MES_OPTION_MIN)))
										break;	
									else
										*p_Rx_index += 1;
								}	
							}										
							return (ERROR_OTHER);		
												
						}
						//-----------------------------------------
						
						//-----------------------------------------
						 /* 	�Ӷ���������� */
						if((ID_temp 	>	ROAD_MAP_ID)||(ID_temp 	<	ROAD_MAP_ID))//
							{
							if((sub_target_temp > SUBOBJ_NUM_MAX) 
								 ||(subobject_table[ID_temp - 0X81][sub_target_temp] == 0))
								return (ERROR_OTHER);	
							}						
						//-----------------------------------------
										
	//=====================   ���±�ʾIPI��ID����ȷ =======
						if(count_index_temp 			==0)				// ����������(������,˫����)
						{
							
						}
						else if(count_index_temp 	==1)  			// ������
						{
								Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  ����1  */
								*p_Rx_index +=1;    																					/*����ռ��1Byte */
								
								if(sub_target_temp ==0)  	//����
								{
								}
								else  				 						//  �����Ӷ���
								{	 
								}
							
						}
						else if(count_index_temp 	==2)  			// ˫����
						{
								Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  ����1  */
								Proto_Blk.opobj_data[i].sub_index[1] = p_Rx_buf[index++];			/*  ����2  */
								*p_Rx_index +=1;    																					/*����1ռ��1Byte */
								*p_Rx_index +=1;    																					/*����2ռ��1Byte */

								if(sub_target_temp ==0)  	//����
								{
								}
								else   										//  �����Ӷ���
								{	 
								}
											
						}
						else																	// ��������
						{
							/*  ������������  */
						}
					
				}
				
		}
		else if((Proto_Blk.operation_type ==0x01) ||	//��������
						(Proto_Blk.operation_type ==0x02) )		//��������,��Ӧ��
		{
					
				for (i=0; i<Proto_Blk.count_target; i++)
				{
					Proto_Blk.opobj_data[i].ID          =  p_Rx_buf[index++];                 //�����ʶ
					Proto_Blk.opobj_data[i].count_index = (p_Rx_buf[index	 ]&0xC0) >> 6;			//��������
					Proto_Blk.opobj_data[i].sub_target  =  p_Rx_buf[index++]&0x3F;       			//�Ӷ����(�ڼ����Ӷ���,0��ʾ���е��Ӷ�������)
					
					*p_Rx_index +=1;		//   "�����ʶ"ռ��һ���ֽ�
					*p_Rx_index +=1;		//   "��������+�Ӷ��ź�"ռ��һ���ֽ�
					
					ID_temp 						=Proto_Blk.opobj_data[i].ID;
					count_index_temp		=Proto_Blk.opobj_data[i].count_index;
					sub_target_temp			=Proto_Blk.opobj_data[i].sub_target;
	
					//-----------------------------------------
					/* 	�����ʶ���� */
// // 					if((ID_temp 	<	MIN_ID)	||   
// // 						 (ID_temp		>	MAX_ID)	)

// 								if((ID_temp 	<	ROAD_MAP_ID)||
// 									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
// 							     ( ID_temp	>	MAX_ID)	)				//mm20140509)	
								if((ID_temp 	<	ROAD_MAP_ID)||
									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
							     ( ID_temp	>	MAX_ID)	)				//mm20140509)	
					{
						/*   ����ID��ռ�õ�λ���п��ܾ�����һ��IPI,��˴��±�Ϊ2��ʼ����  */
						*p_Rx_index = 2;
						if(rx_count <= 5)
						{
								*p_Rx_index =rx_count;    //С�ڵ���5�����ݵİ��������ܴ���������ͷ֮��������������
						}
						else 
						{
							for(i = 2; i < (rx_count-2-1); i++) 
							{
								/*  ����һ������ͷ(IPI�Ͳ�������)  */
								if((p_Rx_buf[i] == MES_IPI) && ((p_Rx_buf[i+1] >= MES_OPTION_MIN)))
									break;	
								else
									*p_Rx_index += 1;
							}	
						}										
						return (ERROR_OTHER);		
											
					}
					//-----------------------------------------					
					
					
					//-----------------------------------------
					 /* 	�Ӷ���������� */
					if((ID_temp 	>	ROAD_MAP_ID)||(ID_temp 	<	ROAD_MAP_ID))//
						{
						if((sub_target_temp > SUBOBJ_NUM_MAX) ||(subobject_table[ID_temp - 0X81][sub_target_temp] == 0))
							return (ERROR_OTHER);	
						}					
					//-----------------------------------------
					
					
					//=====================   ���±�ʾIPI��ID����ȷ =======
					line_temp=Object_table[ID_temp-0x81].Line_Number; 		//�õ���ǰID��Ӧ��������� (line_max)
					
					if(count_index_temp 			==0)									// ����������(������,˫����)
					{	
						/*  �������õĶ����ʶ  */
						if(ID_temp ==0x92)
						{
							*p_Rx_index = rx_count;   //  ���������ӵ�
							 return (ERROR_OTHER);								
						}			
						
						if((ID_temp ==0x85)||(ID_temp ==0x8E)||
							 (ID_temp ==0x91)||(ID_temp ==0x92)||
							 (ID_temp ==0x9F)||(ID_temp ==0xC1)||
							 (ID_temp ==0xCA))   //   ��������̫��
						{
							*p_Rx_index = rx_count;   //  ���������ӵ�
							 return (ERROR_OTHER);	
						}
						else
						{
							/*
							��������ı���"����������"���Ѿ�������"�����ʶ"��ռ��һ���ֽ���,
							�Լ�"��������+�Ӷ����"��ռ��һ���ֽ���
							���,�˴�Ҫ��ȥ 2.
							*/
// // //							msg_data_lenth=(Object_table[ID_temp-0x81].Object_Number - BAOLIU*line_temp -2-1); 
// // //							msg_data_lenth=(Object_table[ID_temp-0x81].Tab_leng  -2-1);
						if(ID_temp == ROAD_MAP_ID)	
							{
								msg_data_lenth = ( ((0x00FF & (p_Rx_buf[4]))<<8) 
								 | (p_Rx_buf[5]) );
								msg_data_lenth += 2;
								
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
							}
							else
							{
								msg_data_lenth=(Object_table[ID_temp-0x81].Tab_leng ); // abcdefg ���еĲ����ĳ���
							
								if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //������ // abcdefg 
									{
										msg_data_lenth -= 1;
									}
								else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //˫���� // abcdefg 
									{
										msg_data_lenth -= 2;
									}
								else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //˫����0x92�� // abcdefg 
									{
										msg_data_lenth -= 1;
									}
									
									
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
						}
					 }

					}
					else if(count_index_temp 	==1)  								// ������
					{
							/*  �������õĶ����ʶ  */
							if(ID_temp ==0x92)
							{
								*p_Rx_index = rx_count;   //  ���������ӵ�
								 return (ERROR_OTHER);								
							}
							
							Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  ����1  */
							*p_Rx_index +=1;    																					/*	����ռ��1Byte */
							
							if(sub_target_temp ==0)  	//����
							{
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//ÿ�еĳ���
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
							}
							else  				 						//  �����Ӷ���
							{	 
								msg_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//�����Ӷ���Ĵ�С
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;								
							}
					
					}
					else if(count_index_temp 	==2)  			// ˫����
					{
							Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  ����1  */
							Proto_Blk.opobj_data[i].sub_index[1] = p_Rx_buf[index++];			/*  ����2  */
							*p_Rx_index +=1;    					/*����1ռ��1Byte */
							*p_Rx_index +=1;    					/*����2ռ��1Byte */

							if(sub_target_temp ==0)  	//����
							{
// //								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//ÿ�еĳ���
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng * Proto_Blk.opobj_data[i].sub_index[1]);			//����һ���еĳ��� // abcdefg //
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
							}
							else  				 						//  �����Ӷ���
							{	 
// // 								msg_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//�����Ӷ���Ĵ�С
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng * Proto_Blk.opobj_data[i].sub_index[1]);		// ����һ���еĳ��� // abcdefg //
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;								
							}
								
					}
					else
					{
						/*  ������������  */
					}
		}
	}
	
	
		else 																					//PC�˲����Ե�����  ��   ����  
		{
			return (ERROR_OTHER);	
		}

	}


	if(*p_Rx_index > rx_count)   // �жϳ��������ݴ���ʵ�ʽ��յ������� 
	{
		*p_Rx_index = rx_count;
		return (ERROR_OTHER);
	}

	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    


	return (ERROR_NO);

}



extern void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);   // debug_lm
extern void eth_set_time	(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);		// debug_lm
extern void eth_read_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);
extern void eth_set_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);
extern u8  copy_to_eth(u8 *p_Data,u32 lenth);
//extern const u8 ID_type[][2];																					// debug_lm
/* �����ʶ���ͱ� */
#define ID_ALL 						0			/*  �����Բ���  */
#define ID_DB							1			/*  ���ݿ����  */
#define ID_NEVER_SET 			2			/*  ��Զ�������ã����ñ������Բ�ѯ */
#define ID_NO_SET 				3			/*  �������ã������ñ������Բ�ѯ  */
#define ID_TIME		 				4			/*  ʱ�����ݲ�д�����ݱ�  */
#define ID_REAL_TIME			5			/*  ʵʱ�������ݲ�д�����ݱ� */
#define ID_PARAMETER			6			/*  �������ñ���ʱ�������ã��ɲ�ѯ������һ����֧ */
#define ID_REPORT					7			/*  д�뱨����־������,�������ã������ñ����ܲ�ѯ */

#define ID_92_NEVER_SET	  8

extern u8 	rt_operate_written(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth);	//debug_lxb
extern u8		rt_operate_read(PROTOCOL_DATA *p_Protocol,to_eth_struct * p_To_eth);					//debug_lxb

extern void eth_read_line_only_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);

extern u32 DB_Read_92(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);

extern u32 DB_Check_C1_M(TARGET_DATA *p_Target);

BOOL proto_explain_lm(PROTOCOL_DATA *p_Protocol ,to_eth_struct * p_To_eth)
{
	u32   i                		= 0;
	u32 lenth_92							= 0;
	u8 ID_NO1;
	u8 ID_temp;
	
	u8 temp;

	u8 *   msg_to_dispatch = NULL;			//����������Ϣ����
	
 	static u8		to_dispatch_msg[5];		//���͸���������������Ϣ

	u8		 trigger_dispatch_temp;			//8d��8e��c0��95����������������ı�ʶ
	
	/*  У��ID�Ƿ�����ͬһ������   */
	ID_temp =p_Protocol->opobj_data[0].ID;

	if(ID_temp == ROAD_MAP_ID)   ////��ʶ ROAD_MAP_ID ����0X70 //mm20140509)
  {
		if(p_Protocol->count_target > 1)  //������� 0x70 ��������� mm20140509)
		{
			p_To_eth->error_type=ERROR_OTHER;
			goto DO_ERROR;
		}
		if(p_Protocol->operation_type ==0) 								/*   ��ѯ����   */
		{
			for (i=0; i<p_Protocol->count_target; i++)
				{
					eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);
					if(p_To_eth->error ==__TRUE)
					{
						goto DO_ERROR;
					}
				}			
		}
		else if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
		{
			for (i=0; i<p_Protocol->count_target; i++)
				{
					eth_set_dbase ( &(p_Protocol->opobj_data[i]) ,  p_To_eth);
					if(p_To_eth->error ==__TRUE)
					{
						goto DO_ERROR;
					}
				}
		}
		
  }
	else  //mm20140509)
	{ //mm20140509)
		ID_NO1  =Object_table[ID_temp-0x81].ID_type;          //abcdefg  //ID_type[ID_temp-0x81][1];//
		for (i=0; i<p_Protocol->count_target; i++)
		{
			ID_temp =p_Protocol->opobj_data[i].ID;
			if((Object_table[ID_temp-0x81].ID_type ==ID_NO1) || //abcdefg  //ID_type[ID_temp-0x81][1]//
				 (Object_table[ID_temp-0x81].ID_type ==ID_ALL) )  //abcdefg  //ID_type[ID_temp-0x81][1]//
			{
				temp++;   
			}
			else 
			{
				p_To_eth->error_type=ERROR_OTHER;
				goto DO_ERROR;
			}
		}
	//----------------------------------
		switch (ID_NO1)
		{

				case ID_ALL:
				{
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);
									
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								if(1)//DB_Check_C1_M( &(p_Protocol->opobj_data[i])) == RIGHT)
								{
									eth_set_dbase ( &(p_Protocol->opobj_data[i]) ,  p_To_eth);
									if(p_To_eth->error ==__TRUE)
									{
										goto DO_ERROR;
									}
									else
									{
										if (p_Protocol->opobj_data[i].ID == 0xC1)
										{
											change_8D_8E_C0_C1_flag =1;
											/* Э���������������������	*/
											to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
											msg_to_dispatch	= to_dispatch_msg;
											os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
										}
									}
								}
								else
								{ 
									p_To_eth->error_type=ERROR_OTHER;
									goto DO_ERROR;
								}
							}
						}
						break;
					}
			
			case ID_DB:
					{
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);
								
	// 							tsk_lock ();  			//  disables task switching 
	// 							RECEIVE_LED_ON();   //liming
	// 							tsk_unlock ();  			//  disables task switching 
									
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_set_dbase ( &(p_Protocol->opobj_data[i]) ,  p_To_eth);
								if(p_To_eth->error ==__TRUE)
								{
									goto DO_ERROR;
								}
								else
								{
									trigger_dispatch_temp = 0;
									if ((p_Protocol->opobj_data[i].ID == 0x8D) ||
											(p_Protocol->opobj_data[i].ID == 0x8E) ||
											(p_Protocol->opobj_data[i].ID == 0xC0) ||
											(p_Protocol->opobj_data[i].ID == 0xC1) ||
											(p_Protocol->opobj_data[i].ID == 0xC8) ||
											(p_Protocol->opobj_data[i].ID == 0x95) )
									{
										change_8D_8E_C0_C1_flag =1;
										/*�������ݱ����£����µ���ִ���µķ���*/
										
										trigger_dispatch_temp = 1;
										/* Э���������������������	*/
										to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
										msg_to_dispatch	= to_dispatch_msg;
										os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
									}
								}
									
							}
							
							if (trigger_dispatch_temp == 1)
							{
								/* Э���������������������	*/
								to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
								msg_to_dispatch	= to_dispatch_msg;
								os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
							}
						}
						break;
					}
			

			case ID_NEVER_SET:
					{ 
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{ 
								switch (ID_temp)
									{
										case 0x85: 
													eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);
										//eth_read_line_only_dbase( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
											break;
										case 0x91: 
											{
												p_To_eth->error = __TRUE;
												p_To_eth->error_type = ERROR_OTHER;
												goto DO_ERROR;
												break;
											}
										default:	
											eth_read_dbase( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
											break;
									}
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else //if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}
			case ID_NO_SET:
					{
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								if(p_Protocol->opobj_data[i].ID == 0xB1) 
								{								
									 if(p_Protocol->opobj_data[i].count_index == 0)
									{
										
										p_To_eth->data[p_To_eth->lenth++]	= p_Protocol->opobj_data[i].ID;
										p_To_eth->data[p_To_eth->lenth++]	= 0;
										p_To_eth->data[p_To_eth->lenth++]	= 2;									
										//TAB_B1
										os_mut_wait (Tab_B1_Mutex,0xfffe);//�ȴ�������
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].index;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].redStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].yellowStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].greenStat;
										
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].index;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].redStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].yellowStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].greenStat;
										os_mut_release (Tab_B1_Mutex);//�ͷŻ�����
										
									}
									else
									{
										p_To_eth->error = __TRUE;
										p_To_eth->error_type = ERROR_OTHER;
										goto DO_ERROR;
									}
								}
								else
								{
									eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);								
									if(p_To_eth->error ==__TRUE)goto DO_ERROR;
								}
							}
						}
						else //if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}
			case ID_TIME:
					{
						//if (red_to_auto ==__TRUE)  //����ȫ��֮ǰ����������ʱ��
						//{
									if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
									{
										for (i=0; i<p_Protocol->count_target; i++)
										{
												eth_read_time( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
												if(p_To_eth->error ==__TRUE)goto DO_ERROR;
										}
									}					
									else if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
									{
										if (red_to_auto ==__TRUE)  //����ȫ��֮ǰ����������ʱ��
										{
													for (i=0; i<p_Protocol->count_target; i++)
													{
														eth_set_time ( &(p_Protocol->opobj_data[i]) ,  p_To_eth);
														if(p_To_eth->error ==__TRUE)
														{
															goto DO_ERROR;		
														}
														else
														{
																/* Э���������������������	*/
																to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
																msg_to_dispatch	= to_dispatch_msg;
																os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
														}
													}
										}
										else
										{
											p_To_eth->error = __TRUE;
											p_To_eth->error_type = ERROR_OTHER;
											goto DO_ERROR;	
										}
									}						
						//}					
						break;
					}
			case ID_REAL_TIME:
					{
						if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							if (rt_operate_written(p_Protocol,  p_To_eth) == __FALSE)	
							{	//�����˲������õ�ID�����ش���
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;	
							}
						}
						else if(p_Protocol->operation_type ==0) 								/*   ��ѯ����   */
						{
							if (rt_operate_read(p_Protocol,  p_To_eth) == __FALSE)	
							{//��ѯ�˲��ò�ѯ��ID�����ش���
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;						
							}
						}
						break;
					}
			case ID_PARAMETER:
					{
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);								
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else //if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}	
			case ID_REPORT:
					{
						if(p_Protocol->operation_type ==0)  										/*   ��ѯ����   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else if(p_Protocol->operation_type ==1) 								/*   ���ò���   */
						{						
							for (i=0; i<p_Protocol->count_target; i++)
							{
								switch (ID_temp)
									{
										case 0x97:
										{
											eth_set_dbase ( &(p_Protocol->opobj_data[i]) ,  p_To_eth);
											if(p_To_eth->error ==__TRUE)
											{
												goto DO_ERROR;
											}
											break;
										}
										
										default:
										{
											p_To_eth->error = __TRUE;
											p_To_eth->error_type=ERROR_EXCEED;
											goto DO_ERROR;
											break;
										}
										
									}
							}						

						}
						else
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type=ERROR_EXCEED;
							goto DO_ERROR;
						}
						break;
					}
			case ID_92_NEVER_SET:/* 92����־ �������� */
					{
						if(p_Protocol->count_target != 1)
							{
								p_To_eth->error = __TRUE;
								p_To_eth->error_type=ERROR_EXCEED;
								goto DO_ERROR;
							}
			
							if((p_Protocol->opobj_data[0].count_index == 0)&&(p_Protocol->opobj_data[0].sub_target == 0))
							{
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;
							}
							
							if((p_Protocol->opobj_data[0].count_index == 2)&&(p_Protocol->opobj_data[0].sub_index[1] != 0))
							{
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;
							}

							if(p_Protocol->opobj_data[0].sub_target != 0)
							{
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;
							}					
							
							
						if(p_Protocol->operation_type ==0) 								/*   ��ѯ����   */
						{
								p_To_eth->data[0] =((p_Protocol->count_target-1) <<4) | (p_Protocol->operation_type) | 
																		(0x04) | (0x80);
								p_To_eth->lenth			= 1 ;			

							
								for (i=0; i<p_Protocol->count_target; i++)						
								{
									os_mut_wait(Spi_Mut_lm,0xFFFF);
									
									lenth_92 = DB_Read_92(&p_Protocol->opobj_data[i] , p_To_eth);  // abcdefg //


									
									os_mut_release(Spi_Mut_lm);	
									if(lenth_92 == 0)
									{
										p_To_eth->error = __TRUE;
										p_To_eth->error_type = ERROR_OTHER;
										goto DO_ERROR;
									}


								}
								if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	�Ѿ���ȷ���͵�ETH-FIFO */  
								{
									os_sem_send(eth_send_data_semaphore);	
								}
								return (__TRUE); 

						}
						else
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						
					}
	//				break;				
			default:
				break;
		}
		
		
		
		if(ID_temp != 0x92)
			{
			if(p_To_eth->lenth > SUB_TARGET_BUFSIZE)  //   liming_?
				goto DO_ERROR;
			}
			if(p_To_eth->error ==__TRUE)
				goto DO_ERROR;
		 
	}   //mm20140509)
	
//==================================================================================
//------------------------  "right"  send to eth   start  -----------------------------------------------
	/*	��Ϣ�����ֽ�	����0x04Ϊ�·������ϴ�����Ϣ����֮��	*/
	p_To_eth->data[0] =((p_Protocol->count_target-1) <<4) | (p_Protocol->operation_type) | 
											(0x04) | (0x80);							
	
	if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	�Ѿ���ȷ���͵�ETH-FIFO */  
	{
		os_sem_send(eth_send_data_semaphore);	
	}
	else
	{
		/*  reserve  */
	}
	return (__TRUE);   
	
//------------------------  "right"  send to eth   end 	  -----------------------------------------------

	
//------------------------  "error"  send to eth   start  -----------------------------------------------
DO_ERROR:	

	p_To_eth->error =__FALSE;
	/*	��Ϣ�����ֽ�	����0x06Ϊ�·������ϴ�����Ϣ����֮��	*/
	p_To_eth->data[0] =((p_Protocol->count_target-1) <<4) | (0x06)| (0x80);							
	p_To_eth->data[1] =p_To_eth->error_type;
	p_To_eth->lenth		=2;
	
	if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	�Ѿ���ȷ���͵�ETH-FIFO */  
	{
		os_sem_send(eth_send_data_semaphore);	
	}
	else
	{
		/*  reserve  */
	}
	
	return (__FALSE);
//------------------------  "error"  send to eth   end  -----------------------------------------------

}

//��̫���������ݸ��º���
u8	Protocol_Flag				= 0;    //debug_lm
__task void DB_Update_Task(void)
{	
	u8 							*msg_send;
 	u8 			Type;
	u32  						i;

//	u8  						Uart_Control[10]					={0};    //  ���ڴ��ڵ�Э�鴦��
//  u8							Uart_Idex									= 0;

  to_eth_struct 	to_eth										={{0}, {1},{__FALSE},{ERROR_NO}};   //������̫�������� 

	u8*							p_Temp 										=NULL;

//	u8 send_temp[2]={0};

	for(;;)
	{
		os_mbx_wait (DB_Update_Mbox, (void *)&msg_send, 0xffff);
		
		#ifdef LIMING_DEBUG_12_23
			printf("-DB_Up-\n");
		#endif	
		tsk_DB_run= (__TRUE);//ι��

		Type = *msg_send;
// 		SEND_LED_ON();
		
// // // 		send_temp[0]=(uint8_t)SEND_LED;          			// LED���
// // // 		send_temp[1]=(uint8_t)LED_ON;												// ����״̬			
// // // 		os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
					
		if(Type == 1)
		{
					do{				
							// ----------------------- Read From FIFO start ------
							tsk_lock();           	 //  disables task switching 
							if(rx_counter !=0)
							{
								if((eth_temp_lenth + rx_counter) >= ETH_TEMP_SIZE)
								{
									for(i=0;i<(ETH_TEMP_SIZE-eth_temp_lenth);i++)   
									{
										eth_temp_data[eth_temp_lenth+i] = eth_rx_fifo[rx_rd_index];
										if (++rx_rd_index == RX_FIFO_SIZE) rx_rd_index=0;
									}
									eth_temp_lenth =ETH_TEMP_SIZE;  //  �õ��������ݰ��ĳ���
									rx_counter = rx_counter -(ETH_TEMP_SIZE-eth_temp_lenth);
								}
								else    
								{
									for(i=0;i<rx_counter;i++)   
									{
										eth_temp_data[eth_temp_lenth+i] = eth_rx_fifo[rx_rd_index];
										if (++rx_rd_index == RX_FIFO_SIZE) rx_rd_index=0;
									}
									eth_temp_lenth += rx_counter;  //  �õ��������ݰ��ĳ���
									rx_counter=0;   /*   ���  ETH_FIFO  */
								}
							}
							tsk_unlock();            //  enable  task switching 
							//-------------------------Read From FIFO end  ------
	
							Protocol_Flag=proto_explain_check(eth_temp_data,&eth_temp_current, eth_temp_lenth);  
							
							if(Protocol_Flag ==0)
							{
								proto_explain_lm( &Proto_Blk, &to_eth);  //Э�����
							}
							else 
							{					
									to_eth.error =__FALSE;
								
									/*	��Ϣ�����ֽ�	����0x06Ϊ�·������ϴ�����Ϣ����֮��	*/
									to_eth.data[0] =(0x06)| (0x80);							
									//to_eth.data[1] =to_eth.error_type;
									to_eth.data[1] =Protocol_Flag;
								
									to_eth.lenth	 =2;
									
									//if(copy_to_eth(&to_eth.data, to_eth.lenth) ==0)/*	�Ѿ���ȷ���͵�ETH-FIFO */  
									if(copy_to_eth(to_eth.data, to_eth.lenth) ==0)/*	�Ѿ���ȷ���͵�ETH-FIFO */ 
									{
										os_sem_send(eth_send_data_semaphore);	
									}
									else
									{
										/*  reserve  */
									}

								Protocol_Flag=0;
							}
						//-----------			clear the Proto_Blk	start	------------------------							
							p_Temp=(u8*)(&Proto_Blk);
							for(i=0;i<sizeof(PROTOCOL_DATA);i++)
							{
								*(p_Temp++)=0;
							}
						//-----------			clear the Proto_Blk	end	------------------------
							
						//-----------			clear the to_eth	start	------------------------							
							p_Temp=(u8*)(&to_eth);
							for(i=0;i<sizeof(to_eth_struct);i++)
							{
								*(p_Temp++)=0;
							}	
						to_eth.lenth=1;						
						//-----------			clear the to_eth	end	------------------------														
							
							//-------------------------���Ʋ���   start   -------------
							for(i=0;i<(eth_temp_current);i++) eth_temp_data[i]=0;
							
							for(i=0;i<(eth_temp_lenth-eth_temp_current);i++)    
							{
								eth_temp_data[i]=eth_temp_data[ i + eth_temp_current];
								eth_temp_data[ i + eth_temp_current]=0;
							}			

							if(eth_temp_lenth <=eth_temp_current)
							{
								for(i=0;i<(eth_temp_lenth);i++) eth_temp_data[i]=0;
								eth_temp_lenth=0;
								break;
							}
							else eth_temp_lenth -= eth_temp_current; 	
							//-------------------------���Ʋ���   end   -------------
							

							
					}		while(eth_temp_lenth);	
		} //end of "if(Type == 1)"

// 		SEND_LED_OFF();
// // // 		send_temp[0]=(uint8_t)SEND_LED;          			// LED���
// // // 		send_temp[1]=(uint8_t)LED_OFF;												// ����״̬			
// // // 		os_mbx_send(mailbox_led,(void*)send_temp,0xFF);

		tsk_DB_over= (__TRUE);//ι��   
	}
}

__task void Write_Log_Task(void)
{
	u8 *msg;
	u8 Log_Type;
	u8 Log_Count;
	u8 Word_data[21];
	u32 i,j,k;
	
	
	static	u8 read_temp[21];

	//u8 semaphore_log_count=0;
	
	
// 	u32 	            address_temp 				= 0;
// 	u32 	            old_address 				= 0;
	u32 	            Data_addr_temp 				= 0;
	u32 	            Data_addr_old 				= 0;
	u32 	            Flag_addr_temp 				= 0;	
	u32 	            Flag_addr_old 				= 0;	
	u32								No_line_temp				= 0;	//�ڼ���
	u32								Index1_lines_count  = 0;
	u32								Index2_lines_count  = 0;
	u32								Index2_lines_count_temp  = 0;

	u8								line_bytes_temp			= 0;	//ÿ���ֽ���

	u8							  ID_temp						  =0;
	u8							  count_index_temp		=0;
	u8							  sub_target_temp		  =0;
	
	u8							  index1_temp		      =1;
	u8							  index2_temp		      =0;

	u8  time_temp[4]={0};
	u8  write_temp[20]={0};
  
	
	u32  occur_time=0;


	//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
	
	ID_temp=0x92;
	count_index_temp	  	=2;
	sub_target_temp	  		=0;
	write_log_rd_index=0;
	
//	old_address	= Object_table[ID_temp-0X81].Data_Addr;//��õ�ַ

	Data_addr_old	= Object_table[ID_temp-0X81].Data_Addr;//������ݵ�ַ
	Data_addr_temp = Data_addr_old;
	Flag_addr_old = Object_table[ID_temp-0X81].Flag_Addr;//��ñ�־��ַ
	Flag_addr_temp = Flag_addr_old;
	
	for(;;)
	{

		//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    

		if(os_mbx_wait(Write_Log_Mbox, (void *)&msg, 0xffff) ==OS_R_OK );
		{
			
			tsk_log_run=(__TRUE);
			
			#ifdef LIMING_DEBUG_12_23
				printf("-Write_L-\n");
			#endif	
			
			tsk_lock();           	 //  disables task switching 
			if(write_log_counter !=0)
			{
				for(i=0;i<21;i++)   //�Ƚ��Ƿ��ظ�����
				{
					if(read_temp[i] == write_log_wr_fifo[write_log_rd_index][i])  k++;
	
				}
				for(i=0;i<21;i++)    //  ���յ������е����ݿ�����temp���棬
				{
					read_temp[i] = write_log_wr_fifo[write_log_rd_index][i];
					write_log_wr_fifo[write_log_rd_index][i] = 0;	   //���ݶ������ڴ���0
				}
				if (++write_log_rd_index > WRITE_LOG_SIZE) write_log_rd_index=0;
					
					--write_log_counter;
				tsk_unlock(); 
				if(k==21)
				{
					k=0;
					//�ظ����ݲ�����
				}
				else 
				{
					k=0;
					os_mut_wait(I2c_Mut_lm,0xFFFF);
					occur_time = Read_DS3231_Time();//�¼�����ʱ��//����
					os_mut_release(I2c_Mut_lm);
						
					time_temp[0] = (u8)((occur_time & 0xFF000000) >> 24);
					time_temp[1] = (u8)((occur_time & 0x00FF0000) >> 16);
					time_temp[2] = (u8)((occur_time & 0x0000FF00) >> 8);
					time_temp[3] = (u8)( occur_time & 0x000000FF);	

					Log_Type = 			read_temp[0];
					Word_data[0] = read_temp[1];
					Word_data[1] = read_temp[2];
					Word_data[2] = read_temp[3];
					Word_data[3] = read_temp[4];	
					Data_addr_temp	=	Data_addr_old+1;	//��������ǰһ�������ֽ�
//				address_temp	=	old_address	+3;	//��������ǰ������־�ֽ�
					index1_temp 	=	Log_Type;
					os_mut_wait(Spi_Mut_lm,0xFFFF);	
					index2_temp  	= Rd_u8(Log_Type - 1 + INDEX_92_ADDR_START); //((Log_Type-1)*(255*(10+2))+(old_address+3) +1 );
	//				index2_temp  	= Rd_u8((Log_Type-1)*(255*(10+2))+(old_address+3) +1 );
					os_mut_release(Spi_Mut_lm);
					
					if(index2_temp ==255)	
					{
						index2_temp=1;
					}					
					else 
					{					
						index2_temp++;
					}
	//=========================================		д92��		
					write_temp[0]=index1_temp;
					write_temp[1]=index2_temp;
					
					write_temp[2]=time_temp[0];
					write_temp[3]=time_temp[1];
					write_temp[4]=time_temp[2];
					write_temp[5]=time_temp[3];
					
					write_temp[6]=Word_data[0];
					write_temp[7]=Word_data[1];
					write_temp[8]=Word_data[2];
					write_temp[9]=Word_data[3];


					if(count_index_temp 	== 2)  			
					{ 
						os_mut_wait(Spi_Mut_lm,0xFFFF);	
						if(index1_temp <= 64)
						{			
							No_line_temp=(index1_temp-1)*255+index2_temp;	//Ӧ��д���ʵ���к�			
							//line_num_temp=1;					
							line_bytes_temp  = 10;	//������ֽ���
							
							if(sub_target_temp ==0)  													//����
							{	
								Wr_u8(Flag_addr_temp,TAB_OK_FLAG);						//д�����־λ		
								Wr_u8((Flag_addr_temp + No_line_temp),LINE_OK_FLAG) ;//д�б�־λ
	//							Wr_u8((old_address+3+(No_line_temp-1)*(2+line_bytes_temp)),line_OK_flag_temp) ;//д�б�־λ

								Data_addr_temp += (No_line_temp-1)*(line_bytes_temp);	//����ǰ��N-1�еĵ�ַ
								
	//							address_temp += 2;										                //�������е�  ��Ч��־ ����
								for(j=0;j<line_bytes_temp;j++)
								{
									Wr_u8(Data_addr_temp++,write_temp[j]) ;	
								}
								Wr_u8((Log_Type - 1 + INDEX_92_ADDR_START ),index2_temp); //=>>д���ڼ�������2����ˮ�ţ�  ���ٿռ�
								//Wr_u8(((Log_Type-1)*(255*(10+2))+(old_address+3) +1 ),index2_temp); //=>>д���ڼ�������2����ˮ�ţ�  ���ٿռ�

							}
							
						}
						//==========line ��������================================
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
								Index2_lines_count_temp = 0;
							}
						}
						Wr_u8(Data_addr_old , Index1_lines_count );					//д����һ��Ч����

						os_mut_release(Spi_Mut_lm);
									
					}

				//========================================================TCP�ϱ�

					//��ӷ��ͱ�������
					client_data_type = 2;//��������  2--��������

					tsk_lock ();  			//  disables task switching 
					write_Client_fifo[write_Client_wr_index][0] = 0x83;
					write_Client_fifo[write_Client_wr_index][1] = Word_data[0];
					write_Client_fifo[write_Client_wr_index][2] = Word_data[1];
					write_Client_fifo[write_Client_wr_index][3] = Word_data[2];
					write_Client_fifo[write_Client_wr_index][4] = Word_data[3];

					for(j = 5;j < 12;j++)
						write_Client_fifo[write_Client_wr_index][j] = 0;				

					if (++write_Client_wr_index == WRITE_Client_SIZE) write_Client_wr_index=0;
												
					
					if (++write_Client_counter  == WRITE_Client_SIZE)
					{
						write_Client_counter=1;
						write_Client_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 	

					os_sem_send(eth_client_sche_semaphore);
					os_dly_wait(10);
						
					}
				}
			tsk_log_over=(__TRUE);
		}
	}	

}

	CAN_ERROR   can_error_lm ;
__task void task_send_CAN (void)
{

	for (;;) 
	{
		os_evt_wait_and(0x0008 , 0xffff);
		
// 		os_mut_wait (Tab_B1_Mutex,0xfffe);//�ȴ�������
//  		Send_Channel_Output_Unit(Tab_B1[1]);
//  		Send_Channel_Output_Unit(Tab_B1[2]);
// 		os_mut_release (Tab_B1_Mutex);//�ͷŻ�����

	}
}
	uint32_t  *ppp=NULL;  //  debug


__task void task_rece_CAN (void)
{
	CAN_msg RxMessage;
//	CAN_msg CAN_Rx_MSG[4][3] 										= {0};
		
//	uint32_t   j = 0;
//	u16 CAN_Error_Msg_Count = 0;
//	u8 CAN_Error_Data[12] = {0};
//	u8 old_id = 0;//����id
//	u8 error_id[8] = {0};//�����id
//	u8 error_id_count = 0;//����

//	u8	count_for_degrade = 0;

	u8 *msg_send;

	//���ϵƺ�
//	U8 err_light_no = 0;
	u8 send_temp1[2]={0};

// U8 xx=0;
	CAN_init (1, 500000);               /* CAN controller 1 init, 100 kbit/s   */

  CAN_rx_object (1, 2,  33, DATA_TYPE | STANDARD_TYPE); /* Enable reception  */

  CAN_hw_testmode(1, CAN_BTR_SILM ); /* Normal         											*/

  CAN_start (1);                      /* Start controller 1   */


	for (;;) 
	{

		//IWDG_ReloadCounter();   /* ι��m&l 	IWDG_LM */    
		tsk_CAN_run=(__TRUE);
		
		/* When message arrives store received value to global variable Rx_val   */
		if (CAN_receive (1, &RxMessage, 0x00FF) == CAN_OK)  
		{
			#ifdef LIMING_DEBUG_12_23
				printf("-rece_CAN-\n");
			#endif				
			
			/*������lxb9.2���ݰ��ͨ��Э�����Э�����޸ĵĲ���*/

			if( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xE1||RxMessage.data[1] == 0xE2||RxMessage.data[1] 
						== 0xE3||RxMessage.data[1] == 0xA7)) //
			{				
					//д��־START===========
					tsk_lock ();  			//  disables task switching 
					if(RxMessage.data[1] == 0xE1) 		 write_log_wr_fifo[write_log_wr_index][0] =Type_E1;
					else if(RxMessage.data[1] == 0xE2) write_log_wr_fifo[write_log_wr_index][0] =Type_E2;
					else if(RxMessage.data[1] == 0xE3) write_log_wr_fifo[write_log_wr_index][0] =Type_E3;
					else if(RxMessage.data[1] == 0xA7) write_log_wr_fifo[write_log_wr_index][0] =Type_A7;				

					write_log_wr_fifo[write_log_wr_index][1] = RxMessage.data[1];   //�¼�ֵ1  �¼���ʶ
					write_log_wr_fifo[write_log_wr_index][2] = RxMessage.data[3];		//�¼�ֵ
					write_log_wr_fifo[write_log_wr_index][3] = RxMessage.data[4];   //�¼�ֵ
					write_log_wr_fifo[write_log_wr_index][4] = RxMessage.data[5];   //�¼�ֵ				
								
					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
												
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
											//д��־END===========

				send_temp1[0]=(uint8_t)ERROR_LED;          			// LED���
				send_temp1[1]=(uint8_t)LED_ON;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);
			}
			else if( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xA6)) //����״̬��
			{				
					//д��־START===========
					tsk_lock ();  			//  disables task switching 

					write_log_wr_fifo[write_log_wr_index][0] =Type_A6;		 //�¼����ͱ��	
					write_log_wr_fifo[write_log_wr_index][1] = RxMessage.data[1];   //�¼�ֵ1  �¼���ʶ
					write_log_wr_fifo[write_log_wr_index][2] = RxMessage.data[3];		//�¼�ֵ
					write_log_wr_fifo[write_log_wr_index][3] = RxMessage.data[4];   //�¼�ֵ
					write_log_wr_fifo[write_log_wr_index][4] = RxMessage.data[5];   //�¼�ֵ
		
								
					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
												
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
											//д��־END===========
							
				send_temp1[0]=(uint8_t)ERROR_LED;          			// LED���
				send_temp1[1]=(uint8_t)LED_ON;												// ����״̬			
				os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);
			}

			if ( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xA7)&& (RxMessage.data[3] == 0xFF)&& (RxMessage.data[5] == 0x01))//�����豸
				{
//					tsk_lock();
				}
		}
		tsk_CAN_over=(__TRUE);
	}
}


