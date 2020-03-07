#ifndef EXEC_OP0_H
#define EXEC_OP0_H

#include "exec_expr0.h"

void* cb_op_semicolon(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_list(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_simple(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_elif(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_if(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_while(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_break(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_continue(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_return(struct tree_descr* descr, struct tree_simple_linear* tree);

void* cb_op_return_void(struct tree_descr* descr, struct tree_simple_linear* tree);

#endif
