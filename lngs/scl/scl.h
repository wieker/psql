#ifndef SCL_H
#define SCL_H

struct scl_callgate_context {
	int L;
	void* handles[100];
};

struct scl_closure {
	struct attnode* node_to_exec;
	struct exec_descr* scl_executor;
	struct scl_callgate_context* callgate_ctx;
};

struct scl_external_ctx {
	void*  vartable;
	void*  typetable;
};

struct scl_input {
	struct attnode* node_to_process;
	struct exec_descr* data_tree_ctx;
	struct scl_external_ctx* external_ctx;
	struct scl_callgate_context* callgate_ctx;
};

struct scl_reg {
	struct exec_descr* data_tree_ctx;
	struct scl_callgate_context* callgate_ctx;
};

#endif
