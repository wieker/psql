#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "stype.h"
#include "scalc.h"

#include <stdio.h>
#include <math.h>

/* static */
int
calc_number(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	char* value = atree_get_attr(node, "value");
	sscanf(value, "%f", & (input->result));
	return 1;
}

/* static */
int
calc_sum(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	float r;
	char* op = atree_get_attr(node, "operation");
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	r = input->result;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (!strcmp(op, "plus"))
		input->result = r + input->result;
	else
		input->result = r - input->result;
	return 1;
}

/* static */
int
calc_prod(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	float r;
	char* op = atree_get_attr(node, "operation");
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	r = input->result;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (!strcmp(op, "mul"))
		input->result = r * input->result;
	else if (input->result != 0)
		input->result = r / input->result;
	else
		return 0;
	return 1;
}

/* static */
int
calc_pow(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	float r;
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	r = input->result;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	input->result = pow(r, input->result);
	return 1;
}

/* static */
int
calc_compare(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	float r;
	char* op = atree_get_attr(node, "operation");
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	r = input->result;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (!strcmp(op, "equal"))
		input->result = (r == input->result);
	else if (!strcmp(op, "less"))
		input->result = r < input->result;
	else
		input->result = r > input->result;
	return 1;
}

/* static */
int
calc_andor(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	float r;
	char* op = atree_get_attr(node, "operation");
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	r = input->result;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (!strcmp(op, "and"))
		input->result = (r != 0) && (input->result != 0);
	else
		input->result = (r != 0) || (input->result != 0);
	return 1;
}

/* static */
int
calc_negative(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
		
	input->result *= -1;
	return 1;
}

/* static */
int
calc_not(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	int nc = atree_count(node, "nottok");
	int i = 0;
	if (! go_deep_scalc(ctx, node, input, closure))
		return 0;
	while (i < nc) {
		if (input->result != 0)
			input->result = 0;
		else
			input->result = 1;
		i ++;
	}
	return 1;
}


struct exec_descr*
reg_cpts_srun()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ctx, "number", (run_atom_cb_t) calc_number, NULL);
	atree_caller_reg(ctx, "sum", (run_atom_cb_t) calc_sum, NULL);
	atree_caller_reg(ctx, "prod", (run_atom_cb_t) calc_prod, NULL);
	atree_caller_reg(ctx, "pow", (run_atom_cb_t) calc_pow, NULL);
	atree_caller_reg(ctx, "compare", (run_atom_cb_t) calc_compare, NULL);
	atree_caller_reg(ctx, "andor", (run_atom_cb_t) calc_andor, NULL);
	atree_caller_reg(ctx, "negative", (run_atom_cb_t) calc_negative, NULL);
	atree_caller_reg(ctx, "not", (run_atom_cb_t) calc_not, NULL);
	//atree_caller_reg(ctx, "lstname", (run_atom_cb_t) make_list, NULL);
	return ctx;
}

