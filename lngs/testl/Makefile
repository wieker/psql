SRCS=parser_wrong.c ../../ptemplate/register.c \
		../../ptemplate/stdp.c ../../ptemplate/iochannel.c \
		../../ptemplate/parser.c ../../ptemplate/ptypes.c \
		../../cpl/utils.c ../../ptemplate/stdt.c \
		../../ptemplate/stdo.c ../../exectpl/exec_tree.c \
		../../cpl/tables.c \
		../../channel/repl_kbd.c \
		exec_expr1.c reg_expr0.c regop0.c exec_op0.c lvalue0.c \
		func0.c

INC=-I../../ptemplate -I../../cpl -I../../exectpl -I../../channel

CFLAGS=-g -Wall -Werror

testle: $(SRCS:.c=.o)
	gcc -o $@ $^ -lm $(CFLAGS)

.c.o:
	gcc -M -MF $*.d -g -c $< $(CFLAGS) $(INC)
	gcc -o $@ -c $< $(CFLAGS) $(INC)

-include $(SRCS:.c=.d)

clean:
	rm -f $(SRCS:.c=.o)

