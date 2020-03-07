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

void static
free_result(struct fl_context* input)
{
	
}

int
intr(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	char* value = atree_get_attr(node, "value");
	int q = atoi(value);
	input->result.fl_type = FL_INTEGER;
	input->result.integer = q;
	return 1;
}

int
lstring(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	char* value = atree_get_attr(node, "value");
	input->result.fl_type = FL_STRING;
	input->result.str = value;
	return 1;
}

int
concat(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	char* str1,* str2;
	int l, l1;
	char* r;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_STRING)
		return 0;
	str1 = input->result.str;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_STRING)
		return 0;
	str2 = input->result.str;
	
	l = (l1 = strlen(str1)) + strlen(str2);
	r = mem_alloc(l + 1);
	strcpy(r, str1);
	strcpy(r + l1, str2);
	
	input->result.str = r;
	
	return 1;
}

int
plus(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	int i1;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	i1 = input->result.integer;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	
	input->result.integer += i1;
	
	return 1;
}

int
minus(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	int i1;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	i1 = input->result.integer;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	
	input->result.integer = i1 - input->result.integer;
	
	return 1;
}

int
less(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	int i1;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	i1 = input->result.integer;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	
	input->result.integer = i1 < input->result.integer;
	
	return 1;
}

int
more(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	int i1;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	i1 = input->result.integer;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	
	input->result.integer = i1 > input->result.integer;
	
	return 1;
}

int
equal(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	int i1;
	
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	i1 = input->result.integer;
	if (! call_atree(ctx, atree_subtree(node, 2), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	
	input->result.integer = i1 == input->result.integer;
	
	return 1;
}

int
statement(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type == FL_INTEGER) {
		printf("integer: %d\n", input->result.integer);
		free_result(input);
	} else if (input->result.fl_type == FL_STRING) {
		printf("string: %s\n", input->result.str);
		free_result(input);
	}
	
	input->result.fl_type = FL_VOID;
	return 1;
}

int
tuplec(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	input->result.fl_type = FL_TUPLE;
	input->result.head = atree_subtree(node, 1);
	input->result.tail = atree_subtree(node, 2);
	input->result.hc = input->closure;
	input->result.tc = input->closure;
	
	return 1;
}

int
tupleh(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type == FL_FUNCTION) {
		if (! try_cast(ctx, node, input, & input->result))
			return 0;
	}
	if (input->result.fl_type != FL_TUPLE)
		return 0;
	struct fl_function* stack;
	stack = input->closure;
	input->closure = input->result.hc;
	if (! call_atree(ctx, input->result.head, input))
		return 0;
	input->closure = stack;
	
	return 1;
}

int
tuplet(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type == FL_FUNCTION) {
		if (! try_cast(ctx, node, input, & input->result))
			return 0;
	}
	if (input->result.fl_type != FL_TUPLE)
		return 0;
	struct fl_function* stack;
	stack = input->closure;
	input->closure = input->result.hc;
	if (! call_atree(ctx, input->result.tail, input))
		return 0;
	input->closure = stack;
	
	return 1;
}



int
ifop(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure)
{
	if (! call_atree(ctx, atree_subtree(node, 1), input))
		return 0;
	if (input->result.fl_type != FL_INTEGER)
		return 0;
	if (input->result.integer) {
		if (! call_atree(ctx, atree_subtree(node, 2), input))
			return 0;
	} else
		if (! call_atree(ctx, atree_subtree(node, 3), input))
			return 0;
	
	return 1;
}
