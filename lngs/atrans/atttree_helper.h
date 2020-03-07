#ifndef ATTTREE_HELPER_H
#define ATTTREE_HELPER_H

#include "attree.h"

#define SEEK_PARENT 1
#define SEEK_CHILD 2
#define SEEK_PREV 3
#define SEEK_NEXT 4

char* atree_get_attr(struct attnode* node, char* name);
struct attnode* atree_subtree(struct attnode* node, int N);
void attnode_add_subtree_N(struct attnode* attnode, struct attnode* subtree, int N);
int next_node(int axis, char* name, struct attnode** currresult);
int cvt_axis(char* axis);

struct attnode* atree_subtree_name(struct attnode* node, char* name, int N);
int atree_subtree_rm(struct attnode* node, struct attnode* child);
void dirty_wrong_recursive_dealloc_wo_types(struct attnode* node);

int attnode_add_after(struct attnode* node, char* name, struct attnode* child);

char* atree_get_subnode_attr(struct attnode* node, int N, char* attr);
int atree_count(struct attnode* node, char* what);


#endif



