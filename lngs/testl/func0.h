#ifndef FUNC0_H
#define FUNC0_H

typedef double (*func_one_t)(double v);

void * cb_func_one(struct tree_descr* descr, struct tree_simple_linear* tree);
void testl_register_func(struct parser_context* ctx, struct tree_descr* ex);

#endif
