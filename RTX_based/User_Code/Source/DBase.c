#include "Include.h"


// // //²Á³ýEEPROM
void Db_Erase(void)    //   just  for  debug   by_lm
{

	u32 		i;

	//IWDG_ReloadCounter();   /* Î¹¹·m&l 	IWDG_LM */    

	for(i=0;i < 0x40000-41;i++)
	{
		Wr_u8(i,0);
	}
	

}
