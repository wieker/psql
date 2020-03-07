#ifndef TCLTREE_H
#define TCLTREE_H

#include "tree.h"

struct arg_list {
	struct arg_list* next;
	union {
		union tcl_tree* arg;
		struct {
			int type;
			char* string;
		};
	};
};

struct tcl_name {
	int type;
	char* name;
};

struct tcl_var {
	int type;
	struct variable* ref;
};

struct tcl_string {
	int type;
	struct arg_list* list;
};

struct tcl_operator {
	int type;
	char* name;
	struct arg_list* args;
};

struct tcl_ifop {
	int type;
	union tree* condition;
	union tcl_tree* body;
};

struct program {
	int type;
	union tcl_tree* ostprog;
	union tcl_tree* operator;
};

union tcl_tree {
	int type;
	union tree tree;
	struct tcl_name tcl_name;
	struct tcl_var tcl_var;
	struct tcl_string tcl_string;
	struct tcl_operator tcl_operator;
	struct tcl_ifop tcl_ifop;
	struct program tcl_program;
};


union tcl_tree* parse_program();
union tcl_tree* parse_operator();
union tcl_tree* parse_name();
union tcl_tree* parse_var();
union tcl_tree* parse_tcl_string();

#define CPT_TCL_OPERATOR_LIST (25 << 2)
#define CPT_TCL_WHILE (26 << 2)
#define CPT_OPERATOR (27 << 2)
#define CPT_TCL_VAR (28 << 2)
#define CPT_TCL_STRING (29 << 2)

#endif
