#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "stype.h"
#include "scalc.h"

#include <stdio.h>

#include "table.h"

/* static */
int
name_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	struct latest_operation* op;
	char* name = atree_get_attr(node, "value");
	if (!table_get_element(variables, name, (void **) &op))
		return 0;
	input->result = op->result;
	cpl_debug("get %s to %f\n", name, op->result);
	return 1;
}

/* static */
int
assign_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	struct latest_operation* op;
	char* varname = atree_get_subnode_attr(node, 1, "value");
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	
	if (table_get_element(variables, varname, (void **) &op))
		op->result = input->result;
	else {
		op = mem_alloc(sizeof(*op));
		op->result = input->result;
		table_set_element(&variables, varname, (void *) op);
	}
	return 1;
}


void
attach_cpts_assign(struct exec_descr* ctx)
{
	atree_caller_reg(ctx, "name", (run_atom_cb_t) name_unirun, NULL);
	atree_caller_reg(ctx, "assign", (run_atom_cb_t) assign_unirun, NULL);
}


