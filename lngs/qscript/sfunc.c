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
function_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	/*
	 * retrivieng function definition from table
	 * */
	char* name = atree_get_subnode_attr(node, 1, "value");
	struct func_descr* def;
	if (!table_get_element(functions, name, (void **) &def))
		return 0;
	/*
	 * define local variables
	 * */
	void* newvars = NULL,* oldvars;
	/*
	 * for each param variable set it to new local variables
	 * */
	int i = 1;
	while (i <= def->pc) {
		struct attnode* to_calc = atree_subtree(node, i + 1);
		if (! to_calc)
			return 0;
		if (! call_atree(ctx, to_calc, input))
			return 0;
		char* def_varname = atree_get_subnode_attr(def->def, i + 1, "value");
		struct latest_operation* op = mem_alloc(sizeof(*op));
		op->result = input->result;
		table_set_element((struct table **) &newvars, def_varname, (void *) op);
		cpl_debug("set %s to %f\n", def_varname, op->result);
		i ++;
	}
	
	/*
	 * exchange old varibales and new; execute and restore
	 * */
	oldvars = variables;
	variables = newvars;
	call_atree(ctx, def->body, input);
	variables = oldvars;
	
	/*
	 * result should be returened through last latest_operation
	 * */
	return 1;
}

/* static */
int
def_unirun(struct exec_descr* ctx, struct attnode* node, struct latest_operation* input, void* closure)
{
	char* funcname = atree_get_subnode_attr(node, 1, "value");
	int param_count = atree_count(node, "name") - 1;
	struct attnode* body = atree_subtree_name(node, "body", 1);
	struct func_descr* d = mem_alloc(sizeof(*d));
	
	d->name = mem_strdup(funcname);
	d->pc = param_count;
	d->body = body;
	d->def = node;
	
	
	table_set_element(&functions, funcname, (void *) d);
	return 1;
}


void
attach_cpts_func(struct exec_descr* ctx)
{
	atree_caller_reg(ctx, "def", (run_atom_cb_t) def_unirun, NULL);
	atree_caller_reg(ctx, "function", (run_atom_cb_t) function_unirun, NULL);
}



