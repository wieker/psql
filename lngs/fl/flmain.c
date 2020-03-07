#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "fl.h"
#include "scalc.h"

int config_debug = 0;

struct fl_context *
fl_context_create()
{
	struct fl_context* ctx = mem_alloc(sizeof(*ctx));
	ctx->preglobal = NULL;
	ctx->result.fl_type = FL_VOID;
	return ctx;
}

int
main(int argc, char** argv)
{
	if (argc > 1)
		config_debug = 1;
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ctx, "intr", (run_atom_cb_t) intr, NULL);
	atree_caller_reg(ctx, "statement", (run_atom_cb_t) statement, NULL);
	atree_caller_reg(ctx, "name", (run_atom_cb_t) name, NULL);
	atree_caller_reg(ctx, "funcall", (run_atom_cb_t) funcall, NULL);
	atree_caller_reg(ctx, "lfunction", (run_atom_cb_t) lfunction, NULL);
	atree_caller_reg(ctx, "defun", (run_atom_cb_t) defun_exec, NULL);
	
	atree_caller_reg(ctx, "concat", (run_atom_cb_t) concat, NULL);
	atree_caller_reg(ctx, "lstring", (run_atom_cb_t) lstring, NULL);
	atree_caller_reg(ctx, "plus", (run_atom_cb_t) plus, NULL);
	
	atree_caller_reg(ctx, "tuplec", (run_atom_cb_t) tuplec, NULL);
	atree_caller_reg(ctx, "tupleh", (run_atom_cb_t) tupleh, NULL);
	atree_caller_reg(ctx, "tuplet", (run_atom_cb_t) tuplet, NULL);
	
	atree_caller_reg(ctx, "minus", (run_atom_cb_t) minus, NULL);
	atree_caller_reg(ctx, "less", (run_atom_cb_t) less, NULL);
	atree_caller_reg(ctx, "more", (run_atom_cb_t) more, NULL);
	atree_caller_reg(ctx, "equal", (run_atom_cb_t) equal, NULL);
	
	atree_caller_reg(ctx, "ifelse", (run_atom_cb_t) ifop, NULL);
	
	
	struct exec_descr* type_ctx = exec_descr();
	atree_caller_set_default_cb(type_ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(type_ctx, "defun", (run_atom_cb_t) defun_global, NULL);
	atree_caller_reg(type_ctx, "lfunction", (run_atom_cb_t) lfunction_global, NULL);
	
	
	struct attnode* indp = generate_att_tree("fl.gr", "test.fl", 0);
	struct fl_context *flctx = fl_context_create();
	if (call_atree(type_ctx, indp, flctx));
		if (call_atree(ctx, indp, flctx))
			return 0;
	
	cpl_printf("FATAL ERROR\n");
	return 0;
}



