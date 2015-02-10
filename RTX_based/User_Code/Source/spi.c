
#include "Include.h"

//======================================================================
// 函数名称：
// spi_ss(void)
//			 
// 说明：NSS控制
//======================================================================

void SPI_EEPROM_ss (u32 ss) 
{
   /* Enable/Disable SPI Chip Select (drive it high or low). */
   
   if (ss == 1)//检测机主板
   {
        GPIO_SetBits(GPIOE, CHIP_SELECT_PIN);
   }
   else
   {
			GPIO_ResetBits(GPIOE, CHIP_SELECT_PIN);
   }
}




//======================================================================
// 函数名称：
// spi_send(void)
//			 
// 说明：发送
//======================================================================
u8 SPI_EEPROM_send (u8 outb) 
{
   /* Write and Read a byte on SPI interface. */

   /* Wait if TXE cleared, Tx FIFO is full. */
   while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE));
   SPI_I2S_SendData(SPI3, outb);

   /* Wait if RNE cleared, Rx FIFO is empty. */    
    while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE)); 
    return SPI_I2S_ReceiveData(SPI3);   
}

//======================================================================
// 函数名称：
// ReadStatusReg(void)
//			 
// 说明：
//======================================================================
u8 SPI_EEPROM_ReadStatusReg(void)
 {  
	u8 status=0;

	SPI_EEPROM_ss(0);
	                                 // 片选变低
	SPI_EEPROM_send(EEPROM_RDSR);

	status = SPI_EEPROM_send(EEPROM_RDSR);

	SPI_EEPROM_ss(1);

	return status;
   
 }
//======================================================================
// 函数名称：
// WriteEnable(void)
//			 
// 说明：
//======================================================================
void SPI_EEPROM_WriteEnable(void)
 { 
     SPI_EEPROM_ss(0);
                                     // 片选变低
     SPI_EEPROM_send(EEPROM_WREN);
 
     SPI_EEPROM_ss(1);   
 }

//======================================================================
// 函数名称：
// PageProgram(void)
//			 
// 说明：最大写入32字节
//====================================================================== 
void SPI_EEPROM_Program(u32 Addr,u32 size,u8* buf)
{
     u32 i = 0;

     while((SPI_EEPROM_ReadStatusReg()&0x01 == 1) && (i <= 65530))
     {
        i++;
     }     

     SPI_EEPROM_WriteEnable();      //写允许
     
     SPI_EEPROM_ss(0);
                                     
     SPI_EEPROM_send(EEPROM_WRITE);

     SPI_EEPROM_send((Addr&0x00FF0000)>>16);

     SPI_EEPROM_send((Addr&0x0000FF00)>>8);

     SPI_EEPROM_send((Addr&0x000000FF)>>0);

     for (i=0; i<size; i++)
     {
        SPI_EEPROM_send(buf[i]);
     }
 
     SPI_EEPROM_ss(1);  
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u8 dat)
//=================================
void WrMemData(u32 addr,u8 dat)		  //写一个字节数据到指定地址
{   
    chip_Select(&addr);//芯片选择
    SPI_EEPROM_Program(addr,1,&dat);
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u8 dat)
//=================================
// void Wr_u8(u32 addr,u8 dat)		  //写一个字节数据到指定地址
// {   
//     chip_Select(&addr);//芯片选择
//     SPI_EEPROM_Program(addr,1,&dat);
// }
//================================
//数据库用驱动
//WrMemData(u32 addr,u16 dat)
//=================================
void Wr_u16(u32 addr,u16 dat)		  //写二个字节数据到指定地址
{
	u8 buf[2] = {0};
	buf[0] = (u8)((dat & 0xFF00) >> 8);
	buf[1] = (u8)(dat & 0x00FF);
        chip_Select(&addr);//芯片选择
	SPI_EEPROM_Program(addr,2,buf);
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u32 dat)
//=================================
void Wr_u32(u32 addr,u32 dat)		  //写四个字节数据到指定地址
{
    	u8 buf[4] = {0};
	buf[0] = (u8)((dat & 0xFF000000) >> 24);
	buf[1] = (u8)((dat & 0x00FF0000) >> 16);
	buf[2] = (u8)((dat & 0x0000FF00) >> 8);
	buf[3] = (u8)(dat & 0x000000FF);
        chip_Select(&addr);//芯片选择
	SPI_EEPROM_Program(addr,4,buf);
}

//================================
//数据库用驱动
//void Wr_n_byte(u32 addr, u32 size, u8* dat)
//=================================
void Wr_n_byte(u32 addr, u32 size, u8* dat)		  //写n个字节数据到指定地址
{       
		u32 i;
		chip_Select(&addr);//芯片选择
		for(i = 0; i < size; i++)
		SPI_EEPROM_Program(addr + i, 1, &dat[i]);
}
//======================================================================
// 函数名称：
// ReadOut(void)
//			 
// 说明：读一个字节
//====================================================================== 
void SPI_EEPROM_ReadOut(u32 Addr, u8* buf)
{
     SPI_EEPROM_ss(0);
                                     
     SPI_EEPROM_send(EEPROM_READ);

     SPI_EEPROM_send((Addr&0x00FF0000)>>16);

     SPI_EEPROM_send((Addr&0x0000FF00)>>8);

     SPI_EEPROM_send((Addr&0x000000FF)>>0);
	
     *buf = SPI_EEPROM_send(0x00);
 
     SPI_EEPROM_ss(1); 
}

//=====================================
//u8 RdMemData(u32 addr)		   //从指定地址读一个字节
//数据库用驱动
//=====================================
u8 RdMemData(u32 addr)		   //从指定地址读一个字节
{ 
  u8 data = 0;
  chip_Select(&addr);//芯片选择
  SPI_EEPROM_ReadOut(addr, &data);
  return data;
}
//=====================================
//u8 RdMemData(u32 addr)		   //从指定地址读一个字节
//数据库用驱动


// u8 Rd_u8(u32 addr)		   //从指定地址读一个字节
// { 
//   u8 data = 0;
//   chip_Select(&addr);//芯片选择
//   SPI_EEPROM_ReadOut(addr, &data);
//   return data;
// }
//=====================================
//u16 RdMemData(u32 addr)		   //从指定地址读二个字节
//数据库用驱动
//=====================================
u16 Rd_u16(u32 addr)		   //从指定地址读二个字节
{ 
  u16 data = 0;
  u8 data1 = 0,data2  = 0;
  chip_Select(&addr);//芯片选择
  data1 = Rd_u8(addr);
  data2 = Rd_u8(addr + 1);
  data = data1;
  data <<= 8;
  data |= data2;
  return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //从指定地址读四个字节
//数据库用驱动
//=====================================
u32 Rd_u32(u32 addr)		   //从指定地址读四个字节
{ 
  u32 data = 0;
  u16 data1 = 0,data2  = 0;
  chip_Select(&addr);//芯片选择
  data1 = Rd_u16(addr);
  data2 = Rd_u16(addr + 2);
  data = data1;
  data <<= 16;
  data |= data2;
  return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //从指定地址读n个字节
//数据库用驱动
//=====================================
void Rd_n_Byte(u32 addr, u16 size, u8* data)		   //从指定地址读n个字节
{ 
	u8 temp = 0;
	u8 i;
        chip_Select(&addr);//芯片选择
	for(i = 0; i < size; i++)
	{
		SPI_EEPROM_ReadOut(addr + i, &temp);
		data[i] = temp;
	}
}
//=====================================
//u8 RdMemData(u32 addr)		   //从指定地址读一个字节
//数据库用驱动
//=====================================
u8 Rd_u8_Log(u32 addr)		   //从指定地址读一个字节
{ 
	u8 data = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
	SPI_EEPROM_ReadOut(addr, &data);
	return data;
}
//=====================================
//u16 RdMemData(u32 addr)		   //从指定地址读二个字节
//数据库用驱动
//=====================================
u16 Rd_u16_Log(u32 addr)		   //从指定地址读二个字节
{ 
	u16 data = 0;
	u8 data1 = 0,data2  = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
	SPI_EEPROM_ReadOut(addr_new,&data1);
	SPI_EEPROM_ReadOut(addr_new+1,&data2);
	data = data1;
	data <<= 8;
	data |= data2;
	return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //从指定地址读四个字节
//数据库用驱动
//=====================================
u32 Rd_u32_Log(u32 addr)		   //从指定地址读四个字节
{ 
	u32 data = 0;
	u16 data1 = 0,data2  = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
	data1 = Rd_u16_Log(addr);
	data2 = Rd_u16_Log(addr + 2);
	data = data1;
	data <<= 16;
	data |= data2;
	return data;
}
//=====================================
//u32 RdMemData(u32 addr)	//从指定地址读n个字节
//日志用驱动,读写均在第二块芯片
//=====================================
void Rd_n_Byte_Log(u32 addr, u16 size, u8* data)		   //从指定地址读n个字节
{ 
	u8 temp = 0;
	u32 addr_new = 0;
	u8 i;
	addr_new = addr + 0x40000;
        chip_Select(&addr_new);//芯片选择
	for(i = 0; i < size; i++)
	{
		SPI_EEPROM_ReadOut(addr + i, &temp);
		data[i] = temp;
	}
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u8 dat)
//=================================
void Wr_u8_Log(u32 addr,u8 dat)		  //写一个字节数据到指定地址
{   
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
	SPI_EEPROM_Program(addr,1,&dat);
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u16 dat)
//=================================
void Wr_u16_Log(u32 addr,u16 dat)		  //写二个字节数据到指定地址
{
	u8 buf[2] = {0};
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
	
	buf[0] = (u8)((dat & 0xFF00) >> 8);
	buf[1] = (u8)(dat & 0x00FF);

	SPI_EEPROM_Program(addr,2,buf);
}
//================================
//数据库用驱动
//WrMemData(u32 addr,u32 dat)
//=================================
void Wr_u32_Log(u32 addr,u32 dat)		  //写四个字节数据到指定地址
{
	u32 addr_new = 0;
	u8 buf[4] = {0};
	
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//芯片选择
    	
	buf[0] = (u8)((dat & 0xFF000000) >> 24);
	buf[1] = (u8)((dat & 0x00FF0000) >> 16);
	buf[2] = (u8)((dat & 0x0000FF00) >> 8);
	buf[3] = (u8)(dat & 0x000000FF);

	SPI_EEPROM_Program(addr,4,buf);
}

//================================
//日志用驱动，读写均在第二块芯片
//void Wr_n_byte(u32 addr, u32 size, u8* dat)
//=================================
void Wr_n_byte_Log(u32 addr, u32 size, u8* dat)		  //写n个字节数据到指定地址
{       
		u32 addr_new = 0;
		u32 i;
		addr_new = addr + 0x40000;
		chip_Select(&addr_new);//芯片选择
		for(i = 0; i < size; i++)
		SPI_EEPROM_Program(addr + i, 1, &dat[i]);
}

//=====================================
#define Bank1_SRAM1_ADDR  ((uint32_t)0x60000000)
void Wr_u8(uint32_t WriteAddr, uint8_t char_write )
{
    /* Transfer data to the memory */
    *(uint8_t *) (Bank1_SRAM1_ADDR + WriteAddr) = char_write;
}
u8 Rd_u8(uint32_t ReadAddr)
{
	u8 data = 0;
   /* Read a half-word from the memory */
   data= *(__IO uint8_t*) (Bank1_SRAM1_ADDR + ReadAddr);
	return data;
}
