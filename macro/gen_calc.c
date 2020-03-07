

int
main(int argc, char** argv)
{
	struct gen_simple_cpt_info* expression_cpt = gen_simple_cpt("expression");
	gen_simple_cpt_next(expression_cpt, "assign");
	gen_simple_cpt_scan(expression_cpt, TOK_COMMA);
	register_cpt_parser_one("expression", parse_right_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	struct gen_simple_cpt_info* assign_cpt = gen_simple_cpt("assign");
	gen_simple_cpt_next(expression_cpt, "andor");
	gen_simple_cpt_scan(expression_cpt, TOK_ASSIGN);
	register_cpt_parser_one("assign", parse_right_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	struct gen_simple_cpt_info* andor_cpt = gen_simple_cpt("andor");
	gen_simple_cpt_next(expression_cpt, "not");
	gen_simple_cpt_scan(expression_cpt, TOK_ANDOR);
	register_cpt_parser_one("andor", parse_left_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	register_cpt_parser_one("not", parse_cpt_null,
			sizeof(struct not), parse_not);
	
	
	struct gen_simple_cpt_info* andor_cpt = gen_simple_cpt("compare");
	gen_simple_cpt_next(expression_cpt, "sum");
	gen_simple_cpt_scan(expression_cpt, TOK_COMPARE);
	register_cpt_parser_one("compare", parse_left_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	struct gen_simple_cpt_info* andor_cpt = gen_simple_cpt("sum");
	gen_simple_cpt_next(expression_cpt, "prod");
	gen_simple_cpt_scan(expression_cpt, TOK_SUM);
	register_cpt_parser_one("sum", parse_left_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	struct gen_simple_cpt_info* andor_cpt = gen_simple_cpt("prod");
	gen_simple_cpt_next(expression_cpt, "pow");
	gen_simple_cpt_scan(expression_cpt, TOK_PROD);
	register_cpt_parser_one("prod", parse_left_simple_op,
			sizeof(struct simple_op), expression_cpt);
	
	
	struct gen_simple_cpt_info* andor_cpt = gen_simple_cpt("pow");
	gen_simple_cpt_next(expression_cpt, "value");
	gen_simple_cpt_scan(expression_cpt, TOK_POW);
	register_cpt_parser_one("pow", parse_right_simple_op,
			sizeof(struct simple_op), expression_cpt);
	return 0;
}
