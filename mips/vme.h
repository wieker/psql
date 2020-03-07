#ifndef VME_H
#define VME_H

#include "utils.h"

typedef int (*opexec_t)(struct vme* vme);

struct opdescr {
	char* opname;
	char* opparam;
	opexec_t exec;
} ops[] = {
	{"add", "$r, $r, $r", op_exec},
	{"addi", "$r, $r, $c, op_exec},
	{NULL, NULL, NULL},
};

#endif
