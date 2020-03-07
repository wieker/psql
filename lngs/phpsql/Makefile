SRCS=parser_wrong.c ../../ptemplate/register.c \
		../../ptemplate/stdp.c ../../ptemplate/iochannel.c \
		../../ptemplate/parser.c ../../ptemplate/ptypes.c ../../cpl/utils.c

INC=-I../../ptemplate -I../../cpl

exec_expr: $(SRCS:.c=.o)
	gcc -o $@ $^ -lm

.c.o:
	gcc -M -MF $*.d -c $< $(INC)
	gcc -o $@ -c $< -Wall -Werror $(INC)

-include $(SRCS:.c=.d)

