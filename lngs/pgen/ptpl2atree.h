#ifndef PTPL2ATREE_H
#define PTPL2ATREE_H

#include "attree.h"
#include "stdt.h"
#include "stdp.h"
#include "stdo.h"
#include "stde.h"
#include "exec_tree.h"


struct attnode* cb_exec_lop(struct tree_descr* descr, struct tree_op_left_cpt* tree);
struct attnode* cb_exec_rop(struct tree_descr* descr, struct tree_op_right_cpt* tree);
struct attnode* cb_exec_linear(struct tree_descr* descr, struct tree_simple_linear* tree);
struct attnode* cb_exec_tone(struct tree_descr* descr, struct gen_tree* tree);
struct attnode* cb_exec_tnumber(struct tree_descr* descr, struct tree_number* tree);
struct attnode* cb_exec_tstring(struct tree_descr* descr, struct tree_string* tree);
struct attnode* cb_exec_tchar(struct tree_descr* descr, struct tree_stdechar* tree);
struct attnode* cb_exec_tinteger(struct tree_descr* descr, struct tree_integer* tree);


#endif

