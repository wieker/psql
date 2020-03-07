#ifndef TOKENIZER_H
#define TOKENIZER_H

int scan_token(int ttype, int* token);

extern char* tok_name;
extern char* input;
extern struct table* table_funcs;


#define TOK_ANY 1
#define TOK_COMMA 2
#define TOK_ANDOR 3
#define TOK_AND 4
#define TOK_OR 5
#define TOK_NOT 6
#define TOK_COMPARE 7
#define TOK_LESS 8
#define TOK_MORE 9
#define TOK_EQUAL 10
#define TOK_SUM 11
#define TOK_PLUS 12
#define TOK_MINUS 13
#define TOK_PROD 14
#define TOK_MUL 15
#define TOK_DIV 16
#define TOK_POW 17
#define TOK_LB 18
#define TOK_RB 19
#define TOK_NUMBER 20
#define TOK_STRING 21
#define TOK_NAME 22
#define TOK_VALUE 23

#define TOK_SEMICOLON 24
#define TOK_TCL_WHILE 25
#define TOK_FLB 26
#define TOK_FRB 27
#define TOK_OPEN_BRACKET 28
#define TOK_CLOSE_BRACKET 29
#define TOK_BRINS 30
#define TOK_DOLLAR 31
#define TOK_DQUOTES 32
#define TOK_TCL_STRIN_DQUOTES 33

#define TOK_EQ 34
#define TOK_ASSIGN 35


#endif

