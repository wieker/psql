#include "table.h"
#include "utils.h"
#include <string.h>

struct table* variables = NULL;
struct table* functions = NULL;

int
table_ins_element(struct table* table, char* key, void* element)
{

	return 1;
}

int
table_del_element(struct table* table, char* key)
{
	
	return 1;
}

int
table_set_element(struct table** table, char* key, void* element)
{
	struct table** vars = table;
	//cpl_printf("set: %s\n", key);
	while (*vars) {
		if (!strcmp((*vars)->key, key)) {
			(*vars)->element = element;
			return 1;
		}
		vars = &((*vars)->next);
	}
	*vars = mem_alloc(sizeof(**vars));
	(*vars)->next = NULL;
	(*vars)->key = mem_strdup(key);
	(*vars)->element = element;
	return 1;
}

int
table_get_element(struct table* table, char* key, void** element)
{
	struct table* vars = table;
	//cpl_printf("get: %s\n", key);
	while (vars) {
		//cpl_printf("get: '%s' == '%s'\n", key, vars->key);
		if (!strcmp(vars->key, key)) {
			(*element) = vars->element;
			return 1;
		}
		vars = vars->next;
	}
	*element = NULL;
	return 0;
}

int
table_get_or_mark_element(struct table* table, char* key, void** element)
{
	return 1;
}


