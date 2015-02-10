#include "Include.h"

extern u8 DB_Read_M(TARGET_DATA *p_Target);  //  debug_lm
extern u8 DB_Write_M(TARGET_DATA *p_Target); //  debug_lm
extern const object_attribute_struct  Object_table[];//  debug_lm
extern u8 DB_Read_Real_Line(TARGET_DATA *p_Target) ; 


#define VALUE_0x82_1B				16
#define VALUE_0x83_2B				((u16) 4 )
#define VALUE_0x84_2B				((u16) 4 )
#define VALUE_0x89_1B				40
#define VALUE_0x8A_1B				16
#define VALUE_0x8B_1B				16
#define VALUE_0x8F_1B				64
#define VALUE_0x90_1B				64
#define VALUE_0x93_1B				16
#define VALUE_0x94_1B				2
#define VALUE_0x98_1B				48
#define VALUE_0x99_1B				8
#define VALUE_0xAE_1B				16
#define VALUE_0xAF_1B				2
#define VALUE_0xB2_1B				32
#define VALUE_0xB3_1B				16
#define VALUE_0xB4_1B				16
#define VALUE_0xC6_1B				8
#define VALUE_0xC7_1B				1



void Init_Db_Value(void)
{
//	u8 Write_Time_Data[7] = {0};

	
//	u8 flag;


	u8 Tab_8D_val1[9] = {1,0xFF,0xFF,0,0xFF,0xFF,0xFF,0xFF,1};
//	u8 Tab_8D_val2[9] = {2,0xFF,0xFF,0,0xFF,0xFF,0xFF,0xFF,1};
//	u8 Tab_8D_val3[9] = {3,0xFF,0xFF,0,0xFF,0xFF,0xFF,0xFF,1};
//	u8 Tab_8D_val4[9] = {4,0xFF,0xFF,0,0xFF,0xFF,0xFF,0xFF,1};
//	u8 Tab_8D_val5[9] = {5,0xFF,0xFF,0,0xFF,0xFF,0xFF,0xFF,1};

// 	u8 Tab_8D_val1[9] = {1,0x02,0x00,0,0x00,0x00,0x20,0x00,1};
// 	u8 Tab_8D_val2[9] = {2,0x02,0x00,0,0x00,0x00,0x00,0x00,2};
// 	u8 Tab_8D_val3[9] = {3,0x02,0x10,0,0x00,0x00,0x00,0x00,3};
// 	u8 Tab_8D_val4[9] = {4,00,00,0,0xFF,0xFF,0xFF,0xFF,4};
// 	u8 Tab_8D_val5[9] = {5,00,00,0,0xFF,0xFF,0xFF,0xFF,5};

// //         
// // 	u8 Tab_8E_val1[8] = {1,1,13,30,0,1,0,0};
// // 	u8 Tab_8E_val2[8] = {2,1,13,40,0,2,0,0};
// // 	u8 Tab_8E_val3[8] = {3,1,13,50,0,27,0,0};
// // 	u8 Tab_8E_val4[8] = {4,1,13,51,0,28,0,0};
// // 	u8 Tab_8E_val5[8] = {5,1,13,52,0,29,0,0};

// // 	u8 Tab_8E_val1[8] = {1,1,13,30,0,1,0,0};
// // 	u8 Tab_8E_val2[8] = {1,2,13,31,0,27,0,0};
// // 	u8 Tab_8E_val3[8] = {1,3,13,33,0,1,0,0};
// // 	u8 Tab_8E_val4[8] = {1,4,13,35,0,28,0,0};
// // 	u8 Tab_8E_val9[8] = {1,5,13,36,0,1,0,0};
	
	
// // //=====================	 (1)
// // // // 	u8 Tab_8E_val1[8] = {1,1,13,30,0,1,0,0};
// // 	u8 Tab_8E_val1[8] = {1,1,13,30,0,1,0,0};	
// // 	u8 Tab_8E_val2[8] = {1,2,13,31,0,27,0,0};
// // 	u8 Tab_8E_val3[8] = {1,3,13,32,0,1,0,0};
// // 	u8 Tab_8E_val4[8] ={0}; //{1,4,00,04,0,27,0,0};
// // 	u8 Tab_8E_val9[8] ={0}; //{1,5,00,05,0,1,0,0};	
// // //======================	
	
	
	
//=====================		(2)
// // 	u8 Tab_8E_val1[8] = {1,1,13,30,0,1,0,0};
// 	u8 Tab_8E_val1[8] ={1,1,00,00,0,2,0,0};	
// 	u8 Tab_8E_val2[8] ={1,2,00,01,0,2,0,0};
// 	u8 Tab_8E_val3[8] ={1,3,00,02,0,2,0,0};
//  	u8 Tab_8E_val4[8] ={1,4,13,30,0,2,0,0};
//  	u8 Tab_8E_val9[8] ={1,5,13,30,0,2,0,0};	
// //======================		
// 	
// 	
// 	
// 	
// 	
// 	
// 	u8 Tab_8E_val5[8] ={2,1,13,30,0,28,0,0};
// 	u8 Tab_8E_val6[8] ={3,1, 0, 0,0,0,0,0};
// 	u8 Tab_8E_val7[8] ={4,1, 0, 0,0,0,0,0};
// 	u8 Tab_8E_val8[8] ={5,1, 0, 0,0,0,0,0};

	u8 Tab_8E_ind1[] ={1,1,00,01,0,1,0,0};
//											1,2,00,01,0,2,0,0,
//											1,3,00,02,0,2,0,0,
//											1,4,13,30,0,2,0,0,
//											1,5,13,30,0,2,0,0};	

	
	
//	u8 Tab_8E_ind2[8] ={2,1,13,30,0,28,0,0};
//	u8 Tab_8E_ind3[8] ={3,1, 0, 0,0,0,0,0};
//	u8 Tab_8E_ind4[8] ={4,1, 0, 0,0,0,0,0};
//	u8 Tab_8E_ind5[8] ={5,1, 0, 0,0,0,0,0};	
        
// // // // 	u8 Tab_A3_val = 6;
// // // // 	u8 Tab_A4_val = 5;
        
// // // // 	u8 Tab_C0_val1[5] = {1,57,10,1,1};
//	u8 Tab_C0_val2[5] = {2,132,10,1,2};
//	u8 Tab_C0_val3[5] = {3,84,10,1,2};
//	u8 Tab_C0_val4[5] = {4,48,10,1,1};
//	u8 Tab_C0_val5[5] = {5,84,10,1,2};
//	u8 Tab_C0_val6[5] = {6,84,10,1,2};
//	u8 Tab_C0_val7[5] = {7,48,10,1,1};
//	u8 Tab_C0_val8[5] = {8,84,10,1,2};
//	u8 Tab_C0_val9[5] = {9,84,10,1,2};
//	u8 Tab_C0_val10[5] = {10,48,10,1,1};
//	u8 Tab_C0_val11[5] = {11,84,10,1,2};
//	u8 Tab_C0_val12[5] = {12,84,10,1,2};
//	u8 Tab_C0_val13[5] = {13,48,10,1,1};
//	u8 Tab_C0_val14[5] = {14,84,10,1,2};
//	u8 Tab_C0_val15[5] = {15,84,10,1,2};
//	u8 Tab_C0_val16[5] = {16,84,10,1,2};
//	u8 Tab_C0_val17[5] = {17,84,10,1,2};
//	u8 Tab_C0_val18[5] = {18,48,10,1,1};
//	u8 Tab_C0_val19[5] = {19,84,10,1,2};
//	u8 Tab_C0_val20[5] = {20,84,10,1,2};
//	u8 Tab_C0_val21[5] = {21,84,10,1,2};
//	u8 Tab_C0_val22[5] = {22,84,10,1,2};
//	u8 Tab_C0_val23[5] = {23,48,10,1,1};
//	u8 Tab_C0_val24[5] = {24,84,10,1,2};
//	u8 Tab_C0_val25[5] = {25,84,10,1,2};
//	u8 Tab_C0_val26[5] = {26,84,10,1,2};
//	u8 Tab_C0_val27[5] = {27,2,0,0,4};//黄闪	暂定
//	u8 Tab_C0_val28[5] = {28,2,0,0,5};//全红	暂定
//	u8 Tab_C0_val29[5] = {29,2,0,0,6};//灭灯
//	u8 Tab_C0_val30[5] = {30,2,0,0,6};
//	u8 Tab_C0_val31[5] = {31,2,3,0,5};
//	u8 Tab_C0_val32[5] = {32,2,0,3,6};
	
	
// // // // 	
// // // // 	u8 Tab_C1_index1_val1[] = {1,1,0x00,0x03,30,4,4,0x60, 
// // // // 														1,2,0x00,0x30,15,4,4,0x60};
	
//	u8 Tab_C1_index1_val2[] = {2,1,0x06,0x06,24,9,3,0x60, 
//														 2,2,0x09,0x09,24,9,3,0x60,
//														 2,3,0x60,0x60,24,9,3,0x60, 
//														 2,4,0x90,0x90,24,9,3,0x60};
//        
//	u8 Tab_C1_val1[8] = {1,1,0x06,0x06,6,3,3,0x60};   
//	u8 Tab_C1_val2[8] = {1,2,0x01,0x01,6,3,3,0x60};
//	u8 Tab_C1_val3[8] = {1,3,0x60,0x60,6,3,3,0x60};
//	u8 Tab_C1_val4[8] = {1,4,0x10,0x10,6,3,3,0x60};
// 	u8 Tab_C1_val5[8] = {1,5,0x10,0x10,6,3,3,0};
// 	u8 Tab_C1_val6[8] = {1,6,0x40,0x40,6,3,3,0};
	
//	u8 Tab_C1_val7[8] = {2,1,0x06,0x06,24,6,3,0x60};
//	u8 Tab_C1_val8[8] = {2,2,0x09,0x09,24,6,3,0x60};
//	u8 Tab_C1_val9[8] = {2,3,0x60,0x60,24,6,3,0x60};
//	u8 Tab_C1_val10[8] = {2,4,0x90,0x90,24,6,3,0x60};
// 	u8 Tab_C1_val11[8] = {2,5,0x01,0x01,12,6,3,0};
// 	u8 Tab_C1_val12[8] = {2,6,0x04,0x04,12,6,3,0};
	
// 	u8 Tab_C1_val9[8] = {3,1,0x02,0x02,8,2,3,0};
// 	u8 Tab_C1_val10[8] = {3,2,0x01,0x01,8,2,3,0};
// 	u8 Tab_C1_val11[8] = {3,3,0x20,0x20,8,2,3,0};
// 	u8 Tab_C1_val12[8] = {3,4,0x10,0x10,8,2,3,0};
// 	u8 Tab_C1_val5[8] = {2,1,0x05,0x05,12,6,3,0};
// 	u8 Tab_C1_val6[8] = {2,2,0x06,0x06,12,6,3,0};
// 	u8 Tab_C1_val7[8] = {2,3,0x50,0x50,12,6,3,0};
// 	u8 Tab_C1_val8[8] = {2,4,0x60,0x60,12,6,3,0};
// 	u8 Tab_C1_val9[8] = {3,1,0x02,0x02,8,2,3,0};
// 	u8 Tab_C1_val10[8] = {3,2,0x01,0x01,8,2,3,0};
// 	u8 Tab_C1_val11[8] = {3,3,0x20,0x20,8,2,3,0};
// 	u8 Tab_C1_val12[8] = {3,4,0x10,0x10,8,2,3,0};

// 	u8 Tab_95_val1[12] = {1,3,2,10,3,20,30,24,5,0x10,0xFF,0};
//	u8 Tab_95_val2[12] = {2,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val3[12] = {3,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val4[12] = {4,3,2,10,3,20,30,24,5,0x40,0xFF,0};
//	u8 Tab_95_val5[12] = {5,3,2,10,3,20,30,24,5,0x10,0xFF,0};
//	u8 Tab_95_val6[12] = {6,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val7[12] = {7,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val8[12] = {8,3,2,10,3,20,30,24,5,0x40,0xFF,0};
//	u8 Tab_95_val9[12] = {9,3,2,10,3,20,30,24,5,0x10,0xFF,0};
//	u8 Tab_95_val10[12] = {10,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val11[12] = {11,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val12[12] = {12,3,2,10,3,20,30,24,5,0x40,0xFF,0};
//	u8 Tab_95_val13[12] = {13,3,2,10,3,20,30,24,5,0x10,0xFF,0};
//	u8 Tab_95_val14[12] = {14,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val15[12] = {15,3,2,10,3,20,30,24,5,0x80,0xFF,0};
//	u8 Tab_95_val16[12] = {16,3,2,10,3,20,30,24,5,0x40,0xFF,0};
        
//	u8 Tab_B7_val = 0;
//	u8 Tab_B8_val = 60;
//	u8 Tab_B9_val = 0;
//	u8 Tab_BA_val = 4;
//	u8 Tab_BE_val[16] = {10,10,10,10,0};
//	u8 Tab_BF_val[16] = {13,13,13,13,0};

//	u8 Tab_C8_val1[39] = {1,2,2,3,1,0};
//	u8 Tab_C8_val2[39] = {2,2,2,7,5,0};
//	u8 Tab_C8_val3[39] = {3,2,2,11,9,0};
//	u8 Tab_C8_val4[39] = {4,2,2,15,13,0};
	
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
	u8 Tab_B0_val[] = { 1,	1,	0,	2,
										  2,	2,	0,	2,
										  3,	0,	0,	2,
										  4,	0,	0,	3,
										  5,	0,	0,	2,
										  6,	0,	0,	2,
										  7,	0,	0,	2,
										  8,	0,	0,	3,
										  9,	0,	0,	2,
										 10, 0,	0,	2,
										 11, 0,	0,	2,
										 12, 0,	0,	3,
										 13, 0,	0,	2,
										 14, 0,	0,	2,
										 15, 0,	0,	2,
										 16, 0,	0,	3};
					
// // // //  u8 Tab_97_val1[3] = {0X01,0x00,0X30};
// // // //  u8 Tab_97_val2[3] = {0X02,0x00,0X30};
// // // //  u8 Tab_97_val5[3] = {0X05,0x00,0X03};
// // // //  u8 Tab_97_val6[3] = {0X06,0x00,0X03};
//-----------------------------------------------------
//83 84 表2bytes 高位在前 低位在后							 
// // // // u8 Tab_83_val[2] = {((u8) ((VALUE_0x83_2B>>8)&0xFF) ) ,((u8)(VALUE_0x83_2B&0xFF)) };
// // // // u8 Tab_84_val[2] = {((u8) ((VALUE_0x84_2B>>8)&0xFF) ) ,((u8)(VALUE_0x84_2B&0xFF)) };
										 
								 										 
					TARGET_DATA 		target;
//	static 	TARGET_DATA 		target_read;
	
	u32 	i=0;
	u32 	write_count=0;
//static 	u32 	lines_count_debug=0;
//static u8   read_temp=0;
//DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD



	IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog

// //------------------------------------------
// // // //  	target.ID							=0x97;
// // // //  	target.count_index		=1;
// // // //  	target.sub_index[0]		=1;
// // // //  	target.sub_index[1]		=0;	
// // // //  	target.sub_target			=0;   //整行
// // // //  	write_count=0;
// // // //  	for(i=0;i<3;i++)
// // // //  	target.sub_buf[write_count++]		=Tab_97_val1[i];

// // // //  	DB_Write_M(&target);
// // // //  	target.ID							=0x97;
// // // //  	target.count_index		=1;
// // // //  	target.sub_index[0]		=2;
// // // //  	target.sub_index[1]		=0;	
// // // //  	target.sub_target			=0;   //整行
// // // //  	write_count=0;
// // // //  	for(i=0;i<3;i++)
// // // //  	target.sub_buf[write_count++]		=Tab_97_val2[i];

// // // //  	DB_Write_M(&target);
	
// // // //  	target.ID							=0x97;
// // // //  	target.count_index		=1;
// // // //  	target.sub_index[0]		=5;
// // // //  	target.sub_index[1]		=0;	
// // // //  	target.sub_target			=0;   //整行
// // // //  	write_count=0;
// // // //  	for(i=0;i<3;i++)
// // // //  	target.sub_buf[write_count++]		=Tab_97_val5[i];

// // // //  	DB_Write_M(&target);
	
// // // //  	target.ID							=0x97;
// // // //  	target.count_index		=1;
// // // //  	target.sub_index[0]		=6;
// // // //  	target.sub_index[1]		=0;	
// // // //  	target.sub_target			=0;   //整行
// // // //  	write_count=0;
// // // //  	for(i=0;i<3;i++)
// // // //  	target.sub_buf[write_count++]		=Tab_97_val6[i];

// // // //  	DB_Write_M(&target);
//------------------------------------------

//----------
//	target.ID							=0x85;
//	target.count_index		=1;
//	target.sub_index[0]		=1;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<8;i++)
//	target.sub_buf[write_count++]		=Tab_C1_val1[i];

//	DB_Write_M(&target);

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//----------
// // // // 	target.ID							=0xC0;
// // // // 	target.count_index		=1;
// // // // 	target.sub_index[0]		=1;
// // // // 	target.sub_index[1]		=0;	
// // // // 	target.sub_target			=0;   //整行
// // // // 	write_count=0;
// // // // 	for(i=0;i<5;i++)
// // // // 	target.sub_buf[write_count++]		=Tab_C0_val1[i];

// // // // 	DB_Write_M(&target);
	
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=1;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行

//		for(i=0;i<5;i++)
//		target.sub_buf[i] = 0;
//	if(DB_Read_M(&target)!=0)
//	{
//		write_count=0;
//		for(i=0;i<5;i++)
//		Tab_C0_val1[i] = target.sub_buf[write_count++];
//	}
////-----------
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=2;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val2[i];

//	DB_Write_M(&target);
////-----------

////----------
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=3;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val3[i];

//	DB_Write_M(&target);
////-----------
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=4;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val4[i];

//	DB_Write_M(&target);
////-----------
////----------
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=5;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val5[i];

//	DB_Write_M(&target);
////-----------

//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=27;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val27[i];

//	DB_Write_M(&target);
////-----------

//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=28;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val28[i];

//	DB_Write_M(&target);
////-----------
//	target.ID							=0xC0;
//	target.count_index		=1;
//	target.sub_index[0]		=29;
//	target.sub_index[1]		=0;	
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<5;i++)
//	target.sub_buf[write_count++]		=Tab_C0_val29[i];

//	DB_Write_M(&target);
//-----------
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

//	target.ID							=0x95;
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val1[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val2[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val3[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val4[i];
//	
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val5[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val6[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val7[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val8[i];
//	
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val9[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val10[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val11[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val12[i];
//	
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val13[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val14[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val15[i];
//	for(i=0;i<12;i++)
//	target.sub_buf[write_count++]		=Tab_95_val16[i];	

//	DB_Write_M(&target);


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

// // //--------------------------------------------
//jjjjjjjjjjjjjjjjjjjj
//jjjjjjjjjjjjjjjjjjjjjjjj	

	target.ID							=0x8D;
	target.count_index		=1;
	target.sub_index[0]		=1;
	target.sub_index[1]		=0;		
	target.sub_target			=0;   //整行
	write_count=0;
	for(i=0;i<9;i++)
	target.sub_buf[write_count++]		=Tab_8D_val1[i];

	DB_Write_M(&target);
	
//	
//	target.ID							=0x8D;
//	target.count_index		=1;
//	target.sub_index[0]		=2;
//	target.sub_index[1]		=0;		
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<9;i++)
//	target.sub_buf[write_count++]		=Tab_8D_val2[i];

//	DB_Write_M(&target);	
//	
//	target.ID							=0x8D;
//	target.count_index		=1;
//	target.sub_index[0]		=3;
//	target.sub_index[1]		=0;		
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<9;i++)
//	target.sub_buf[write_count++]		=Tab_8D_val3[i];

//	DB_Write_M(&target);		
//	
//	target.ID							=0x8D;
//	target.count_index		=1;
//	target.sub_index[0]		=4;
//	target.sub_index[1]		=0;		
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<9;i++)
//	target.sub_buf[write_count++]		=Tab_8D_val4[i];

//	DB_Write_M(&target);		
//	
//	
//	target.ID							=0x8D;
//	target.count_index		=1;
//	target.sub_index[0]		=5;
//	target.sub_index[1]		=0;		
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<9;i++)
//	target.sub_buf[write_count++]		=Tab_8D_val5[i];

//	DB_Write_M(&target);		
// // 

//jjjjjjjjjjjjjjjjjjjj
//jjjjjjjjjjjjjjjjjjjj
// // 	
// // 	



// // // // // //------------------------------------------
// // // // 	target.ID							=0xA3;	//初始化
// // // // 	target.count_index		=0;
// // // // 	target.sub_index[0]		=0;	
// // // // 	target.sub_index[1]		=0;
// // // // 	target.sub_target			=0;   
// // // // 	
// // // // 	write_count=0;
// // // // 	
// // // // 	for(i=0;i<1;i++)
// // // // 		target.sub_buf[write_count++]		=Tab_A3_val;
// // // // 	DB_Write_M(&target);
	
//------------------------------------------
// // // // 	target.ID							=0xA4;	//初始化
// // // // 	target.count_index		=0;
// // // // 	target.sub_index[0]		=0;	
// // // // 	target.sub_index[1]		=0;
// // // // 	target.sub_target			=0;   
// // // // 	
// // // // 	write_count=0;
// // // // 	
// // // // 	for(i=0;i<1;i++)
// // // // 		target.sub_buf[write_count++]		=Tab_A4_val;
// // // // 	DB_Write_M(&target);

// // // // // //--------------------------------------------

//--------------------------------------------
	target.ID							=0xB0;//整表   //初始化
	target.count_index		=0;
	target.sub_index[0]		=0;	
	target.sub_index[1]		=0;
	target.sub_target			=0;
	
	write_count=0;	
	for(i=0;i<64;i++)
		target.sub_buf[write_count++]		=Tab_B0_val[i];
		
	DB_Write_M(&target);
//------------------------------------------
//	target.ID							=0xB7;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=Tab_B7_val;
//	DB_Write_M(&target);
//------------------------------------------
//	target.ID							=0xB8;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=Tab_B8_val;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0xB9;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=Tab_B9_val;
//	DB_Write_M(&target);
//------------------------------------------
//	target.ID							=0xBA;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=Tab_BA_val;
//	DB_Write_M(&target);
//------------------------------------------
//	target.ID							=0xBE;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<16;i++)
//		target.sub_buf[write_count++]		=Tab_BE_val[i];
//	DB_Write_M(&target);

//------------------------------------------
//	target.ID							=0xBF;	//初始化
//		target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 
//	write_count=0;
//	
//	for(i=0;i<16;i++)
//		target.sub_buf[write_count++]		=Tab_BF_val[i];
//	DB_Write_M(&target);

////-------------------------------------------------

//// // //--------------------------------------------

//	target.ID							=0xC8;
//	target.count_index		=1;
//	target.sub_index[0]		=1;
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 	
//	write_count=0;	
//	for(i=0;i<39;i++)
//		target.sub_buf[write_count++]		=Tab_C8_val1[i];
//	DB_Write_M(&target);	
//		
//	target.ID							=0xC8;
//	target.count_index		=1;
//	target.sub_index[0]		=2;
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 	
//	write_count=0;	
//	for(i=0;i<39;i++)
//		target.sub_buf[write_count++]		=Tab_C8_val2[i];
//	DB_Write_M(&target);	

//	target.ID							=0xC8;
//	target.count_index		=1;
//	target.sub_index[0]		=3;
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 	
//	write_count=0;	
//	for(i=0;i<39;i++)
//		target.sub_buf[write_count++]		=Tab_C8_val3[i];
//	DB_Write_M(&target);	
//	
//	target.ID							=0xC8;
//	target.count_index		=1;
//	target.sub_index[0]		=4;
//	target.sub_index[1]		=0;
//	target.sub_target			=0; 	
//	write_count=0;	
//	for(i=0;i<39;i++)
//		target.sub_buf[write_count++]		=Tab_C8_val4[i];
//	DB_Write_M(&target);	

// // //------------------------------------------------------

// // // // // 	target.ID							=0xC1;
// // // // // 	target.count_index		=2;
// // // // // 	target.sub_index[0]		=1;
// // // // // 	target.sub_index[1]		=2;
// // // // // 	target.sub_target			=0;   //整行
// // // // // 	write_count=0;
// // // // // 	for(i=0;i<16;i++)
// // // // // 	target.sub_buf[write_count++]		=Tab_C1_index1_val1[i];

// // // // // 	DB_Write_M(&target);
	
//		target.ID							=0xC1;
//	target.count_index		=2;
//	target.sub_index[0]		=2;
//	target.sub_index[1]		=4;
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<32;i++)
//	target.sub_buf[write_count++]		=Tab_C1_index1_val2[i];

//	DB_Write_M(&target);
//--------------
					 
// // 	target.ID							=0xC1;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_C1_val1[i];

// // 	DB_Write_M(&target);
// // 	
// // 	
// // 	target.ID							=0xC1;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=2;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_C1_val2[i];

// // 	DB_Write_M(&target);	
// // 	
// // 	target.ID							=0xC1;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=3;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_C1_val3[i];

// // 	DB_Write_M(&target);		
// // 	
// // 	
// // 	target.ID							=0xC1;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=4;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_C1_val4[i];

// // 	DB_Write_M(&target);


// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=1;
// 	target.sub_index[1]		=5;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val5[i];

// 	DB_Write_M(&target);
	
	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=1;
// 	target.sub_index[1]		=6;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val6[i];

// 	DB_Write_M(&target);		

// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=1;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val7[i];

// 	DB_Write_M(&target);
// 	
// 	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=2;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val8[i];

// 	DB_Write_M(&target);	
// 	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=3;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val9[i];

// 	DB_Write_M(&target);		
// 	
// 	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=4;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val10[i];

// 	DB_Write_M(&target);
	
	
	
	
// 		target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=5;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val11[i];

// 	DB_Write_M(&target);
// 	
// 	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=2;
// 	target.sub_index[1]		=6;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val12[i];

// 	DB_Write_M(&target);	
	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=3;
// 	target.sub_index[1]		=3;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val11[i];

// 	DB_Write_M(&target);		
// 	
// 	
// 	target.ID							=0xC1;
// 	target.count_index		=2;
// 	target.sub_index[0]		=3;
// 	target.sub_index[1]		=4;
// 	target.sub_target			=0;   //整行
// 	write_count=0;
// 	for(i=0;i<8;i++)
// 	target.sub_buf[write_count++]		=Tab_C1_val12[i];

// 	DB_Write_M(&target);
// 	


 //----------------------------------------
//DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD 

	target.ID							=0x8E;
	target.count_index		=2;
	target.sub_index[0]		=1;
	target.sub_index[1]		=1;
	target.sub_target			=0;   //整行
	write_count=0;
	for(i=0;i<8;i++)
	target.sub_buf[write_count++]		=Tab_8E_ind1[i];

	DB_Write_M(&target);				
		
					
//	target.ID							=0x8E;
//	target.count_index		=2;
//	target.sub_index[0]		=2;
//	target.sub_index[1]		=1;
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<8;i++)
//	target.sub_buf[write_count++]		=Tab_8E_ind2[i];

//	DB_Write_M(&target);	
//					
//	target.ID							=0x8E;
//	target.count_index		=2;
//	target.sub_index[0]		=3;
//	target.sub_index[1]		=1;
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<8;i++)
//	target.sub_buf[write_count++]		=Tab_8E_ind3[i];
//	
//	DB_Write_M(&target);
//	
//	target.ID							=0x8E;
//	target.count_index		=2;
//	target.sub_index[0]		=4;
//	target.sub_index[1]		=1;
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<8;i++)
//	target.sub_buf[write_count++]		=Tab_8E_ind4[i];

//	DB_Write_M(&target);	

//	target.ID							=0x8E;
//	target.count_index		=2;
//	target.sub_index[0]		=5;
//	target.sub_index[1]		=1;
//	target.sub_target			=0;   //整行
//	write_count=0;
//	for(i=0;i<8;i++)
//	target.sub_buf[write_count++]		=Tab_8E_ind5[i];
//	
//	DB_Write_M(&target);
	
// // // // // //--------------------------------------------	
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val1[i];

// // 	DB_Write_M(&target);				
// // 		
// // 					
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=2;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val2[i];

// // 	DB_Write_M(&target);	
// // 					
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=3;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val3[i];
// // 	
// // 	DB_Write_M(&target);
// // 	
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=4;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val4[i];

// // 	DB_Write_M(&target);	

// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=1;
// // 	target.sub_index[1]		=5;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val9[i];
// // 	

// // 	DB_Write_M(&target);	
// // 	
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=2;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val5[i];

// // 	DB_Write_M(&target);	


// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=3;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val6[i];

// // 	DB_Write_M(&target);	

// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=4;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val7[i];

// // 	DB_Write_M(&target);		
// // 					
// // 	target.ID							=0x8E;
// // 	target.count_index		=2;
// // 	target.sub_index[0]		=5;
// // 	target.sub_index[1]		=1;
// // 	target.sub_target			=0;   //整行
// // 	write_count=0;
// // 	for(i=0;i<8;i++)
// // 	target.sub_buf[write_count++]		=Tab_8E_val8[i];

// // 	DB_Write_M(&target);		
	
	

//lllllllllllllllllllllllllllllll
















//// // //------------------------------------------
//	target.ID							=0x83;	//初始化  单对象 2个字节
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<2;i++)
//		target.sub_buf[write_count++]		=Tab_83_val[i];
//	DB_Write_M(&target);
//// // //------------------------------------------
//	target.ID							=0x84;	//初始化  单对象 2个字节
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<2;i++)
//		target.sub_buf[write_count++]		=Tab_84_val[i];
//	DB_Write_M(&target);
//// // //------------------------------------------



////------------------------------------------
//	target.ID							=0x82;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x82_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x89;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x89_1B;
//	DB_Write_M(&target);
////------------------------------------------


//	target.ID							=0x8A;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x8A_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x8B;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x8B_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x8F;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x8F_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x90;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x90_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x93;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x93_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x94;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x94_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x98;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x98_1B;
//	DB_Write_M(&target);
////------------------------------------------
//	target.ID							=0x99;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0x99_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xAE;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xAE_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xAF;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xAF_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xB2;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xB2_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xB3;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xB3_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xB4;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xB4_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xC6;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xC6_1B;
//	DB_Write_M(&target);
////------------------------------------------
//target.ID							=0xC7;	//初始化
//	target.count_index		=0;
//	target.sub_index[0]		=0;	
//	target.sub_index[1]		=0;
//	target.sub_target			=0;   
//	
//	write_count=0;
//	
//	for(i=0;i<1;i++)
//		target.sub_buf[write_count++]		=VALUE_0xC7_1B;
//	DB_Write_M(&target);
//------------------------------------------











// //=================================================
//--------------------------
// // 	target_read.ID							=0xC0;
// // 	target_read.count_index			=0;
// // 	target_read.sub_index[0]		=0;
// // 	target_read.sub_index[1]		=0;
// // 	target_read.sub_target			=0;   //整行
// // lines_count_debug	 =DB_Read_Real_Line(&target_read);
// // read_temp=	DB_Read_M(&target_read);
// //--------------------------



// // // // //=================================================
// // //--------------------------
// // 	target_read.ID							=0x92;
// // 	target_read.count_index			=2;
// // 	target_read.sub_index[0]		=Opera_Warn;
// // 	target_read.sub_index[1]		=1;
// // 	target_read.sub_target			=0;   //整行
// // //	 DB_Read_Real_Line(&target_read);
// // read_temp=	DB_Read_M(&target_read);
// // // //--------------------------

// // //--------------------------
// // 	target_read.ID							=0x92;
// // 	target_read.count_index			=2;
// // 	target_read.sub_index[0]		=Ctrl_Mode;
// // 	target_read.sub_index[1]		=1;
// // 	target_read.sub_target			=0;   //整行
// // //	 DB_Read_Real_Line(&target_read);
// // read_temp=	DB_Read_M(&target_read);
// // // //--------------------------


// // //--------------------------
// // 	target_read.ID							=0x92;
// // 	target_read.count_index			=2;
// // 	target_read.sub_index[0]		=Yellow_Type;
// // 	target_read.sub_index[1]		=1;
// // 	target_read.sub_target			=0;   //整行
// // //	 DB_Read_Real_Line(&target_read);
// // read_temp=	DB_Read_M(&target_read);
// // // //--------------------------

// // //=================================================



// // 	Write_Time_Data[0] = 0x02;
// // 	Write_Time_Data[1] = 0x30;
// // 	Write_Time_Data[2] = 0x13;
// // 	Write_Time_Data[3] = 0x03;
// // 	Write_Time_Data[4] = 0x21;
// // 	Write_Time_Data[5] = 0x03;
// // 	Write_Time_Data[6] = 0x12;

// // 	Write_Time_Data[0] = 0x02;
// // 	Write_Time_Data[1] = 0x00;
// // 	Write_Time_Data[2] = 0x00;
// // 	Write_Time_Data[3] = 0x03;
// // 	Write_Time_Data[4] = 0x14;
// // 	Write_Time_Data[5] = 0x02;
// // 	Write_Time_Data[6] = 0x14;



// // 	os_mut_wait(I2c_Mut_lm,0xFFFF);   //  I2C_lm
// // 	I2C_Write_DS3231(Write_Time_Data);
// // 	os_mut_release(I2c_Mut_lm);	

	IWDG_ReloadCounter();   /* 喂狗m&l 	IWDG_LM */    
	GPIO_PinReverse(GPIOG, GPIO_Pin_7);//hard_watchdog
}




void Yellow_Flicker(void)
{


}


// //串口发送校验信息
 void Usart_Send_Check(void)
 {

 }
