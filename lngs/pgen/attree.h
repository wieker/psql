#ifndef ATTREE_H
#define ATTREE_H

#include "exec_tree.h"

typedef struct list node_list;
typedef struct list attr_list;

struct attnode {
	char* name;
	node_list* subtrees;
	attr_list* attrs;
	struct attnode* parent;
	struct attnode* root;
	struct attnode* prev;
	struct attnode* next;
	node_list* cached_prev;
	node_list* cached_next;
	//struct raw* raw;
	//struct node* node;
};

typedef char attr_type;

struct att_attr {
	char* name;
	attr_type* type;
	void* value;
};

struct attnode* attnode(char* name);
void attnode_add_subtree(struct attnode* attnode, struct attnode* subtree);
void attnode_add_attr(struct attnode* attnode, struct att_attr* attr);

struct att_attr* att_attr(char* name, attr_type* type, void* value);

#endif
