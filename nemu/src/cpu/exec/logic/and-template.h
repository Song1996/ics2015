#include "cpu/exec/template-start.h"

#define instr and

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	OPERAND_W(op_dest, result);

	// TODO: Update EFLAGS. 
	//panic("please implement me");
	cpu.OF = 0;//(left<min && right>=min && result>=min)||(left>=min && right<min && result<min)||(left==0 && right==min);
	cpu.SF = !!MSB(result);
	cpu.ZF = !result;
	cpu.CF = 0;//(result>left);
	int i = 0;
	DATA_TYPE copy = result;
	int bit = 1;
	for(i=0;i<8;i++)
	{
		    bit^=(copy&1);
			    copy>>=1;
	}
	cpu.PF=bit;
	print_asm_template2();
}

/*
static void do_execute () {
		DATA_TYPE result = op_dest->val & op_src->val;
			OPERAND_W(op_dest, result);

				 TODO: Update EFLAGS. 
				cpu.CF=cpu.OF=0;
					cpu.ZF=!result;
						unsigned n=sizeof(DATA_TYPE)*8-1;
							cpu.SF=(result>>n)&1;
								bool bit=result&1;
									int i;
										for(i=0;i<7;i++)
												{
															result>>=1;
																	bit^=result&1;
																		}
											cpu.PF=!bit;

												print_asm_template2();
}
*/
make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
