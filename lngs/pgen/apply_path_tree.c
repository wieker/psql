
#include "gen_attr.h"

int config_debug = 1;

int
main(int argc, char** argv)
{
	generate_att_tree(argv[1], argv[2], 0);
	return 0;
}

