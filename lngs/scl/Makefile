
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
		../../ptemplate/tokseek.c \
		../qscript/typel.c ../qscript/general.c \
		sclexec.c sclmain.c \
		scall/gate.c agen/agen.c \
		treew/treew.c treew/vartable.c \
		../../mips/stm.c \
		sgen/typeassign.c

INC=-I../../ptemplate -I../../cpl -I../../exectpl -I../../channel \
		-I../pgen -I../atrans -I../qscript -I../scl -I../fl

CFLAGS=-g -Wall -Werror -ldl -rdynamic -lm -lffi

intle: $(SRCS:.c=.o)
	gcc -o $@ $^ $(CFLAGS)

.c.o:
	gcc -M -MF $*.d -g -c $< $(CFLAGS) $(INC)
	gcc -o $@ -c $< $(CFLAGS) $(INC)

-include $(SRCS:.c=.d)

clean:
	rm -f $(SRCS:.c=.o)
	rm -f $(SRCS:.c=.d)



