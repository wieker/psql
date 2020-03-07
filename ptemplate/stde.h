#ifndef STDE_H
#define STDE_H

#include "register.h"
#include "parser.h"

struct tree_stdechar {
	struct gen_tree gen_tree;
	char chr;
};

struct tree_integer {
	struct gen_tree gen_tree;
	int integer;
};

struct stde_lexer_onenotsymb* stde_lexer_onenotsymb(struct parser_context* ctx, char* exception, char* name);
struct stde_lexer_backslashsymb* stde_lexer_backslashsymb(struct parser_context* ctx, char* exception, char* name);
struct gen_cpt_info* stde_lexer_integer(struct parser_context* ctx, char* name);
struct gen_cpt_info* stde_lexer_cname(struct parser_context* ctx, char* name);

#endif


