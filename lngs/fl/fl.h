#ifndef FL_H
#define FL_H

struct fl_function;

union fl_result {
	enum fl_type { FL_INTEGER, FL_FLOAT, FL_BOOL, FL_STRING, FL_CHAR, FL_TUPLE, FL_FUNCTION, FL_VOID } fl_type;
	struct {
		enum fl_type type_int;
		int integer;
	};
	struct {
		enum fl_type type_float;
		float number;
	};
	struct {
		enum fl_type type_str;
		char* str;
	};
	struct {
		enum fl_type type_char;
		char symb;
	};
	struct {
		enum fl_type type_bool;
		int bool;
	};
	struct {
		enum fl_type type_tuple;
		struct attnode* head;
		struct attnode* tail;
		struct fl_function* hc;
		struct fl_function* tc;
	};
	struct {
		enum fl_type type_function;
		struct fl_function* function;
	};
	struct {
		enum fl_type type_void;
	};
};

struct fl_context {
	struct table* preglobal;
	union fl_result result;
	struct fl_function* closure;
};

struct fl_function {
	enum { FL_TEMPLATE, FL_CLOSURE, FL_VALUE } fl_function_type;
	
	struct table* local;
	struct table* param;
	struct fl_function* closure;
	// struct table* global;
	
	struct attnode* body;
	struct attnode* expr;
	int argcount;
	
	union fl_result result;
	int closed;
};

int intr(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int statement(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int lfunction_global(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int defun_global(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);


int name(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int funcall(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int lfunction(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int defun_exec(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);

int concat(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int lstring(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int plus(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);

int tuplec(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int tupleh(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int tuplet(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);

int minus(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int less(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int more(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);
int equal(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);


int ifop(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, void* closure);

                                                                                                                                                                                
int try_cast(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, union fl_result* cast);

#endif
