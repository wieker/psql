#ifndef PARSER_H
#define PARSER_H

#include "register.h"

struct gen_tree {
	int type;
};

struct gen_tree* parser_parse(struct parser_context* parser, char* cpt_name);

struct gen_tree* parser_default_alloc(struct gen_cpt_info* cpt);
struct gen_tree* parser_alloc(struct parser_context* ctx, struct gen_cpt_info* cpt);

void parser_default_unref(struct gen_cpt_info* cpt, struct gen_tree* tree);
void parser_unref(struct parser_context* ctx, struct gen_tree* tree);

void parser_default_recover(struct gen_cpt_info* cpt, struct gen_tree* tree);
void parser_recover(struct parser_context* ctx, struct gen_tree* tree);


#endif

