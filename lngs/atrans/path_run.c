#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

struct path_context {
	struct attnode* input;
	node_list* curr;
};

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	node_list* list = node->subtrees;
	
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
	list_flush(&(input->curr));
	list_add(&(input->curr), input);
	cpl_printf("go_root\n");
	return 1;
}

static
int
go_axis(struct exec_descr* ctx, struct attnode* node, struct path_context* input, void* closure)
{
	char* axis = atree_get_attr(atree_subtree(node, 1), "name");
	cpl_printf("go_axis %s\n", axis);
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "tree_root", (run_atom_cb_t) go_root, NULL);
	atree_caller_reg(ctx, "axis", (run_atom_cb_t) go_axis, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	struct attnode* in = generate_att_tree(argv[2], argv[3]);
	struct attnode* lng = generate_att_tree("design", argv[1]);
	
	struct exec_descr* ctx = reg_cpts();
	struct path_context path_ctx;
	path_ctx.input = in;
	path_ctx.curr = NULL;
	list_add(&(path_ctx.curr), in);
	call_atree(ctx, lng, & path_ctx);
	
	return 0;
}

