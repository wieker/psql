#ifndef CPTS_H
#define CPTS_H

#include "tcltree.h"
#include "utils.h"

#define CPT_RES_VOID NULL
#define CPT_RES_ERROR 65530
#define CPT_RES_SUCC 65531
#define CPT_RES_NUMBER 1
struct cpt_result_number {
	int type;
	double number;
};

#define CPT_RES_NAME 2
struct cpt_result_name {
	int type;
	char* name;
};

union cpt_result {
	int type;
	struct cpt_result_number number;
	struct cpt_result_name name;
};

extern const union cpt_result VOID;
extern const union cpt_result ERROR;
extern const union cpt_result SUCC;

struct cpt_descr {
	int cpt;
	char* description;
	union cpt_result (*cb)(union tree* tree);
};

union cpt_result call_tree(union tree* tree, struct cpt_descr*);

#endif
