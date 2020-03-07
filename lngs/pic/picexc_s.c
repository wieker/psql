#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "picexc_s.h"

#include <stdlib.h>
#include <math.h>

#ifndef RGB
#define COLORS 4
#else
#define COLORS 3
#endif

void
set_point(struct pic_context* input, float x, float y)
{
	png_byte* row = input->file->row_pointers[(int) floor(y)];
	png_byte* ptr = &(row[(int) floor(x)*COLORS]);
#ifndef RGB
	ptr[3] = 255;
#endif
	ptr[0] = input->r;
	ptr[1] = input->g;
	ptr[2] = input->b;
	//cpl_debug("point at %d %d\n\n", (int) floor(x), (int) floor(y));
}

int
point(struct exec_descr* ctx, struct attnode* node, struct pic_context* input, void* closure)
{
	float x = atof(atree_get_subnode_attr(node, 1, "value"));
	float y = atof(atree_get_subnode_attr(node, 2, "value"));
	set_point(input, x, y);
	return 1;
}

int
color(struct exec_descr* ctx, struct attnode* node, struct pic_context* input, void* closure)
{
	float r = atof(atree_get_subnode_attr(node, 1, "value"));
	float g = atof(atree_get_subnode_attr(node, 2, "value"));
	float b = atof(atree_get_subnode_attr(node, 3, "value"));
	input->r = r;
	input->g = g;
	input->b = b;
	return 1;
}

int
line(struct exec_descr* ctx, struct attnode* node, struct pic_context* input, void* closure)
{
	float x1 = atof(atree_get_subnode_attr(atree_subtree(node, 1), 1, "value"));
	float y1 = atof(atree_get_subnode_attr(atree_subtree(node, 1), 2, "value"));
	float x2 = atof(atree_get_subnode_attr(atree_subtree(node, 2), 1, "value"));
	float y2 = atof(atree_get_subnode_attr(atree_subtree(node, 2), 2, "value"));
	
	float diff_x = x2 - x1;
	float diff_y = y2 - y1;
	int steps;
	cpl_debug("diff at %f %f\n", diff_x, diff_y);

	if (fabs(diff_x) > fabs(diff_y))
		steps = ceilf(fabs(diff_x));
	else
		steps = ceilf(fabs(diff_y));
	cpl_debug("steps at %d\n", steps);
	diff_x /= steps;
	diff_y /= steps;
	
	int i = 0;
	while (i <= steps) {
		set_point(input, x1, y1);
		x1 += diff_x;
		y1 += diff_y;
		i ++;
	}
	/*
	 * char* cptname = atree_get_subnode_attr(node, 1, "value");
	char* typename = atree_get_subnode_attr(node, 2, "value");
	call_atree(ctx, atree_subtree(node, 3), input);
	void* reader_cb = atree_get_subnode_attr(node, 3, "callback");
	
	cpl_debug("CPTFATAL registering exec for cpt %s as atom\n", cptname);
	atree_caller_reg(input->work_type_tree, cptname, (run_atom_cb_t) atom_type_setter, typename);
	atree_caller_reg(input->work_exec_tree, cptname, (run_atom_cb_t) reader_cb, NULL);
	* */
	return 1;
}

int
triangle(struct exec_descr* ctx, struct attnode* node, struct pic_context* input, void* closure)
{
	
	/*
	 * struct complex_container ct;
	cpl_debug("CPTFATAL type inference for cpt %s as complex\n", atree_get_attr(node, "name"));
	ct = *closure;
	ct.node_to_process = node;
	ct.tree = 1;
	ct.pass = 1;
	ct.ctx_to_process = ctx;
	ct.input = input;
	call_atree(closure->to_check, closure->check_node, & ct);
	if (! ct.pass) {
		struct att_attr* attr;
		cpl_debug("Error in type checking for cpt %s\n", atree_get_attr(node, "name"));
		attr = att_attr("type1", "psz", "#error_t");
		attnode_add_attr(node, attr);
	}
	* */
	return 1;
}
