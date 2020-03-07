#include "atttree_helper.h"
#include "attree.h"
#include <string.h>
#include "utils.h"

char*
atree_get_attr(struct attnode* node, char* name)
{
	struct list_iter list = list_iter(node->attrs);
	
	while (CONT(& list)) {
		if (! strcmp(name, ((struct att_attr *)GET_EL(& list))->name))
			return ((struct att_attr *)GET_EL(& list))->value;
		NEXT_EL(& list);
	}
		cpl_debug("%s %s\n", node->name, name);
	return NULL;
}

struct attnode*
atree_subtree(struct attnode* node, int N)
{
	struct list_iter list = list_iter(node->subtrees);
	int i = 1;
	
	while (CONT(& list)) {
		if (i == N)
			return (struct attnode *)GET_EL(& list);
		NEXT_EL(& list);
		i ++;
	}
	return NULL;
}

struct attnode*
atree_subtree_name(struct attnode* node, char* name, int N)
{
	struct list_iter list = list_iter(node->subtrees);
	int i = 1;
	struct attnode* n;
	char* strname;
	
	while (CONT(& list)) {
		n = (struct attnode *)GET_EL(& list);
		if ((strname = atree_get_attr(n, "name")) && ! strcmp(name, strname)) {
			if (i == N)
				return (struct attnode *)GET_EL(& list);
			i ++;
		}
		NEXT_EL(& list);
	}
	return NULL;
}

void
attnode_add_subtree_N(struct attnode* attnode, struct attnode* subtree, int N)
{
	static int* ord = NULL;
	struct att_attr* subtreeord = att_attr("treeord", "pointer(number)", (void *) (ord + N));
	struct list_iter list = list_iter(attnode->subtrees);
	attnode_add_attr(subtree, subtreeord);
	
	//добавляем в пустой список
	if (! CONT(& list)) {
		ADD_EL(attnode->subtrees, subtree);
		return;
	}
	
	//добавляем перед первым элементом
	if ((((int *) atree_get_attr(
						(struct attnode *) GET_EL(& list), "treeord")) - ord) > N) {
		PRE_EL(& list, subtree);
		return;
	}
	
	//добавляем посередине списка
	while (CONT(& list)) {
		if ((((int *) atree_get_attr(
						(struct attnode *) GET_EL(& list), "treeord")) - ord) > N) {
			PRE_EL(& list, subtree);
			return;
		}
		NEXT_EL(& list);
	}
	//добавляем в конец
	ADD_EL(attnode->subtrees, subtree);
}

//axis == 1 - родителя
//axis == 2 - потомка
//axis == 3 - по предыдущим узлам
//axis == 4 - по последующим узлам
int
next_node(int axis, char* name, struct attnode** currresult)
{
	struct attnode* curr = *currresult;
	struct list_iter list = list_iter(curr->subtrees);
	char* elname;
	
	if (SEEK_CHILD == axis) {
		while (CONT(& list)) {
			elname = atree_get_attr(GET_EL(& list), "name");
			cpl_debug("element %s == %s\n", elname, name);
			if (! strcmp(name, elname)) {
				*currresult = (struct attnode*) GET_EL(& list);
				return 1;
			}
			*currresult = (struct attnode*) GET_EL(& list);
			if (next_node(axis, name, currresult))
				return 1;
			NEXT_EL(& list);
		}
		return 0;
	}
	
	if (SEEK_PARENT == axis) {
		if (!curr->parent)
			return 0;
		elname = atree_get_attr(curr->parent, "name");
		if (! strcmp(name, elname)) {
			*currresult = curr->parent;
			return 1;
		}
		*currresult = curr->parent;
		return next_node(axis, name, currresult);
	}
	
	if (SEEK_PREV == axis) {
		if (!curr->prev)
			return 0;
		elname = atree_get_attr(curr->prev, "name");
		if (! strcmp(name, elname)) {
			*currresult = curr->prev;
			return 1;
		}
		*currresult = curr->prev;
		return next_node(axis, name, currresult);
	}
	
	if (SEEK_NEXT == axis) {
		if (!curr->next)
			return 0;
		elname = atree_get_attr(curr->next, "name");
		if (! strcmp(name, elname)) {
			*currresult = curr->next;
			return 1;
		}
		*currresult = curr->next;
		return next_node(axis, name, currresult);
	}
	return 0;
}

int
cvt_axis(char* axis)
{
	if (!strcmp(axis, "prev"))
		return SEEK_PREV;
	if (!strcmp(axis, "next"))
		return SEEK_NEXT;
	if (!strcmp(axis, "child"))
		return SEEK_CHILD;
	if (!strcmp(axis, "parent"))
		return SEEK_PARENT;
	return 0;
}



int
atree_subtree_rm(struct attnode* node, struct attnode* child)
{
	struct list_iter list = list_iter(node->subtrees);
	
	while (CONT(& list)) {
		if (GET_EL(& list) == child) {
			RM_EL(& list);
			return 0;
		}
		NEXT_EL(& list);
	}
	return 0;
}

static
void
dirty_wrong_attrs(attr_list* attrs)
{
	struct list_iter list = list_iter(attrs);
	while (CONT(& list)) {
		/*
		 * Too meny statical allocs...
		 * */
		//mem_free(n->name);
		mem_free(((struct att_attr *)  (GET_EL(& list)))->value);
		RM_EL(& list);
	}
}

void
dirty_wrong_recursive_dealloc_wo_types(struct attnode* node)
{
	dirty_wrong_attrs(node->attrs);
	struct list_iter list = list_iter(node->subtrees);
	while (CONT(& list)) {
		/*
		 * Too meny statical allocs...
		 * */
		//mem_free(n->name);
		dirty_wrong_recursive_dealloc_wo_types(GET_EL(& list));
		RM_EL(& list);
	}
	mem_free(node);
}



int
attnode_add_after(struct attnode* node, char* name, struct attnode* child)
{
	struct list_iter list = list_iter(node->subtrees);
	
	while (CONT(& list)) {
		if (!strcmp(name, atree_get_attr(GET_EL(& list), "name"))) {
			INS_EL(& list, child);
			return 1;
		}
		NEXT_EL(& list);
	}
	return 0;
}

char*
atree_get_subnode_attr(struct attnode* node, int N, char* attr)
{
	return atree_get_attr(atree_subtree(node, N), attr);
}

int
atree_count(struct attnode* node, char* what)
{
	struct list_iter list = list_iter(node->subtrees);
	int i = 0;
	
	while (CONT(& list)) {
		if (!strcmp(what, atree_get_attr(GET_EL(& list), "name"))) {
			i ++;
		}
		NEXT_EL(& list);
	}
	return i;
}
