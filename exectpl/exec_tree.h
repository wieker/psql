#ifndef EXEC_TREE_H
#define EXEC_TREE_H

#include "parser.h"
#include "register.h"

struct tree_descr;

typedef struct cpt_result* (*exec_cb)(struct tree_descr* descr, struct gen_tree* tree);

struct cpt_descr {
	int cpt;
	exec_cb cb;
	struct cpt_descr* next;
};

struct tree_descr {
	struct cpt_descr* cpts;
	struct parser_context* pctx;
	void* curr_data;
};

struct cpt_result {
	int type;
	int size;
	int alloced;
};

extern const struct cpt_result VOID;
extern const struct cpt_result EXEC_ERROR;
extern const struct cpt_result SUCC;

const struct cpt_result* call_tree(struct gen_tree* tree, struct tree_descr* tree_descr);
struct tree_descr* tree_descr(struct parser_context* pctx);
void executor_reg(struct tree_descr* descr, char* name, exec_cb cb);

int result_wait(char* name, struct gen_tree* tree, struct tree_descr* tree_descr,
			struct cpt_result** rt);
void* result_alloc_0(struct tree_descr* descr, char* name, int size);

#endif
