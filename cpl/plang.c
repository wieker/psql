#include "tcltree.h"
#include "tokenizer.h"
#include "utils.h"
#include "table.h"

union tcl_tree *
parse_program()
{
	struct program* program;
	union tcl_tree* operator;
	union tcl_tree* ostprog;
	operator = parse_operator();
	if (! operator)
		return NULL;
	program = (struct program *) operator;
	if (scan_token(TOK_SEMICOLON, NULL)) {
		ostprog = parse_program();
		if (! ostprog)
			return NULL;
		program = (struct program *) mem_alloc(sizeof(* program));
		program->type = CPT_TCL_OPERATOR_LIST;
		program->operator = operator;
		program->ostprog = ostprog;
	}
	return (union tcl_tree *) program;
}

union tcl_tree *
parse_while()
{
	struct tcl_ifop* opwhile;
	union tree* cond;
	union tcl_tree* body;
	int token;
	
	if (!scan_token(TOK_TCL_WHILE, NULL))
		return NULL;
	if (scan_token(TOK_FLB, &token))
		cond = parse_expression();
	scan_token(TOK_FRB, &token);
	if (scan_token(TOK_FLB, &token))
		body = parse_program();
	scan_token(TOK_FRB, &token);
	
	opwhile = mem_alloc(sizeof(*opwhile));
	opwhile->type = CPT_TCL_WHILE;
	opwhile->condition = cond;
	opwhile->body = body;
	return (union tcl_tree *) opwhile;
}

union tcl_tree *
parse_operator()
{
	char* name;
	struct arg_list args;
	struct arg_list* tmp = &args;
	int token;
	struct tcl_operator* operator;
	
	if (!scan_token(TOK_NAME, NULL))
		return NULL;
	if ((operator = (struct tcl_operator *) parse_while()))
		return (union tcl_tree *) operator;
	name = mem_strdup(tok_name);
	while (!scan_token(TOK_SEMICOLON, &token)) {
		tmp->next = mem_alloc(sizeof(*tmp));
		tmp = tmp->next;
		tmp->next = NULL;
		if ((tmp->arg = parse_name()))
			continue;
		if ((tmp->arg = parse_var()))
			continue;
		if ((tmp->arg = parse_tcl_string()))
			continue;
		if (scan_token(TOK_OPEN_BRACKET, &token)) {
			tmp->arg = parse_program();
			scan_token(TOK_CLOSE_BRACKET, &token);
		}
		if (scan_token(TOK_FLB, &token)) {
			scan_token(TOK_BRINS, &token);
			tmp->type = CPT_STRING;
			tmp->string = mem_strdup(tok_name);
			scan_token(TOK_FRB, &token);
		}
	}
	operator = (struct tcl_operator *) mem_alloc(sizeof(*operator));
	operator->type = CPT_OPERATOR;
	operator->name = name;
	operator->args = args.next;
	return (union tcl_tree *) operator;
}

union tcl_tree *
parse_name()
{
	struct tcl_name* name;
	if (!scan_token(TOK_NAME, NULL))
		return NULL;
	name = mem_alloc(sizeof(*name));
	name->type = CPT_NAME;
	name->name = mem_strdup(tok_name);
	return (union tcl_tree *) name;
}

union tcl_tree *
parse_var()
{
	struct tcl_var* var;
	if (!scan_token(TOK_DOLLAR, NULL))
		return NULL;
	if (!scan_token(TOK_NAME, NULL))
		return NULL;
	var = mem_alloc(sizeof(*var));
	var->type = CPT_VAR;
	table_get_or_mark_element(variables, tok_name, (void **) &var->ref);
	return (union tcl_tree *) var;
}

union tcl_tree *
parse_tcl_string()
{
	struct tcl_string* string;
	struct arg_list list;
	struct arg_list* tmp = &list;
	int token;
	if (!scan_token(TOK_DQUOTES, NULL))
		return NULL;
	while (!scan_token(TOK_DQUOTES, &token)) {
		tmp->next = mem_alloc(sizeof(*tmp));
		tmp = tmp->next;
		tmp->next = NULL;
		if ((tmp->arg = parse_var()))
			continue;
		if (scan_token(TOK_TCL_STRIN_DQUOTES, NULL))
			tmp->string = mem_strdup(tok_name);
	}
	string = mem_alloc(sizeof(*string));
	string->list = list.next;
	string->type = CPT_TCL_STRING;
	return (union tcl_tree *) string;
}

