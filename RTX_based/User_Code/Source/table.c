
/* 对象标识 地址宏定义 xxxxxx */
#define OBJ_ADDR_START 			((u32)0x0200)												/* 数据库起始地址     */
#define OBJ_ADDR_SET_FLAG		OBJ_ADDR_START										/* 地址设置与否标志位 */
#define TAB_81_ADDR 				((u32)Object_ADDR_Setted_FLAG + 4)
#define	TAB_82_ADDR					((u32)TAB_81_ADDR + 10 + 1)
#define	TAB_83_ADDR					((u32)TAB_82_ADDR + 10 + 1)
#define	TAB_84_ADDR					((u32)TAB_83_ADDR + 10 + 1)
#define	TAB_85_ADDR					((u32)TAB_84_ADDR + 10 + 1)
#define	TAB_86_ADDR					((u32)TAB_85_ADDR + 100 + 1)
#define	TAB_87_ADDR					((u32)TAB_86_ADDR + 10 + 1)
#define	TAB_88_ADDR					((u32)TAB_87_ADDR + 10 + 1)
#define	TAB_89_ADDR					((u32)TAB_88_ADDR + 10 + 1)
#define	TAB_8A_ADDR					((u32)TAB_89_ADDR + 10 + 1)
#define	TAB_8B_ADDR					((u32)TAB_8A_ADDR + 10 + 1)
#define	TAB_8C_ADDR					((u32)TAB_8B_ADDR + 10 + 1)
#define	TAB_8D_ADDR					((u32)TAB_8C_ADDR + 10 + 1)
#define	TAB_8E_ADDR					((u32)TAB_8D_ADDR + 244 + 1)
#define	TAB_8F_ADDR					((u32)TAB_8E_ADDR + 4612 + 1)
#define	TAB_90_ADDR					((u32)TAB_8F_ADDR + 10 + 1)
#define	TAB_91_ADDR					((u32)TAB_90_ADDR + 10 + 1)
#define	TAB_92_ADDR					((u32)TAB_91_ADDR + 4 + 1)
#define	TAB_93_ADDR					((u32)TAB_92_ADDR + 4 + 1)
#define	TAB_94_ADDR					((u32)TAB_93_ADDR + 10 + 1)
#define	TAB_95_ADDR					((u32)TAB_94_ADDR + 10 + 1)
#define	TAB_96_ADDR					((u32)TAB_95_ADDR + 100 + 1)
#define	TAB_97_ADDR					((u32)TAB_96_ADDR + 16 + 1)
#define	TAB_98_ADDR					((u32)TAB_97_ADDR + 100 + 1)
#define	TAB_99_ADDR					((u32)TAB_98_ADDR + 10 + 1)
#define	TAB_9A_ADDR					((u32)TAB_99_ADDR + 10 + 1)
#define	TAB_9B_ADDR					((u32)TAB_9A_ADDR + 10 + 1)
#define	TAB_9C_ADDR					((u32)TAB_9B_ADDR + 10 + 1)
#define	TAB_9D_ADDR					((u32)TAB_9C_ADDR + 10 + 1)
#define	TAB_9E_ADDR					((u32)TAB_9D_ADDR + 10 + 1)
#define	TAB_9F_ADDR					((u32)TAB_9E_ADDR + 10 + 1)
#define	TAB_A0_ADDR					((u32)TAB_9F_ADDR + 292 + 1)
#define	TAB_A1_ADDR					((u32)TAB_A0_ADDR + 52 + 1)
#define	TAB_A2_ADDR					((u32)TAB_A1_ADDR + 292 + 1)
#define	TAB_A3_ADDR					((u32)TAB_A2_ADDR + 292 + 1)
#define	TAB_A4_ADDR					((u32)TAB_A3_ADDR + 10 + 1)
#define	TAB_A5_ADDR					((u32)TAB_A4_ADDR + 10 + 1)
#define	TAB_A6_ADDR					((u32)TAB_A5_ADDR + 10 + 1)
#define	TAB_A7_ADDR					((u32)TAB_A6_ADDR + 10 + 1)
#define	TAB_A8_ADDR					((u32)TAB_A7_ADDR + 10 + 1)
#define	TAB_A9_ADDR					((u32)TAB_A8_ADDR + 10 + 1)
#define	TAB_AA_ADDR					((u32)TAB_A9_ADDR + 10 + 1)
#define	TAB_AB_ADDR					((u32)TAB_AA_ADDR + 10 + 1)
#define	TAB_AC_ADDR					((u32)TAB_AB_ADDR + 10 + 1)
#define	TAB_AD_ADDR					((u32)TAB_AC_ADDR + 10 + 1)
#define	TAB_AE_ADDR					((u32)TAB_AD_ADDR + 10 + 1)
#define	TAB_AF_ADDR					((u32)TAB_AE_ADDR + 10 + 1)
#define	TAB_B0_ADDR					((u32)TAB_AF_ADDR + 10 + 1)
#define	TAB_B1_ADDR					((u32)TAB_B0_ADDR + 100 + 1)
#define	TAB_B2_ADDR					((u32)TAB_B1_ADDR + 16 + 1)
#define	TAB_B3_ADDR					((u32)TAB_B2_ADDR + 10 + 1)
#define	TAB_B4_ADDR					((u32)TAB_B3_ADDR + 10 + 1)
#define	TAB_B5_ADDR					((u32)TAB_B4_ADDR + 10 + 1)
#define	TAB_B6_ADDR					((u32)TAB_B5_ADDR + 10 + 1)
#define	TAB_B7_ADDR					((u32)TAB_B6_ADDR + 10 + 1)
#define	TAB_B8_ADDR					((u32)TAB_B7_ADDR + 10 + 1)
#define	TAB_B9_ADDR					((u32)TAB_B8_ADDR + 10 + 1)
#define	TAB_BA_ADDR					((u32)TAB_B9_ADDR + 10 + 1)
#define	TAB_BB_ADDR					((u32)TAB_BA_ADDR + 10 + 1)
#define	TAB_BC_ADDR					((u32)TAB_BB_ADDR + 10 + 1)
#define	TAB_BD_ADDR					((u32)TAB_BC_ADDR + 10 + 1)
#define	TAB_BE_ADDR					((u32)TAB_BD_ADDR + 10 + 1)
#define	TAB_BF_ADDR					((u32)TAB_BE_ADDR + 10 + 1)
#define	TAB_C0_ADDR					((u32)TAB_BF_ADDR + 10 + 1)
#define	TAB_C1_ADDR					((u32)TAB_C0_ADDR + 196 + 1)
#define	TAB_C2_ADDR					((u32)TAB_C1_ADDR + 1540 + 1)
#define	TAB_C3_ADDR					((u32)TAB_C2_ADDR + 10 + 1)
#define	TAB_C4_ADDR					((u32)TAB_C3_ADDR + 10 + 1)
#define	TAB_C5_ADDR					((u32)TAB_C4_ADDR + 10 + 1)
#define	TAB_C6_ADDR					((u32)TAB_C5_ADDR + 10 + 1)
#define	TAB_C7_ADDR					((u32)TAB_C6_ADDR + 10 + 1)
#define	TAB_C8_ADDR					((u32)TAB_C7_ADDR + 10 + 1)
#define	TAB_C9_ADDR					((u32)TAB_C8_ADDR + 52 + 1)
#define	TAB_CA_ADDR					((u32)TAB_C9_ADDR + 10 + 1)
#define	TAB_CB_ADDR					((u32)TAB_CA_ADDR + 1348 + 1)
#define	TAB_CC_ADDR					((u32)TAB_CB_ADDR + 100 + 1)
#define	TAB_CD_ADDR					((u32)TAB_CC_ADDR + 100 + 1)
#define	TAB_CE_ADDR					((u32)TAB_CD_ADDR + 10 + 1)
#define	TAB_CF_ADDR					((u32)TAB_CE_ADDR + 10 + 1)
#define	TAB_D0_ADDR					((u32)TAB_CF_ADDR + 10 + 1)
#define OBJ_ADDR_END 				((u32)TAB_D0_ADDR + 106 + 1)

/* 表对象属性结构体 xxxxxx */
typedef struct 
{
	u8 	Object_ID;			  		/* 对象标识					*/
	u32	Object_Addr;					/* 地址							*/
	u16	Line_Number;					/* 支持行数					*/
	u8	Subobject_Number;			/* 子对象数					*/
	u8	Line_Byte_Max;				/* 每行字节数				*/
	u8  Line_Byte_Number;		  /* 每行实际字节数		*/
	u32 Object_Number;				/* 对象表字节数			*/
}Object_Attribute;


/* 参数表  xxxxxx */
Object_Attribute  Index_Table[] = 
{
/* 																					 B代表bytes：字节数			*/
/* 		0					1							2				3				4 			5			6		    */
/* 对象标识;  地址     ;   行数;子对象数;每行B;实际行B;对象表B;   */
  {	0X81	,	TAB_81_ADDR	,			1	,			1	,			2	,			4	,		 7	},
  {	0X82	,	TAB_82_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X83	,	TAB_83_ADDR	,			1	,			1	,			2	,			4	,		 7	},
  {	0X84	,	TAB_84_ADDR	,			1	,			1	,			2	,			4	,		 7	},
  {	0X85	,	TAB_85_ADDR	,		 16 ,		 10 ,		102 ,		104	,	1667	},
  {	0X86	,	TAB_86_ADDR	,			1	,			1	,			4	,			6	,		 9	},
  {	0X87	,	TAB_87_ADDR	,			1	,			1	,			4	,			6	,		 9	},
  {	0X88	,	TAB_88_ADDR	,			1	,			1	,			4	,			6	,		 9	},
  {	0X89	,	TAB_89_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X8A	,	TAB_8A_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X8B	,	TAB_8B_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X8C	,	TAB_8C_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X8D	,	TAB_8D_ADDR	,		 40 ,			5	,			9	,		 11	,	 443	},
  {	0X8E	,	TAB_8E_ADDR	,		768	,			8	,			8	,		 10	,	7683	},
  {	0X8F	,	TAB_8F_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X90	,	TAB_90_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X91	,	TAB_91_ADDR	,		 64 ,			5	,			8	,		 10	,	 643	},
  {	0X92	,	TAB_92_ADDR	,	16320	,		  4	,		 10	,		 12	,	195843},
  {	0X93	,	TAB_93_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X94	,	TAB_94_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X95	,	TAB_95_ADDR	,		 16 ,		 12 ,		 12	,		 14	,	 227	},
  {	0X96	,	TAB_96_ADDR	,			2	,			4	,			4	,			6	,		15	},
  {	0X97	,	TAB_97_ADDR	,		 16 ,			3	,			3	,			5	,		83	},
  {	0X98	,	TAB_98_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X99	,	TAB_99_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9A	,	TAB_9A_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9B	,	TAB_9B_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9C	,	TAB_9C_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9D	,	TAB_9D_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9E	,	TAB_9E_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0X9F	,	TAB_9F_ADDR	,		 48 ,			8	,			9	,		 11	,	 531	},
  {	0XA0	,	TAB_A0_ADDR	,		  8 ,			3	,			3	,			5	,		43	},
  {	0XA1	,	TAB_A1_ADDR	,		 48 ,			7	,			7	,			9	,	 435	},
  {	0XA2	,	TAB_A2_ADDR	,		 48 ,			3	,			3	,			5	,	 243	},
  {	0XA3	,	TAB_A3_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA4	,	TAB_A4_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA5	,	TAB_A5_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA6	,	TAB_A6_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA7	,	TAB_A7_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA8	,	TAB_A8_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XA9	,	TAB_A9_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XAA	,	TAB_AA_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XAB	,	TAB_AB_ADDR ,			1	,			4	,			1	,			3	,		 6	},
  {	0XAC	,	TAB_AC_ADDR ,			1	,			1	,			4	,			6	,		 9	},
  {	0XAD	,	TAB_AD_ADDR ,			1	,			4	,			4	,			6	,		 9	},
  {	0XAE	,	TAB_AE_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XAF	,	TAB_AF_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB0	,	TAB_B0_ADDR	,		 16 ,			4	,			4	,			6	,		99	},
  {	0XB1	,	TAB_B1_ADDR	,			2	,			4	,			4	,			6	,		15	},
  {	0XB2	,	TAB_B2_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB3	,	TAB_B3_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB4	,	TAB_B4_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB5	,	TAB_B5_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB6	,	TAB_B6_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB7	,	TAB_B7_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB8	,	TAB_B8_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XB9	,	TAB_B9_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XBA	,	TAB_BA_ADDR ,			1	,			1	,			1	,			3	,		 6	},
  {	0XBB	,	TAB_BB_ADDR ,			1	,			1	,			1	,			3	,		 6	},
  {	0XBC	,	TAB_BC_ADDR ,			1	,			1	,			1	,			3	,		 6	},
  {	0XBD	,	TAB_BD_ADDR ,			1	,			1	,		 14 ,		 16	,		19	},
  {	0XBE	,	TAB_BE_ADDR ,			1	,			1	,		 16 ,		 18	,		21	},
  {	0XBF	,	TAB_BF_ADDR ,			1	,			1	,		 16 ,		 18	,		21	},
  {	0XC0	,	TAB_C0_ADDR	,		 32	,			5	,			5	,			7	,	 227	},
  {	0XC1	,	TAB_C1_ADDR	,		256	,			7	,			8	,		 10	,	2563	},
  {	0XC2	,	TAB_C2_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XC3	,	TAB_C3_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XC4	,	TAB_C4_ADDR	,			1	,			1	,			2	,			4	,		 7	},
  {	0XC5	,	TAB_C5_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XC6	,	TAB_C6_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XC7	,	TAB_C7_ADDR	,			1	,			1	,			1	,			3	,		 6	},
  {	0XC8	,	TAB_C8_ADDR	,		  8 ,			1	,		 39 ,		 41	,	 331	},
  {	0XC9	,	TAB_C9_ADDR	,			1	,			4	,			4	,			6	,		 9	},
  {	0XCA	,	TAB_CA_ADDR ,		224	,			4	,			2	,			4	,	 899	},
  {	0XCB	,	TAB_CB_ADDR ,		 16 ,			3	,			2	,			4	,		67	},
  {	0XCC	,	TAB_CC_ADDR ,		 16 ,			4	,			2	,			4	,		67	},
  {	0XCD	,	TAB_CD_ADDR ,			1	,		 10 ,		 12 ,		 14	,		17	},
  {	0XCE	,	TAB_CE_ADDR ,			1	,			1	,			2	,			4	,		 7	},
  {	0XCF	,	TAB_CF_ADDR ,			1	,			1	,			4	,			6	,		 9	},
  {	0XD0	,	TAB_D0_ADDR	,		 17 ,			4	,			2	,			4	,		71	},
};

/* 子对象表 */
vu8 Adjust_Table[12][11] = 
{
	/*ID   	1		2		 3		 4		 5		 6		 7		8		 9		10     */ 
	{0X85,	1	,	1	,	 24 ,	 1	,	24	,	 1	,	24	,	1	,	24	,	 1	},	/* 0X85 10个子对象 */
	{0X8D,	1	,	2	,		1	,	 4	,	 1	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0X8D  5个子对象 */
	{0X91,	1	,	4	,		1	,	 1	,	 1	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0X91  5个子对象 */
	{0X92,	1	,	1	,		4	,	 4	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0X92  4个子对象 */
	{0X97,	1	,	2	,		0	,	 0	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0X97  2个子对象 */
	{0X9F,	1	,	1	,		1	,	 1	,	 1	,	 1	,	 2	,	1	,	 0	,	 0	},	/* 0X9F  8个子对象 */
	{0XC1,	1	,	1	,		2	,	 1	,	 1	,	 1	,	 1	,	0	,	 0	,	 0	},	/* 0XC1  7个子对象 */
	{0XC8,	1	,	1	,		1	,	16	,	 1	,	16	,	 1	,	1	,	 1	,	 0	},	/* 0XC8  9个子对象 */
	{0XCA,	1	,	1	,		2	,	 1	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0XCA  4个子对象 */
	{0XCC,	1	,	2	,		2	,	 2	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	},	/* 0XCC  4个子对象 */
	{0XCD,	1	,	4	,		4	,	 4	,	 4	,	 4	,	 2	,	6	,	12	,	12	},	/* 0XCD 10个子对象 */
	{0XD0,	1	,	2	,		2	,	 2	,	 0	,	 0	,	 0	,	0	,	 0	,	 0	} 	/* 0XD0  4个子对象 */
};

/* 单索引表  
   0X85||0X8D||0X91||0X92||0X95||0X96||0X97||0X9F||0XA0
||0XA1||0XA2||0XB0||0XB1||0XC0||0XC8||0XC9 */

/* 双索引表 
	 0X8E||0XC1 */
/* 双索引行数 == (索引1当前值-1)*索引2最大值 + 索引2当前值 */

/* 单对象
   (( >0X80) && ( <0XCA))的其他表 */




/* 实时处理数据时 不写入数据库的表 */
/* 0X83||0X84||0X86||0X87||0X88||0XA0||0XA1||0XB1||0XB5
||0XB6||0XB7||0XB8||0XB9||0XBE||0XBF||0XAA */

/* 写入报警日志的数据 */
/* 0X97||0XA2||0XA5||0XA6||0XA7||0XA8||0XA9 */

/* 参数设置表  xxxxxx */
/* 0X82||0X89||0X8A||0X8B||0X8F||0X90||0X93||0X94||0X98
||0X99||0XAE||0XAF||0XB2||0XB3||0XB4||0XC6||0XC7 */
vu8 Parameter_Table[][]=
{
	{ 0X82,	16 },
	{ 0X89,	40 },
	{ 0X8A,	16 },
	{ 0X8B,	48 },
	{ 0X8F,	64 },
	{ 0X90,	64 },
	{ 0X93,	16 },
	{ 0X94,	 2 },
	{ 0X98,	48 },
	{ 0X99,	 8 },
	{ 0XAE,	16 },
	{ 0XAF,	 2 },
	{ 0XB2,	32 },
	{ 0XB3,	16 },
	{ 0XB4,	16 },
	{ 0XC6,	 8 },
	{ 0XC7,	 1 }
};

/* 关联表 */
/*  */
