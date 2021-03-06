#include "cpts.h"
#include "utils.h"
#include "tokenizer.h"
#include "table.h"
#include "type_cast.h"

#include "func_descr.h"

#include <math.h>


union cpt_result cb_expr(union tree* tree);
union cpt_result cb_assign(union tree* tree);
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
union cpt_result cb_function(union tree* tree);


struct cpt_descr expr_cpt_descr[] = {
	{CPT_EXPRESSION, "expression", cb_expr,},
	{CPT_ASSIGN, "assign", cb_assign,},
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
	{CPT_FUNCTION, "function", cb_function,},
	{0, NULL, NULL},
};

#define call_tree(tree) call_tree(tree, expr_cpt_descr)

union cpt_result
cb_expr(union tree* tree)
{
	union cpt_result res;
	if (tree->expression.assign)
		res = call_tree(tree->expression.assign);
	if (type_cast(&res, CPT_RES_NUMBER))
		cpl_printf("result: %f\n", res.number.number);
	else
		cpl_printf("error\n");
	if (tree->expression.ostexpr)
		res = call_tree(tree->expression.ostexpr);
	if (type_cast(&res, CPT_RES_NUMBER))
		cpl_printf("result: %f\n", res.number.number);
	else if (! type_cast(&res, CPT_RES_SUCC))
		cpl_printf("error\n");
	return SUCC;
}

union cpt_result
cb_assign(union tree* tree)
{
	union cpt_result res;
	union cpt_result* sp;
	if (tree->assign.var->type != CPT_VAR)
		return ERROR;
	if (tree->assign.next)
		res = call_tree(tree->assign.next);
	else
		return ERROR;
	if (!type_cast(&res, CPT_RES_NUMBER))
		return ERROR;
	sp = mem_alloc(sizeof(*sp));
	*sp = res;
	table_set_element(& variables, tree->assign.var->variable.name,
				(void *) sp);
	return res;
}

union cpt_result
cb_andor(union tree* tree)
{
	union cpt_result res, op1, op2;
	
	if (tree->andor.first_andor)
		op1 = call_tree(tree->andor.first_andor);
	else
		return ERROR;
	if (!(type_cast(&op1, CPT_RES_NUMBER)))
		return ERROR;
		
	
	if (tree->andor.not)
		op2 = call_tree(tree->andor.not);
	else
		return ERROR;
	if (!type_cast(&op2, CPT_RES_NUMBER))
		return ERROR;
	res.type = CPT_RES_NUMBER;
	
	if (tree->andor.op_type == TOK_AND) {
		if ((op1.number.number) && (op2.number.number))
			res.number.number = 1;
		else
			res.number.number = 0;
	} else {
		if ((op1.number.number) || (op2.number.number))
			res.number.number = 1;
		else
			res.number.number = 0;
	}
	return res;
}

union cpt_result
cb_sum(union tree* tree)
{
	union cpt_result res, op1, op2;
	
	if (tree->sum.left)
		op1 = call_tree(tree->sum.left);
	else
		return ERROR;
	if (!type_cast(&op1, CPT_RES_NUMBER))
		return ERROR;
		
	
	if (tree->sum.right)
		op2 = call_tree(tree->sum.right);
	else
		return ERROR;
	if (!type_cast(&op2, CPT_RES_NUMBER))
		return ERROR;
	
	res.type = CPT_RES_NUMBER;
	if (tree->sum.op_type == TOK_PLUS)
		res.number.number = op1.number.number + op2.number.number;
	else
		res.number.number = op1.number.number - op2.number.number;
	//cpl_printf("sum: %f\n", res.number.number);
	return res;
}

union cpt_result
cb_prod(union tree* tree)
{
	union cpt_result res, op1, op2;
	
	if (tree->prod.left)
		op1 = call_tree(tree->prod.left);
	else
		return ERROR;
	if (!type_cast(&op1, CPT_RES_NUMBER))
		return ERROR;
		
	
	if (tree->prod.right)
		op2 = call_tree(tree->prod.right);
	else
		return ERROR;
	if (!type_cast(&op2, CPT_RES_NUMBER))
		return ERROR;
	res.type = CPT_RES_NUMBER;
	
	if (tree->prod.op_type == TOK_MUL)
		res.number.number = op1.number.number * op2.number.number;
	else if (op2.number.number != 0)
		res.number.number = op1.number.number / op2.number.number;
	else
		return ERROR;
	//cpl_printf("prod: %f\n", res.number.number);
	return res;
}

union cpt_result
cb_number(union tree* tree)
{
	union cpt_result number;
	number.type = CPT_RES_NUMBER;
	number.number.number = tree->number.number;
	return number;
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
	union cpt_result res, op1;
	
	if (tree->not.compare)
		op1 = call_tree(tree->not.compare);
	else
		return ERROR;
	if (!type_cast(&op1, CPT_RES_NUMBER))
		return ERROR;
	res.type = CPT_RES_NUMBER;
	
	res = op1;
	if (tree->not.not) {
		if (res.number.number)
			res.number.number = 0;
		else
			res.number.number = 1;
	}
	return res;
}

union cpt_result
cb_compare(union tree* tree)
{
	union cpt_result res, op1, op2;
	
	if (tree->compare.left)
		op1 = call_tree(tree->compare.left);
	else
		return ERROR;
	if (!type_cast(&op1, CPT_RES_NUMBER))
		return ERROR;
		
	
	if (tree->compare.right)
		op2 = call_tree(tree->compare.right);
	else
		return ERROR;
	if (!type_cast(&op2, CPT_RES_NUMBER))
		return ERROR;
	res.type = CPT_RES_NUMBER;
	
	if (tree->compare.op_type == TOK_MORE) {
		if (op1.number.number > op2.number.number)
			res.number.number = 1;
		else
			res.number.number = 0;
	} else if (tree->compare.op_type == TOK_EQ) {
		if (op1.number.number == op2.number.number)
			res.number.number = 1;
		else
			res.number.number = 0;
	} else {
		if (op1.number.number < op2.number.number)
			res.number.number = 1;
		else
			res.number.number = 0;
	}
	return res;
}

union cpt_result
cb_pow(union tree* tree)
{
	union cpt_result res, op1, op2;
	
	if (tree->pow.left)
		op1 = call_tree(tree->pow.left);
	else
		return ERROR;
	if (!type_cast(&op1, CPT_RES_NUMBER))
		return ERROR;
		
	
	if (tree->pow.right)
		op2 = call_tree(tree->pow.right);
	else
		return ERROR;
	if (!type_cast(&op2, CPT_RES_NUMBER))
		return ERROR;
	res.type = CPT_RES_NUMBER;
	
	res.number.number = pow(op1.number.number, op2.number.number);
	return res;
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
	struct variable* var = (struct variable *) tree;
	union cpt_result res;
	res.type = CPT_RES_NAME;
	res.name.name = mem_strdup(var->name);
	return res;
}

union cpt_result
cb_tcl_string(union tree* tree)
{
	union tcl_tree* tt = (union tcl_tree *) tree;
	tt = NULL;
	return VOID;
}

union cpt_result
cb_function(union tree* tree)
{
	struct function* function = (struct function *) tree;
	struct func_descr* dsc;
	struct typed_args* args;
	struct expression* params;
	int ca = 0, cp = 0;
	union cpt_result res;
	
	if (!table_get_element(functions, function->name,
				(void **) &dsc))
		return ERROR;
	
	args = dsc->args;
	params = function->params->params.expression;
	while (args) {
		ca ++;
		args = args->next;
	}
	args = dsc->args;
	if ((!params) && (!ca))
		return (dsc->caller)(function->name, dsc->args);
	while (params) {
		cp ++;
		if (cp > ca)
			return ERROR;
		if (params->type == CPT_EXPRESSION)
			res = call_tree(params->assign);
		else
			res = call_tree((union tree*) params);
		if (!(type_cast(&res, args->type)))
			return ERROR;
		
		args->param = res;
		if (params->type != CPT_EXPRESSION)
			break;
		args = args->next;
		params = (struct expression*) params->ostexpr;
	}
	if (ca > cp)
		return ERROR;
	
	return (dsc->caller)(function->name, dsc->args);;
}


int
main(int argc, char** argv)
{
	union tree* tree;
	//input = "3 + 1, 2, 4 / (2 - 6) / 5, 2 > 3 and not 4 == 5 or 3 == 3, b = (a = 4) / 2, b, a, getpi()";
	input = argv[1];
	reg_builtin();
	tree = parse_expression();
	if (tree)
		call_tree(tree);
	cpl_printf("\n");
	return (tree == NULL);
}
