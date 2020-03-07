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

struct agen_ctx {
	struct parser_context* ctx;
	struct tree_descr* internal_ex;
};

struct iotype {
	int fd;
	char* input;
};

static
struct attnode *
cb_grammar(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct agen_ctx* agen = mem_alloc(sizeof(*agen));
	struct iotype* iotype = (struct iotype*)descr->curr_data;
	if (! iotype->input)
		agen->ctx = new_parser_with_io((new_repl_kbd(iotype->fd)));
	else
		agen->ctx = new_parser_with_io((new_repl_0(iotype->input)));
	agen->internal_ex = tree_descr(agen->ctx);
	descr->curr_data = agen;
	
	while (list) {
		call_tree(list->cpt, descr);
		list = list->next;
	}
	
	struct gen_tree* gtree = parser_parse(agen->ctx, "root");
	
	struct attnode* atttree = (struct attnode *) call_tree(gtree, agen->internal_ex);
	wrxml(atttree);
	
	return atttree;
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
cb_lop(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt,
					* op = (struct tree_string *) list->next->next->cpt;
	
	stdp_op_left_cpt(agen->ctx, name->string, child->string, op->string);
	//printf("registering %s, %s, %s\n", name->string, child->string, op->string);
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_lop);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_rop(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt,
					* op = (struct tree_string *) list->next->next->cpt;
	
	stdp_op_right_cpt(agen->ctx, name->string, child->string, op->string);
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_rop);
	
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
			char* sym = ((struct tree_string*) child->cpt)->string + 1;
			sym[strlen(sym) - 1] = 0;
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
	
	char* sym = child->string + 1;
	sym[strlen(sym) - 1] = 0;
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
cb_quoted(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_quoted_string(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tendl(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_eol(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tone);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tendf(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_eof(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tone);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_negative(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	struct tree_string* negative = (struct tree_string *) list->next->cpt;
	
	gen_negative_cpt(agen->ctx, name->string, negative->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tone);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_specq(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_quoted_string* delimiter = (struct tree_quoted_string *) list->cpt;
	struct tree_string* name = (struct tree_string *) list->next->cpt;
	
	stdt_lexer_specq(agen->ctx, delimiter->string, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_notsymb(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_quoted_string* delimiter = (struct tree_quoted_string *) list->cpt;
	struct tree_string* name = (struct tree_string *) list->next->cpt;
	
	stdt_lexer_notsymb(agen->ctx, delimiter->string, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_space(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stdt_lexer_space(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tone);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_stdbe_simple(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_quoted_string* begin = (struct tree_quoted_string *) list->cpt;
	struct tree_quoted_string* end = (struct tree_quoted_string *) list->next->cpt;
	struct tree_string* name = (struct tree_string *) list->next->next->cpt;
	
	stdbe_simple(agen->ctx, begin->string, end->string, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_stde_onenotsymb(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_quoted_string* exception = (struct tree_quoted_string *) list->cpt;
	struct tree_string* name = (struct tree_string *) list->next->cpt;
	
	stde_lexer_onenotsymb(agen->ctx, exception->string, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tchar);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_stde_escsymb(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_quoted_string* exception = (struct tree_quoted_string *) list->cpt;
	struct tree_string* name = (struct tree_string *) list->next->cpt;
	
	stde_lexer_backslashsymb(agen->ctx, exception->string, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tchar);
	
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

static
void *
cb_stde_cname(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct agen_ctx* agen = (struct agen_ctx*) descr->curr_data;
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	stde_lexer_cname(agen->ctx, name->string);
	
	executor_reg(agen->internal_ex, name->string, (exec_cb) cb_exec_tstring);
	
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
	gen_simple_select_cpt_next_select(lc, "pgen.lop");
	gen_simple_select_cpt_next_select(lc, "pgen.rop");
	gen_simple_select_cpt_next_select(lc, "pgen.linear");
	gen_simple_select_cpt_next_select(lc, "pgen.select");
	gen_simple_select_cpt_next_select(lc, "pgen.tone");
	gen_simple_select_cpt_next_select(lc, "pgen.tstring");
	gen_simple_select_cpt_next_select(lc, "pgen.tnumber");
	gen_simple_select_cpt_next_select(lc, "pgen.quoted");
	gen_simple_select_cpt_next_select(lc, "pgen.tendl");
	gen_simple_select_cpt_next_select(lc, "pgen.tendf");
	gen_simple_select_cpt_next_select(lc, "pgen.negative");
	gen_simple_select_cpt_next_select(lc, "pgen.specq");
	gen_simple_select_cpt_next_select(lc, "pgen.notsymb");
	gen_simple_select_cpt_next_select(lc, "pgen.space");
	gen_simple_select_cpt_next_select(lc, "pgen.stdbe");
	gen_simple_select_cpt_next_select(lc, "pgen.onenotsymb");
	gen_simple_select_cpt_next_select(lc, "pgen.escsymb");
	gen_simple_select_cpt_next_select(lc, "pgen.integer");
	gen_simple_select_cpt_next_select(lc, "pgen.cname");
	gen_simple_linear_token_next(lr, "tok_semicolon");
	executor_reg(executor, "pgen.rule", (exec_cb) cb_rule);
}

static void
register_lop(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.lop");
	gen_simple_linear_token_next(lc, "left_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_token_next(lc, "def_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "left_tok", "left");
	stdt_lexer_one(ctx, "def_tok", ":=");
	stdt_lexer_string(ctx, "pgen.name");
	executor_reg(executor, "pgen.lop", (exec_cb) cb_lop);
}

static void
register_rop(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.rop");
	gen_simple_linear_token_next(lc, "right_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_token_next(lc, "def_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "right_tok", "right");
	executor_reg(executor, "pgen.rop", (exec_cb) cb_rop);
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
	stdt_lexer_quoted_string(ctx, "pgen.token");
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
register_quoted(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.quoted");
	gen_simple_linear_token_next(lc, "quoted_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "quoted_tok", "quoted");
	executor_reg(executor, "pgen.quoted", (exec_cb) cb_quoted);
}

static void
register_tendl(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.tendl");
	gen_simple_linear_token_next(lc, "tendl_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "tendl_tok", "tendl");
	executor_reg(executor, "pgen.tendl", (exec_cb) cb_tendl);
}

static void
register_tendf(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.tendf");
	gen_simple_linear_token_next(lc, "tendf_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "tendf_tok", "tendf");
	executor_reg(executor, "pgen.tendf", (exec_cb) cb_tendf);
}

static void
register_negative(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.negative");
	gen_simple_linear_token_next(lc, "negative_tok");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	stdt_lexer_one(ctx, "negative_tok", "negative");
	executor_reg(executor, "pgen.negative", (exec_cb) cb_negative);
}

static void
register_specq(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.specq");
	gen_simple_linear_token_next(lc, "specq_tok");
	stdt_lexer_one(ctx, "specq_tok", "specq");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter");
	stdt_lexer_quoted_string(ctx, "pgen.delimiter");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.specq", (exec_cb) cb_specq);
}

static void
register_notsymb(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.notsymb");
	gen_simple_linear_token_next(lc, "notsymb_tok");
	stdt_lexer_one(ctx, "notsymb_tok", "notsymb");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter_quoted");
	stdt_lexer_specq(ctx, "\"\"\"", "pgen.delimiter_quoted");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.notsymb", (exec_cb) cb_notsymb);
}

static void
register_space(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.space");
	gen_simple_linear_token_next(lc, "space_tok");
	stdt_lexer_one(ctx, "space_tok", "space");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.space", (exec_cb) cb_space);
}

static void
register_stdbe_simple(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.stdbe");
	gen_simple_linear_token_next(lc, "stdbe_tok");
	stdt_lexer_one(ctx, "stdbe_tok", "stdbe");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter_quoted");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter_quoted");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.stdbe", (exec_cb) cb_stdbe_simple);
}

static void
register_stde_onenotsymb(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.onenotsymb");
	gen_simple_linear_token_next(lc, "onenotsymb_tok");
	stdt_lexer_one(ctx, "onenotsymb_tok", "onenotsymb");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter_quoted");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.onenotsymb", (exec_cb) cb_stde_onenotsymb);
}

static void
register_stde_escsymb(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.escsymb");
	gen_simple_linear_token_next(lc, "escsymb_tok");
	stdt_lexer_one(ctx, "escsymb_tok", "escsymb");
	gen_simple_linear_cpt_next(lc, "pgen.delimiter_quoted");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.escsymb", (exec_cb) cb_stde_escsymb);
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

static void
register_stde_cname(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "pgen.cname");
	gen_simple_linear_token_next(lc, "cname_tok");
	stdt_lexer_one(ctx, "cname_tok", "cname");
	gen_simple_linear_cpt_next(lc, "pgen.name");
	executor_reg(executor, "pgen.cname", (exec_cb) cb_stde_cname);
}

struct attnode *
generate_att_tree(char* grammar, char* input, int fd)
{
	struct parser_context* ctx = new_parser_with_io(new_repl_0(grammar));
	struct tree_descr* ex = tree_descr(ctx);
	struct iotype* iotype = mem_alloc(sizeof(*iotype));
	ex->curr_data = iotype;
	iotype->input = input;
	iotype->fd = fd;
	
	register_grammar(ctx, ex);
	register_rule(ctx, ex);
	register_lop(ctx, ex);
	register_rop(ctx, ex);
	register_linear(ctx, ex);
	register_symspec(ctx, ex);
	register_select(ctx, ex);
	register_tone(ctx, ex);
	register_tstring(ctx, ex);
	register_tnumber(ctx, ex);
	register_quoted(ctx, ex);
	register_tendl(ctx, ex);
	register_tendf(ctx, ex);
	register_negative(ctx, ex);
	register_specq(ctx, ex);
	register_notsymb(ctx, ex);
	register_space(ctx, ex);
	register_stdbe_simple(ctx, ex);
	register_stde_onenotsymb(ctx, ex);
	register_stde_escsymb(ctx, ex);
	register_stde_integer(ctx, ex);
	register_stde_cname(ctx, ex);
	
	
	struct gen_tree* tree = parser_parse(ctx, "pgen.grammar");
	
	return (struct attnode *) call_tree(tree, ex);
}

