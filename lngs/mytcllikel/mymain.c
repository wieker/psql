#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"
#include "scalc.h"

#include "recore.h"

int config_debug = 1;

int
main(int argc, char** argv)
{
	struct recore_pointer ctx;
	struct exec_descr* ex = exec_descr();
	atree_caller_set_default_cb(ex, (run_atom_cb_t) go_deep_scalc, NULL);
	atree_caller_reg(ex, "recore", (run_atom_cb_t) recore, NULL);
	atree_caller_reg(ex, "recoretop", (run_atom_cb_t) recore_top, NULL);
	
	
	struct attnode* pic = generate_att_tree("base.gr", "my.in", 0);
	
	call_atree(ex, pic, & ctx);
	
	return 0;
}



