#include "repl_kbd.h"

#include "iochannel.h"
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>

#include "repl0.h"

int static
kbd_stopped(struct iochannel* io, int read)
{
	return (0 == read);
}

struct iochannel *
new_repl_kbd(int fd)
{
	struct iochannel* io = iochannel_new(fd);
	struct iochannel_repl* repl;
	repl = mem_alloc(sizeof(*repl));
	
	io->stopped = kbd_stopped;
	(*(struct iochannel*) repl) = *io;
	cpl_debug("kbd init\n");
	
	return (struct iochannel *) repl;
}

void
repl_kbd_reset(struct iochannel_repl* io, int succ)
{
	cpl_debug("kbd reset\n");
	io->base.stop = 0;
	io->base.pos = 0;
	io->base.last = NULL;
	if (succ) {
		io->base.iobuf = io->base.iobufs = NULL;
	} else {
		io->base.iobuf = io->base.iobufs;
	}
}

