#include "repl0.h"

#include "iochannel.h"
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>

struct iochannel *
new_repl_0(char* file)
{
	int fd = open(file, O_RDONLY);
	struct iochannel* io = iochannel_new(fd);
	struct iochannel_repl* repl;
	repl = mem_alloc(sizeof(*repl));
	
	(*(struct iochannel*) repl) = *io;
	repl->name = mem_strdup(file);
	
	return (struct iochannel *) repl;
}

void
repl_0_reset(struct iochannel_repl* io)
{
	close(io->base.fd);
	io->base.fd = open(io->name, O_RDONLY);
	
	io->base.stop = 0;
	io->base.pos = 0;
	io->base.last = NULL;
	io->base.iobuf = io->base.iobufs;
}

