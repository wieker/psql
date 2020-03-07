
#include "boolops.h"

static
void *
cb_andor(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct type_bool* left,* right;
	int op;
	
	if (tree->optype == parser_get_cpt(descr->pctx, "and")->type)
		op = 1;
	else
		op = 0;
	
	if (! result_wait_type("type_bool", tree->left, descr, (struct cpt_result**) &left))
		return (void *) &EXEC_ERROR;
	
	if (op && left->b == 0) {
		left->b = 0;
		return left;
	}
	if (!op && left->b != 0) {
		left->b = 1;
		return left;
	}
	
	if (! result_wait_type("type_bool", tree->right, descr, (struct cpt_result**) &right))
		return (void *) &EXEC_ERROR;
	
	if (right->b == 0) {
		left->b = 0;
		mem_free(right);
		return left;
	} else {
		left->b = 1;
		mem_free(right);
		return left;
	}
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_not(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_bool* result;
	struct cpt_list* l = tree->cpts;
	int b = 0;
	
	while (l) {
		if (l->cpt->type == parser_get_cpt(descr->pctx, "not_tok")->type) {
			b += 1;
		} else
			if (! result_wait_type("type_bool", l->cpt, descr, (struct cpt_result**) &result))
				return (void *) &EXEC_ERROR;
		l = l->next;
	}
	
	if (! b)
		return result;
	if (b % 2)
		result->b = ! result->b;
	return result;
}



static void *
bool_integer(struct tree_descr* descr, struct type_integer* i)
{
	return (void *) i;
}

static void
reg_type(struct parser_context* ctx, struct tree_descr* ex)
{
	executor_reg_type(executor, "type_bool");
	executor_reg_cast(executor, "type_bool", "type_integer", (exec_cb) bool_integer);
}

static void
register_andor(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "bool.andor", "bool.not", "andor_toks");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "andor_toks");
	gen_simple_select_cpt_next_select(st, "and");
	gen_simple_select_cpt_next_select(st, "or");
	stdt_lexer_one(ctx, "and", "and");
	stdt_lexer_one(ctx, "or", "or");
	executor_reg(executor, "bool.andor", (exec_cb) cb_andor);
}

static void
register_not(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "bool.not");
	gen_simple_linear_cpt_next_more(lc, "not_tok");
	gen_simple_linear_cpt_next(lc, "bool.compare");
	stdt_lexer_one(ctx, "not_tok", "not");
	executor_reg(executor, "bool.not", (exec_cb) cb_not);
}

static void
register_compare(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* st = gen_simple_linear_cpt(ctx, "bool.selector");
	gen_simple_linear_token_next(st, "bool.selector.token.open");
	gen_simple_linear_token_next(st, "bool.selector.token.close");
	gen_simple_linear_cpt_next(st, "andor");
	stdt_lexer_one(ctx, "bool.selector.token.open", "$$$lang.bool");
	stdt_lexer_one(ctx, "bool.selector.token.close", "lang.bool$$$");
}

static void
register_compare(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "bool.compare");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "compare_toks");
	gen_simple_select_cpt_next_select(st, "less");
	gen_simple_select_cpt_next_select(st, "more");
	gen_simple_select_cpt_next_select(st, "eq");
	stdt_lexer_one(ctx, "less", "<");
	stdt_lexer_one(ctx, "more", ">");
	stdt_lexer_one(ctx, "eq", "==");
}

void
register_boolops(struct parser_context* ctx, struct tree_descr* ex)
{
	register_andor(ctx, ex);
	register_not(ctx, ex);
	register_compare(ctx, ex);
	register_selector(ctx, ex);
}

