#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "typel.h"
#include "scalc.h"


int config_debug = 0;

int
main(int argc, char** argv)
{
	struct exec_descr* wtt = exec_descr();
	atree_caller_set_default_cb(wtt, (run_atom_cb_t) go_deep_scalc, NULL);
	struct exec_descr* wet = exec_descr();
	atree_caller_set_default_cb(wet, (run_atom_cb_t) go_deep_scalc, NULL);
	
	rework_types(wtt, wet, "genle.in");
	
	struct attnode* pic = generate_att_tree("genl.gr", argv[1], 0);
	
	call_atree(wet, pic, NULL);
	
	return 0;
}



