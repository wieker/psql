SRCS=exec_expr.c ../cpl/pexpr.c ../cpl/utils.c ../cpl/tables.c ../cpl/tokenizer.c exec_tree.c \
	type_cast.c builtin_fncs.c

exec_expr: $(SRCS:.c=.o)
	gcc -o $@ $^ -lm

.c.o:
	gcc -M -MF $*.d -c $< -I../cpl
	gcc -o $@ -c $< -I../cpl -Wall -Werror

-include $(SRCS:.c=.d)

