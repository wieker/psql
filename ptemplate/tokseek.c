#include "tokseek.h"
#include "string.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>

int
seek_space(struct parser_context* parser, struct iochannel* io)
{
	char* i;
	int s = 0;
	
	io->begin(io);
	for (;;) {
		i = io->read(io, 1);
		if (! *i) {
			io->recover(io);
			mem_free(i);
			return s;
		}
		if (! strchr(parser->spaces, *i)) {
			io->recover(io);
			mem_free(i);
			return s;
		}
		s += 1;
		mem_free(i);
	}
	return 0;
}

int
seek_simple_key(struct iochannel* io)
{
	char* i;
	int s = 0;
	
	io->begin(io);
	for (;;) {
		i = io->read(io, 1);
		if (! *i) {
			io->recover(io);
			mem_free(i);
			return s;
		}
		if (isalpha(*(unsigned char *)i) || (*i == '_')) {
			mem_free(i);
			s ++;
		} else {
			io->recover(io);
			mem_free(i);
			return s;
		}
	}
	return 0;
}

int
seek_simple_number(struct iochannel* io)
{
	char* i;
	int s = 0;
	
	io->begin(io);
	for (;;) {
		i = io->read(io, 1);
		if (! *i) {
			io->recover(io);
			mem_free(i);
			return s;
		}
		if (isdigit(*(unsigned char *)i)) {
			mem_free(i);
			s ++;
		} else {
			io->recover(io);
			mem_free(i);
			return s;
		}
	}
	return 0;
}

int
seek_dot_number(struct iochannel* io)
{
	char* i;
	int s = 0;
	io->begin(io);
	i = io->read(io, 1);
	if (! *i) {
		io->recover(io);
		return s;
	}
	if ((*i) == '.') {
		mem_free(i);
		s = seek_simple_number(io);
	}
	io->recover(io);
	return s;
}

int
seek_backslash(struct iochannel* io, int* type)
{
	char* i;
	int s = 1;
	while (1) {
		i = io->read(io, 1);
		if (! *i) {
			*type = 0;
			s = -1;
			break;
		}
		if ((*i) == '\\') {
			*type = 1;
			mem_free(i);
			break;
		}
		if ((*i) == '"') {
			*type = 2;
			mem_free(i);
			break;
		}
		mem_free(i);
		s ++;
	}
	return s;
}

int
seek_const(struct iochannel* io, char* cpt)
{
	char* i;
	while (*cpt) {
		i = io->read(io, 1);
		if (!*i) {
			mem_free(i);
			return 0;
		}
		if (*cpt != *i) {
			mem_free(i);
			return 0;
		}
		mem_free(i);
		cpt ++;
	}
	return 1;
}

int
seek_doublequote_close(struct iochannel* io)
{
	char* i;
	int state = 0; // 0 - at the begining of the processing; 1 - in the middle; 2 - after the \ symbol
	int s = 0;
	while (1) {
		i = io->read(io, 1);
		switch (*i) {
			case '"':
			if (state == 0) {
				state = 1;
				mem_free(i);
				s ++;
				continue;
			}
			if (state == 1) {
				state = 1;
				mem_free(i);
				s ++;
				return s;
			}
			if (state == 2) {
				state = 1;
				mem_free(i);
				s ++;
				continue;
			}
			break;
			case '\\':
			if (state == 0) {
				mem_free(i);
				return -1;
			}
			if (state == 1) {
				state = 2;
				mem_free(i);
				s ++;
				continue;
			}
			if (state == 2) {
				state = 1;
				mem_free(i);
				s ++;
				continue;
			}
			break;
			default:
			if (state == 0) {
				mem_free(i);
				return -1;
			}
			if (state == 1) {
				mem_free(i);
				s ++;
				continue;
			}
			if (state == 2) {
				mem_free(i);
				return -1;
			}
		}
	}
	return -1;
}
