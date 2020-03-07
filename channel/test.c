#include "iochannel.h"
#include "repl_kbd.h"
#include <stdio.h>

int config_debug = 1;

void static
flush(struct iochannel* io)
{
	struct iobuf_list* buf;
	buf = io->iobufs;
	while (buf) {
		//cpl_debug("buf len: %d\n", buf->len);
		//fflush(stdin);
		write(1, buf->iobuf, buf->len);
		//cpl_debug("\nbuf end\n");
		buf = buf->next;
	}
}

int
main(int argc, char** argv)
{
	struct iochannel* io = new_repl_kbd(0);
	char* s;
	(io->begin(io));
	while (*(s = (io->read(io, 7))) != 0) ;// printf("%s", s);
	(io->recover(io));
	while (*(s = (io->read(io, 7))) != 0) printf("%s", s);
	//flush(io);
}
