SRCS= ../../ptemplate/register.c \
		../../ptemplate/stdp.c ../../ptemplate/iochannel.c \
		../../ptemplate/parser.c ../../ptemplate/ptypes.c \
		../../cpl/utils.c ../../ptemplate/stdt.c \
		../../ptemplate/stdo.c ../../exectpl/exec_tree.c \
		gen_atree.c ptpl2atree.c attree.c \
		../../channel/repl0.c ../../channel/repl_kbd.c \
		wrxml.c apply_path_tree.c \
		../../ptemplate/stdbe.c ../../ptemplate/stde.c \
		../../ptemplate/tokseek.c

INC=-I../../ptemplate -I../../cpl -I../../exectpl -I../../channel

CFLAGS=-g -Wall -Werror

intle: $(SRCS:.c=.o)
	gcc -o $@ $^ -lm $(CFLAGS)

.c.o:
	gcc -M -MF $*.d -g -c $< $(CFLAGS) $(INC)
	gcc -o $@ -c $< $(CFLAGS) $(INC)

-include $(SRCS:.c=.d)

clean:
	rm -f $(SRCS:.c=.o)



