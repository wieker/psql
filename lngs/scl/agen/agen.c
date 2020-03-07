#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "scl.h"
#include "scalc.h"

int ip = 0;
int code[10000];

int nlocal = 0;

struct mark {
	struct mark* stack;
	int localmark;
	int value;
	char* name;
}* marks = NULL;

struct setmark {
	struct setmark* stack;
	char* name;
	int coord;
	int localmark;
}* setmarks = NULL;

struct op {
	char* name;
	int opcode;
} ops[] = {
	{"\"jmp\"", 1},
	{"\"jnz\"", 2},
	{"\"lt\"", 3},
	{"\"mul\"", 4},
	{"\"lea\"", 5},
	{"\"load\"", 6},
	{"\"store\"", 7},
	{"\"leave\"", 8},
	{"\"enter\"", 9},
	{"\"ret\"", 10},
	{"\"li\"", 11},
	{"\"add\"", 12},
	{"\"sub\"", 13},
	{"\"div\"", 14},
	{"\"and\"", 15},
	{"\"or\"", 16},
	{"\"not\"", 17},
	{"\"eq\"", 18},
	{"\"mt\"", 19},
	{"\"dup\"", 20},
	{"\"down\"", 21},
	{"\"neg\"", 22},
	{"\"stop\"", 23},
	{"\"lz\"", 24},
	{"\"ne\"", 25},
	{NULL, 0},	
};

struct localvar {
	struct localvar* next;
	char* name;
	int offset;
}* vars = NULL;


int
enter_block()
{
	struct mark* m = marks;
	struct mark* n = mem_alloc(sizeof(*n));
	
	n->stack = m;
	n->localmark = 1;
	
	marks = n;
	
	struct setmark* m1 = setmarks;
	struct setmark* n1 = mem_alloc(sizeof(*n1));
	
	n1->stack = m1;
	n1->localmark = 1;
	
	
	setmarks = n1;
	return 1;
}

int
load_mark(char* param)
{
	char* name = mem_strdup(param);
	struct setmark* m = setmarks;
	struct setmark* n = mem_alloc(sizeof(*n));
	
	n->stack = m;
	n->name = name;
	n->coord = ip;
	n->localmark = 0;
	ip ++;
	
	
	setmarks = n;
	return 1;
}

int
write_st(char* param)
{
	struct op* op = ops;
	while (op->name) {
		if (! strcmp(op->name, param)) {
			code[ip] = op->opcode;
			ip ++;
		}
		op ++;
	}
	return 1;
}

int
set_mark(char* param)
{
	char* name = mem_strdup(param);
	struct mark* m = marks;
	struct mark* n = mem_alloc(sizeof(*n));
	
	n->stack = m;
	n->localmark = 0;
	
	n->name = name;
	n->value = ip;
	
	marks = n;
	return 1;
}

int
leave_block()
{
	/*
	 * Befor leaving block there are:
	 * - all marks are set
	 * - we can resolve all marks names and write it to code
	 * */
	/*
	 * ищем все отметки в списке и записываем значения
	 * затем освобождаем из стека отметки текущего блока
	 * */
	
	struct mark* m = marks;
	struct setmark* s;
	
	do {
		m = marks;
		while (setmarks && ! setmarks->localmark && strcmp(setmarks->name, m->name))
			m = m->stack;
		if (!setmarks ||
						setmarks->localmark) //если нашли отметку поставленную не в текущем блоке - значит
				//обработка блока завершена
			break;
		
		code[setmarks->coord] = m->value;
		s = setmarks;
		setmarks = setmarks->stack;
		
		mem_free(s->name);
		mem_free(s);
	} while (1);
	
	m = marks;
	while (m && ! m->localmark) {
		m = marks->stack;
		free(marks->name);
		free(marks);
		marks = m;
	}
	if (m && m->localmark) {
		m = marks->stack;
		//free(marks->name);
		free(marks);
		marks = m;
	}
	
	return 1;
}

int
load_const(char* param)
{
	int p = atoi(param);
	code[ip] = p;
	ip ++;
	return 1;
}

int
load_local(char* param)
{
	struct localvar* v = vars;
	while (v) {
		if (! strcmp(param, v->name))
			break;
		v = v->next;
	}
	
	if (v) {
		code[ip] = v->offset;
		ip ++;
	} else {
		v = mem_alloc(sizeof(*v));
		v->offset = nlocal;
		nlocal ++;
		v->name = mem_strdup(param);
		
		v->next = vars;
		vars = v;
		
		code[ip] = v->offset;
		ip ++;
	}
	
	return 1;
}

int
set_global_mark(char* param)
{
	
	return 1;
}

int
load_frame_size(char* param)
{
	
	return 1;
}

int
set_const_char(char* param)
{
	code[ip] = *param;
	ip ++;
	return 1;
}
