#include "attree.h"
#include "utils.h"



struct attnode*
attnode(char* name)
{
	struct attnode* new = mem_alloc(sizeof(*new));
	
	new->name = mem_strdup(name);
	new->subtrees = list_create();
	new->attrs = list_create();
	
	return new;
}

void
attnode_add_subtree(struct attnode* attnode, struct attnode* subtree)
{
	ADD_EL(attnode->subtrees, subtree);
}

void
attnode_add_attr(struct attnode* attnode, struct att_attr* attr)
{
	ADD_EL(attnode->attrs, attr);
}

struct att_attr*
att_attr(char* name, attr_type* type, void* value)
{
	struct att_attr* new = mem_alloc(sizeof(*new));
	
	new->name = mem_strdup(name);
	new->type = type;
	new->value = value;
	
	return new;
}
