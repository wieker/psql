#ifndef STDBE_H
#define STDBE_H

#include "register.h"
#include "parser.h"

struct tree_stdbe_simple {
	struct gen_tree gen_tree;
	char* string;
};

struct stdbe_simple* stdbe_simple(struct parser_context* ctx, char* begin, char* end, char* name);


#endif


