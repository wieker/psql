#include "register.h"
#include "stdp.h"
#include "stdt.h"
#include "stdo.h"
#include "parser.h"
#include "exec_tree.h"
#include "utils.h"
#include "exec0.h"


struct exec_result *
cb_expr(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	cpl_printf("(");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_assign(struct tree_descr* descr, struct tree_op_right_cpt* tree)
{
	cpl_printf("(= ");
		
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_andor(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	cpl_printf("(");
	if (tree->optype == parser_get_cpt(descr->pctx, "and")->type)
		cpl_printf("and ");
	else
		cpl_printf("or ");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_not(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* l = tree->cpts;
	int b = 0;
	
	while (l) {
		if (l->cpt->type == parser_get_cpt(descr->pctx, "not_tok")->type) {
			if (!b)
				cpl_printf("(");
			b = 1;
			cpl_printf("not ");
		} else
			call_tree(l->cpt, descr);
		l = l->next;
	}
	
	if (b)
		cpl_printf(") ");
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_pow(struct tree_descr* descr, struct tree_op_right_cpt* tree)
{
	cpl_printf("(^ ");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_prod(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	cpl_printf("(");
	if (tree->optype == parser_get_cpt(descr->pctx, "div")->type)
		cpl_printf("/ ");
	else
		cpl_printf("* ");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_sum(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	cpl_printf("(");
	if (tree->optype == parser_get_cpt(descr->pctx, "minus")->type)
		cpl_printf("- ");
	else
		cpl_printf("+ ");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_number(struct tree_descr* descr, struct tree_number* tree)
{
	cpl_printf("%lf ", tree->number);
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_compare(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	cpl_printf("(");
	if (tree->optype == parser_get_cpt(descr->pctx, "less")->type)
		cpl_printf("< ");
	else if (tree->optype == parser_get_cpt(descr->pctx, "more")->type)
		cpl_printf("> ");
	else
		cpl_printf("== ");
	
	call_tree(tree->left, descr);
	call_tree(tree->right, descr);
	
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}


struct exec_result *
cb_brackets(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	cpl_printf("(");
	call_tree(tree->cpts->cpt, descr);
	cpl_printf(") ");
	
	return (struct exec_result *) &VOID;
}
