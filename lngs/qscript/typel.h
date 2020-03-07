#ifndef TYPEL_H
#define TYPEL_H


#include "gen_attr.h"
#include "atrex.h"

typedef int (*caster_t)(struct attnode* node, char* typename);
typedef int (*recognitor_t)(struct attnode* node, char* typename);

int rework_types(struct exec_descr* wtt, struct exec_descr* wet, char* file);

#endif
