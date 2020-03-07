#include "tokenizer.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

char* input = "1 + 1";
int tokpos = 0;
char* tok_name;

typedef int (*lexer_t)(char* pattern);
int recogn_token(char* pattern);
int scan_recurse(char* pattern);
int token_border(char* pattern);

struct tok_descr {
	int token;
	char* grammar;
	lexer_t lexer;
} tokens[] = {
	{TOK_ANY, "\\oneof", recogn_token},
	{TOK_COMMA, ",", recogn_token},
	{TOK_ANDOR, "\\oneof", recogn_token},
	{TOK_AND, "and", recogn_token},
	{TOK_OR, "or", recogn_token},
	{TOK_ANDOR, "\\endof", recogn_token},
	{TOK_NOT, "not", recogn_token},
	{TOK_COMPARE, "\\oneof", recogn_token},
	{TOK_LESS, "<", recogn_token},
	{TOK_EQ, "==", recogn_token},
	{TOK_MORE, ">", recogn_token},
	{TOK_COMPARE, "\\endof", recogn_token},
	{TOK_EQUAL, "=", recogn_token},
	{TOK_SUM, "\\oneof", recogn_token},
	{TOK_PLUS, "+", recogn_token},
	{TOK_MINUS, "-", recogn_token},
	{TOK_SUM, "\\endof", recogn_token},
	{TOK_PROD, "\\oneof", recogn_token},
	{TOK_MUL, "*", recogn_token},
	{TOK_DIV, "/", recogn_token},
	{TOK_PROD, "\\endof", recogn_token},
	{TOK_POW, "^", recogn_token},
	{TOK_VALUE, "\\oneof", recogn_token},
	{TOK_LB, "(", recogn_token},
	{TOK_NUMBER, "\\+\\d\\?.\\*\\d", recogn_token},
	{TOK_STRING, "\"\\+\\a\"", recogn_token},
	{TOK_NAME, "\\+\\a", recogn_token},
	{TOK_VALUE, "\\endof", recogn_token},
	{TOK_RB, ")", recogn_token},
	{TOK_ANY, "\\endof", recogn_token},
	
	
	{TOK_SEMICOLON, ";", recogn_token},
	{TOK_TCL_WHILE, "while", recogn_token},
	{TOK_FLB, "{", recogn_token},
	{TOK_FRB, "}", recogn_token},
	{TOK_OPEN_BRACKET, "(", recogn_token},
	{TOK_CLOSE_BRACKET, ")", recogn_token},
	{TOK_BRINS, "\\{}", scan_recurse},
	{TOK_DOLLAR, "$", recogn_token},
	{TOK_DQUOTES, "\"", recogn_token},
	{TOK_TCL_STRIN_DQUOTES, "$\"", token_border},
	
	
	{TOK_ASSIGN, "=", recogn_token},
	{0, 0},
};

/*
 * check long
 * 0 - один символ
 * 1 - 0 или 1 символ (?)
 * 2 - 1 или больше символов (+)
 * 4 - 0 или больше символов (*)
 */
int
check_long(char* pattern, int* step)
{
	int rt = 0;
	*step = 0;
	if (*pattern != '\\')
		return 0;
	if (! strncmp(pattern, "\\?", 2)) {
		*step = 2;
		rt = 1;
	}
	if (! strncmp(pattern, "\\+", 2)) {
		*step = 2;
		rt = 2;
	}
	if (! strncmp(pattern, "\\*", 2)) {
		*step = 2;
		rt = 4;
	}
	return rt;
}

int
cmp_group(char* pattern, int* step, int stream_pos)
{
	if (*pattern != '\\') {
		*step = 1;
		return ((*pattern) == input[stream_pos]);
	}
	*step = 2;
	if (! strncmp(pattern, "\\d", 2))
		return isdigit(input[stream_pos]);
	if (! strncmp(pattern, "\\a", 2))
		return isalpha(input[stream_pos]);
	return 0;
}

int
inc_stream(int* stream_pos)
{
	if (!input[*stream_pos])
		return 0;
	else
		(*stream_pos) ++;
	return 1;
}

/*
 * распознавание токена. возвращает - если попал.
 */
int
recogn_token(char* pattern)
{
	int l, c, s;
	int was = 0;
	int stream_pos, ppos;
	//printf("PATTERN: %s\n", pattern);
	ppos = stream_pos = tokpos;
	while (isspace(input[stream_pos])) {
		stream_pos ++; ppos ++;
	}
	while (*pattern) {
		l = check_long(pattern, &s);
		pattern += s;
		was = 0;
		do {
			c = cmp_group(pattern, &s, stream_pos);
			//printf("'%s' '%s' %d %d %d %d\n", pattern, &input[stream_pos], l, c, was, s);
			if (l == 0 && c == 0)
				return 0;
			if (l == 0 && c) {
				pattern += s;
				if (! inc_stream(&stream_pos))
					return 0;
				break;
			}
			
			if (l == 1 && c == 0) {
				pattern += s;
				break;
			}
			if (l == 1 && c) {
				pattern += s;
				if (! inc_stream(&stream_pos))
					return 0;
				break;
			}
			
			if (l == 2 && c == 0 && was == 0)
				return 0;
			if (l == 2 && c == 0 && was == 1) {
				pattern += s;
				break;
			}
			if (l == 2 && c) {
				if (! inc_stream(&stream_pos))
					return 0;
				was = 1;
				continue;
			}
			
			if (l == 4 && c == 0 && was == 0) {
				pattern += s;
				break;
			}
			if (l == 4 && c == 0 && was == 1) {
				pattern += s;
				break;
			}
			if (l == 4 && c) {
				if (! inc_stream(&stream_pos))
					return 0;
				was = 1;
				continue;
			}
			printf("FATAL\n");
		} while (1);
	}
	free(tok_name);
	tok_name = strndup(&input[ppos], stream_pos - ppos + 1);
	tok_name[stream_pos - ppos] = 0x0;
	tokpos = stream_pos;
	return 1;
}

int
token_border(char* pattern)
{
	int stream_pos;
	char* ptr;
	//printf("PATTERN: %s\n", pattern);
	stream_pos = tokpos;
	while (input[stream_pos]) {
		ptr = pattern;
		while (ptr[0] != input[stream_pos])
			ptr ++;
		if (ptr[0])
			break;
		stream_pos ++;
	}
	stream_pos --;
	free(tok_name);
	tok_name = strndup(&input[tokpos], stream_pos - tokpos + 1);
	tok_name[stream_pos - tokpos] = 0x0;
	tokpos = stream_pos;
	return 1;
}

int
scan_recurse(char* pattern)
{
	int stream_pos;
	int deep;
	//printf("PATTERN: %s\n", pattern);
	stream_pos = tokpos;
	deep = 1;
	while (input[stream_pos]) {
		if (input[stream_pos] == pattern[0])
			deep ++;
		if (input[stream_pos] == pattern[1])
			deep --;
		if (! deep)
			break;
		stream_pos ++;
	}
	stream_pos --;
	free(tok_name);
	tok_name = strndup(&input[tokpos], stream_pos - tokpos + 1);
	tok_name[stream_pos - tokpos] = 0x0;
	tokpos = stream_pos;
	return 1;
}

int
check_list(struct tok_descr* tkdsc)
{
	return ! strcmp(tkdsc->grammar, "\\oneof");
}

int
end_list(struct tok_descr* tkdsc)
{
	return ! strcmp(tkdsc->grammar, "\\endof");
}

int
scan_token(int ttype, int* token)
{
	struct tok_descr* tkdsc = tokens;
	int inside = 0;
	int tmptok;
	
	//printf("SCAN TOKEN: %d\n", ttype);
	if (! token)
		token = &tmptok;
	for(;tkdsc->token; tkdsc ++) {
		if (ttype == tkdsc->token && check_list(tkdsc)) {
			inside = 1;
			continue;
		}
		if (ttype == tkdsc->token && end_list(tkdsc)) {
			inside = 0;
			continue;
		}
		if (inside && (check_list(tkdsc) || end_list(tkdsc))) {
			continue;
		}
		if (ttype == tkdsc->token || inside) {
			if (tkdsc->lexer(tkdsc->grammar)) {
				*token = tkdsc->token;
				return 1;
			}
		}
	}
	return 0;
}
