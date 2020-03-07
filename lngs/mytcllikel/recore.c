#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <dlfcn.h>

#include "recore.h"
#include "table.h"

void *recore_list = NULL;


int
recore_top(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* cptname;
	char* funcname;
	void* _cb;
	
	cptname = atree_get_subnode_attr(node, 1, "value");
	funcname = atree_get_subnode_attr(node, 2, "value");
	_cb = dlsym(dlopen(NULL, RTLD_LAZY), funcname);
	cpl_debug("recore for %s is %p caused %s\n", cptname, _cb, dlerror());
	
	atree_caller_reg(ctx, cptname, _cb, NULL);
	return 1;
}


int
recore(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* cptname;
	char* funcname;
	void* _cb;
	
	cptname = atree_get_subnode_attr(node, 1, "value");
	funcname = atree_get_subnode_attr(node, 2, "value");
	_cb = dlsym(dlopen(NULL, RTLD_LAZY), funcname);
	
	table_set_element((struct table **) & recore_list, cptname, _cb);
	return 1;
}
