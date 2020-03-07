#ifndef RECORE_H
#define RECORE_H

struct recore_pointer {
	void *p;
};

extern void *recore_list;

int recore(struct exec_descr* ctx, struct attnode* node, void* input, void* closure);
int recore_top(struct exec_descr* ctx, struct attnode* node, void* input, void* closure);

#endif
