#include "ptpl2atree.h"

#include "register.h"
#include "utils.h"

#include <stdio.h>

struct cb_info {
	struct attnode* root;
	struct attnode* parent;
	struct attnode* last;
};

static void
enter(struct tree_descr* descr, struct attnode* node)
{
	struct cb_info* info = (struct cb_info*) descr->curr_data;
	
	if (info) {
		node->parent = info->parent;
		node->root = info->root;
		node->prev = info->last;
		node->next = NULL;
		if (node->prev)
			node->prev->next = node;
		info->last = node;
		info->parent = node;
	} else {
		node->root = node;
		node->parent = NULL;
		node->prev = NULL;
		node->next = NULL;
		info = mem_alloc(sizeof(*info));
		info->root = node;
		info->last = node;
		info->parent = node;
		descr->curr_data = info;
	}
}

static void
add_name(struct attnode* node, struct tree_descr* descr, struct gen_tree* tree)
{
	struct att_attr* attr;
	attr = att_attr("name", "", mem_strdup(parser_get_cpt_type(descr->pctx, tree->type)->cptname));
	attnode_add_attr(node, attr);
}


struct attnode*
cb_exec_lop(struct tree_descr* descr, struct tree_op_left_cpt* tree)
{
	struct attnode* node = attnode("left");
	struct att_attr* attr;
	enter(descr, node);
	struct cb_info* info = (struct cb_info*) descr->curr_data;
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	attnode_add_subtree(node, (struct attnode*) call_tree(tree->left, descr));
	info->parent = node;
	
	attr = att_attr("operation", "", parser_get_cpt_type(descr->pctx, tree->optype)->cptname);
	attnode_add_attr(node, attr);
	
	attnode_add_subtree(node, (struct attnode*) call_tree(tree->right, descr));
	info->parent = node;
	
	return node;
}

struct attnode*
cb_exec_rop(struct tree_descr* descr, struct tree_op_right_cpt* tree)
{
	struct attnode* node = attnode("right");
	struct att_attr* attr;
	enter(descr, node);
	struct cb_info* info = (struct cb_info*) descr->curr_data;
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	attnode_add_subtree(node, (struct attnode*) call_tree(tree->left, descr));
	info->parent = node;
	
	attr = att_attr("operation", "", parser_get_cpt_type(descr->pctx, tree->optype)->cptname);
	attnode_add_attr(node, attr);
	
	attnode_add_subtree(node, (struct attnode*) call_tree(tree->right, descr));
	info->parent = node;
	
	return node;
}

struct attnode*
cb_exec_linear(struct tree_descr* descr, struct tree_simple_linear* tree)
{
	struct cpt_list* list = tree->cpts;
	struct attnode* node = attnode("linear");
	struct cpt_list* child = list;
	enter(descr, node);
	struct cb_info* info = (struct cb_info*) descr->curr_data;
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	while (child) {
		attnode_add_subtree(node, (struct attnode*) call_tree(child->cpt, descr));
		info->parent = node;
		child = child->next;
	}
	
	return node;
}

struct attnode*
cb_exec_tone(struct tree_descr* descr, struct gen_tree* tree)
{
	struct attnode* node = attnode("tone");
	enter(descr, node);
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	return node;
}

struct attnode*
cb_exec_tnumber(struct tree_descr* descr, struct tree_number* tree)
{
	struct attnode* node = attnode("tnumber");
	struct att_attr* attr;
	char number[100];
	enter(descr, node);
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	snprintf(number, 100, "%lf", tree->number);
	attr = att_attr("value", "", mem_strdup(number));
	attnode_add_attr(node, attr);
	
	return node;
}

struct attnode*
cb_exec_tstring(struct tree_descr* descr, struct tree_string* tree)
{
	struct attnode* node = attnode("tstring");
	struct att_attr* attr;
	enter(descr, node);
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	attr = att_attr("value", "", tree->string);
	attnode_add_attr(node, attr);
	
	return node;
}

struct attnode*
cb_exec_tchar(struct tree_descr* descr, struct tree_stdechar* tree)
{
	struct attnode* node = attnode("tchar");
	struct att_attr* attr;
	char* string;
	enter(descr, node);
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	string = mem_alloc(2);
	snprintf(string, 2, "%c", tree->chr);
	attr = att_attr("value", "", string);
	attnode_add_attr(node, attr);
	
	return node;
}


struct attnode*
cb_exec_tinteger(struct tree_descr* descr, struct tree_integer* tree)
{
	struct attnode* node = attnode("tinteger");
	struct att_attr* attr;
	char* string;
	enter(descr, node);
	
	add_name(node, descr, (struct gen_tree *) tree);
	
	string = mem_alloc(20);
	snprintf(string, 20, "%d", tree->integer);
	attr = att_attr("value", "", string);
	attnode_add_attr(node, attr);
	
	return node;
}

