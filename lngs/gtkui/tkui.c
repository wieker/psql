#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "gtkcore.h"

#include <dlfcn.h>

int config_debug = 1;

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, struct ui_context* input, void* closure)
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
make_button(struct exec_descr* ctx, struct attnode* node, struct ui_context* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(node, 1), "value");
	char* cb = atree_get_attr(atree_subtree(node, 2), "value");
	cpl_debug("default for %s::endl\n", cb);
	cpl_debug("dlerror: %s\n", dlerror());
	void* handle = dlsym(input->dlhandle, cb);
	cpl_debug("dlerror: %s %p\n", dlerror(), input->dlhandle);
	tkui_gtkcore_button(input, 0, 0, handle, name);
	return 1;
}

static
int
make_edlist(struct exec_descr* ctx, struct attnode* node, struct ui_context* input, void* closure)
{
	input->last = tkui_gtkcore_edlist(input);
	char* name = atree_get_attr(atree_subtree(node, 1), "value");
	int i = 2;
	
	node_list* list = node->subtrees->next;
	
	input->last = tkui_gtkcore_edlist(input);
	
	cpl_debug("list %s\n", name);
	while (list) {
		tkui_gtkcore_column(input, atree_get_attr(atree_subtree(node, i), "value"));
		list = list->next;
		i ++;
	}
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "button", (run_atom_cb_t) make_button, NULL);
	atree_caller_reg(ctx, "edlist", (run_atom_cb_t) make_edlist, NULL);
	//atree_caller_reg(ctx, "lstname", (run_atom_cb_t) make_list, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	struct attnode* ui_desc = generate_att_tree("uigr.gr", "ui.in", 0);
	
	struct exec_descr* ctx = reg_cpts();
	struct ui_context ui_ctx;
	tkui_gtkcore_init(&ui_ctx, &argc, &argv);
	ui_ctx.dlhandle = dlopen(NULL, RTLD_LAZY);
	
	call_atree(ctx, ui_desc, &ui_ctx);
	
	tkui_gtkcore_start(&ui_ctx);
	
	return 0;
}

