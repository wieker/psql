#ifndef IOCHANNEL_H
#define IOCHANNEL_H

#define IOBUFLEN 10000

struct iobuf_list {
	struct iobuf_list* next;
	struct iobuf_list* prev;
	char* iobuf;
	int len;
};

struct transact_list {
	struct transact_list* next;
	struct transact_list* prev;
	struct iobuf_list* iobuf;
	int pos;
};

struct iochannel {
	char* (*read)(struct iochannel* channel, int size);
	struct transact_list* (*begin)(struct iochannel* channel);
	void (*recover)(struct iochannel* channel);
	void (*commit)(struct iochannel* channel);
	void (*recover_to)(struct iochannel* channel, struct transact_list* to);
	
	int fd;
	struct iobuf_list* iobufs;
	struct transact_list* last;
	int stop;
	
	int pos;
	struct iobuf_list* iobuf;
	
	int (*stopped)(struct iochannel* channel, int read);
};

struct iochannel* iochannel_new(int fd);

#endif
