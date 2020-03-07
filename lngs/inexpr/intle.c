#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "intops.h"
#include "boolops.h"

void testl_register_expr(struct parser_context* ctx, struct tree_descr* executor);
void testl_register_op(struct parser_context* ctx, struct tree_descr* executor);
void testl_register_lvalue(struct parser_context* ctx, struct tree_descr* executor);

static void
register_all()
{
	struct parser_context* ctx = new_parser(0);
	struct tree_descr* ex = tree_descr(ctx);
	
	testl_register_expr(ctx, ex);
	testl_register_op(ctx, ex);
	testl_register_lvalue(ctx, ex);
	
	struct gen_tree* tree = parser_parse(ctx, "operator");
	
	cpl_printf("result: %lf\n", ((struct type_number *) call_tree(tree, ex))->v);
}


int
main(int argc, char** argv)
{
	register_all();
	return 0;
}
