#include "register.h"
#include <string.h>
#include "ptypes.h"
#include "iochannel.h"
#include "utils.h"

struct gen_cpt_info *
parser_get_cpt(struct parser_context* ctx, char* cptname)
{
	struct cpt_info_list* list = ctx->cpts;
	
	while (list) {
		if (list->info->cptname && !strcmp(cptname, list->info->cptname))
			return list->info;
		list = list->next;
	}
	
	return NULL;
}

struct gen_cpt_info *
parser_get_cpt_type(struct parser_context* ctx, int type)
{
	struct cpt_info_list* list = ctx->cpts;
	
	while (list) {
		if (list->info->type == type)
			return list->info;
		list = list->next;
	}
	
	return NULL;
}

int
parser_get_type(struct parser_context* ctx, struct gen_cpt_info* cpt)
{
	return cpt->type;
}

struct gen_tree *
parse_general_cpt(struct gen_cpt_info* cpt)
{
	struct gen_tree* tree;
	
	tree = cpt->parse(cpt);
	if (gen_tree_cmp(tree, RECOVERABLE_ERROR))
		parser_recover(cpt->ctx, tree);
	
	return tree;
}

struct gen_tree *
parse_general(struct parser_context* ctx, char* cptname)
{
	struct gen_tree* tree;
	
	struct gen_cpt_info* cpt = parser_get_cpt(ctx, cptname);
	cpl_debug("begin, cpt %s\n", cptname);
	if (!cpt) {
		cpl_printf("error, cpt %s not found!\n", cptname);
		return ERROR;
	}
	
	tree = parse_general_cpt(cpt);
	if (tree->type != RECOVERABLE_ERROR->type)
		cpl_debug("end, cpt %s OK\n", cptname);
	else
		cpl_debug("end, cpt %s FAIL\n", cptname);
	
	return tree;
}

struct gen_tree *
parser_const(struct gen_cpt_info_const* cpt)
{
	struct gen_tree* tree;
	
	tree = cpt->const_parse();
	
	return tree;
}

void
recover_general(struct gen_cpt_info* cpt, struct gen_tree* tree)
{
	cpt->ctx->iochannel->recover(cpt->ctx->iochannel);
}

void
register_cpt(struct parser_context* ctx, struct gen_cpt_info* cpt)
{
	struct cpt_info_list** cpts = &(ctx->cpts);
	
	while (*cpts) {
		cpts = &((*cpts)->next);
	}
	
	(*cpts) = mem_alloc(sizeof(**cpts));
	(*cpts)->next = NULL;
	(*cpts)->info = cpt;
	cpt->type = ctx->types;
	ctx->types ++;
	cpt->ctx = ctx;
}

struct parser_context*
new_parser(int fd)
{
	struct parser_context* rt;
	
	rt = mem_alloc(sizeof(*rt));
	rt->cpts = NULL;
	rt->types = 0;
	ptypes_reg(rt);
	rt->iochannel = iochannel_new(fd);
	rt->spaces = " \f\t\n\r\v";
	
	return rt;
}

struct parser_context*
new_parser_with_io(struct iochannel* io)
{
	struct parser_context* rt;
	
	rt = mem_alloc(sizeof(*rt));
	rt->cpts = NULL;
	rt->types = 0;
	ptypes_reg(rt);
	rt->iochannel = io;
	rt->spaces = " \f\t\n\r\v";
	
	return rt;
}

struct gen_cpt_info *
cptreg_alloc_default(struct parser_context* ctx, char* name, unsigned int cptsize, unsigned int treesize)
{
	struct gen_cpt_info *cpt = mem_alloc(cptsize);
	cpt->ctx = ctx;
	cpt->cptname = mem_strdup(name);
	cpt->cpt_len = treesize;
	cpt->alloc = parser_default_alloc;
	cpt->recover = parser_default_recover;;
	cpt->unref = parser_default_unref;
	return cpt;
}
