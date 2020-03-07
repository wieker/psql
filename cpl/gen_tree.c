#include "tree.h"
#include "tokenizer.h"
#include "utils.h"
#include "table.h"
#include "cpts.h"
#include "tcltree.h"

#include <stdio.h>


union cpt_result cb_expr(union tree* tree);
union cpt_result cb_andor(union tree* tree);
union cpt_result cb_not(union tree* tree);
union cpt_result cb_compare(union tree* tree);
union cpt_result cb_sum(union tree* tree);
union cpt_result cb_prod(union tree* tree);
union cpt_result cb_pow(union tree* tree);
union cpt_result cb_number(union tree* tree);
union cpt_result cb_string(union tree* tree);
union cpt_result cb_tcl(union tree* tree);
union cpt_result cb_tcl_while(union tree* tree);
union cpt_result cb_tcl_operator(union tree* tree);
union cpt_result cb_tcl_name(union tree* tree);
union cpt_result cb_tcl_var(union tree* tree);
union cpt_result cb_tcl_string(union tree* tree);

struct cpt_descr cpt_descr[] = {
	{CPT_EXPRESSION, "expression", cb_expr,},
	{CPT_ANDOR, "andor", cb_andor,},
	{CPT_NOT, "not", cb_not,},
	{CPT_COMPARE, "compare", cb_compare,},
	{CPT_SUM, "sum", cb_sum,},
	{CPT_PROD, "prod", cb_prod,},
	{CPT_POW, "expression", cb_pow,},
	{CPT_NUMBER, "number", cb_number,},
	{CPT_STRING, "expression", cb_string,},
	//{CPT_NAME, "expression", NULL,},
	//{CPT_COLUMN, "expression", NULL,},
	//{CPT_FUNCTION, "expression", NULL,},
	{CPT_TCL_OPERATOR_LIST, "tcl_prog", cb_tcl,},
	{CPT_TCL_WHILE, "expression", cb_tcl_while,},
	{CPT_OPERATOR, "expression", cb_tcl_operator,},
	{CPT_NAME, "expression", cb_tcl_name,},
	{CPT_VAR, "expression", cb_tcl_var,},
	{CPT_TCL_STRING, "expression", cb_tcl_string,},
	{0, NULL, NULL},
};

#define call_tree(tree) call_tree(tree, cpt_descr);

union cpt_result
cb_expr(union tree* tree)
{
	cpl_printf("(EXPRESSION ");
	if (tree->expression.assign)
		call_tree(tree->expression.assign);
	if (tree->expression.ostexpr)
		call_tree(tree->expression.ostexpr);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_andor(union tree* tree)
{
	if (tree->andor.op_type == TOK_AND)
		cpl_printf("(AND ");
	else
		cpl_printf("(OR ");
	if (tree->andor.first_andor)
		call_tree(tree->andor.first_andor);
	if (tree->andor.not)
		call_tree(tree->andor.not);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_sum(union tree* tree)
{
	if (tree->sum.op_type == TOK_PLUS)
		cpl_printf("(+ ");
	else
		cpl_printf("(- ");
	if (tree->sum.left)
		call_tree(tree->sum.left);
	if (tree->sum.right)
		call_tree(tree->sum.right);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_prod(union tree* tree)
{
	if (tree->prod.op_type == TOK_MUL)
		cpl_printf("(* ");
	else
		cpl_printf("(/ ");
	if (tree->prod.left)
		call_tree(tree->prod.left);
	if (tree->prod.right)
		call_tree(tree->prod.right);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_number(union tree* tree)
{
	cpl_printf("%f ", tree->number.number);
	return VOID;
}

union cpt_result
cb_string(union tree* tree)
{
	cpl_printf("%s ", tree->string.string);
	return VOID;
}

union cpt_result
cb_not(union tree* tree)
{
	if (tree->not.not)
		cpl_printf("(NOT ");
	if (tree->not.compare)
		call_tree(tree->not.compare);
	if (tree->not.not)
		cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_compare(union tree* tree)
{
	if (tree->compare.op_type == TOK_MORE)
		cpl_printf("(> ");
	else if (tree->compare.op_type == TOK_EQ)
		cpl_printf("(= ");
	else
		cpl_printf("(< ");
	if (tree->compare.left)
		call_tree(tree->compare.left);
	if (tree->compare.right)
		call_tree(tree->compare.right);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_pow(union tree* tree)
{
	cpl_printf("(^ ");
	if (tree->pow.left)
		call_tree(tree->pow.left);
	if (tree->pow.right)
		call_tree(tree->pow.right);
	cpl_printf(") ");
	return VOID;
}


union cpt_result
cb_tcl(union tree* tree)
{
	union tcl_tree* tt = (union tcl_tree *) tree;
	cpl_printf("(TCL_PROG ");
	if (tt->tcl_program.operator)
		call_tree((union tree *) tt->tcl_program.operator);
	if (tt->tcl_program.ostprog)
		call_tree((union tree *) tt->tcl_program.ostprog);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_tcl_while(union tree* tree)
{
	struct tcl_ifop* tt = (struct tcl_ifop *) tree;
	cpl_printf("(WHILE ");
	if (tt->condition)
		call_tree((union tree *) tt->condition);
	if (tt->body)
		call_tree((union tree *) tt->body);
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_tcl_operator(union tree* tree)
{
	struct arg_list* args;
	struct tcl_operator* tt = (struct tcl_operator *) tree;
	cpl_printf("(%s ", tt->name);
	args = tt->args;
	while (args) {
		if (args->arg)
			call_tree((union tree *) args->arg);
		args = args->next;
	}
	cpl_printf(") ");
	return VOID;
}

union cpt_result
cb_tcl_name(union tree* tree)
{
	struct tcl_name* name = (struct tcl_name *) tree;
	cpl_printf("%s ", name->name);
	return VOID;
}

union cpt_result
cb_tcl_var(union tree* tree)
{
	union tcl_tree* tt = (union tcl_tree *) tree;
	tt = NULL;
	return VOID;
}

union cpt_result
cb_tcl_string(union tree* tree)
{
	union tcl_tree* tt = (union tcl_tree *) tree;
	tt = NULL;
	return VOID;
}


int
main(int argc, char** argv)
{
	union tree* tree;
	input = "1 + 1, 2, 4 / (2 - 6) / 5, 2 > 3 and not 4 == 5 or 3 == 3";
	tree = parse_expression();
	if (tree)
		call_tree(tree);
	cpl_printf("\n");
	return (tree == NULL);
}
