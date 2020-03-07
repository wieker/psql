#include "exec_tree.h"
#include "stdp.h"
#include "exec_op0.h"
#include "utils.h"

void *
cb_op_semicolon(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	return (void *) & VOID;
}

void *
cb_op_list(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* l = tree->cpts;
	
	while (l) {
		if (l->cpt->type == parser_get_cpt(descr->pctx, "left_fig_br")->type) {
			l = l->next;
			continue;
		} else if (l->cpt->type == parser_get_cpt(descr->pctx, "right_fig_br")->type) {
			return (void *) & VOID;
		}
		if (call_tree((struct gen_tree *) l->cpt, descr)-> type == EXEC_ERROR.type)
			return (void *) & EXEC_ERROR;
		l = l->next;
	}
	
	return (void *) & VOID;
}

void *
cb_op_simple(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct type_number* result;
	if (! result_wait("number", tree->cpts->cpt, descr, (struct cpt_result**) &result))
		return (void *) &EXEC_ERROR;
	else {
		cpl_printf("result: %lf\n", result->v);
	}
	
	return (void *) & VOID;
}

void *
cb_op_elif(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* cond = tree->cpts;
	struct cpt_list* body = cond->next;
	struct cpt_list* el = body->next;
	struct type_number* result;
	
	if (! result_wait("number", cond->cpt, descr, (struct cpt_result**) &result))
		return (void *) & EXEC_ERROR;
	if (result->v == 0) {
		if (call_tree((struct gen_tree *) el->cpt, descr)-> type == EXEC_ERROR.type)
			return (void *) & EXEC_ERROR;
		else 
			return (void *) & VOID;
	}
	if (call_tree((struct gen_tree *) body->cpt, descr)-> type == EXEC_ERROR.type)
		return (void *) & EXEC_ERROR;
	
	return (void *) & VOID;
}

void *
cb_op_if(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* cond = tree->cpts;
	struct cpt_list* body = cond->next;
	struct type_number* result;
	
	if (! result_wait("number", cond->cpt, descr, (struct cpt_result**) &result))
		return (void *) & EXEC_ERROR;
	if (result->v == 0)
		return (void *) & VOID;
	if (call_tree((struct gen_tree *) body->cpt, descr)-> type == EXEC_ERROR.type)
		return (void *) & EXEC_ERROR;
	
	return (void *) & VOID;
}

void *
cb_op_while(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* cond = tree->cpts;
	struct cpt_list* body = cond->next;
	struct type_number* result;
	
	for (;;) {
		if (! result_wait("number", cond->cpt, descr, (struct cpt_result**) &result))
			return (void *) &EXEC_ERROR;
		if (result->v == 0)
			return (void *) & VOID;
		if (call_tree((struct gen_tree *) body->cpt, descr)-> type == EXEC_ERROR.type)
			return (void *) & EXEC_ERROR;
	}
	
	return (void *) & VOID;
}

void *
cb_op_break(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	
	return (void *) & VOID;
}

void *
cb_op_continue(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	
	return (void *) & VOID;
}

void *
cb_op_return(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	
	return (void *) & VOID;
}

void *
cb_op_return_void(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	
	return (void *) & VOID;
}

