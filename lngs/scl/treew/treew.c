#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "scl.h"
#include "scalc.h"

int
treew_set_text_attr(struct scl_input* input, char* attr, char* value)
{
	struct att_attr* attr_ = att_attr(attr, "pstrz", value);
	attnode_add_attr(input->node_to_process, attr_);
	return 1;
}

int
treew_set_from_tree(struct attnode* node, char* name, struct scl_input* input, char* attr)
{
	char* value = atree_get_attr(node, name);
	struct att_attr* attr_ = att_attr(attr, "pstrz", value);
	attnode_add_attr(input->node_to_process, attr_);
	return 1;
}


