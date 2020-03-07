#include "register.h"
#include "parser.h"
#include "ptypes.h"
#include "stdbe.h"

#include <string.h>

#include "utils.h"
#include "iochannel.h"
#include "tokseek.h"
#include "stdnt.h"

struct stdnt_quote {
	struct gen_cpt_info gen_info;
};


int static
calculate_len(char* str)
{
	int len = 0;
	int state = 0;
	str ++;
	while (* str) {
		switch (* str) {
			case '\\':
			if (state == 0) {
				state = 1;
				break;
			}
			default:
			len ++;
			state = 0;
		}
		str ++;
	}
	return len;
}

static char*
decode_string(char* str)
{
	int len = calculate_len(str);
	char* newstr = mem_alloc(len);
	char* saved = newstr;
	int state = 0;
	
	str ++;
	while (*str) {
		switch (*str) {
			case '\\':
			if (state == 0) {
				state = 1;
				break;
			}
			default:
			state = 0;
			*newstr ++ = *str;
		}
		str ++;
	}
	newstr --;
	*newstr = 0;
	
	return saved;
}

/*
 * good quote parser
 * '', "", '\'"', "'\""
 * */
static
struct tree_stdnt_quote *
parse_stdnt_quote(struct stdnt_quote* cpt)
{
	struct tree_stdnt_quote* tree;
	struct parser_context* parser = cpt->gen_info.ctx;
	int s;
	char* input;
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
	
	s = seek_doublequote_close(parser->iochannel);
	//cpl_debug("%d = \"\n", s);
	if (s == -1) {
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_stdnt_quote *) RECOVERABLE_ERROR;
	}
	
	parser->iochannel->recover(parser->iochannel);
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, seek_space(parser, parser->iochannel));
	input = parser->iochannel->read(parser->iochannel, s);

	tree = (struct tree_stdnt_quote *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->string = decode_string(input);
	mem_free(input);
	cpl_printf("stdnt_quote: %s\n", tree->string);
	return tree;
}

static
void
recover_stdnt_quote(struct stdnt_quote* cpt, struct tree_stdnt_quote* tree)
{
	mem_free(tree->string);
	cpt->gen_info.ctx->iochannel->recover(cpt->gen_info.ctx->iochannel);
}

/*
 * Registration function for RAW segments parser
 * */
struct stdnt_quote *
stdnt_quote(struct parser_context* ctx, char* name)
{
	struct stdnt_quote* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_stdnt_quote);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_stdnt_quote;
	cpt->gen_info.unref = (gen_cpt_unref_t) parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_stdnt_quote;
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

