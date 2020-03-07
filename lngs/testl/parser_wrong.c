#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec_expr0.h"
#include "repl_kbd.h"
#include "ptypes.h"
#include "func0.h"

void testl_register_expr(struct parser_context* ctx, struct tree_descr* executor);
void testl_register_op(struct parser_context* ctx, struct tree_descr* executor);
void testl_register_lvalue(struct parser_context* ctx, struct tree_descr* executor);

int config_debug = 0;

static void
register_all()
{
	struct iochannel* io = new_repl_kbd(0);
	struct parser_context* ctx = new_parser_with_io(io);
	struct tree_descr* ex = tree_descr(ctx);
	struct gen_tree* tree;
	
	testl_register_expr(ctx, ex);
	testl_register_op(ctx, ex);
	testl_register_lvalue(ctx, ex);
	testl_register_func(ctx, ex);
	
	while (1) {
		//cpl_debug("iterate\n");
		if ((tree = parser_parse(ctx, "operator"))->type == RECOVERABLE_ERROR->type) {
			cpl_printf("parser error\n");
			repl_kbd_reset((struct iochannel_repl*) io, 1);
		}
		else if (tree->type == ERROR->type)
			return;
		else {
			//cpl_printf("result: %lf\n", ((struct type_number *) call_tree(tree, ex))->v);
			call_tree(tree, ex);
			//repl_kbd_reset((struct iochannel_repl*) io, 1);
		}
	}
}


int
main(int argc, char** argv)
{
	register_all();
	return 0;
}
