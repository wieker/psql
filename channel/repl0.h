#ifndef REPL0_H
#define REPL0_H

#include "iochannel.h"

struct iochannel_repl {
	struct iochannel base;
	char* name;
};

struct iochannel* new_repl_0(char* file);

void repl_0_reset(struct iochannel_repl* io);


#endif


