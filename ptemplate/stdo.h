#ifndef STDO_H
#define STDO_H

#include "register.h"
#include "parser.h"

struct tree_op_left_cpt {
	struct gen_tree gen_tree;
	int optype;
	struct gen_tree* left;
	struct gen_tree* right;
	struct transact_list* rec_point;
};

struct tree_op_right_cpt {
	struct gen_tree gen_tree;
	int optype;
	struct gen_tree* left;
	struct gen_tree* right;
	struct transact_list* rec_point;
};

struct stdp_op_left_cpt* stdp_op_left_cpt(struct parser_context* ctx, char* name, char* sub, char* ops);
struct stdp_op_right_cpt* stdp_op_right_cpt(struct parser_context* ctx, char* name, char* sub, char* ops);
struct stdp_left_mod_cpt* stdp_left_mod_cpt(struct parser_context* ctx, char* name, char* sub, char* ops);


#endif

