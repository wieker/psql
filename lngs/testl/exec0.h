#ifndef EXEC0_H
#define EXEC0_H

#include "exec_tree.h"

struct exec_result {
	struct cpt_result base;
	double v;
};


struct exec_result* cb_expr(struct tree_descr* descr, struct tree_op_left_cpt* tree);
struct exec_result* cb_assign(struct tree_descr* descr, struct gen_simple_linear_cpt* tree);

struct exec_result* cb_andor(struct tree_descr* descr, struct tree_op_left_cpt* tree);
struct exec_result* cb_not(struct tree_descr* descr, struct tree_simple_linear* tree);
struct exec_result* cb_compare(struct tree_descr* descr, struct tree_op_left_cpt* tree);

struct exec_result* cb_sum(struct tree_descr* descr, struct tree_op_left_cpt* tree);
struct exec_result* cb_prod(struct tree_descr* descr, struct tree_op_left_cpt* tree);
struct exec_result* cb_pow(struct tree_descr* descr, struct tree_op_right_cpt* tree);

struct exec_result* cb_number(struct tree_descr* descr, struct tree_number* tree);
struct exec_result* cb_brackets(struct tree_descr* descr, struct tree_simple_linear* tree);

struct exec_result* cb_func_one(struct tree_descr* descr, struct tree_simple_linear* tree);

#endif

