#ifndef UTILS_H
#define UTILS_H

#ifndef NULL
#define NULL 0
#endif

struct list;

struct list_el {
	struct list_el* next;
	struct list_el* prev;
	void* element;
	struct list* real_list;
};

struct list {
	struct list_el* first;
	struct list_el* last;
	int size;
};

struct list_iter {
	struct list* rlist;
	struct list_el* el;
};

struct list* list_create();
void  ADD_EL(struct list* list, void* element);
struct list_iter list_iter(struct list* list);
int COUNT_ELS(struct list* list);
void* GET_EL(struct list_iter* list);
void  NEXT_EL(struct list_iter* list);
void  RM_EL(struct list_iter* list);
void  INS_EL(struct list_iter* list, void* element);
void  PRE_EL(struct list_iter* list, void* element);
void  SET_EL(struct list_iter* list, void* element);
int   CONT(struct list_iter* list);


void* mem_alloc(int size);
void mem_free(void *);
void mem_copy(void* dst, void* src, int size);

char* mem_strdup(char*);

int convert_str_to_number(char*);

int cpl_printf(const char* fromat, ...);
int cpl_debug(const char* fromat, ...);

#endif
