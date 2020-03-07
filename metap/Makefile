SRCS=../cpl/utils.c ../cpl/tables.c ../cpl/tokenizer.c ../exec/exec_tree.c pmp.c main.c

pmp: $(SRCS:.c=.o)
	gcc -o $@ $^

.c.o:
	gcc -M -MF $*.d -c $< -I../cpl -I../exec
	gcc -o $@ -c $< -I../cpl -I../exec -Wall -Werror

-include $(SRCS:.c=.d)
