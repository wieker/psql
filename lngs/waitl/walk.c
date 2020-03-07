#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#ifdef WALK_ALONE
int config_debug = 0;
#endif

#include "table.h"

#include "expcore.h"

#include <tcl8.5/expect.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


struct table* states = NULL;
char* initname;
char* firstmatched;
#ifdef WALK_ALONE
int channel;
#else
extern int channel;
#endif
/*
 * Should be in stack...
 * */
char* lastmatched;

int wrpipe;
int depth = 1000;

pid_t cpid = 0;

extern int retryies;

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
init(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(node, 1), "value");
	initname = mem_strdup(name);
	return 1;
}

static
int
state(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* name = atree_get_attr(atree_subtree(node, 1), "value");
	table_set_element(& states, name, (void *) atree_subtree(node, 2));
	return 1;
}

static
int
send(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	struct attnode* param_node = atree_subtree(node, 1);
	char* name = atree_get_attr(param_node, "value");
	cpl_debug("Remote send: %s\n", name);
	send_helper(channel, name, NULL, NULL);
	return 1;
}

static
int
sendv(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	cpl_debug("Remote send var: %s\n", lastmatched);
	send_helper(channel, lastmatched, NULL, NULL);
	return 1;
}

static
int
sendf(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	cpl_debug("Remote send var: %s\n", firstmatched);
	printf("Remote send matched: %s\n", firstmatched);
	send_helper(channel, firstmatched, NULL, NULL);
	return 1;
}

static
int
sleepm(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	system("sleep 1");
	return 1;
}

static
int
statedescr(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	/*
	 * This part should be done on "compilation stage"
	 * This part simple repeated on each iteration and can be cached
	 * BEGIN
	 * */
	struct list_iter list = list_iter(node->subtrees);
	struct attnode* choice;
	int chc = 0;
	struct exp_case* cases;
	int result;
	int i = 0;
	//int save;
	
	while (CONT(&list)) {
		NEXT_EL(&list);
		chc += 1;
	}
	
	cases = mem_alloc((chc + 1) * sizeof *cases);
	list = list_iter(node->subtrees);
	
	while (CONT(&list)) {
		choice = (struct attnode*) GET_EL(&list);
		cases[i].type = exp_compiled;
		cases[i].value = i;
		cases[i].pattern = mem_strdup(atree_get_attr(atree_subtree(choice, 1), "value"));
		cases[i].re = TclRegComp(cases[i].pattern);
		NEXT_EL(&list);
		i ++;
	}
	
	cases[i].type = exp_end;
	
	/*
	 * END
	 * */
	
	int tm = 0;
	retry:
	result = exp_expectv(channel, cases);
	if (result == EXP_TIMEOUT) {
		tm ++;
		cpl_debug("Retrying due to timeout %d\n", result);
		if (tm < retryies)
			goto retry;
	}
	if (result == EXP_EOF)
		cpl_debug("EOF %d\n", result);
	if (result >= 0 && result < chc)
		choice = atree_subtree_name(node, "choice", result + 1);
	else {
		cpl_debug("Fatal error %d\n", result);
		mem_free(cases); /*
		Should free regexps
		* */
		return 0;
	}
	
	mem_free(initname);
	initname = mem_strdup(atree_get_attr(atree_subtree(choice, 2), "value"));
	//save = (*cases[result].re->endp[0]);
	//(*cases[result].re->endp[0]) = 0;
	lastmatched = mem_strdup(cases[result].re->startp[0]);
	//(*cases[result].re->endp[0]) = save;
	
	if (cases[result].re->startp[1]) {
		//save = (*cases[result].re->endp[1]);
		(*cases[result].re->endp[1]) = 0;
		firstmatched = mem_strdup(cases[result].re->startp[1]);
		//(*cases[result].re->endp[1]) = save;
	}
	
	cpl_debug("pointers: %p %p %p %p\n", cases[result].re->startp[0], cases[result].re->endp[0], cases[result].re->startp[1], cases[result].re->endp[1]);
	
	call_atree(ctx, choice, input);
	
	mem_free(cases); /*
	Should free regexps
	* */
	return 1;
}

static
int
receiver(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* filename = atree_get_subnode_attr(node, 1, "value");
	int pipefd[2];
	pipe(pipefd);
	if (! (cpid = fork()) ) {
		close(0);
		dup2(pipefd[0], 0);
		execl(filename, filename, NULL);
		exit(0);
	}
	wrpipe = pipefd[1];
	return 1;
}

static
int
forwardl(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	write(wrpipe, lastmatched, strlen(lastmatched));
	write(wrpipe, "\r\n", 1);
	return 1;
}

static
int
deep(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* deep_ = atree_get_subnode_attr(node, 1, "value");
	depth = atoi(deep_);
	return 1;
}

static
int
incdep(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	depth --;
	return 1;
}

static
int
times(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	char* deep_ = atree_get_subnode_attr(node, 1, "value");
	retryies = atoi(deep_);
	return 1;
}

static
struct exec_descr*
reg_cpts()
{
	struct exec_descr* ctx = exec_descr();
	atree_caller_set_default_cb(ctx, (run_atom_cb_t) go_deep, NULL);
	atree_caller_reg(ctx, "state", (run_atom_cb_t) state, NULL);
	atree_caller_reg(ctx, "statedescr", (run_atom_cb_t) statedescr, NULL);
	atree_caller_reg(ctx, "send", (run_atom_cb_t) send, NULL);
	atree_caller_reg(ctx, "sendv", (run_atom_cb_t) sendv, NULL);
	atree_caller_reg(ctx, "sendf", (run_atom_cb_t) sendf, NULL);
	atree_caller_reg(ctx, "sleep", (run_atom_cb_t) sleepm, NULL);
	atree_caller_reg(ctx, "init", (run_atom_cb_t) init, NULL);
	atree_caller_reg(ctx, "receiver", (run_atom_cb_t) receiver, NULL);
	atree_caller_reg(ctx, "forwardl", (run_atom_cb_t) forwardl, NULL);
	atree_caller_reg(ctx, "deep", (run_atom_cb_t) deep, NULL);
	atree_caller_reg(ctx, "incdep", (run_atom_cb_t) incdep, NULL);
	atree_caller_reg(ctx, "times", (run_atom_cb_t) times, NULL);
	atree_caller_reg(ctx, "cb", (run_atom_cb_t) NULL, NULL);
	return ctx;
}

#ifdef WALK_ALONE
int
main(int argc, char** argv)
{
	char* host = argv[1];
	char* pass = argv[2];
	char* name = argv[3];
	char* cmd = argv[4];
	char* connected = argv[5];
	int rtval;
	channel = exconnect(host, pass, name, cmd, connected);
	
	struct attnode* _desc = generate_att_tree("walk.gr", "walkfile.in", 0);
	
	struct exec_descr* ctx = reg_cpts();
	
	struct attnode* rule = NULL;
	call_atree(ctx, _desc, NULL);
	do {
		table_get_element(states, initname, (void *) & rule);
	
		if (rule)
			rtval = call_atree(ctx, rule, NULL);
		else
			cpl_printf("Fatal: state %s not found\n", initname);
	} while (rule && rtval);
	
	return 0;
}
#else
int
walk()
{
	int rtval;
	struct attnode* _desc = generate_att_tree("walk.gr", "walkfile.in", 0);
	
	struct exec_descr* ctx = reg_cpts();
	
	struct attnode* rule = NULL;
	call_atree(ctx, _desc, NULL);
	do {
		table_get_element(states, initname, (void *) & rule);
	
		if (rule)
			rtval = call_atree(ctx, rule, NULL);
		else
			cpl_printf("Fatal: state %s not found\n", initname);
	} while (rule && rtval && depth);
	if (cpid) {
		close(wrpipe);
		kill(cpid, SIGTERM);
		wait(& depth);
	}
	
	retryies = 1000;
	return 0;
}
#endif

