#include "../cpl/cpts.h"

union cpt_result
call_tree(union tree* tree, struct cpt_descr* cpt_descr)
{
	struct cpt_descr* iter;
	for (iter = cpt_descr; iter->cpt; iter ++) {
		//cpl_printf("check tree %d == %d %s\n", tree->type, iter->cpt, iter->description);
		if (iter->cpt == tree->type && iter->cb) {
			return iter->cb(tree);
			;
		}
	}
	cpl_printf("fatal tree %d\n", tree->type);
	return ERROR;
}

const union cpt_result VOID = { .type = CPT_RES_VOID };
const union cpt_result ERROR = { .type = CPT_RES_ERROR };
const union cpt_result SUCC = { .type = CPT_RES_SUCC };

