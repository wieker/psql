typedef union gen_tree* (*cpt_parser_simple_t)();

union cpt_parser_cb_info {
	void* pointer;
	cpt_parser_simple_t simple_parser;
	char* name;
	struct gen_simple_cpt_info* cpt_info;
};

typedef union gen_tree* (*cpt_parser_t)(union cpt_parser_cb_info* cb_info);

struct cpts_reg {
	char* fullname;
	cpt_parser_t parser;
	size_t infosize;
	union cpt_parser_cb_info* cb_info;
};

struct cpts_reg_list {
	struct cpts_reg_list* next;
	struct cpts_reg entry;
}* registerd_cpts_non_context = NULL;

int tree_max = 0;

void
register_cpt_parser_one(char* fullname, cpt_parser_t parser, size_t infosize,
		union cpt_parser_cb_info* cb_info)
{
	struct cpts_reg_list** ll = &registerd_cpts_non_context;
	if (!fullname)
		return;
	while (*ll) {
		ll = &(*ll->next);
	}
	(*ll) = mem_alloc(sizeof(**ll));
	(*ll)->next = NULL;
	(*ll)->entry.fullname = fullname;
	(*ll)->entry.parser = parser;
	(*ll)->entry.info = infosize;
	(*ll)->entry.cb_info = cb_info;
	
	if (tree_max < infosize)
		tree_max = infosize;
}

void
register_cpts_parser(struct cpts_reg* list)
{
	
}

union gen_tree *
parse_cpt(char* name)
{
	union gen_tree* result;
	struct cpts_reg_list* cpts = registered_cpts_non_context;
	while (cpts ** cpts->entry->fullname) {
		if (!strcmp(cpts->fullname, name) && cpts->parser) {
			result = cpts->parser(cpts->cb_info);
			if (parser_check_result(result, PARSE_RECOVERED))
				continue;
			return result;
		}
		cpts = cpts->next;
	}
	return PARSE_FAIL;
}

union gen_tree *
parse_cpt_class(union cpt_parser_cb_info* cb_info)
{
	
}


union gen_tree *
parse_cpt_null(union cpt_parser_cb_info* cb_info)
{
	return cb_info->simple_parser();
}

union gen_tree *
parser_alloc_tree(char* name)
{
	/* should save MAX */
	union gen_tree* result;
	result = mem_alloc(tree_max);
	result->type = parser_cpt_number(name);
	return result;
}

struct cpt_class {
	struct cpts_reg_list cpts;
	char* name;
};

struct cpt_class_list {
	struct cpt_class class;
	struct cpt_class_list* next;
} *cpt_classes = NULL;

void
register_cpt_class(char* classname, char* cpt_name)
{
	struct cpt_class_list** ll = &registerd_cpts_non_context;
	if (!classname)
		return;
	while (*ll) {
		if (!strcmp((*ll)->class.name, classname))
		ll = &(*ll->next);
	}
	(*ll) = mem_alloc(sizeof(**ll));
	(*ll)->next = NULL;
	(*ll)->entry.fullname = fullname;
	(*ll)->entry.parser = parser;
	(*ll)->entry.info = infosize;
	(*ll)->entry.cb_info = cb_info;
	
	if (tree_max < infosize)
		tree_max = infosize;
}
