#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "pathdecl.h"

#include <stdio.h>

#include "wrxml.h"

int config_debug = 0;

struct paths {
	struct paths* next;
	struct path* path;
};

void wrml(struct attnode* plist);
void wrjson(struct attnode* plist);

static int
cb_action(struct path* path, struct attnode* xml, struct attnode* node)
{
	cpl_debug("cb called\n");
	return 0;
}

static int
cb_empty(struct path* path, struct attnode* xml, struct attnode* node)
{
	struct attnode* n;
	while ((n = atree_subtree_name(xml, "text", 1))) {
		atree_subtree_rm(xml, n);
		dirty_wrong_recursive_dealloc_wo_types(n);
	}
	while ((n = atree_subtree_name(xml, "complex", 1))) {
		atree_subtree_rm(xml, n);
		dirty_wrong_recursive_dealloc_wo_types(n);
	}
	while ((n = atree_subtree_name(xml, "simple", 1))) {
		atree_subtree_rm(xml, n);
		dirty_wrong_recursive_dealloc_wo_types(n);
	}
	return 1;
}

static int
cb_add(struct path* path, struct attnode* xml, struct attnode* node)
{
	char* tagname = atree_get_attr(atree_subtree_name(node, "prms", 1), "value");
	char* text = atree_get_attr(atree_subtree_name(node, "prms", 2), "value");
	struct att_attr* attr;
	
	struct attnode* newnode = attnode("linear");
	attr = att_attr("name", "", "complex");
	attnode_add_attr(newnode, attr);
	
	struct attnode* namenode = attnode("linear");
	attr = att_attr("name", "", "name");
	attnode_add_attr(namenode, attr);
	attr = att_attr("value", "", mem_strdup(tagname));
	attnode_add_attr(namenode, attr);
	attnode_add_subtree(newnode, namenode);
	
	namenode = attnode("linear");
	attr = att_attr("name", "", "text");
	attnode_add_attr(namenode, attr);
	attr = att_attr("value", "", mem_strdup(text));
	attnode_add_attr(namenode, attr);
	attnode_add_subtree(newnode, namenode);
	
	namenode = attnode("linear");
	attr = att_attr("name", "", "name");
	attnode_add_attr(namenode, attr);
	attr = att_attr("value", "", mem_strdup(tagname));
	attnode_add_attr(namenode, attr);
	attnode_add_subtree(newnode, namenode);
	
	if (! attnode_add_after(xml, "param", newnode))
		attnode_add_after(xml, "name", newnode);
	
	return 1;
}

static int
cb_write(struct path* path, struct attnode* xml, struct attnode* node)
{
	struct list_iter list = list_iter(node->subtrees);
	struct list_iter xmlnodes;
	struct attnode* n1;
	char* name,* value;
	while (CONT(& list)) {
		n1 = (struct attnode *) GET_EL(& list);
		name = atree_get_attr(n1, "name");
		value = atree_get_attr(n1, "value");
		if (!strcmp(name, "symb")) {
			printf("%s", value);
		} else if (!strcmp(name, "printesc")) {
			printf("%s", value);
		} else if (!strcmp(name, "specesc")) {
			cpl_debug("Special character\n");
			if (!strcmp(value, "$")) {
				xmlnodes = list_iter(xml->subtrees);
				cpl_debug("Writing nodes\n");
				while (CONT(& xmlnodes)) {
					n1 = (struct attnode *) GET_EL(& xmlnodes);
					name = atree_get_attr(n1, "name");
					value = atree_get_attr(n1, "value");
					cpl_debug("Processing %s\n", name);
					if (!strcmp(name, "text"))
						printf("%s", value);
					NEXT_EL(& xmlnodes);
				}
			}
		}
		NEXT_EL(& list);
	}
	return 0;
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
path(struct exec_descr* ctx, struct attnode* node, struct paths*** paths, void* closure)
{
	struct paths* p;
	struct path** n;
	printf("Generating for path\n");
	
	p = mem_alloc(sizeof(*p));
	//p = mem_alloc(sizeof(*p));
	p->next = NULL;
	
	n = & (p->path);
	go_deep(ctx, node, & n, closure);
	
	**paths = p;
	*paths = &(p->next);
	return 0;
}

static
int
step(struct exec_descr* ctx, struct attnode* node, struct path*** rpath, void* closure)
{
	struct path* path = mem_alloc(sizeof(*path));
	path->next = NULL;
	path->type = PATH_TAG;
	path->cb = NULL;
	printf("Generating for step\n");
	
	struct attnode* name_node = atree_subtree_name(node, "tagname", 1);
	char* tagname = atree_get_attr(name_node, "value");
	
	path->path_step_tag.tagname = mem_strdup(tagname);
	
	**rpath = path;
	*rpath = &(path->next);
	go_deep(ctx, node, rpath, closure);
	return 0;
}

static
int
write(struct exec_descr* ctx, struct attnode* node, struct path*** rpath, void* closure)
{
	struct path* path = mem_alloc(sizeof(*path));
	path->next = NULL;
	path->type = PATH_ACTION;
	path->cb = (path_cb) cb_write;
	path->data = node;
	**rpath = path;
	return 0;
}

static
int
add(struct exec_descr* ctx, struct attnode* node, struct path*** rpath, void* closure)
{
	struct path* path = mem_alloc(sizeof(*path));
	path->next = NULL;
	path->type = PATH_ACTION;
	path->cb = (path_cb) cb_add;
	path->data = node;
	**rpath = path;
	return 0;
}

static
int
empty(struct exec_descr* ctx, struct attnode* node, struct path*** rpath, void* closure)
{
	struct path* path = mem_alloc(sizeof(*path));
	path->next = NULL;
	path->type = PATH_ACTION;
	path->cb = (path_cb) cb_empty;
	path->data = node;
	**rpath = path;
	return 0;
}

static
int
action(struct exec_descr* ctx, struct attnode* node, struct path*** rpath, void* closure)
{
	struct path* path = mem_alloc(sizeof(*path));
	path->next = NULL;
	path->type = PATH_ACTION;
	path->cb = (path_cb)cb_action;
	path->data = node;
	**rpath = path;
	return 0;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "step", (run_atom_cb_t) step, NULL);
	atree_caller_reg(ctx, "write", (run_atom_cb_t) write, NULL);
	atree_caller_reg(ctx, "action", (run_atom_cb_t) action, NULL);
	atree_caller_reg(ctx, "empty", (run_atom_cb_t) empty, NULL);
	atree_caller_reg(ctx, "path", (run_atom_cb_t) path, NULL);
	atree_caller_reg(ctx, "add", (run_atom_cb_t) add, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	struct exec_descr* pathex = reg_cpts();
	struct exec_descr* xmlex = reg_cpts_xml();
	struct attnode* _path = generate_att_tree("path.gr", "path.in", 0);
	struct attnode* _xml = generate_att_tree("plist.gr", argv[1], 0);
	
	struct paths* path = NULL;
	struct paths** dc = &path;
	
	call_atree(pathex, _path, & dc);
	while (path) {
		call_atree(xmlex, _xml, path->path);
		path = path->next;
	}

	//config_debug = 1;
	//wrml(_xml);
	wrjson(_xml);
	
	return 0;
}

