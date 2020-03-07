#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include "wrxml.h"

#include "typel.h"
#include "scalc.h"

#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>

int config_debug = 0;

struct test_ctx {
	int in;
	int out;
};

int
main(int argc, char** argv)
{
	struct exec_descr* wtt = exec_descr();
	atree_caller_set_default_cb(wtt, (run_atom_cb_t) go_deep_scalc, NULL);
	struct exec_descr* wet = exec_descr();
	atree_caller_set_default_cb(wet, (run_atom_cb_t) go_deep_scalc, NULL);
	
	rework_types(wtt, wet, "qtypel.in");
	
	/*struct exec_descr* run = reg_cpts_srun();
	
	attach_cpts_operator(run);
	attach_cpts_assign(run);
	attach_cpts_func(run);
	call_atree(run, scalc, & op);

	wrxml(scalc);*/
	
	struct attnode* scalc = generate_att_tree("test.gr", NULL, 0);
	
	if (! call_atree(wtt, scalc, NULL) )
		cpl_debug("type error\n");
	wrxml(scalc);
	struct test_ctx ctx;
	call_atree(wet, scalc, & ctx);
	
	return 0;
}

void
spawn(char* path, struct test_ctx* fds) {
	int in[2];
	int out[2];
	pid_t childpid;
	
	pipe(in);
	pipe(out);
	
	if((childpid = fork()) == -1) {
		printf("fork fatal\n");
		return;
	}
	
	if(childpid == 0) {
		//close(0);
		//close(1);
		//close(2);
		dup2(in[0], 0);
		dup2(out[1], 1);
		close(in[1]);
		close(out[0]);
		
		chdir("..");
		chdir(path);
		execlp("./intle", "./intle", NULL);
		printf("exec fatal\n");
	} else {
		close(in[0]);
		close(out[1]);
		fds->out = in[1];
		fds->in = out[0];
	}
}

int
calltest(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	go_deep_scalc(ctx, node, input, closure);
	printf("Test!\n");
	return 1;
}

int
header(struct exec_descr* ctx, struct attnode* node, struct test_ctx* input, void* closure)
{
	printf("Header!\n");
	go_deep_scalc(ctx, node, input, closure);
	char* path = atree_get_subnode_attr(node, 1, "value");
	printf(path);
	printf("\n");
	spawn(path, input);
	return 1;
}

int
command(struct exec_descr* ctx, struct attnode* node, struct test_ctx* input, void* closure)
{
	printf("Command!\n");
	go_deep_scalc(ctx, node, input, closure);
	char* action = atree_get_subnode_attr(node, 1, "value");
	char* expected = atree_get_subnode_attr(node, 2, "value");
	int len_action = strlen(action);
	int len_expected = strlen(expected);
	char* actual = mem_alloc(len_expected + 2);
	write(input->out, action, len_action);
	close(input->out);
	int c = read(input->in, actual, len_expected + 1);
	actual[c] = 0;
	if (c == len_expected && ! strncmp(actual, expected, c))
		printf("Test passed\n");
	else
		printf("Test failed: %d'%s' != %d'%s'\n", c, actual, len_expected, expected);
	return 1;
}

int
quot(struct exec_descr* ctx, struct attnode* node, void* input, void* closure)
{
	printf(atree_get_attr(node, "value"));
	printf("\n");
	return 1;
}

int
equal(struct attnode* node, char* typename)
{
	return 1;
}


