#ifndef SCALC_H
#define SCALC_H

#include "gen_attr.h"
#include "atrex.h"

struct latest_operation {
	float result;
};

struct func_descr {
	char* name;
	int pc;
	struct attnode* body;
	struct attnode* def;
};

int go_deep_scalc(struct exec_descr* ctx, struct attnode* node, void* input, void* closure);
struct exec_descr* reg_cpts_srun();

int chk_subtree_type(struct attnode* node, int N, char* type);

void attach_cpts_operator(struct exec_descr* ctx);
void attach_cpts_assign(struct exec_descr* ctx);
void attach_cpts_func(struct exec_descr* ctx);

#endif
