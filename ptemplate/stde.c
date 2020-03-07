#include "register.h"
#include "parser.h"
#include "ptypes.h"
#include "stde.h"

#include <string.h>

#include "utils.h"
#include "iochannel.h"

#include <stdlib.h>
#include <ctype.h>

#include "tokseek.h"

#include "lextree.h"

struct stde_lexer_onenotsymb {
	struct gen_cpt_info gen_info;
	char* exception;
};

static
struct tree_stdechar *
parse_lexer_onenotsymb(struct stde_lexer_onenotsymb* cpt)
{
	struct tree_stdechar* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	char* ch;
	
	parser->iochannel->begin(parser->iochannel);
	
	ch = parser->iochannel->read(parser->iochannel, 1);
	if (strchr(cpt->exception, *ch) || !*ch) {
		mem_free(ch);
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_stdechar *) RECOVERABLE_ERROR;
	}
	
	tree = (struct tree_stdechar *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->chr = *ch;
	mem_free(ch);
	return tree;
}

struct stde_lexer_onenotsymb *
stde_lexer_onenotsymb(struct parser_context* ctx, char* exception, char* name)
{
	struct stde_lexer_onenotsymb* cpt;
	
	cpt = (struct stde_lexer_onenotsymb *) CPT_ALLOC(ctx, name, cpt, struct tree_stdechar);
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_onenotsymb;
	cpt->exception = strdup(exception);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stde_lexer_backslashsymb {
	struct gen_cpt_info gen_info;
	char* exception;
};

static
struct tree_stdechar *
parse_lexer_backslashsymb(struct stde_lexer_backslashsymb* cpt)
{
	struct tree_stdechar* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	char* ch;
	
	parser->iochannel->begin(parser->iochannel);
	
	ch = parser->iochannel->read(parser->iochannel, 1);
	if (*ch != '\\' || !*ch) {
		mem_free(ch);
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_stdechar *) RECOVERABLE_ERROR;
	}
	mem_free(ch);
	ch = parser->iochannel->read(parser->iochannel, 1);
	if (!strchr(cpt->exception, *ch) || !*ch) {
		mem_free(ch);
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_stdechar *) RECOVERABLE_ERROR;
	}
	
	tree = (struct tree_stdechar *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->chr = *ch;
	mem_free(ch);
	return tree;
}

struct stde_lexer_backslashsymb *
stde_lexer_backslashsymb(struct parser_context* ctx, char* exception, char* name)
{
	struct stde_lexer_backslashsymb* cpt;
	
	cpt = (struct stde_lexer_backslashsymb *) CPT_ALLOC(ctx, name, cpt, struct tree_stdechar);
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_backslashsymb;
	cpt->exception = strdup(exception);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

static
struct tree_integer *
parse_lexer_integer(struct gen_cpt_info* cpt)
{
	struct tree_integer* tree;
	struct parser_context* parser = cpt->ctx;
	unsigned char* ch;
	int s = 0;
	int spaces;
	int flag_float = 0;
	
	parser->iochannel->begin(parser->iochannel);
	spaces = seek_space(parser, parser->iochannel);
	parser->iochannel->read(parser->iochannel, spaces);
	
	while (1) {
		ch = (unsigned char *) parser->iochannel->read(parser->iochannel, 1);
		if (! isdigit(*ch)) {
			if (*ch == '.')
				flag_float = 1;
			mem_free(ch);
			break;
		}
		s ++;
		mem_free(ch);
	}
	
	parser->iochannel->recover(parser->iochannel);
	if (! s || flag_float) {
		return (struct tree_integer *) RECOVERABLE_ERROR;
	}
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, spaces);
	ch = (unsigned char *) parser->iochannel->read(parser->iochannel, s);
	
	tree = (struct tree_integer *) parser_alloc(parser, cpt);
	tree->integer = atoi((char *) ch);
	mem_free(ch);
	return tree;
}

struct gen_cpt_info *
stde_lexer_integer(struct parser_context* ctx, char* name)
{
	struct gen_cpt_info* cpt;
	
	cpt = CPT_ALLOC(ctx, name, cpt, struct tree_integer);
	
	cpt->parse = (gen_cpt_parser_t) parse_lexer_integer;
	
	register_cpt(ctx, cpt);
	
	return cpt;
}

static
struct tree_string *
parse_lexer_cname(struct gen_cpt_info* cpt)
{
	struct tree_string* tree;
	struct parser_context* parser = cpt->ctx;
	char* ch;
	int to_read = 0;
	int spaces;
	
	parser->iochannel->begin(parser->iochannel);
	spaces = seek_space(parser, parser->iochannel);
	parser->iochannel->read(parser->iochannel, spaces);
	
	while (1) {
		ch = parser->iochannel->read(parser->iochannel, 1);
		if (isalpha(*(unsigned char *)ch) || *ch == '_') {
			to_read ++;
			mem_free(ch);
			continue;
		}
		if (isdigit(*(unsigned char *)ch) && to_read > 0) {
			to_read ++;
			mem_free(ch);
			continue;
		}
		break;
	}
	
	parser->iochannel->recover(parser->iochannel);
	if (! to_read) {
		return (struct tree_string *) RECOVERABLE_ERROR;
	}
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, spaces);
	ch = parser->iochannel->read(parser->iochannel, to_read);
	
	tree = (struct tree_string *) parser_alloc(parser, cpt);
	tree->string = mem_strdup(ch);
	mem_free(ch);
	return tree;
}

static
void
unref_lexer_cname(struct gen_cpt_info* cpt, struct tree_string* tree)
{
	mem_free(tree->string);
	mem_free(tree);
}

struct gen_cpt_info *
stde_lexer_cname(struct parser_context* ctx, char* name)
{
	struct gen_cpt_info* cpt;
	
	cpt = CPT_ALLOC(ctx, name, cpt, struct tree_string);
	
	cpt->parse = (gen_cpt_parser_t) parse_lexer_cname;
	cpt->unref = (gen_cpt_unref_t) unref_lexer_cname;
	
	register_cpt(ctx, cpt);
	
	return cpt;
}
