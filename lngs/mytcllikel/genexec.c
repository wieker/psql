#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <dlfcn.h>

#include "recore.h"
#include "table.h"
#include "scalc.h"

struct attnode*
recore_node(struct attnode* node)
{
	char* nodename = atree_get_subnode_attr(node, 1, "value");
	struct attnode* new_node = attnode(nodename);
	node_list* subtrees = node->subtrees->next;
	struct att_attr* att = att_attr("name", "strz", nodename);
	attnode_add_attr(new_node, att);
	new_node->subtrees = subtrees;
	return new_node;
}


int
exec_subexpr(struct exec_descr* ctx, struct attnode* node, struct recore_pointer* input, void* closure)
{
	char* funcname;
	run_atom_cb_t _cb;
	struct attnode* new_node;
	
	go_deep_scalc(ctx, node, input, closure);
	
	funcname = atree_get_subnode_attr(node, 1, "value");
	table_get_element((struct table *) recore_list, funcname, (void **) & _cb);
	new_node = recore_node(node);
	
	_cb(ctx, new_node, input->p, closure);
	return 1;
}


int
word(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* word;
	int len = 0;
	node_list* subtrees = node->subtrees;
	char* name;
	
	while (subtrees) {
		name = atree_get_attr(subtrees->element, "name");
		if (! strcmp(name, "letter")) {
			len ++;
		}
		subtrees = subtrees->next;
	}
	word = mem_alloc(sizeof(char) * (1 + len));
	len = 0;
	
	subtrees = node->subtrees;
	while (subtrees) {
		name = atree_get_attr(subtrees->element, "name");
		if (! strcmp(name, "letter")) {
			word[len] = *atree_get_attr(subtrees->element, "value");
			len ++;
		}
		subtrees = subtrees->next;
	}
	
	struct att_attr* att = att_attr("value", "strz", word);
	attnode_add_attr(node, att);
	return 1;
}


int
ctx(struct exec_descr* ctx, struct attnode* node, struct recore_pointer* input, void* closure)
{
	char* funcname;
	void* (*_cb)();
	
	funcname = atree_get_subnode_attr(node, 1, "value");
	_cb = dlsym(dlopen(NULL, RTLD_LAZY), funcname);
	
	input->p = _cb();
	return 1;
}

