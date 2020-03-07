#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec_expr0.h"

#include "table.h"
#include "lvalue0.h"

#include <math.h>


void *
cb_expr(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_number* result;
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &result)) {
		return (void *) &EXEC_ERROR;
	} else {
		cpl_printf("result: %lf\n", result->v);
	}
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	
	return (void *) result;
}


void *
cb_assign(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_number* svd,* right;
	struct type_lvalue* lvalue;
	struct cpt_list* l = tree->cpts;
		
	if (! result_wait("lvalue", l->cpt, descr, (struct cpt_result**) &lvalue))
		return (void *) &EXEC_ERROR;
	if (! result_wait("number", l->next->cpt, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	if (table_get_element(variables, lvalue->name, (void **) &svd))
		*svd = *right;
	else
		table_set_element(&variables, lvalue->name, (void *) right);
	
	return (void *) right;
}


void *
cb_andor(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_number* left,* right;
	int op;
	
	if (tree->optype == parser_get_cpt(descr->pctx, "and")->type)
		op = 1;
	else
		op = 0;
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (op && left->v == 0) {
		left->v = 0;
		return left;
	}
	if (!op && left->v != 0) {
		left->v = 1;
		return left;
	}
	
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	if (right->v == 0) {
		left->v = 0;
		mem_free(right);
		return left;
	} else {
		left->v = 1;
		mem_free(right);
		return left;
	}
	
	return (void *) &EXEC_ERROR;
}


void *
cb_not(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_number* result;
	struct cpt_list* l = tree->cpts;
	int b = 0;
	
	while (l) {
		if (l->cpt->type == parser_get_cpt(descr->pctx, "not_tok")->type) {
			b += 1;
		} else
			if (! result_wait("number", l->cpt, descr, (struct cpt_result**) &result))
				return (void *) &EXEC_ERROR;
		l = l->next;
	}
	
	if (! b)
		return result;
	if (b % 2)
		result->v = ! result->v;
	else
		result->v = ! ! result->v;
	return result;
}


void *
cb_pow(struct tree_descr* descr, struct tree_op_right_cpt* tree)
{
	struct type_number* left,* right,* result;
	double r;
	//cpl_printf("entering PROD\n");
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "number", sizeof(*result));
	
	r = right->v;
	result->v = pow(left->v, r);
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


void *
cb_prod(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_number* left,* right,* result;
	double r;
	//cpl_printf("entering PROD\n");
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "number", sizeof(*result));
	
	r = right->v;
	if (tree->optype == parser_get_cpt(descr->pctx, "div")->type) {
		if (r == 0)
			return (void *) &EXEC_ERROR;
		r = 1 / r;
	}
	result->v = left->v * r;
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


void *
cb_sum(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_number* left,* right,* result;
	double r;
	//cpl_printf("entering SUM\n");
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "number", sizeof(*result));
	
	r = right->v;
	if (tree->optype == parser_get_cpt(descr->pctx, "minus")->type)
		r *= -1;
	result->v = left->v + r;
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


void *
cb_number(struct tree_descr* descr, struct tree_number* tree)
{
	struct type_number* result = result_alloc_0(descr, "number", sizeof(*result));
	result->v = tree->number;
	
	return result;
}


void *
cb_compare(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_number* left,* right,* result;
	
	if (! result_wait("number", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("number", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;

	result = result_alloc_0(descr, "number", sizeof(*result));

	if (tree->optype == parser_get_cpt(descr->pctx, "less")->type)
		result->v = (left->v < right->v);
	else if (tree->optype == parser_get_cpt(descr->pctx, "more")->type)
		result->v = (left->v > right->v);
	else
		result->v = (left->v == right->v);
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


void *
cb_brackets(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_number* result = result_alloc_0(descr, "number", sizeof(*result));
	if (! result_wait("number", tree->cpts->cpt, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	
	
	return (void *) result;
}

