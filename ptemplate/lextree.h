#ifndef LEXTREE_H
#define LEXTREE_H

struct tree_string {
	struct gen_tree gen_tree;
	char* string;
};

struct tree_number {
	struct gen_tree gen_tree;
	double number;
};

#endif
