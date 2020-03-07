#ifndef STDP_H
#define STDP_H

#include "register.h"
#include "iochannel.h"
#include "parser.h"

struct cpt_list {
	struct gen_tree* cpt;
	struct cpt_list* next;
};

struct tree_simple_linear {
	struct gen_tree gen_tree;
	
	struct cpt_list* cpts;
	struct transact_list* rec_point;
};

struct gen_simple_linear_cpt* gen_simple_linear_cpt(struct parser_context* ctx, char* name);

void gen_simple_linear_cpt_next(struct gen_simple_linear_cpt* cpt, char* name);

void gen_simple_linear_token_next(struct gen_simple_linear_cpt* cpt, char* name);

void gen_simple_linear_cpt_next_more(struct gen_simple_linear_cpt* cpt, char* name);

struct gen_simple_select_cpt* gen_simple_select_cpt(struct parser_context* ctx, char* name);

void gen_simple_select_cpt_next_select(struct gen_simple_select_cpt* cpt, char* name);

struct gen_negative_cpt * gen_negative_cpt(struct parser_context* ctx, char* name, char* negative);


#endif
