#include "gen_attr.h"
#include "atrex.h"
#include "utils.h"
#include "atttree_helper.h"
#include <string.h>

int
call_atree(struct exec_descr* ctx, struct attnode* node, void* exec_data)
{
	struct run_atom* iter;
	char* name;
	for (iter = ctx->atoms; iter; iter = iter->next) {
		name = atree_get_attr(node, "name");
		cpl_debug("check tree #%s# == #%s#\n", name, iter->name);
		if ((! strcmp(iter->name, name)) && iter->cb) {
			return iter->cb(ctx, node, exec_data, iter->closure);
			;
		}
	}
	if (ctx->default_cb)
		return ctx->default_cb(ctx, node, exec_data, ctx->default_closure);
	cpl_debug("fatal tree (NEW ex) %s\n", node->name);
	return 1;
}

int
atree_caller_set_default_cb(struct exec_descr* ctx, run_atom_cb_t cb, void* closure)
{
	ctx->default_cb = cb;
	ctx->default_closure = closure;
	return 1;
}

void
atree_caller_reg(struct exec_descr* ctx, char* name, run_atom_cb_t cb, void* closure)
{
	struct run_atom** atom = &(ctx->atoms);
	while(*atom) {
		atom = &((*atom)->next);
	}
	(*atom) = mem_alloc(sizeof(**atom));
	(*atom)->next = NULL;
	(*atom)->name = mem_strdup(name);
	(*atom)->cb = cb;
	(*atom)->closure = closure;
}

struct exec_descr*
exec_descr()
{
	struct exec_descr* rt = mem_alloc(sizeof(*rt));
	rt->default_cb = NULL;
	rt->default_closure = NULL;
	rt->atoms = NULL;
	return rt;
}
