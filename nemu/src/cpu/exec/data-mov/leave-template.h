#include "cpu/exec/template-start.h"

make_helper(concat(leave_,SUFFIX))
{
	cpu.esp=cpu.ebp;
	cpu.ebp=swaddr_read(cpu.esp,DATA_BYTE);
	cpu.esp+=4;
	print_asm("leave");
	return 1;
}


#include "cpu/exec/template-end.h"
