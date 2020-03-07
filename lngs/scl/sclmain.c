#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "typel.h"
#include "scalc.h"

#include "fl.h"
#include "scl.h"


void stm_run();
//void stm_init(int *_code, int *_data);

int config_debug = 0;

int
main(int argc, char** argv)
{
	/*
	 * Old executors; needed to registrate modern scl executor
	 * */
	struct exec_descr* wtt = exec_descr();
	atree_caller_set_default_cb(wtt, (run_atom_cb_t) go_deep_scalc, NULL);
	struct exec_descr* wet = exec_descr();
	atree_caller_set_default_cb(wet, (run_atom_cb_t) go_deep_scalc, NULL);
	rework_types(wtt, wet, "scle.in");
	
	/*
	 * Registration of type assignment unit
	 * */
	struct attnode* scl_type = generate_att_tree("scl.gr", "typesys.scl", 0);
	struct scl_reg reg_type;
	reg_type.data_tree_ctx = exec_descr();
	atree_caller_set_default_cb(reg_type.data_tree_ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	call_atree(wet, scl_type, & reg_type);
	
	/*
	 * Registration of code generation unit
	 * */
	/*struct attnode* scl_gen = generate_att_tree("scl.gr", "scl.in", 0);
	struct scl_reg reg_gen;
	reg_gen.data_tree_ctx = exec_descr();
	atree_caller_set_default_cb(reg_gen.data_tree_ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	call_atree(wet, scl_gen, & reg_gen);*/
	
	/*
	 * Registration of FL unit
	 * */
	/*struct attnode* scl_fl = generate_att_tree("scl.gr", "fl.scl", 0);
	struct scl_reg reg_fl;
	reg_fl.data_tree_ctx = exec_descr();
	atree_caller_set_default_cb(reg_fl.data_tree_ctx, (run_atom_cb_t) go_deep_scalc, NULL);
	call_atree(wet, scl_fl, & reg_fl);*/
	
	
	struct attnode* indp = generate_att_tree("calc.gr", "simple.in", 0);
	//call_atree(reg_type.data_tree_ctx, indp, NULL);
	//call_atree(reg_gen.data_tree_ctx, indp, NULL);
	
	struct scl_external_ctx* external_ctx = mem_alloc(sizeof(*external_ctx));
	call_atree(reg_type.data_tree_ctx, indp, external_ctx);
	
	//stm_run();
	config_debug = 1;
	wrxml(indp);
	
	return 0;
}



