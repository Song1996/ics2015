#include "cpu/exec/helper.h"
/*
#define DATA_BYTE 1
#include "movsx-template.h"
#undef DATA_BYTE
*/
#define DATA_BYTE 2
#include "movsx-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "movsx-template.h"
#undef DATA_BYTE

make_helper_v(movsx_rm2r)
make_helper_v(movsx2_rm2r)
