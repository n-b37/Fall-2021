#ifndef SIM_PROC_H
#define SIM_PROC_H



typedef struct proc_params{
     long int rob_size;
     long int iq_size;
     long int width;
}proc_params;





typedef struct ROBcell{
     int dest_reg;
     int ready_bit;
     int pc;
    
}robcell;



typedef struct RMTcell{
     int valid;
     int tag;
     int pc;
}rmtcell;




typedef struct Instruction
{
     long int pc;
    int op_type, dest, src1, src2;
    int cycle_count;
	//int pc, type, dst, rs1, rs2, count;
    int src1_actual,src2_actual;
	int s1_readybit_in_iq, s2_readybit_in_iq, s1_is_from_ROB, s2_is_from_ROB;
	int fe_s, fe_cyc, de_s, de_cyc, rn_strt, rn_cyc, rr_s, rr_cyc, dis, di_cyc;
	int is_s, is_cyc, ex_s, ex_cyc, wb_s, wb_cyc, rt_s, rt_cyc;
	//int rs1_og, rs2_og;

	
}instrr;




#endif
