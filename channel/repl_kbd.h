#ifndef REPL_KBD_H
#define REPL_KBD_H

#include "iochannel.h"

#include "repl0.h"

struct iochannel* new_repl_kbd(int fd);

void repl_kbd_reset(struct iochannel_repl* io, int succ);


#endif



