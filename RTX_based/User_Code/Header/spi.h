#ifndef __SPI_H
#define __SPI_H
//8K Byte����
#define EEPROM_READ     (0x03)
#define EEPROM_WRITE    (0x02)
#define EEPROM_WREN     (0x06)
#define EEPROM_WRDI     (0x04)
#define EEPROM_RDSR     (0x05)
#define EEPROM_WRSR     (0x01)

void SPI_EEPROM_ss (u32 ss);
u8   SPI_EEPROM_send (u8 outb);
u8   SPI_EEPROM_ReadStatusReg(void);
void SPI_EEPROM_WriteEnable(void);
void SPI_EEPROM_Program(u32 Addr,u32 size,u8* buf);
void SPI_EEPROM_ReadOut(u32 Addr, u8* buf);
u8 RdMemData(u32 addr);		   //��ָ����ַ��һ���ֽ�
void WrMemData(u32 addr,u8 dat);		  //дһ���ֽ����ݵ�ָ����ַ
void Wr_u8(u32 addr,u8 dat);
void Wr_u16(u32 addr,u16 dat);
void Wr_u32(u32 addr,u32 dat);
void Wr_n_byte(u32 addr, u32 size, u8* dat);		  //дn���ֽ����ݵ�ָ����ַ
u8 Rd_u8(u32 addr);
u16 Rd_u16(u32 addr);
u32 Rd_u32(u32 addr);
void Rd_n_Byte(u32 addr, u16 size, u8* data);
u8 Rd_u8_Log(u32 addr);
u16 Rd_u16_Log(u32 addr);
u32 Rd_u32_Log(u32 addr);
void Rd_n_Byte_Log(u32 addr, u16 size, u8* data);
void Wr_u8_Log(u32 addr,u8 dat);
void Wr_u16_Log(u32 addr,u16 dat);
void Wr_u32_Log(u32 addr,u32 dat);
void Wr_n_byte_Log(u32 addr, u32 size, u8* dat);
#endif
