#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "stype.h"
#include "scalc.h"

int
chk_subtree_type(struct attnode* node, int N, char* type)
{
	char* tt = atree_get_subnode_attr(node, N, "type1");
	if (tt)
		return ! strcmp(type, tt);
	return 0;
}

static void
add_type(struct attnode* node, char* type)
{
	struct att_attr* attr;
	attr = att_attr("type1", "psz", type);
	attnode_add_attr(node, attr);
}

static
int
type_number(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	add_type(node, "float");
	return 1;
}

static
int
type_string(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	add_type(node, "string");
	return 1;
}

static
int
type_integer(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	add_type(node, "integer");
	return 1;
}

static
int
type_numbin(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	if (chk_subtree_type(node, 1, "number") && chk_subtree_type(node, 2, "number")) {
		add_type(node, "number");
		return 1;
	} else
		add_type(node, "#error_t");
	return 0;
}

static
int
type_operation(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	add_type(node, atree_get_subnode_attr(node, 1, "type1"));
	return 1;
}


struct exec_descr*
reg_cpts_types()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ctx, "number", (run_atom_cb_t) type_number, NULL);
	atree_caller_reg(ctx, "integer", (run_atom_cb_t) type_integer, NULL);
	atree_caller_reg(ctx, "string", (run_atom_cb_t) type_string, NULL);
	atree_caller_reg(ctx, "sum", (run_atom_cb_t) type_numbin, NULL);
	atree_caller_reg(ctx, "prod", (run_atom_cb_t) type_numbin, NULL);
	atree_caller_reg(ctx, "pow", (run_atom_cb_t) type_numbin, NULL);
	atree_caller_reg(ctx, "operation", (run_atom_cb_t) type_operation, NULL);
	//atree_caller_reg(ctx, "lstname", (run_atom_cb_t) make_list, NULL);
	return ctx;
}

int
equal(struct attnode* node, char* typename)
{
	char* type = atree_get_attr(node, "type1");
	if (! type && (node = atree_subtree(node, 1)))
		return equal(node, typename);
	cpl_debug("type comparing: %s == %s\n", type, typename);
	return type && ! strcmp(type, typename);
}

int
recurse(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	return 1;
}

int
left_type(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	char* ltype = atree_get_subnode_attr(node, 2, "type1");
	attnode_add_attr(node, att_attr("type1", "psz", ltype));
	attnode_add_attr(atree_subtree(node, 1), att_attr("type1", "psz", ltype));
	return 1;
}

int
func_type(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	char* ltype = "float";
	attnode_add_attr(node, att_attr("type1", "psz", ltype));
	return 1;
}
