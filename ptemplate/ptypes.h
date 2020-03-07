#ifndef PTYPES_H
#define PTYPES_H

#include "register.h"
#include "parser.h"

extern struct gen_tree* RECOVERABLE_ERROR;
extern struct gen_tree* ERROR;

int gen_tree_cmp(struct gen_tree* A, struct gen_tree * B);
int parser_check_result(struct parser_context* parser, struct gen_tree* A, struct gen_tree * B);

void ptypes_reg(struct parser_context* ctx);

#endif


