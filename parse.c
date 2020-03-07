struct tree;

struct select {
	int type;
	union tree* select_list;
	union tree* from;
	union tree* where;
};

struct select_list {
	int type;
	union tree* expression;
};

struct expression {
	int type;
	union tree* scalar;
	struct expression* ostexpr;
};

union tree {
	int type;
	struct select select;
	struct select_list select_list;
	struct expression expression;
};

union tree *
parse_select()
{
	struct select select;
	if (! scan_token(TOK_SELECT, SCAN_SEEK))
		return 0;
	select.type = CPT_SELECT;
	select.select_list = parse_select_list();
	select.from = parse_from();
	select.where = parse_where();
	return strndup(&select, sizeof(select));
}

union tree*
parse_select_list()
{
	struct select_list select_list;
	select_list.type = CPT_SELECT_LIST;
	if (scan_token(TOK_SELALL, SCAN_SEEK))
		select_list.expression = 0;
	else
		select_list.expression = parse_expression();
	return strndup(&select_list, sizeof(select_list));
}

union tree*
parse_expression()
{
	struct expression expression;
	expression.type = CPT_EXPRESSION;
	if (! (expression.scalar = parse_scalar()))
		return 0;
	expression.ostexpr = NULL;
	if (scan_token(TOK_COMMA, SCAN_SEEK))
		expression.ostexpr = parse_expression();
	return strndup(&expression, sizeof(expression));
}

union tree*
pasre_scalar()
{
	struct scalar scalar;
	
	return strndup(&scalar, sizeof(scalar));
}


