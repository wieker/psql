#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdio.h>

//int tabpos;
struct exec_descr* e;
struct exec_descr* p;

void static
header()
{
	printf( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\r\n"
			"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
			);
}

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct list_iter list = list_iter(node->subtrees);
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (CONT(& list)) {
		if (call_atree(ctx, (struct attnode*) GET_EL(& list), input))
			return 1;
		NEXT_EL(& list);
	}
	return 0;
}

static
int
pcomplex(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* tagname = atree_get_attr(name_node, "value");
	printf("<%s", tagname);
	call_atree(e, node, NULL);
	printf(">");
	go_deep(ctx, node, path, closure);
	printf("</%s>", tagname);
	return 0;
}

static
int
wsimple(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(name_node, "value");
	printf("<%s", pname);
	call_atree(e, node, NULL);
	printf("/>");
	return 0;
}

static
int
wtext(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(node, "value");
	printf("%s", pname);
	return 0;
}

static
int
name(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(node, "value");
	printf(" %s", pname);
	return 0;
}

static
int
quoted(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(node, "value");
	printf("=%s", pname);
	return 0;
}

static
int
param(struct exec_descr* ctx, struct attnode* node, void* path, void* closure)
{
	call_atree(p, node, NULL);
	return 0;
}

static
struct exec_descr*
reg_cpts_xml()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "complex", (run_atom_cb_t) pcomplex, NULL);
	atree_caller_reg(ctx, "simple", (run_atom_cb_t) wsimple, NULL);
	atree_caller_reg(ctx, "text", (run_atom_cb_t) wtext, NULL);
	return ctx;
}

static
struct exec_descr*
reg_cpts_param()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "name", (run_atom_cb_t) name, NULL);
	atree_caller_reg(ctx, "quoted", (run_atom_cb_t) quoted, NULL);
	e = exec_descr();
	atree_caller_set_default_cb(e, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(e, "param", (run_atom_cb_t) param, NULL);
	return ctx;
}

void
wrml(struct attnode* plist)
{
	struct exec_descr* d = reg_cpts_xml();
	p = reg_cpts_param();
	header();
	call_atree(d, plist, NULL);
}

