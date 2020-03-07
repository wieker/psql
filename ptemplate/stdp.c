#include "stdp.h"
#include "utils.h"
#include "ptypes.h"

struct cpt_name_list {
	char* cpt_name;
	int more;
	int token;
	struct cpt_name_list* next;
};

struct gen_simple_linear_cpt {
	struct gen_cpt_info gen_info;
	
	struct cpt_name_list* cpts_name;
};

struct gen_simple_select_cpt {
	struct gen_cpt_info gen_info;
	
	struct cpt_name_list* cpts_name;
};

struct gen_negative_cpt {
	struct gen_cpt_info gen_info;
	
	char* negative;
};

static
struct tree_simple_linear *
parse_linear_cpt(struct gen_simple_linear_cpt* cpt)
{
	struct tree_simple_linear* rt;
	struct gen_tree* spt;
	struct cpt_name_list* subparse = cpt->cpts_name;
	struct parser_context* parser = cpt->gen_info.ctx;
	struct cpt_list** rtcpts;
	
	rt = (struct tree_simple_linear *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	rt->cpts = NULL;
	rtcpts = &(rt->cpts);
	rt->gen_tree.type = parser_get_type(parser, (struct gen_cpt_info *) cpt);
	rt->rec_point = parser->iochannel->begin(parser->iochannel);
	
	while (subparse) {
		//cpl_debug("more for %s\n", cpt->gen_info.cptname);
		spt = parser_parse(parser, subparse->cpt_name);
		if (gen_tree_cmp(spt, ERROR)) {
			parser_unref(parser, (struct gen_tree *) rt);
			parser_unref(parser, spt);
			return (struct tree_simple_linear *) ERROR;
		}
		if ((subparse->token == 1) && !(gen_tree_cmp(spt, RECOVERABLE_ERROR))) {
			parser_unref(parser, spt);
			subparse = subparse->next;
			continue;
		} //TOKENS should be saved for recover purposes ... or not
		if (subparse->more == 0) {
			if (gen_tree_cmp(spt, RECOVERABLE_ERROR)) {
				parser_recover(parser, (struct gen_tree *) rt);
				parser_unref(parser, (struct gen_tree *) rt);
				parser_unref(parser, spt);
				return (struct tree_simple_linear *) RECOVERABLE_ERROR;
			}
			(*rtcpts) = mem_alloc(sizeof(**rtcpts));
			(*rtcpts)->cpt = spt;
			(*rtcpts)->next = NULL;
			rtcpts = &((*rtcpts)->next);
			subparse = subparse->next;
			continue;
		}
		if (gen_tree_cmp(spt, RECOVERABLE_ERROR)) {
			subparse = subparse->next;
			parser_unref(parser, spt);
			continue;
		}

		(*rtcpts) = mem_alloc(sizeof(**rtcpts));
		(*rtcpts)->cpt = spt;
		(*rtcpts)->next = NULL;
		rtcpts = &((*rtcpts)->next);
	}
	
	return rt;
}

static
void
recover_linear_cpt(struct gen_simple_linear_cpt* cpt,
		struct tree_simple_linear* tree)
{
	struct parser_context* parser = cpt->gen_info.ctx;
	//struct cpt_list* cptl = tree->cpts;
	
	/*while (cptl) {
		parser_recover(parser, cptl->cpt);
		cptl = cptl->next;
	}*/
	//cpl_debug("recover to\n");
	
	parser->iochannel->recover_to(parser->iochannel, tree->rec_point);
}

static
void
unref_linear_cpt(struct gen_simple_linear_cpt* cpt,
		struct tree_simple_linear* tree)
{
	struct parser_context* parser = cpt->gen_info.ctx;
	struct cpt_list* cptl = tree->cpts;
	
	while (cptl) {
		parser_unref(parser, cptl->cpt);
		cptl = cptl->next;
	}
	
	mem_free(tree);
}

struct gen_simple_linear_cpt *
gen_simple_linear_cpt(struct parser_context* ctx, char* name)
{
	struct gen_simple_linear_cpt* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_simple_linear);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_linear_cpt;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_linear_cpt;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_linear_cpt;
	
	cpt->cpts_name = NULL;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

void
gen_simple_linear_cpt_next(struct gen_simple_linear_cpt* cpt, char* name)
{
	struct cpt_name_list** rtcpts = &(cpt->cpts_name);
	
	while (*rtcpts) {
		rtcpts = &((*rtcpts)->next);
	}
	(*rtcpts) = mem_alloc(sizeof(**rtcpts));
	(*rtcpts)->next = NULL;
	(*rtcpts)->cpt_name = mem_strdup(name);
	(*rtcpts)->more = 0;
	(*rtcpts)->token = 0;
}

void
gen_simple_linear_token_next(struct gen_simple_linear_cpt* cpt, char* name)
{
	struct cpt_name_list** rtcpts = &(cpt->cpts_name);
	
	while (*rtcpts) {
		rtcpts = &((*rtcpts)->next);
	}
	(*rtcpts) = mem_alloc(sizeof(**rtcpts));
	(*rtcpts)->next = NULL;
	(*rtcpts)->cpt_name = mem_strdup(name);
	(*rtcpts)->more = 0;
	(*rtcpts)->token = 1;
}

void
gen_simple_linear_cpt_next_more(struct gen_simple_linear_cpt* cpt, char* name)
{
	struct cpt_name_list** rtcpts = &(cpt->cpts_name);
	
	while (*rtcpts) {
		rtcpts = &((*rtcpts)->next);
	}
	(*rtcpts) = mem_alloc(sizeof(**rtcpts));
	(*rtcpts)->next = NULL;
	(*rtcpts)->cpt_name = mem_strdup(name);
	(*rtcpts)->more = 1;
	(*rtcpts)->token = 0;
}


static
struct gen_tree *
parse_select_cpt(struct gen_simple_select_cpt* cpt)
{
	struct gen_tree* spt;
	struct cpt_name_list* subparse = cpt->cpts_name;
	struct parser_context* parser = cpt->gen_info.ctx;
	
	while (subparse) {
		spt = parser_parse(parser, subparse->cpt_name);
		
		if (gen_tree_cmp(spt, ERROR)) {
			parser_unref(parser, spt);
			return ERROR;
		}
		
		if (gen_tree_cmp(spt, RECOVERABLE_ERROR)) {
			subparse = subparse->next;
			parser_unref(parser, spt);
			continue;
		}

		return spt;
	}
	
	return RECOVERABLE_ERROR;
}

struct gen_simple_select_cpt *
gen_simple_select_cpt(struct parser_context* ctx, char* name)
{
	struct gen_simple_select_cpt* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_simple_linear);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = parser_default_recover;
	cpt->gen_info.unref = parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_select_cpt;
	
	cpt->cpts_name = NULL;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

void
gen_simple_select_cpt_next_select(struct gen_simple_select_cpt* cpt, char* name)
{
	struct cpt_name_list** rtcpts = &(cpt->cpts_name);
	
	while (*rtcpts) {
		rtcpts = &((*rtcpts)->next);
	}
	(*rtcpts) = mem_alloc(sizeof(**rtcpts));
	(*rtcpts)->next = NULL;
	(*rtcpts)->cpt_name = mem_strdup(name);
}


static
struct gen_tree *
parse_negative_cpt(struct gen_negative_cpt* cpt)
{
	struct gen_tree* spt;
	struct parser_context* parser = cpt->gen_info.ctx;
	
	struct transact_list* rec = parser->iochannel->begin(parser->iochannel);
	
	spt = parser_parse(parser, cpt->negative);
	
	
	if (gen_tree_cmp(spt, RECOVERABLE_ERROR)) {
		parser_recover(parser, spt);
		parser_unref(parser, spt);
		return parser_alloc(parser, (struct gen_cpt_info *) cpt);
	} else {
		parser_recover(parser, spt);
		parser_unref(parser, spt);
		parser->iochannel->recover_to(parser->iochannel, rec);
		return RECOVERABLE_ERROR;
	}
}

struct gen_negative_cpt *
gen_negative_cpt(struct parser_context* ctx, char* name, char* negative)
{
	struct gen_negative_cpt* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct gen_tree);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = parser_default_recover;
	cpt->gen_info.unref = parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_negative_cpt;
	
	cpt->negative = mem_strdup(negative);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}
