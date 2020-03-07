#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "scalc.h"
#include "stype.h"

#include "typel.h"

int config_debug = 0;

int
main(int argc, char** argv)
{
	struct latest_operation op;
	
	struct exec_descr* wtt = exec_descr();
	atree_caller_set_default_cb(wtt, (run_atom_cb_t) go_deep_scalc, NULL);
	struct exec_descr* wet = exec_descr();
	atree_caller_set_default_cb(wet, (run_atom_cb_t) go_deep_scalc, NULL);
	
	rework_types(wtt, wet, "qtypel.in");
	
	/*struct exec_descr* run = reg_cpts_srun();
	
	attach_cpts_operator(run);
	attach_cpts_assign(run);
	attach_cpts_func(run);
	call_atree(run, scalc, & op);

	wrxml(scalc);*/
	
	struct attnode* scalc = generate_att_tree("scalc.gr", NULL, 0);
	
	if (! call_atree(wtt, scalc, & op) )
		cpl_debug("type error\n");
	wrxml(scalc);
	call_atree(wet, scalc, & op);
	
	return 0;
}


