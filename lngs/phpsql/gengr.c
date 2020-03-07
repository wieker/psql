#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"

static void
register_object(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "object");
	gen_simple_linear_cpt_next(lc, "objname");
	gen_simple_linear_token_next(lc, "object1");
	stdt_lexer_one(ctx, "object1", "{");
	gen_simple_linear_cpt_next(lc, "objfield");
	gen_simple_linear_token_next(lc, "object3");
	stdt_lexer_one(ctx, "object3", "}");
	executor_reg(executor, "object", (exec_cb) cb_object);
}

static void
register_objfield(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_select_cpt* lc = gen_simple_select_cpt(ctx, "objfield");
	gen_simple_select_cpt_next_select(lc, "scalar");
	gen_simple_select_cpt_next_select(lc, "morelink");
	gen_simple_select_cpt_next_select(lc, "onelink");
}

static void
register_morelink(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "morelink");
	gen_simple_linear_cpt_next(lc, "objname");
	gen_simple_linear_token_next(lc, "morelink1");
	stdt_lexer_one(ctx, "morelink1", "[");
	gen_simple_linear_token_next(lc, "morelink2");
	stdt_lexer_one(ctx, "morelink2", "]");
	executor_reg(executor, "morelink", (exec_cb) cb_morelink);
}

static void
register_onelink(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "onelink");
	gen_simple_linear_cpt_next(lc, "objname");
	executor_reg(executor, "onelink", (exec_cb) cb_onelink);
}

static void
register_scalar(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "scalar");
	gen_simple_linear_token_next(lc, "scalar0");
	stdt_lexer_one(ctx, "scalar0", "(");
	gen_simple_linear_cpt_next(lc, "name");
	gen_simple_linear_cpt_next(lc, "type_name");
	gen_simple_linear_token_next(lc, "scalar3");
	stdt_lexer_one(ctx, "scalar3", ")");
	executor_reg(executor, "scalar", (exec_cb) cb_scalar);
}

static void
register_page(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "page");
	gen_simple_linear_cpt_next(lc, "page_name");
	gen_simple_linear_cpt_next(lc, "objname");
	gen_simple_linear_token_next(lc, "page2");
	stdt_lexer_one(ctx, "page2", ".");
	gen_simple_linear_cpt_next(lc, "action");
	executor_reg(executor, "page", (exec_cb) cb_page);
}

static void
register_action(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "action");
	gen_simple_linear_cpt_next(lc, "action_name");
	gen_simple_linear_token_next(lc, "action1");
	stdt_lexer_one(ctx, "action1", "(");
	gen_simple_linear_cpt_next_more(lc, "objfield");
	gen_simple_linear_token_next(lc, "action3");
	stdt_lexer_one(ctx, "action3", ")");
	executor_reg(executor, "action", (exec_cb) cb_action);
}

static void
register_name(struct parser_context* ctx, struct tree_descr* executor)
{
	stdt_lexer_string(ctx, "name");
}

static void
register_objname(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "objname");
	gen_simple_linear_cpt_next(lc, "name");
}

static void
register_type_name(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "type_name");
	gen_simple_linear_cpt_next(lc, "name");
}

static void
register_page_name(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "page_name");
	gen_simple_linear_cpt_next(lc, "name");
}

static void
register_action_name(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "action_name");
	gen_simple_linear_cpt_next(lc, "name");
}

