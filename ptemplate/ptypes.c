#include "ptypes.h"
#include "register.h"
#include "utils.h"

void
error_unref(struct gen_cpt_info* cpt, struct gen_tree* tree)
{
	return;
}

void
error_recover(struct gen_cpt_info* cpt, struct gen_tree* tree)
{
	return;
}


struct gen_cpt_info error = {
	.cpt_len = sizeof(struct gen_tree),
	.cptname = "error",
	.alloc = parser_default_alloc,
	.parse = NULL,
	.unref = error_unref,
	.recover = error_recover,
};

struct gen_cpt_info r_error = {
	.cpt_len = sizeof(struct gen_tree),
	.cptname = "error",
	.alloc = parser_default_alloc,
	.parse = NULL,
	.unref = error_unref,
	.recover = error_recover,
};

struct gen_tree* RECOVERABLE_ERROR;
struct gen_tree* ERROR;

int
gen_tree_cmp(struct gen_tree* A, struct gen_tree * B)
{
	return (A->type == B->type);
}

int parser_check_result(struct parser_context* parser, struct gen_tree* A, struct gen_tree * B)
{
	return (A->type == B->type);
}

void
ptypes_reg(struct parser_context* ctx)
{	
	register_cpt(ctx, &r_error);
	register_cpt(ctx, &error);
	RECOVERABLE_ERROR = parser_alloc(ctx, &r_error);
	ERROR = parser_alloc(ctx, &error);
}
