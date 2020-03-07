#ifndef REGISTER_H
#define REGISTER_H

struct parser_context;
struct gen_cpt_info;

typedef struct gen_tree* (*gen_cpt_parser_t)(struct gen_cpt_info* cpt);
typedef void (*gen_cpt_restore_t)(struct gen_cpt_info* cpt, struct gen_tree* tree);
typedef struct gen_tree* (*gen_cpt_alloc_t)(struct gen_cpt_info* cpt);
typedef void (*gen_cpt_unref_t)(struct gen_cpt_info* cpt, struct gen_tree* tree);

struct gen_cpt_info {
	struct parser_context* ctx;
	char* cptname;
	int cpt_len;
	int type;
	gen_cpt_parser_t parse;
	gen_cpt_restore_t recover;
	gen_cpt_alloc_t alloc;
	gen_cpt_unref_t unref;
};

struct gen_cpt_info_const {
	struct parser_context* ctx;
	char* cptname;
	int cpt_len;
	gen_cpt_parser_t parse;
	gen_cpt_restore_t recover;
	gen_cpt_alloc_t alloc;
	gen_cpt_unref_t unref;
	
	
	struct gen_tree* (*const_parse)();
};

struct cpt_info_list {
	struct gen_cpt_info* info;
	struct cpt_info_list* next;
};

struct parser_context {
	struct cpt_info_list* cpts;
	struct iochannel* iochannel;
	int types;
	char* spaces;
};

int parser_get_type(struct parser_context* ctx, struct gen_cpt_info* cpt);
void register_cpt(struct parser_context* ctx, struct gen_cpt_info* cpt);
struct gen_tree* parse_general(struct parser_context* ctx, char* cptname);
struct gen_cpt_info* parser_get_cpt_type(struct parser_context* ctx, int type);
struct gen_cpt_info* parser_get_cpt(struct parser_context* ctx, char* cptname);
struct parser_context* new_parser(int fd);
struct parser_context* new_parser_with_io(struct iochannel* io);

struct gen_cpt_info* cptreg_alloc_default(struct parser_context* ctx, char* name, unsigned int cptsize, unsigned int treesize);

#define CPT_ALLOC(ctx, name, CPT_POINTER, TREE_STRUCT) ( cptreg_alloc_default(ctx, name, sizeof(*CPT_POINTER), sizeof(TREE_STRUCT)) )

#endif
