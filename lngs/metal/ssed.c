
/*
 * simple(stupid) sematic executor description
 * used to execute files by description
 * */

#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "scalc.h"
#include "stype.h"

int config_debug = 0;


static int
go_deep_met(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	node_list* list = node->subtrees;
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (list) {
		if (!call_atree(ctx, (struct attnode*) list->element, input))
			return 0;
		list = list->next;
	}
	return 1;
}


static int
go_deep_met(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* name = atree_get_subnode_attr(node, 1, "value");
	char* func = atree_get_subnode_attr(node, 2, "value");
	void* dlhandle = dlopen(NULL, RTLD_LAZY);
	void* handle = dlsym(input->dlhandle, func);
	atree_caller_reg((struct exec_descr*) input, name, (run_atom_cb_t) handle, NULL);
	return 1;
}


static struct exec_descr*
reg_cpts_ssed()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_met, NULL);
	atree_caller_reg(ctx, "cptdecl", (run_atom_cb_t) cptdecl, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	struct attnode* scalc = generate_att_tree("ssed.gr", argv[1], 0);
	struct exec_descr* run = reg_cpts_ssed();
	struct exec_descr* newdescr = exec_descr();
	atree_caller_set_default_cb(newdescr, (run_atom_cb_t) go_deep_met, NULL);
	call_atree(run, scalc, & newdescr);

	wrxml(scalc);
	
	return 0;
}



