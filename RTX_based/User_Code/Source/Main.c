
#include "Include.h"
#include <stdio.h>
#include <rt_misc.h>






#define COM_TO_PC  UART4


#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A

// // #define IP_ADD_START   					(0x40000 -100)  				/* �ڶ��������ַƫ��50  */
// // #define LOCAL_IP_ADDR      			IP_ADD_START  						/* 4B */
// // #define LOCAL_MASK_ADDR       	(LOCAL_IP_ADDR					+4) 	/* 4B */
// // #define LOCAL_GATEWAY_ADDR    	(LOCAL_MASK_ADDR			+4)  	/* 4B */
// // #define LOCAL_PORT_ADDR    			(LOCAL_GATEWAY_ADDR		+4)  	/* 2B */
// // #define LOCAL_MAC_ADDR    			(LOCAL_PORT_ADDR			+2)  	/* 6B */
// // #define SERVER_IP_ADDR    			(LOCAL_MAC_ADDR				+6)  	/* 4B */
// // #define SERVER_PORT_ADDR    		(SERVER_IP_ADDR				+4)  	/* 2B */


#define IP_ADD_START   					(0x40000 -100)  				/* �ڶ��������ַƫ��50  */
#define LOCAL_IP_ADDR      			IP_ADD_START  							/* 4B */
#define LOCAL_MASK_ADDR       	(LOCAL_IP_ADDR				+4) 	/* 4B */
#define LOCAL_GATEWAY_ADDR    	(LOCAL_MASK_ADDR			+4)  	/* 4B */
#define LOCAL_PORT_ADDR    			(LOCAL_GATEWAY_ADDR		+4)  	/* 2B */
#define SERVER_IP_ADDR    			(LOCAL_PORT_ADDR			+2)  	/* 4B */
#define SERVER_PORT_ADDR    		(SERVER_IP_ADDR				+4)  	/* 2B */


#define LOCAL_MAC_ADDR    			((0x40000 -40))  	/* 6B */


extern U8 own_hw_adr[];
extern U8 lhost_name[];
extern LOCALM localm[];

u8 aa = 0;
u8 bb =0;
u8 cc = 0;
u32 xx=0x39000;

U8  mac_adr[6] = { 0,1,2,3,4,5 };

LOCALM  ip_config = {
   { 192,168,1,150 },             // IP address
   { 192,168,1,1 },               // Default Gateway
   { 255,255,255,0 },             // Net mask
   { 194,25,2,129 },              // Primary DNS server
   { 194,25,2,130 }               // Secondary DNS server
};


// // const char menu[] =
// //    "\n"
// //    "*******************************  �źŻ���������  ******************************\n"
// // 	 "|                                                                             |\n"
// //    "|   �����źŻ��ı���IP���˿ڡ�MAC��ַ������Ŀ��������˵�IP���˿ڡ�           |\n"
// //    "|   ������⴮�ڶ��������û���κ����ö�������5����֮������Ӧ�á�             |\n"
// //    "|                                                                             |\n"
// //    "+ ���� +----------- ��ʽ ----------------+------------ ���� ------------------+\n" 
// //    "|   A  | A xxx.xxx.xxx.xxx    (ʮ����)   | ���� ����IP                        |\n"	 
// //    "|   B  | B xxx.xxx.xxx.xxx    (ʮ����)   | ���� ������������                  |\n"	 
// //    "|   C  | C xxx.xxx.xxx.xxx    (ʮ����)   | ���� ��������                      |\n"	 	 
// //    "|   D  | D xxxx               (ʮ����)   | ���� �����˿ں�                    |\n"
// //    "|   E  | E xx.xx.xx.xx.xx.xx  (ʮ������) | ���� ����MAC							        	|\n"	
// //    "|      |                                 |                                    |\n"	
// //    "|   F  | F xxx.xxx.xxx.xxx    (ʮ����)   | ���� Ŀ���������IP                |\n"	
// //    "|   G  | G xxxx               (ʮ����)   | ���� Ŀ��������Ķ˿ں�            |\n"	
// //    "|      |                                 |                                    |\n"	
// //    "|   H  | H                               | ��ʾ ��������(����)                |\n"			 
// //    "|   I  | I                               | ��ʾ ��ǰ��������                  |\n"		 
// //    "|   Q  | Q                               | �˳� ���ò�����Ӧ��                |\n"
// //    "|   X  | X                               | �ָ� ��������                      |\n"	
// //    "*******************************************************************************\n";


const char menu_T1[] =   "*******************************  �źŻ���������  ******************************\n";
const char menu_T2[] =	 "|                                                                             |\n";
const char menu_T3[] =   "|   �����źŻ��ı���IP���˿ڡ�MAC��ַ������Ŀ��������˵�IP���˿ڡ�           |\n";
const char menu_T4[] =   "|   ������⴮�ڶ��������û���κ����ö�������5����֮������Ӧ�á�             |\n";
const char menu_T5[] =   "|                                                                             |\n";
const char menu_T6[] =   "+ ���� +----------- ��ʽ ----------------+------------ ���� ------------------+\n" ;
const char menu_T7[] =  "|   A  | A xxx.xxx.xxx.xxx    (ʮ����)   | ���� ����IP                        |\n"	 ;
const char menu_T8[] =   "|   B  | B xxx.xxx.xxx.xxx    (ʮ����)   | ���� ������������                  |\n"	; 
const char menu_T9[] =   "|   C  | C xxx.xxx.xxx.xxx    (ʮ����)   | ���� ��������                      |\n";	 	 
const char menu_TA[] =   "|   D  | D xxxx               (ʮ����)   | ���� �����˿ں�                    |\n";
const char menu_TB[] =   "|   E  | E xx.xx.xx.xx.xx.xx  (ʮ������) | ���� ����MAC							        	|\n";	
const char menu_TC[] =   "|      |                                 |                                    |\n";	
const char menu_TD[] =   "|   F  | F xxx.xxx.xxx.xxx    (ʮ����)   | ���� Ŀ���������IP                |\n";	
const char menu_TE[] =   "|   G  | G xxxx               (ʮ����)   | ���� Ŀ��������Ķ˿ں�            |\n";	
const char menu_TF[] =   "|      |                                 |                                    |\n";	
const char menu_TG[] =   "|   H  | H                               | ��ʾ ��������(����)                |\n";			 
const char menu_TH[] =   "|   I  | I                               | ��ʾ ��ǰ��������                  |\n";		 
const char menu_TI[] =   "|   Q  | Q                               | �˳� ���ò�����Ӧ��                |\n";
const char menu_TJ[] =   "|   X  | X                               | �ָ� ��������                      |\n";	
const char menu_TK[] =   "*******************************************************************************\n";

char ERROR_STR [] = "\n*** ERROR: %s\n";     /* ERROR message string in code  */


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutChar (int c) 
{
  while (!(COM_TO_PC->SR & USART_SR_TXE));
  COM_TO_PC->DR = (c & 0x1FF);
  return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) 
{
	U32 i=0;
	
  while (!(COM_TO_PC->SR & USART_SR_RXNE))
	{
		i++;
		if(i==0x100000)
		{
			i=0;
			GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog;
		}
	}
  return (COM_TO_PC->DR & 0xFF);
}


/*----------------------------------------------------------------------------
  Check if a character is received
 *----------------------------------------------------------------------------*/
int SER_CheckChar (void) 
{
  if (COM_TO_PC->SR & USART_SR_RXNE)
    return (1);
  else
    return (0);
}


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
FILE __stderr;


int fputc(int c, FILE *f) 
{
  if (c == '\n')  
		{
			SER_PutChar('\r');
		}
  return (SER_PutChar(c));
}


int fgetc(FILE *f) 
{
  return (SER_PutChar(SER_GetChar()));
}


int ferror(FILE *f) 
{
  return EOF;
}


void _ttywrch(int c) 
{
  SER_PutChar(c);
}


void _sys_exit(int return_code) 
{
label:  goto label;  
}




/*----------------------------------------------------------------------------
  Line Editor
 *----------------------------------------------------------------------------*/
void getline (char *line, int n) {
  int  cnt = 0;
  char c;

  do {
    if ((c = SER_GetChar()) == CR) 
			c = LF;  
    if (c == BACKSPACE  ||  c == DEL) {     
      if (cnt != 0)  {
        cnt--;                              
        line--;                             
        putchar (BACKSPACE);               
        putchar (' ');
        putchar (BACKSPACE);
      }
    }
    else if (c != CNTLQ && c != CNTLS) {    
      putchar (*line = c);                 
      line++;                              
      cnt++;                                
    }
		
		
		
  } while (cnt < n - 1  &&  c != LF);      
  *(line - 1) = 0;                          
}

/*==============================================================================================
* �﷨��ʽ��    void set_local_ip(char* buffer)
* ʵ�ֹ��ܣ�    ���ñ���IP
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_local_ip(char* buffer)
{
	int args;                              
	int write_addr;
	int i;
	unsigned int  ip_int_temp[4]={0};
	unsigned char ip_temp[4]={0};
	
  args = sscanf (buffer, "%d.%d.%d.%d",     
                 &ip_int_temp[0],                  
                 &ip_int_temp[1], 
                 &ip_int_temp[2], 
								 &ip_int_temp[3] );

   
		if ( args < 4  ||  args == EOF)  
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else if((ip_int_temp[0]>255) ||
						(ip_int_temp[1]>255) ||
						(ip_int_temp[2]>255) ||
						(ip_int_temp[3]>255) 	)
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else 
		{
			ip_temp[0]=(unsigned char)ip_int_temp[0];
			ip_temp[1]=(unsigned char)ip_int_temp[1];
			ip_temp[2]=(unsigned char)ip_int_temp[2];
			ip_temp[3]=(unsigned char)ip_int_temp[3];
			
			printf ("\n������IP= %d.%d.%d.%d\n", ip_temp[0],
																					ip_temp[1],
																					ip_temp[2],
																					ip_temp[3]);
			
			write_addr	=LOCAL_IP_ADDR;
			for(i=0;i<4;i++)
			{
				Wr_u8(write_addr+i,ip_temp[i]) ;	
			}
		}
}
/*==============================================================================================
* �﷨��ʽ��    void set_local_mask(char* buffer)
* ʵ�ֹ��ܣ�    ���ñ�����������
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_local_mask(char* buffer)
{
	int args;                              
	int write_addr;
	int i;
	unsigned int  ip_int_temp[4]={0};
	unsigned char ip_temp[4]={0};
	
  args = sscanf (buffer, "%d.%d.%d.%d",     
                 &ip_int_temp[0],                  
                 &ip_int_temp[1], 
                 &ip_int_temp[2], 
								 &ip_int_temp[3] );

   
		if ( args < 4  ||  args == EOF)  
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else if((ip_int_temp[0]>255) ||
						(ip_int_temp[1]>255) ||
						(ip_int_temp[2]>255) ||
						(ip_int_temp[3]>255) 	)
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else 
		{
			ip_temp[0]=(unsigned char)ip_int_temp[0];
			ip_temp[1]=(unsigned char)ip_int_temp[1];
			ip_temp[2]=(unsigned char)ip_int_temp[2];
			ip_temp[3]=(unsigned char)ip_int_temp[3];
			
			printf ("\n������IP= %d.%d.%d.%d\n", ip_temp[0],
																					ip_temp[1],
																					ip_temp[2],
																					ip_temp[3]);
			
			write_addr	=LOCAL_MASK_ADDR;
			for(i=0;i<4;i++)
			{
				Wr_u8(write_addr+i,ip_temp[i]) ;	
			}
		}
}

/*==============================================================================================
* �﷨��ʽ��    void set_local_gateway(char* buffer)
* ʵ�ֹ��ܣ�    ���ñ�������
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_local_gateway(char* buffer)
{
	int args;                              
	int write_addr;
	int i;
	unsigned int  ip_int_temp[4]={0};
	unsigned char ip_temp[4]={0};
	
  args = sscanf (buffer, "%d.%d.%d.%d",     
                 &ip_int_temp[0],                  
                 &ip_int_temp[1], 
                 &ip_int_temp[2], 
								 &ip_int_temp[3] );

   
		if ( args < 4  ||  args == EOF)  
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else if((ip_int_temp[0]>255) ||
						(ip_int_temp[1]>255) ||
						(ip_int_temp[2]>255) ||
						(ip_int_temp[3]>255) 	)
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else 
		{
			ip_temp[0]=(unsigned char)ip_int_temp[0];
			ip_temp[1]=(unsigned char)ip_int_temp[1];
			ip_temp[2]=(unsigned char)ip_int_temp[2];
			ip_temp[3]=(unsigned char)ip_int_temp[3];
			
			printf ("\n������IP= %d.%d.%d.%d\n", ip_temp[0],
																					ip_temp[1],
																					ip_temp[2],
																					ip_temp[3]);
			
			write_addr	=LOCAL_GATEWAY_ADDR;
			for(i=0;i<4;i++)
			{
				Wr_u8(write_addr+i,ip_temp[i]) ;	
			}
		}
}

/*==============================================================================================
* �﷨��ʽ��    void set_local_port(char* buffer)
* ʵ�ֹ��ܣ�    ���ñ��ض˿�
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_local_port(char* buffer)
{
	int args;                              /* number of arguments               */
	int write_addr;
	int i;
	unsigned int  port_int_temp;
	unsigned char port_temp[2]={0};


  args = sscanf (buffer, "%d",     
                 &port_int_temp);


  if (  args == EOF)  
		{
			printf (ERROR_STR, "����Ķ˿ڸ�ʽ");
		}
		else if(port_int_temp>65535)
		{
			printf (ERROR_STR, "����Ķ˿ڸ�ʽ");
		}
		else 
		{
			printf ("\n������˿�= %d\n", port_int_temp);
		}
		port_temp[0]=(unsigned char)(port_int_temp&0xFF);
		port_temp[1]=(unsigned char)((port_int_temp&0xFF00)>>8);
		
		write_addr	=LOCAL_PORT_ADDR;
		for(i=0;i<2;i++)
		{
			Wr_u8(write_addr+i,port_temp[i]) ;	
		}
}
/*==============================================================================================
* �﷨��ʽ��    void set_local_mac(char* buffer)
* ʵ�ֹ��ܣ�    ���ñ���MAC
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_local_mac(char* buffer)
{
	int args;                              
	int write_addr;
	int i;
	unsigned int  mac_int_temp[6]={0};
	unsigned char mac_temp[6]={0};
	
  args = sscanf (buffer, "%x.%x.%x.%x.%x.%x",     
                 &mac_int_temp[0],                  
                 &mac_int_temp[1], 
                 &mac_int_temp[2], 
								 &mac_int_temp[3],
								 &mac_int_temp[4],
								 &mac_int_temp[5] );

   
		if ( args < 6  ||  args == EOF)  
		{
			printf (ERROR_STR, "�����MAC��ʽ");
		}
		else if((mac_int_temp[0]>255) ||
						(mac_int_temp[1]>255) ||
						(mac_int_temp[2]>255) ||
						(mac_int_temp[3]>255) ||
						(mac_int_temp[4]>255) ||
						(mac_int_temp[5]>255) 	)
		{
			printf (ERROR_STR, "�����MAC��ʽ");
		}
		else 
		{
			mac_temp[0]=(unsigned char)mac_int_temp[0];
			mac_temp[1]=(unsigned char)mac_int_temp[1];
			mac_temp[2]=(unsigned char)mac_int_temp[2];
			mac_temp[3]=(unsigned char)mac_int_temp[3];
			mac_temp[4]=(unsigned char)mac_int_temp[4];
			mac_temp[5]=(unsigned char)mac_int_temp[5];			
			
			printf ("\n������MAC= %x.%x.%x.%x.%x.%x\n", 	mac_temp[0],
																									mac_temp[1],
																									mac_temp[2],
																									mac_temp[3],
																									mac_temp[4],
																									mac_temp[5]);
			
			write_addr	=LOCAL_MAC_ADDR;
			for(i=0;i<6;i++)
			{
				Wr_u8(write_addr+i,mac_temp[i]) ;	
			}
		}
}
/*==============================================================================================
* �﷨��ʽ��    void set_server_ip(char* buffer)
* ʵ�ֹ��ܣ�    ����Ŀ�������IP
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_server_ip(char* buffer)
{
	int args;                              
	int write_addr;
	int i;
	unsigned int  ip_int_temp[4]={0};
	unsigned char ip_temp[4]={0};
	
  args = sscanf (buffer, "%d.%d.%d.%d",     
                 &ip_int_temp[0],                  
                 &ip_int_temp[1], 
                 &ip_int_temp[2], 
								 &ip_int_temp[3] );

   
		if ( args < 4  ||  args == EOF)  
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else if((ip_int_temp[0]>255) ||
						(ip_int_temp[1]>255) ||
						(ip_int_temp[2]>255) ||
						(ip_int_temp[3]>255) 	)
		{
			printf (ERROR_STR, "�����IP��ʽ");
		}
		else 
		{
			ip_temp[0]=(unsigned char)ip_int_temp[0];
			ip_temp[1]=(unsigned char)ip_int_temp[1];
			ip_temp[2]=(unsigned char)ip_int_temp[2];
			ip_temp[3]=(unsigned char)ip_int_temp[3];
			
			printf ("\n������IP= %d.%d.%d.%d\n", ip_temp[0],
																					ip_temp[1],
																					ip_temp[2],
																					ip_temp[3]);
			
			write_addr	=SERVER_IP_ADDR;
			for(i=0;i<4;i++)
			{
				Wr_u8(write_addr+i,ip_temp[i]) ;	
			}
		}
}

/*==============================================================================================
* �﷨��ʽ��    void set_server_port(char* buffer)
* ʵ�ֹ��ܣ�    ����Ŀ��������˿�
* ����1��       ���ڽ��ջ��� 
* ����ֵ��      ��
===============================================================================================*/
void set_server_port(char* buffer)
{
	int args;                              /* number of arguments               */
	int write_addr;
	int i;
	unsigned int  port_int_temp;
	unsigned char port_temp[2]={0};


  args = sscanf (buffer, "%d",     
                 &port_int_temp);


  if (  args == EOF)  
		{
			printf (ERROR_STR, "����Ķ˿ڸ�ʽ");
		}
		else if(port_int_temp>65535)
		{
			printf (ERROR_STR, "����Ķ˿ڸ�ʽ");
		}
		else 
		{
			printf ("\n������˿�= %d\n", port_int_temp);
		}
		port_temp[0]=(unsigned char)(port_int_temp&0xFF);
		port_temp[1]=(unsigned char)((port_int_temp&0xFF00)>>8);
		
		write_addr	=SERVER_PORT_ADDR;
		for(i=0;i<2;i++)
		{
			Wr_u8(write_addr+i,port_temp[i]) ;	
		}
}
/*==============================================================================================
* �﷨��ʽ��    void display_all_env(void)
* ʵ�ֹ��ܣ�    ��ʾ���е����ò���
* ����1��       ��
* ����ֵ��      ��
===============================================================================================*/
void display_all_env(void)
{
	unsigned char read_temp[6]={0};
	unsigned int  read_int_temp=0;

//=========================
	read_temp[0]=Rd_u8(LOCAL_IP_ADDR);
	read_temp[1]=Rd_u8(LOCAL_IP_ADDR+1);
	read_temp[2]=Rd_u8(LOCAL_IP_ADDR+2);
	read_temp[3]=Rd_u8(LOCAL_IP_ADDR+3);
	
	read_int_temp	=	read_temp[0]+
									read_temp[1]+
									read_temp[2]+
									read_temp[3];			
	if(read_int_temp ==0)
	{
		ip_config.IpAdr[0]=192;
		ip_config.IpAdr[1]=168;
		ip_config.IpAdr[2]=1;
		ip_config.IpAdr[3]=107;
	}
	else
	{
		ip_config.IpAdr[0]=read_temp[0];	
		ip_config.IpAdr[1]=read_temp[1];	
		ip_config.IpAdr[2]=read_temp[2];	
		ip_config.IpAdr[3]=read_temp[3];	
	}
	printf ("\n����IP= %d.%d.%d.%d\n", 	ip_config.IpAdr[0],
																			ip_config.IpAdr[1],
																			ip_config.IpAdr[2],
																			ip_config.IpAdr[3]);	
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
//-----------------		
	read_temp[0]=Rd_u8(LOCAL_MASK_ADDR);
	read_temp[1]=Rd_u8(LOCAL_MASK_ADDR+1);
	read_temp[2]=Rd_u8(LOCAL_MASK_ADDR+2);
	read_temp[3]=Rd_u8(LOCAL_MASK_ADDR+3);
	
	read_int_temp	=	read_temp[0]+
									read_temp[1]+
									read_temp[2]+
									read_temp[3];			
	if(read_int_temp ==0)
	{
		ip_config.NetMask[0]=255;
		ip_config.NetMask[1]=255;
		ip_config.NetMask[2]=255;
		ip_config.NetMask[3]=0;
	}
	else
	{
		ip_config.NetMask[0]=read_temp[0];	
		ip_config.NetMask[1]=read_temp[1];	
		ip_config.NetMask[2]=read_temp[2];	
		ip_config.NetMask[3]=read_temp[3];	
	}
	printf ("\n������������= %d.%d.%d.%d\n", 	ip_config.NetMask[0],
																						ip_config.NetMask[1],
																						ip_config.NetMask[2],
																						ip_config.NetMask[3]);
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
//------------------------------------
	read_temp[0]=Rd_u8(LOCAL_GATEWAY_ADDR);
	read_temp[1]=Rd_u8(LOCAL_GATEWAY_ADDR+1);
	read_temp[2]=Rd_u8(LOCAL_GATEWAY_ADDR+2);
	read_temp[3]=Rd_u8(LOCAL_GATEWAY_ADDR+3);

	read_int_temp	=	read_temp[0]+
									read_temp[1]+
									read_temp[2]+
									read_temp[3];			
	if(read_int_temp ==0)
	{
		ip_config.DefGW[0]=192;
		ip_config.DefGW[1]=168;
		ip_config.DefGW[2]=1;
		ip_config.DefGW[3]=1;
	}
	else
	{
		ip_config.DefGW[0]=read_temp[0];	
		ip_config.DefGW[1]=read_temp[1];	
		ip_config.DefGW[2]=read_temp[2];	
		ip_config.DefGW[3]=read_temp[3];	
	}	
	printf ("\n��������= %d.%d.%d.%d\n", 	ip_config.DefGW[0],
																				ip_config.DefGW[1],
																				ip_config.DefGW[2],
																				ip_config.DefGW[3]);	
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
//------------------------------
	read_temp[0]=Rd_u8(LOCAL_PORT_ADDR);
	read_temp[1]=Rd_u8(LOCAL_PORT_ADDR+1);

	read_int_temp	=	read_temp[0]+
									read_temp[1];			
	if(read_int_temp ==0)
	{
		local_port	=90;
	}
	else
	{
		local_port	=(read_temp[0]+((int)read_temp[1]<<8));
	}	
 	printf ("\n�����˿ں�= %d\n", 	local_port);	
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog	
//------------------------------
//------------------------------------
	read_temp[0]=Rd_u8(LOCAL_MAC_ADDR);
	read_temp[1]=Rd_u8(LOCAL_MAC_ADDR+1);
	read_temp[2]=Rd_u8(LOCAL_MAC_ADDR+2);
	read_temp[3]=Rd_u8(LOCAL_MAC_ADDR+3);
	read_temp[4]=Rd_u8(LOCAL_MAC_ADDR+4);
	read_temp[5]=Rd_u8(LOCAL_MAC_ADDR+5);
	
	read_int_temp	=	read_temp[0]+
									read_temp[1]+
									read_temp[2]+
									read_temp[3]+
									read_temp[4]+
									read_temp[5];			
	if(read_int_temp ==0)
	{
		mac_adr[0]=0x00;
		mac_adr[1]=0x50;
		mac_adr[2]=0xc2;
		mac_adr[3]=0x48;
		mac_adr[4]=0xf4;
		mac_adr[5]=0xf1;
	}
	else
	{
		mac_adr[0]=read_temp[0];
		mac_adr[1]=read_temp[1];
		mac_adr[2]=read_temp[2];
		mac_adr[3]=read_temp[3];
		mac_adr[4]=read_temp[4];
		mac_adr[5]=read_temp[5];
	}	
	printf ("\n����MAC= %x.%x.%x.%x.%x.%x\n", 	mac_adr[0],
																							mac_adr[1],
																							mac_adr[2],
																							mac_adr[3],
																							mac_adr[4],
																							mac_adr[5]);		
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	
	
//------------------------------
//------------------------------------
	read_temp[0]=Rd_u8(SERVER_IP_ADDR);
	read_temp[1]=Rd_u8(SERVER_IP_ADDR+1);
	read_temp[2]=Rd_u8(SERVER_IP_ADDR+2);
	read_temp[3]=Rd_u8(SERVER_IP_ADDR+3);
	
	read_int_temp	=	read_temp[0]+
									read_temp[1]+
									read_temp[2]+
									read_temp[3];			
	if(read_int_temp ==0)
	{
		server_ip[0]=192;
		server_ip[1]=168;
		server_ip[2]=1;
		server_ip[3]=106;
	}
	else
	{
		server_ip[0]=read_temp[0];	
		server_ip[1]=read_temp[1];	
		server_ip[2]=read_temp[2];	
		server_ip[3]=read_temp[3];	
	}	
	printf ("\nĿ�������IP= %d.%d.%d.%d\n", 	server_ip[0],
																						server_ip[1],
																						server_ip[2],
																						server_ip[3]);
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
//------------------------------
//------------------------------
	read_temp[0]=Rd_u8(SERVER_PORT_ADDR);
	read_temp[1]=Rd_u8(SERVER_PORT_ADDR+1);

	read_int_temp	=	read_temp[0]+
									read_temp[1];			
	if(read_int_temp ==0)
	{
		server_port	=2046;
	}
	else
	{
		server_port	=(read_temp[0]+((int)read_temp[1]<<8));
	}	
	printf ("\nĿ��������˿ں�= %d\n", server_port);
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
//------------------------------


	
}



//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM			BBBBBBBBBBBBBBBBBBBBBB
/*==============================================================================================
* �﷨��ʽ��    void BKPRAM_Configuration(void)
* ʵ�ֹ��ܣ�    4k�󱸴洢������
* ����1��       ��
* ����ֵ��      ��
===============================================================================================*/
void BKPRAM_Configuration(void)
{
 /* Enable the PWR APB1 Clock Interface */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable BKPRAM Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
}
/*==============================================================================================
* �﷨��ʽ��    void BKPRAM_Write(u8*p_Char,u8 lenth,uint32_t addr)
* ʵ�ֹ��ܣ�    д��󱸴洢��
* ����1��       ��
* ����ֵ��      ��
===============================================================================================*/
void BKPRAM_Write(u8*p_Char,u8 lenth,uint32_t addr)
{
	uint32_t i=0;
	
  /* Write to Backup SRAM with 32-Bit Data */
  for (i = 0x0; i < lenth; i ++)
  {
    *(__IO uint32_t *) (addr + (i)) = (uint32_t)(*(p_Char+i));
  }	
}

 



/*==============================================================================================
* �﷨��ʽ��    void BKPRAM_Read(u8*p_Char,u8 lenth,uint32_t addr)
* ʵ�ֹ��ܣ�    �Ӻ󱸴洢������
* ����1��       ��
* ����ֵ��      ��
===============================================================================================*/
void BKPRAM_Read(u8*p_Char,u8 lenth,uint32_t addr)
{
	uint32_t i=0;
	uint32_t read_temp=0;

  for (i = 0x0; i < lenth; i ++)
  {
		read_temp	=*(__IO uint32_t *) (addr + (i));
		*(p_Char+i) =(u8)read_temp;
  }
}

//BBBBBBBBBBBBBBBBBBBBBB		BKPRAM_LM end			BBBBBBBBBBBBBBBBBBBBBB



//GGGGGGGGGGGGGGGGGGGGGG		IWDG_LM		GGGGGGGGGGGGGGGGGGGGGG

uint32_t	 IWDGRST_flag	=__FALSE;

/*==============================================================================================
* �﷨��ʽ��    void IWDG_Configuration(void)
* ʵ�ֹ��ܣ�    ���Ź�����
* ����1��       ��
* ����ֵ��      ��
===============================================================================================*/
void IWDG_Configuration(void)
{
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
// //   IWDG_SetPrescaler(IWDG_Prescaler_32);
	
	IWDG_SetPrescaler(IWDG_Prescaler_128); /*  4��  */
	

  /* Set counter reload value to obtain 1000ms IWDG TimeOut. */
 	IWDG_SetReload(245*5);			/* 5000ms */  
	
	//IWDG_SetReload(245);			/* 1000ms */  
	

  /* Reload IWDG counter */
  IWDG_ReloadCounter();
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
	
	IWDG_ReloadCounter();   /* ι�� */  
}
//GGGGGGGGGGGGGGGGGGGGGG		IWDG_LM	end		GGGGGGGGGGGGGGGGGGGGGG

void display_meun (void)
{
	printf ( menu_T1 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T2 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T3 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T4 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T5 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T6 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T7 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_T8 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog	
	printf ( menu_T9 ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TA ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TB ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TC ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TD ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TE ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TF ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TG ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog		
	printf ( menu_TH ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TI ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TJ ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	printf ( menu_TK ); GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
}

#define  START_TIME_5MIN  (5)  								// TIM3��ʱ����1s����ʱ5s

vu32 start_time_count=0;

int main(void)
{
	static char cmdbuf [30];                   /* command input buffer          */
  int i;                                     /* index for command buffer      */

	
	char dly_count								=START_TIME_5MIN; 
	u32  old_start_time_count 		=start_time_count+1;

	
	u32 delay_time=0;

	DeviceInit();

// 	TIM_Cmd(TIM2, ENABLE);
// 	TIM_Cmd(TIM4, ENABLE);

	BKPRAM_Configuration();

  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)  /* IWDGRST flag set */
  {

    /* Clear reset flags */
    RCC_ClearFlag();	
		
		IWDGRST_flag	=__TRUE;
		
		printf ( "\n\n  " ); 
		printf ( "*********************************************" ); 
		printf ( "*********************************************" ); 
		printf ( "\n " ); 
		printf ( "**************** ���Ź���λ�� *************** " ); 
		printf ( "\n " ); 
		printf ( "*********************************************" ); 
		printf ( "*********************************************" ); 		
		printf ( "\n\n " ); 

		goto start_work;	/*  ������������  */   
  }
  else
  {
    /* IWDGRST flag is not set */
		IWDGRST_flag	=__FALSE;
  }
//GGGGGGGGGGGGGGGGGGGGGG		IWDG_LM	end		GGGGGGGGGGGGGGGGGGGGGG

//	printf ( menu );  
	display_meun();

	printf ("\n");
	while(1)
	{
		//GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
		
    if (SER_CheckChar() ==0)
		{
			if (start_time_count   !=old_start_time_count)
			{
				old_start_time_count =start_time_count;
				
				printf ("\r�������: %d s",dly_count);
				dly_count--;
			}

			if(dly_count ==0) 
			{						
				printf ("\n����Ӧ��!\n");
				goto start_work;	
			}
			delay_time++;
			if(delay_time>=0x350000)
			{	
				delay_time=0;
				start_time_count++;
				
			}			
			if((delay_time ==0x100000)||(delay_time ==0x200000)||(delay_time ==0x300000))
			{	
				GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
			}	
			
		}
		else 
		{
				SER_GetChar();
				while(1)		
				{
					GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
					
					printf ("\n\n���� (����h): ");
					getline (&cmdbuf[0], sizeof (cmdbuf));   /* input command line            */

					for (i = 0; cmdbuf[i] != 0; i++)  /* convert to upper characters   */
					{      
						cmdbuf[i] = toupper(cmdbuf[i]);
					}

					for (i = 0; cmdbuf[i] == ' '; i++);      /* skip blanks                   */		

					switch (cmdbuf[i])  
						{                   
							case 'A': 
								set_local_ip(&cmdbuf[i+1]);                            
								break;
							case 'B': 
								set_local_mask(&cmdbuf[i+1]);                            
								break;
							case 'C': 
								set_local_gateway(&cmdbuf[i+1]);                            
								break;
							case 'D': 
								set_local_port(&cmdbuf[i+1]);                            
								break;
							case 'E': 
								set_local_mac(&cmdbuf[i+1]);                            
								break;
							
							case 'F': 
								set_server_ip(&cmdbuf[i+1]);                            
								break;							
							case 'G': 
								set_server_port(&cmdbuf[i+1]);                            
								break;	
							
							case 'I': 
								display_all_env();                            
								break;		
							case 'Q':                              
								if(cmdbuf[i+1] ==0)
								{
									printf ("\n����Ӧ��!\n");
									goto start_work;
								}
								else 
								{
									printf (ERROR_STR, "��������");                      
								}
								break;
							case 'H':                              
								if(cmdbuf[i+1] ==0)
								{
									//printf (menu);   
									display_meun();
								}
								else 
								{
									printf (ERROR_STR, "��������");       
								}
								break;
							case 'X':                              
								if(cmdbuf[i+1] ==0)
								{
									printf ("\n�������......\n"); 
									Db_Erase();
									printf ("\n����������\n");  
									printf ("\n--------------------\n");  
									Init_Db_Value();								
									printf ("\n����Ĭ�Ϸ���\n");  									
									
								}
								else 
								{
									printf (ERROR_STR, "��������");       
								}
								break;
// // 							case 'Z':                              
// // 								if(cmdbuf[i+1] ==0)
// // 								{
// // 									Init_Db_Value();								
// // 									printf ("\n����Ĭ�Ϸ���\n");  
// // 								}
// // 								else 
// // 								{
// // 									printf (ERROR_STR, "��������");       
// // 								}
// // 								break;								
							default:                               
								printf (ERROR_STR, "��������");                  
								break;
						}

					
				}	

		}


	}
	
	
	
//=============================================================

start_work:

 	IWDG_ReloadCounter();   /* ι�� */  
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
	
// 	Db_Erase();
// 	Init_Db_Value();
	
	display_all_env();
	
	GPIO_SetBits(GPIOB, GPIO_Pin_9);//ds3231��λ���
	

	
 	os_sys_init (maintask);					/*Initialize RTX and start init*/
}
