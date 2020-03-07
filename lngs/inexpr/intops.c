#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"

#include "intops.h"


void *
cb_expr(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_integer* result;
	
	if (! result_wait("type_integer", tree->left, descr, (struct cpt_result**) &result)) {
		return (void *) &EXEC_ERROR;
	} else {
		cpl_printf("result: %lf\n", result->v);
	}
	if (! result_wait("type_integer", tree->right, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	
	return (void *) result;
}


void *
cb_assign(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_integer* left,* right;
	struct cpt_list* l = tree->cpts;
		
	if (! result_wait("lvalue", l->cpt, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	if (! result_wait("type_integer", l->next->cpt, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	return (void *) right;
}

void *
cb_pow(struct tree_descr* descr, struct tree_op_right_cpt* tree)
{
	struct type_integer* left,* right,* result;
	double r;
	//cpl_printf("entering PROD\n");
	
	if (! result_wait("type_integer", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("type_integer", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "type_integer", sizeof(*result));
	
	r = right->v;
	result->v = pow(left->v, r);
	
	mem_free(left);
	mem_free(right);
	
	return result;
}


void *
cb_prod(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_integer* left,* right,* result;
	double r;
	//cpl_printf("entering PROD\n");
	
	if (! result_wait("type_integer", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("type_integer", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "type_integer", sizeof(*result));
	
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
	struct type_integer* left,* right,* result;
	double r;
	//cpl_printf("entering SUM\n");
	
	if (! result_wait("type_integer", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("type_integer", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "type_integer", sizeof(*result));
	
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
	struct type_integer* left,* right,* result;
	
	if (! result_wait("type_integer", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (! result_wait("type_integer", tree->right, descr, (struct cpt_result**) &right))
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
	struct type_integer* result = result_alloc_0(descr, "type_integer", sizeof(*result));
	if (! result_wait("type_integer", tree->cpts->cpt, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	
	
	return (void *) result;
}



static void
register_expr(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "expr", "assign", "comma");
	stdt_lexer_one(ctx, "comma", ",");
	executor_reg(executor, "expr", (exec_cb) cb_expr);
}

static void
register_assign(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "assign");
	gen_simple_select_cpt_next_select(st, "assign_real");
	gen_simple_select_cpt_next_select(st, "andor");
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "assign_real");
	gen_simple_linear_cpt_next(lc, "lvalue");
	gen_simple_linear_token_next(lc, "assign_tok");
	gen_simple_linear_cpt_next(lc, "andor");
	stdt_lexer_one(ctx, "assign_tok", "=");
	executor_reg(executor, "assign_real", (exec_cb) cb_assign);
}

static void
register_compare(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "int.compare", "sum", "compare_toks");
	gen_simple_select_cpt_next_select(parser_get_cpt(cxt, "bool.compare"), "and");
	executor_reg(executor, "int.compare", (exec_cb) cb_compare);
}

static void
register_sum(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "sum", "prod", "sum_toks");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "sum_toks");
	gen_simple_select_cpt_next_select(st, "plus");
	gen_simple_select_cpt_next_select(st, "minus");
	stdt_lexer_one(ctx, "plus", "+");
	stdt_lexer_one(ctx, "minus", "-");
	executor_reg(executor, "sum", (exec_cb) cb_sum);
}

static void
register_prod(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "prod", "pow", "prod_toks");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "prod_toks");
	gen_simple_select_cpt_next_select(st, "mul");
	gen_simple_select_cpt_next_select(st, "div");
	stdt_lexer_one(ctx, "mul", "*");
	stdt_lexer_one(ctx, "div", "/");
	executor_reg(executor, "prod", (exec_cb) cb_prod);
}

void
register_pow(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_right_cpt(ctx, "pow", "value", "pow_tok");
	stdt_lexer_one(ctx, "pow_tok", "^");
	executor_reg(executor, "pow", (exec_cb) cb_pow);
}

static void
register_value(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "value");
	gen_simple_select_cpt_next_select(st, "type_integer");
	gen_simple_select_cpt_next_select(st, "brackets");
	gen_simple_select_cpt_next_select(st, "rvalue");
}

static void
register_number(struct parser_context* ctx, struct tree_descr* executor)
{
	stdt_lexer_number(ctx, "number");
	executor_reg(executor, "number", (exec_cb) cb_number);
}

static void
register_brackets(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "brackets");
	gen_simple_linear_token_next(lc, "left_br");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "right_br");
	stdt_lexer_one(ctx, "left_br", "(");
	stdt_lexer_one(ctx, "right_br", ")");
	executor_reg(executor, "brackets", (exec_cb) cb_brackets);
}

void
testl_register_expr(struct parser_context* ctx, struct tree_descr* ex)
{
	register_expr(ctx, ex);
	register_assign(ctx, ex);
	register_andor(ctx, ex);
	register_not(ctx, ex);
	register_compare(ctx, ex);
	register_sum(ctx, ex);
	register_prod(ctx, ex);
	register_pow(ctx, ex);
	register_value(ctx, ex);
	register_number(ctx, ex);
	register_brackets(ctx, ex);
}
