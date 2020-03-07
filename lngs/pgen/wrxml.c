#include "wrxml.h"
#include "utils.h"
#include "string.h"

struct wrctx {
	int tabpos;
};

void static
wrtab(struct wrctx* ctx)
{
	int i = 0;
	while (i < ctx->tabpos) {
		cpl_debug(" ");
		i ++;
	}
}

void static
wrnode(struct attnode* node, struct wrctx* ctx)
{
	struct list_iter attrs = list_iter(node->attrs), child;
	struct att_attr* attr;
	char* name = NULL;
	while (CONT(& attrs)) {
		attr = (struct att_attr*) GET_EL(& attrs);
		if (! strcmp(attr->name, "name")) {
			name = (char *) attr->value;
			break;
		}
		NEXT_EL(& attrs);
	}
	wrtab(ctx);
	cpl_debug("<");
	if (name)
		cpl_debug("%s", name);
	attrs = list_iter(node->attrs);
	while (CONT(& attrs)) {
		attr = (struct att_attr*) GET_EL(& attrs);
		if (strcmp(attr->name, "name")) {
			cpl_debug(" %s=%s", attr->name, attr->value);
		}
		NEXT_EL(& attrs);
	}
	child = list_iter(node->subtrees);
	if (! CONT(& child)) {
		cpl_debug(" />\n");
		return;
	} else
		cpl_debug(">\n");
	ctx->tabpos ++;
	while (CONT(& child)) {
		wrnode((struct attnode*) GET_EL(& child), ctx);
		NEXT_EL(& child);
	}
	ctx->tabpos --;
	wrtab(ctx);
	cpl_debug("</%s>\n", name);
}

void
wrxml(struct attnode* root)
{
	struct wrctx ctx;
	ctx.tabpos = 0;
	wrnode(root, &ctx);
}
