#include "exec_tree.h"
#include "parser.h"
#include "utils.h"

#define CPT_RES_VOID NULL
#define CPT_RES_ERROR 65530
#define CPT_RES_SUCC 65531

const struct cpt_result VOID = { .type = CPT_RES_VOID, .alloced = 0 };
const struct cpt_result EXEC_ERROR = { .type = CPT_RES_ERROR, .alloced = 0  };
const struct cpt_result SUCC = { .type = CPT_RES_SUCC, .alloced = 0  };

const struct cpt_result *
call_tree(struct gen_tree* tree, struct tree_descr* tree_descr)
{
	struct cpt_descr* iter;
	for (iter = tree_descr->cpts; iter; iter = iter->next) {
		//cpl_printf("check tree %d == %d\n", tree->type, iter->cpt);
		if (iter->cpt == tree->type && iter->cb) {
			return iter->cb(tree_descr, tree);
			;
		}
	}
	cpl_printf("fatal tree %d: %s\n", tree->type,
			parser_get_cpt_type(tree_descr->pctx, tree->type)->cptname);
	return &EXEC_ERROR;
}

struct tree_descr*
tree_descr(struct parser_context* pctx)
{
	struct tree_descr* rt;
	
	rt = mem_alloc(sizeof(*rt));
	rt->pctx = pctx;
	rt->cpts = NULL;
	rt->curr_data = NULL;
	
	return rt;
}

void
executor_reg(struct tree_descr* descr, char* name, exec_cb cb)
{
	struct cpt_descr** l = &(descr->cpts);
	while (*l)
		l = &((*l)->next);
	(*l) = mem_alloc(sizeof(**l));
	(*l)->next = NULL;
	(*l)->cb = cb;
	(*l)->cpt = parser_get_cpt(descr->pctx, name)->type;
}

int
result_wait(char* name, struct gen_tree* tree, struct tree_descr* descr,
			struct cpt_result** rt)
{
	struct cpt_result* left = (*rt) = (struct cpt_result*) call_tree(tree, descr);
	if (left->type ==
			parser_get_cpt(descr->pctx, name)->type)
		return 1;
	if (left->alloced)
		mem_free(left);
	return 0;
}

void *
result_alloc_0(struct tree_descr* descr, char* name, int size)
{
	struct cpt_result* rt = mem_alloc(size);
	rt->size = size;
	rt->type = parser_get_cpt(descr->pctx, name)->type;
	rt->alloced = 1;
	return rt;
}
