#include "iochannel.h"
#include "utils.h"
#include <unistd.h>

static struct transact_list*
iochannel_begin(struct iochannel* io)
{
	struct transact_list* tr;
	tr = mem_alloc(sizeof(*tr));
	tr->next = NULL;
	tr->prev = io->last;
	//cpl_debug("enter %p %d\n", io->iobuf, io->pos);
	
	tr->iobuf = io->iobuf;
	tr->pos = io->pos;
	
	if (io->last)
		io->last->next = tr;
	io->last = tr;
	
	return tr;
}

static void
iochannel_recover(struct iochannel* io)
{
	struct transact_list* tr;
	tr = io->last;
	//cpl_debug("exit %p %d\n", tr->iobuf, tr->pos);
	
	io->pos = tr->pos;
	io->iobuf = tr->iobuf;
	
	io->last = tr->prev;
	if (tr->prev)
		tr->prev->next = NULL;
	
	mem_free(tr);
	
	if (!io->iobuf)
		io->iobuf = io->iobufs;
}

static void
iochannel_recover_to(struct iochannel* io, struct transact_list* to)
{
	while (io->last != to)
		io->recover(io);
	io->recover(io);
}

static void
io_fill_next(struct iochannel* io)
{
	struct iobuf_list* buf;
	struct iobuf_list** c;
	int r;
	buf = mem_alloc(sizeof(*buf));
	//cpl_debug("new buf, old: %p\n", io->iobuf);
	
	c = &(io->iobufs);
	while (*c)
		c = &((*c)->next);
	
	buf->next = NULL;
	buf->prev = *c;
	(*c) = buf;
	buf->iobuf = mem_alloc(IOBUFLEN);
	
	r = read(io->fd, buf->iobuf, IOBUFLEN);
	buf->len = r;
	
	if (io->stopped(io, r))
		io->stop = 1;
	
	io->iobuf = buf;
	
	//cpl_printf("read %d bytes from %d\n", r, io->fd);
}

static int
copy_existing(struct iochannel* io, char* buf, int len)
{
	int r = 0, cr;
	struct iobuf_list* iobuf = io->iobuf;
	//cpl_debug("old: %p\n", io->iobuf);
	
	while (iobuf && len) {
		cr = (iobuf->len - io->pos);
		if (len <= cr) {
			//cpl_debug("old: %d %d\n", iobuf->len, io->pos);
			mem_copy(buf, iobuf->iobuf + io->pos, len);
			io->pos += len;
			r += len;
			return r;
		} else {
			//cpl_debug("new: %d %d\n", iobuf->len, io->pos);
			mem_copy(buf, iobuf->iobuf + io->pos, cr);
			io->pos = 0;
			r += cr;
			len -= cr;
			iobuf = io->iobuf = iobuf->next;
			buf += cr;
		}
	}
	
	return r;
}

static char *
iochannel_read(struct iochannel* io, int len)
{
	/*
	 * 4 случая. есть 2 границы - прочитанного и блока.
	 * никакая граница не пересекается.
	 * пересекается прочитанное
	 * пересекается блок. или несколько блоков.
	 * 
	 */
	 char* new = mem_alloc(len + 1);
	 char* pos = new;
	 int rd;
	 int stp;
	 //cpl_debug("\nread: pos - %d buf - %p len - %d\n", io->pos, io->iobuf, len);
	 
	 while (1) {
		rd = copy_existing(io, pos, len);
		//cpl_debug("rd: %d\n", rd);
		pos += rd;
		len -= rd;
		stp = io->stop;
		if (stp || (0 == len)) {
			*pos = '\0';
			return new;
		}
		io_fill_next(io);
	}
}

void static
iochannel_commit(struct iochannel* io)
{
	if (io->last->prev) {
		io->last = io->last->prev;
		mem_free(io->last->next);
		io->last->next = NULL;
	} else {
		mem_free(io->last);
		io->last = NULL;
	}
}

int static
iochannel_stopped(struct iochannel* io, int read)
{
	return (0 == read);
}

struct iochannel *
iochannel_new(int fd)
{
	struct iochannel* io;
	io = mem_alloc(sizeof(*io));
	
	io->recover = iochannel_recover;
	io->begin = iochannel_begin;
	io->read = iochannel_read;
	io->commit = iochannel_commit;
	io->recover_to = iochannel_recover_to;
	
	io->iobufs = NULL;
	io->iobuf = NULL;
	io->last = NULL;
	io->stop = 0;
	io->pos = 0;
	
	io->fd = fd;
	
	io->stopped = iochannel_stopped;
	
	return io;
}
