#include "utils.h"
#include "tokenizer.h"
#include <string.h>
#include "cpts.h"
#include <stdlib.h>

/*
 * SYMBOL = TERM
 * SYMBOL = NONTERM
 * NONTERM_DESCR = NAME = [ NAME | TERM ]
 * */

union mp_tree;

struct symb_list {
	struct symb_list* next;
	int isterm;
	union {
		char* term;
		char* nonterm;
	};
};

struct grammar {
	int type;
	union mp_tree* tdescr;
	union mp_tree* next;
};

struct tdescr {
	int type;
	char* name;
	union mp_tree* dbody;
};

struct dbody {
	int type;
	union mp_tree* dcase;
	union mp_tree* next;
};

struct dcase {
	int type;
	union mp_tree* symbol;
	union mp_tree* next;
};

struct nonterm {
	int type;
	char* name;
};

struct term {
	int type;
	char* name;
};

union mp_tree {
	int type;
	struct tdescr tdescr;
	struct dbody dbody;
	struct dcase dcase;
	struct nonterm nonterm;
	struct term term;
};

#define CPT_GRAMMAR 6
#define CPT_TDESCR  1
#define CPT_DBODY  2
#define CPT_DCASE  3
#define CPT_TERM  4
#define CPT_NONTERM  5

union mp_tree* parse_tdescr();
union mp_tree* parse_dbody();
union mp_tree* parse_dcase();
union mp_tree* parse_term();
union mp_tree* parse_nonterm();

union cpt_result gen_grammar();
union cpt_result gen_tdescr();
union cpt_result gen_dbody();
union cpt_result gen_dcase();
union cpt_result gen_term();
union cpt_result gen_nonterm();

struct cpt_descr grammar_cpt_descr[] = {
	{CPT_GRAMMAR, "grammar", gen_grammar,},
	{CPT_TDESCR, "tdescr", gen_tdescr,},
	{CPT_DBODY, "dbody", gen_dbody,},
	{CPT_DCASE, "dcase", gen_dcase,},
	{CPT_TERM, "term", gen_term,},
	{CPT_NONTERM, "nonterm", gen_nonterm,},
	{0, NULL, NULL},
};

union mp_tree *
parse_grammar()
{
	struct grammar* grammar;
	union mp_tree* tdescr;
	
	if (!(tdescr = parse_tdescr()))
		return NULL;
	if (!scan_token(TOK_SEMICOLON, NULL))
		return NULL;
	
	grammar = mem_alloc(sizeof(*grammar));
	grammar->type = CPT_GRAMMAR;
	grammar->next = parse_grammar();
	grammar->tdescr = tdescr;
	
	return (union mp_tree *) grammar;
}

union mp_tree *
parse_tdescr()
{
	struct tdescr* tdescr;
	char* name;
	
	if (!scan_token(TOK_NAME, NULL))
		return NULL;
	name = mem_strdup(tok_name);
	if (!scan_token(TOK_EQUAL, NULL)) {
		free(name);
		return NULL;
	}
	
	tdescr = mem_alloc(sizeof(*tdescr));
	tdescr->type = CPT_TDESCR;
	tdescr->name = name;
	tdescr->dbody = parse_dbody();
	
	return (union mp_tree *) tdescr;
}

union mp_tree *
parse_dbody()
{
	struct dbody* dbody;
	union mp_tree* dcase;
	
	dcase = parse_dcase();
	
	if (!scan_token(TOK_DIV, NULL))
		return dcase;
	
	dbody = mem_alloc(sizeof(*dbody));
	dbody->type = CPT_DBODY;
	dbody->dcase = dcase;
	dbody->next = parse_dbody();
	
	return (union mp_tree *) dbody;
}

union mp_tree *
parse_dcase()
{
	struct dcase* dcase;
	union mp_tree* symb;
	union mp_tree* next;
	
	if (! (symb = parse_term()))
		if (! (symb = parse_nonterm()))
			return NULL;
	next = parse_dcase();
	//cpl_printf("here dcase %s = %d\n", ((struct nonterm *) symb)->name, ((struct nonterm *) symb)->type);
	if (! next)
		return symb;
	
	dcase = mem_alloc(sizeof(*dcase));
	dcase->type = CPT_DCASE;
	dcase->symbol = symb;
	dcase->next = next;
	
	return (union mp_tree *) dcase;
}

union mp_tree *
parse_term()
{
	struct term* term;
	int l;
	
	if (!scan_token(TOK_STRING, NULL))
		return NULL;
	
	term = mem_alloc(sizeof(*term));
	term->type = CPT_TERM;
	term->name = mem_alloc((l = strlen(tok_name)) - 1);
	strncpy(term->name, tok_name + 1, l - 1);
	
	return (union mp_tree *) term;
}

union mp_tree *
parse_nonterm()
{
	struct nonterm* nonterm;
	
	if (!scan_token(TOK_NAME, NULL))
		return NULL;
	
	nonterm = mem_alloc(sizeof(*nonterm));
	nonterm->type = CPT_NONTERM;
	nonterm->name = mem_strdup(tok_name);
	//cpl_printf("here nonterm\n");
	
	return (union mp_tree *) nonterm;
}

union cpt_result
gen_grammar(struct grammar* grammar)
{
	if (! grammar)
		return VOID;
	call_tree((union tree *) grammar->tdescr, grammar_cpt_descr);
	if (grammar->next)
		return call_tree((union tree *) grammar->next, grammar_cpt_descr);
	return VOID;
}

union cpt_result
gen_tdescr(struct tdescr* tree)
{
	if (! tree)
		return VOID;
	cpl_printf("parse_%s() {\n", tree->name);
	
	call_tree((union tree *) tree->dbody, grammar_cpt_descr);
	
	cpl_printf("}\n");
	return VOID;
}

union cpt_result
gen_dbody(struct dbody* tree)
{
	if (! tree)
		return VOID;
	cpl_printf("{ ");
	call_tree((union tree *) tree->dcase, grammar_cpt_descr);
	cpl_printf(" }\n");
	return call_tree((union tree *) tree->next, grammar_cpt_descr);
}

union cpt_result
gen_dcase(struct dcase* tree)
{
	union cpt_result symres;
	if (! tree)
		return VOID;
	symres = call_tree((union tree *) tree->symbol, grammar_cpt_descr);
	cpl_printf(" ");
	call_tree((union tree *) tree->next, grammar_cpt_descr);
	return VOID;
}

union cpt_result
gen_term(struct term* tree)
{
	cpl_printf("\"%s\"", tree->name);
	return VOID;
}

union cpt_result
gen_nonterm(struct nonterm* tree)
{
	cpl_printf("%s", tree->name);
	return VOID;
}
