#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "scalc.h"

int
go_deep_scalc(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct list_iter list = list_iter(node->subtrees);
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (CONT(& list)) {
		if (!call_atree(ctx, (struct attnode*) GET_EL(& list), input))
			return 0;
		NEXT_EL(& list);
	}
	return 1;
}
