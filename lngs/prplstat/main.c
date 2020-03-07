
#include "gen_attr.h"
#include "wrxml.h"
#include "atrex.h"

#include "utils.h"
#include "atttree_helper.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

struct dt_list {
	struct dt_list* next;
	
	int D;
	int M;
	int Y;
	
	int h;
	int m;
	int s;
};

struct ppl_list {
	struct ppl_list* next;
	char* family;
	char* name;
	struct dt_list* dts;
};

struct stat_s {
	struct ppl_list* start;
	struct ppl_list* stop;
	
	struct ppl_list* all;
	struct ppl_list* curr;
	struct dt_list* day;
} stat_s;

int config_debug = 0;

struct ppl_list *
find_ppl(struct ppl_list* ppl, char* name, char* family)
{
	while (ppl) {
		if (!strcmp(name, ppl->name) && !strcmp(family, ppl->family))
			return ppl;
		ppl = ppl->next;
	}
	return NULL;
}

struct ppl_list *
add_ppl(struct ppl_list* ppl, char* name, char* family)
{
	struct ppl_list* rt;
	rt = mem_alloc(sizeof(*rt));
	rt->next = ppl;
	rt->dts = NULL;
	rt->name = name;
	rt->family = family;
	return rt;
}

struct dt_list *
find_day(struct dt_list* dt, int Y, int M, int D, int h, int m, int s)
{
	while (dt) {
		if (dt->Y == Y && dt->M == M && dt->D == D) {
			if (h < dt->h) {
				dt->h = h;
				dt->m = m;
				dt->s = s;
				return dt;
			}
			if (h == dt->h && m < dt->m) {
				dt->m = m;
				dt->s = s;
				return dt;
			}
			if (h == dt->h && m == dt->m && s < dt->s) {
				dt->s = s;
				return dt;
			}
			return dt;
		}
		dt = dt->next;
	}
	return NULL;
}

struct dt_list *
add_day(struct dt_list* dt, int Y, int M, int D, int h, int m, int s)
{
	struct dt_list* rt;
	rt = mem_alloc(sizeof(*rt));
	rt->next = dt;
	rt->h = h;
	rt->m = m;
	rt->s = s;
	rt->D = D;
	rt->M = M;
	rt->Y = Y;
	return rt;
}

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, struct stat_s* input, void* closure)
{
	node_list* list = node->subtrees;
	
	while (list) {
		call_atree(ctx, (struct attnode*) list->element, input);
		list = list->next;
	}
	return 1;
}

static
int
go_stop(struct exec_descr* ctx, struct attnode* node, struct stat_s* input, void* closure)
{
	node_list* list = node->subtrees;
	
	input->all = input->stop;
	
	while (list) {
		call_atree(ctx, (struct attnode*) list->element, input);
		list = list->next;
	}
	
	input->stop = input->all;
	return 1;
}

static
int
go_start(struct exec_descr* ctx, struct attnode* node, struct stat_s* input, void* closure)
{
	node_list* list = node->subtrees;
	
	input->all = input->start;
	
	while (list) {
		call_atree(ctx, (struct attnode*) list->element, input);
		list = list->next;
	}
	
	input->start = input->all;
	
	return 1;
}

static
int
go_name(struct exec_descr* ctx, struct attnode* node, struct stat_s* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(node, 1), "value");
	char* family = atree_get_attr(atree_subtree(node, 2), "value");
	//printf("%s %s\n", name, family);
	
	input->curr = find_ppl(input->all, name, family);
	if (! input->curr)
		input->all = input->curr = add_ppl(input->all, name, family);

	return 1;
}

static
int
go_date(struct exec_descr* ctx, struct attnode* node, struct stat_s* input, void* closure)
{
	int Y, M, D, h, m, s;
	char* HS;
	sscanf(atree_get_attr(atree_subtree(node, 1), "value"), "%d", & Y);
	sscanf(atree_get_attr(atree_subtree(node, 2), "value"), "%d", & M);
	sscanf(atree_get_attr(atree_subtree(node, 3), "value"), "%d", & D);
	sscanf(atree_get_attr(atree_subtree(node, 4), "value"), "%d", & h);
	sscanf(atree_get_attr(atree_subtree(node, 5), "value"), "%d", & m);
	sscanf(atree_get_attr(atree_subtree(node, 6), "value"), "%d", & s);
	HS = atree_get_attr(atree_subtree(node, 7), "value");
	
	//printf("%s %s\n", input->curr->name, input->curr->family);
	//printf("%d %d %d - %d %d %d %s\n", Y, M, D, h, m, s, HS);
	
	if (h == 12)
		h = 0;
	if (! strcmp(HS, "PM"))
		h += 12;
	
	input->day = find_day(input->curr->dts, Y, M, D, h, m, s);
	if (! input->day)
		input->day = input->curr->dts = add_day(input->curr->dts, Y, M, D, h, m, s);
	
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "stop", (run_atom_cb_t) go_stop, NULL);
	atree_caller_reg(ctx, "start", (run_atom_cb_t) go_start, NULL);
	atree_caller_reg(ctx, "name", (run_atom_cb_t) go_name, NULL);
	atree_caller_reg(ctx, "date", (run_atom_cb_t) go_date, NULL);
	return ctx;
}

void
simple_stat(struct ppl_list* ppl)
{
	float M;
	int l;
	struct dt_list* dt;
	int h, m;
	while (ppl) {
		dt = ppl->dts;
		M = 0;
		l = 0;
		while (dt) {
			M += dt->h * 3600 + dt->m * 60 + dt->s;
			//printf("hour: %d %d %d\n", dt->h, dt->m, dt->s);
			dt = dt->next;
			l ++;
		}
		M /= l;
		h = ((int) M) / 3600;
		m = ((int) M) / 60 % 60;
		cpl_printf("Average time for %s %s:\n%d hours, %d minutes, %d seconds on %d days\n", ppl->name, ppl->family,
						h, m, ((int) M) % 60, l);
		ppl = ppl->next;
	}
}


int
main(int argc, char** argv)
{
	struct attnode *tree = generate_att_tree("prpl.gr", argv[1], 0);
	//config_debug = 1;
	//wrxml(tree);
	config_debug = 0;
	stat_s.all = NULL;
	stat_s.stop = NULL;
	stat_s.start = NULL;
	call_atree(reg_cpts(), tree, & stat_s);
	simple_stat(stat_s.start);
	simple_stat(stat_s.stop);
	return 0;
}

