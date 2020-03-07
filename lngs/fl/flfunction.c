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

static
struct fl_function *
calculate_function(struct exec_descr* ctx, struct attnode* node, struct fl_context* input)
{
	struct attnode* function_tree = atree_subtree(node, 1);
	if (! call_atree(ctx, function_tree, input))
		return NULL;
	if (input->result.fl_type != FL_FUNCTION)
		return NULL;
	return input->result.function;
}

static
int
compare_arg_count(struct fl_function * func, struct attnode* node)
{
	struct list_iter iter = list_iter(node->subtrees);
	int n = 0;
	NEXT_EL(& iter);
	while (CONT(& iter)) {
		n ++;
		NEXT_EL(& iter);
	}
	return (n == (func->argcount - func->closed));
}

static
int
calculate_args(struct fl_function * func, struct exec_descr* ctx, struct attnode* node, struct fl_context* input)
{
	struct list_iter iter_args_trees = list_iter(node->subtrees);
	NEXT_EL(& iter_args_trees);
	struct list_iter iter_args_decls = list_iter(func->body->subtrees);
	NEXT_EL(& iter_args_decls);
	
	struct attnode* arg_tree;
	union fl_result* arg_value;
	char* argname;
	
	while (CONT(& iter_args_trees)) {
		argname = atree_get_attr(
					(struct attnode *) GET_EL(& iter_args_decls),
					"value");
		
		arg_tree = (struct attnode *) GET_EL(& iter_args_trees);
		if (! call_atree(ctx, arg_tree, input))
			return 0;
		arg_value = mem_alloc(sizeof(*arg_value));
		*arg_value = input->result;
		
		table_set_element(& func->param, argname, arg_value);
		
		NEXT_EL(& iter_args_decls);
		NEXT_EL(& iter_args_trees);
	}
	return 1;
}

int
funcall(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	struct fl_function *to_call;
	struct fl_function *stack;
	if (! (to_call = calculate_function(ctx, node, input)))
		return 0;
	if (! compare_arg_count(to_call, node))
		return 0;
	if (! calculate_args(to_call, ctx, node, input))
		return 0;
	/*input->result.fl_type = FL_FUNCTION;
	input->result.function = to_call;*/
	stack = input->closure;
	input->closure = to_call;
	if (! call_atree(ctx, to_call->expr, input))
		return 0;
	input->closure = stack;
	return 1;
}

static
struct fl_function *
dup_func(struct fl_function *f)
{
	struct fl_function *rt = mem_alloc(sizeof(*rt));
	
	*rt = *f;
	rt->param = NULL;
	struct table* tbl = f->param;
	while (tbl) {
		table_set_element(& rt->param, tbl->key, tbl->element);
		tbl = tbl->next;
	}
	
	return rt;
}

int
name(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	char* name = atree_get_attr(node, "value");
	union fl_result* value = NULL;
	cpl_debug("Nme request:: %s\n", name);
	table_get_element(input->closure->param, name, (void **) & value);
	if (value) {
		input->result = *value;
		cpl_debug("Var type: %d\n", input->result.fl_type);
			if (input->result.fl_type == FL_FUNCTION)
				input->result.function = dup_func(input->result.function);
		return 1;
	}
	struct fl_function* cl = input->closure;
	while (cl->closure) {
		cl = cl->closure;
		table_get_element(input->closure->param, name, (void **) & value);
		if (value) {
			input->result = *value;
			if (input->result.fl_type == FL_FUNCTION)
				input->result.function = dup_func(input->result.function);
			return 1;
		}
	}
	cpl_debug("Search name in:: %p\n", input->preglobal);
	table_get_element(input->preglobal, name, (void **) & cl);
	if (cl) {
		union fl_result r;
		r.fl_type = FL_FUNCTION;
		r.function = dup_func(cl);
		input->result = r;
		return 1;
	}
	return 0;
}

int
lfunction(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	struct fl_function* new = dup_func((struct fl_function*) atree_get_attr(node, "function"));
	new->closure = input->closure;
	input->result.fl_type = FL_FUNCTION;
	input->result.function = new;
	return 1;
}
