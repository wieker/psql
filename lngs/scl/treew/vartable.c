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

#include "table.h"

int
treew_vartable_init(struct scl_input* input)
{
	input->external_ctx->vartable = NULL;
	input->external_ctx->typetable = NULL;
	return 1;
}

int
treew_vartable_set_from_attr(struct scl_input* input, char* attr1, char* attr2)
{
	char* key = atree_get_attr(input->node_to_process, attr1);
	char* value = atree_get_attr(input->node_to_process, attr2);
	table_set_element((struct table **) & input->external_ctx->vartable, key, value);
	return 1;
}

