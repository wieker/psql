#ifndef TREE_H
#define TREE_H

union tree;

struct expression {
	int type;
	union tree* assign;
	union tree* ostexpr;
};

struct andor {
	int type;
	int op_type;
	union tree* first_andor;
	union tree* not;
};

struct assign {
	int type;
	union tree* var;
	union tree* next;
};

struct not {
	int type;
	int not;
	union tree* compare;
};

struct compare {
	int type;
	int op_type;
	union tree* left;
	union tree* right;
};

struct sum {
	int type;
	int op_type;
	union tree* left;
	union tree* right;
};

struct prod {
	int type;
	int op_type;
	union tree* left;
	union tree* right;
};

struct pow {
	int type;
	union tree* left;
	union tree* right;
};

struct function {
	int type;
	char* name;
	union tree* params;
};

struct variable {
	int type;
	char* name;
};

struct column {
	int type;
	char* name;
};

struct string {
	int type;
	char* string;
};

struct number {
	int type;
	double number;
};

struct params {
	int type;
	struct expression* expression;
};

union tree {
	int type;
	struct expression expression;
	struct assign assign;
	struct andor andor;
	struct not not;
	struct compare compare;
	struct sum sum;
	struct prod prod;
	struct pow pow;
	struct function function;
	struct variable variable;
	struct column column;
	struct string string;
	struct number number;
	struct params params;
};

union tree* parse_expression();

union tree* parse_assign();

union tree* parse_andor();

union tree* parse_not();

union tree* parse_compare();

union tree* parse_sum();

union tree* parse_prod();

union tree* parse_pow();

union tree* parse_value();

union tree* parse_params();




#define CPT_EXPRESSION 1
#define CPT_COMMA 2
#define CPT_ANDOR 3
#define CPT_AND 4
#define CPT_OR 5
#define CPT_NOT 6
#define CPT_COMPARE 7
#define CPT_LESS 8
#define CPT_MORE 9
#define CPT_EQUAL 10
#define CPT_SUM 11
#define CPT_PLUS 12
#define CPT_MINUS 13
#define CPT_PROD 14
#define CPT_MUL 15
#define CPT_DIV 16
#define CPT_POW 17
#define CPT_LB 18
#define CPT_RB 19
#define CPT_NUMBER 20
#define CPT_STRING 21
#define CPT_NAME 22
#define CPT_COLUMN 23
#define CPT_FUNCTION 24

#define CPT_ASSIGN 25
#define CPT_VAR 26
#define CPT_FUNC_PARAMS 27


#endif
