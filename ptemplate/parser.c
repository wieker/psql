#include "register.h"
#include "parser.h"
#include "utils.h"
#include "iochannel.h"

struct gen_tree *
parser_parse(struct parser_context* parser, char* cpt_name)
{
	//cpl_printf("parsing %s\n", cpt_name);
	return parse_general(parser, cpt_name);
}

struct gen_tree *
parser_default_alloc(struct gen_cpt_info* cpt)
{
	struct gen_tree * rt = mem_alloc(cpt->cpt_len);
	rt->type = cpt->type;
	return rt;
}

struct gen_tree *
parser_alloc(struct parser_context* ctx, struct gen_cpt_info* cpt)
{
	return cpt->alloc(cpt);
}

void
parser_default_unref(struct gen_cpt_info* cpt, struct gen_tree* tree)
{
	mem_free(tree);
}

void
parser_unref(struct parser_context* ctx, struct gen_tree* tree)
{
	struct gen_cpt_info* cpt;
	cpt = parser_get_cpt_type(ctx, tree->type);
	if (!cpt) {
		cpl_printf("error, cpt %d not found!\n", tree->type);
		return;
	}
	cpt->unref(cpt, tree);
}

void
parser_default_recover(struct gen_cpt_info* cpt, struct gen_tree* tree)
{
	cpt->ctx->iochannel->recover(cpt->ctx->iochannel);
}

void
parser_recover(struct parser_context* ctx, struct gen_tree* tree)
{
	struct gen_cpt_info* cpt;
	cpt = parser_get_cpt_type(ctx, tree->type);
	if (!cpt) {
		cpl_printf("error, cpt %d not found!\n", tree->type);
		return;
	}
	cpt->recover(cpt, tree);
}
