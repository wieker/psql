#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "root.h"
#include <string.h>

int
go_deep_poly(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct list_iter list = list_iter(node->subtrees);
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (CONT(& list)) {
		if (!call_atree(ctx, (struct attnode*) GET_EL(& list), input))
			return 0;
		NEXT_EL(& list);
	}
	return 1;
}

static int
integer(struct exec_descr* ctx, struct attnode* node, struct poly_state* input, struct poly_closure* closure)
{
	char* value = atree_get_attr(node, "value");
	struct complex *r = mem_alloc(sizeof(*r));
	input->N = 1;
	r->angle = 0;
	r->r = atoi(value);
	r->dev = 0;
	input->roots = r;
	printf("Roots number: %d\n", 1);
	return 1;
}

static int
lpow(struct exec_descr* ctx, struct attnode* node, struct poly_state* input, struct poly_closure* closure)
{
	struct attnode* left,* right;
	struct poly_state left_result, right_result;
	struct complex *roots;
	struct complex *temp;
	int i;
	int N;
	
	left = atree_subtree(node, 1);
	if (!call_atree(ctx, left, & left_result))
			return 0;
	
	right = atree_subtree(node, 2);
	if (!call_atree(ctx, right, & right_result))
			return 0;
	
	int nRoots = (int) right_result.roots->r;
	N = left_result.N * nRoots;
	roots = mem_alloc(N * sizeof(*roots));
	for (i = 0; i < left_result.N; i ++) {
		temp = get_roots(nRoots, left_result.roots + i);
		memcpy(roots + i * nRoots, temp, nRoots * sizeof(*roots));
		mem_free(temp);
	}
	
	input->N = N;
	input->roots = roots;
	printf("Roots number (pow): %d\n", N);
	for (i = 0; i < input->N; i ++) {
		printf("%f %f\n", input->roots[i].r, input->roots[i].angle);
	}
	return 1;
}

static int
op(struct exec_descr* ctx, struct attnode* node, struct poly_state* input, struct poly_closure* closure)
{
	struct attnode* left,* ostop,* opcode_tree,* right_tree,* right;
	char* op;
	struct poly_state left_result, right_result;
	struct complex *roots;
	struct complex *temp;
	int N;
	int i, j;
	
	left = atree_subtree(node, 1);
	if (!call_atree(ctx, left, & left_result))
			return 0;
	N = left_result.N;
	roots = left_result.roots;
	
	ostop = atree_subtree(node, 2);
	while (ostop) {
		opcode_tree = atree_subtree(ostop, 1);
		right_tree = atree_subtree(ostop, 2);
		right = atree_subtree(right_tree, 1);
		op = atree_get_attr(opcode_tree, "name");
		
		if (!call_atree(ctx, right, & right_result))
			return 0;
		
		N = right_result.N * left_result.N;
		roots = mem_alloc(N * sizeof(*roots));
		for (i = 0; i < left_result.N; i ++) {
			for (j = 0; j < right_result.N; j ++) {
				if (!strcmp(op, "plus")) {
					temp = cpadd(left_result.roots + i, right_result.roots + j);
				} else if (!strcmp(op, "minus")) {
					temp = cpsub(left_result.roots + i, right_result.roots + j);
				} else if (!strcmp(op, "mul")) {
					temp = cpmul(left_result.roots + i, right_result.roots + j);
				} else if (!strcmp(op, "div")) {
					temp = cpdiv(left_result.roots + i, right_result.roots + j);
				}
				memcpy(roots + i * right_result.N + j, temp, sizeof(*roots));
				mem_free(temp);
			}
		}
		
		left_result.N = N;
		left_result.roots = roots;
		
		ostop = atree_subtree(right_tree, 2);
	}
	
	input->N = left_result.N;
	input->roots = left_result.roots;
	printf("Roots number (op): %d\n", left_result.N);
	for (i = 0; i < left_result.N; i ++) {
		printf("%f %f\n", left_result.roots[i].r, left_result.roots[i].angle);
	}
	return 1;
}

static int
root(struct exec_descr* ctx, struct attnode* node, draw_func draw, struct poly_closure* closure)
{
	struct attnode* left;
	struct poly_state left_result;
	int i;
	
	left = atree_subtree(node, 1);
	if (!call_atree(ctx, left, & left_result))
			return 0;
	printf("Roots number: %d\n", left_result.N);
	
	for (i = 0; i < left_result.N; i ++) {
		printf("%f %f\n", left_result.roots[i].r, left_result.roots[i].angle);
		draw(left_result.roots[i].r * cosf(left_result.roots[i].angle),
				left_result.roots[i].r * sinf(left_result.roots[i].angle));
	}
	return 1;
}

struct exec_descr*
reg_cpts_poly()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep_poly, NULL);
	atree_caller_reg(ctx, "integer", (run_atom_cb_t) integer, NULL);
	atree_caller_reg(ctx, "pow", (run_atom_cb_t) lpow, NULL);
	atree_caller_reg(ctx, "prod", (run_atom_cb_t) op, NULL);
	atree_caller_reg(ctx, "sum", (run_atom_cb_t) op, NULL);
	atree_caller_reg(ctx, "root", (run_atom_cb_t) root, NULL);
	return ctx;
}


