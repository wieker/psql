#include "register.h"
#include "exec_tree.h"
#include "stdp.h"
#include "stdt.h"
#include "utils.h"
#include "func0.h"
#include "exec_expr0.h"

#include "table.h"

#include <math.h>

void *
cb_func_one(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_number* result;
	func_one_t saved;
	cpl_debug("Enter func exec\n");
	
	if (!table_get_element(functions, ((struct tree_string *) tree->cpts->cpt)->string, (void **) &saved))
		return (void *) &EXEC_ERROR;
	
	result = result_alloc_0(descr, "number", sizeof(*result));
	if (! result_wait("number", tree->cpts->next->cpt, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	
	result->v = saved(result->v);
	
	return (void *) result;
}



static void
register_func_one(struct parser_context* ctx, struct tree_descr* executor)
{
	struct gen_simple_linear_cpt* lc = gen_simple_linear_cpt(ctx, "func_one");
	gen_simple_linear_cpt_next(lc, "funcname");
	stdt_lexer_string(ctx, "funcname");
	gen_simple_linear_cpt_next(lc, "value");
	executor_reg(executor, "func_one", (exec_cb) cb_func_one);
}

void
testl_register_func(struct parser_context* ctx, struct tree_descr* ex)
{
	register_func_one(ctx, ex);
	table_set_element(&functions, "log", (void *) log2);
}
