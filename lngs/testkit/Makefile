SRCS= \
		../../ptemplate/register.c \
		../../ptemplate/stdp.c \
		../../ptemplate/iochannel.c \
		../../ptemplate/parser.c \
		../../ptemplate/ptypes.c \
		../../ptemplate/stdbe.c \
		../../ptemplate/tokseek.c \
		../../ptemplate/stde.c \
		../../ptemplate/stdt.c \
		../../ptemplate/stdo.c \
		\
		../../exectpl/exec_tree.c \
		\
		../pgen/gen_atree.c \
		../pgen/ptpl2atree.c \
		../pgen/attree.c \
		../pgen/wrxml.c \
		../atrans/atttree_helper.c \
		../atrans/atrex.c \
		\
		../../channel/repl_kbd.c \
		../../channel/repl0.c \
		../../cpl/tables.c \
		../../cpl/utils.c \
		\
		../qscript/general.c \
		../qscript/typel.c \
		\
		testkit.c

INC=-I../../ptemplate -I../../cpl -I../../exectpl -I../../channel -I../pgen -I../atrans -I../qscript

CFLAGS=-g -Wall -Werror -ldl -rdynamic

intle: $(SRCS:.c=.o)
	gcc -o $@ $^ -lm $(CFLAGS)

.c.o:
	gcc -M -MF $*.d -g -c $< $(CFLAGS) $(INC)
	gcc -o $@ -c $< $(CFLAGS) $(INC)

-include $(SRCS:.c=.d)

clean:
	rm -f $(SRCS:.c=.o)
	rm -f $(SRCS:.c=.d)

install: intle
	cp -r intle rhelper scripts expin.gr $(DST)



