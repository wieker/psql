EXPLIBS=/home/abramovich/srcs/

SRCS= ../../ptemplate/register.c \
		../../ptemplate/stdp.c ../../ptemplate/iochannel.c \
		../../ptemplate/parser.c ../../ptemplate/ptypes.c \
		../../cpl/utils.c ../../ptemplate/stdt.c \
		../../ptemplate/stdo.c ../../exectpl/exec_tree.c \
		../pgen/gen_atree.c ../pgen/ptpl2atree.c ../pgen/attree.c \
		../../channel/repl0.c ../pgen/wrxml.c \
		../atrans/atttree_helper.c ../atrans/atrex.c \
		../../channel/repl_kbd.c \
		../../cpl/tables.c \
		../../ptemplate/stdbe.c ../../ptemplate/stde.c \
		../qscript/typel.c ../qscript/general.c \
		$(EXPLIBS)/libkvec/lowlibs/png.c \
		picexc_s.c picmain.c \
		../../ptemplate/tokseek.c

INC=-I../../ptemplate -I../../cpl -I../../exectpl -I../../channel -I../pgen -I../atrans -I../qscript \
		-I$(EXPLIBS)

CFLAGS=-g -Wall -Werror -ldl -rdynamic -lpng 

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



