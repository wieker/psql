#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "stype.h"
#include "scalc.h"
#include "typel.h"

#include <dlfcn.h>

struct typel_context {
	void* dlhandle;
	struct exec_descr* typel;
	struct exec_descr* work_type_tree;
	struct exec_descr* work_exec_tree;
};

struct complex_container {
	struct exec_descr* to_check;
	struct attnode* check_node;
	struct typel_context* ctx;
	
	struct exec_descr* ctx_to_process;
	struct attnode* node_to_process;
	int tree;
	int pass;
	void* input;
};

static
int
atom_type_setter(struct exec_descr* ctx, struct attnode* node, void* input, char* typename)
{
	struct att_attr* attr;
	cpl_debug("CPTFATAL type inference for cpt %s as atom\n", atree_get_attr(node, "name"));
	attr = att_attr("type1", "psz", typename);
	attnode_add_attr(node, attr);
	return 1;
}

static
int
atom(struct exec_descr* ctx, struct attnode* node, struct typel_context* input, void* closure)
{
	char* cptname = atree_get_subnode_attr(node, 1, "value");
	char* typename = atree_get_subnode_attr(node, 2, "value");
	call_atree(ctx, atree_subtree(node, 3), input);
	void* reader_cb = atree_get_subnode_attr(node, 3, "callback");
	
	//cpl_debug("CPTFATAL registering exec for cpt %s as atom\n", cptname);
	atree_caller_reg(input->work_type_tree, cptname, (run_atom_cb_t) atom_type_setter, typename);
	atree_caller_reg(input->work_exec_tree, cptname, (run_atom_cb_t) reader_cb, NULL);
	return 1;
}

static
int
complex_cb(struct exec_descr* ctx, struct attnode* node, void* input, struct complex_container* closure)
{
	struct complex_container ct;
	cpl_debug("CPTFATAL type inference for cpt %s as complex\n", atree_get_attr(node, "name"));
	ct = *closure;
	ct.node_to_process = node;
	ct.tree = 1;
	ct.pass = 1;
	ct.ctx_to_process = ctx;
	ct.input = input;
	call_atree(closure->to_check, closure->check_node, & ct);
	if (! ct.pass) {
		struct att_attr* attr;
		cpl_debug("Error in type checking for cpt %s\n", atree_get_attr(node, "name"));
		attr = att_attr("type1", "psz", "#error_t");
		attnode_add_attr(node, attr);
	}
	return ct.pass;
}

static
int
executor_runer(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	run_atom_cb_t _cb = (run_atom_cb_t) atree_get_attr(node, "executor");
	_cb(ctx, node, input, closure);
	return 1;
}

static
int
complex(struct exec_descr* ctx, struct attnode* node, struct typel_context* input, void* closure)
{
	char* cptname = atree_get_subnode_attr(node, 1, "value");
	struct attnode* infs_descr = atree_subtree(node, 2);
	
	struct complex_container* ct = mem_alloc(sizeof(*ct));
	ct->to_check = ctx;
	ct->check_node = infs_descr;
	ct->ctx = input;
	
	//cpl_debug("CPTFATAL registering exec for cpt %s as complex\n", cptname);
	atree_caller_reg(input->work_type_tree, cptname, (run_atom_cb_t) complex_cb, ct);
	atree_caller_reg(input->work_exec_tree, cptname, (run_atom_cb_t) executor_runer, ct);
	return 1;
}

static
int
cast(struct exec_descr* ctx, struct attnode* node, struct complex_container* input, void* closure)
{
	
	go_deep_scalc(ctx, node, input, closure);
	
	if (! input->pass)
		return 0;
	int tree = input->tree;
	/*
	 * BUG: should check for count of subtrees
	 * */
	//void* recognitor = atree_get_subnode_attr(node, tree, 	"callback");
	char* typename = atree_get_subnode_attr(node, tree + 1, "value");
	void* executor = atree_get_subnode_attr(node, tree + 2, "callback");
	
	/*
	 * BUG:
	 * should use recognitor ...
	 * and comment all that...
	 * */
	
	struct att_attr* attr;
	attr = att_attr("type1", "psz", typename);
	attnode_add_attr(input->node_to_process, attr);
	
	attr = att_attr("executor", "func_pointer_cb_c", executor);
	attnode_add_attr(input->node_to_process, attr);
	
	input->tree = 1;
	return 1;
}

static
int
castrec(struct exec_descr* ctx, struct attnode* node, struct complex_container* input, void* closure)
{
	
	go_deep_scalc(ctx, node, input->ctx, closure);
	
	if (! input->pass)
		return 0;
	int tree = input->tree;
	cpl_debug("CPTDEEP procesing arg %d for complex %s\n", tree, atree_get_attr(input->node_to_process, "name"));
	/*
	 * BUG: should check for count of subtrees
	 * */
	char* typename = atree_get_subnode_attr(node, 1, 	"value");
	recognitor_t recognitor = (recognitor_t) atree_get_subnode_attr(node, 2, "callback");
	caster_t caster = (caster_t) atree_get_subnode_attr(node, 3, "callback");
	
	struct attnode* processing_node = atree_subtree(input->node_to_process, tree);
	
	/*
	 * BUG:
	 * should use recognitor ...
	 * and comment all that...
	 * */
	
	call_atree(input->ctx_to_process, processing_node, input->input);
	
	input->pass = input->pass && recognitor(processing_node, typename) && caster(processing_node, typename);
	
	input->tree += 1;
	cpl_debug("CPTDEEP finished arg %d for complex %s\n", tree, atree_get_attr(input->node_to_process, "name"));
	return 1;
}

static
int
dl_resolve(struct exec_descr* ctx, struct attnode* node, struct typel_context* input, void* closure)
{
	char* cb = atree_get_attr(node, "value");
	void* handle = dlsym(input->dlhandle, cb);
	struct att_attr* attr;
	attr = att_attr("callback", "func_pointer_cb_c", handle);
	attnode_add_attr(node, attr);
	cpl_debug("dlerror: %s for ##%s##\n", dlerror(), cb);
	return 1;
}

static
int
self(struct exec_descr* ctx, struct attnode* node, struct typel_context* input, void* closure)
{
	char* cptname = atree_get_subnode_attr(node, 1, "value");
	call_atree(ctx, atree_subtree(node, 2), input);
	void* recognitor = atree_get_subnode_attr(node, 2, "callback");
	call_atree(ctx, atree_subtree(node, 3), input);
	void* executor = atree_get_subnode_attr(node, 3, "callback");
	
	cpl_debug("CPTSELF Registering self functions for %s\n", cptname);
	atree_caller_reg(input->work_type_tree, cptname, (run_atom_cb_t) recognitor, NULL);
	atree_caller_reg(input->work_exec_tree, cptname, (run_atom_cb_t) executor, NULL);
	return 1;
}

static
struct exec_descr*
reg_cpts_typel()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ctx, "atom", (run_atom_cb_t) atom, NULL);
	atree_caller_reg(ctx, "complex", (run_atom_cb_t) complex, NULL);
	atree_caller_reg(ctx, "cast", (run_atom_cb_t) cast, NULL);
	atree_caller_reg(ctx, "castrec", (run_atom_cb_t) castrec, NULL);
	atree_caller_reg(ctx, "self", (run_atom_cb_t) self, NULL);
	return ctx;
}

static
struct exec_descr*
reg_cpts_handle()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ctx, "reader", (run_atom_cb_t) dl_resolve, NULL);
	atree_caller_reg(ctx, "executor", (run_atom_cb_t) dl_resolve, NULL);
	atree_caller_reg(ctx, "recognitor", (run_atom_cb_t) dl_resolve, NULL);
	atree_caller_reg(ctx, "caster", (run_atom_cb_t) dl_resolve, NULL);
	return ctx;
}

int
rework_types(struct exec_descr* wtt, struct exec_descr* wet, char* file)
{
	struct attnode* scalc = generate_att_tree("typel.gr", file, 0);
	struct exec_descr* run = reg_cpts_typel();
	struct exec_descr* runh = reg_cpts_handle();
	
	struct typel_context ctx;
	ctx.work_type_tree = wtt;
	ctx.work_exec_tree = wet;
	ctx.dlhandle = dlopen(NULL, RTLD_LAZY);
	
	call_atree(runh, scalc, & ctx);
	call_atree(run, scalc, & ctx);
	
	return 0;
}




