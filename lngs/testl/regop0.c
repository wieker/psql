
#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec_expr0.h"
#include "exec_op0.h"

static void
register_op(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* st = gen_simple_select_cpt(ctx, "operator");
	gen_simple_select_cpt_next_select(st, "op_semicolon");
	gen_simple_select_cpt_next_select(st, "op_list");
	
	
	gen_simple_select_cpt_next_select(st, "op_simple");
	gen_simple_select_cpt_next_select(st, "op_elif");
	gen_simple_select_cpt_next_select(st, "op_if");
	gen_simple_select_cpt_next_select(st, "op_while");
	
	gen_simple_select_cpt_next_select(st, "op_break");
	gen_simple_select_cpt_next_select(st, "op_continue");
	gen_simple_select_cpt_next_select(st, "op_return");
	gen_simple_select_cpt_next_select(st, "op_return_void");
}

static void
register_semicolon(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_semicolon");
	gen_simple_linear_token_next(lc, "semicolon");
	stdt_lexer_one(ctx, "semicolon", ";");
	executor_reg(executor, "op_semicolon", (exec_cb) cb_op_semicolon);
}

static void
register_list(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_list");
	gen_simple_linear_token_next(lc, "left_fig_br");
	gen_simple_linear_cpt_next_more(lc, "operator");
	gen_simple_linear_token_next(lc, "right_fig_br");
	stdt_lexer_one(ctx, "left_fig_br", "{");
	stdt_lexer_one(ctx, "right_fig_br", "}");
	executor_reg(executor, "op_list", (exec_cb) cb_op_list);
}

static void
register_simple(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_simple");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "semicolon");
	executor_reg(executor, "op_simple", (exec_cb) cb_op_simple);
}

static void
register_elif(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_elif");
	gen_simple_linear_token_next(lc, "if");
	gen_simple_linear_token_next(lc, "left_br");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "right_br");
	gen_simple_linear_cpt_next(lc, "operator");
	gen_simple_linear_token_next(lc, "else");
	gen_simple_linear_cpt_next(lc, "operator");
	stdt_lexer_one(ctx, "if", "if");
	stdt_lexer_one(ctx, "else", "else");
	executor_reg(executor, "op_elif", (exec_cb) cb_op_elif);
}

static void
register_if(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_if");
	gen_simple_linear_token_next(lc, "if");
	gen_simple_linear_token_next(lc, "left_br");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "right_br");
	gen_simple_linear_cpt_next(lc, "operator");
	executor_reg(executor, "op_if", (exec_cb) cb_op_if);
}

static void
register_while(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_while");
	gen_simple_linear_token_next(lc, "while");
	gen_simple_linear_token_next(lc, "left_br");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "right_br");
	gen_simple_linear_cpt_next(lc, "operator");
	stdt_lexer_one(ctx, "while", "while");
	executor_reg(executor, "op_while", (exec_cb) cb_op_while);
}

static void
register_break(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_break");
	gen_simple_linear_token_next(lc, "break");
	gen_simple_linear_token_next(lc, "semicolon");
	stdt_lexer_one(ctx, "break", "break");
	executor_reg(executor, "op_break", (exec_cb) cb_op_break);
}

static void
register_continue(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_continue");
	gen_simple_linear_token_next(lc, "continue");
	gen_simple_linear_token_next(lc, "semicolon");
	stdt_lexer_one(ctx, "continue", "continue");
	executor_reg(executor, "op_continue", (exec_cb) cb_op_continue);
}

static void
register_return(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_return");
	gen_simple_linear_token_next(lc, "return");
	gen_simple_linear_cpt_next(lc, "expr");
	gen_simple_linear_token_next(lc, "semicolon");
	stdt_lexer_one(ctx, "return", "return");
	executor_reg(executor, "op_return", (exec_cb) cb_op_return);
}

static void
register_return_void(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "op_return_void");
	gen_simple_linear_token_next(lc, "return");
	gen_simple_linear_token_next(lc, "semicolon");
	executor_reg(executor, "op_return_void", (exec_cb) cb_op_return_void);
}

void
testl_register_op(struct parser_context* ctx, struct tree_descr* ex)
{
	register_op(ctx, ex);
	register_semicolon(ctx, ex);
	register_list(ctx, ex);
	register_simple(ctx, ex);
	register_elif(ctx, ex);
	register_if(ctx, ex);
	register_while(ctx, ex);
	register_break(ctx, ex);
	register_continue(ctx, ex);
	register_return(ctx, ex);
	register_return_void(ctx, ex);
}
