#include "scalc.h"
#include "stype.h"
#include "utils.h"

void *
qscript_ctx()
{
	struct latest_operation* op = mem_alloc(sizeof(*op));
	return op;
}
