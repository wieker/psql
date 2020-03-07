#include "register.h"
#include "parser.h"
#include "ptypes.h"
#include "stdbe.h"

#include <string.h>

#include "utils.h"
#include "iochannel.h"

struct stdbe_simple {
	struct gen_cpt_info gen_info;
	char* begin;
	char* end;
};


/*
 * parser of segments of RAW text in input stream
 * There is to delimiters: ... BEGIN RAW END ...
 * BEGIN can be not eq to END
 * */
static
struct tree_stdbe_simple *
parse_stdbe_simple(struct stdbe_simple* cpt)
{
	struct parser_context* parser = cpt->gen_info.ctx;
	char* iobuf;
	char* b = cpt->begin;
	char* e = cpt->end;
	int s = 0;
	int bl = strlen(b), el = strlen(e);
	char* strres;
	struct tree_stdbe_simple* tree;
	parser->iochannel->begin(parser->iochannel);
	
	
	/*
	 * Reading open border
	 * */
	while (*(iobuf = parser->iochannel->read(parser->iochannel, 1)) == *b) {
		mem_free(iobuf);
		if (! *b)
			break;
		b ++;
	}
	if (*b) {
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_stdbe_simple *) RECOVERABLE_ERROR;
	}
	
	/*
	 * Restore one character
	 * */
	parser->iochannel->recover(parser->iochannel);
	parser->iochannel->begin(parser->iochannel);
	parser->iochannel->read(parser->iochannel, bl);
	
	
	while (1) {
		/*
		 * Reading close border
		 * */
		parser->iochannel->begin(parser->iochannel);
		while (*(iobuf = parser->iochannel->read(parser->iochannel, 1)) == *e) {
			mem_free(iobuf);
			if (! *e)
				break;
			e ++;
		}
		if (! *e) {
			parser->iochannel->recover(parser->iochannel);
			parser->iochannel->recover(parser->iochannel);
			parser->iochannel->begin(parser->iochannel);
			mem_free(parser->iochannel->read(parser->iochannel, bl));
			strres = parser->iochannel->read(parser->iochannel, s);
			mem_free(parser->iochannel->read(parser->iochannel, el));
			break;
		}
		e = cpt->end;
		parser->iochannel->recover(parser->iochannel);
		/*
		 * counting internal characters
		 * */
		/*
		 * as usual in my code (in other tokenizers this bug so
		 * FATAL BUG: no check for EOF. wrong opening sequence without closed
		 * sequence cause infinity loop (and other tokenizers so...
		 * */
		 /*
		  * BUG fixed; but REMIND for other tokenizers should stay here...*/
		iobuf = parser->iochannel->read(parser->iochannel, 1);
		if (!*iobuf) {
			mem_free(iobuf);
			parser->iochannel->recover(parser->iochannel);
			return (struct tree_stdbe_simple *) RECOVERABLE_ERROR;
		}
		mem_free(iobuf);
		s ++;
	}
	
	tree = (struct tree_stdbe_simple *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	tree->string = strres;
	cpl_debug("stdbe: %d %d %d => %s\n", bl, s, el, strres);
	return tree;
}

static
void
recover_stdbe_simple(struct stdbe_simple* cpt, struct tree_stdbe_simple* tree)
{
	mem_free(tree->string);
	cpt->gen_info.ctx->iochannel->recover(cpt->gen_info.ctx->iochannel);
}

/*
 * Registration function for RAW segments parser
 * */
struct stdbe_simple *
stdbe_simple(struct parser_context* ctx, char* begin, char* end, char* name)
{
	struct stdbe_simple* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_stdbe_simple);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_stdbe_simple;
	cpt->gen_info.unref = (gen_cpt_unref_t) parser_default_unref;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_stdbe_simple;
	cpt->begin = strdup(begin);
	cpt->end = strdup(end);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}
