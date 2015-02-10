#include "Include.h"

//芯片选择函数*p为地址
u16 CHIP_SELECT_PIN = GPIO_Pin_8;
void chip_Select(u32 *p)
{
	if(*p<0x40000)	//first chip
	{
		CHIP_SELECT_PIN = GPIO_Pin_8;
		GPIO_SetBits(GPIOE,GPIO_Pin_9);
	}
	else if((*p>=0x40000)&&(*p<0x80000))	//second chip
	{
		CHIP_SELECT_PIN = GPIO_Pin_9;
		*p = *p - 0x40000;
		GPIO_SetBits(GPIOE,GPIO_Pin_8);
	}	
}

//======================================================================
// 函数名称：
// Flash_Read
//			 
// 说明：读FLASH
//======================================================================
void Flash_Read(u32 StartAddress, void* Data)
{ 
    //memcpy(Data, (void*)(StartAddress), 4);
    
    u8 i, data[4] = {0};
    chip_Select(&StartAddress);//芯片选择
    for (i=0; i<4; i++)
    {
        SPI_EEPROM_ReadOut(StartAddress+i, &data[i]);
    }

    *(u32*)Data = 0x00000000 | (u32)data[0] | ((u32)data[1])<<8 | ((u32)data[2])<<16 | ((u32)data[3])<<24;
}

//======================================================================
// 函数名称：
// Flash_Write
//			 
// 说明：写FLASH
//======================================================================
void Flash_Write(u32 StartAddress,u32 Data)
{
    //u32 Address;    
    //Address = StartAddress;
    //FLASH_ProgramWord(Address,Data);

    u8 data[4] = {0};
    u16 i = 0;
    chip_Select(&StartAddress);//芯片选择
    data[0] = (u8)((Data & 0x000000FF)>>0);
    data[1] = (u8)((Data & 0x0000FF00)>>8);
    data[2] = (u8)((Data & 0x00FF0000)>>16);
    data[3] = (u8)((Data & 0xFF000000)>>24);
    
    SPI_EEPROM_Program(StartAddress, 4, data);
    while((SPI_EEPROM_ReadStatusReg()&0x01 == 1) && (i <= 65530))
    {
       i++;
    }     
}

