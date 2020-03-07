
void
exec_select(struct select* tree)
{
	struct cursor* cursor = NULL;
	struct row* row_result;
	while (cursor = exec_from(tree->from, cursor)) {
		if (exec_cond(tree->cond)) {
			row_result = exec_select_list(tree->select_list, cursor));
			send_result(row_result);
		}
	}
}



