#include "stdlib.h"
#include "string.h"
#include "utils.h"
#include "stdarg.h"
#include "stdio.h"

extern int config_debug;

void*
mem_alloc(int size)
{
	return malloc(size);
}

void
mem_free(void* mem)
{
	free(mem);
}

void
mem_copy(void* dst, void* src, int size)
{
	memcpy(dst, src, size);
}

char*
mem_strdup(char* str)
{
	return strdup(str);
}

int
convert_str_to_number(char* str)
{
	return atoi(str);
}

int
cpl_printf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	return 0;
}

int
cpl_debug(const char* format, ...)
{
	if (!config_debug)
		return 0;
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	return 0;
}


struct list *
list_create()
{
	struct list* lst = mem_alloc(sizeof(*lst));
	lst->size = 0;
	lst->first = NULL;
	lst->last = NULL;
	return lst;
}

void
ADD_EL(struct list* list, void* element)
{
	if (list->last) {
		list->last->next = mem_alloc(sizeof((*(list->last->next))));
		list->last->next->next = NULL;
		list->last->next->prev = list->last;
		list->last->next->element = element;
		list->last->next->real_list = list;
		list->last = list->last->next;
	} else {
		list->last = list->first = mem_alloc(sizeof((*(list->last))));
		list->last->next = NULL;
		list->last->prev = NULL;
		list->last->element = element;
		list->last->real_list = list;
	}
	list->size ++;
}

struct list_iter
list_iter(struct list* list)
{
	struct list_iter i;
	i.rlist = list;
	i.el = list->first;
	return i;
}

void *
GET_EL(struct list_iter* list)
{
	return list->el->element;
}

void
NEXT_EL(struct list_iter* list)
{
	if (list->el)
		list->el = list->el->next;
}

int
CONT(struct list_iter* list)
{
	return (NULL != list->el);
}

void
RM_EL(struct list_iter* list)
{
	struct list_el* el = list->el;
	if (! list->el)
		return;
	if (list->rlist->first == list->el)
		list->rlist->first = list->el->next;
	if (list->rlist->last == list->el)
		list->rlist->last = list->el->prev;
	list->rlist->size --;
	if (list->el->prev)
		list->el->prev->next = list->el->next;
	if (list->el->next)
		list->el->next->prev = list->el->prev;
	list->el = list->el->next;
	mem_free(el);
}

void
INS_EL(struct list_iter* list, void* element)
{
	struct list_el* el = list->el->next;
	list->el->next = mem_alloc(sizeof((*(list->el->next))));
	list->el->next->next = el;
	list->el->next->prev = list->el;
	list->el->next->element = element;
	list->el->next->real_list = list->rlist;
	if (el)
		el->prev = list->el->next;
	else
		list->rlist->last = list->el->next;
	list->rlist->size ++;
}

void
PRE_EL(struct list_iter* list, void* element)
{
	struct list_el* el = list->el->prev;
	list->el->prev = mem_alloc(sizeof((*(list->el->next))));
	list->el->prev->next = list->el;
	list->el->prev->prev = el;
	list->el->prev->element = element;
	list->el->prev->real_list = list->rlist;
	if (el)
		el->next = list->el->prev;
	else
		list->rlist->first = list->el->prev;
	list->rlist->size ++;
}

void
SET_EL(struct list_iter* list, void* element)
{
	list->el->element = element;
}

int
COUNT_ELS(struct list* list)
{
	return list->size;
}
