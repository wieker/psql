#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "stype.h"
#include "scalc.h"

#include <stdio.h>

/* static */
int
opop_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	call_atree(ctx, atree_subtree(node, 1), input);
	printf("result: %f\n", input->result);
	return 1;
}

/* static */
int
if_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	call_atree(ctx, atree_subtree(node, 1), input);
	if (input->result)
		call_atree(ctx, atree_subtree(node, 2), input);
	return 1;
}

/* static */
int
while_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	while (call_atree(ctx, atree_subtree(node, 1), input) && input->result &&
			call_atree(ctx, atree_subtree(node, 2), input))
		;
	return 1;
}


void
attach_cpts_operator(struct exec_descr* ctx)
{
	atree_caller_reg(ctx, "opop", (run_atom_cb_t) opop_unirun, NULL);
	atree_caller_reg(ctx, "if", (run_atom_cb_t) if_unirun, NULL);
	atree_caller_reg(ctx, "while", (run_atom_cb_t) while_unirun, NULL);
}


