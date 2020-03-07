#ifndef TABLE_H
#define TABLE_H

struct table {
	struct table* next;
	char* key;
	void* element;
};

extern struct table* variables;
extern struct table* functions;

int table_set_element(struct table **, char* key, void* element);

int table_get_element(struct table *, char* key, void** element);


//UNDEFINED

int table_ins_element(struct table *, char* key, void* element);

int table_del_element(struct table *, char* key);

int table_exists(struct table *, char* key);

int table_get_or_mark_element(struct table* table, char* key, void** element);


#endif

