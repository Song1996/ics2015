#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){
	int src = op_src->val;
	cpu.eip += src;
	cpu.esp-=4;
	swaddr_write(cpu.esp,4,cpu.eip+5);
	//printf("call $0x%x",src);
	print_asm_template1();
	
}
//#if DATA_BYTE==2||DATA_BYPE==4
make_instr_helper(i)
//#endif

#include "cpu/exec/template-end.h"