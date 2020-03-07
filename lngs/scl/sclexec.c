#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "scl.h"
#include "scalc.h"




static int
data_cb(struct exec_descr* ctx, struct attnode* node, void* input, struct scl_closure* closure)
{
	struct scl_input* i = mem_alloc(sizeof(*i));
	i->node_to_process = node;
	i->data_tree_ctx = ctx;
	i->external_ctx = input;
	
	i->callgate_ctx = closure->callgate_ctx;
	
	int rt = go_deep_scalc(closure->scl_executor, closure->node_to_exec, i, NULL);
	mem_free(i);
	return rt;
}

int
regop(struct exec_descr* ctx, struct attnode* node, struct scl_reg* input, void* closure)
{
	struct scl_closure* c = mem_alloc(sizeof(*c));
	c->node_to_exec = node;
	c->scl_executor = ctx;
	
	c->callgate_ctx = input->callgate_ctx;
	
	char* nodename = atree_get_subnode_attr(node, 1, "value");
	cpl_debug("registering cplop for %s\n", nodename);
	atree_caller_reg(input->data_tree_ctx, nodename, (run_atom_cb_t) data_cb, c);
	return 1;
}

int
symb(struct exec_descr* ctx, struct attnode* node, struct scl_input* input, void* closure)
{
	char* symb0 = atree_get_attr(node, "value");
	printf("%s", symb0);
	return 1;
}

int
spec(struct exec_descr* ctx, struct attnode* node, struct scl_input* input, void* closure)
{
	char* symb0 = atree_get_attr(node, "value");
	if (strspn(symb0, "0123456789")) {
		int n = atoi(symb0);
		return call_atree(input->data_tree_ctx,
					atree_subtree(input->node_to_process, n), input->external_ctx);
	}
	if (strspn(symb0, "$")) {
		printf("%s", atree_get_attr(input->node_to_process, "value"));
		return 1;
	}
	printf("%s", symb0);
	return 1;
}

