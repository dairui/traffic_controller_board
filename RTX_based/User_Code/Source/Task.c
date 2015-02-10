#include "Include.h"

u8	Sche_Dispatch_MSG	[5] = {0};					//方案调度任务消息
u8	Sche_Execute_MSG	[5] = {0};					//方案执行任务消息
u8	Sche_Data_Update_MSG[5] = {0};				//方案数据任务
u8	Sche_Interim_MSG[5] = {0};						//过渡方案

u8	Degrade_MSG	[5] = {0};								//降级任务消息

OS_ID  	tmr1_yellow_flicker;
OS_MUT	Mutex_Data_Update;
OS_MUT	Tab_B1_Mutex;
OS_MUT	Output_Data_Mut;
OS_MUT 	RT_Operate_Mut;
OS_MUT 	I2c_Mut_lm;
OS_MUT 	Spi_Mut_lm;
OS_MUT 	Stage_Count_Mut; 			//用于阶段配时表各灯色时间计数值 BY lxb1.13

OS_SEM eth_client_sche_semaphore;
OS_SEM eth_send_data_semaphore;

os_mbx_declare(Sche_Output_Mbox, 			20);	
os_mbx_declare(Sche_Dispatch_Mbox, 		20);						//方案 调度	任务
os_mbx_declare(Sche_Data_Update_Mbox, 20);						//方案 数据 任务
os_mbx_declare(Sche_Execute_Mbox, 		20);						//方案 执行 任务
os_mbx_declare(Write_Log_Mbox, 				20);
os_mbx_declare(DB_Update_Mbox, 				20);
os_mbx_declare (mailbox_led, 20);//led_task

OS_TID t_maintask;									//初始化任务
OS_TID t_DB_Update_Task;						//数据库更新任务
OS_TID t_Write_Log_Task;						//记录事件日志任务
OS_TID t_Sche_Dispatch_Task;				//方案调度任务
OS_TID t_Sche_Data_Update_Task;			//方案数据任务
OS_TID t_Sche_Execute_Task;					//方案执行任务
OS_TID t_Sche_Output_Task;					//方案输出(计数器任务)  //以前的心跳  //by_lm   
// OS_TID t_Stagecount_Input_Task;			//计数值赋值任务lxb
OS_TID t_tick_timer; 
OS_TID t_tcp_task; 
OS_TID t_task_rece_CAN;
OS_TID t_led;                           /* assigned task id of task: led     */
OS_TID t_matrix_keyboard;               /* assigned task id of task: matix_keyboard */

//  栈必须以8字节为边界对齐，以U64类型声明(无符号的长整型)。    
U64 DB_Update_Stack									[1000];
U64 Write_Log_Stack									[300];
U64 tcp_stack												[200];
U64 Sche_Dispatch_Stack							[400];			//方案 调度 任务
U64 Sche_Data_Update_Stack					[600];			//方案 数据 任务
U64 Sche_Execute_Stack							[400];			//方案 执行 任务
U64 Sche_Output_Stack								[400];			//方案 执行 任务
// U64 Stagecount_Input_Stack					[50];				//计数器任务
U64 CAN_Rece_Stack									[200];			//方案 执行 任务
U64 Led_Stack												[50];			//方案 执行 任务
U8 soc_state_lm;

 U8 TT_cont1;    //计数器用于防止数据重发
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

#define SCHE_NUM_BKPRAM_ADDR  	(BKPSRAM_BASE)				/*	方案调度数组 在BKPRAM中的起始地址	*/  //	BKPRAM_LM
#define NOW_STAGE_BKPRAM_ADDR  	(BKPSRAM_BASE+50)    	/*	当前阶段 在BKPRAM中的起始地址	*/			//	BKPRAM_LM
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

// // 	tmr1_yellow_flicker = os_tmr_create (ETH_CLIENT_TIC_TIMES, 1);    // 创建一个定时器

	//主控板小灯控制
	send_temp[0]=(uint8_t)RUN_LED;          			// LED序号
	send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
	os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
	send_temp1[0]=(uint8_t)ERROR_LED;          			// LED序号
	send_temp1[1]=(uint8_t)LED_OFF;												// 工作状态			
	os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

/*
  以下三行代码表明，从所使用的 os_mbx_send()来看，现在只是把消息以指针的形式发送过去，
	也就是说，消息指针所指向的数据就是消息本身，而不是消息的地址，这样的话，最多只能发送4byte的消息。

	还有一点就是   接下来的任务中，等待Sche_Dispatch_Mbox的任蜸che_Dispatch_Task()是最先得到执行的。
	
*/
	if(IWDGRST_flag ==__TRUE)  			/*  看门狗复位	*/
	{
		Sche_Dispatch_MSG[0] = 4; 		/*	触发方案调度中的“方案周期结束”	*/
	}
	else 
	{
		Sche_Dispatch_MSG[0] = 1;			/*	触发方案调度中的“上电黄闪”	*/
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
	
		if(TT_cont1>0) TT_cont1--;   //计数器用于 保证数据短时间内重复发送
		if(TT_cont2>0) TT_cont2--;
		if(TT_cont3>0) TT_cont3--;
		if(TT_cont4>0) TT_cont4--;
		led_init();//重新配置PA8 pc9
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
				os_sem_wait(eth_send_data_semaphore,0);  /*  清除掉别人给他的信号  */
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
		tsk_tcp_run = (__TRUE);//喂狗
		
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
		if(semaphore_one_ok>0 )//服务器端口响应请求数据
		{
			if (tcp_check_send (tcp_soc_server)) 
			{
				semaphore_one_ok  -=1;
				
// 				RECEIVE_LED_ON();   //liming
				send_temp[0]=(uint8_t)RECEIVE_LED;          			// LED序号
				send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
				
				tsk_lock();           	 //  disables task switching 
				if(tx_len_counter !=0)    																 //得到待发数据的长度
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
							tx_counter -= (maxlen-1);   /*   清空  ETH_FIFO  */
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
							tx_counter -= tx_lenth_temp;   /*   清空  ETH_FIFO  */
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
									tx_counter -= (maxlen-1);   /*   清空  ETH_FIFO  */
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
									tx_counter -= tx_lenth_temp;   /*   清空  ETH_FIFO  */
								}
								tsk_unlock();            //  enable  task switching 				
								
								sendbuf_len = tx_lenth_temp;
								tcp_send (tcp_soc_server,sendbuf,sendbuf_len);
								
								tx_lenth_temp =0;
						}
					
				}
				

			}while(tx_lenth_temp !=0);
				
// 					RECEIVE_LED_OFF();    //  liming
				send_temp[0]=(uint8_t)RECEIVE_LED;          			// LED序号
				send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			}
		}	
		
		//  下面是心跳数据包发送部分-- 发数据时不发心跳  						
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
								//IWDG_ReloadCounter();   /* 喂狗 */  // IWDG_LM
						 break;
			}	//end of switch			

		}  //end of "if(Eth_client_tic_time_ovr ==1)"
		
		
		if(os_sem_wait(eth_client_sche_semaphore,0) ==OS_R_OK )semaphore_thr_ok +=1; 	//等待信号的到来
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
							if(write_Client_counter !=0)//仅仅只赋值一条??
							{
								for(i=0;i<12;i++)    //  把收到的所有的数据拷贝到sendbuf里面，
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
		
			if(semaphore_thr_ok>=WRITE_Client_SIZE)  //   以太网没能连接上，需要上传的数据条fifo满了。
			{
				semaphore_thr_ok=1;
				write_Client_buffer_overflow=__FALSE;
			}
		}				
		tsk_tcp_over = (__TRUE);//喂狗
		os_tsk_pass();				
  }
}


__task void led (void) 
{
	static Led_TypeDef 			leds =LED_NONE;  					//用于接收消息
	static Led_Run_TypeDef 	rec_run_type =LED_OFF;		//用于接收消息
	
	static Led_Run_TypeDef 	run_type[HAND_LED_NUM+LED_NUM];				//闪烁类型
//	static BOOL  						run_stats[HAND_LED_NUM+LED_NUM];      	//开、关
	
	u8 LED_Status_time = 0;
	u8 LED_Mark = 0;
	u8 Hand_LED_Mark = 0;
	
	uint8_t led_stats=0;	/* 用于保存前端LED状态 */
	uint16_t hand_led_stats=0;	/* 用于手动面板led的状态 */
	
	uint16_t sd2_sd11_status = 0;//SDLED2~SDLED11灯的组合状态(因各个灯互斥性，所以需组合状态)
	
	uint32_t   	i=0;
	uint8_t * 	receive=(uint8_t *)i;
	
  while(1)
	{
		os_mbx_wait (&mailbox_led, (void*)(&receive), 0xffff);
		// LED 任务的时钟是100ms
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
				//对于SD_LED2~SD_LED11,接收组合灯状态,并赋值给hand_led_stats相应位
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
			if (LED_Status_time<6) LED_Status_time++;  //闪烁计数器
			else LED_Status_time=0;
			
			//------------------前端led显示----------------
			run_type[0] = LED_RUN;
			run_type[1] = LED_OFF;//暂关闭
			run_type[2] = LED_OFF;//暂关闭
			run_type[3] = LED_OFF;//暂关闭
			run_type[4] = LED_OFF;//暂关闭
			run_type[5] = LED_OFF;//暂关闭
			run_type[7] = LED_OFF;//暂关闭
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
						if (LED_Status_time==1)  LED_Mark=~LED_Mark; //标志取反
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
						{//LED灭
								led_stats &= ~(0x01<<i);
						}
						break;							
					default:
						break;
				}			

			}	
			output_595(led_stats);	
			
			//------------手动面板led显示-----------------------
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
						if (LED_Status_time==1)  Hand_LED_Mark=~Hand_LED_Mark; //标志取反
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
						{//LED灭
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
	
	uint8_t  Enable_Keyboard_flag	= 0;	//使能手动面板标志，为1表示除F8键外的其他按键有效；反之无效
	
	u8 *msg_send;
	static u8 Uart_to_Sche_Dispatch_MSG[8] = {0};

	while(1)
	{
	//	ROWALL_OUTPUT();//输出所有行，等待中断触发
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
						if(count >=4)	//去抖10+4*10 = 50ms
						{
							count = 0;
							
							if((the_key_number == 7)&&(Enable_Keyboard_flag==0))//使能键F8按下，亮灯
							{
								send_temp[0]=(uint8_t)HAND_CTRL_LED;          			// 手动面板LED序号
								send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							if(the_key_number == 3)//正常键按下，亮灯
							{
								send_temp[0]=(uint8_t)NORMAL_CTRL_LED;          			// 手动面板LED序号
								send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							while(key_ReadInputNumber() == the_key_number)
							{
								os_dly_wait(10);//等待弹起
							}
							
							if((the_key_number == 7)&&(Enable_Keyboard_flag==0))//使能键F8弹起，灭灯
							{
								send_temp[0]=(uint8_t)HAND_CTRL_LED;          			// 手动面板LED序号
								send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							
							if(the_key_number == 3)//正常键弹起，灭灯
							{
								send_temp[0]=(uint8_t)NORMAL_CTRL_LED;          			// 手动面板LED序号
								send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
							}
							//key pressed
							if(the_key_number == 7)						//使能键F8按下，则对使能键盘标志Enable_Keyboard_flag置1
							{
								Enable_Keyboard_flag = 1;//手动
							}
							else
							{
									if(Enable_Keyboard_flag == 1)		//使能键盘标志Enable_Keyboard_flag=1，表示键盘可操作
									{
										switch(the_key_number)
										{
// // 											case 2:
// // 											{
// // 													break;
// // 											}
											case 1://全红
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 3;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
												
// 												printf("全红\n");
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
											case 2://闪光
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 2;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
// 												printf("黄闪\n");
												break;
											}
// // 											case 7:
// // 											{
// // 													break;
// // 											}
											case 6://步进
											{
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
												printf("步进\n");
												break;
											}
											case 3://正常
											{
												Enable_Keyboard_flag = 0;//使能键盘标志Enable_Keyboard_flag置0
												Uart_to_Sche_Dispatch_MSG[0] = 2;
												Uart_to_Sche_Dispatch_MSG[1] = 0;
												Uart_to_Sche_Dispatch_MSG[3] = 0xb5;
												msg_send = Uart_to_Sche_Dispatch_MSG;
												os_mbx_send(Sche_Dispatch_Mbox, msg_send, 0x10);
																	
												send_temp[0]=(uint8_t)HAND_CTRL_LED;  // 手动面板LED序号
												send_temp[1]=(uint8_t)LED_OFF;					// 工作状态			
												os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
												
// 												printf("正常\n");
												break;
												//自主多时段
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
				;//抖动
			}		
		}

	}

}

extern u8 	search_8D_all_timeTbId (time_struct *current_time,TIME_TABLE_t *table_0x8E_return);

/**
  * @brief  搜方案与控制方式，并写闹钟u8 Find_Scheme(u8 *sche_num_type)
  * @param  *sche_num_type,依次为方案号与控制方式
  * @retval 返回的u8无用
	* @invovled function
	*					search_8D_all_timeTbId (Read_Time_Data,&table_0x8E_return)
	*					I2C_Read_DS3231 (Read_Time_Data)
	*					I2C_Write_Alarm(Alram_Time_Data)
  */

TIME_TABLE_t Find_Scheme(time_struct *current_time)
{
	u8					read_pre_flag;												//往前搜索得到控制方式、方案号、闹钟的标志

	//用于保存从8d8e全表搜索后得到的控制方式、方案号以及闹钟的时与分
	TIME_TABLE_t table_0x8E_return = {0};	

	/*
	搜8d8e全表，输入当前时间，以及返回的结构体table_0x8E_return（具体上见定义）
	返回标志说明：
	read_pre_flag == 0，表明没查找到任何方案，降级
	read_pre_flag == 1，表明查找到了方案与当天的闹钟
	read_pre_flag == 2，表明查找到了方案，但该方案为改天最后时段的方案，所以闹钟应该写为下一天的0点
	*/
	read_pre_flag = search_8D_all_timeTbId (current_time,&table_0x8E_return);

	if(read_pre_flag == 0)
	{
	//降级处理
	table_0x8E_return.schemeId	= 	27;
	table_0x8E_return.ctlMode 	= 	0;
	return table_0x8E_return;
	}
	else 
	{
	//方案号与控制方式返回
	table_0x8E_return.schemeId	= 	table_0x8E_return.schemeId;
	table_0x8E_return.ctlMode		= 	table_0x8E_return.ctlMode;
	//写闹钟
	table_0x8E_return.minute		= 	table_0x8E_return.minute;
	table_0x8E_return.hour			= 	table_0x8E_return.hour;					
	return table_0x8E_return;
	}
}

/*说明
*本调度任务的核心数据即sche_change_temp[][]数组的操作，分为两部分：
*前部分为给sche_change_temp[][]填充数据（协议解析部分在外面填充，这里有待统一？），
 并通知方案数据更新任务更新数据；
*后部分为扫描sche_change_temp[][]，并填充当前方案sche_change_temp[NOW_C]；
 并通知副CPU执行相关操作以及写日志（写日志也可放在前半部分）
*任务最后为发消息操作，通知方案数据更新或者执行任务
*/
vu32 optimize_time_count = 0;     		//优化时长计数器
vu32 optimize_time_flag = 0;

vu8	red_to_auto						= __FALSE;		//开机全红后切换成自主多手段控制，只用一次

vu8 start_yellow_red_flag     = __TRUE;			//开机黄闪全红标识，全红后置0

/*若有故障，则一直降级*/
u8 degrade_always_flag = 0;


/*开机黄闪全红标志，时间*/
U8 start_yellow_flag = 0U;
U8 start_red_flag = 0U;
U8 start_yellow_time = 0U;
U8 start_red_time = 0U;

u8 phaseT_make = 0;

/* 相序表 */
U16 phase_order_t[PHASET_ARR_NO][5]= 
{
	/* 相序号     放行相位   放行时长   对应阶段号  相序时长 */
	0
};
__task void Sche_Dispatch_Task(void)
{
	
	//用于保存从8d8e全表搜索后得到的控制方式、方案号以及闹钟的时与分
	TIME_TABLE_t table_0x8E_return = {0};	
	//当前时间结构体
	time_struct			current_time = {0};

	u8  i,j;
//	u8	k = 0;
	u8	*msg								= NULL;												//接收指针
	u8	*msg_send						= NULL;
	u8	Message_Type 				= 0;		//表示待发送的任务的类型，如方案更新或方案执行任务// // 	u32	Sche_Index 					= 0;		//消息索引
	u8	Flash_Time 					= 0;		//黄闪时间
	u8	Red_Time 						= 0;		//全红时间
	u8	Change_Flag 				= 0;		//方案周期结束，改变方案标志，若改变，则发消息给方案更新任务
	u8 	Read_Time_Data[7] = {0};
	u8 	Alram_Time_Data[4] = {0};

	u32 									read_lenth_temp 	=		0;	
	TARGET_DATA 					data_Target;	

//  u8 Vice_Led_Ctrl[20] 					= {0};

/*临时变量数组，方案调度信息数组，与全局变量sche_change_tpye[][]对应*/
	u8  sche_change_temp[][4] =
	{
	/* 索引				控制类型		方案号			控制方式*/
		{0,					0,					0,					0},   /*	开机黄闪			*/
		{1,					1,					0,					0},		/*	开机全红			*/
		
		{2,					2,					0,					0},		/*	步进	0xBB		*/
		{3,					3,					0,					0},		/*	手动  0xB5 		*/
		{4,					4,					0,					0},		/*	系统  0xB6		*/
		{5,					5,					0,					0},		/*	自主控制			*/
		{6,					6,					0,					0},		/*	黄闪控制	*/
		{7,					0,					0,					0},		/*	当前控制方案	*/
	};

/*触发类型，1为开机黄闪触发	
	触发类型，2为协议解析触发	
	触发类型，3为闹钟触发	
	触发类型，4为方案周期结束触发	*/
	u8	trigger_type 					= 0;
/*控制方式，1为开机黄闪触发	
	控制方式，2为协议解析触发	
	控制方式，3为闹钟触发	
	控制方式，4为方案周期结束触发	*/	
	u8	rx_control_way 				= 0;
/*方案号，1为开机黄闪触发	
	方案号，2为协议解析触发	
	方案号，3为闹钟触发	
	方案号，4为方案周期结束触发	*/	
	u8	rx_sche_number 				= 0;	
	u8	start_red_enable			= 0;		//开机全红使能标志，开机黄闪后置1，闹钟触发全红后清0
	u8  sche_old_temp[4] 			=	{0};	//保存前一个运行控制方案数组的数据
	
	u8 to_execute_temp[3];							//方案执行邮箱数据存储临时变量
	u8 to_update_temp[3]; 							//方案数据更新邮箱数据存储临时变量
//	u8 from_find_scheme[3];							//闹钟中，找方案得到的方案号索引0和控制方式索引1

	u8	rx_con_type =0;
	
	u8  send_temp[4] = {0};
	u8  send_temp2[4] = {0};
	
	
	
	NVIC_InitTypeDef nvic_init;
	
	
	for(;;)
	{
		/*等待消息触发，1为开机黄闪触发	,2为协议解析触发，
										3为闹钟触发,4为方案周期结束触发	*/
		os_mbx_wait(Sche_Dispatch_Mbox, (void *)&msg, 0xffff);
		#ifdef LIMING_DEBUG_12_23
			printf("-Dispatch-\n");
		#endif		
		
		/*消息格式，第一字节为触发类型，2为控制方式，3为方案号	*/
		trigger_type 	 = *msg;
		rx_control_way = *(++msg);
		rx_sche_number = *(++msg);
		rx_con_type    = *(++msg);

		tsk_dispatch_run= (__TRUE);//喂狗
		
		/*若有故障，则一直降级*/
		if(degrade_always_flag == 1)
		{
			trigger_type = 5;
		}
		
		/*该变量为0，表示需发消息给方案执行任务，为1表示发给方案数据更新任务*/
 		Message_Type = 0;
		/*各消息源触发后，填充sche_change_temp[][]数组	*/				
		switch (trigger_type)
		{
//========================================================================== (1)				
				case 1:										//上电黄闪
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
			
					if(Flash_Time < 0x0A)//最短10秒
					{
						Flash_Time = 0x0a;
					}
					start_yellow_flag = (__TRUE);
					start_yellow_time = Flash_Time;
					
					start_red_enable 		= 1;					//开启开机全红
					sche_change_temp[STA_YELLOW][SCHE_CON_WAY]  = YELLOW_WAY;
					sche_change_temp[STA_YELLOW][SCHE_NUM] = 27;//开机黄闪方案号27
					sche_change_temp[NOW_C][SCHE_CON_WAY]  = sche_change_temp[STA_YELLOW][SCHE_CON_WAY];
					sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[STA_YELLOW][SCHE_NUM];
						
					/*发送方案数据更新数据	*/
					Message_Type = 1;//
					to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
					to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];
					/*通知副CPU	*/

					/*开机日志，*/
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
				case 2:										//协议解析
				{
					
						if(rx_control_way == 0)													//恢复正常
						{
							if((sche_change_temp[NOW_C][SCHE_CON_WAY] >= OFF_WAY) && 
								 (sche_change_temp[NOW_C][SCHE_CON_WAY] <= RED_WAY))
							{//黄闪全红灭灯恢复正常
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
								to_execute_temp[0] = 6;//消息值
								to_execute_temp[1] = 1;
							}
							else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == HOLD_WAY)//看守恢复正常
							{
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//看守
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//看守方案
								
								Message_Type = 2;
								to_execute_temp[0] = 6;//消息值
								to_execute_temp[1] = 2;
							}
							else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY)//步进恢复正常
							{
								sche_change_temp[BY_STEP][SCHE_CON_WAY] = 0;//步进
								sche_change_temp[BY_STEP][SCHE_NUM] = 0;//步进方案
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//步进
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//步进方案
								
								sche_change_temp[NOW_C][SCHE_CON_WAY] = 0;	//清零，以备下一次方案调度
								sche_change_temp[NOW_C][SCHE_NUM] = 0;			
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = AUTO_C;

								send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // 手动面板LED序号
								send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
								send_temp[2]=(uint8_t)0x44;         	// SDLED2~SDLED11低8bit-自主 定周期灯
								send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11高8bit-
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								
								Message_Type = 2;
								to_execute_temp[0] = 6;//消息值
								to_execute_temp[1] = 3;
							}
							else if((sche_change_temp[NOW_C][SCHE_CON_WAY] == 8) || 
											(sche_change_temp[NOW_C][SCHE_CON_WAY] == OPTIMIZE_WAY))
							{
								sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;//优化
								sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;//优化方案

								send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED序号
								send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
							}
							else
							{
								
							}
							//写日志
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
										(rx_control_way <= RED_WAY))						//	灭灯	黄闪	全红（灭灯动作取消）
						{
							/*此处黄闪全红来源不定，可以为以太网发的系统黄闪，或者是USART手动黄闪
							*灭灯暂不处理
							*暂时以系统黄闪处理，有待？？？lxb
							*/
							
							/*	全红黄闪有条件，正在执行步进时不做任何处理  	*/
							if(sche_change_temp[NOW_C][SCHE_CON_WAY] != STEP_WAY)
							{
								switch (rx_con_type)
								{
									case 0xb5:
										{
												if(rx_control_way == YELLOW_WAY)//黄闪
												{
													sche_change_temp[MANUAL_C][SCHE_CON_WAY] = rx_control_way;
													sche_change_temp[MANUAL_C][SCHE_NUM] = 27;						 

													//写日志
													tsk_lock ();  			//  disables task switching 		
													write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
													write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
													write_log_wr_fifo[write_log_wr_index][2] = 0x00;//
													write_log_wr_fifo[write_log_wr_index][3] = 0x04;//手动面板控制 
													write_log_wr_fifo[write_log_wr_index][4] = 12;
				

												}
												if(rx_control_way == RED_WAY)//全红
												{
													sche_change_temp[MANUAL_C][SCHE_CON_WAY] = rx_control_way;
													sche_change_temp[MANUAL_C][SCHE_NUM] = 28;
													//写日志
													tsk_lock ();  			//  disables task switching 		
													write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
													write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
													write_log_wr_fifo[write_log_wr_index][2] = 0x00;
													write_log_wr_fifo[write_log_wr_index][3] = 0x04;//手动面板控制 
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
														if(rx_control_way == YELLOW_WAY)//黄闪
														{
															sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;
															sche_change_temp[SYSTEM_C][SCHE_NUM] = 27;						 

															//写日志
															tsk_lock ();  			//  disables task switching 
															write_log_wr_fifo[write_log_wr_index][0] =Sys_Ctrl_Type;		
															write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
															write_log_wr_fifo[write_log_wr_index][2] = 0;
															write_log_wr_fifo[write_log_wr_index][3] = 0x02;
															write_log_wr_fifo[write_log_wr_index][4] = 12;

														}
														if(rx_control_way == RED_WAY)//全红
														{
															sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;
															sche_change_temp[SYSTEM_C][SCHE_NUM] = 28;
															//写日志
															tsk_lock ();  			//  disables task switching 
															write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;		
															write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
															write_log_wr_fifo[write_log_wr_index][2] = 0;
															write_log_wr_fifo[write_log_wr_index][3] = 0x02;
															write_log_wr_fifo[write_log_wr_index][4] = 13;//全红

														}												 
												}
												
										}
										break;
									default:
										break;
								}

									
									Message_Type = 2;
									to_execute_temp[0] = 4;//消息值
						
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
						else if(rx_control_way == HOLD_WAY)							// 看守
						{
							if(	(sche_change_temp[NOW_C][SCHE_CON_WAY] == YELLOW_WAY) 	||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == RED_WAY) 			||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY) 		);
							else
							{
									sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;			//看守
									
									if(rx_sche_number ==0)
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];					
									}
									else
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = rx_sche_number;					//仍有疑问lxb
									}

									
									Message_Type = 2;
									to_execute_temp[0] = 4;//消息值

									//写日志
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
						else if(rx_control_way == STEP_WAY)							// 步进
						{
							/*	步进有条件，lxb全红黄闪不能步进*/
							if((sche_change_temp[NOW_C][SCHE_CON_WAY] == 0) ||
								 (sche_change_temp[NOW_C][SCHE_CON_WAY] > 3))
							{
								sche_change_temp[BY_STEP][SCHE_CON_WAY] = rx_control_way;//步进
								sche_change_temp[BY_STEP][SCHE_NUM] 		= sche_change_temp[NOW_C][SCHE_NUM];
								/*	
									由于"步进"没有正常的阶段周期(阶段时长为无限长)，在此之后也就不会
									进入到"else if(trigger_type == 4)								//方案周期结束"
									因此，在此把"步进"方案和控制方式赋值的同时，
									也应该改变"当前"控制类型， "当前"控制方式和方案号赋值

								*/
								sche_change_temp[NOW_C][SCHE_CON_TYPE]  	= BY_STEP ;
								//sche_change_temp[NOW_C][SCHE_NUM] 				= sche_change_temp[BY_STEP][SCHE_NUM];  //这句可以不用
								sche_change_temp[NOW_C][SCHE_CON_WAY] 		= sche_change_temp[BY_STEP][SCHE_CON_WAY];							
								
								send_temp[0]=(uint8_t)STEP_CTRL_LED;  // 手动面板LED序号
								send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
								send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11低8bit-
								send_temp[3]=(uint8_t)0x01;						// SDLED2~SDLED11高8bit-步进灯
								os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // 手动面板LED序号
								send_temp2[1]=(uint8_t)LED_ON;					// 工作状态			
								os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
								
								Message_Type = 2;
								to_execute_temp[0] = 5;//消息值
								

								//写日志
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
						else if((rx_control_way == 8) ||                //  liming_?   为什么这里有两种情况
										(rx_control_way == OPTIMIZE_WAY))				// 优化（单点优化没做）
						{
							if(	(sche_change_temp[NOW_C][SCHE_CON_WAY] == YELLOW_WAY) 	||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == RED_WAY) 			||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == HOLD_WAY) 		||
									(sche_change_temp[NOW_C][SCHE_CON_WAY] == STEP_WAY) 		);
							else 
							{
									sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = rx_control_way;//优化
									if (rx_sche_number != 0)					//优化指定方案
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = rx_sche_number;
									}
									else															//优化无方案，赋值当前方案
									{
										sche_change_temp[SYSTEM_C][SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
									}
// // 									optimize_time_count = 0;   //启动定时器
									
							}
																	
							//写日志
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
				case 3:										//闹钟
				{
					
					if(start_red_enable == 1)				//开机全红，开机黄闪后使能全红，闹钟触发
					{
						start_red_enable = 0;					//关闭开机全红

						start_yellow_flag = (__FALSE);//关闭开机黄闪
						
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

						if(Red_Time < 0x05)//最短6秒
						{
							Red_Time = 0x05;
						}
						start_red_flag = (__TRUE);
						start_red_time = Red_Time;
										

						 /*  以下是清除黄闪的方案  */
						sche_change_temp[STA_YELLOW][SCHE_CON_WAY]  = YELLOW_WAY;
						sche_change_temp[STA_YELLOW][SCHE_NUM] = 0;//开机黄闪方案号27
						
						//开启开机全红方案
						sche_change_temp[STA_RED][SCHE_CON_WAY]  = RED_WAY;
						sche_change_temp[STA_RED][SCHE_NUM] = 28;//全红方案号					
						sche_change_temp[NOW_C][SCHE_CON_WAY]  = sche_change_temp[STA_RED][SCHE_CON_WAY];
						sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[STA_RED][SCHE_NUM];
				
						//通知方案数据更新任务
						Message_Type = 1;
						to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];			
		// // // 				to_update_temp[2] = 88;			//应该不需要这个
						//写日志
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
					else														//闹钟触发，闹钟时间到，调度新方案
					{
						//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

						I2C_Clear_Alarm_Register();
						
						Exti_Configuration();
						
						NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
						nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn;
						nvic_init.NVIC_IRQChannelPreemptionPriority = 0;          						//by_lm   0:2
						nvic_init.NVIC_IRQChannelSubPriority = 2;
						nvic_init.NVIC_IRQChannelCmd = ENABLE;
						NVIC_Init(&nvic_init); 
	
						if(red_to_auto == __FALSE)//开机全红结束后，闹钟触发多时段
						{
							red_to_auto = __TRUE;						//屏蔽该if
							start_yellow_red_flag = (__FALSE);	//开机全红结束
							 /*  以下是清除全红的方案  */
							sche_change_temp[STA_RED][SCHE_CON_WAY]  = RED_WAY;
							sche_change_temp[STA_RED][SCHE_NUM] = 0;//全红方案号									
							
							/*	为自主控制做准备	*/
							sche_change_temp[BY_STEP][SCHE_CON_WAY] = 0;
							sche_change_temp[BY_STEP][SCHE_NUM] = 0;
							sche_change_temp[SYSTEM_C][SCHE_CON_WAY] = 0;
							sche_change_temp[SYSTEM_C][SCHE_NUM] = 0;
							sche_change_temp[MANUAL_C][SCHE_CON_WAY] = 0;
							sche_change_temp[MANUAL_C][SCHE_NUM] = 0;		

						}
						
						/*下面该段需要标准化，局部变量化	lxb*/
						//方案执行时间填写
						os_mut_wait(I2c_Mut_lm,0xFFFF);
						I2C_Read_DS3231(Read_Time_Data);			//  I2C_lm   有待改动的	
						os_mut_release(I2c_Mut_lm);	
						
						os_mut_wait (Mutex_Data_Update, 0xffff);				
						Plan_Used.Plane_Execute_Time[0] = Read_Time_Data[2];//小时
						Plan_Used.Plane_Execute_Time[1] = Read_Time_Data[1];//分钟
						Plan_Used.Plane_Execute_Time[2] = Read_Time_Data[0];//分钟
						os_mut_release (Mutex_Data_Update);	  
						
						
						os_mut_wait(I2c_Mut_lm,0xFFFF);
						I2C_Read_DS3231(Read_Time_Data);
						os_mut_release(I2c_Mut_lm);	
						
						//下面部分，保留，，留作以后改变读时钟函数后用
						current_time.second =	Read_Time_Data[0];
						current_time.minute =	Read_Time_Data[1];
						current_time.hour 	=	Read_Time_Data[2];
						current_time.week	  =	Read_Time_Data[3];
						current_time.day 		=	Read_Time_Data[4];
						current_time.month 	=	Read_Time_Data[5];
						current_time.year 	=	Read_Time_Data[6];

						
						table_0x8E_return = Find_Scheme(&current_time);//查到当前控制方式和方案号，并写入下个时段的闹钟
						/*赋值下一阶段方案号和控制方式，自主控制，lxb	*/
						sche_change_temp[AUTO_C][SCHE_CON_WAY] = table_0x8E_return.ctlMode;
						sche_change_temp[AUTO_C][SCHE_NUM] = table_0x8E_return.schemeId;
						//写闹钟
						Alram_Time_Data[0] 			= 	0;
						Alram_Time_Data[1] 			= 	table_0x8E_return.minute/10*16 + table_0x8E_return.minute%10;
						Alram_Time_Data[2] 			= 	table_0x8E_return.hour/10*16 + table_0x8E_return.hour%10;
						Alram_Time_Data[3] 			= 	0x80;

						os_mut_wait(I2c_Mut_lm,0xFFFF);   	//  I2C_lm
						I2C_Write_Alarm(Alram_Time_Data);		//黄闪时间写入时钟芯片闹钟寄存器   
						I2C_Write_Alarm_Register(0x05);			//使能时钟芯片闹钟中断输出		 
						os_mut_release(I2c_Mut_lm);	


						send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED序号
						send_temp[1]=(uint8_t)LED_RUN;												// 工作状态
						
						os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
					} 

				}  /*  end of "else if(Trigger_Type == 3)" */
					break;
//==========================================================================	(4)			
				case 4:										//方案周期结束
				{
					
					if(IWDGRST_flag ==__TRUE)  			/*  看门狗复位	*/
					{
						/*  从后备区中读出方案数据	*/
						BKPRAM_Read((u8*)sche_change_temp,sizeof(sche_change_temp),SCHE_NUM_BKPRAM_ADDR);

						//把不为0的方案与控制类型及控制方式赋给当前方案
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
					
						
// // SCHE_CON_TYPE		1				//控制类型列索引
// // SCHE_NUM					2				//方案号列索引
// // SCHE_CON_WAY		3				//控制方式列索引
					}			
					/*   扫描数组 sche_change_temp各控制类型的方案号   */
					for (i =STA_YELLOW;i<= AUTO_C;i++)   /*    0 ----- 5   */
					{
						if (i == BY_STEP)									//步进扫描控制方式
						{
							if (sche_change_temp[i][SCHE_CON_WAY] == STEP_WAY)
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//把不为0的方案与控制类型及控制方式赋给当前方案
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = i;
								sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
											
								break;
							}
						}
						else																	//其他类型扫描方案号
						{
							if (sche_change_temp[i][SCHE_NUM] != 0)
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//把不为0的方案与控制类型及控制方式赋给当前方案
								sche_change_temp[NOW_C][SCHE_CON_TYPE] = i;
								sche_change_temp[NOW_C][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
											
								break;
							}
							else if (i == AUTO_C)						//步进、手动、系统控制都为空，则为自主控制
							{
								sche_old_temp[SCHE_CON_TYPE] = sche_change_temp[NOW_C][SCHE_CON_TYPE];
								sche_old_temp[SCHE_NUM] = sche_change_temp[NOW_C][SCHE_NUM];
								sche_old_temp[SCHE_CON_WAY] = sche_change_temp[NOW_C][SCHE_CON_WAY];
								
								//把不为0的方案与控制类型及控制方式赋给当前方案
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
						case STA_YELLOW:				//是否为黄闪
							{
								Change_Flag=0;							//继续黄闪，不更新方案数据
							}
							break;	
						case STA_RED:				//是否为全红
							{
								Change_Flag=0;							//继续全红，不更新方案数据
							}
							break;									
						case BY_STEP:						//是否为步进
							{
								Change_Flag=0;						//继续步进，不更新方案数据
							}
							break;
						case MANUAL_C:					//是否为手动控制，手动包括全红灭灯黄闪
							{
								if(sche_old_temp[SCHE_CON_WAY] != 
									 sche_change_temp[NOW_C][SCHE_CON_WAY])
								{//若即将运行的方案与上一方案不一致，则发送消息给方案数据更新任务
									Change_Flag=1;
								}
								else
								{//若即将运行的方案与上一方案一致，则直接通知方案执行任务
									Change_Flag=0;
								}
							}
							break;
						case SYSTEM_C:					//是否为系统控制
							{
								switch (sche_change_temp[SYSTEM_C][SCHE_CON_WAY])
								{
									case YELLOW_WAY:
									case RED_WAY:
										{
											/*若即将运行的方案与上一方案不一致，则发送消息给方案数据更新任务
											一致则通知方案执行任务*/
											if(sche_old_temp[SCHE_CON_WAY] != 
												 sche_change_temp[NOW_C][SCHE_CON_WAY])
											{//黄闪全红
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
										{									//优化
											optimize_time_flag = 1;
//											optimize_time_count = 0;   //启动定时器
											Change_Flag=1;
										}			
								
										break;
									default:
										break;
								}
							}
							break;
						case AUTO_C:						//是否为自主控制/*系统控制、手动控制没有有效数据，则查找多时段数据*/
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
									&& (sche_change_temp[NOW_C][SCHE_NUM] != 0))//方案有变
								{
									Change_Flag = 1;
								}
								if((sche_old_temp[SCHE_CON_WAY] != 
										sche_change_temp[NOW_C][SCHE_CON_WAY]) && 
									 (sche_change_temp[NOW_C][SCHE_CON_WAY] != 0))//控制方式有变
								{
									Change_Flag = 1;
								}
								if((sche_old_temp[SCHE_CON_TYPE] != 
										sche_change_temp[NOW_C][SCHE_CON_TYPE]) )//控制类型有变
								{
									Change_Flag = 1;
								}	

							}
							break;
						default:
							break;
					}
					
					if(Change_Flag == 0)								//通知方案执行任务，执行当前方案
					{
						Message_Type = 2;
						to_execute_temp[0] = 3;//消息值为3
						to_execute_temp[1] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_execute_temp[2] = 0;
						if(sche_change_temp[NOW_C][SCHE_CON_TYPE] == HOLD_WAY)
						{
							/*
							目前的方案执行任务并不接收方案号,
							那这个to_execute_temp[2]意义何在       //liming_?
							*/
							os_mut_wait(RT_Operate_Mut,0xFFFF);   
							to_execute_temp[2] = sche_hold_guand.index;//需要加锁，这是阶段配时方案号 
							os_mut_release(RT_Operate_Mut);		
						}
					}
					else																//通知方案数据更新任务，更新方案数据
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
					
					//键盘led控制--组合输出SD2~SD11
					if(((sche_change_temp[NOW_C][SCHE_CON_WAY] != 1) && (sche_change_temp[NOW_C][SCHE_CON_WAY] <= 6)) 
						|| (sche_change_temp[NOW_C][SCHE_CON_WAY] == 8) || (sche_change_temp[NOW_C][SCHE_CON_WAY] == 12))
					{	
						
						switch (sche_change_temp[NOW_C][SCHE_CON_TYPE])
						{
							case STA_YELLOW:						//自主黄闪
								{	
									send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // 手动面板LED序号
									send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
									send_temp[2]=(uint8_t)0x04;         	// SDLED2~SDLED11低8bit-自主灯
									send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11高8bit-闪光灯
									os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								}
								break;
							case STA_RED:						//是否为全红
								{
									send_temp[0]=(uint8_t)RED_CTRL_LED;  // 手动面板LED序号
									send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
									send_temp[2]=(uint8_t)0x04;         	// SDLED2~SDLED11低8bit-自主灯
									send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11高8bit-闪光灯
									os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
								}
								break;
// // // 							case BY_STEP:						//是否为步进
// // // 								{
// // // 									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 5)//步进
// // // 									{					
// // // 										send_temp[0]=(uint8_t)STEP_CTRL_LED;  // 手动面板LED序号
// // // 										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
// // // 										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11低8bit-
// // // 										send_temp[3]=(uint8_t)0x01;						// SDLED2~SDLED11高8bit-步进灯
// // // 										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
// // // 										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // 手动面板LED序号
// // // 										send_temp2[1]=(uint8_t)LED_ON;					// 工作状态			
// // // 										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
// // // 									}	
// // // 									else
// // // 									{
// // // 									}
// // // 								}
// // // 								break;
							case MANUAL_C:					//是否为手动控制
								{
									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 2)//手动黄闪
									{				
										send_temp[0]=(uint8_t)YELLOW_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11低8bit-
										send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11高8bit-黄闪灯
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // 手动面板LED序号
										send_temp2[1]=(uint8_t)LED_ON;					// 工作状态			
										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 3)//手动全红
									{							
										send_temp[0]=(uint8_t)RED_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x00;         	// SDLED2~SDLED11低8bit-
										send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11高8bit-步进灯
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
										send_temp2[0]=(uint8_t)HAND_CTRL_LED;  // 手动面板LED序号
										send_temp2[1]=(uint8_t)LED_ON;					// 工作状态			
										os_mbx_send(mailbox_led,(void*)send_temp2,0xFF);
									}
									else
									{
									}
								}
								break;
							case SYSTEM_C:					//是否为系统控制
								{
									if(sche_change_temp[NOW_C][SCHE_NUM] == 27)//系统黄闪
									{						
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x02;         	// SDLED2~SDLED11低8bit-系统灯
										send_temp[3]=(uint8_t)0x04;						// SDLED2~SDLED11高8bit-闪光灯
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_NUM] == 28)//系统全红
									{				
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x02;         	// SDLED2~SDLED11低8bit-系统灯
										send_temp[3]=(uint8_t)0x02;						// SDLED2~SDLED11高8bit-全红灯
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 4)//系统看守
									{	
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x82;         	// SDLED2~SDLED11低8bit-系统灯 看守灯
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11高8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 12)//系统优化
									{		
										send_temp[0]=(uint8_t)SYSTEM_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x12;         	// SDLED2~SDLED11低8bit-系统灯 优化灯
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11高8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else
									{
									}
								}
								break;
							case AUTO_C:						//是否为自主控制/*系统控制、手动控制没有有效数据，则查找多时段数据*/
								{
									if(sche_change_temp[NOW_C][SCHE_CON_WAY] == 6)//自主感应
									{	
										send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x24;         	// SDLED2~SDLED11低8bit-自主灯 感应灯
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11高8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
									}
									else //自主控制
									{			
										send_temp[0]=(uint8_t)ATOU_CTRL_LED;  // 手动面板LED序号
										send_temp[1]=(uint8_t)LED_ON;					// 工作状态			
										send_temp[2]=(uint8_t)0x44;         	// SDLED2~SDLED11低8bit-自主灯 定周期灯
										send_temp[3]=(uint8_t)0x00;						// SDLED2~SDLED11高8bit-
										os_mbx_send(mailbox_led,(void*)send_temp,0xFF);	
									  if(TT_cont2==0)
										{
											TT_cont2=5;
											tsk_lock ();  			//  disables task switching 
											write_log_wr_fifo[write_log_wr_index][0] = Sys_Ctrl_Type;
											write_log_wr_fifo[write_log_wr_index][1] = 0xA5;
											write_log_wr_fifo[write_log_wr_index][2] = 0x00;
											write_log_wr_fifo[write_log_wr_index][3] = 0;
											write_log_wr_fifo[write_log_wr_index][4] = 0x05;		  //本机多时段控制		
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
					
					/*  把方案数据写入后备区	*/
					BKPRAM_Write((u8*)sche_change_temp,sizeof(sche_change_temp),SCHE_NUM_BKPRAM_ADDR);
				}   /*  end of "else if(Trigger_Type == 4)" */
				
					break;
//==========================================================================	(5)			
				case 5:										//降级
				{
					switch (sche_change_temp[NOW_C][SCHE_CON_TYPE])
					{
						case STA_YELLOW:		
							{
								Change_Flag=0;							//继续黄闪，不降级
							}
							break;	
						case STA_RED:						
						case BY_STEP:					
						case MANUAL_C:							
						case SYSTEM_C:								
							{
								/*   
									如果在"全红"、"步进"、"手动"、"系统"控制期间需要降级,
									则清除相应的方案号 ，
									降级为自主控制。
								*/
								for (i =STA_YELLOW;i< AUTO_C;i++)   /*    0 ----- 4   */
								{						
									sche_change_temp[i][SCHE_NUM] = 0;
								}
								
								sche_change_temp[NOW_C][SCHE_CON_TYPE]	=sche_change_temp[AUTO_C][SCHE_CON_TYPE];
								sche_change_temp[NOW_C][SCHE_NUM]				=sche_change_temp[AUTO_C][SCHE_NUM];
								sche_change_temp[NOW_C][SCHE_CON_WAY]		=sche_change_temp[AUTO_C][SCHE_CON_WAY];

								Change_Flag=1;							//更新方案数据
							}
							break;		
						case AUTO_C:	
						case YELLOW_C:					
							{
// 								/*   
// 									如果在"自主控制"期间发生了降级：
// 									1、方案号不为0，则降级为0号方案，控制方式不变
// 									2、方案号为0	，则降级为黄闪控制
// 								*/						
// 								if(sche_change_temp[NOW_C][SCHE_NUM] !=0)
// 								{
// 									sche_change_temp[AUTO_C][SCHE_CON_TYPE]	=AUTO_C;
// 									sche_change_temp[AUTO_C][SCHE_NUM]			=0; 
// 									//sche_change_temp[AUTO_C][SCHE_CON_WAY]	=sche_old_temp[SCHE_CON_WAY];		//这句可以不用写					
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
					
					if(Change_Flag == 0)								//通知方案执行任务，执行当前方案
					{
						Message_Type = 2;
						to_execute_temp[0] = 3;						//消息值为3
						to_execute_temp[1] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_execute_temp[2] = 0;
					}
					else																//通知方案数据更新任务，更新方案数据
					{
						Message_Type = 1;	
						to_update_temp[0] = sche_change_temp[NOW_C][SCHE_CON_WAY];
						to_update_temp[1] = sche_change_temp[NOW_C][SCHE_NUM];
					}
				}  // end of "else if(trigger_type ==5)	"								//降级
					break;
//==========================================================================	(6)			
				default:
					break;
//==========================================================================		
		}		
		/*填充全局变量数组sche_change_tpye[][4]，以便以太网查询相关数据;
		* 此处填充了sche_change_tpye，则协议解析部分不用填充sche_change_tpye数组
		*/
		os_mut_wait(RT_Operate_Mut,0xFFFF);   //用于方案数据数组修改
		for ( i=0;i<=NOW_C;i++)
		{
			sche_change_tpye[i][SCHE_CON_TYPE] = sche_change_temp[i][SCHE_CON_TYPE];
			sche_change_tpye[i][SCHE_CON_WAY] = sche_change_temp[i][SCHE_CON_WAY];
			sche_change_tpye[i][SCHE_NUM] = sche_change_temp[i][SCHE_NUM];
		}
		os_mut_release(RT_Operate_Mut);
		
		tsk_dispatch_over= (__TRUE);//喂狗
		
		if(Message_Type == 1)//方案数据任务   //  to  Sche_Data_Update_Task()  //by_lm
		{
			msg_send = to_update_temp;
			os_mbx_send(Sche_Data_Update_Mbox, msg_send, 0x10);    
		}
		else if(Message_Type == 2)//方案执行任务
		{
			msg_send = to_execute_temp;
			os_mbx_send(Sche_Execute_Mbox, msg_send, 0x10);  
		}
	}
}
		
/*	黄闪阶段数据-只读	*/
extern const STAGE_TIMING_TABLE_t Yel_Stage_Time_Table[16];
/*	全红阶段数据-只读	*/
extern const STAGE_TIMING_TABLE_t Red_Stage_Time_Table[16];
__task void Sche_Data_Update_Task(void)
{
	u8										*msg;
	u8 										*msg_send;
	u32 									i,j;
	u8 										time_start_flag;//定时器开启标志-确保定时器只开启一次
	
	


	/*
	方案数据读取所需变量
	*/

	u8 										degrade_flag;	//降级标志
	u8 										scheme_num_temp;
	u8 										control_type_temp;
	CONFIG_SCHEME_t 			check_0xc0_temp;
	STAGE_TIMING_TABLE_t 	check_0xc1_temp[16];
	PHASE_PARAM_t 				phase_par_temp[16];
	FOLLOW_PHASE_PARAM_t 	tab_0xc8_temp[9];
	CHANNEL_PARAM_t 			tab_0xb0_temp[Channel_Max_Define];
	/*
	过渡方案计算所需变量
	*/
	u32 			temp;	
	u8				Stage_State_temp;								//阶段总数
	float 		stage_time[16];              		//新方案各阶段时长
	u32 			stage_time_temp[3][16];   			//过渡方案各阶段时长
	float 		c;															//周期漂移值c'
	u32 			time_differ_temp = 0;						//方案实际执行时间与计划执行时间之差
	u32 			time_current_temp;							//DS3231时间-转化成的秒数
	u8 				time_data_temp[7] = {0};				//DS3231时间-数组
	u8				new_cycle_time_temp;      			//新周期时长(计算周期漂移值c')
	u8				new_phase_differ_temp;       		//新相位差(计算周期漂移值c')
	u8				old_phase_differ_temp;    			//旧相位差(计算周期漂移值c')
	u8 				fangan_hour_temp;								//新方案开始执行时间-小时
	u8 				fangan_min_temp;								//新方案开始执行时间-分钟
	u32				fangan_time_temp;								//新方案开始执行时间-转化成秒数
	
	
	u8  			read_index=0;
	u8  			hold_stageId=0;
	
	u32  			B0_error_count=0;
	
	//========================  debug_lm

	//=========================
	
	//================================3.23
	u16 work_phase;//放行相位
		/*上报数据*/
	u8 Phase_conflict_answer_Data[7] = {0};
	u16 phase_conflict;//冲突放行相位
	
	u8 send_temp[2]={0};
	u8 send_temp1[2]={0};
	
	/*	
	数据库操作需要的变量	
	*/
	u32										read_lenth_temp		=		0;	
	TARGET_DATA 					data_Target;	
		
	control_type_temp++;
	
	for(;;)
	{
		
		tsk_data_run= (__TRUE);//喂狗
		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
		
		os_mbx_wait(Sche_Data_Update_Mbox, (void *)&msg, 0xffff);
		control_type_temp	= *msg;  
		scheme_num_temp 	= *(++msg);

		
	
		
		#ifdef LIMING_DEBUG_12_23
			printf("-Data_Up-\n");
		#endif		

		
		/*
		初始化temp数据---方案数据读取所需变量全部赋0
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
		初始化temp数据---过渡方案计算所需变量全部赋0
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
		/*=======读数据库操作 ，得到的数据放在temp中=======*/
		
		/*
		读0xC0表
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

		
		if(	read_lenth_temp 			== 	0	)	//error  赋0
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
			u8 		Period_Time;			//公共周期时长B8	=====替换C0表
			u8 		Phase_Difference;		//协调相位差B9		=====替换C0表
			u8 		Stage_Time[16];		//优化阶段时长BE	=====替换C1表
			u8 		Key_Phase_Time[16];	//优化阶段关键相位绿灯时间BF
		}sche_optimize_struct;
		*/
		//////////==========youhua
		if(	control_type_temp	== OPTIMIZE_WAY)  //优化
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF); //加锁
			check_0xc0_temp.period		= 	sche_optimize.Period_Time;
			check_0xc0_temp.phaseDif	= 	sche_optimize.Phase_Difference;
			os_mut_release(RT_Operate_Mut);	  
		}

		
		/*
		读0xC1表
		*/
		read_index = 0;
		if	(scheme_num_temp == 27)		//黄闪
		{
				for(i = 0;i < 16;i++)
				{
					check_0xc1_temp[i] = Yel_Stage_Time_Table[i];
				}
				read_index=15;
		}
		else if	(scheme_num_temp == 28)		//全红
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
				
				if(	read_lenth_temp 		!= 	0	)	//error  不能赋0
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

		if(	control_type_temp	== HOLD_WAY)  //看守              //debug_lm
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF);  										 			//加锁
			hold_stageId			=sche_hold_guand.stageId;
			check_0xc1_temp[hold_stageId-1]	=sche_hold_guand;							//  此处的i等于多少   //liming_?
			os_mut_release(RT_Operate_Mut);	     													//解锁
		}
	
/*	
sche_optimize	
typedef struct {
	u8 		Period_Time;			//公共周期时长B8	=====替换C0表
	u8 		Phase_Difference;		//协调相位差B9		=====替换C0表
	u8 		Stage_Time[16];		//优化阶段时长BE	=====替换C1表
	u8 		Key_Phase_Time[16];	//优化阶段关键相位绿灯时间BF
}sche_optimize_struct;
*/
		//////////==========youhua
		if(	control_type_temp	== OPTIMIZE_WAY)  //优化
		{
			os_mut_wait(RT_Operate_Mut,0xFFFF); //加锁
			/*
			阶段绿灯时间替换
			首先判断是否为关键相位
			若为关键相位，则阶段绿灯时间替换为Key_Phase_Time[]
			若不为关键相位，则用Stage_Time[]替换
			*/
			for(i = 0;i < read_index;i++)
			{
				if(((check_0xc1_temp[i].stAlterParam) & 0x10) != 0 )//关键相位
				{
					if(sche_optimize.Key_Phase_Time[i] >= Green_Min)
					{    
						check_0xc1_temp[i].stGreenTime = sche_optimize.Key_Phase_Time[i];
					}
         }
				else//非关键相位
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
		读0x95表
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
			
			if(read_lenth_temp == 	0)//error	赋0
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
		读0xC8表	
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
		读0xB0表	
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
		校验方案是否完整	
		由于1个方案号对应一个
		阶段号,目前为Plan_Used.Stage_State
		个配时方案号
		*/
		if(read_index>=1)
		{

			for(i=0;	i		< check_0xc1_temp[read_index - 1].stageId; 	i++)
			{
				if(	check_0xc1_temp[i].stageId 				== 0		)																								//此组没数，方案不完整
					degrade_flag	=	1;																																//方案不完整
				if(	(check_0xc1_temp[i].stGreenTime 	< Green_Min)		&&	(scheme_num_temp < 27)	)	//绿灯时间小于最小值
					degrade_flag	=	1;
				if(	(check_0xc1_temp[i].stYellowTime 	< Yellow_Min)		&& 	(scheme_num_temp < 27)	)	//黄灯时间小于最小值
					degrade_flag	=	1;
				if(	(check_0xc1_temp[i].stRedTime 		< Red_Min)			&& 	(scheme_num_temp < 27)	)	//红灯时间小于最小值
					degrade_flag	=	1;
			}
		}
		else
		{
			
			degrade_flag = 1;
		}
			
		/*
		根据校验结果进行处理，
		不完整则降级
		完整则拷贝数据至方案数据
		*/
		if(degrade_flag == 1)
		{
			/*	
			降级处理	
			*/
// 			RUN_LED_OFF();
// 			ERROR_LED_ON();
			send_temp[0]=(uint8_t)RUN_LED;          			// LED序号
			send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			send_temp1[0]=(uint8_t)ERROR_LED;          			// LED序号
			send_temp1[1]=(uint8_t)LED_ON;												// 工作状态			
			os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

			/*触发降级*/
			Degrade_MSG[0]	= 	5;
			msg_send				= 	Degrade_MSG;
			os_mbx_send(Sche_Dispatch_Mbox,msg_send,0x10);				
			/*	
			写日志	
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
		/*=======将temp中的数据转到方案数据中=======*/
// 			ERROR_LED_OFF();
// 			RUN_LED_ON();

		send_temp[0]=(uint8_t)RUN_LED;          			// LED序号
		send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
		os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
		send_temp1[0]=(uint8_t)ERROR_LED;          			// LED序号
		send_temp1[1]=(uint8_t)LED_OFF;												// 工作状态			
		os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);

			/*
			等待互斥量，访问方案数据
			*/
			os_mut_wait (Mutex_Data_Update, 0xffff);						
			
			Plan_Used.Plan_Num 			= 	scheme_num_temp;
			Plan_Used.Stage_State 	= 	check_0xc1_temp[read_index - 1].stageId;
			/*
			写0xC0表到方案数据	
			*/
			Plan_Used.Period_Time 				= 	check_0xc0_temp.period;
			Plan_Used.Phase_Difference 		= 	check_0xc0_temp.phaseDif;
			Plan_Used.Coordination_Phase	= 	check_0xc0_temp.coPhase;			
			/*	
			写0xC1表到方案数据	
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

			//判断相位故障
			if(scheme_num_temp != 27 && scheme_num_temp != 28)
			{
				for(i = 0; i < 16; i++)
				{
					work_phase = Plan_Used.Stage_Contex[i].relPhase;
					phase_conflict = Phase_Collide_Detect(work_phase);
					if(phase_conflict > 0)
					{
							//上报相位冲突表，降级运行,冲突
							Phase_conflict_answer_Data[0] = 0x83;
							Phase_conflict_answer_Data[1] = 0x97;
							Phase_conflict_answer_Data[2] = 0x00;
							Phase_conflict_answer_Data[3] = i+1;//行号此时等于阶段号?
							Phase_conflict_answer_Data[4] = (u8)((work_phase & 0xFF00) >> 8);//冲突相位;
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
							
// 							/*若相位板出现故障，则不发送数据给相位板黄闪板*/
// 							unsend_to_phase_flag = 1;	
// 							break;
							/*	触发降级		*/
							Degrade_MSG[0]	= 	5;
							msg_send				= 	Degrade_MSG;
							os_mbx_send(Sche_Dispatch_Mbox,msg_send,0x10);	
					}
// 					else
// 					{
// 						/*若相位板出现故障，则不发送数据给相位板黄闪板*/
// 							unsend_to_phase_flag = 0;
// 					}						
				}
				
				//相位驱动模块
			}
			
			//================================3.23
			/*	
			写0x95表到方案数据	
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
			关键相位绿灯时间写到方案数据	
			*/
			for(i = 0; i < 16;i++)
				Plan_Used.Key_Phase_Time[i] 	= 	Phase_Par[i].minGreenTime;
			/*		
			写0xC8表到方案数据	
			*/
			for(i = 1;i <= 8;i++)
			{
				Tab_C8[i].phaseId 				= 	tab_0xc8_temp[i].phaseId ;
				Tab_C8[i].incPhaseLenth		= 	tab_0xc8_temp[i].incPhaseLenth;
				for(j = 0; j < 16; j++)
					Tab_C8[i].incPhase[j] 	= 	tab_0xc8_temp[i].incPhase[j];
			}
			/*	
			写0xB0表到方案数据	
			*/
			for(i=1;i <= Channel_Max;i++)
			{
				Tab_B0[i].chnCtlType 		= 	tab_0xb0_temp[i].chnCtlType;
				Tab_B0[i].chnId 				= 	tab_0xb0_temp[i].chnId;
				Tab_B0[i].chnStat 			= 	tab_0xb0_temp[i].chnStat;
				Tab_B0[i].signalSrc 		= 	tab_0xb0_temp[i].signalSrc;
			}

			/*
			判断是否需要执行过渡方案
			如果新方案是黄闪（27）、全红（28），则不不需要执行和计算
			若不是，则需要执行和计算
			*/
			if(	(scheme_num_temp == 27)		||	(scheme_num_temp == 28)	
				||(	control_type_temp	== HOLD_WAY)
				||(	control_type_temp	== OPTIMIZE_WAY)) 	
			{
				/*
				释放信号量，退出方案数据访问
				*/
				//os_mut_release (Mutex_Data_Update);	
				Plan_Used_Count = 0;//过渡方案   1号方案
			}
			else
			{		
				//==================================
				/*=======过渡方案计算并赋值=======*/
						
				fangan_hour_temp 			= 	Plan_Used.Plane_Execute_Time[0];	//新方案开始执行时间-小时
				fangan_min_temp 			= 	Plan_Used.Plane_Execute_Time[1];	//新方案开始执行时间-分钟		
				new_cycle_time_temp		= 	Plan_Used.Period_Time;						//新方案周期时长
				new_phase_differ_temp	= 	Plan_Used.Phase_Difference;				//新方案相位差
				Stage_State_temp 			= 	Plan_Used.Stage_State;						//阶段总数
				
				fangan_time_temp 			= 	fangan_hour_temp * 60 * 60 	+ 	fangan_min_temp * 60;

				os_mut_wait(I2c_Mut_lm,0xFFFF);
				I2C_Read_DS3231(time_data_temp);														//  I2C_lm   有待改动的	
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
				填充配时方案表
				周期时长计算得来
				相位差和协调相位均无意义，赋0

				计算周期时长	
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
				else//理论上不会出现此情况
				{

				}

				/*
				填充阶段配时表
				阶段号、放行相位与新方案的相同        
				*/		

				/*	计算放行相位	*/
				for(i = 0;i < 3;i++)
				{
					for(j = 0;j < Stage_State_temp;j++)
					{
						Interim_Scheme[i].Stage_Contex[j].index 				= Plan_Used.Stage_Contex[j].index;          //阶段配时方案号
						Interim_Scheme[i].Stage_Contex[j].stageId 			= Plan_Used.Stage_Contex[j].stageId;        //阶段号
						Interim_Scheme[i].Stage_Contex[j].relPhase 			= Plan_Used.Stage_Contex[j].relPhase;       //放行相位
						Interim_Scheme[i].Stage_Contex[j].stAlterParam 	= Plan_Used.Stage_Contex[j].stAlterParam;		//阶段选项参数
					}
				}
				
				/*			
				计算各色灯时间
				(按新方案个阶段时长比例
				分配过渡方案各阶段时长，
				最后一阶段取剩余时间)		
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
						stage_time_temp[i][Stage_State_temp - 1] 	+= 	stage_time_temp[i][j];					//统计前几个阶段时长之和
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
						Interim_Scheme[i].Stage_Contex[j].stGreenTime 	= 	stage_time_temp[i][j] - 6;	//阶段绿灯时间
						Interim_Scheme[i].Stage_Contex[j].stYellowTime 	= 	4;													//阶段黄灯时间
						Interim_Scheme[i].Stage_Contex[j].stRedTime 		= 	2;													//阶段红灯时间
					}
				}
				Plan_Used_Count = 1;//过渡方案   1号方案
			}
			
			/*
			释放信号量，退出方案数据访问
			*/
			os_mut_release (Mutex_Data_Update);	  
			/*
			开启定时启动
			只执行一次
			*/
			if(time_start_flag == 0)
			{
				time_start_flag = 1;

				TIM_Cmd(TIM3, ENABLE);
				//TIM_Cmd(TIM4, ENABLE);
				TIM_Cmd(TIM5, ENABLE);
			}
			
			
			
			
			//========有待改正====================================
			//====================================================		
			/*
			消息值有待和其他任务进行确认
			*/
			
// 			if(Plan_Used.Coordination_Phase != 0)//有协调相位,黄闪全红无协调相位
// 			{
// 				//过渡方案
// 				Sche_Interim_MSG[0] = scheme_num_temp;
// 				msg_send 			= Sche_Interim_MSG;
// 				os_mbx_send(Sche_Interim_Mbox,msg_send,0x10);
// 			}
// 			else
			//{//无需过渡方案
				//发送消息C2
				Sche_Execute_MSG[0] = 2;
				Sche_Execute_MSG[1] = scheme_num_temp;
				Sche_Execute_MSG[2] =	hold_stageId;
				msg_send = Sche_Execute_MSG;
				os_mbx_send(Sche_Execute_Mbox,msg_send,0x10);
			//}	
		}

		tsk_data_over= (__TRUE);//喂狗
		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	}
}
//方案执行任务
//阶段配时表各灯色时间转换的计数值，全局变量，暂保留/*步进用*/
STAGE_TIMING_COUNT_t		stage_timing_count = {0};
/*无步进时当前的阶段绿灯计数值*//*步进用*/
u16 current_stage_green_count = 0;
/*当当前绿灯正在运行，第一次按步进时，需要将0xffff付给计数值。该标志表示第一次进入步进*//*步进用*/
u8 first_keep_current_step = 0;
/*数据库设定的行人绿闪时间*//*步进用*/
u8 walker_green_flicker = 6;

u8 sche_cnt = 0;
u8 sche_end = 0;
__task void Sche_Execute_Task(void)
{

	u32 i, p;
	u8  fangan_NO, MSG_Send_Flag;
	
	u8  *msg;
	u8  *msg_send;
	u8  msg_rx_type = 0;			//消息类型
	u8  Stage_Total = 0;			//总的阶段数
	u8 	Step_En_Flag = 0;			//步进标识
	u8 	Back_Type 	= 0;			//恢复正常之前类型
	u8	current_stage_no = 0;	//当前阶段号
	u8	to_Sche_Dispatch_MSG_temp[5] = {0};		//给方案调度任务邮箱内容

//	u8	to_Sche_Output_Mbox_temp=0;

	u8  hold_stageId=0;

	Plan_Model                Plan_Used_temp;	//应用模块(局部)
	static Induction_Str 	Schedule_Struct_exe_temp[Channel_Max_Define] 	= {0};//方案模块结构体

	/*	lxb相位输出新接口1.13		*/
	STAGE_TIMING_COUNT_t		stage_count_temp 		= {0};//阶段配时表各灯色时间转换的计数值局部结构体，用于输出给方案输出任务
	u32 msg_to_output = 0;
	u8	step_flag =0;
	/*步进用*/
	STAGE_TIMING_COUNT_t stage_count_feedback_temp = {0};//当前方案输出计数值结构体由方案输出任务反馈给调度任务的值
	
	U8 step_stage = 0;//==5,步进开始;==0xf0,步进操作;==0xf1，步进结束；
	
	/*已完成的绿灯计数值completed_green_count*/
	u16 completed_green_count= 0;/*步进用*/
	/*	lxb相位输出新接口1.13		*/
			
	U8 send_temp[2] = {0};
	
	for(;;)
	{
		os_mbx_wait(Sche_Execute_Mbox, (void *)&msg, 0xffff);
		
		#ifdef LIMING_DEBUG_12_23
			printf("-Execute-\n");
		#endif		
		
		tsk_execute_run= (__TRUE);//喂狗  

		msg_rx_type 		= *msg;
		Back_Type 			= *(++msg);//恢复正常之前类型，msg_rx_type == 6时使用
		hold_stageId		= *(++msg);
		
		MSG_Send_Flag 	= 0;

	/*	lxb相位输出新接口1.13		*/		
		stage_count_temp.index 				= 0;								//	阶段配时方案号			
		stage_count_temp.stageId 			= 0;								//阶段号	
		stage_count_temp.stAlterParam = 0;	//阶段选项参数
			stage_count_temp.relPhase = 0;					//放行相位2B
		for(i = 0; i < Channel_Max; i++)
		{
			stage_count_temp.stGreenCount[i] 	= 0;//阶段16通道绿灯计数
			stage_count_temp.stYellowCount[i] = 0;//阶段16通道黄灯计数
			stage_count_temp.stRedCount[i] 		= 0;//阶段16通道红灯计数
		}
	/*	lxb相位输出新接口1.13		*/
			
		os_mut_wait (Mutex_Data_Update,0xFFFF);	
		Plan_Used_temp	=Plan_Used;
		os_mut_release (Mutex_Data_Update);	
		
		os_mut_wait(Output_Data_Mut,0xFFFF);			//加锁，或关调度
		for(i = 1; i < Channel_Max_Define; i++)
		{
			Schedule_Struct_exe_temp[i] = Schedule_Struct[i];
		}
		os_mut_release(Output_Data_Mut); 				//解锁，或开调度	
			
// 		if((Plan_Used_temp.Plan_Num!=27)&&(Plan_Used_temp.Plan_Num!=28))
// 		{
// 			if(sche_cnt == 0)
// 			{
// 				phaseT_make = __TRUE;
// 				sche_cnt = 0xf0;
// 			}
// 		}
//================================================================(1)			
		/*   来自方案输出任务 (阶段结束)  */
		if(msg_rx_type == 1)
		{
			step_flag = 0;//阶段结束后，需暂时关闭步进方式，以在方案输出里给黄红计数值赋值/*步进用*/
			
			if(sche_end ==1) 
			{
				current_stage_no = Stage_Total;
				sche_end = 0;
			}
			
			if( current_stage_no < Stage_Total)//方案未结束
			{
				current_stage_no ++;//阶段自加，下一阶段
				Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
			}
			else// 方案结束
			{
				to_Sche_Dispatch_MSG_temp[0] 	= 4;
				//==========发送消息
				MSG_Send_Flag 								= 1;
				
			}
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB	
				/*  把当前阶段号写到后备区	*/
				BKPRAM_Write(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);		
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB					
			
		}
//================================================================
//================================================================(2)	
		else if(msg_rx_type == 2)//新周期   来自方案数据更新任务
		{
			/*			*/
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB					
			if(IWDGRST_flag ==__TRUE)  			/*  看门狗复位	*/
			{
						IWDGRST_flag =__FALSE;			/*   最后一次实用了，该清除标志了  */
						
						/*  读出当前阶段号	*/
						BKPRAM_Read(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);
				
						if(hold_stageId !=0)  
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//方案号
							Stage_Total 			= Plan_Used_temp.Stage_State;	//阶段总数
							//current_stage_no  =	hold_stageId;
							hold_stageId			=0;	   //  以备下一次使用 
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
							for(p = 0; p <= Channel_Max; p++)
							{
								if(Schedule_Struct_exe_temp[p].release_phase == 0x0A)//相位看守放行
								{
									Schedule_Struct_exe_temp[p].green_count 			= Schedule_Struct_exe_temp[p].green_count * 60;//分钟转化秒数
									Schedule_Struct_exe_temp[p].max_green_count 	= Schedule_Struct_exe_temp[p].green_count * 60;
								}
							}
						}
						else 
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//方案号
							Stage_Total 			= Plan_Used_temp.Stage_State;	//阶段总数
							//current_stage_no 	= 1;					//当前阶段号 = 1
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
						}								
			}	/*   end of "if(IWDGRST_flag ==__TRUE) "  */
			else 							/*  没有看门狗复位	*/
			{
						if(hold_stageId !=0)  
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//方案号
							Stage_Total 			= Plan_Used_temp.Stage_State;	//阶段总数
							current_stage_no  =	hold_stageId;
							hold_stageId			=0;	   //  以备下一次使用 
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
							for(p = 0; p <= Channel_Max; p++)
							{
								if(Schedule_Struct_exe_temp[p].release_phase == 0x0A)//相位看守放行
								{
									Schedule_Struct_exe_temp[p].max_green_count 	= Schedule_Struct_exe_temp[p].green_count * 60;
									Schedule_Struct_exe_temp[p].green_count 			= Schedule_Struct_exe_temp[p].green_count * 60;//分钟转化秒数
								}
							}
						}
						else 
						{
							fangan_NO 				= Plan_Used_temp.Plan_Num;	//方案号
							Stage_Total 			= Plan_Used_temp.Stage_State;	//阶段总数
							current_stage_no 	= 1;					//当前阶段号 = 1
							Fangan_Model(fangan_NO, current_stage_no,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);
						}
			}	
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB

			
		}
//================================================================
//================================================================(3)			
		else if(msg_rx_type == 3)//新周期，方案未更新，调度任务直接过来
		{
				fangan_NO 				= Plan_Used_temp.Plan_Num;	//方案号
				Stage_Total 			= Plan_Used_temp.Stage_State;	//阶段总数
				current_stage_no 	= 1;					//当前阶段号 = 1
				Fangan_Model(fangan_NO, 1,Step_En_Flag, &Plan_Used_temp, Schedule_Struct_exe_temp);//给计数器赋值			
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM 			BBBBBBBBBBBBBBBBBBBBBB	
				/*  把当前阶段号写到后备区	*/
				BKPRAM_Write(&current_stage_no,sizeof(current_stage_no),NOW_STAGE_BKPRAM_ADDR);		
			//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB		
		}
		
//================================================================
//================================================================(4)			
		else if(msg_rx_type == 4)//黄闪全红灭灯看守
		{//提前结束当前周期		
			
			send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED序号
			send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			for(p = 0; p <= Channel_Max; p++)
			{
				if(Schedule_Struct_exe_temp[p].green_count > 1)
				{
					Schedule_Struct_exe_temp[p].green_count 			= 0;
					Schedule_Struct_exe_temp[p].max_green_count 	= 0;
				}
			}
			Stage_Total 			= 0;//总阶段数清零
			os_mut_wait (Mutex_Data_Update,0xFFFF);	
  		Plan_Used_Count = 0;//过渡方案//chen11.29
  		os_mut_release (Mutex_Data_Update);	
		}
//================================================================
//================================================================(5)			
		else if(msg_rx_type == 5)//步进/*步进用*/
		{
			
			send_temp[0]=(uint8_t)CTLR_TYPE1_LED;          			// LED序号
			send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
			os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			step_flag = 1;//步进标识
	
			/*
				阶段配时表各灯色时间计数局部化
			*/
			os_mut_wait(Stage_Count_Mut,0xFFFF);
			stage_count_feedback_temp = stage_timing_count;				
			os_mut_release(Stage_Count_Mut);
			
			os_mut_wait (Mutex_Data_Update,0xFFFF);	
  		Plan_Used_Count = 0;//过渡方案//chen11.29
  		os_mut_release (Mutex_Data_Update);	
			
			if(Step_En_Flag == 0)//步进第一步
			{
				step_stage = 5;
				for(p = 0; p <= Channel_Max; p++)
				{
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{
						Schedule_Struct_exe_temp[p].green_count 			= 0xFFFF;
						Schedule_Struct_exe_temp[p].max_green_count 	= 0xFFFF;
						
						/*把已完成的绿灯计数值付给completed_green_count*/
						completed_green_count = current_stage_green_count-stage_count_feedback_temp.stGreenCount[p-1];
					}
				}
				Step_En_Flag = 1;//步进标识置位
				
			}
			else//步进第二步后
			{
				step_stage = 0xf0;
				for(p = 0; p <= Channel_Max; p++)
				{
// 					/*
// 						阶段配时表各灯色时间计数局部化
// 					*/
// 					os_mut_wait(Stage_Count_Mut,0xFFFF);
// 					stage_count_dispatch_temp = stage_timing_count;				
// 					os_mut_release(Stage_Count_Mut);
					if(Schedule_Struct_exe_temp[p].green_count > 1)
					{
						Schedule_Struct_exe_temp[p].green_count 			= walker_green_flicker*Int_Time_Per_Second;
						Schedule_Struct_exe_temp[p].max_green_count 	= walker_green_flicker*Int_Time_Per_Second;
						
						/*已完成的绿灯计数值清零*/
						completed_green_count = 0;
					}
				}
			}
		}
//================================================================
//================================================================(6)			
		else if(msg_rx_type == 6)//恢复正常
		{
			if(Back_Type == 1)//黄山全红灭灯
			{
				Stage_Total = 0;
// 				CTLR_TYPE2_LED_OFF();
				send_temp[0]=(uint8_t)CTLR_TYPE2_LED;          			// LED序号
				send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
			
			}
			else if(Back_Type == 2)//看守
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
			else if(Back_Type == 3)//步进
			{
				step_stage = 0xf1;
// 				CTLR_TYPE2_LED_OFF();
				send_temp[0]=(uint8_t)CTLR_TYPE2_LED;          			// LED序号
				send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp,0xFF);
				
				step_flag = 0;//步进标识
				
				/*当当前绿灯正在运行，第一次按步进时，需要将0xffff付给计数值。
				该标志表示第一次进入步进*/
				first_keep_current_step = 0;
				
				/*
					阶段配时表各灯色时间计数局部化步进用
				*/
				os_mut_wait(Stage_Count_Mut,0xFFFF);
				stage_count_feedback_temp = stage_timing_count;				
				os_mut_release(Stage_Count_Mut);
				
				/*
					手动面板切换到自动时，须计算已执行完的绿灯时间，若已执行的绿灯时间与绿闪时间6s大于
				当前阶段本来要执行的时间，则赋值绿闪时间即可，否则赋值大的值，步进用
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
// 				Step_En_Flag = 0;//步进标识置位
			}
			/*else if(Back_Type == 4)//优化
			{
				//优化数据清空
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
			
			/*	lxb相位输出新接口1.13		*/
			if(Plan_Used.Plan_Num>0)
			{
				stage_count_temp.index 				= Plan_Used.Plan_Num;								//	阶段配时方案号
			}
			else
			{
				stage_count_temp.index = 0xff;
			}
			stage_count_temp.stageId 			= current_stage_no;								//阶段号
			
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
			stage_count_temp.stAlterParam = Plan_Used_temp.Stage_Contex[current_stage_no-1].stAlterParam;//阶段选项参数
			/*数据库设定的行人绿闪时间*/
			walker_green_flicker = ((stage_count_temp.stAlterParam & 0xe0)>>4);
			if(walker_green_flicker<6) walker_green_flicker = 6;
			for(i = 0; i < Channel_Max; i++)
			{
				if(Schedule_Struct_exe_temp[i+1].release_phase == 0x0A)
				{
					stage_count_temp.relPhase |= (1<<i);					//放行相位2B
				}
				stage_count_temp.stGreenCount[i] 	= Schedule_Struct_exe_temp[i+1].green_count;		//阶段16通道绿灯计数
				stage_count_temp.stYellowCount[i] = Schedule_Struct_exe_temp[i+1].yellow_count;		//阶段16通道黄灯计数
				stage_count_temp.stRedCount[i] 		= Schedule_Struct_exe_temp[i+1].red_count;			//阶段16通道红灯计数
			}
			/*	lxb相位输出新接口1.13		*/
				
			tsk_execute_over= (__TRUE);//喂狗
			
			if(MSG_Send_Flag == 1)//发送消息
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

//输出任务——计数器任务

//输出任务——计数器任务
#define 	Msg_From_Execute_Task	(1)	//方案执行任务触发
#define 	Msg_From_Tim4_Int			(2)	//定时中断触发

extern CHANNEL_OUTPUT_TOB1_t Passage_Out_B1(STAGE_TIMING_COUNT_t *channel_output_C1,u16* channel_ability,u16* passage_walker);
extern u16 Phase_Out_96(STAGE_TIMING_COUNT_t *channel_output_C1,PHASE_OUTPUT_16STAT_t *phase_output_temp);
u8		Flashing_pulse_500ms		= 0;//500ms状态值，500ms翻转一次
u8		Flashing_pulse_1s				= 0;//1000ms状态值，1000ms翻转一次

extern U16	type2_follow_phase;//第三种类型的跟随相位

extern U8 find_current_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U16 current_phase_run);
extern U8 find_next_phaseNO(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO);
extern U8 is_current_phase_over(U16 phase_order_t[PHASET_ARR_NO][5],U8 current_phaseNO,STAGE_TIMING_COUNT_t *channel_output_C1);

extern void follow_phase_c8(STAGE_TIMING_COUNT_t *channel_output_C1,u16* stage_followrelPhase);
extern FOLLOW_PHASE_WORKING_t follow_phase_para[C8_LINE];
__task void Sche_Output_Task(void)
{
	/*阶段配时表各灯色时间转换的计数值局部结构体*/
	STAGE_TIMING_COUNT_t		*pstage_count_temp 	= NULL;
	/*阶段配时表各灯色时间转换的计数值局部结构体*/
	STAGE_TIMING_COUNT_t		stage_count_temp 	  = {0};
	/*16相位的相位状态表，用来转换成96表*/
	PHASE_OUTPUT_16STAT_t		phase_16status_temp	= {0};
	/*16通道的通道状态表，用来转换成B1表*/
	CHANNEL_OUTPUT_TOB1_t		channel_toB1_temp		= {0};
	/*16通道的通道使能表，某位为1，表示该通道启用，该表由B0表获得*/
	u16 channel_ability													=		0;
	/*16通道的行人通道表，某位为1，表示该通道为行人通道，该表由B0表获得*/
	u16	passage_walker 													=		0;
	u8						Flashing_pulse_Time = 0;		//500ms、1s状态翻转值计数
	u8						phase_count = 0;						//相位数计数
	u8						i = 0,j=0;
	u32						*msg;
	
	u8 send_num = 0;
	/*发给执行任务，通知阶段结束	*/
	u8 						to_Sche_Execute_MSG_temp[3] = {0};
	u8						*msg_send	= NULL;

	u8 to_Sche_Dispatch_MSG_temp[3]={0};
	u8 *msg_dispatch = NULL;
	u16						current_phase_run = 0xFFFF;		//执行完计数值--后，仍然放行的相位，若为0，则表示阶段结束
	
	u8 B0_toPhase[8]={0};//给相位板发送B0表的信息
	
	U16 stage_followrelPhase = 0;
	
	/*相序表变量*/
	U16 phase_order_num,last_phase_run = 0;
	
	/*步进处理变量*/
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
	
	U16 actual_phase = 0;//实际放行相位
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
		
		tsk_output_run= (__TRUE);//喂狗   
		
		if(*msg > 0)									//方案执行触发
		{
			pstage_count_temp = (STAGE_TIMING_COUNT_t	*)*msg;
			stage_count_temp.control_way  = pstage_count_temp->control_way;				//控制方式
			if(step_stat==0)
			{
				stage_count_temp.index 				= pstage_count_temp->index;				//	阶段配时方案号
				stage_count_temp.stageId 			= pstage_count_temp->stageId;	//阶段号
				stage_count_temp.stAlterParam = pstage_count_temp->stAlterParam;//阶段选项参数
				stage_count_temp.relPhase 		= pstage_count_temp->relPhase;					//放行相位2B
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
					case 2U://step handle 结束当前绿灯步进
					{
						step_stat = 0;
						next_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
						
// // // 						if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[next_phaseNO][1])
// // // 						{//延后放行
// // // 							current_over_phase = (phase_order_t[current_phaseNO][1] & (~phase_order_t[next_phaseNO][1]));
// // // 						}
// // // 						else
// // // 							current_over_phase = phase_order_t[current_phaseNO][1];
						if((phase_order_t[current_phaseNO][1] & phase_order_t[next_phaseNO][1]) == phase_order_t[current_phaseNO][1])
						{//延后放行
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
						stage_count_temp.stGreenCount[i] 		= pstage_count_temp->stGreenCount[i];//阶段16通道绿灯计数
						stage_count_temp.stYellowCount[i] 	= pstage_count_temp->stYellowCount[i];//阶段16通道黄灯计数
						stage_count_temp.stRedCount[i] 		  = pstage_count_temp->stRedCount[i];//阶段16通道红灯计数
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
					
// // // // // // // // 					current_phase_run = Phase_run_step(&stage_count_temp,type2_follow_phase);		//得到16相位状态表
// // 					current_stage_phase = Phase_Out_96(&stage_count_temp,&phase_16status_temp);
					current_phase_run = Phase_Out_96(&stage_count_temp,&phase_16status_temp);
// 					current_phase_run |= type2_follow_phase;
					for(phase_count=0;phase_count<16;phase_count++)
					{
						if(stage_count_temp.stGreenCount[phase_count] > 0)
						{
							stage_count_temp.stGreenCount[phase_count]--;											//绿灯计数器
						}
						else if(stage_count_temp.stYellowCount[phase_count] > 0)
						{
							stage_count_temp.stYellowCount[phase_count]--;										//黄灯计数器
						}
						else if(stage_count_temp.stRedCount[phase_count] > 0)
						{
							stage_count_temp.stRedCount[phase_count]--;												//红灯计数器
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
				
				if((current_phase_run&(~type2_follow_phase)) == 0)					//阶段结束，通知方案执行任务
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
				/* 获取500ms与1s状态翻转值 */    
				if(Flashing_pulse_Time == 2)
				{
					Flashing_pulse_500ms = ~Flashing_pulse_500ms;//1s脉冲周期，高500ms，低500ms
				}
				else if(Flashing_pulse_Time == 4)
				{
					Flashing_pulse_Time = 0;
					Flashing_pulse_500ms = ~Flashing_pulse_500ms;
					Flashing_pulse_1s = ~Flashing_pulse_1s;	//2s脉冲周期，高500ms，低500ms
				}
				else
				{
				}
				Flashing_pulse_Time++;	
				
				/*步进相序表循环*/
				if(step_flag == __TRUE)
				{
					if(step_ending == __TRUE)
					{	
						if(is_current_phase_over(phase_order_t,current_phaseNO,&stage_count_temp) == __TRUE)
						{
							current_phaseNO = find_next_phaseNO(phase_order_t,current_phaseNO);
							if(current_phaseNO == 0)					//阶段结束，通知方案执行任务
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
					输出16相位状态表96表
					16相位输出状态表行数 id=0x96
					4*2 = 8B
					typedef struct PHASE_OUTPUT_16STAT_S
					{
						US8		  statId;				//相位状态组的行号
						US16		redStat;				//相位的红灯输出状态
						US16		yellowStat;		//相位的黄灯输出状态
						US16		greenStat;			//相位的绿灯输出状态
					}PHASE_OUTPUT_16STAT_t;
				*/
				current_phase_run = Phase_Out_96(&stage_count_temp,&phase_16status_temp);		//得到16相位状态表
				if((current_phase_run != stage_count_temp.relPhase)||(current_phase_run==0)) 
				{
					//阶段结束或是感应阶段相位结束
				}
				/*
					16相位状态表96表转换成8相位状态表96表
					相位输出状态表行数 id=0x96
					4*2 = 8B
					typedef struct PHASE_OUTPUT_STAT_S
					{
						US8		statId;				//相位状态组的行号
						US8		redStat;				//相位的红灯输出状态
						US8		yellowStat;		//相位的黄灯输出状态
						US8		greenStat;			//相位的绿灯输出状态
					}PHASE_OUTPUT_STAT_t;
				*/
				
				
				/*
					输出16通道状态表B1表
					typedef struct CHANNEL_OUTPUT_TOB1_S
					{
						US8			index;							//	行号
						US16		redStat;					//16通道的红灯输出状态
						US16		yellowStat;				//16通道的黄灯输出状态
						US16		greenStat;				//16通道的绿灯输出状态
					}CHANNEL_OUTPUT_TOB1_T;
				*/
				channel_toB1_temp = Passage_Out_B1(&stage_count_temp,&channel_ability,&passage_walker);		//得到16通道状态表
				
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
							/*  软件复位 */
							NVIC_SystemReset();	 
						}
						//上报相位冲突表，降级运行,冲突
						Phase_conflict_answer_Data[0] = 0x83;
						Phase_conflict_answer_Data[1] = 0x97;
						Phase_conflict_answer_Data[2] = 0x00;
						Phase_conflict_answer_Data[3] = stage_count_temp.stageId+1;//行号此时等于阶段号?
						Phase_conflict_answer_Data[4] = (u8)((actual_phase & 0xFF00) >> 8);//冲突相位;
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
						
						/*	触发降级		*/
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
					16通道状态表B1表转换成输出8通道状态表B1表
					typedef struct CHANNEL_OUTPUT_TOB1_S
					{
						US8			index;					//	行号
						US8		redStat;					//8通道的红灯输出状态
						US8		yellowStat;				//8通道的黄灯输出状态
						US8		greenStat;				//8通道的绿灯输出状态
					}CHANNEL_OUTPUT_TOB1_T;
				*/
				//给相位板发送灯状态信息
				if(os_mut_wait(Tab_B1_Mutex,40) == OS_R_OK)//等待互斥量
				{
					Tab_B1[1].index = 1;
					Tab_B1[1].redStat 		= (u8)(channel_toB1_temp.redStat 		& 0xff);
					Tab_B1[1].yellowStat  = (u8)(channel_toB1_temp.yellowStat & 0xff);
					Tab_B1[1].greenStat 	= (u8)(channel_toB1_temp.greenStat 	& 0xff);
					Tab_B1[2].index = 2;
					Tab_B1[2].redStat 		= (u8)((channel_toB1_temp.redStat>>8) 	 & 0xff);
					Tab_B1[2].yellowStat  = (u8)((channel_toB1_temp.yellowStat>>8) & 0xff);
					Tab_B1[2].greenStat 	= (u8)((channel_toB1_temp.greenStat>>8)  & 0xff);
					
					/*发送通道使能表by lxb3.24，暂用该b1表发送结构*/
					B0_toPhase[3] 			= (u8)(channel_ability& 0xff);
					B0_toPhase[4] 		= (u8)((channel_ability>>8) & 0xff);
					
					/*发送16通道的行人通道表，某位为1，表示该通道为行人通道，该表由B0表获得*/
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
				os_mut_release (Tab_B1_Mutex);//释放互斥量
				
				/*
					阶段配时表各灯色时间计数减1
					typedef struct STAGE_TIMING_COUNT_S
					{
						US8			index;					//	阶段配时方案号
						US8			stageId;					//阶段号
						US16		relPhase;				//放行相位2B
						US16		stGreenCount[16];		//阶段16通道绿灯计数
						US16		stYellowCount[16];	//阶段16通道黄灯计数
						US16		stRedCount[16];			//阶段16通道红灯计数
						US8			stAlterParam;		//阶段选项参数
					}STAGE_TIMING_COUNT_t; 
				*/
				for(phase_count=0;phase_count<16;phase_count++)
				{
					if(stage_count_temp.stGreenCount[phase_count] > 0)
					{
						stage_count_temp.stGreenCount[phase_count]--;											//绿灯计数器
					}
					else if(stage_count_temp.stYellowCount[phase_count] > 0)
					{
						stage_count_temp.stYellowCount[phase_count]--;										//黄灯计数器
					}
					else if(stage_count_temp.stRedCount[phase_count] > 0)
					{
						stage_count_temp.stRedCount[phase_count]--;												//红灯计数器
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
					阶段配时表各灯色时间计数全局更新
				*/
				os_mut_wait(Stage_Count_Mut,0xFFFF);
				stage_timing_count = stage_count_temp;				
				os_mut_release(Stage_Count_Mut);
			
				if(step_flag == __FALSE)
				{
					if((current_phase_run&(~type2_follow_phase)) == 0)					//阶段结束，通知方案执行任务
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

u8   	 eth_temp_data[ETH_TEMP_SIZE]={0};    //  用于保存整个数据包 到temp			//by_lm
u32    eth_temp_lenth=0;				//  用于保存整个数据包的长度		  
u32    eth_temp_current=0;			//  用于保存但前命令的长度	 eth_temp_current <= eth_temp_lenth	

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

	u8 								line_temp						=0;  	//   行数
//	u8 								cor_temp						=0;		//   列数
	u32                msg_data_lenth  		=0;
	
	/*
		查找最近的IPI 和 操作类型
		如果第一个数是IPI,第二个数是正确的操作类型,则"*p_Rx_index += 1;"不会被执行.
		反之,查找到最后一个字节为止
	*/
	*p_Rx_index =0;

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

	for(i = 0; i < (rx_count-1); i++)  
	{
		/*  找命令头(IPI和操作类型)  */
		if((p_Rx_buf[i] == MES_IPI) && ((p_Rx_buf[i+1] >= MES_OPTION_MIN)))
			break;	
		else
			*p_Rx_index += 1;
	}	

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

		
	/*
			整个包的数据都不对 
			没能找到一个正确的命令头(IPI和操作类型)
	*/
	if(*p_Rx_index ==rx_count-1)  
	{
		*p_Rx_index = rx_count;
		return (ERROR_OTHER);
	}
	
	/*
			命令包的数据前面的 ="*p_Rx_index"字节数是不对的
			之后找到一个正确的命令头(IPI和操作类型)
	*/		
	else if(*p_Rx_index !=0) 
	{
		return (ERROR_OTHER);	
	}
	else 
	{		
		if(rx_count<4)   //   接收到的数据太短
		{
			*p_Rx_index = rx_count;
			return (ERROR_TOOSHORT);	
		}			

		Proto_Blk.count_target   = (u8)(( (p_Rx_buf[1] & 0x70) >> 4) + 1);  //对象个数
		Proto_Blk.operation_type = (u8)(   p_Rx_buf[1] & 0x0F);             //操作类型
		
		*p_Rx_index=2;		// IPI占用1B，操作类型占用1B
		index = *p_Rx_index;
    if(Proto_Blk.operation_type ==0)   						//查询请求
		{
				for (i=0; i<Proto_Blk.count_target; i++)
				{
						Proto_Blk.opobj_data[i].ID          =  p_Rx_buf[index++];                 //对象标识
						Proto_Blk.opobj_data[i].count_index = (p_Rx_buf[index	 ]&0xC0) >> 6;			//索引个数
						Proto_Blk.opobj_data[i].sub_target  =  p_Rx_buf[index++]&0x3F;       			//子对象号(第几个子对象,0表示所有的子对象即整行)
						
						*p_Rx_index +=1;		//   "对象标识"占用一个字节
						*p_Rx_index +=1;		//   "索引个数+子对信号"占用一个字节
						
						ID_temp 						=Proto_Blk.opobj_data[i].ID;
						count_index_temp		=Proto_Blk.opobj_data[i].count_index;
						sub_target_temp			=Proto_Blk.opobj_data[i].sub_target;
						

						//-----------------------------------------
						/* 	对象标识错误 */
////						if((ID_temp 	<	MIN_ID)||    
////							 ( ID_temp	>	MAX_ID)	)
								if((ID_temp 	<	ROAD_MAP_ID)||
									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
							     ( ID_temp	>	MAX_ID)	) //mm20140509)
						  
						{
							/*   现在ID所占用的位置有可能就是下一个IPI,因此从下标为2开始查找  */
							*p_Rx_index = 2;
							if(rx_count <=5)
							{
									*p_Rx_index =rx_count;    //小于等于5个数据的包，不可能存在两个包头之后还有完整的命令
							}
							else 
							{
								for(i = 2; i < (rx_count-2-1); i++) 
								{
									/*  找下一个命令头(IPI和操作类型)  */
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
						 /* 	子对象个数错误 */
						if((ID_temp 	>	ROAD_MAP_ID)||(ID_temp 	<	ROAD_MAP_ID))//
							{
							if((sub_target_temp > SUBOBJ_NUM_MAX) 
								 ||(subobject_table[ID_temp - 0X81][sub_target_temp] == 0))
								return (ERROR_OTHER);	
							}						
						//-----------------------------------------
										
	//=====================   以下表示IPI，ID都正确 =======
						if(count_index_temp 			==0)				// 单对象、整表(单索引,双索引)
						{
							
						}
						else if(count_index_temp 	==1)  			// 单索引
						{
								Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  索引1  */
								*p_Rx_index +=1;    																					/*索引占用1Byte */
								
								if(sub_target_temp ==0)  	//整行
								{
								}
								else  				 						//  单个子对象
								{	 
								}
							
						}
						else if(count_index_temp 	==2)  			// 双索引
						{
								Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  索引1  */
								Proto_Blk.opobj_data[i].sub_index[1] = p_Rx_buf[index++];			/*  索引2  */
								*p_Rx_index +=1;    																					/*索引1占用1Byte */
								*p_Rx_index +=1;    																					/*索引2占用1Byte */

								if(sub_target_temp ==0)  	//整行
								{
								}
								else   										//  单个子对象
								{	 
								}
											
						}
						else																	// 三索引表
						{
							/*  三索引表，保留  */
						}
					
				}
				
		}
		else if((Proto_Blk.operation_type ==0x01) ||	//设置请求
						(Proto_Blk.operation_type ==0x02) )		//设置请求,无应答
		{
					
				for (i=0; i<Proto_Blk.count_target; i++)
				{
					Proto_Blk.opobj_data[i].ID          =  p_Rx_buf[index++];                 //对象标识
					Proto_Blk.opobj_data[i].count_index = (p_Rx_buf[index	 ]&0xC0) >> 6;			//索引个数
					Proto_Blk.opobj_data[i].sub_target  =  p_Rx_buf[index++]&0x3F;       			//子对象号(第几个子对象,0表示所有的子对象即整行)
					
					*p_Rx_index +=1;		//   "对象标识"占用一个字节
					*p_Rx_index +=1;		//   "索引个数+子对信号"占用一个字节
					
					ID_temp 						=Proto_Blk.opobj_data[i].ID;
					count_index_temp		=Proto_Blk.opobj_data[i].count_index;
					sub_target_temp			=Proto_Blk.opobj_data[i].sub_target;
	
					//-----------------------------------------
					/* 	对象标识错误 */
// // 					if((ID_temp 	<	MIN_ID)	||   
// // 						 (ID_temp		>	MAX_ID)	)

// 								if((ID_temp 	<	ROAD_MAP_ID)||
// 									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
// 							     ( ID_temp	>	MAX_ID)	)				//mm20140509)	
								if((ID_temp 	<	ROAD_MAP_ID)||
									 ((ID_temp 	<	ROAD_MAP_ID)&&(ID_temp 	<	MIN_ID)) ||    
							     ( ID_temp	>	MAX_ID)	)				//mm20140509)	
					{
						/*   现在ID所占用的位置有可能就是下一个IPI,因此从下标为2开始查找  */
						*p_Rx_index = 2;
						if(rx_count <= 5)
						{
								*p_Rx_index =rx_count;    //小于等于5个数据的包，不可能存在两个包头之后还有完整的命令
						}
						else 
						{
							for(i = 2; i < (rx_count-2-1); i++) 
							{
								/*  找下一个命令头(IPI和操作类型)  */
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
					 /* 	子对象个数错误 */
					if((ID_temp 	>	ROAD_MAP_ID)||(ID_temp 	<	ROAD_MAP_ID))//
						{
						if((sub_target_temp > SUBOBJ_NUM_MAX) ||(subobject_table[ID_temp - 0X81][sub_target_temp] == 0))
							return (ERROR_OTHER);	
						}					
					//-----------------------------------------
					
					
					//=====================   以下表示IPI，ID都正确 =======
					line_temp=Object_table[ID_temp-0x81].Line_Number; 		//得到当前ID对应的最大行数 (line_max)
					
					if(count_index_temp 			==0)									// 单对象、整表(单索引,双索引)
					{	
						/*  不可设置的对象标识  */
						if(ID_temp ==0x92)
						{
							*p_Rx_index = rx_count;   //  把整个包扔掉
							 return (ERROR_OTHER);								
						}			
						
						if((ID_temp ==0x85)||(ID_temp ==0x8E)||
							 (ID_temp ==0x91)||(ID_temp ==0x92)||
							 (ID_temp ==0x9F)||(ID_temp ==0xC1)||
							 (ID_temp ==0xCA))   //   整表数据太大
						{
							*p_Rx_index = rx_count;   //  把整个包扔掉
							 return (ERROR_OTHER);	
						}
						else
						{
							/*
							由于所查的表中"单对象、整表"长已经包含了"对象标识"所占用一个字节数,
							以及"索引个数+子对象号"所占用一个字节数
							因此,此处要减去 2.
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
								msg_data_lenth=(Object_table[ID_temp-0x81].Tab_leng ); // abcdefg 所有的参数的长度
							
								if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 == 0)) //单索引 // abcdefg 
									{
										msg_data_lenth -= 1;
									}
								else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp != 0x92)) //双索引 // abcdefg 
									{
										msg_data_lenth -= 2;
									}
								else if((Object_table[ID_temp-0X81].Index1 != 0)  && (Object_table[ID_temp-0X81].Index2 != 0) && (ID_temp == 0x92)) //双索引0x92表 // abcdefg 
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
					else if(count_index_temp 	==1)  								// 单索引
					{
							/*  不可设置的对象标识  */
							if(ID_temp ==0x92)
							{
								*p_Rx_index = rx_count;   //  把整个包扔掉
								 return (ERROR_OTHER);								
							}
							
							Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  索引1  */
							*p_Rx_index +=1;    																					/*	索引占用1Byte */
							
							if(sub_target_temp ==0)  	//整行
							{
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//每行的长度
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
							}
							else  				 						//  单个子对象
							{	 
								msg_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//单个子对象的大小
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;								
							}
					
					}
					else if(count_index_temp 	==2)  			// 双索引
					{
							Proto_Blk.opobj_data[i].sub_index[0] = p_Rx_buf[index++];			/*  索引1  */
							Proto_Blk.opobj_data[i].sub_index[1] = p_Rx_buf[index++];			/*  索引2  */
							*p_Rx_index +=1;    					/*索引1占用1Byte */
							*p_Rx_index +=1;    					/*索引2占用1Byte */

							if(sub_target_temp ==0)  	//整行
							{
// //								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng);			//每行的长度
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng * Proto_Blk.opobj_data[i].sub_index[1]);			//索引一整行的长度 // abcdefg //
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;
							}
							else  				 						//  单个子对象
							{	 
// // 								msg_data_lenth=subobject_table[ID_temp-0x81][sub_target_temp];	//单个子对象的大小
								msg_data_lenth=(Object_table[ID_temp-0x81].Line_leng * Proto_Blk.opobj_data[i].sub_index[1]);		// 索引一整行的长度 // abcdefg //
								for(j=0;j<msg_data_lenth;j++)
								{
									Proto_Blk.opobj_data[i].sub_buf[j]=p_Rx_buf[index++];
								}					
								*p_Rx_index += msg_data_lenth;								
							}
								
					}
					else
					{
						/*  三索引表，保留  */
					}
		}
	}
	
	
		else 																					//PC端不可以的请求  和   保留  
		{
			return (ERROR_OTHER);	
		}

	}


	if(*p_Rx_index > rx_count)   // 判断出来的数据大于实际接收到的数据 
	{
		*p_Rx_index = rx_count;
		return (ERROR_OTHER);
	}

	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    


	return (ERROR_NO);

}



extern void eth_read_time(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);   // debug_lm
extern void eth_set_time	(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);		// debug_lm
extern void eth_read_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);
extern void eth_set_dbase(TARGET_DATA *p_Target,to_eth_struct * p_To_eth);
extern u8  copy_to_eth(u8 *p_Data,u32 lenth);
//extern const u8 ID_type[][2];																					// debug_lm
/* 对象标识类型表 */
#define ID_ALL 						0			/*  都可以操作  */
#define ID_DB							1			/*  数据库操作  */
#define ID_NEVER_SET 			2			/*  永远不能设置，设置报错，可以查询 */
#define ID_NO_SET 				3			/*  不能设置，有设置报错，可以查询  */
#define ID_TIME		 				4			/*  时钟数据不写入数据表  */
#define ID_REAL_TIME			5			/*  实时处理数据不写入数据表 */
#define ID_PARAMETER			6			/*  参数设置表，暂时不能设置，可查询，单独一个分支 */
#define ID_REPORT					7			/*  写入报警日志的数据,不能设置，有设置报错，能查询 */

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

	u8 *   msg_to_dispatch = NULL;			//发送邮箱消息变量
	
 	static u8		to_dispatch_msg[5];		//发送给方案调度任务消息

	u8		 trigger_dispatch_temp;			//8d、8e、c0、95表触发方案调度任务的标识
	
	/*  校验ID是否属于同一个类型   */
	ID_temp =p_Protocol->opobj_data[0].ID;

	if(ID_temp == ROAD_MAP_ID)   ////标识 ROAD_MAP_ID 暂用0X70 //mm20140509)
  {
		if(p_Protocol->count_target > 1)  //错误检验 0x70 命令单独操作 mm20140509)
		{
			p_To_eth->error_type=ERROR_OTHER;
			goto DO_ERROR;
		}
		if(p_Protocol->operation_type ==0) 								/*   查询操作   */
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
		else if(p_Protocol->operation_type ==1) 								/*   设置操作   */
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
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);
									
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else if(p_Protocol->operation_type ==1) 								/*   设置操作   */
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
											/* 协议解析触发方案调度任务	*/
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
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
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
						else if(p_Protocol->operation_type ==1) 								/*   设置操作   */
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
										/*方案数据被更新，重新调度执行新的方案*/
										
										trigger_dispatch_temp = 1;
										/* 协议解析触发方案调度任务	*/
										to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
										msg_to_dispatch	= to_dispatch_msg;
										os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
									}
								}
									
							}
							
							if (trigger_dispatch_temp == 1)
							{
								/* 协议解析触发方案调度任务	*/
								to_dispatch_msg[MSGA_TRIGGER_WAY] = 3;
								msg_to_dispatch	= to_dispatch_msg;
								os_mbx_send(Sche_Dispatch_Mbox, msg_to_dispatch, 0x10);
							}
						}
						break;
					}
			

			case ID_NEVER_SET:
					{ 
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
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
						else //if(p_Protocol->operation_type ==1) 								/*   设置操作   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}
			case ID_NO_SET:
					{
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
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
										os_mut_wait (Tab_B1_Mutex,0xfffe);//等待互斥量
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].index;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].redStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].yellowStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[1].greenStat;
										
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].index;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].redStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].yellowStat;
										p_To_eth->data[p_To_eth->lenth++]	= Tab_B1[2].greenStat;
										os_mut_release (Tab_B1_Mutex);//释放互斥量
										
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
						else //if(p_Protocol->operation_type ==1) 								/*   设置操作   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}
			case ID_TIME:
					{
						//if (red_to_auto ==__TRUE)  //开机全红之前不允许设置时间
						//{
									if(p_Protocol->operation_type ==0)  										/*   查询操作   */
									{
										for (i=0; i<p_Protocol->count_target; i++)
										{
												eth_read_time( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
												if(p_To_eth->error ==__TRUE)goto DO_ERROR;
										}
									}					
									else if(p_Protocol->operation_type ==1) 								/*   设置操作   */
									{
										if (red_to_auto ==__TRUE)  //开机全红之前不允许设置时间
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
																/* 协议解析触发方案调度任务	*/
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
						if(p_Protocol->operation_type ==1) 								/*   设置操作   */
						{
							if (rt_operate_written(p_Protocol,  p_To_eth) == __FALSE)	
							{	//设置了不该设置的ID，返回错误
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;	
							}
						}
						else if(p_Protocol->operation_type ==0) 								/*   查询操作   */
						{
							if (rt_operate_read(p_Protocol,  p_To_eth) == __FALSE)	
							{//查询了不该查询的ID，返回错误
								p_To_eth->error = __TRUE;
								p_To_eth->error_type = ERROR_OTHER;
								goto DO_ERROR;						
							}
						}
						break;
					}
			case ID_PARAMETER:
					{
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &p_Protocol->opobj_data[i] ,  p_To_eth);								
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else //if(p_Protocol->operation_type ==1) 								/*   设置操作   */
						{
							p_To_eth->error = __TRUE;
							p_To_eth->error_type = ERROR_OTHER;
							goto DO_ERROR;
						}
						break;
					}	
			case ID_REPORT:
					{
						if(p_Protocol->operation_type ==0)  										/*   查询操作   */
						{
							for (i=0; i<p_Protocol->count_target; i++)
							{
								eth_read_dbase( &(p_Protocol->opobj_data[i]) ,  p_To_eth); 
								if(p_To_eth->error ==__TRUE)goto DO_ERROR;
							}
						}
						else if(p_Protocol->operation_type ==1) 								/*   设置操作   */
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
			case ID_92_NEVER_SET:/* 92表日志 单独处理 */
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
							
							
						if(p_Protocol->operation_type ==0) 								/*   查询操作   */
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
								if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	已经正确发送到ETH-FIFO */  
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
	/*	消息类型字节	其中0x04为下发的与上传的消息类型之差	*/
	p_To_eth->data[0] =((p_Protocol->count_target-1) <<4) | (p_Protocol->operation_type) | 
											(0x04) | (0x80);							
	
	if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	已经正确发送到ETH-FIFO */  
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
	/*	消息类型字节	其中0x06为下发的与上传的消息类型之差	*/
	p_To_eth->data[0] =((p_Protocol->count_target-1) <<4) | (0x06)| (0x80);							
	p_To_eth->data[1] =p_To_eth->error_type;
	p_To_eth->lenth		=2;
	
	if(copy_to_eth(p_To_eth->data, p_To_eth->lenth) ==0)/*	已经正确发送到ETH-FIFO */  
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

//以太网——数据更新函数
u8	Protocol_Flag				= 0;    //debug_lm
__task void DB_Update_Task(void)
{	
	u8 							*msg_send;
 	u8 			Type;
	u32  						i;

//	u8  						Uart_Control[10]					={0};    //  用于串口的协议处理
//  u8							Uart_Idex									= 0;

  to_eth_struct 	to_eth										={{0}, {1},{__FALSE},{ERROR_NO}};   //发往以太网的数据 

	u8*							p_Temp 										=NULL;

//	u8 send_temp[2]={0};

	for(;;)
	{
		os_mbx_wait (DB_Update_Mbox, (void *)&msg_send, 0xffff);
		
		#ifdef LIMING_DEBUG_12_23
			printf("-DB_Up-\n");
		#endif	
		tsk_DB_run= (__TRUE);//喂狗

		Type = *msg_send;
// 		SEND_LED_ON();
		
// // // 		send_temp[0]=(uint8_t)SEND_LED;          			// LED序号
// // // 		send_temp[1]=(uint8_t)LED_ON;												// 工作状态			
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
									eth_temp_lenth =ETH_TEMP_SIZE;  //  得到整个数据包的长度
									rx_counter = rx_counter -(ETH_TEMP_SIZE-eth_temp_lenth);
								}
								else    
								{
									for(i=0;i<rx_counter;i++)   
									{
										eth_temp_data[eth_temp_lenth+i] = eth_rx_fifo[rx_rd_index];
										if (++rx_rd_index == RX_FIFO_SIZE) rx_rd_index=0;
									}
									eth_temp_lenth += rx_counter;  //  得到整个数据包的长度
									rx_counter=0;   /*   清空  ETH_FIFO  */
								}
							}
							tsk_unlock();            //  enable  task switching 
							//-------------------------Read From FIFO end  ------
	
							Protocol_Flag=proto_explain_check(eth_temp_data,&eth_temp_current, eth_temp_lenth);  
							
							if(Protocol_Flag ==0)
							{
								proto_explain_lm( &Proto_Blk, &to_eth);  //协议解析
							}
							else 
							{					
									to_eth.error =__FALSE;
								
									/*	消息类型字节	其中0x06为下发的与上传的消息类型之差	*/
									to_eth.data[0] =(0x06)| (0x80);							
									//to_eth.data[1] =to_eth.error_type;
									to_eth.data[1] =Protocol_Flag;
								
									to_eth.lenth	 =2;
									
									//if(copy_to_eth(&to_eth.data, to_eth.lenth) ==0)/*	已经正确发送到ETH-FIFO */  
									if(copy_to_eth(to_eth.data, to_eth.lenth) ==0)/*	已经正确发送到ETH-FIFO */ 
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
							
							//-------------------------左移操作   start   -------------
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
							//-------------------------左移操作   end   -------------
							

							
					}		while(eth_temp_lenth);	
		} //end of "if(Type == 1)"

// 		SEND_LED_OFF();
// // // 		send_temp[0]=(uint8_t)SEND_LED;          			// LED序号
// // // 		send_temp[1]=(uint8_t)LED_OFF;												// 工作状态			
// // // 		os_mbx_send(mailbox_led,(void*)send_temp,0xFF);

		tsk_DB_over= (__TRUE);//喂狗   
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
	u32								No_line_temp				= 0;	//第几行
	u32								Index1_lines_count  = 0;
	u32								Index2_lines_count  = 0;
	u32								Index2_lines_count_temp  = 0;

	u8								line_bytes_temp			= 0;	//每行字节数

	u8							  ID_temp						  =0;
	u8							  count_index_temp		=0;
	u8							  sub_target_temp		  =0;
	
	u8							  index1_temp		      =1;
	u8							  index2_temp		      =0;

	u8  time_temp[4]={0};
	u8  write_temp[20]={0};
  
	
	u32  occur_time=0;


	//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	
	ID_temp=0x92;
	count_index_temp	  	=2;
	sub_target_temp	  		=0;
	write_log_rd_index=0;
	
//	old_address	= Object_table[ID_temp-0X81].Data_Addr;//获得地址

	Data_addr_old	= Object_table[ID_temp-0X81].Data_Addr;//获得数据地址
	Data_addr_temp = Data_addr_old;
	Flag_addr_old = Object_table[ID_temp-0X81].Flag_Addr;//获得标志地址
	Flag_addr_temp = Flag_addr_old;
	
	for(;;)
	{

		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    

		if(os_mbx_wait(Write_Log_Mbox, (void *)&msg, 0xffff) ==OS_R_OK );
		{
			
			tsk_log_run=(__TRUE);
			
			#ifdef LIMING_DEBUG_12_23
				printf("-Write_L-\n");
			#endif	
			
			tsk_lock();           	 //  disables task switching 
			if(write_log_counter !=0)
			{
				for(i=0;i<21;i++)   //比较是否重复数据
				{
					if(read_temp[i] == write_log_wr_fifo[write_log_rd_index][i])  k++;
	
				}
				for(i=0;i<21;i++)    //  把收到的所有的数据拷贝到temp里面，
				{
					read_temp[i] = write_log_wr_fifo[write_log_rd_index][i];
					write_log_wr_fifo[write_log_rd_index][i] = 0;	   //数据读出后，内存清0
				}
				if (++write_log_rd_index > WRITE_LOG_SIZE) write_log_rd_index=0;
					
					--write_log_counter;
				tsk_unlock(); 
				if(k==21)
				{
					k=0;
					//重复数据不处理
				}
				else 
				{
					k=0;
					os_mut_wait(I2c_Mut_lm,0xFFFF);
					occur_time = Read_DS3231_Time();//事件发生时间//待开
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
					Data_addr_temp	=	Data_addr_old+1;	//跳过整表前一个行数字节
//				address_temp	=	old_address	+3;	//跳过整表前三个标志字节
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
	//=========================================		写92表		
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
							No_line_temp=(index1_temp-1)*255+index2_temp;	//应该写入的实际行号			
							//line_num_temp=1;					
							line_bytes_temp  = 10;	//获得行字节数
							
							if(sub_target_temp ==0)  													//整行
							{	
								Wr_u8(Flag_addr_temp,TAB_OK_FLAG);						//写整表标志位		
								Wr_u8((Flag_addr_temp + No_line_temp),LINE_OK_FLAG) ;//写行标志位
	//							Wr_u8((old_address+3+(No_line_temp-1)*(2+line_bytes_temp)),line_OK_flag_temp) ;//写行标志位

								Data_addr_temp += (No_line_temp-1)*(line_bytes_temp);	//跳过前面N-1行的地址
								
	//							address_temp += 2;										                //跳过本行的  有效标志 保留
								for(j=0;j<line_bytes_temp;j++)
								{
									Wr_u8(Data_addr_temp++,write_temp[j]) ;	
								}
								Wr_u8((Log_Type - 1 + INDEX_92_ADDR_START ),index2_temp); //=>>写到第几个索引2（流水号）  另开辟空间
								//Wr_u8(((Log_Type-1)*(255*(10+2))+(old_address+3) +1 ),index2_temp); //=>>写到第几个索引2（流水号）  另开辟空间

							}
							
						}
						//==========line 行数处理================================
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
						Wr_u8(Data_addr_old , Index1_lines_count );					//写索引一有效行数

						os_mut_release(Spi_Mut_lm);
									
					}

				//========================================================TCP上报

					//添加发送报警数据
					client_data_type = 2;//数据类型  2--其他数据

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
		
// 		os_mut_wait (Tab_B1_Mutex,0xfffe);//等待互斥量
//  		Send_Channel_Output_Unit(Tab_B1[1]);
//  		Send_Channel_Output_Unit(Tab_B1[2]);
// 		os_mut_release (Tab_B1_Mutex);//释放互斥量

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
//	u8 old_id = 0;//备份id
//	u8 error_id[8] = {0};//错误板id
//	u8 error_id_count = 0;//个数

//	u8	count_for_degrade = 0;

	u8 *msg_send;

	//故障灯号
//	U8 err_light_no = 0;
	u8 send_temp1[2]={0};

// U8 xx=0;
	CAN_init (1, 500000);               /* CAN controller 1 init, 100 kbit/s   */

  CAN_rx_object (1, 2,  33, DATA_TYPE | STANDARD_TYPE); /* Enable reception  */

  CAN_hw_testmode(1, CAN_BTR_SILM ); /* Normal         											*/

  CAN_start (1);                      /* Start controller 1   */


	for (;;) 
	{

		//IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
		tsk_CAN_run=(__TRUE);
		
		/* When message arrives store received value to global variable Rx_val   */
		if (CAN_receive (1, &RxMessage, 0x00FF) == CAN_OK)  
		{
			#ifdef LIMING_DEBUG_12_23
				printf("-rece_CAN-\n");
			#endif				
			
			/*以下是lxb9.2根据板际通信协议的新协议所修改的部分*/

			if( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xE1||RxMessage.data[1] == 0xE2||RxMessage.data[1] 
						== 0xE3||RxMessage.data[1] == 0xA7)) //
			{				
					//写日志START===========
					tsk_lock ();  			//  disables task switching 
					if(RxMessage.data[1] == 0xE1) 		 write_log_wr_fifo[write_log_wr_index][0] =Type_E1;
					else if(RxMessage.data[1] == 0xE2) write_log_wr_fifo[write_log_wr_index][0] =Type_E2;
					else if(RxMessage.data[1] == 0xE3) write_log_wr_fifo[write_log_wr_index][0] =Type_E3;
					else if(RxMessage.data[1] == 0xA7) write_log_wr_fifo[write_log_wr_index][0] =Type_A7;				

					write_log_wr_fifo[write_log_wr_index][1] = RxMessage.data[1];   //事件值1  事件标识
					write_log_wr_fifo[write_log_wr_index][2] = RxMessage.data[3];		//事件值
					write_log_wr_fifo[write_log_wr_index][3] = RxMessage.data[4];   //事件值
					write_log_wr_fifo[write_log_wr_index][4] = RxMessage.data[5];   //事件值				
								
					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
												
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
											//写日志END===========

				send_temp1[0]=(uint8_t)ERROR_LED;          			// LED序号
				send_temp1[1]=(uint8_t)LED_ON;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);
			}
			else if( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xA6)) //黄闪状态表
			{				
					//写日志START===========
					tsk_lock ();  			//  disables task switching 

					write_log_wr_fifo[write_log_wr_index][0] =Type_A6;		 //事件类型编号	
					write_log_wr_fifo[write_log_wr_index][1] = RxMessage.data[1];   //事件值1  事件标识
					write_log_wr_fifo[write_log_wr_index][2] = RxMessage.data[3];		//事件值
					write_log_wr_fifo[write_log_wr_index][3] = RxMessage.data[4];   //事件值
					write_log_wr_fifo[write_log_wr_index][4] = RxMessage.data[5];   //事件值
		
								
					if (++write_log_wr_index > WRITE_LOG_SIZE) write_log_wr_index=0;
												
					if (++write_log_counter  > WRITE_LOG_SIZE)
					{
						write_log_counter=WRITE_LOG_SIZE;
						write_log_buffer_overflow=__TRUE;
					};
					tsk_unlock (); 			//  enable  task switching 			
					os_mbx_send (Write_Log_Mbox, msg_send, WRITE_LOG_WAIT_TIMES);
											//写日志END===========
							
				send_temp1[0]=(uint8_t)ERROR_LED;          			// LED序号
				send_temp1[1]=(uint8_t)LED_ON;												// 工作状态			
				os_mbx_send(mailbox_led,(void*)send_temp1,0xFF);
			}

			if ( (RxMessage.len == 8) 
				&& (RxMessage.data[7] == MSG_END)
				&& (RxMessage.data[0] == IPI)
				&& (RxMessage.data[1] == 0xA7)&& (RxMessage.data[3] == 0xFF)&& (RxMessage.data[5] == 0x01))//重启设备
				{
//					tsk_lock();
				}
		}
		tsk_CAN_over=(__TRUE);
	}
}


