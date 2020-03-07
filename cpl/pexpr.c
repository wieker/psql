#include "tree.h"
#include "tokenizer.h"
#include "utils.h"
#include "table.h"

#include <stdlib.h>

union tree*
parse_expression()
{
	struct expression* expression;
	union tree* assign;
	union tree* ostexpr;
	assign = parse_assign();
	if (! assign)
		return NULL;
	expression = (struct expression *) assign;
	if (scan_token(TOK_COMMA, NULL)) {
		ostexpr = parse_expression();
		if (! ostexpr)
			return NULL;
		expression = mem_alloc(sizeof(* expression));
		expression->type = CPT_EXPRESSION;
		expression->assign = assign;
		expression->ostexpr = ostexpr;
	}
	return (union tree *) expression;
}

union tree*
parse_assign()
{
	struct assign* assign;
	union tree* andor;
	union tree* ostexpr;
	andor = parse_andor();
	if (! andor)
		return NULL;
	assign = (struct assign *) andor;
	if (scan_token(TOK_ASSIGN, NULL)) {
		ostexpr = parse_assign();
		if (! ostexpr)
			return NULL;
		assign = mem_alloc(sizeof(* assign));
		assign->type = CPT_ASSIGN;
		assign->var = andor;
		assign->next = ostexpr;
	}
	return (union tree *) assign;
}

union tree*
parse_andor()
{
	struct andor* andor;
	struct andor* newao;
	int token;
	andor = (struct andor *) parse_not();
	if (! andor)
		return NULL;
	while (scan_token(TOK_ANDOR, &token)) {
		newao = mem_alloc(sizeof(* newao));
		newao->type = CPT_ANDOR;
		newao->first_andor = (union tree *) andor;
		newao->not = parse_not();
		newao->op_type = token;
		andor = newao;
	}
	return (union tree *) andor;
}

union tree*
parse_not()
{
	struct not* not;
	if (scan_token(TOK_NOT, NULL)) {
		not = mem_alloc(sizeof(* not));
		not->type = CPT_NOT;
		not->not = 1;
		not->compare = parse_compare();
		return (union tree *) not;
	}
	return (union tree *) parse_compare();
}

union tree*
parse_compare()
{
	struct compare* compare;
	struct compare* newcmp;
	int token;
	compare = (struct compare*) parse_sum();
	if (! compare)
		return NULL;
	while (scan_token(TOK_COMPARE, &token)) {
		newcmp = mem_alloc(sizeof(* newcmp));
		newcmp->left = (union tree *) compare;
		newcmp->right = parse_sum();
		newcmp->op_type = token;
		compare = newcmp;
		compare->type = CPT_COMPARE;
	}
	return (union tree *) compare;
}

union tree*
parse_sum()
{
	union tree* left;
	struct sum* sum;
	int token;
	sum = (struct sum *) (left = parse_prod());
	if (! sum)
		return NULL;
	while (scan_token(TOK_SUM, &token)) {
		sum = mem_alloc(sizeof(* sum));
		sum->type = CPT_SUM;
		sum->left = left;
		sum->right = parse_prod();
		sum->op_type = token;
		left = (union tree *) sum;
	}
	return (union tree *) sum;
}

union tree*
parse_prod()
{
	union tree* left;
	struct prod* prod;
	int token;
	prod = (struct prod *) (left = parse_pow());
	if (! prod)
		return NULL;
	while (scan_token(TOK_PROD, &token)) {
		prod = mem_alloc(sizeof(* prod));
		prod->type = CPT_PROD;
		prod->left = left;
		prod->right = parse_pow();
		prod->op_type = token;
		left = (union tree *) prod;
	}
	return (union tree *) prod;
}


union tree*
parse_pow()
{
	union tree* left;
	struct pow* pow;
	pow = (struct pow *) (left = parse_value());
	if (! pow)
		return NULL;
	while (scan_token(TOK_POW, NULL)) {
		pow = mem_alloc(sizeof(* pow));
		pow->type = CPT_POW;
		pow->left = left;
		pow->right = parse_value();
		left = (union tree *) pow;
	}
	return (union tree *) pow;
}

union tree*
parse_value()
{
	union tree* value;
	union tree* helper;
	int token;
	char* name;
	
	scan_token(TOK_VALUE, &token);
	switch (token) {
		case TOK_NUMBER:
			value = mem_alloc(sizeof(* value));
			((struct number *) value)->type = CPT_NUMBER;
			((struct number *) value)->number = convert_str_to_number(tok_name);
			return value;
		break;
		case TOK_STRING:
			value = mem_alloc(sizeof(* value));
			((struct string *) value)->type = CPT_STRING;
			((struct string *) value)->string = mem_strdup(tok_name);
			return value;
		break;
		case TOK_NAME:
			name = mem_strdup(tok_name);
			if ((helper = parse_params())) {
				value = mem_alloc(sizeof(* value));
				((struct function *) value)->type = CPT_FUNCTION;
				((struct function *) value)->params = helper;
				((struct function *) value)->name = name;
				return value;
			}/* else {
				((struct column *) value)->type = CPT_COLUMN;
				((struct column *) value)->name = mem_strdup(tok_name);
			}*/
			value = mem_alloc(sizeof(* value));
			value->variable.type = CPT_VAR;
			value->variable.name = name;
			return value;
		break;
		case TOK_LB:
			/*if ((helper = parse_operator()) && scan_token(TOK_RB, NULL))
				return helper;*/
			if ((helper = parse_expression()) && scan_token(TOK_RB, NULL))
				return helper;
		break;
	}
	
	return NULL;
}

union tree*
parse_params()
{
	struct expression* expr;
	struct params* params;
	if ((!scan_token(TOK_LB, NULL)))
		return NULL;
	expr = (struct expression*) parse_expression();
	if (!scan_token(TOK_RB, NULL))
		return NULL;
	params = mem_alloc(sizeof(*params));
	params->type = CPT_FUNC_PARAMS;
	params->expression = expr;
	return (union tree*) params;
}



