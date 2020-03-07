#include "table.h"
#include "func_descr.h"

union cpt_result
getpi(char* name, struct typed_args* args)
{
	union cpt_result res;
	res.type = CPT_RES_NUMBER;
	res.number.number = 3.14;
	return res;
}

struct func_descr builtins[] = {
	{"getpi", getpi, NULL},
	{NULL, NULL, NULL},
};

void
reg_builtin()
{
	int i;
	for (i = 0; builtins[i].caller; i ++) {
		table_set_element(& functions, builtins[i].name, &(builtins[i]));
	}
}

