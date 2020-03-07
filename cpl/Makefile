SRCS=gen_tree.c plang.c pexpr.c tokenizer.c utils.c tables.c ../exec/exec_tree.c

cpl: $(SRCS:.c=.o)
	gcc -o $@ $^

.c.o:
	gcc -M -MF $*.d -c $<
	gcc -o $@ -c $< -Wall -Werror

-include $(SRCS:.c=.d)
