#include <include.h>
#include <stdlib.h>


extern u16 CHIP_SELECT_PIN;
//协议解析
#define SUB_TARGET_BUFSIZE 484
#define ID_MIN	0x81
#define ID_MAX	0xD0

#define  ROAD_MAP_ID 0X70 ////标识 ROAD_MAP_ID 暂用0X70 mm20140509


// extern   u16  data_size[8]; 	
// extern 	u8    Rx_Buffer[8][SUB_TARGET_BUFSIZE];
// extern   u8     Rx_Obj_Count;
// extern   u8     Tx_Obj_Count;//发送对象个数
// extern   u8     ETH_Tx_Count;
// extern   u8     Double_Index_Line[33];//查整表时，双索引表返回的行数

 typedef enum
{
	ERROR_NO = 0,
	ERROR_TOOLONG,   	//1
	ERROR_TYPE,				//2
	ERROR_EXCEED,			//3
	ERROR_TOOSHORT,		//4
	ERROR_OTHER,			//5
	ERROR_NONE,
	ERROR_IGNORE
}ERROR_TYPE_E;


//各表结构体
//////////////////======================
typedef u8	US8;
typedef u32	US32;
typedef u16	US16;

#pragma pack(push,1)
/*
模块表行数参数 id=0x85
102*16 = 1632
*/
typedef struct MODULE_TABLE_S
{
	US8 		index;
	US8 		nodeLen;
	US8 		devNode[24];
	US8 		ManuLen;
	US8		Manufacture[24];
	US8		ModelLen;
	US8		Model[24];
	US8 		VerLen;
	US8 		Version[24];
	US8		ModuleType;
}MODULE_TABLE_t;


/*
时基调度表行数id=0x8D
9*40 = 360B
*/


typedef struct TIME_SCHEDULE_S
{
	US8 	planid;
	US16	month;
	US8	dayWeek;
	US32	dayMonth;
	US8	timeTbId;
}TIME_SCHEDULE_t;

/*
时段表行数id=0x8E
8*768 = 6144B = 6KB
*/
typedef struct TIME_TABLE_S
{
	US8 	timeId;
	US8 	eventId;
	US8 	hour;
	US8 	minute;
	US8	ctlMode;
	US8	schemeId;
	US8	auOutput;
	US8	spOutput;
}TIME_TABLE_t;

/*
事件类型表行数id=0x91
255*260 = 65K
*/
typedef struct EVENT_TYPE_TABLE_S
{
	US8		typeId;
	US32	cleanTime;
	US8		dspLen;
	US8		description[255];
	US8		envCurNo;
}EVENT_TYPE_TABLE_t;


/*
事件日志表行数&  事件流水号id=0x92
10*255*255=640KB
*/
// typedef struct EVENT_LOG_S
// {
// 	US8		envTypeId;
// 	US8		envId;
// 	US32 	detTime;
// 	//US32	envValue;
// 	US8		envValue[8];
// }EVENT_LOG_t;



/*
相位参数行数	id=0x95
16*12=192B
*/
typedef struct PHASE_PARAM_S
{
	US8 		phaseId;		//相位号
	US8 		greenSec;		//行人过街绿灯的秒散
	US8 		cleanTime;	//行人清空时间
	US8		minGreenTime;//最小绿灯时间
	US8 		expandGreen;	//单位绿灯延长时间
	US8		maxGreen_1;	////最大绿灯时问1
	US8		maxGreen_2;	//最大绿灯时问2
	US8 		stadyGreen;		//弹性相位固定绿灯时间
	US8		greenShine;		//绿闪时间
	US8		phaseType;		//相位类型
	US8		phaseFunc;		//相位选项功能
	US8		reserved;			//扩展字段
}PHASE_PARAM_t;


/*
相位输出状态表行数 id=0x96
4*2 = 8B
*/
typedef struct PHASE_OUTPUT_STAT_S
{
	US8		statId;				//相位状态组的行号
	US8		redStat;				//相位的红灯输出状态
	US8		yellowStat;		//相位的黄灯输出状态
	US8		greenStat;			//相位的绿灯输出状态
}PHASE_OUTPUT_STAT_t;


/*
相位冲突袭行数id=0x97
16*3=48B
*/
typedef struct PHASE_CONFLICT_S
{
	US8		index;					//行号
	US16	phase;				//冲突相位	
}PHASE_CONFLICT_t;


/*
车辆检测髓参数定义裹行数 id=0x9F
9*48 = 432B
*/
typedef struct MOTOR_DECTOR_PARAM_S
{
	US8		dectorId;		//检测器号
	US8		reqPhase;		//检测器对应的请求相位
	US8		dectorType;	//车辆检测器类登参数
	US8		dectorDir;		//检测器方向
	US8		validTime;		//检测器请求有效时问
	US8		alterParam;	//	车辆检测器选项参数
	US16	satFlow;			//对应关键车遭的饱和流量
	US8		satOcRatio;	//对应关键车道的饱和占有率
}MOTOR_DECTOR_PARAM_t;

/*
检测器状态裹参数列表id=0xA0
8*3=24
*/
typedef struct DECTOR_STAT_PARAM_S
{
	US8		dectStatId;		//检测器状态表行号
	US8		dectStat;			//检测器状态
	US8		dectAlarm;		//检铡器状态报警
}DECTOR_STAT_PARAM_t;


/*
交通检测器数据衰行数id=0xA1
48*7 = 336B
*/
typedef struct DECTOR_DATA_S
{
	US8		index;				//	检测器表行号
	US8		totalFlow;		//总流量
	US8		largeCar;		//大型车流量
	US8		smallCar;		//小型车流量
	US8		OcRatio;			//占有率
	US8		speed;			//速度
	US8		bobyLenth;	//车身长度
}DECTOR_DATA_t;

/*
车辆检测器告警表行数id=0xA2
48*3 = 144B
*/
typedef struct DECTOR_ALARM_TABLE_S
{
	US8		dectId;					//检测器表行号
	US8		dectAlarmStat;	//检测器报警状态
	US8		ocilAlarmStat;		//感应线圈报警状态
}DECTOR_ALARM_TABLE_t;


/*
通道参数衰行数id=0xB0
16*4=64B
*/
typedef struct CHANNEL_PARAM_S
{
	US8		chnId;					//	1 通道号
	US8		signalSrc;				// 2 通道控制的信号源
	US8		chnStat;					// 3 定 义 Automatic Flash控制模式时的通道状态
	US8		chnCtlType;			// 4 通道控制类型
}CHANNEL_PARAM_t;

/*
通道输出状态衰行数id=0xb1
2*4=8B
*/
typedef struct CHANNEL_OUTPUT_STAT_S
{
	US8		index;						//	行号
	US8		redStat;					//通道的红灯输出状态
	US8		yellowStat;			//通道的黄灯输出状态
	US8		greenStat;				//通道的绿灯输出状态
}CHANNEL_OUTPUT_STAT_t;

/*
配置方案表行数id=0xC0
32*5=160B
*/
typedef struct	CONFIG_SCHEME_S
{
	US8		schemeId;				//方案号
	US8		period;					//周期时长
	US8		phaseDif;				//相位差
	US8		coPhase;				//协调相位
	US8		stageTimeTb;		//对应的阶段配时表号
}CONFIG_SCHEME_t;


/*
阶段配时衰行敛id=0xC1
16*16*8 = 2K
*/
typedef struct STAGE_TIMING_TABLE_S
{
	US8		index;					//	阶段配时方案号
	US8		stageId;					//阶段号
	US16	relPhase;				//放行相位2B
	US8		stGreenTime;		//阶段绿灯时同
	US8		stYellowTime;		//阶段黄灯时间
	US8		stRedTime;			//阶段红灯时间
	US8		stAlterParam;		//阶段选项参数
}STAGE_TIMING_TABLE_t;

/*
跟随相位参数衰行数id=0xC8
8*39 = 312B
*/
typedef struct FOLLOW_PHASE_PARAM_S
{
	US8		phaseId;				//跟随相位表号
	US8		operationType;	//操作类型
	US8		incPhaseLenth;	//包古相位长度
	US8		incPhase[16];		//包含相位
	US8		fixPhaseLenth;		//修正相位长度
	US8		fixPhase[16];		//修正相位
	US8		tailGreen;				//尾部绿灯
	US8		tailYellow;				//尾部黄灯
	US8		tailRed;					//尾部红灯
}FOLLOW_PHASE_PARAM_t;


/*
跟随相位状态组表行数id=0xC9
1*4=4B
*/
typedef struct FOLLOW_PHASE_STAT_S
{
	US8		index;						//跟随相位状态组表号
	US8		redStatFlag;			//红灯输出状态标志
	US8		yellowStatFlag;	//黄灯输出状态标志
	US8		greenStatFlag;		//绿灯输出状态标志
}FOLLOW_PHASE_STAT_t;

/*
环状态表行数 id=0xCA
4*16*5=320B
*/
typedef struct CYCLE_STAT_TABLE_S
{
	US8		cycleId;					//环号 
	US8		phaseId;				//相位号 
	US16	phase;					//并发相位 
	US8		funcOptions;			//功能选项 
}CYCLE_STAT_TABLE_t;

/*
相序表行数id=0xCB
16*3=48B
*/
typedef struct SEQUENCE_TABLE_S
{
	US8 		sequenceId;			//相序号 
	US8		cycleId;					//所属环号 
	US8		phaseId;				//相位号
}SEQUENCE_TABLE_t;


/*
通道电流状态表行数 id=0xCC
16*7=112B
*/
typedef struct CHANNLE_CURRENT_STAT_S
{
	US8		index;				//	行号 
	US16	redCurStat;			//通道红灯电流状态 
	US16	yellowCurStat;		//通道的黄灯电流状态 
	US16	greenCurStat;		//通道的绿灯电流状态 
}CHANNLE_CURRENT_STAT_t;

#pragma pack(pop)
//================================================================================
//校验结构体
typedef struct 
{
	u8 Object_ID;		       //对象标识
	u16 Default_Value;      //最大值
	u8 Subobj_Num;          //子对象号
	u8 Byte_Max;              //所占字节数
        u8 Controlled_ID;       //受控对象标识
}Check_Attribute;
//校验结构体
typedef struct 
{
	u8 Object_ID;		       //对象标识
	u8 Subobj_NO;          //子对象号
	u8 * Line_Max;              //所占字节数
}Var_Tab_Check;

//表对象属性结构体
typedef struct 
{
	u8 	Object_ID;							//对象标识
	u32	Object_Addr;						//地址
	u16	Line_Number;						//支持行数
	u8	Subobject_Number;				//子对象数
	u8	Subobject_Size;					//每个子对象占用字节数
	u8  Index2_Number_Max;			//索引2支持最大行数(用于2维表)
}Object_Attribute;

//协议对象数据结构体
typedef struct Target_Data
{
     u8   ID;                           //操作对象标识 
     u8   count_index;                  //操作索引个数 
     u8   sub_index[3];                 //索引
     u8   sub_target;                   //子对象 
     u8   sub_target_type;              //子对象操作类型 
    u8   sub_buf[SUB_TARGET_BUFSIZE];  //操作数据
}TARGET_DATA;



//协议内容结构体
typedef struct Protocol_Data
{
     u8           count_target;              //操作对象数 
     u8           operation_type;            //操作类型 
     TARGET_DATA  opobj_data[8];       //操作对象数据 
}PROTOCOL_DATA;

// #define RX_BUFSIZE          (200)                      //定义接收缓冲区最大长度
// #define TX_BUFSIZE          (500)                      //定义发送缓冲区最大长度,单日志最大长度


//ETH_struct
typedef struct ETH_data
{
     u8  eth_tx[TX_BUFSIZE];
     u8  eth_rx[RX_BUFSIZE];
     u16 rx_count;                   //接收字节数量
     u16 tx_count;                   //发送字节数量  
}ETH_DATA;
//extern     ETH_DATA		Eth_ControlBlk;

//extern PROTOCOL_DATA	Protocol_ControlBlk;

//==============================================
#define Flag_SubOb_En	'!'
#define Flag_Line_En		'&'
#define Flag_Tab_En		'#'

#define  Write_Error		0
#define  Read_Error		0
#define  Write_Success		1
#define  Read_Success		1

#define  Adjust_Error		0 
#define  Adjust_Success	1

#define  Adjust_Max		240   //校验表支持最大个数

#define Object_Addr_Start 			((u32)0x0200)	//数据库起始地址
#define Object_Addr_Setted_FLAG	Object_Addr_Start	//地址设置与否标志位
#define Tab_81_Addr 				((u32)Object_Addr_Setted_FLAG + 4)
#define	Tab_82_Addr				((u32)Tab_81_Addr + 10 + 1)
#define	Tab_83_Addr				((u32)Tab_82_Addr + 10 + 1)
#define	Tab_84_Addr				((u32)Tab_83_Addr + 10 + 1)
#define	Tab_85_Addr				((u32)Tab_84_Addr + 10 + 1)
#define	Tab_86_Addr				((u32)Tab_85_Addr + 100 + 1)
#define	Tab_87_Addr				((u32)Tab_86_Addr + 10 + 1)
#define	Tab_88_Addr				((u32)Tab_87_Addr + 10 + 1)
#define	Tab_89_Addr				((u32)Tab_88_Addr + 10 + 1)
#define	Tab_8A_Addr				((u32)Tab_89_Addr + 10 + 1)
#define	Tab_8B_Addr				((u32)Tab_8A_Addr + 10 + 1)
#define	Tab_8C_Addr				((u32)Tab_8B_Addr + 10 + 1)
#define	Tab_8D_Addr				((u32)Tab_8C_Addr + 10 + 1)
#define	Tab_8E_Addr				((u32)Tab_8D_Addr + 244 + 1)
#define	Tab_8F_Addr				((u32)Tab_8E_Addr + 4612 + 1)
#define	Tab_90_Addr				((u32)Tab_8F_Addr + 10 + 1)
#define	Tab_91_Addr				((u32)Tab_90_Addr + 10 + 1)
#define	Tab_92_Addr				((u32)Tab_91_Addr + 4 + 1)
#define	Tab_93_Addr				((u32)Tab_92_Addr + 4 + 1)
#define	Tab_94_Addr				((u32)Tab_93_Addr + 10 + 1)
#define	Tab_95_Addr				((u32)Tab_94_Addr + 10 + 1)
#define	Tab_96_Addr				((u32)Tab_95_Addr + 100 + 1)
#define	Tab_97_Addr				((u32)Tab_96_Addr + 16 + 1)
#define	Tab_98_Addr				((u32)Tab_97_Addr + 100 + 1)
#define	Tab_99_Addr				((u32)Tab_98_Addr + 10 + 1)
#define	Tab_9A_Addr				((u32)Tab_99_Addr + 10 + 1)
#define	Tab_9B_Addr				((u32)Tab_9A_Addr + 10 + 1)
#define	Tab_9C_Addr				((u32)Tab_9B_Addr + 10 + 1)
#define	Tab_9D_Addr				((u32)Tab_9C_Addr + 10 + 1)
#define	Tab_9E_Addr				((u32)Tab_9D_Addr + 10 + 1)
#define	Tab_9F_Addr				((u32)Tab_9E_Addr + 10 + 1)
#define	Tab_A0_Addr				((u32)Tab_9F_Addr + 292 + 1)
#define	Tab_A1_Addr				((u32)Tab_A0_Addr + 52 + 1)
#define	Tab_A2_Addr				((u32)Tab_A1_Addr + 292 + 1)
#define	Tab_A3_Addr				((u32)Tab_A2_Addr + 292 + 1)
#define	Tab_A4_Addr				((u32)Tab_A3_Addr + 10 + 1)
#define	Tab_A5_Addr				((u32)Tab_A4_Addr + 10 + 1)
#define	Tab_A6_Addr				((u32)Tab_A5_Addr + 10 + 1)
#define	Tab_A7_Addr				((u32)Tab_A6_Addr + 10 + 1)
#define	Tab_A8_Addr				((u32)Tab_A7_Addr + 10 + 1)
#define	Tab_A9_Addr				((u32)Tab_A8_Addr + 10 + 1)
#define	Tab_AA_Addr				((u32)Tab_A9_Addr + 10 + 1)
#define	Tab_AB_Addr				((u32)Tab_AA_Addr + 10 + 1)
#define	Tab_AC_Addr				((u32)Tab_AB_Addr + 10 + 1)
#define	Tab_AD_Addr				((u32)Tab_AC_Addr + 10 + 1)
#define	Tab_AE_Addr				((u32)Tab_AD_Addr + 10 + 1)
#define	Tab_AF_Addr				((u32)Tab_AE_Addr + 10 + 1)
#define	Tab_B0_Addr				((u32)Tab_AF_Addr + 10 + 1)
#define	Tab_B1_Addr				((u32)Tab_B0_Addr + 100 + 1)
#define	Tab_B2_Addr				((u32)Tab_B1_Addr + 16 + 1)
#define	Tab_B3_Addr				((u32)Tab_B2_Addr + 10 + 1)
#define	Tab_B4_Addr				((u32)Tab_B3_Addr + 10 + 1)
#define	Tab_B5_Addr				((u32)Tab_B4_Addr + 10 + 1)
#define	Tab_B6_Addr				((u32)Tab_B5_Addr + 10 + 1)
#define	Tab_B7_Addr				((u32)Tab_B6_Addr + 10 + 1)
#define	Tab_B8_Addr				((u32)Tab_B7_Addr + 10 + 1)
#define	Tab_B9_Addr				((u32)Tab_B8_Addr + 10 + 1)
#define	Tab_BA_Addr				((u32)Tab_B9_Addr + 10 + 1)
#define	Tab_BB_Addr				((u32)Tab_BA_Addr + 10 + 1)
#define	Tab_BC_Addr				((u32)Tab_BB_Addr + 10 + 1)
#define	Tab_BD_Addr				((u32)Tab_BC_Addr + 10 + 1)
#define	Tab_BE_Addr				((u32)Tab_BD_Addr + 10 + 1)
#define	Tab_BF_Addr				((u32)Tab_BE_Addr + 10 + 1)
#define	Tab_C0_Addr				((u32)Tab_BF_Addr + 10 + 1)
#define	Tab_C1_Addr				((u32)Tab_C0_Addr + 196 + 1)
#define	Tab_C2_Addr				((u32)Tab_C1_Addr + 1540 + 1)
#define	Tab_C3_Addr				((u32)Tab_C2_Addr + 10 + 1)
#define	Tab_C4_Addr				((u32)Tab_C3_Addr + 10 + 1)
#define	Tab_C5_Addr				((u32)Tab_C4_Addr + 10 + 1)
#define	Tab_C6_Addr				((u32)Tab_C5_Addr + 10 + 1)
#define	Tab_C7_Addr				((u32)Tab_C6_Addr + 10 + 1)
#define	Tab_C8_Addr				((u32)Tab_C7_Addr + 10 + 1)
#define	Tab_C9_Addr				((u32)Tab_C8_Addr + 52 + 1)
#define	Tab_CA_Addr				((u32)Tab_C9_Addr + 10 + 1)
#define	Tab_CB_Addr				((u32)Tab_CA_Addr + 1348 + 1)
#define	Tab_CC_Addr				((u32)Tab_CB_Addr + 100 + 1)
#define	Tab_CD_Addr				((u32)Tab_CC_Addr + 100 + 1)
#define	Tab_CE_Addr				((u32)Tab_CD_Addr + 10 + 1)
#define	Tab_CF_Addr				((u32)Tab_CE_Addr + 10 + 1)
#define	Tab_D0_Addr				((u32)Tab_CF_Addr + 10 + 1)
#define 	Object_Addr_End 			((u32)Tab_D0_Addr + 106 + 1)

#define 	Subobject_Addr_start		((u32)Object_Addr_End +10)

//索引表

// extern Object_Attribute  Index_Table[80];
// extern vu8 Adjust_Table[12][11];

// extern Var_Tab_Check Var_Tab[13];
// extern u8 Channel_Num;
// extern u8 Channel_Num_3;
// extern u8 Channel_Num_2;
// extern u8 Channel_Num_8;
// extern u8 Channel_Num_17;
//extern u8 Lock_Obj[19][2];
//extern TIME_TABLE_t	     Table_0x8E_read;



//事件日志缓冲结构体
//extern EVENT_LOG_t Event_Record_Buff[10];
//extern u8 Event_Num[64];
//extern u8 Event_Flag;
//extern u8 Log_Event_Value[4];//事件值

// #define	Var_95_Tab	(*Channel_Num_Link)
// #define	Var_96_Tab	(*Channel_Num_Link)/8
// #define	Var_97_Tab	(*Channel_Num_Link)
// #define	Var_9F_Tab	(*Channel_Num_Link)*3
// #define	Var_A0_Tab	(*Channel_Num_Link)/2
// #define	Var_A1_Tab	(*Channel_Num_Link)*3
// #define	Var_B0_Tab	(*Channel_Num_Link)
// #define	Var_C8_Tab	(*Channel_Num_Link)/2
// #define	Var_C9_Tab	1+(*Channel_Num_Link)/17
// #define	Var_CA_Tab	(*Channel_Num_Link)
// #define	Var_CB_Tab	(*Channel_Num_Link)
// #define	Var_CC_Tab	(*Channel_Num_Link)
// #define	Var_D0_Tab	(*Channel_Num_Link)
void chip_Select(u32 *p);
void Db_Erase(void);
//void Table_Erase(u8 ID_Start, u8 ID_End);
//u32 Subobject_Addr(u8 Object_ID,u16 Line_number);
//u8 Wr_Sub_Object(u8 Object_ID, u8 Line_NO, u8 Sub_NO, u8 size, u8 *data);
//u8 Rd_Sub_Object(u8 Object_ID, u8 Line_NO, u8 Sub_NO, u8 *data);
//void get_subObj(u8 id, u32 addr, u8 subObjId);
// u8 insert_lineRecord(u8 Object_ID, u32 addr, u8 *val);
// u8 insert_subObj(u8 Object_ID, u32 addr, u8 Sub_NO, u8 *data);
// u8 Db_Insert_Line(u8 Object_ID, u16 line, u8 *val);
// u8 Db_Insert_SubObj(u8 Object_ID, u16 line, u8 Subobj, u8 *val);
// u8 Db_Write(u8 Object_ID, u8 Index1, u8 Index2, u8 Sub_Count, u8 *val);
// u8 Db_Get_SubObj(u8 Object_ID, u16 line, u8 Subobj);
// u8 Db_Get_SubObj(u8 Object_ID, u16 line, u8 Subobj);
// u8 Db_Read(u8 Object_ID, u8 Index1, u8 Index2, u8 Sub_Count);
// void get_lineRecord(u8 id, u32 addr);
// u32 update_RecordInfo(u32 addr, u32 addrTmp, u8 curLine);
// u8 get_LineNum(u32 addr);
// u32 get_TableAddr(u8 id, u32 *addr);
// static void writeROMBySection(u32 *address, u8 *val, u16 num);
// static void readROMBySection(u32 *address, u8 *val, u16 num);
// void Adjust_Db_to_Eth_Subobj(u8 Object_ID, u8 *Eth_data, u8 *Db_data,u8 Subobj);
// void Adjust_Eth_to_Db(u8 Object_ID, u8 *Eth_data, u8 *Db_data);
// void  Adjust_Db_to_Eth_Line(u8 Object_ID, u8 *Eth_data, u8 Db_data[][24]);

// //u8 Protocol_Explain_In(u8 *rx_buffer, u16 rx_count);
// u8 Protocol_Explain_Out(u8 *tx_buffer, u16 tx_count);
// u8 Double_Protocol_Explain_Out(u8 *tx_buffer, u16 tx_count);
// u8 Db_Communication(u8 *rx_buffer, u16 rx_count, u8 *tx_buffer, u16 tx_count);
// //u8 Db_Filter(u8 *rx_buffer, u16 rx_count);
// void Db_MemCopy(u8 ID, void *Struct, u8 *data, u8 N);
// //u8 Lock_Object(u8 ID, u8 RorW, u8 *tx_buffer);
// void Set_Answer(u8 *tx_buffer, u16 tx_count);
// //u8 Get_VariableID_Line(u8 ID, u8 Sub_Num);
// u8 Set_Read_Lock_Subobj(u8 ID, u8 Sub_Count, u8 *val);
// u8 Set_Read_Lock_Object(u8 ID, u8 Index1, u8 Index2, u8 Sub_Count, u8 *val);
// u8 Set_Phase_Channel_num(u8 val);
// void Log_Erase(void);
// u8 Write_Log(u8 Log_Num, u8 Word_data[]);
// u8 Read_Log(u8 Log_Num, u8 return_data[]);
// u8 Read_Log_Flow(u8 Log_Num, u8 Flow_Group, u8 return_data[]);
// void Read_Log_Fun(void);
// void Log_Test(void);


//================================================
//================================================

#define   TAB_OK_FLAG  0xcc
#define   LINE_OK_FLAG 0xdd


#define   MES_IPI   			0x21
#define   MES_OPTION_MIN  0x80

#define   ID_MIN          0x81
#define   ID_MAX          0xD0   //目前做到最大为0xD0对象标识

#define   RIGHT   			0x01
#define   WRONG   			0x00
//================================================

/*  行字节数 宏定义 xxxxxx */
#define LINE_81_BYTES 				2
#define	LINE_82_BYTES					1
#define	LINE_83_BYTES					2
#define	LINE_84_BYTES					2
#define	LINE_85_BYTES					102
#define	LINE_86_BYTES					4
#define	LINE_87_BYTES					4
#define	LINE_88_BYTES					4
#define	LINE_89_BYTES					1
#define	LINE_8A_BYTES					1
#define	LINE_8B_BYTES					1
#define	LINE_8C_BYTES					1
#define	LINE_8D_BYTES					9
#define	LINE_8E_BYTES					8
#define	LINE_8F_BYTES					1
#define	LINE_90_BYTES					1
#define	LINE_91_BYTES					8
#define	LINE_92_BYTES					10
#define	LINE_93_BYTES					1
#define	LINE_94_BYTES					1
#define	LINE_95_BYTES					12
#define	LINE_96_BYTES					4
#define	LINE_97_BYTES					3
#define	LINE_98_BYTES					1
#define	LINE_99_BYTES					1
#define	LINE_9A_BYTES					1
#define	LINE_9B_BYTES					1
#define	LINE_9C_BYTES					1
#define	LINE_9D_BYTES					1
#define	LINE_9E_BYTES					1
#define	LINE_9F_BYTES					9
#define	LINE_A0_BYTES					3
#define	LINE_A1_BYTES					7
#define	LINE_A2_BYTES					3
#define	LINE_A3_BYTES					1
#define	LINE_A4_BYTES					1
#define	LINE_A5_BYTES					1
#define	LINE_A6_BYTES					1
#define	LINE_A7_BYTES					1
#define	LINE_A8_BYTES					1
#define	LINE_A9_BYTES					1
#define	LINE_AA_BYTES					1
#define	LINE_AB_BYTES					1
#define	LINE_AC_BYTES					4
#define	LINE_AD_BYTES					4
#define	LINE_AE_BYTES					1
#define	LINE_AF_BYTES					1
#define	LINE_B0_BYTES					4
#define	LINE_B1_BYTES					4
#define	LINE_B2_BYTES					1
#define	LINE_B3_BYTES					1
#define	LINE_B4_BYTES					1
#define	LINE_B5_BYTES					1
#define	LINE_B6_BYTES					1
#define	LINE_B7_BYTES					1
#define	LINE_B8_BYTES					1
#define	LINE_B9_BYTES					1
#define	LINE_BA_BYTES					1
#define	LINE_BB_BYTES					1
#define	LINE_BC_BYTES					1
#define	LINE_BD_BYTES					14
#define	LINE_BE_BYTES					16
#define	LINE_BF_BYTES					16
#define	LINE_C0_BYTES					5
#define	LINE_C1_BYTES					8
#define	LINE_C2_BYTES					1
#define	LINE_C3_BYTES					1
#define	LINE_C4_BYTES					2
#define	LINE_C5_BYTES					1
#define	LINE_C6_BYTES					1
#define	LINE_C7_BYTES					1
#define	LINE_C8_BYTES					39
#define	LINE_C9_BYTES					4
#define	LINE_CA_BYTES					5
#define	LINE_CB_BYTES					3
#define	LINE_CC_BYTES					7
#define	LINE_CD_BYTES					17
#define	LINE_CE_BYTES					7
#define	LINE_CF_BYTES					9
#define	LINE_D0_BYTES					71

/*  整表行数 宏定义 xxxxxx */
#define TAB_81_LINES 					1
#define	TAB_82_LINES					1
#define	TAB_83_LINES					1
#define	TAB_84_LINES					1
#define	TAB_85_LINES					16
#define	TAB_86_LINES					1
#define	TAB_87_LINES					1
#define	TAB_88_LINES					1
#define	TAB_89_LINES					1
#define	TAB_8A_LINES					1
#define	TAB_8B_LINES					1
#define	TAB_8C_LINES					1
#define	TAB_8D_LINES					40
#define	TAB_8E_LINES					768
#define	TAB_8F_LINES					1
#define	TAB_90_LINES					1
#define	TAB_91_LINES					64
#define	TAB_92_LINES					16320 //255 or 16320  ???????
#define	TAB_93_LINES					1
#define	TAB_94_LINES					1
#define	TAB_95_LINES					16
#define	TAB_96_LINES					4
#define	TAB_97_LINES					16 //32 or 16   ??
#define	TAB_98_LINES					1
#define	TAB_99_LINES					1
#define	TAB_9A_LINES					1
#define	TAB_9B_LINES					1
#define	TAB_9C_LINES					1
#define	TAB_9D_LINES					1
#define	TAB_9E_LINES					1
#define	TAB_9F_LINES					48
#define	TAB_A0_LINES					8
#define	TAB_A1_LINES					48
#define	TAB_A2_LINES					48
#define	TAB_A3_LINES					1
#define	TAB_A4_LINES					1
#define	TAB_A5_LINES					1
#define	TAB_A6_LINES					1
#define	TAB_A7_LINES					1
#define	TAB_A8_LINES					1
#define	TAB_A9_LINES					1
#define	TAB_AA_LINES					1
#define	TAB_AB_LINES					1
#define	TAB_AC_LINES					1
#define	TAB_AD_LINES					1
#define	TAB_AE_LINES					1
#define	TAB_AF_LINES					1
#define	TAB_B0_LINES					16
#define	TAB_B1_LINES					2
#define	TAB_B2_LINES					1
#define	TAB_B3_LINES					1
#define	TAB_B4_LINES					1
#define	TAB_B5_LINES					1
#define	TAB_B6_LINES					1
#define	TAB_B7_LINES					1
#define	TAB_B8_LINES					1
#define	TAB_B9_LINES					1
#define	TAB_BA_LINES					1
#define	TAB_BB_LINES					1
#define	TAB_BC_LINES					1
#define	TAB_BD_LINES					1
#define	TAB_BE_LINES					1
#define	TAB_BF_LINES					1
#define	TAB_C0_LINES					32
#define	TAB_C1_LINES					256
#define	TAB_C2_LINES					1
#define	TAB_C3_LINES					1
#define	TAB_C4_LINES					1
#define	TAB_C5_LINES					1
#define	TAB_C6_LINES					1
#define	TAB_C7_LINES					1
#define	TAB_C8_LINES					8
#define	TAB_C9_LINES					1
#define	TAB_CA_LINES					64
#define	TAB_CB_LINES					64
#define	TAB_CC_LINES					16
#define	TAB_CD_LINES					1
#define	TAB_CE_LINES					1
#define	TAB_CF_LINES					1
#define	TAB_D0_LINES					17

/*  整表字节数 宏定义 xxxxxx */
#define TAB_81_BYTES 					((TAB_81_LINES) * (LINE_81_BYTES))
#define	TAB_82_BYTES					((TAB_82_LINES) * (LINE_82_BYTES))
#define	TAB_83_BYTES					((TAB_83_LINES) * (LINE_83_BYTES))
#define	TAB_84_BYTES					((TAB_84_LINES) * (LINE_84_BYTES))
#define	TAB_85_BYTES					((TAB_85_LINES) * (LINE_85_BYTES) + 1)
#define	TAB_86_BYTES					((TAB_86_LINES) * (LINE_86_BYTES))
#define	TAB_87_BYTES					((TAB_87_LINES) * (LINE_87_BYTES))
#define	TAB_88_BYTES					((TAB_88_LINES) * (LINE_88_BYTES))
#define	TAB_89_BYTES					((TAB_89_LINES) * (LINE_89_BYTES))
#define	TAB_8A_BYTES					((TAB_8A_LINES) * (LINE_8A_BYTES))
#define	TAB_8B_BYTES					((TAB_8B_LINES) * (LINE_8B_BYTES))
#define	TAB_8C_BYTES					((TAB_8C_LINES) * (LINE_8C_BYTES))
#define	TAB_8D_BYTES					((TAB_8D_LINES) * (LINE_8D_BYTES) + 1)
#define	TAB_8E_BYTES					((TAB_8E_LINES) * (LINE_8E_BYTES) + 2)
#define	TAB_8F_BYTES					((TAB_8F_LINES) * (LINE_8F_BYTES))
#define	TAB_90_BYTES					((TAB_90_LINES) * (LINE_90_BYTES))
#define	TAB_91_BYTES					((TAB_91_LINES) * (LINE_91_BYTES))
#define	TAB_92_BYTES					((TAB_92_LINES) * (LINE_92_BYTES) + 1)
#define	TAB_93_BYTES					((TAB_93_LINES) * (LINE_93_BYTES))
#define	TAB_94_BYTES					((TAB_94_LINES) * (LINE_94_BYTES))
#define	TAB_95_BYTES					((TAB_95_LINES) * (LINE_95_BYTES) + 1)
#define	TAB_96_BYTES					((TAB_96_LINES) * (LINE_96_BYTES) + 1)
#define	TAB_97_BYTES					((TAB_97_LINES) * (LINE_97_BYTES) + 1)
#define	TAB_98_BYTES					((TAB_98_LINES) * (LINE_98_BYTES))
#define	TAB_99_BYTES					((TAB_99_LINES) * (LINE_99_BYTES))
#define	TAB_9A_BYTES					((TAB_9A_LINES) * (LINE_9A_BYTES))
#define	TAB_9B_BYTES					((TAB_9B_LINES) * (LINE_9B_BYTES))
#define	TAB_9C_BYTES					((TAB_9C_LINES) * (LINE_9C_BYTES))
#define	TAB_9D_BYTES					((TAB_9D_LINES) * (LINE_9D_BYTES))
#define	TAB_9E_BYTES					((TAB_9E_LINES) * (LINE_9E_BYTES))
#define	TAB_9F_BYTES					((TAB_9F_LINES) * (LINE_9F_BYTES) + 1)
#define	TAB_A0_BYTES					((TAB_A0_LINES) * (LINE_A0_BYTES) + 1)
#define	TAB_A1_BYTES					((TAB_A1_LINES) * (LINE_A1_BYTES) + 1)
#define	TAB_A2_BYTES					((TAB_A2_LINES) * (LINE_A2_BYTES) + 1)
#define	TAB_A3_BYTES					((TAB_A3_LINES) * (LINE_A3_BYTES))
#define	TAB_A4_BYTES					((TAB_A4_LINES) * (LINE_A4_BYTES))
#define	TAB_A5_BYTES					((TAB_A5_LINES) * (LINE_A5_BYTES))
#define	TAB_A6_BYTES					((TAB_A6_LINES) * (LINE_A6_BYTES))
#define	TAB_A7_BYTES					((TAB_A7_LINES) * (LINE_A7_BYTES))
#define	TAB_A8_BYTES					((TAB_A8_LINES) * (LINE_A8_BYTES))
#define	TAB_A9_BYTES					((TAB_A9_LINES) * (LINE_A9_BYTES))
#define	TAB_AA_BYTES					((TAB_AA_LINES) * (LINE_AA_BYTES))
#define	TAB_AB_BYTES					((TAB_AB_LINES) * (LINE_AB_BYTES))
#define	TAB_AC_BYTES					((TAB_AC_LINES) * (LINE_AC_BYTES))
#define	TAB_AD_BYTES					((TAB_AD_LINES) * (LINE_AD_BYTES))
#define	TAB_AE_BYTES					((TAB_AE_LINES) * (LINE_AE_BYTES))
#define	TAB_AF_BYTES					((TAB_AF_LINES) * (LINE_AF_BYTES))
#define	TAB_B0_BYTES					((TAB_B0_LINES) * (LINE_B0_BYTES) + 1)
#define	TAB_B1_BYTES					((TAB_B1_LINES) * (LINE_B1_BYTES) + 1)
#define	TAB_B2_BYTES					((TAB_B2_LINES) * (LINE_B2_BYTES))
#define	TAB_B3_BYTES					((TAB_B3_LINES) * (LINE_B3_BYTES))
#define	TAB_B4_BYTES					((TAB_B4_LINES) * (LINE_B4_BYTES))
#define	TAB_B5_BYTES					((TAB_B5_LINES) * (LINE_B5_BYTES))
#define	TAB_B6_BYTES					((TAB_B6_LINES) * (LINE_B6_BYTES))
#define	TAB_B7_BYTES					((TAB_B7_LINES) * (LINE_B7_BYTES))
#define	TAB_B8_BYTES					((TAB_B8_LINES) * (LINE_B8_BYTES))
#define	TAB_B9_BYTES					((TAB_B9_LINES) * (LINE_B9_BYTES))
#define	TAB_BA_BYTES					((TAB_BA_LINES) * (LINE_BA_BYTES))
#define	TAB_BB_BYTES					((TAB_BB_LINES) * (LINE_BB_BYTES))
#define	TAB_BC_BYTES					((TAB_BC_LINES) * (LINE_BC_BYTES))
#define	TAB_BD_BYTES					((TAB_BD_LINES) * (LINE_BD_BYTES))
#define	TAB_BE_BYTES					((TAB_BE_LINES) * (LINE_BE_BYTES))
#define	TAB_BF_BYTES					((TAB_BF_LINES) * (LINE_BF_BYTES))
#define	TAB_C0_BYTES					((TAB_C0_LINES) * (LINE_C0_BYTES) + 1)
#define	TAB_C1_BYTES					((TAB_C1_LINES) * (LINE_C1_BYTES) + 2)
#define	TAB_C2_BYTES					((TAB_C2_LINES) * (LINE_C2_BYTES))
#define	TAB_C3_BYTES					((TAB_C3_LINES) * (LINE_C3_BYTES))
#define	TAB_C4_BYTES					((TAB_C4_LINES) * (LINE_C4_BYTES))
#define	TAB_C5_BYTES					((TAB_C5_LINES) * (LINE_C5_BYTES))
#define	TAB_C6_BYTES					((TAB_C6_LINES) * (LINE_C6_BYTES))
#define	TAB_C7_BYTES					((TAB_C7_LINES) * (LINE_C7_BYTES))
#define	TAB_C8_BYTES					((TAB_C8_LINES) * (LINE_C8_BYTES) + 1)
#define	TAB_C9_BYTES					((TAB_C9_LINES) * (LINE_C9_BYTES) + 1)
#define	TAB_CA_BYTES					((TAB_CA_LINES) * (LINE_CA_BYTES) + 2)
#define	TAB_CB_BYTES					((TAB_CB_LINES) * (LINE_CB_BYTES) + 1)
#define	TAB_CC_BYTES					((TAB_CC_LINES) * (LINE_CC_BYTES) + 1)
#define	TAB_CD_BYTES					((TAB_CD_LINES) * (LINE_CD_BYTES))
#define	TAB_CE_BYTES					((TAB_CE_LINES) * (LINE_CE_BYTES))
#define	TAB_CF_BYTES					((TAB_CF_LINES) * (LINE_CF_BYTES))
#define	TAB_D0_BYTES					((TAB_D0_LINES) * (LINE_D0_BYTES))



//============================================================================
//============================================================================
//标志表部分
/* 对象标识表 有效标志  地址宏定义 xxxxxx */
#define OBJ_FLAG_ADDR_START 			((u32)0x0200)												/* 标志表起始位置     */
#define OBJ_FLAG_ADDR_SET_FLAG		OBJ_FLAG_ADDR_START									/* 地址设置与否标志位 */
#define TAB_81_FLAG_ADDR 					((u32)OBJ_FLAG_ADDR_SET_FLAG + 4)
#define	TAB_82_FLAG_ADDR					((u32)TAB_81_FLAG_ADDR + TAB_81_LINES + 1)
#define	TAB_83_FLAG_ADDR					((u32)TAB_82_FLAG_ADDR + TAB_82_LINES + 1)
#define	TAB_84_FLAG_ADDR					((u32)TAB_83_FLAG_ADDR + TAB_83_LINES + 1)
#define	TAB_85_FLAG_ADDR					((u32)TAB_84_FLAG_ADDR + TAB_84_LINES + 1)
#define	TAB_86_FLAG_ADDR					((u32)TAB_85_FLAG_ADDR + TAB_85_LINES + 1)
#define	TAB_87_FLAG_ADDR					((u32)TAB_86_FLAG_ADDR + TAB_86_LINES + 1)
#define	TAB_88_FLAG_ADDR					((u32)TAB_87_FLAG_ADDR + TAB_87_LINES + 1)
#define	TAB_89_FLAG_ADDR					((u32)TAB_88_FLAG_ADDR + TAB_88_LINES + 1)
#define	TAB_8A_FLAG_ADDR					((u32)TAB_89_FLAG_ADDR + TAB_89_LINES + 1)
#define	TAB_8B_FLAG_ADDR					((u32)TAB_8A_FLAG_ADDR + TAB_8A_LINES + 1)
#define	TAB_8C_FLAG_ADDR					((u32)TAB_8B_FLAG_ADDR + TAB_8B_LINES + 1)
#define	TAB_8D_FLAG_ADDR					((u32)TAB_8C_FLAG_ADDR + TAB_8C_LINES + 1)
#define	TAB_8E_FLAG_ADDR					((u32)TAB_8D_FLAG_ADDR + TAB_8D_LINES + 1)
#define	TAB_8F_FLAG_ADDR					((u32)TAB_8E_FLAG_ADDR + TAB_8E_LINES + 1)
#define	TAB_90_FLAG_ADDR					((u32)TAB_8F_FLAG_ADDR + TAB_8F_LINES + 1)
#define	TAB_91_FLAG_ADDR					((u32)TAB_90_FLAG_ADDR + TAB_90_LINES + 1)
#define	TAB_92_FLAG_ADDR					((u32)TAB_91_FLAG_ADDR + TAB_91_LINES + 1)
#define	TAB_93_FLAG_ADDR					((u32)TAB_92_FLAG_ADDR + TAB_92_LINES + 1)
#define	TAB_94_FLAG_ADDR					((u32)TAB_93_FLAG_ADDR + TAB_93_LINES + 1)
#define	TAB_95_FLAG_ADDR					((u32)TAB_94_FLAG_ADDR + TAB_94_LINES + 1)
#define	TAB_96_FLAG_ADDR					((u32)TAB_95_FLAG_ADDR + TAB_95_LINES + 1)
#define	TAB_97_FLAG_ADDR					((u32)TAB_96_FLAG_ADDR + TAB_96_LINES + 1)
#define	TAB_98_FLAG_ADDR					((u32)TAB_97_FLAG_ADDR + TAB_97_LINES + 1)
#define	TAB_99_FLAG_ADDR					((u32)TAB_98_FLAG_ADDR + TAB_98_LINES + 1)
#define	TAB_9A_FLAG_ADDR					((u32)TAB_99_FLAG_ADDR + TAB_99_LINES + 1)
#define	TAB_9B_FLAG_ADDR					((u32)TAB_9A_FLAG_ADDR + TAB_9A_LINES + 1)
#define	TAB_9C_FLAG_ADDR					((u32)TAB_9B_FLAG_ADDR + TAB_9B_LINES + 1)
#define	TAB_9D_FLAG_ADDR					((u32)TAB_9C_FLAG_ADDR + TAB_9C_LINES + 1)
#define	TAB_9E_FLAG_ADDR					((u32)TAB_9D_FLAG_ADDR + TAB_9D_LINES + 1)
#define	TAB_9F_FLAG_ADDR					((u32)TAB_9E_FLAG_ADDR + TAB_9E_LINES + 1)
#define	TAB_A0_FLAG_ADDR					((u32)TAB_9F_FLAG_ADDR + TAB_9F_LINES + 1)
#define	TAB_A1_FLAG_ADDR					((u32)TAB_A0_FLAG_ADDR + TAB_A0_LINES + 1)
#define	TAB_A2_FLAG_ADDR					((u32)TAB_A1_FLAG_ADDR + TAB_A1_LINES + 1)
#define	TAB_A3_FLAG_ADDR					((u32)TAB_A2_FLAG_ADDR + TAB_A2_LINES + 1)
#define	TAB_A4_FLAG_ADDR					((u32)TAB_A3_FLAG_ADDR + TAB_A3_LINES + 1)
#define	TAB_A5_FLAG_ADDR					((u32)TAB_A4_FLAG_ADDR + TAB_A4_LINES + 1)
#define	TAB_A6_FLAG_ADDR					((u32)TAB_A5_FLAG_ADDR + TAB_A5_LINES + 1)
#define	TAB_A7_FLAG_ADDR					((u32)TAB_A6_FLAG_ADDR + TAB_A6_LINES + 1)
#define	TAB_A8_FLAG_ADDR					((u32)TAB_A7_FLAG_ADDR + TAB_A7_LINES + 1)
#define	TAB_A9_FLAG_ADDR					((u32)TAB_A8_FLAG_ADDR + TAB_A8_LINES + 1)
#define	TAB_AA_FLAG_ADDR					((u32)TAB_A9_FLAG_ADDR + TAB_A9_LINES + 1)
#define	TAB_AB_FLAG_ADDR					((u32)TAB_AA_FLAG_ADDR + TAB_AA_LINES + 1)
#define	TAB_AC_FLAG_ADDR					((u32)TAB_AB_FLAG_ADDR + TAB_AB_LINES + 1)
#define	TAB_AD_FLAG_ADDR					((u32)TAB_AC_FLAG_ADDR + TAB_AC_LINES + 1)
#define	TAB_AE_FLAG_ADDR					((u32)TAB_AD_FLAG_ADDR + TAB_AD_LINES + 1)
#define	TAB_AF_FLAG_ADDR					((u32)TAB_AE_FLAG_ADDR + TAB_AE_LINES + 1)
#define	TAB_B0_FLAG_ADDR					((u32)TAB_AF_FLAG_ADDR + TAB_AF_LINES + 1)
#define	TAB_B1_FLAG_ADDR					((u32)TAB_B0_FLAG_ADDR + TAB_B0_LINES + 1)
#define	TAB_B2_FLAG_ADDR					((u32)TAB_B1_FLAG_ADDR + TAB_B1_LINES + 1)
#define	TAB_B3_FLAG_ADDR					((u32)TAB_B2_FLAG_ADDR + TAB_B2_LINES + 1)
#define	TAB_B4_FLAG_ADDR					((u32)TAB_B3_FLAG_ADDR + TAB_B3_LINES + 1)
#define	TAB_B5_FLAG_ADDR					((u32)TAB_B4_FLAG_ADDR + TAB_B4_LINES + 1)
#define	TAB_B6_FLAG_ADDR					((u32)TAB_B5_FLAG_ADDR + TAB_B5_LINES + 1)
#define	TAB_B7_FLAG_ADDR					((u32)TAB_B6_FLAG_ADDR + TAB_B6_LINES + 1)
#define	TAB_B8_FLAG_ADDR					((u32)TAB_B7_FLAG_ADDR + TAB_B7_LINES + 1)
#define	TAB_B9_FLAG_ADDR					((u32)TAB_B8_FLAG_ADDR + TAB_B8_LINES + 1)
#define	TAB_BA_FLAG_ADDR					((u32)TAB_B9_FLAG_ADDR + TAB_B9_LINES + 1)
#define	TAB_BB_FLAG_ADDR					((u32)TAB_BA_FLAG_ADDR + TAB_BA_LINES + 1)
#define	TAB_BC_FLAG_ADDR					((u32)TAB_BB_FLAG_ADDR + TAB_BB_LINES + 1)
#define	TAB_BD_FLAG_ADDR					((u32)TAB_BC_FLAG_ADDR + TAB_BC_LINES + 1)
#define	TAB_BE_FLAG_ADDR					((u32)TAB_BD_FLAG_ADDR + TAB_BD_LINES + 1)
#define	TAB_BF_FLAG_ADDR					((u32)TAB_BE_FLAG_ADDR + TAB_BE_LINES + 1)
#define	TAB_C0_FLAG_ADDR					((u32)TAB_BF_FLAG_ADDR + TAB_BF_LINES + 1)
#define	TAB_C1_FLAG_ADDR					((u32)TAB_C0_FLAG_ADDR + TAB_C0_LINES + 1)
#define	TAB_C2_FLAG_ADDR					((u32)TAB_C1_FLAG_ADDR + TAB_C1_LINES + 1)
#define	TAB_C3_FLAG_ADDR					((u32)TAB_C2_FLAG_ADDR + TAB_C2_LINES + 1)
#define	TAB_C4_FLAG_ADDR					((u32)TAB_C3_FLAG_ADDR + TAB_C3_LINES + 1)
#define	TAB_C5_FLAG_ADDR					((u32)TAB_C4_FLAG_ADDR + TAB_C4_LINES + 1)
#define	TAB_C6_FLAG_ADDR					((u32)TAB_C5_FLAG_ADDR + TAB_C5_LINES + 1)
#define	TAB_C7_FLAG_ADDR					((u32)TAB_C6_FLAG_ADDR + TAB_C6_LINES + 1)
#define	TAB_C8_FLAG_ADDR					((u32)TAB_C7_FLAG_ADDR + TAB_C7_LINES + 1)
#define	TAB_C9_FLAG_ADDR					((u32)TAB_C8_FLAG_ADDR + TAB_C8_LINES + 1)
#define	TAB_CA_FLAG_ADDR					((u32)TAB_C9_FLAG_ADDR + TAB_C9_LINES + 1)
#define	TAB_CB_FLAG_ADDR					((u32)TAB_CA_FLAG_ADDR + TAB_CA_LINES + 1)
#define	TAB_CC_FLAG_ADDR					((u32)TAB_CB_FLAG_ADDR + TAB_CB_LINES + 1)
#define	TAB_CD_FLAG_ADDR					((u32)TAB_CC_FLAG_ADDR + TAB_CC_LINES + 1)
#define	TAB_CE_FLAG_ADDR					((u32)TAB_CD_FLAG_ADDR + TAB_CD_LINES + 1)
#define	TAB_CF_FLAG_ADDR					((u32)TAB_CE_FLAG_ADDR + TAB_CE_LINES + 1)
#define	TAB_D0_FLAG_ADDR					((u32)TAB_CF_FLAG_ADDR + TAB_CF_LINES + 1)
#define OBJ_FLAG_ADDR_END 				((u32)TAB_D0_FLAG_ADDR + TAB_D0_LINES + 1)


//================================================
//数据表部分
/* 对象标识 数据 地址宏定义 xxxxxx */
#define OBJ_ADDR_START 			((u32)0x0200)												/* 数据表起始位置     */
#define OBJ_ADDR_SET_FLAG		((u32)OBJ_FLAG_ADDR_END	+ 2)	/* 地址设置与否标志位 OBJ_ADDR_START*/
#define TAB_81_ADDR 				((u32)OBJ_ADDR_SET_FLAG + 4)
#define	TAB_82_ADDR					((u32)TAB_81_ADDR + TAB_81_BYTES)
#define	TAB_83_ADDR					((u32)TAB_82_ADDR + TAB_82_BYTES)
#define	TAB_84_ADDR					((u32)TAB_83_ADDR + TAB_83_BYTES)
#define	TAB_85_ADDR					((u32)TAB_84_ADDR + TAB_84_BYTES)
#define	TAB_86_ADDR					((u32)TAB_85_ADDR + TAB_85_BYTES)
#define	TAB_87_ADDR					((u32)TAB_86_ADDR + TAB_86_BYTES)
#define	TAB_88_ADDR					((u32)TAB_87_ADDR + TAB_87_BYTES)
#define	TAB_89_ADDR					((u32)TAB_88_ADDR + TAB_88_BYTES)
#define	TAB_8A_ADDR					((u32)TAB_89_ADDR + TAB_89_BYTES)
#define	TAB_8B_ADDR					((u32)TAB_8A_ADDR + TAB_8A_BYTES)
#define	TAB_8C_ADDR					((u32)TAB_8B_ADDR + TAB_8B_BYTES)
#define	TAB_8D_ADDR					((u32)TAB_8C_ADDR + TAB_8C_BYTES)
#define	TAB_8E_ADDR					((u32)TAB_8D_ADDR + TAB_8D_BYTES)
#define	TAB_8F_ADDR					((u32)TAB_8E_ADDR + TAB_8E_BYTES)
#define	TAB_90_ADDR					((u32)TAB_8F_ADDR + TAB_8F_BYTES)
#define	TAB_91_ADDR					((u32)TAB_90_ADDR + TAB_90_BYTES)
#define	TAB_92_ADDR					((u32)TAB_91_ADDR + TAB_91_BYTES)
#define	TAB_93_ADDR					((u32)TAB_92_ADDR + TAB_92_BYTES)
#define	TAB_94_ADDR					((u32)TAB_93_ADDR + TAB_93_BYTES)
#define	TAB_95_ADDR					((u32)TAB_94_ADDR + TAB_94_BYTES)
#define	TAB_96_ADDR					((u32)TAB_95_ADDR + TAB_95_BYTES)
#define	TAB_97_ADDR					((u32)TAB_96_ADDR + TAB_96_BYTES)
#define	TAB_98_ADDR					((u32)TAB_97_ADDR + TAB_97_BYTES)
#define	TAB_99_ADDR					((u32)TAB_98_ADDR + TAB_98_BYTES)
#define	TAB_9A_ADDR					((u32)TAB_99_ADDR + TAB_99_BYTES)
#define	TAB_9B_ADDR					((u32)TAB_9A_ADDR + TAB_9A_BYTES)
#define	TAB_9C_ADDR					((u32)TAB_9B_ADDR + TAB_9B_BYTES)
#define	TAB_9D_ADDR					((u32)TAB_9C_ADDR + TAB_9C_BYTES)
#define	TAB_9E_ADDR					((u32)TAB_9D_ADDR + TAB_9D_BYTES)
#define	TAB_9F_ADDR					((u32)TAB_9E_ADDR + TAB_9E_BYTES)
#define	TAB_A0_ADDR					((u32)TAB_9F_ADDR + TAB_9F_BYTES)
#define	TAB_A1_ADDR					((u32)TAB_A0_ADDR + TAB_A0_BYTES)
#define	TAB_A2_ADDR					((u32)TAB_A1_ADDR + TAB_A1_BYTES)
#define	TAB_A3_ADDR					((u32)TAB_A2_ADDR + TAB_A2_BYTES)
#define	TAB_A4_ADDR					((u32)TAB_A3_ADDR + TAB_A3_BYTES)
#define	TAB_A5_ADDR					((u32)TAB_A4_ADDR + TAB_A4_BYTES)
#define	TAB_A6_ADDR					((u32)TAB_A5_ADDR + TAB_A5_BYTES)
#define	TAB_A7_ADDR					((u32)TAB_A6_ADDR + TAB_A6_BYTES)
#define	TAB_A8_ADDR					((u32)TAB_A7_ADDR + TAB_A7_BYTES)
#define	TAB_A9_ADDR					((u32)TAB_A8_ADDR + TAB_A8_BYTES)
#define	TAB_AA_ADDR					((u32)TAB_A9_ADDR + TAB_A9_BYTES)
#define	TAB_AB_ADDR					((u32)TAB_AA_ADDR + TAB_AA_BYTES)
#define	TAB_AC_ADDR					((u32)TAB_AB_ADDR + TAB_AB_BYTES)
#define	TAB_AD_ADDR					((u32)TAB_AC_ADDR + TAB_AC_BYTES)
#define	TAB_AE_ADDR					((u32)TAB_AD_ADDR + TAB_AD_BYTES)
#define	TAB_AF_ADDR					((u32)TAB_AE_ADDR + TAB_AE_BYTES)
#define	TAB_B0_ADDR					((u32)TAB_AF_ADDR + TAB_AF_BYTES)
#define	TAB_B1_ADDR					((u32)TAB_B0_ADDR + TAB_B0_BYTES)
#define	TAB_B2_ADDR					((u32)TAB_B1_ADDR + TAB_B1_BYTES)
#define	TAB_B3_ADDR					((u32)TAB_B2_ADDR + TAB_B2_BYTES)
#define	TAB_B4_ADDR					((u32)TAB_B3_ADDR + TAB_B3_BYTES)
#define	TAB_B5_ADDR					((u32)TAB_B4_ADDR + TAB_B4_BYTES)
#define	TAB_B6_ADDR					((u32)TAB_B5_ADDR + TAB_B5_BYTES)
#define	TAB_B7_ADDR					((u32)TAB_B6_ADDR + TAB_B6_BYTES)
#define	TAB_B8_ADDR					((u32)TAB_B7_ADDR + TAB_B7_BYTES)
#define	TAB_B9_ADDR					((u32)TAB_B8_ADDR + TAB_B8_BYTES)
#define	TAB_BA_ADDR					((u32)TAB_B9_ADDR + TAB_B9_BYTES)
#define	TAB_BB_ADDR					((u32)TAB_BA_ADDR + TAB_BA_BYTES)
#define	TAB_BC_ADDR					((u32)TAB_BB_ADDR + TAB_BB_BYTES)
#define	TAB_BD_ADDR					((u32)TAB_BC_ADDR + TAB_BC_BYTES)
#define	TAB_BE_ADDR					((u32)TAB_BD_ADDR + TAB_BD_BYTES)
#define	TAB_BF_ADDR					((u32)TAB_BE_ADDR + TAB_BE_BYTES)
#define	TAB_C0_ADDR					((u32)TAB_BF_ADDR + TAB_BF_BYTES)
#define	TAB_C1_ADDR					((u32)TAB_C0_ADDR + TAB_C0_BYTES)
#define	TAB_C2_ADDR					((u32)TAB_C1_ADDR + TAB_C1_BYTES)
#define	TAB_C3_ADDR					((u32)TAB_C2_ADDR + TAB_C2_BYTES)
#define	TAB_C4_ADDR					((u32)TAB_C3_ADDR + TAB_C3_BYTES)
#define	TAB_C5_ADDR					((u32)TAB_C4_ADDR + TAB_C4_BYTES)
#define	TAB_C6_ADDR					((u32)TAB_C5_ADDR + TAB_C5_BYTES)
#define	TAB_C7_ADDR					((u32)TAB_C6_ADDR + TAB_C6_BYTES)
#define	TAB_C8_ADDR					((u32)TAB_C7_ADDR + TAB_C7_BYTES)
#define	TAB_C9_ADDR					((u32)TAB_C8_ADDR + TAB_C8_BYTES)
#define	TAB_CA_ADDR					((u32)TAB_C9_ADDR + TAB_C9_BYTES)
#define	TAB_CB_ADDR					((u32)TAB_CA_ADDR + TAB_CA_BYTES)
#define	TAB_CC_ADDR					((u32)TAB_CB_ADDR + TAB_CB_BYTES)
#define	TAB_CD_ADDR					((u32)TAB_CC_ADDR + TAB_CC_BYTES)
#define	TAB_CE_ADDR					((u32)TAB_CD_ADDR + TAB_CD_BYTES)
#define	TAB_CF_ADDR					((u32)TAB_CE_ADDR + TAB_CE_BYTES)
#define	TAB_D0_ADDR					((u32)TAB_CF_ADDR + TAB_CF_BYTES)
#define OBJ_ADDR_END 				((u32)TAB_D0_ADDR + TAB_D0_BYTES)


//-------92表索引二当前值保存地址-----------------------
#define INDEX_92_ADDR_START ((u32)OBJ_ADDR_END + 2)	
#define INDEX_92_ADDR_END ((u32)INDEX_92_ADDR_START + 255)	

//------路口地图信息数据 ID暂用 0x70-----------
#define	TAB_ROAD_MAP_ID_ADDR					((u32)INDEX_92_ADDR_END+ 10) //0X70标识 ROAD_MAP_ID mm20140509
//
//================================================


// /*  整表字节数 宏定义 xxxxxx */
// #define TAB_81_BYTES 					7
// #define	TAB_82_BYTES					6
// #define	TAB_83_BYTES					7
// #define	TAB_84_BYTES					7
// #define	TAB_85_BYTES					1667
// #define	TAB_86_BYTES					9
// #define	TAB_87_BYTES					9
// #define	TAB_88_BYTES					9
// #define	TAB_89_BYTES					6
// #define	TAB_8A_BYTES					6
// #define	TAB_8B_BYTES					6
// #define	TAB_8C_BYTES					6
// #define	TAB_8D_BYTES					443
// #define	TAB_8E_BYTES					7683
// #define	TAB_8F_BYTES					6
// #define	TAB_90_BYTES					6
// #define	TAB_91_BYTES					643
// #define	TAB_92_BYTES					195843
// #define	TAB_93_BYTES					6
// #define	TAB_94_BYTES					6
// #define	TAB_95_BYTES					227
// #define	TAB_96_BYTES					15
// #define	TAB_97_BYTES					83
// #define	TAB_98_BYTES					6
// #define	TAB_99_BYTES					6
// #define	TAB_9A_BYTES					6
// #define	TAB_9B_BYTES					6
// #define	TAB_9C_BYTES					6
// #define	TAB_9D_BYTES					6
// #define	TAB_9E_BYTES					6
// #define	TAB_9F_BYTES					531
// #define	TAB_A0_BYTES					43
// #define	TAB_A1_BYTES					435
// #define	TAB_A2_BYTES					243
// #define	TAB_A3_BYTES					6
// #define	TAB_A4_BYTES					6
// #define	TAB_A5_BYTES					6
// #define	TAB_A6_BYTES					6
// #define	TAB_A7_BYTES					6
// #define	TAB_A8_BYTES					6
// #define	TAB_A9_BYTES					6
// #define	TAB_AA_BYTES					6
// #define	TAB_AB_BYTES					6
// #define	TAB_AC_BYTES					9
// #define	TAB_AD_BYTES					9
// #define	TAB_AE_BYTES					6
// #define	TAB_AF_BYTES					6
// #define	TAB_B0_BYTES					99
// #define	TAB_B1_BYTES					15
// #define	TAB_B2_BYTES					6
// #define	TAB_B3_BYTES					6
// #define	TAB_B4_BYTES					6
// #define	TAB_B5_BYTES					6
// #define	TAB_B6_BYTES					6
// #define	TAB_B7_BYTES					6
// #define	TAB_B8_BYTES					6
// #define	TAB_B9_BYTES					6
// #define	TAB_BA_BYTES					6
// #define	TAB_BB_BYTES					6
// #define	TAB_BC_BYTES					6
// #define	TAB_BD_BYTES					19
// #define	TAB_BE_BYTES					21
// #define	TAB_BF_BYTES					21
// #define	TAB_C0_BYTES					227
// #define	TAB_C1_BYTES					2563
// #define	TAB_C2_BYTES					6
// #define	TAB_C3_BYTES					6
// #define	TAB_C4_BYTES					7
// #define	TAB_C5_BYTES					6
// #define	TAB_C6_BYTES					6
// #define	TAB_C7_BYTES					6
// #define	TAB_C8_BYTES					331
// #define	TAB_C9_BYTES					9
// #define	TAB_CA_BYTES					899
// #define	TAB_CB_BYTES					67
// #define	TAB_CC_BYTES					67
// #define	TAB_CD_BYTES					17
// #define	TAB_CE_BYTES					7
// #define	TAB_CF_BYTES					9
// #define	TAB_D0_BYTES					71
//
// /* 对象标识 新表 地址宏定义 xxxxxx */
// //#define OBJ_ADDR_START 			((u32)0x0200)												/* 数据库起始地址     */
// #define OBJ_ADDR_START 			((u32)0x0200)												/* 数据库起始地址     */
// #define OBJ_ADDR_SET_FLAG		OBJ_ADDR_START											/* 地址设置与否标志位 */
// #define TAB_81_ADDR 				((u32)OBJ_ADDR_SET_FLAG + 4)
// #define	TAB_82_ADDR					((u32)TAB_81_ADDR + TAB_81_BYTES)
// #define	TAB_83_ADDR					((u32)TAB_82_ADDR + TAB_82_BYTES)
// #define	TAB_84_ADDR					((u32)TAB_83_ADDR + TAB_83_BYTES)
// #define	TAB_85_ADDR					((u32)TAB_84_ADDR + TAB_84_BYTES)
// #define	TAB_86_ADDR					((u32)TAB_85_ADDR + TAB_85_BYTES)
// #define	TAB_87_ADDR					((u32)TAB_86_ADDR + TAB_86_BYTES)
// #define	TAB_88_ADDR					((u32)TAB_87_ADDR + TAB_87_BYTES)
// #define	TAB_89_ADDR					((u32)TAB_88_ADDR + TAB_88_BYTES)
// #define	TAB_8A_ADDR					((u32)TAB_89_ADDR + TAB_89_BYTES)
// #define	TAB_8B_ADDR					((u32)TAB_8A_ADDR + TAB_8A_BYTES)
// #define	TAB_8C_ADDR					((u32)TAB_8B_ADDR + TAB_8B_BYTES)
// #define	TAB_8D_ADDR					((u32)TAB_8C_ADDR + TAB_8C_BYTES)
// #define	TAB_8E_ADDR					((u32)TAB_8D_ADDR + TAB_8D_BYTES)
// #define	TAB_8F_ADDR					((u32)TAB_8E_ADDR + TAB_8E_BYTES)
// #define	TAB_90_ADDR					((u32)TAB_8F_ADDR + TAB_8F_BYTES)
// #define	TAB_91_ADDR					((u32)TAB_90_ADDR + TAB_90_BYTES)
// #define	TAB_92_ADDR					((u32)TAB_91_ADDR + TAB_91_BYTES)
// #define	TAB_93_ADDR					((u32)TAB_92_ADDR + TAB_92_BYTES)
// #define	TAB_94_ADDR					((u32)TAB_93_ADDR + TAB_93_BYTES)
// #define	TAB_95_ADDR					((u32)TAB_94_ADDR + TAB_94_BYTES)
// #define	TAB_96_ADDR					((u32)TAB_95_ADDR + TAB_95_BYTES)
// #define	TAB_97_ADDR					((u32)TAB_96_ADDR + TAB_96_BYTES)
// #define	TAB_98_ADDR					((u32)TAB_97_ADDR + TAB_97_BYTES)
// #define	TAB_99_ADDR					((u32)TAB_98_ADDR + TAB_98_BYTES)
// #define	TAB_9A_ADDR					((u32)TAB_99_ADDR + TAB_99_BYTES)
// #define	TAB_9B_ADDR					((u32)TAB_9A_ADDR + TAB_9A_BYTES)
// #define	TAB_9C_ADDR					((u32)TAB_9B_ADDR + TAB_9B_BYTES)
// #define	TAB_9D_ADDR					((u32)TAB_9C_ADDR + TAB_9C_BYTES)
// #define	TAB_9E_ADDR					((u32)TAB_9D_ADDR + TAB_9D_BYTES)
// #define	TAB_9F_ADDR					((u32)TAB_9E_ADDR + TAB_9E_BYTES)
// #define	TAB_A0_ADDR					((u32)TAB_9F_ADDR + TAB_9F_BYTES)
// #define	TAB_A1_ADDR					((u32)TAB_A0_ADDR + TAB_A0_BYTES)
// #define	TAB_A2_ADDR					((u32)TAB_A1_ADDR + TAB_A1_BYTES)
// #define	TAB_A3_ADDR					((u32)TAB_A2_ADDR + TAB_A2_BYTES)
// #define	TAB_A4_ADDR					((u32)TAB_A3_ADDR + TAB_A3_BYTES)
// #define	TAB_A5_ADDR					((u32)TAB_A4_ADDR + TAB_A4_BYTES)
// #define	TAB_A6_ADDR					((u32)TAB_A5_ADDR + TAB_A5_BYTES)
// #define	TAB_A7_ADDR					((u32)TAB_A6_ADDR + TAB_A6_BYTES)
// #define	TAB_A8_ADDR					((u32)TAB_A7_ADDR + TAB_A7_BYTES)
// #define	TAB_A9_ADDR					((u32)TAB_A8_ADDR + TAB_A8_BYTES)
// #define	TAB_AA_ADDR					((u32)TAB_A9_ADDR + TAB_A9_BYTES)
// #define	TAB_AB_ADDR					((u32)TAB_AA_ADDR + TAB_AA_BYTES)
// #define	TAB_AC_ADDR					((u32)TAB_AB_ADDR + TAB_AB_BYTES)
// #define	TAB_AD_ADDR					((u32)TAB_AC_ADDR + TAB_AC_BYTES)
// #define	TAB_AE_ADDR					((u32)TAB_AD_ADDR + TAB_AD_BYTES)
// #define	TAB_AF_ADDR					((u32)TAB_AE_ADDR + TAB_AE_BYTES)
// #define	TAB_B0_ADDR					((u32)TAB_AF_ADDR + TAB_AF_BYTES)
// #define	TAB_B1_ADDR					((u32)TAB_B0_ADDR + TAB_B0_BYTES)
// #define	TAB_B2_ADDR					((u32)TAB_B1_ADDR + TAB_B1_BYTES)
// #define	TAB_B3_ADDR					((u32)TAB_B2_ADDR + TAB_B2_BYTES)
// #define	TAB_B4_ADDR					((u32)TAB_B3_ADDR + TAB_B3_BYTES)
// #define	TAB_B5_ADDR					((u32)TAB_B4_ADDR + TAB_B4_BYTES)
// #define	TAB_B6_ADDR					((u32)TAB_B5_ADDR + TAB_B5_BYTES)
// #define	TAB_B7_ADDR					((u32)TAB_B6_ADDR + TAB_B6_BYTES)
// #define	TAB_B8_ADDR					((u32)TAB_B7_ADDR + TAB_B7_BYTES)
// #define	TAB_B9_ADDR					((u32)TAB_B8_ADDR + TAB_B8_BYTES)
// #define	TAB_BA_ADDR					((u32)TAB_B9_ADDR + TAB_B9_BYTES)
// #define	TAB_BB_ADDR					((u32)TAB_BA_ADDR + TAB_BA_BYTES)
// #define	TAB_BC_ADDR					((u32)TAB_BB_ADDR + TAB_BB_BYTES)
// #define	TAB_BD_ADDR					((u32)TAB_BC_ADDR + TAB_BC_BYTES)
// #define	TAB_BE_ADDR					((u32)TAB_BD_ADDR + TAB_BD_BYTES)
// #define	TAB_BF_ADDR					((u32)TAB_BE_ADDR + TAB_BE_BYTES)
// #define	TAB_C0_ADDR					((u32)TAB_BF_ADDR + TAB_BF_BYTES)
// #define	TAB_C1_ADDR					((u32)TAB_C0_ADDR + TAB_C0_BYTES)
// #define	TAB_C2_ADDR					((u32)TAB_C1_ADDR + TAB_C1_BYTES)
// #define	TAB_C3_ADDR					((u32)TAB_C2_ADDR + TAB_C2_BYTES)
// #define	TAB_C4_ADDR					((u32)TAB_C3_ADDR + TAB_C3_BYTES)
// #define	TAB_C5_ADDR					((u32)TAB_C4_ADDR + TAB_C4_BYTES)
// #define	TAB_C6_ADDR					((u32)TAB_C5_ADDR + TAB_C5_BYTES)
// #define	TAB_C7_ADDR					((u32)TAB_C6_ADDR + TAB_C6_BYTES)
// #define	TAB_C8_ADDR					((u32)TAB_C7_ADDR + TAB_C7_BYTES)
// #define	TAB_C9_ADDR					((u32)TAB_C8_ADDR + TAB_C8_BYTES)
// #define	TAB_CA_ADDR					((u32)TAB_C9_ADDR + TAB_C9_BYTES)
// #define	TAB_CB_ADDR					((u32)TAB_CA_ADDR + TAB_CA_BYTES)
// #define	TAB_CC_ADDR					((u32)TAB_CB_ADDR + TAB_CB_BYTES)
// #define	TAB_CD_ADDR					((u32)TAB_CC_ADDR + TAB_CC_BYTES)
// #define	TAB_CE_ADDR					((u32)TAB_CD_ADDR + TAB_CD_BYTES)
// #define	TAB_CF_ADDR					((u32)TAB_CE_ADDR + TAB_CE_BYTES)
// #define	TAB_D0_ADDR					((u32)TAB_CF_ADDR + TAB_CF_BYTES)
// #define OBJ_ADDR_END 				((u32)TAB_D0_ADDR + TAB_D0_BYTES)


/* 表对象属性结构体 xxxxxx */
typedef struct 
{
	u8 	ID;			  						/* 对象标识					*/
	u32	Data_Addr;					  /* 数据地址					*/
	u32	Flag_Addr;			 			/* 标志地址					*/
	u16	Line_Number;					/* 支持行数					*/
	u8	Subobj_Number;			  /* 子对象数					*/
	u8	Line_leng;			    	/* 每行字节数				*/
	u32 Tab_leng;			      	/* 对象表字节数			*/
	u8  Index1;								/* 索引1最大值			*/
	u8  Index2;								/* 索引2最大值			*/
	u8  ID_type;              /* 标志类型 */
}object_attribute_struct;
// typedef struct 
// {
// 	u8 	Object_ID;			  		/* 对象标识					*/
// 	u32	Object_Addr;					/* 地址							*/
// 	u16	Line_Number;					/* 支持行数					*/
// 	u8	Subobject_Number;			/* 子对象数					*/
// 	u8	Line_Byte_Max;				/* 每行字节数				*/
// 	u8  Line_Byte_Number;		  /* 每行实际字节数		*/
// 	u32 Object_Number;				/* 对象表字节数			*/
// 	u8  Index1;
// 	u8  Index2;
// }object_attribute_struct;

#define  MIN_ID   0X81
#define  MAX_ID   0XC9    //目前做到最大为0XC9对象标识  
#define  BAOLIU   2      	//保留的数据长度 
#define  SUBOBJ_NUM_MAX   12


/* 发往以太网  */
 typedef struct
{
	u8 			data[2600];   //  缓存,发往以太网的数据
	u32 		lenth;		//  发往以太网的数据的长度
	BOOL    error;	
	ERROR_TYPE_E  error_type;
}to_eth_struct;

/* 读写数据库API结构  */
 typedef struct
{
	u8 									action;					// write  or  read
	u32 *								p_Lenth;				// if read,the data_lenth	
	u8 *								p_Error;				// __TRUE or __FALSE
	OS_SEM *						p_Src_sem;			// source_task  要写数据库的任务等待的信号量
	TARGET_DATA *				p_Target;  			// 待写入(读出)数据库的对象(对象标识)
}db_wr_re_struct;

#define DB_ACTION_READ  	1    //  debug_lm
#define DB_ACTION_WRITE  	2    //  debug_lm 

