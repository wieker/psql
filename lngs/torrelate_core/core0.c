#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

struct path_context {
	struct attnode* input;
	struct attnode* current;
	struct exec_descr* input_ctx;
	struct exec_descr* trans_ctx;
};

int config_debug = 1;

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	node_list* list = node->subtrees;
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (list) {
		call_atree(ctx, (struct attnode*) list->element, input);
		list = list->next;
	}
	return 1;
}

static
int
go_root(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	input->current = input->input;
	call_atree(ctx, atree_subtree(node, 1), input);
	return 1;
}

static
int
go_element(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	char* prefix = atree_get_attr(atree_subtree(node, 1), "name");
	char* axis = atree_get_attr(atree_subtree(node, 2), "name");
	char* name = atree_get_attr(atree_subtree(node, 3), "value");
	if (strcmp(prefix, "null")) {
		attnode_add_subtree_N(atree_subtree(node, 1), atree_subtree(node, 2), 1);
		attnode_add_subtree_N(atree_subtree(node, 1), atree_subtree(node, 3), 2);
		call_atree(ctx, atree_subtree(node, 1), input);
	}
	cpl_debug("search %s:%s\n", axis, name);
	if (next_node(cvt_axis(axis), name, &(input->current))) {
		cpl_debug("%s:%s found!\n", axis, name);
		return 1;
	}
	return 0;
}

static
int
do_hook(struct exec_descr* ctx, struct attnode* node, struct path_context* input, struct attnode* template)
{
	cpl_debug("template triggered for node %s\n", atree_get_attr(node, "name"));
	input->current = node;
	call_atree(input->trans_ctx, atree_subtree(template, 2), input);
	go_deep(ctx, node, input, NULL);
	return 1;
}

static
int
do_print(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	cpl_printf("print %s\n", atree_get_attr(input->current, "name"));
	return 1;
}

static
int
do_template(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	atree_caller_reg(input->input_ctx,
				atree_get_attr(atree_subtree(node, 1), "value"),
				(run_atom_cb_t) do_hook, node);
	return 1;
}

static
int
do_foreach(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	struct attnode* el = atree_subtree(node, 1);
	struct attnode* exec = atree_subtree(node, 2);
	while (call_atree(ctx, el, (void*) input))
		call_atree(ctx, exec, (void*) input);
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "absolute", (run_atom_cb_t) go_root, NULL);
	atree_caller_reg(ctx, "element", (run_atom_cb_t) go_element, NULL);
	atree_caller_reg(ctx, "template", (run_atom_cb_t) do_template, NULL);
	atree_caller_reg(ctx, "print", (run_atom_cb_t) do_print, NULL);
	atree_caller_reg(ctx, "foreach", (run_atom_cb_t) do_foreach, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	struct attnode* in = generate_att_tree("input.gr", "input.in");
	struct attnode* lng = generate_att_tree("slang0.gr", "transform0.tr");
	
	struct exec_descr* ctx = reg_cpts();
	struct exec_descr* input_ctx = exec_descr();
	struct path_context path_ctx;
	
	path_ctx.input = in;
	path_ctx.input_ctx = input_ctx;
	call_atree(ctx, lng, & path_ctx);
	
	path_ctx.trans_ctx = ctx;
	atree_caller_set_default_cb(path_ctx.input_ctx, (run_atom_cb_t) go_deep, NULL);
	call_atree(path_ctx.input_ctx, in, & path_ctx);
	
	return 0;
}

