#ifndef TOKSEEK_H
#define TOKSEEK_H

#include "iochannel.h"
#include "register.h"
#include "parser.h"

int seek_space(struct parser_context* parser, struct iochannel* io);

int seek_simple_key(struct iochannel* io);

int seek_simple_number(struct iochannel* io);

int seek_dot_number(struct iochannel* io);

int seek_backslash(struct iochannel* io, int* type);

int seek_doublequote_close(struct iochannel* io);

int seek_const(struct iochannel* io, char* cpt);

#endif
