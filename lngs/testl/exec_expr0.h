#ifndef EXEC_EXPR0_H
#define EXEC_EXPR0_H


#include "exec_tree.h"
#include "stdo.h"
#include "stdt.h"


struct type_number {
	struct cpt_result base;
	double v;
};


void* cb_expr(struct tree_descr* descr, struct tree_op_left_cpt* tree);
void* cb_assign(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_andor(struct tree_descr* descr, struct tree_op_left_cpt* tree);
void* cb_not(struct tree_descr* descr, struct tree_simple_linear* tree);
void* cb_compare(struct tree_descr* descr, struct tree_op_left_cpt* tree);

void* cb_sum(struct tree_descr* descr, struct tree_op_left_cpt* tree);
void* cb_prod(struct tree_descr* descr, struct tree_op_left_cpt* tree);
void* cb_pow(struct tree_descr* descr, struct tree_op_right_cpt* tree);

void* cb_number(struct tree_descr* descr, struct tree_number* tree);
void* cb_brackets(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_log(struct tree_descr* descr, struct tree_simple_linear* tree);

#endif
