#ifndef TABLE_H
#define TABLE_H

typedef void* table;

table table_create();

void table_set_mark(table tbl, char* mark);
void table_set_mark_strdup(table tbl, char* mark, char* value);
void table_set_mark_int(table tbl, char* mark, int value);
void table_set_mark_ptr(table tbl, char* mark, void* value);
void table_set_mark_tptr(table tbl, char* mark, char* type, void* value);

#endif
