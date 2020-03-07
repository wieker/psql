#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdio.h>

#include "wrxml.h"

#include "repl0.h"
#include "repl_kbd.h"

int config_debug = 0;

#include <stdio.h>

void add_cpts_portion(char* grammar, struct parser_context* ctx_to_add_cpts, struct tree_descr* executor_to_add_hooks);
struct attnode *fixate_cpts(struct parser_context* ctx_to_add_cpts, struct tree_descr* executor_to_add_hooks, char* root);

struct exec_descr* reg_cpts_poly();

typedef void (*draw_func)(float x, float y);

void
calc_roots(draw_func drf)
{
	struct parser_context* ctx = new_parser_with_io(new_repl_0("poly.in"));
	struct tree_descr* internal_ex = tree_descr(ctx);
	
	add_cpts_portion("poly.gr", ctx, internal_ex);
	struct attnode* poly = fixate_cpts(ctx, internal_ex, "root");

	config_debug = 1;
	wrxml(poly);
	
	struct exec_descr* exc = reg_cpts_poly();
	call_atree(exc, poly, drf);
}

#if 0
int
main(int argc, char** argv)
{
	
	
	return 0;
}
#endif

