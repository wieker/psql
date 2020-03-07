enum synnode_type { stat_list, statement, st_decl, st_expr, expression, term, ostexpr, primary, ostterm };

union syntree_node {
	enum synnode_type type;
	enum synnode_type sub_type;
	
	struct {
		enum synnode_type type;
		enum synnode_type sub_type;
		char* funcname;
		union syntree_node* params;
		union syntree_node* expression;
	} node_statement;
	
	struct {
		enum synnode_type type;
		enum synnode_type sub_type;
		union syntree_node* term;
		union syntree_node* ostexpr;
	} node_expression;
	
	struct {
		enum synnode_type type;
		enum synnode_type sub_type;
		union syntree_node* prim;
		union syntree_node* ostterm;
		double number;
	} node_term;
	
	struct {
		enum synnode_type type;
		
	} node_primary;
}

union syntree_node *
parse_statement()
{
	union syntree_node* st = malloc(sizeof(*st));
	st.type = statement;
	get_token();
	switch (curr_tok) {
	case PROC:
		printf("proc declared\n");
		st.sub_type = st_proc;
		st.funcname = scan_name();
		st.params = parse_params();
		st.expression = parse_expr();
		return st;
	default:
		pushback ++;
		st.sub_type = st_expr;
		st.expression = parse_expr();
	}
	return st;
}

union syntree_node *
parse_expression()
{
	union syntree_node* t = parse_term();;
	union syntree_node* ex = malloc(sizeof(*ex));
	union syntree_node* ostexpr;

	ex.type = expression;
	ex.subtype = ex_expr;
	ex.node_expression.term = t;
	ex.node_expression.ostexpr = NULL;
	ostexpr = ex;
	
	for (;;) {
		get_token();
		switch (curr_tok) {
		case PLUS:
			ostexpr = ostexpr.node_expression.ostexpr = malloc(sizeof(*ex));
			ostexpr.type = expression;
			ostexpr.subtype = ex_plus;
			ostexpr.node_expression.term = parse_term();
			ostexpr.node_expression.ostexpr = NULL;
			break;
		case MINUS:
			ostexpr = ostexpr.node_expression.ostexpr = malloc(sizeof(*ex));
			ostexpr.type = expression;
			ostexpr.subtype = ex_minus;
			ostexpr.node_expression.term = parse_term();
			ostexpr.node_expression.ostexpr = NULL;
			break;
		default:
			pushback ++;
		}
	}
	return ex;
}

union syntree_node *
parse_term()
{
	union syntree_node* p = parse_prim();;
	union syntree_node* t = malloc(sizeof(*ex));
	union syntree_node* ostterm;

	t.type = term;
	t.subtype = ex_expr;
	t.node_term.prim = p;
	t.node_term.ostterm = NULL;
	ostterm = t;
	
	for (;;) {
		get_token();
		switch (curr_tok) {
		case MUL:
			ostterm = ostterm.node_term.ostterm = malloc(sizeof(*ex));
			ostterm.type = term;
			ostterm.subtype = t_mul;
			ostterm.node_term.term = parse_prim();
			ostterm.node_term.ostterm = NULL;
			break;
		case DIV:
			ostterm = ostexpr.node_term.ostterm = malloc(sizeof(*ex));
			ostterm.type = term;
			ostterm.subtype = t_div;
			ostterm.node_term.term = parse_prim();
			ostterm.node_term.ostterm = NULL;
			break;
		default:
			pushback ++;
		}
	}
	return t;
}

union syntree_node *
parse_prim()
{
	union syntree_node* prim = malloc(sizeof(*t));
	prim.type = primary;
	
	get_token();

	switch (curr_tok) {
	case LP:
		prim.sub_type = expression;
		prim.node_term.expr = parse_expression();
		break;
	case MINUS:
		prim.subtype = ex_minus;
		prim.node_term.expr = parse_prim();
		break;
	case NUMBER:
		prim.subtype = number;
		prim.node_term.number = tok_value;
		break;
	case NAME:
	}

	return prim;
}



