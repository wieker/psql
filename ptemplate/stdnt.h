#ifndef STDNT_H
#define STDNT_H

#include "register.h"
#include "parser.h"

struct tree_stdnt_quote {
	struct gen_tree gen_tree;
	char* string;
};

struct stdnt_quote* stdnt_quote(struct parser_context* ctx, char* name);


#endif


