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

#include "table.h"

static struct type_cast {
	char* op;
	char* t1;
	char* t2;
	char* r;
} cast2[] = {
	{"sum", "integer", "integer", "integer"},
	{"sum", "integer", "float", "float"},
	{"sum", "float", "integer", "float"},
	{"sum", "float", "float", "float"},
	{"prod", "integer", "float", "float"},
	{"prod", "float", "integer", "float"},
	{"prod", "integer", "integer", "integer"},
	{"prod", "float", "float", "float"},
	{"operator", "voidOP", "voidOP", "voidOP"},
	{"brackets", "integer", "integer", "integer"},
	{"brackets", "float", "integer", "integer"},
	{"brackets", "integer", "float", "float"},
	{"brackets", "float", "float", "float"},
	{"compare", "float", "float", "integer"},
	{"compare", "float", "integer", "integer"},
	{"compare", "integer", "float", "integer"},
	{"compare", "integer", "integer", "integer"},
	{"logic", "integer", "integer", "integer"},
	{NULL, NULL, NULL, NULL},
};

struct implicit_type_convert {
	char* from;
	char* to;
};

int
type_assign_derive(struct scl_input* input)
{
	/*if (! call_atree(reg_type.data_tree_ctx, indp, NULL)
		return 0;*/
	char* value = atree_get_subnode_attr(input->node_to_process, 1, "type");
	if (! value)
		return 0;
	struct att_attr* attr_ = att_attr("type", "pstrz", value);
	attnode_add_attr(input->node_to_process, attr_);
	return 1;
}

int
type_assign_operation_one_or_two(struct scl_input* input, char* op)
{
	struct attnode* node = atree_subtree(input->node_to_process, 1);
	if (! node)
		return 0;
	int r = call_atree(input->data_tree_ctx,
					node, input->external_ctx);
	if (! r)
		return 0;
	char* value1 = atree_get_subnode_attr(input->node_to_process, 1, "type");
	if (! value1)
		return 0;
	
	char* value2;
	int cnode = 2;
	node = atree_subtree(input->node_to_process, cnode);
	while (node) {
		r = call_atree(input->data_tree_ctx,
					node, input->external_ctx);
		if (! r)
			return 0;
		value2 = atree_get_subnode_attr(input->node_to_process, cnode, "type");
		if (! value2)
			return 0;
		
		struct type_cast* cast = cast2;
		while (cast->op) {
			if (! strcmp(op, cast->op) && ! strcmp(value1, cast->t1) &&
					! strcmp(value2, cast->t2))
				break;
			cast ++;
		}
		
		if (! cast->r)
			return 0;
		
		value1 = cast->r;
		
		cnode ++;
		node = atree_subtree(input->node_to_process, cnode);
	}

	struct att_attr* attr_ = att_attr("type", "pstrz", value1);
	attnode_add_attr(input->node_to_process, attr_);
	
	return 1;
}

int
type_assign_look_name(struct scl_input* input)
{
	/*if (! call_atree(reg_type.data_tree_ctx, indp, NULL)
		return 0;*/
	char* varname = atree_get_attr(input->node_to_process, "value");
	char* vartype;
	
	if (! table_get_element((struct table *) input->external_ctx->typetable, varname, (void **) &vartype))
		return 0;
	
	struct att_attr* attr_ = att_attr("type", "pstrz", vartype);
	attnode_add_attr(input->node_to_process, attr_);
	return 1;
}

int
type_assign_look_def(struct scl_input* input)
{
	char* typename = atree_get_subnode_attr(input->node_to_process, 1, "value");
	char* varname  = atree_get_subnode_attr(input->node_to_process, 2, "value");
	table_set_element((struct table **) & (input->external_ctx->typetable), varname, typename);
	
	struct att_attr* attr_ = att_attr("type", "pstrz", "voidOP");
	attnode_add_attr(input->node_to_process, attr_);
	return 1;
}


