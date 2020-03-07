#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec0.h"


struct exec_result *
cb_sum(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct exec_result* left,* right,* result;
	double r;
	cpl_printf("entering SUM\n");
	
	left = (struct exec_result *) call_tree(tree->left, descr);
	if (left->base.type != 
			parser_get_cpt(descr->pctx, "number")->type)
		return (struct exec_result *) &EXEC_ERROR;
	
	right = (struct exec_result *) call_tree(tree->right, descr);
	if (right->base.type != 
			parser_get_cpt(descr->pctx, "number")->type)
		return (struct exec_result *) &EXEC_ERROR;
	
	result = mem_alloc(sizeof(*result));
	result->base.type = parser_get_cpt(descr->pctx, "number")->type;
	result->base.size = 0;
	r = right->v;
	if (tree->optype == parser_get_cpt(descr->pctx, "minus")->type)
		r *= -1;
	result->v = left->v + r;
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


struct exec_result *
cb_number(struct tree_descr* descr, struct tree_number* tree)
{
	struct exec_result* result;
	cpl_printf("entering NUMBER\n");
	
	result = mem_alloc(sizeof(*result));
	result->base.type = parser_get_cpt(descr->pctx, "number")->type;
	result->v = tree->number;
	
	return result;
}
