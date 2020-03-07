#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"

static
void *
cb_grammar(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	
	cpl_printf(
		"#include \"register.h\"\n"
		"#include \"stdp.h\"\n"
		"#include \"stdt.h\"\n"
		"#include \"stdo.h\"\n"
		"#include \"parser.h\"\n"
		"#include \"exec_tree.h\"\n"
		"#include \"utils.h\"\n\n"
	);
	
	while (list) {
		call_tree(list->cpt, descr);
		list = list->next;
	}
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_rule(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	call_tree(tree->cpts->cpt, descr);
	return (void *) &EXEC_ERROR;
}

static
void *
cb_lop(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt,
					* op = (struct tree_string *) list->next->next->cpt;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	stdp_op_left_cpt(ctx, \"%s\", \"%s\", \"%s\");\n"
		"}\n\n",
		name->string, name->string, child->string, op->string
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_rop(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt,
					* op = (struct tree_string *) list->next->next->cpt;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	stdp_op_right_cpt(ctx, \"%s\", \"%s\", \"%s\");\n"
		"}\n\n",
		name->string, name->string, child->string, op->string
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_linear(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string*) list->cpt;
	struct cpt_list* child = list->next;
	int i = 0;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, \"%s\");\n",
		name->string, name->string
	);
	while (child) {
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.name")->type) {
			cpl_printf(
			"	gen_simple_linear_cpt_next(lc, \"%s\");\n",
				((struct tree_string*) child->cpt)->string
			);
		}
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.more_name")->type) {
			cpl_printf(
			"	gen_simple_linear_cpt_next_more(lc, \"%s\");\n",
				((struct tree_string*) ((struct tree_simple_linear*) child->cpt)->cpts->cpt)->string
			);
		}
		if (child->cpt->type == parser_get_cpt(descr->pctx, "pgen.token")->type) {
			cpl_printf(
			"	gen_simple_linear_token_next(lc, \"%s%d\");\n"
			"	stdt_lexer_one(ctx, \"%s%d\", %s);\n",
				name->string, i, name->string, i,
				((struct tree_string*) child->cpt)->string
			);
		}
		child = child->next;
		i ++;
	}
	cpl_printf(
		"}\n\n"
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_select(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string*) list->cpt;
	struct cpt_list* child = list->next;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	struct gen_simple_select_cpt* lc = gen_simple_select_cpt(ctx, \"%s\");\n",
		name->string, name->string
	);
	while (child) {
		cpl_printf(
		"	gen_simple_select_cpt_next_select(lc, \"%s\");\n",
			((struct tree_string*) child->cpt)->string
		);
		child = child->next;
	}
	cpl_printf(
		"}\n\n"
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tone(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt,
					* child = (struct tree_string *) list->next->cpt;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	stdt_lexer_one(ctx, \"%s\", %s);\n"
		"}\n\n",
		name->string, name->string, child->string
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tnumber(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	stdt_lexer_number(ctx, \"%s\");\n"
		"}\n\n",
		name->string, name->string
	);
	
	return (void *) &EXEC_ERROR;
}

static
void *
cb_tstring(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct tree_string* name = (struct tree_string *) list->cpt;
	
	cpl_printf(
		"static void\n"
		"register_%s(struct parser_context* ctx, struct tree_descr* executor)\n"
		"{\n"
		"	stdt_lexer_string(ctx, \"%s\");\n"
		"}\n\n",
		name->string, name->string
	);
	
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
	gen_simple_linear_cpt_next_more(lc, "pgen.name");
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
	executor_reg(executor, "pgen.quoted", (exec_cb) cb_tstring);
}

static void
register_all()
{
	struct parser_context* ctx = new_parser(0);
	struct tree_descr* ex = tree_descr(ctx);
	
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
	
	
	struct gen_tree* tree = parser_parse(ctx, "pgen.grammar");
	
	call_tree(tree, ex);
}


int
main(int argc, char** argv)
{
	register_all();
	return 0;
}
