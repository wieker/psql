#ifndef FUNC_DESCR_H
#define FUNC_DESCR_H

#include "cpts.h"

struct typed_args {
	struct typed_args* next;
	int type;
	union cpt_result param;
};

typedef union cpt_result (*caller_t) (char* name, struct typed_args* args);

struct func_descr {
	char* name;
	caller_t caller;
	struct typed_args* args;
};

void reg_builtin();

#endif
