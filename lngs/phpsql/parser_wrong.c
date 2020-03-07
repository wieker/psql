#include "register.h"
#include "stdp.h"

void
register_objdecl(struct parser_context* ctx)
{
	struct gen_simple_linear_cpt* cpt = gen_simple_linear_cpt(ctx, "objdecl");
	if (!cpt)
		return;
	gen_simple_linear_cpt_next(cpt, "objname");
	gen_simple_linear_token_next(cpt, "left_figure");
	gen_simple_linear_cpt_next_more(cpt, "objfield");
	gen_simple_linear_token_next(cpt, "right_figure");
	
	struct gen_simple_select_cpt* cpt0 = gen_simple_select_cpt(ctx, "objfield");
	if (!cpt0)
		return;
	gen_simple_select_cpt_next_select(cpt0, "scalar");
	gen_simple_select_cpt_next_select(cpt0, "link");
	
	cpt0 = gen_simple_select_cpt(ctx, "link");
	if (!cpt0)
		return;
	gen_simple_select_cpt_next_select(cpt0, "morelink");
	gen_simple_select_cpt_next_select(cpt0, "onelink");
	
	cpt = gen_simple_linear_cpt(ctx, "onelink");
	if (!cpt)
		return;
	gen_simple_linear_cpt_next(cpt, "objname");
	
	cpt = gen_simple_linear_cpt(ctx, "morelink");
	if (!cpt)
		return;
	gen_simple_linear_cpt_next(cpt, "objname");
	gen_simple_linear_token_next(cpt, "left_rect");
	gen_simple_linear_token_next(cpt, "right_rect");
	
	
	
	cpt = gen_simple_linear_cpt(ctx, "pagedecl");
	if (!cpt)
		return;
	gen_simple_linear_cpt_next(cpt, "link");
	gen_simple_linear_token_next(cpt, "dot");
	gen_simple_linear_cpt_next(cpt, "actname");
	gen_simple_linear_token_next(cpt, "left_br");
	gen_simple_linear_cpt_next_more(cpt, "actarg");
	gen_simple_linear_token_next(cpt, "right_br");
	
	cpt = gen_simple_linear_cpt(ctx, "actarg");
	if (!cpt)
		return;
	gen_simple_linear_cpt_next(cpt, "link");
	
	
}

int
main(int argc, char** argv)
{
	struct parser_context* ctx = new_parser(0);
	register_objdecl(ctx);
	return 0;
}
