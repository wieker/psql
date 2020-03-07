#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "fl.h"
#include "scalc.h"

#include "table.h"

int
defun_global(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	struct attnode* body = atree_subtree(node, 2);
	char* name = atree_get_subnode_attr(body, 1, "value");
	int argcount = 0;
	struct list_iter iter = list_iter(body->subtrees);
	NEXT_EL(& iter);
	while (! strcmp(atree_get_attr(GET_EL(& iter), "name"), "name")) {
		NEXT_EL(& iter);
		argcount ++;
	}
	while (strcmp(atree_get_attr(GET_EL(& iter), "name"), "expression"))
		NEXT_EL(& iter);
	
	struct fl_function* func = mem_alloc(sizeof(*func));
	func->body = body;
	func->expr = GET_EL(& iter);
	//func->global = input->preglobal;
	func->closure = NULL;
	func->local = NULL;
	func->param = NULL;
	func->fl_function_type = FL_TEMPLATE;
	func->argcount = argcount;
	
	struct att_attr* attr = att_attr("function", "FL_func", func);
	attnode_add_attr(GET_EL(& iter), attr);
	
	iter = list_iter(body->subtrees);
	NEXT_EL(& iter);
	while (! strcmp(atree_get_attr(GET_EL(& iter), "name"), "name")) {
		table_set_element(& func->param, atree_get_attr(GET_EL(& iter), "value"), NULL);
		NEXT_EL(& iter);
	}
	
	table_set_element(& input->preglobal, name, func);
	
	input->closure = func;
	return go_deep_scalc(ctx, node, input, closure);
}

int
lfunction_global(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	struct attnode* body = node;
	int argcount = 0;
	struct list_iter iter = list_iter(node->subtrees);
	if (strcmp(atree_get_attr(GET_EL(& iter), "name"), "expression")) {
		NEXT_EL(& iter);
	}
	while (! strcmp(atree_get_attr(GET_EL(& iter), "name"), "name")) {
		NEXT_EL(& iter);
		argcount ++;
	}
	while (strcmp(atree_get_attr(GET_EL(& iter), "name"), "expression"))
		NEXT_EL(& iter);
	
	struct fl_function* func = mem_alloc(sizeof(*func));
	func->body = body;
	func->expr = GET_EL(& iter);
	//func->global = input->preglobal;
	func->closure = input->closure;
	func->local = NULL;
	func->param = NULL;
	func->fl_function_type = FL_TEMPLATE;
	func->argcount = argcount;
	
	struct att_attr* attr = att_attr("function", "FL_func", func);
	attnode_add_attr(node, attr);
	
	iter = list_iter(node->subtrees);
	if (strcmp(atree_get_attr(GET_EL(& iter), "name"), "expression")) {
		NEXT_EL(& iter);
	}
	while (! strcmp(atree_get_attr(GET_EL(& iter), "name"), "name")) {
		table_set_element(& func->param, atree_get_attr(GET_EL(& iter), "value"), NULL);
		NEXT_EL(& iter);
	}
	
	input->closure = func;
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	input->closure = func->closure;
	return 1;
}

int
defun_exec(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	return 1;
}
