
struct sum {
	int type;
	int op;
	union gen_tree* prod1;
	union gen_tree* prod2;
};

struct simple_op {
	int type;
	int op;
	union gen_tree* left;
	union gen_tree* right;
};

union gen_tree*
parse_sum()
{
	union gen_tree* result1, result2, result;
	int tok;
	result1 = parse_cpt("CPT_TEST_CALC_PROD");
	if (parser_check_result(result1, PARSE_FAIL) || parser_check_result(result1, PARSE_RECOVERED))
		return result1;
	do {
		if (! scan_token(TOK_SUM, &tok))
			return result1;
		result2 = parse_cpt("CPT_TEST_CALC_PROD");
		if (parser_check_result(result2, PARSE_FAIL) || parser_check_result(result2, PARSE_RECOVERED)) {
			parser_unref_tree(result1);
			parser_unref_tree(result2);
			return PARSE_FAIL;
		}
		result = parser_alloc_tree("CPT_TEST_CALC_SUM");
		((struct sum *) result)->op = tok;
		((struct sum *) result)->prod1 = result1;
		((struct sum *) result)->prod2 = result2;
		result1 = result;
	} while (1);
	return result;
}

union gen_tree*
parse_left_simple_op(union cpt_parser_cb_info* cb_info)
{
	struct gen_simple_cpt_info* name = cb_info->cpt_info;
	union gen_tree* result1, result2, result;
	int tok;
	result1 = parser_first_cpt(name);
	if (parser_check_result(result1, PARSE_FAIL) || parser_check_result(result1, PARSE_RECOVERED))
		return result1;
	do {
		if (! parser_op_token(name, &tok))
			return result1;
		result2 = parser_next_cpt(name);
		if (parser_check_result(result2, PARSE_FAIL) || parser_check_result(result2, PARSE_RECOVERED)) {
			parser_unref_tree(result1);
			parser_unref_tree(result2);
			return PARSE_FAIL;
		}
		result = parser_alloc_tree(name->name);
		((struct simple_op *) result)->op = tok;
		((struct simple_op *) result)->left = result1;
		((struct simple_op *) result)->right = result2;
		result1 = result;
	} while (1);
	return result;
}

union gen_tree*
parse_right_simple_op(union cpt_parser_cb_info* cb_info)
{
	struct gen_simple_cpt_info* name = cb_info->cpt_info;
	union gen_tree* result1, result2, result;
	int tok;
	
	result1 = parser_next_cpt(name);
	if (parser_check_result(result1, PARSE_FAIL) || parser_check_result(result1, PARSE_RECOVERED))
		return result1;
	if (! parser_op_token(name, &tok))
		return result1;
	result2 = parse_right_simple_op(name);
	if (parser_check_result(result2, PARSE_FAIL) || parser_check_result(result2, PARSE_RECOVERED)) {
		parser_unref_tree(result1);
		parser_unref_tree(result2);
		return PARSE_FAIL;
	}
	result = parser_alloc_tree(name->name);
	((struct simple_op *) result)->op = tok;
	((struct simple_op *) result)->left = result1;
	((struct simple_op *) result)->right = result2;
	return result;
}


