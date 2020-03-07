#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "typel.h"
#include "scalc.h"

#include "picexc_s.h"

int config_debug = 0;

int
main(int argc, char** argv)
{
	struct pic_context op;
	char* progfile;
	char* infile;
	char* outfile;
	if (argc == 4) {
		progfile = argv[1];
		infile = argv[2];
		outfile = argv[3];
	} else {
		progfile = "pic.in";
		infile = "infile.png";
		outfile = "outfile.png";
	}
	
	struct exec_descr* wtt = exec_descr();
	atree_caller_set_default_cb(wtt, (run_atom_cb_t) go_deep_scalc, NULL);
	struct exec_descr* wet = exec_descr();
	atree_caller_set_default_cb(wet, (run_atom_cb_t) go_deep_scalc, NULL);
	
	rework_types(wtt, wet, "pice.in");
	
	struct attnode* pic = generate_att_tree("pic.gr", progfile, 0);
	
	op.file = read_png_file(infile);
	op.r = op.b = op.g = 0;
	
	call_atree(wet, pic, & op);
	write_png_file(outfile, op.file);
	
	return 0;
}



