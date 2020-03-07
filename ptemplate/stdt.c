#include "stdt.h"
#include "iochannel.h"
#include "string.h"
#include "parser.h"
#include "utils.h"
#include "ptypes.h"
#include <ctype.h>
#include <stdio.h>
#include "tokseek.h"

struct stdt_lexer_one {
	struct gen_cpt_info gen_info;
	char* token;
	int len;
};

static
struct gen_tree *
parse_lexer_one(struct stdt_lexer_one* cpt)
{
	struct gen_tree* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	cpt->gen_info.ctx->iochannel->begin(cpt->gen_info.ctx->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, cpt->gen_info.ctx->iochannel));
	//char* iobuf = parser->iochannel->read(parser->iochannel, cpt->len);
	//cpl_debug("token = %s\n", iobuf);
	//if (iobuf && !strcmp(iobuf, cpt->token)/* && !seek_simple_key(parser->iochannel)*/) {
	if (seek_const(parser->iochannel, cpt->token)) {
		//mem_free(iobuf);
		tree = parser_alloc(parser, (struct gen_cpt_info *) cpt);
		return tree;
	}
	//mem_free(iobuf);
	parser->iochannel->recover(parser->iochannel);
	return RECOVERABLE_ERROR;
}

static
void
recover_lexer_one(struct stdt_lexer_one* cpt, struct gen_tree* tree)
{
	cpt->gen_info.ctx->iochannel->recover(cpt->gen_info.ctx->iochannel);
}

struct stdt_lexer_one *
stdt_lexer_one(struct parser_context* ctx, char* name, char* token)
{
	struct stdt_lexer_one* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct gen_tree);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_one;
	cpt->gen_info.unref = parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_one;
	
	cpt->token = mem_strdup(token);
	cpt->len = strlen(token);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stdt_lexer_string {
	struct gen_cpt_info gen_info;
};

static
struct tree_string *
parse_lexer_string(struct stdt_lexer_string* cpt)
{
	struct tree_string* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	cpt->gen_info.ctx->iochannel->begin(cpt->gen_info.ctx->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, cpt->gen_info.ctx->iochannel));
	int l = seek_simple_key(cpt->gen_info.ctx->iochannel);
	if (l) {
		char* iobuf = parser->iochannel->read(parser->iochannel, l);
		tree = (struct tree_string *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
		tree->string = iobuf;
		//cpl_debug("%d = =>%s<=\n", l, iobuf);
		return tree;
	}
	parser->iochannel->recover(parser->iochannel);
	return (struct tree_string *) RECOVERABLE_ERROR;
}

static
void
recover_lexer_string(struct stdt_lexer_string* cpt, struct tree_string* tree)
{
	cpt->gen_info.ctx->iochannel->recover(cpt->gen_info.ctx->iochannel);
}

static
void
unref_lexer_string(struct stdt_lexer_string* cpt, struct tree_string* tree)
{
	mem_free(tree->string);
	mem_free(tree);
}

struct stdt_lexer_string *
stdt_lexer_string(struct parser_context* ctx, char* name)
{
	struct stdt_lexer_string* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_string);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_string;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_lexer_string;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_string;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stdt_lexer_number {
	struct gen_cpt_info gen_info;
};

static
struct tree_number *
parse_lexer_number(struct stdt_lexer_number* cpt)
{
	struct tree_number* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	cpt->gen_info.ctx->iochannel->begin(cpt->gen_info.ctx->iochannel);
	//cpl_printf("spaces = %d\n", seek_space(cpt->gen_info.ctx->iochannel));
	parser->iochannel->read(parser->iochannel, seek_space(parser, cpt->gen_info.ctx->iochannel));
	int l = seek_simple_number(cpt->gen_info.ctx->iochannel);
	//cpl_printf("l = %d\n", l);
	if (l) {
		char* iobuf = parser->iochannel->read(parser->iochannel, l);
		int d = seek_dot_number(cpt->gen_info.ctx->iochannel);
		//cpl_printf("l = %d d = %d\n", l, d);
		if (d) {
			mem_free(iobuf);
			parser->iochannel->recover(parser->iochannel);
			parser->iochannel->read(parser->iochannel, seek_space(parser, cpt->gen_info.ctx->iochannel));
			iobuf = parser->iochannel->read(parser->iochannel, l + 1 + d);
		}
		tree = (struct tree_number *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
		sscanf(iobuf, "%lf", &tree->number);
		return tree;
	}
	parser->iochannel->recover(parser->iochannel);
	return (struct tree_number *) RECOVERABLE_ERROR;
}

struct stdt_lexer_number *
stdt_lexer_number(struct parser_context* ctx, char* name)
{
	struct stdt_lexer_number* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_number);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = parser_default_recover;
	cpt->gen_info.unref = parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_number;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stdt_lexer_quoted_string {
	struct gen_cpt_info gen_info;
};

static
struct tree_quoted_string *
parse_lexer_quoted_string(struct stdt_lexer_quoted_string* cpt)
{
	struct tree_quoted_string* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	int s, ps, type, ptype = 0;
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
	
	ps = s = seek_backslash(parser->iochannel, &type);
	//cpl_debug("%d = \"\n", s);
	if (s != 1 || type != 2) {
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_quoted_string *) RECOVERABLE_ERROR;
	}
	
	while (1) {
		ps += s = seek_backslash(parser->iochannel, &type);
		if (s == -1) {
			parser->iochannel->recover(parser->iochannel);
			return (struct tree_quoted_string *) RECOVERABLE_ERROR;
		}
		if (type == 2 && s > 1) {
			parser->iochannel->recover(parser->iochannel);
			parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
			char* iobuf = parser->iochannel->read(parser->iochannel, ps);
			tree = (struct tree_quoted_string *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
			tree->string = iobuf;
			//cpl_printf("%s\n", iobuf);
			return tree;
		}
		if (type == 2 && s == 1 && ptype != 1) {
			parser->iochannel->recover(parser->iochannel);
			parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
			char* iobuf = parser->iochannel->read(parser->iochannel, ps);
			tree = (struct tree_quoted_string *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
			tree->string = iobuf;
			//cpl_printf("%s\n", iobuf);
			return tree;
		}
		if (type == 2 && s == 1 && ptype == 1) {
			ptype = 0;
		}
		if (type == 1 && s == 1 && ptype == 1) {
			ptype = 0;
		}
		if (type == 1 && s == 1 && ptype != 1) {
			ptype = 1;
		}
		if (type == 1 && s > 1) {
			ptype = 1;
		}
		
		
	}
	parser->iochannel->recover(parser->iochannel);
	return (struct tree_quoted_string *) RECOVERABLE_ERROR;
}

struct stdt_lexer_quoted_string *
stdt_lexer_quoted_string(struct parser_context* ctx, char* name)
{
	struct stdt_lexer_quoted_string* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_quoted_string);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_string;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_lexer_string;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_quoted_string;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

static
struct gen_tree *
parse_lexer_eol(struct gen_cpt_info* cpt)
{
	struct parser_context* parser = cpt->ctx;
	char* iobuf;
	parser->iochannel->begin(parser->iochannel);
	iobuf = parser->iochannel->read(parser->iochannel, 1);
	//iobuf = parser->iochannel->read(parser->iochannel, 1);
	//cpl_debug("eol: %s<=\n", iobuf);
	if (iobuf && !strcmp(iobuf, "\n")/* && !seek_simple_key(parser->iochannel)*/) {
		mem_free(iobuf);
		return parser_alloc(parser, cpt);
	}
	mem_free(iobuf);
	parser->iochannel->recover(parser->iochannel);
	return RECOVERABLE_ERROR;
}

struct gen_cpt_info *
stdt_lexer_eol(struct parser_context* ctx, char* name)
{
	struct gen_cpt_info* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->ctx = ctx;
	cpt->cptname = mem_strdup(name);
	cpt->cpt_len = sizeof(struct gen_cpt_info);
	cpt->alloc = parser_default_alloc;
	cpt->recover = (gen_cpt_restore_t) recover_lexer_one;
	cpt->unref = (gen_cpt_unref_t) parser_default_unref;
	
	cpt->parse = (gen_cpt_parser_t) parse_lexer_eol;
	
	register_cpt(ctx, cpt);
	
	return cpt;
}

static
struct gen_tree *
parse_lexer_eof(struct gen_cpt_info* cpt)
{
	struct parser_context* parser = cpt->ctx;
	char* ch;
	parser->iochannel->begin(parser->iochannel);
	ch = parser->iochannel->read(parser->iochannel, 1);
	if (*ch == 0)
		return parser_alloc(parser, cpt);
	parser->iochannel->recover(parser->iochannel);
	return RECOVERABLE_ERROR;
}

struct gen_cpt_info *
stdt_lexer_eof(struct parser_context* ctx, char* name)
{
	struct gen_cpt_info* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->ctx = ctx;
	cpt->cptname = mem_strdup(name);
	cpt->cpt_len = sizeof(struct gen_cpt_info);
	cpt->alloc = parser_default_alloc;
	cpt->recover = (gen_cpt_restore_t) recover_lexer_one;
	cpt->unref = (gen_cpt_unref_t) parser_default_unref;
	
	cpt->parse = (gen_cpt_parser_t) parse_lexer_eof;
	
	register_cpt(ctx, cpt);
	
	return cpt;
}

struct stdt_lexer_one *
stdt_lexer_expr(struct parser_context* ctx, char* name, char* token)
{
	struct stdt_lexer_one* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_string);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_one;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_lexer_string;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_one;
	
	cpt->token = mem_strdup(token);
	cpt->len = strlen(token);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stdt_lexer_specq {
	struct gen_cpt_info gen_info;
	char* delimiter;
};

static
struct tree_quoted_string *
parse_lexer_specq(struct stdt_lexer_specq* cpt)
{
	struct tree_quoted_string* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	int s = 0;
	
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
	
	parser->iochannel->begin(parser->iochannel);
	cpl_debug("Seeking for delimiter %s in %.3s\n", cpt->delimiter, parser->iochannel->read(parser->iochannel, 3));
	parser->iochannel->recover(parser->iochannel);
	if (! seek_const(parser->iochannel, cpt->delimiter)) {
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_quoted_string *) RECOVERABLE_ERROR;
	}
	cpl_debug("Found!\n");
	
	while (1) {
		parser->iochannel->begin(parser->iochannel);
		if (seek_const(parser->iochannel, cpt->delimiter))
			break;
		parser->iochannel->recover(parser->iochannel);
		parser->iochannel->read(parser->iochannel, 1);
		s ++;
	}
	parser->iochannel->recover(parser->iochannel);
	parser->iochannel->recover(parser->iochannel);
	
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
	seek_const(parser->iochannel, cpt->delimiter);
	tree = (struct tree_quoted_string *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->string = parser->iochannel->read(parser->iochannel, s);
	seek_const(parser->iochannel, cpt->delimiter);
	cpl_debug("Read: %s\n", tree->string);
	return tree;
}

struct stdt_lexer_specq *
stdt_lexer_specq(struct parser_context* ctx, char* delimiter, char* name)
{
	struct stdt_lexer_specq* cpt;
	delimiter = strdup(delimiter);
	delimiter[strlen(delimiter) - 1] = 0;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_quoted_string);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_string;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_lexer_string;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_specq;
	cpt->delimiter = strdup(delimiter + 1);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	mem_free(delimiter);
	
	return cpt;
}

struct stdt_lexer_notsymb {
	struct gen_cpt_info gen_info;
	char* delimiter;
};

static
struct tree_quoted_string *
parse_lexer_notsymb(struct stdt_lexer_notsymb* cpt)
{
	struct tree_quoted_string* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	int s = 0;
	
	parser->iochannel->begin(parser->iochannel);
	
	while (1) {
		if (strchr(cpt->delimiter, * parser->iochannel->read(parser->iochannel, 1)))
			break;
		s ++;
	}
	parser->iochannel->recover(parser->iochannel);
	if (! s) {
		return (struct tree_quoted_string *) RECOVERABLE_ERROR;
	}
	
	parser->iochannel->begin(parser->iochannel);
	tree = (struct tree_quoted_string *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->string = parser->iochannel->read(parser->iochannel, s);
	return tree;
}

struct stdt_lexer_notsymb *
stdt_lexer_notsymb(struct parser_context* ctx, char* delimiter, char* name)
{
	struct stdt_lexer_notsymb* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_quoted_string);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_lexer_string;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_lexer_string;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_lexer_notsymb;
	cpt->delimiter = strdup(delimiter);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

static
struct gen_tree *
parse_lexer_space(struct gen_cpt_info* cpt)
{
	struct parser_context* parser = cpt->ctx;
	char* iobuf;
	parser->iochannel->begin(parser->iochannel);
	iobuf = parser->iochannel->read(parser->iochannel, 1);
	cpl_debug("read #%s#\n", iobuf);
	if (iobuf && *iobuf && strchr(parser->spaces, *iobuf)) {
		mem_free(iobuf);
		return parser_alloc(parser, cpt);
	}
	mem_free(iobuf);
	parser->iochannel->recover(parser->iochannel);
	return RECOVERABLE_ERROR;
}

struct gen_cpt_info *
stdt_lexer_space(struct parser_context* ctx, char* name)
{
	struct gen_cpt_info* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->ctx = ctx;
	cpt->cptname = mem_strdup(name);
	cpt->cpt_len = sizeof(struct gen_tree);
	cpt->alloc = parser_default_alloc;
	cpt->recover = (gen_cpt_restore_t) recover_lexer_one;
	cpt->unref = (gen_cpt_unref_t) parser_default_unref;
	
	cpt->parse = (gen_cpt_parser_t) parse_lexer_space;
	
	register_cpt(ctx, cpt);
	
	return cpt;
}
