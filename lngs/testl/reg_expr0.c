#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec0.h"

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
register_andor(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "andor", "not", "andor_toks");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "andor_toks");
	gen_simple_select_cpt_next_select(st, "and");
	gen_simple_select_cpt_next_select(st, "or");
	stdt_lexer_one(ctx, "and", "and");
	stdt_lexer_one(ctx, "or", "or");
	executor_reg(executor, "andor", (exec_cb) cb_andor);
}

static void
register_not(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "not");
	gen_simple_linear_cpt_next_more(lc, "not_tok");
	gen_simple_linear_cpt_next(lc, "compare");
	stdt_lexer_one(ctx, "not_tok", "not");
	executor_reg(executor, "not", (exec_cb) cb_not);
}

static void
register_compare(struct parser_context* ctx, struct tree_descr* executor)
{
	stdp_op_left_cpt(ctx, "compare", "sum", "compare_toks");
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "compare_toks");
	gen_simple_select_cpt_next_select(st, "less");
	gen_simple_select_cpt_next_select(st, "more");
	gen_simple_select_cpt_next_select(st, "eq");
	stdt_lexer_one(ctx, "less", "<");
	stdt_lexer_one(ctx, "more", ">");
	stdt_lexer_one(ctx, "eq", "==");
	executor_reg(executor, "compare", (exec_cb) cb_compare);
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
	gen_simple_select_cpt_next_select(st, "func_one");
	gen_simple_select_cpt_next_select(st, "number");
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
