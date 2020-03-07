#ifndef ATREX_H
#define ATREX_H

struct exec_descr;

typedef int (*run_atom_cb_t)(struct exec_descr* ctx, struct attnode* node, void* exec_data, void* closure);

struct run_atom {
	char* name;
	void* closure;
	struct run_atom* next;
	run_atom_cb_t cb;
};

struct exec_descr {
	struct run_atom* atoms;
	run_atom_cb_t default_cb;
	void* default_closure;
};

int call_atree(struct exec_descr* ctx, struct attnode* node, void* exec_data);
int atree_caller_set_default_cb(struct exec_descr* ctx, run_atom_cb_t cb, void* closure);
void atree_caller_reg(struct exec_descr* ctx, char* name, run_atom_cb_t cb, void* closure);
struct exec_descr* exec_descr();

#endif
