#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "scl.h"
#include "scalc.h"

#include <dlfcn.h>
#include <ffi.h>

static int *
alloc_int(char* str)
{
	int* p = mem_alloc(sizeof(int));
	*p = atoi(str);
	return p;
}

static float *
alloc_float(char* str)
{
	float* p = mem_alloc(sizeof(float));
	*p = atof(str);
	return p;
}

static void *
alloc_ptr(void* str)
{
	char** p = mem_alloc(sizeof(p));
	*p = (str);
	return p;
}

static void *
get_cb(char* funcname, struct scl_callgate_context* cc)
{
	int i = 0;
	void* _cb;
	while (! (_cb = dlsym(cc->handles[i], funcname)) && i < cc->L)
		i ++;
	return _cb;
}

int
ccall_gate(struct exec_descr* ctx, struct attnode* node, struct scl_input* input, void* closure)
{
	char* funcname;
	int count;
	void* _cb;
	ffi_type **argtypes;
	ffi_cif ffi;
	int i = 0;
	int rc;
	void **values;
	int stn;
	char* name;
	
	funcname = atree_get_subnode_attr(node, 1, "value");
	_cb = get_cb(funcname, input->callgate_ctx);
	cpl_debug("recore for %s is %p caused %s\n", funcname, _cb, dlerror());
	
	count = COUNT_ELS(node->subtrees) - 1;
	argtypes = mem_alloc(sizeof(*argtypes) * count);
	values = mem_alloc(sizeof(*values) * count);
	
	struct list_iter params = list_iter(node->subtrees);
	NEXT_EL(& params);
	i = 0;
	while (CONT(& params)) {
		if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "string")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_get_subnode_attr(node, i + 2, "value"));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "sqstring")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_get_subnode_attr(node, i + 2, "value"));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "rinteger")) {
			argtypes[i] = &ffi_type_sint;
			values[i] = alloc_int(atree_get_subnode_attr(atree_subtree(node, i + 2), 1, "value"));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "float")) {
			argtypes[i] = &ffi_type_float;
			values[i] = alloc_float(atree_get_subnode_attr(node, i + 2, "value"));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "tree")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(node);
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "ctx")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(ctx);
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "input")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(input);
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "input_subtree")) {
			stn = atoi(atree_get_subnode_attr(GET_EL(& params), 1, "value"));
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_subtree(input->node_to_process, stn));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "closure")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(closure);
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "subtree")) {
			stn = atoi(atree_get_subnode_attr(GET_EL(& params), 2, "value"));
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_subtree(node, stn));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "attr")) {
			name = (atree_get_subnode_attr(GET_EL(& params), 2, "value"));
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_get_attr(node, name));
		} else if (! strcmp(atree_get_attr(GET_EL(& params), "name"), "inval")) {
			argtypes[i] = &ffi_type_pointer;
			values[i] = alloc_ptr(atree_get_attr(input->node_to_process, "value"));
		} else {
			_cb = NULL;
			values[i] = NULL;
			break;
		}
		NEXT_EL(& params);
		i ++;
	}
	
	rc = 0;
	if (_cb && (ffi_prep_cif(&ffi, FFI_DEFAULT_ABI, count,
				&ffi_type_uint, argtypes) == FFI_OK)) {
		ffi_call(&ffi, _cb, &rc, values);
	}
	
	params = list_iter(node->subtrees);
	NEXT_EL(& params);
	i = 0;
	while (CONT(& params) && values[i]) {
		free(values[i]);
		NEXT_EL(& params);
		i ++;
	}
	
	if (! rc)
		cpl_debug("failed params for %s is %p\n", funcname, _cb);
	return rc;
}

int
small_test_blabla()
{
	printf("%s\n", "test");
	return 1;
}

int
small_test_blabla_print(char* str)
{
	printf("%s\n", str);
	return 1;
}


int
ccall_callarg(struct exec_descr* ctx, struct attnode* node, struct scl_input* input, void* closure)
{
	char* funcname;
	void* _cb;
	char* value;
	
	funcname = atree_get_subnode_attr(node, 1, "value");
	_cb = get_cb(funcname, input->callgate_ctx);
	cpl_debug("callarg for %s is %p caused %s\n", funcname, _cb, dlerror());
	
	value = atree_get_attr(input->node_to_process, "value");
	int (*f)(char*) = _cb;
	
	return f(value);
}

int
header_processor(struct exec_descr* ctx, struct attnode* node, struct scl_reg* input, void* closure)
{
	struct scl_callgate_context* cc = mem_alloc(sizeof(*cc));
	input->callgate_ctx = cc;
	
	cc->L = 1;
	cc->handles[0] = dlopen(NULL, RTLD_LAZY);
	
	fprintf(stderr, "%p\n", cc->handles[0]);
	
	return 1;
}

int
ccall_loadm(struct exec_descr* ctx, struct attnode* node, struct scl_input* input, void* closure)
{
	char* mname;
	void* handle;
	char str[128];
	
	mname = atree_get_subnode_attr(node, 1, "value");
	if (! mname)
		return 0;
	
	snprintf(str, sizeof(str), "../../modules/%s/%s.so", mname, mname);
	
	handle = dlopen(str, RTLD_LAZY);
	if (! handle)
		return 0;
	
	input->callgate_ctx->handles[input->callgate_ctx->L] = handle;
	input->callgate_ctx->L ++;
	
	return 1;
}
