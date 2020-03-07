#include "register.h"
#include "exec_tree.h"
#include "stdp.h"
#include "stdt.h"
#include "utils.h"
#include "lvalue0.h"
#include "exec_expr0.h"

#include "table.h"

static
void *
cb_lvalue(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_lvalue* result = result_alloc_0(descr, "lvalue", sizeof(*result));
	result->name = mem_strdup(((struct tree_string *) tree->cpts->cpt)->string);
	
	return result;
}

static
void *
cb_rvalue(struct tree_descr* descr, struct tree_string* tree)
{
	char* name = tree->string;
	struct type_number* result,* saved;
	
	if (!table_get_element(variables, name, (void **) &saved))
		return (void *) &EXEC_ERROR;
	else {
		result = result_alloc_0(descr, "number", sizeof(*result));
		*result = *saved;
		return result;
	}
}

static
void *
cb_name(struct tree_descr* descr, struct stdt_lexer_string* tree)
{
	
	return (void *) & VOID;
}

static void
register_lvalue(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "lvalue");
	gen_simple_linear_cpt_next(lc, "name");
	executor_reg(executor, "lvalue", (exec_cb) cb_lvalue);
}

static void
register_rvalue(struct parser_context* ctx, struct tree_descr* executor)
{
	//struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "rvalue");
	//gen_simple_linear_cpt_next(lc, "name");
	stdt_lexer_string(ctx, "rvalue");
	executor_reg(executor, "rvalue", (exec_cb) cb_rvalue);
}

static void
register_name(struct parser_context* ctx, struct tree_descr* executor)
{
	stdt_lexer_string(ctx, "name");
	executor_reg(executor, "name", (exec_cb) cb_name);
}


void
testl_register_lvalue(struct parser_context* ctx, struct tree_descr* ex)
{
	register_lvalue(ctx, ex);
	register_rvalue(ctx, ex);
	register_name(ctx, ex);
}
