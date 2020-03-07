#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "pathdecl.h"

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct list_iter list = list_iter(node->subtrees);
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (CONT(& list)) {
		if (call_atree(ctx, (struct attnode*) GET_EL(& list), input))
			return 1;
		NEXT_EL(& list);
	}
	return 0;
}

/*
 * Search for complex tag;
 * For each complex tag trying to compare for it
 * now - stupid and broken alg
 * */
static
int
pcomplex(struct exec_descr* ctx, struct attnode* node, struct path* path, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* tagname = atree_get_attr(name_node, "value");
	if (! path)
		return 1;
	cpl_debug("Comparing tags: #%s# and #%s#\n", tagname, path->path_step_tag.tagname);
	if (path->type == PATH_TAG && ! strcmp(tagname, path->path_step_tag.tagname)) {
		path = path->next;
		if (path && path->cb && path->cb(path, node, path->data))
				return 1;
		return go_deep(ctx, node, path, closure);
	}
	if (path->type == PATH_ALL) {
		path = path->next;
		return go_deep(ctx, node, path, closure);
	}
	return 0;
}

/*
 * Search for tag param;
 *  - stupid and broken alg
 * */
static
int
wparam(struct exec_descr* ctx, struct attnode* node, struct path* path, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(name_node, "value");
	if (! path)
		return 1;
	if (path->type == PATH_PARAM && ! strcmp(pname, path->path_step_param.pname)) {
		path = path->next;
		if (path && path->cb && path->cb(path, node, path->data))
				return 1;
		return go_deep(ctx, node, path, closure);
	}
	if (path->type == PATH_ALL) {
		path = path->next;
		return go_deep(ctx, node, path, closure);
	}
	return 0;
}

struct exec_descr*
reg_cpts_xml()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "complex", (run_atom_cb_t) pcomplex, NULL);
	atree_caller_reg(ctx, "param", (run_atom_cb_t) wparam, NULL);
	return ctx;
}

