#ifndef PATHDECL_H
#define PATHDECL_H

#define PATH_TAG 1
#define PATH_PARAM 2
#define PATH_ALL 3
#define PATH_ACTION 4

struct path;

typedef int (* path_cb)(struct path* path, struct attnode* node, void* data);

struct path_step_tag {
	int type;
	char* tagname;
};

struct path_step_param {
	int type;
	char* pname;
};

struct path {
	struct path* next;
	path_cb cb;
union {
	int type;
	struct path_step_tag path_step_tag;
	struct path_step_param path_step_param;
};
	void* data;
};

struct exec_descr* reg_cpts_xml();

#endif
