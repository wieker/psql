#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

int config_debug = 0;

#include "table.h"
struct table* rules = NULL;
char cmdname[256];

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define SKIPPING \
	if (*(int *) input) { \
		cpl_printf("Skipping\n"); \
		return 1; \
	}

#include "expcore.h"

int channel;
char* host;
char* pass;

char* rhost;
char* rpass;

extern int retyies;
int walk();

static
int
go_deep(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct list_iter list = list_iter(node->subtrees);
	
	cpl_debug("default for %s\n", atree_get_attr(node, "name"));
	while (CONT(&list)) {
		call_atree(ctx, (struct attnode*) GET_EL(&list), input);
		NEXT_EL(&list);
	}
	return 1;
}

static
int
reg_rule(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(atree_subtree(atree_subtree(node, 1), 1), 2), "value");
	table_set_element(& rules, name, (void *) atree_subtree(node, 1));
	return 1;
}

static
int
ex_rule(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	//char* name = atree_get_attr(atree_subtree(atree_subtree(node, 1), 2), "value");
	struct attnode* precond = atree_subtree(node, 2);
	struct attnode* postcond = atree_subtree(node, 3);
	struct attnode* actions = atree_subtree(node, 4);
	int rtval = 0;
	SKIPPING
	call_atree(ctx, postcond, (void *) & rtval);
	if (! rtval)
		return 1;
	rtval = 0;
	call_atree(ctx, precond, & rtval);
	call_atree(ctx, actions, & rtval);
	(*(int *) input) |= rtval;
	return 1;
}

static
int
call_rule(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(node, 2), "value");
	struct attnode* rule;
	table_get_element(rules, name, (void *) & rule);
	SKIPPING
	if (! rule) {
		(*(int *) input) = 1;
		fprintf(stderr, "FATAL: rule %s not found\n", name);
		return 1;
	}
	int rtval = 0;
	call_atree(ctx, rule, & rtval);
	(*(int *) input) |= rtval;
	return 1;
}


/*
 * исполняет action
 * */
static
int
ex_atom(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	SKIPPING
	/*
	 * 
	 * */
	 char* name;
	struct attnode* base_action = atree_subtree(node, 1);
	char* action_name = atree_get_attr(base_action, "name");
	int local = 0;
	if (! strcmp(action_name, "uploadrec")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Uploading: %s\n", nname);
		(*(int *) input) |= upload_file(host, pass, nname);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "rspawn")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Remote spawn: %s\n", nname);
		(*(int *) input) |= spawn_helper(channel, nname, host, pass);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "rsend")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Remote send: %s\n", nname);
		(*(int *) input) |= send_helper(channel, nname, host, pass);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "rwait")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Remote wait: %s\n", nname);
		(*(int *) input) |= wait_helper(channel, nname, host, pass);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "dnld")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Remote wait: %s\n", nname);
		(*(int *) input) |= dnld_helper(channel, nname, host, pass);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "regexp")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		name = atree_get_attr(param_node, "value");
		char* nname = strdup(name + 1);
		nname[strlen(nname) - 1] = 0;
		cpl_debug("Remote wait regexp: %s\n", nname);
		(*(int *) input) |= regexp_helper(channel, nname, NULL);
		free(nname);
		return 1;
	}
	if (! strcmp(action_name, "rcall")) {
		struct attnode* param_node = atree_subtree_name(base_action, "quoted", 1);
		char* thost = atree_get_attr(param_node, "value");
		param_node = atree_subtree_name(base_action, "quoted", 2);
		char* tpass = atree_get_attr(param_node, "value");
		
		host = strdup(thost + 1);
		host[strlen(host) - 1] = 0;
		pass = strdup(tpass + 1);
		pass[strlen(pass) - 1] = 0;
		
		upload_file(host, pass, "rhelper/rhelper");
		channel = ssh_channel(host, pass);
		return 1;
	}
	if (! strcmp(action_name, "restore")) {
		host = rhost;
		pass = rpass;
		channel = ssh_channel(host, pass);
		return 1;
	}
	if (! strcmp(action_name, "walk")) {
		walk(host, pass);
		return 1;
	}
	if (! strcmp(action_name, "sendeot")) {
		char eot = 4;
		write(channel, & eot, 1);
		return 1;
	}
	if (! strcmp(action_name, "ctrlc")) {
		char eot = 3;
		write(channel, & eot, 1);
		return 1;
	}
	
	struct attnode* pfx;
	if ((pfx = atree_subtree_name(base_action, "local_pfx", 1))) {
		local = 1;
	}
	
	struct attnode* scr;
	if ((scr = atree_subtree_name(base_action, "script_name", 1)))
		name = atree_get_attr(scr, "value");
	if ((scr = atree_subtree_name(base_action, "raw", 1))) {
		name = "__raw";
		FILE* fl = fopen("scripts/__raw.sh", "w");
		fprintf(fl, "%s", atree_get_attr(scr, "value"));
		fclose(fl);
	}
	
	int rtval;
	if (local) {
		snprintf(cmdname, sizeof(cmdname), "sh scripts/%s.sh", name);
		rtval = system(cmdname);
	} else {
		cpl_debug("Starting remote\n");
		rtval = call_rhelper(channel, name, host, pass);
	}
	(*(int *) input) |= rtval;
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "defrule", (run_atom_cb_t) reg_rule, NULL);
	atree_caller_reg(ctx, "rule", (run_atom_cb_t) ex_rule, NULL);
	atree_caller_reg(ctx, "rule_name", (run_atom_cb_t) call_rule, NULL);
	atree_caller_reg(ctx, "action_tpl", (run_atom_cb_t) ex_atom, NULL);
	return ctx;
}

int
main(int argc, char** argv)
{
	rhost = host = argv[1];
	rpass = pass = argv[2];
	char* rname = argv[3];
	upload_file(host, pass, "rhelper/rhelper");
	channel = ssh_channel(host, pass);
	
	struct attnode* _desc = generate_att_tree("expin.gr", "actfile.in", 0);
	
	struct exec_descr* ctx = reg_cpts();
	
	struct attnode* rule = NULL;
	call_atree(ctx, _desc, NULL);
	table_get_element(rules, rname, (void *) & rule);
	
	int rtval = 0;
	if (rule)
		call_atree(ctx, rule, & rtval);
	else
		cpl_printf("Fatal: rule %s not found\n", rname);
	
	return 0;
}

