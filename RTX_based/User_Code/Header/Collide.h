#ifndef __COLLIDE_H__
#define __COLLIDE_H__

extern u16 Error_Record_Count;

//collide
//=====³åÍ»¼ì²â.»·×´Ì¬±í+ barrier_flag,ring_flag
typedef struct {
	u8 Ring_num;
	u8 Phase_num;
	u16 SIMUL_Phase;
	u8 Func_ON_OFF;

	u8 phase_simul[16];
	u8 barrier_flag;
}Ring_Stutas;

//=====³åÍ»¼ì²â.»·×´Ì¬±í
typedef struct {
	u8 Ring_num;
	u8 Phase_num;
	u16 SIMUL_Phase;
	u8 Func_ON_OFF;
}Ring_Stutas_Table;
//=====³åÍ»¼ì²â.»·×´Ì¬±í
typedef struct {
	u8 Phase_Order_num;
	u8 Ring_num;
	u8 Phase_num;
}Phase_Order_Table;
//===================

extern Ring_Stutas ring_stutas[17];
extern Ring_Stutas_Table phase_8[17];
//»·×´Ì¬
extern Ring_Stutas_Table 		Table_0xCA_read;
extern Phase_Order_Table                Table_0xCB_read;


u8 Collide_Detect( u16 phase_bits);
void Barrier(void);




#endif
