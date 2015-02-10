#ifndef __FLASH_H
#define __FLASH_H


extern u32 used_flag;
extern u16 CHIP_SELECT_PIN;
//flash使用标记 
#define FLASH_USED_FLAG        ((u32)0x0000)                    //必须为0xBC
/*
//1 以太网参数
#define FLASH_ETHERNET_PARA    ((u32)0x0000)                    //以太网参数
#define FLASH_USER_NAME        ((u32)FLASH_ETHERNET_PARA+4)     //用户名 16Byte
#define FLASH_USER_PASSWORD    ((u32)FLASH_USER_NAME+4*16)      //密码   16Byte
#define ETHERNET_LOCAL_IP      ((u32)FLASH_USER_PASSWORD+4*16)  //本地IP
#define ETHERNET_NET_MASK      ((u32)ETHERNET_LOCAL_IP+4)       //子网掩码
#define ETHERNET_GATEWAY       ((u32)ETHERNET_NET_MASK+4)       //网关
#define ETHERNET_REMOTE_IP     ((u32)ETHERNET_GATEWAY+4)        //远程主机IP 
#define ETHERNET_PORT          ((u32)ETHERNET_REMOTE_IP+4)      //端口号
#define ETHERNET_DNS           ((u32)ETHERNET_PORT+4)           //DNS 
#define ETHERNET_MAC           ((u32)ETHERNET_DNS+4)            //MAC  
#define ETHERNET_END           ((u32)ETHERNET_MAC+4*6)

#define data_EEPROM_ETHEND             ((u32)ETHERNET_END)
*/
void chip_Select(u32 *p);
void Flash_Write(u32 StartAddress,u32 Data);
void Flash_Read(u32 StartAddress, void* Data);
void Flash_AreaInit(void);
/*void Flash_SaveEthernetPara(void);
void Flash_ReadEthernetPara(void);
void Flash_EthernetParaInit(void);
void Flash_AreaErase(void);
void Flash_SaveLoginPara(void);
*/


#endif

