#include "stdo.h"
#include "parser.h"
#include "ptypes.h"
#include "utils.h"
#include "iochannel.h"



struct stdp_op_left_cpt {
	struct gen_cpt_info gen_info;
	
	char* subname;
	char* opclass;
};

struct stdp_op_right_cpt {
	struct gen_cpt_info gen_info;
	
	char* subname;
	char* opclass;
};

struct stdp_left_mod_cpt {
	struct gen_cpt_info gen_info;
	
	
};


struct tree_op_left_cpt*
parse_left_simple_op(struct stdp_op_left_cpt* cpt)
{
	struct gen_tree* result1,* result2,* result3;
	struct tree_op_left_cpt* tail;
	int optype;
	struct parser_context* parser = cpt->gen_info.ctx;
	
	result1 = parser_parse(parser, cpt->subname);
	if (parser_check_result(parser, result1, ERROR) || parser_check_result(parser, result1, RECOVERABLE_ERROR))
		return (struct tree_op_left_cpt*) result1;
	
	do {
		struct transact_list* rec_point = parser->iochannel->begin(parser->iochannel);
		result2 = parser_parse(parser, cpt->opclass);
		if (parser_check_result(parser, result2, ERROR)) {
			return (struct tree_op_left_cpt*) ERROR;
		}
		if  (parser_check_result(parser, result2, RECOVERABLE_ERROR)) {
			parser_recover(parser, result2);
			parser_unref(parser, result2);
			parser->iochannel->recover(parser->iochannel);
			return (struct tree_op_left_cpt*) result1;
		}
		optype = result2->type;
		
		result3 = parser_parse(parser, cpt->subname);
		if (parser_check_result(parser, result3, ERROR)) {
			return (struct tree_op_left_cpt*) ERROR;
		}
		if (parser_check_result(parser, result3, RECOVERABLE_ERROR)) {
			parser_recover(parser, result3);
			parser_unref(parser, result3);
			parser_recover(parser, result2);
			parser_unref(parser, result2);
			parser->iochannel->recover(parser->iochannel);
			return (struct tree_op_left_cpt*) result1;
		}
	
		tail = (struct tree_op_left_cpt*) parser_alloc(parser, (struct gen_cpt_info *) cpt);
		tail->optype = optype;
		tail->left = result1;
		tail->right = result3;
		tail->rec_point = rec_point;
		result1 = (struct gen_tree *) tail;
		
		parser_unref(parser, result2);
	} while (1);
	
	return (struct tree_op_left_cpt*) tail;
}

struct tree_op_right_cpt*
parse_right_simple_op(struct stdp_op_right_cpt* cpt)
{
	struct gen_tree* result1,* result2,* result3;
	struct tree_op_right_cpt* result;
	int optype;
	struct parser_context* parser = cpt->gen_info.ctx;
	
	result1 = parser_parse(parser, cpt->subname);
	if (parser_check_result(parser, result1, ERROR) || parser_check_result(parser, result1, RECOVERABLE_ERROR))
		return (struct tree_op_right_cpt *) result1;
	
	struct transact_list* rec_point = parser->iochannel->begin(parser->iochannel);
	result2 = parser_parse(parser, cpt->opclass);
	if (parser_check_result(parser, result2, ERROR)) {
		parser_recover(parser, result1);
		parser_unref(parser, result1);
		return (struct tree_op_right_cpt *) result2;
	}
	if  (parser_check_result(parser, result2, RECOVERABLE_ERROR)) {
		parser_recover(parser, result2);
		parser_unref(parser, result2);
		parser->iochannel->recover(parser->iochannel);
		return (struct tree_op_right_cpt *) result1;
	}
	optype = result2->type;
	
	result3 = (struct gen_tree *) parse_right_simple_op(cpt);
	if (parser_check_result(parser, result3, ERROR) || parser_check_result(parser, result3, RECOVERABLE_ERROR)) {
		parser_recover(parser, result2);
		parser_unref(parser, result2);
		parser->iochannel->recover(parser->iochannel);
		parser_recover(parser, result1);
		parser_unref(parser, result1);
		return (struct tree_op_right_cpt *) result3;
	}
	
	result = (struct tree_op_right_cpt *) parser_alloc(parser, (struct gen_cpt_info *) cpt);
	result->optype = optype;
	result->left = result1;
	result->right = result3;
	result->rec_point = rec_point;
	
	parser_unref(parser, result2);
	return result;
}

static
void
recover_simple_op_cpt(struct stdp_op_left_cpt* cpt,
		struct tree_op_left_cpt* tree)
{
	struct parser_context* parser = cpt->gen_info.ctx;
	parser_recover(parser, tree->right);
	parser->iochannel->recover_to(parser->iochannel, tree->rec_point);
	parser_recover(parser, tree->left);
}

static
void
unref_simple_op_cpt(struct stdp_op_left_cpt* cpt,
		struct tree_op_left_cpt* tree)
{
	struct parser_context* parser = cpt->gen_info.ctx;
	parser_unref(parser, tree->right);
	parser_unref(parser, tree->left);
	
	mem_free(tree);
}

struct stdp_op_left_cpt *
stdp_op_left_cpt(struct parser_context* ctx, char* name, char* sub, char* ops)
{
	struct stdp_op_left_cpt* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_op_left_cpt);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_simple_op_cpt;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_simple_op_cpt;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_left_simple_op;
	cpt->subname = mem_strdup(sub);
	cpt->opclass = mem_strdup(ops);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

struct stdp_op_right_cpt *
stdp_op_right_cpt(struct parser_context* ctx, char* name, char* sub, char* ops)
{
	struct stdp_op_right_cpt* cpt;
	
	cpt = mem_alloc(sizeof(*cpt));
	cpt->gen_info.ctx = ctx;
	cpt->gen_info.cptname = mem_strdup(name);
	cpt->gen_info.cpt_len = sizeof(struct tree_op_right_cpt);
	cpt->gen_info.alloc = parser_default_alloc;
	cpt->gen_info.recover = (gen_cpt_restore_t) recover_simple_op_cpt;
	cpt->gen_info.unref = (gen_cpt_unref_t) unref_simple_op_cpt;
	
	cpt->gen_info.parse = (gen_cpt_parser_t) parse_right_simple_op;
	cpt->subname = mem_strdup(sub);
	cpt->opclass = mem_strdup(ops);
	
	register_cpt(ctx, (struct gen_cpt_info *) cpt);
	
	return cpt;
}

