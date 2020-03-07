#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdio.h>

//int tabpos;
struct exec_descr* e;
struct exec_descr* p;

struct comma_state {
	int should_be;
};

void static
header()
{
	printf("{ \"result\" : [");
}

void static
footer()
{
	printf("] }");
}

int static
calculate_len(char* str)
{
	int len = 0;
	while (*str) {
		switch (* str) {
			case '"':
			case '\\':
			case '/':
			case '\b':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			len ++;
		}
		len ++;
		str ++;
	}
	return len;
}

static char*
encode_string(char* str)
{
	int len = calculate_len(str);
	char* newstr = mem_alloc(len + 1);
	char* saved = newstr;
	
	while (*str) {
		switch (* str) {
			case '"':
			case '\\':
			case '/':
			case '\b':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			*newstr ++ = '\\';
		}
		switch (* str) {
			case '"':
			*newstr ++ = '"';
			break;
			case '\\':
			*newstr ++ = '\\';
			break;
			case '/':
			*newstr ++ = '/';
			break;
			case '\b':
			*newstr ++ = 'b';
			break;
			case '\f':
			*newstr ++ = 'f';
			break;
			case '\n':
			*newstr ++ = 'n';
			break;
			case '\r':
			*newstr ++ = 'r';
			break;
			case '\t':
			*newstr ++ = 't';
			break;
			default:
			*newstr ++ = *str;
		}
		str ++;
	}
	*newstr = 0;
	
	return saved;
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
pcomplex(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* tagname = atree_get_attr(name_node, "value");
	if (cs->should_be)
		printf(", ");
	printf("{ \"complex\" : { \"tag\" : \"%s\", \"attrs\" : [ ", tagname);
	
	cs->should_be = 0;
	call_atree(e, node, NULL);
	printf("], \"content\" : [ ");
	cs->should_be = 0;
	go_deep(ctx, node, cs, closure);
	
	printf("] } } ");
	cs->should_be = 1;
	return 0;
}

static
int
wsimple(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* tagname = atree_get_attr(name_node, "value");
	if (cs->should_be)
		printf(", ");
	printf("{ \"simple\" : { \"tag\" : \"%s\", \"attrs\" : [ ", tagname);
	
	cs->should_be = 0;
	call_atree(e, node, cs);
	
	printf("] } } ");
	cs->should_be = 1;
	return 0;
}

static
int
wtext(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = encode_string(atree_get_attr(node, "value")); //MEMORY LEAK
	if (cs->should_be)
		printf(", ");
	printf("{ \"text\" : \"%s\" } ", pname);
	cs->should_be = 1;
	return 0;
}

static
int
name(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(node, "value");
	printf("{ \"%s\" : ", pname);
	return 0;
}

static
int
quoted(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	//struct attnode* name_node = atree_subtree_name(node, "name", 1);
	char* pname = atree_get_attr(node, "value");
	printf(" %s } ", pname);
	return 0;
}

static
int
param(struct exec_descr* ctx, struct attnode* node, struct comma_state* cs, void* closure)
{
	call_atree(p, node, cs);
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
wrjson(struct attnode* plist)
{
	struct exec_descr* d = reg_cpts_xml();
	struct comma_state cs;
	cs.should_be = 0;
	p = reg_cpts_param();
	header();
	call_atree(d, plist, &cs);
	footer();
}


