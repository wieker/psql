int
general_executor(struct tree* tree, struct rttree* rt)
{
	int succ = 1;
	struct tree*  symbol;
	struct rttree* vsp;
	while (symbol = get_next_symb(tree, succ)) {
		succ = general_executor(symbol);
	}
}
