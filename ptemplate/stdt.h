#ifndef STDT_H
#define STDT_H

#include "register.h"
#include "parser.h"

#include "lextree.h"

struct tree_quoted_string {
	struct gen_tree gen_tree;
	char* string;
};

struct stdt_lexer_quoted_string* stdt_lexer_quoted_string(struct parser_context* ctx, char* name);

struct stdt_lexer_one* stdt_lexer_one(struct parser_context* ctx, char* name, char* token);
struct stdt_lexer_string* stdt_lexer_string(struct parser_context* ctx, char* name);
struct stdt_lexer_number* stdt_lexer_number(struct parser_context* ctx, char* name);
struct gen_cpt_info* stdt_lexer_eol(struct parser_context* ctx, char* name);
struct gen_cpt_info* stdt_lexer_eof(struct parser_context* ctx, char* name);
struct stdt_lexer_specq* stdt_lexer_specq(struct parser_context* ctx, char* delimiter, char* name);
struct stdt_lexer_notsymb* stdt_lexer_notsymb(struct parser_context* ctx, char* delimiter, char* name);
struct gen_cpt_info* stdt_lexer_space(struct parser_context* ctx, char* name);


#endif

