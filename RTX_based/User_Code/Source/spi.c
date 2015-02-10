
#include "Include.h"

//======================================================================
// �������ƣ�
// spi_ss(void)
//			 
// ˵����NSS����
//======================================================================

void SPI_EEPROM_ss (u32 ss) 
{
   /* Enable/Disable SPI Chip Select (drive it high or low). */
   
   if (ss == 1)//��������
   {
        GPIO_SetBits(GPIOE, CHIP_SELECT_PIN);
   }
   else
   {
			GPIO_ResetBits(GPIOE, CHIP_SELECT_PIN);
   }
}




//======================================================================
// �������ƣ�
// spi_send(void)
//			 
// ˵��������
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
// �������ƣ�
// ReadStatusReg(void)
//			 
// ˵����
//======================================================================
u8 SPI_EEPROM_ReadStatusReg(void)
 {  
	u8 status=0;

	SPI_EEPROM_ss(0);
	                                 // Ƭѡ���
	SPI_EEPROM_send(EEPROM_RDSR);

	status = SPI_EEPROM_send(EEPROM_RDSR);

	SPI_EEPROM_ss(1);

	return status;
   
 }
//======================================================================
// �������ƣ�
// WriteEnable(void)
//			 
// ˵����
//======================================================================
void SPI_EEPROM_WriteEnable(void)
 { 
     SPI_EEPROM_ss(0);
                                     // Ƭѡ���
     SPI_EEPROM_send(EEPROM_WREN);
 
     SPI_EEPROM_ss(1);   
 }

//======================================================================
// �������ƣ�
// PageProgram(void)
//			 
// ˵�������д��32�ֽ�
//====================================================================== 
void SPI_EEPROM_Program(u32 Addr,u32 size,u8* buf)
{
     u32 i = 0;

     while((SPI_EEPROM_ReadStatusReg()&0x01 == 1) && (i <= 65530))
     {
        i++;
     }     

     SPI_EEPROM_WriteEnable();      //д����
     
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
//���ݿ�������
//WrMemData(u32 addr,u8 dat)
//=================================
void WrMemData(u32 addr,u8 dat)		  //дһ���ֽ����ݵ�ָ����ַ
{   
    chip_Select(&addr);//оƬѡ��
    SPI_EEPROM_Program(addr,1,&dat);
}
//================================
//���ݿ�������
//WrMemData(u32 addr,u8 dat)
//=================================
// void Wr_u8(u32 addr,u8 dat)		  //дһ���ֽ����ݵ�ָ����ַ
// {   
//     chip_Select(&addr);//оƬѡ��
//     SPI_EEPROM_Program(addr,1,&dat);
// }
//================================
//���ݿ�������
//WrMemData(u32 addr,u16 dat)
//=================================
void Wr_u16(u32 addr,u16 dat)		  //д�����ֽ����ݵ�ָ����ַ
{
	u8 buf[2] = {0};
	buf[0] = (u8)((dat & 0xFF00) >> 8);
	buf[1] = (u8)(dat & 0x00FF);
        chip_Select(&addr);//оƬѡ��
	SPI_EEPROM_Program(addr,2,buf);
}
//================================
//���ݿ�������
//WrMemData(u32 addr,u32 dat)
//=================================
void Wr_u32(u32 addr,u32 dat)		  //д�ĸ��ֽ����ݵ�ָ����ַ
{
    	u8 buf[4] = {0};
	buf[0] = (u8)((dat & 0xFF000000) >> 24);
	buf[1] = (u8)((dat & 0x00FF0000) >> 16);
	buf[2] = (u8)((dat & 0x0000FF00) >> 8);
	buf[3] = (u8)(dat & 0x000000FF);
        chip_Select(&addr);//оƬѡ��
	SPI_EEPROM_Program(addr,4,buf);
}

//================================
//���ݿ�������
//void Wr_n_byte(u32 addr, u32 size, u8* dat)
//=================================
void Wr_n_byte(u32 addr, u32 size, u8* dat)		  //дn���ֽ����ݵ�ָ����ַ
{       
		u32 i;
		chip_Select(&addr);//оƬѡ��
		for(i = 0; i < size; i++)
		SPI_EEPROM_Program(addr + i, 1, &dat[i]);
}
//======================================================================
// �������ƣ�
// ReadOut(void)
//			 
// ˵������һ���ֽ�
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
//u8 RdMemData(u32 addr)		   //��ָ����ַ��һ���ֽ�
//���ݿ�������
//=====================================
u8 RdMemData(u32 addr)		   //��ָ����ַ��һ���ֽ�
{ 
  u8 data = 0;
  chip_Select(&addr);//оƬѡ��
  SPI_EEPROM_ReadOut(addr, &data);
  return data;
}
//=====================================
//u8 RdMemData(u32 addr)		   //��ָ����ַ��һ���ֽ�
//���ݿ�������


// u8 Rd_u8(u32 addr)		   //��ָ����ַ��һ���ֽ�
// { 
//   u8 data = 0;
//   chip_Select(&addr);//оƬѡ��
//   SPI_EEPROM_ReadOut(addr, &data);
//   return data;
// }
//=====================================
//u16 RdMemData(u32 addr)		   //��ָ����ַ�������ֽ�
//���ݿ�������
//=====================================
u16 Rd_u16(u32 addr)		   //��ָ����ַ�������ֽ�
{ 
  u16 data = 0;
  u8 data1 = 0,data2  = 0;
  chip_Select(&addr);//оƬѡ��
  data1 = Rd_u8(addr);
  data2 = Rd_u8(addr + 1);
  data = data1;
  data <<= 8;
  data |= data2;
  return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //��ָ����ַ���ĸ��ֽ�
//���ݿ�������
//=====================================
u32 Rd_u32(u32 addr)		   //��ָ����ַ���ĸ��ֽ�
{ 
  u32 data = 0;
  u16 data1 = 0,data2  = 0;
  chip_Select(&addr);//оƬѡ��
  data1 = Rd_u16(addr);
  data2 = Rd_u16(addr + 2);
  data = data1;
  data <<= 16;
  data |= data2;
  return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //��ָ����ַ��n���ֽ�
//���ݿ�������
//=====================================
void Rd_n_Byte(u32 addr, u16 size, u8* data)		   //��ָ����ַ��n���ֽ�
{ 
	u8 temp = 0;
	u8 i;
        chip_Select(&addr);//оƬѡ��
	for(i = 0; i < size; i++)
	{
		SPI_EEPROM_ReadOut(addr + i, &temp);
		data[i] = temp;
	}
}
//=====================================
//u8 RdMemData(u32 addr)		   //��ָ����ַ��һ���ֽ�
//���ݿ�������
//=====================================
u8 Rd_u8_Log(u32 addr)		   //��ָ����ַ��һ���ֽ�
{ 
	u8 data = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
	SPI_EEPROM_ReadOut(addr, &data);
	return data;
}
//=====================================
//u16 RdMemData(u32 addr)		   //��ָ����ַ�������ֽ�
//���ݿ�������
//=====================================
u16 Rd_u16_Log(u32 addr)		   //��ָ����ַ�������ֽ�
{ 
	u16 data = 0;
	u8 data1 = 0,data2  = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
	SPI_EEPROM_ReadOut(addr_new,&data1);
	SPI_EEPROM_ReadOut(addr_new+1,&data2);
	data = data1;
	data <<= 8;
	data |= data2;
	return data;
}
//=====================================
//u32 RdMemData(u32 addr)		   //��ָ����ַ���ĸ��ֽ�
//���ݿ�������
//=====================================
u32 Rd_u32_Log(u32 addr)		   //��ָ����ַ���ĸ��ֽ�
{ 
	u32 data = 0;
	u16 data1 = 0,data2  = 0;
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
	data1 = Rd_u16_Log(addr);
	data2 = Rd_u16_Log(addr + 2);
	data = data1;
	data <<= 16;
	data |= data2;
	return data;
}
//=====================================
//u32 RdMemData(u32 addr)	//��ָ����ַ��n���ֽ�
//��־������,��д���ڵڶ���оƬ
//=====================================
void Rd_n_Byte_Log(u32 addr, u16 size, u8* data)		   //��ָ����ַ��n���ֽ�
{ 
	u8 temp = 0;
	u32 addr_new = 0;
	u8 i;
	addr_new = addr + 0x40000;
        chip_Select(&addr_new);//оƬѡ��
	for(i = 0; i < size; i++)
	{
		SPI_EEPROM_ReadOut(addr + i, &temp);
		data[i] = temp;
	}
}
//================================
//���ݿ�������
//WrMemData(u32 addr,u8 dat)
//=================================
void Wr_u8_Log(u32 addr,u8 dat)		  //дһ���ֽ����ݵ�ָ����ַ
{   
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
	SPI_EEPROM_Program(addr,1,&dat);
}
//================================
//���ݿ�������
//WrMemData(u32 addr,u16 dat)
//=================================
void Wr_u16_Log(u32 addr,u16 dat)		  //д�����ֽ����ݵ�ָ����ַ
{
	u8 buf[2] = {0};
	u32 addr_new = 0;
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
	
	buf[0] = (u8)((dat & 0xFF00) >> 8);
	buf[1] = (u8)(dat & 0x00FF);

	SPI_EEPROM_Program(addr,2,buf);
}
//================================
//���ݿ�������
//WrMemData(u32 addr,u32 dat)
//=================================
void Wr_u32_Log(u32 addr,u32 dat)		  //д�ĸ��ֽ����ݵ�ָ����ַ
{
	u32 addr_new = 0;
	u8 buf[4] = {0};
	
	addr_new = addr + 0x40000;
	chip_Select(&addr_new);//оƬѡ��
    	
	buf[0] = (u8)((dat & 0xFF000000) >> 24);
	buf[1] = (u8)((dat & 0x00FF0000) >> 16);
	buf[2] = (u8)((dat & 0x0000FF00) >> 8);
	buf[3] = (u8)(dat & 0x000000FF);

	SPI_EEPROM_Program(addr,4,buf);
}

//================================
//��־����������д���ڵڶ���оƬ
//void Wr_n_byte(u32 addr, u32 size, u8* dat)
//=================================
void Wr_n_byte_Log(u32 addr, u32 size, u8* dat)		  //дn���ֽ����ݵ�ָ����ַ
{       
		u32 addr_new = 0;
		u32 i;
		addr_new = addr + 0x40000;
		chip_Select(&addr_new);//оƬѡ��
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
