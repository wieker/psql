#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>

#include "ptpl2atree.h"
#include "wrxml.h"

#include "repl0.h"
#include "repl_kbd.h"
#include "gen_attr.h"

#include "stdbe.h"
#include "stde.h"
#include "stdnt.h"

struct agen_ctx {
	struct parser_context* ctx;
	struct tree_descr* internal_ex;
};

struct iotype {
	int fd;
	char* input;
};

static
void
cb_grammar(struct tree_descr* descr, struct tree_simple_linear* tree, struct parser_context* ctx_to_add_cpts,
																		struct tree_descr* executor_to_add_hooks)
{
	struct cpt_list* list = tree->cpts;
	struct agen_ctx* agen = mem_alloc(sizeof(*agen));
	
	agen->ctx = ctx_to_add_cpts;
	agen->internal_ex = executor_to_add_hooks;
	descr->curr_data = agen;
	
	while (list) {
		call_tree(list->cpt, descr);
		list = list->next;
	}
}

static
void *
cb_rule(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	call_tree(tree->cpts->cpt, descr);
	cpl_debug("reg rule %d %s\n", tree->cpts->cpt->type,
			parser_get_cpt_type(descr->pctx, tree->cpts->cpt->type)->cptname);
	return (void *) &EXEC_ERROR;
}

static
void *
cb_linear(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string*) list->cpt;
	struct cpt_list* child = list->next;
	int i = 0;
	
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(agen->ctx, name->string);
	while (child) {
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.name")->type) {
			gen_simple_linear_cpt_next(lc, ((struct tree_string*) child->cpt)->string);
		}
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.more_name")->type) {
			gen_simple_linear_cpt_next_more(lc,
				((struct tree_string*) ((struct tree_simple_linear*) child->cpt)->cpts->cpt)->string);
		}
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.token")->type) {
			char* sym = ((struct tree_string*) child->cpt)->string;
			char tok[100];
			snprintf(tok, 100, "%s%d", name->string, i);
			gen_simple_linear_token_next(lc, tok);
			stdt_lexer_one(agen->ctx, tok, sym);
		}
		child = child->next;
		i ++;
	}
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_linear);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_select(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string*) list->cpt;
	struct cpt_list* child = list->next;
	
	struct gen_simple_select_cpt* lc = gen_simple_select_cpt(agen->ctx, name->string);
	while (child) {
		gen_simple_select_cpt_next_select(lc, ((struct tree_string*) child->cpt)->string);
		child = child->next;
	}
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tone(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt;
	
	char* sym = child->string ;
	sym[strlen(sym)] = 0;
	stdt_lexer_one(agen->ctx, name->string, sym);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tone);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tnumber(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_number(agen->ctx,name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tnumber);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tstring(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_string(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_stdnt_quote(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdnt_quote(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_stde_integer(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stde_lexer_integer(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tinteger);
	
	return (void *) &EXEC_ERROR;
}

static void
register_grammar(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.grammar");
	gen_simple_linear_cpt_next_more(lc, "pgen.rule");
	stdt_lexer_one(ctx, "tok_semicolon", ";");
	executor_reg(executor, "pgen.grammar", (exec_cb) cb_grammar);
}

static void
register_rule(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lr = gen_simple_linear_cpt(ctx, "pgen.rule");
	gen_simple_linear_cpt_next(lr, "pgen.rules");
	struct gen_simple_select_cpt* lc = gen_simple_select_cpt(ctx, "pgen.rules");
	gen_simple_select_cpt_next_select(lc, "pgen.linear");
	gen_simple_select_cpt_next_select(lc, "pgen.select");
	gen_simple_select_cpt_next_select(lc, "pgen.tone");
	gen_simple_select_cpt_next_select(lc, "pgen.tstring");
	gen_simple_select_cpt_next_select(lc, "pgen.tnumber");
	gen_simple_select_cpt_next_select(lc, "pgen.stdntquote");
	gen_simple_select_cpt_next_select(lc, "pgen.integer");
	gen_simple_linear_token_next(lr, "tok_semicolon");
	executor_reg(executor, "pgen.rule", (exec_cb) cb_rule);
}

static void
register_linear(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.linear");
	gen_simple_linear_token_next(lc, "linear_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_token_next(lc, "def_tok");
	gen_simple_linear_cpt_next_more(lc, "pgen.symspec");
	stdt_lexer_one(ctx, "linear_tok", "linear");
	
	stdt_lexer_string(ctx, "pgen.name");
	stdt_lexer_one(ctx, "def_tok", ":=");
	
	executor_reg(executor, "pgen.linear", (exec_cb) cb_linear);
}

static void
register_symspec(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lcpt;
	struct gen_simple_select_cpt* lc = gen_simple_select_cpt(ctx, "pgen.symspec");
	
	gen_simple_select_cpt_next_select(lc, "pgen.name");
	
	gen_simple_select_cpt_next_select(lc, "pgen.more_name");
	lcpt = gen_simple_linear_cpt(ctx, "pgen.more_name");
	gen_simple_linear_token_next(lcpt, "tok_sq_br_left");
	gen_simple_linear_cpt_next(lcpt, "pgen.name");
	gen_simple_linear_token_next(lcpt, "tok_sq_br_right");
	stdt_lexer_one(ctx, "tok_sq_br_left", "[");
	stdt_lexer_one(ctx, "tok_sq_br_right", "]");
	
	gen_simple_select_cpt_next_select(lc, "pgen.token");
	
	//gen_simple_select_cpt_next_select(lc, "pgen.template");
	//gen_simple_select_cpt_next_select(lc, "pgen.subrule");
}

static void
register_select(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.select");
	gen_simple_linear_token_next(lc, "select_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_token_next(lc, "def_tok");
	
	gen_simple_linear_cpt_next_more(lc, "pgen.selelem");
	struct gen_simple_select_cpt* sc = gen_simple_select_cpt(ctx, "pgen.selelem");
	gen_simple_select_cpt_next_select(sc, "pgen.name");
	gen_simple_select_cpt_next_select(sc, "pgen.token");
	
	stdt_lexer_one(ctx, "select_tok", "select");
	executor_reg(executor, "pgen.select", (exec_cb) cb_select);
}

static void
register_tone(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.tone");
	gen_simple_linear_token_next(lc, "tone_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_token_next(lc, "def_tok");
	gen_simple_linear_cpt_next(lc, "pgen.token");
	stdt_lexer_one(ctx, "tone_tok", "tone");
	stdnt_quote(ctx, "pgen.token");
	executor_reg(executor, "pgen.tone", (exec_cb) cb_tone);
}

static void
register_tnumber(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.tnumber");
	gen_simple_linear_token_next(lc, "tnumber_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "tnumber_tok", "tnumber");
	executor_reg(executor, "pgen.tnumber", (exec_cb) cb_tnumber);
}

static void
register_tstring(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.tstring");
	gen_simple_linear_token_next(lc, "tstring_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "tstring_tok", "tstring");
	executor_reg(executor, "pgen.tstring", (exec_cb) cb_tstring);
}

static void
register_stdnt_quote(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.stdntquote");
	gen_simple_linear_token_next(lc, "stdnt_quote_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "stdnt_quote_tok", "dq");
	executor_reg(executor, "pgen.stdntquote", (exec_cb) cb_stdnt_quote);
}

static void
register_stde_integer(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.integer");
	gen_simple_linear_token_next(lc, "integer_tok");
	stdt_lexer_one(ctx, "integer_tok", "integer");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.integer", (exec_cb) cb_stde_integer);
}

void
add_cpts_portion(char* grammar, struct parser_context* ctx_to_add_cpts,
																		struct tree_descr* executor_to_add_hooks)
{
	struct parser_context* ctx = new_parser_with_io(new_repl_0(grammar));
	struct tree_descr* ex = tree_descr(ctx);
	
	register_grammar(ctx, ex);
	register_rule(ctx, ex);
	register_linear(ctx, ex);
	register_symspec(ctx, ex);
	register_select(ctx, ex);
	register_tone(ctx, ex);
	register_tstring(ctx, ex);
	register_tnumber(ctx, ex);
	register_stdnt_quote(ctx, ex);
	register_stde_integer(ctx, ex);
	
	struct gen_tree* tree = parser_parse(ctx, "pgen.grammar");
	
	cb_grammar(ex, (struct tree_simple_linear *) tree, ctx_to_add_cpts, executor_to_add_hooks);
	
	return;
}

struct attnode *
fixate_cpts(struct parser_context* ctx_to_add_cpts, struct tree_descr* executor_to_add_hooks, char* root)
{
	struct gen_tree* gtree = parser_parse(ctx_to_add_cpts, root);
	
	struct attnode* atttree = (struct attnode *) call_tree(gtree, executor_to_add_hooks);
	wrxml(atttree);
	
	return atttree;
}

