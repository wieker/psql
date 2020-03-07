#include <stdlib.h>
#include <stdio.h>

static int ip = 0;
int sp = 0;
int stack[10000];
extern int code[10000];
int bp = 0;
int data[10000];

int running = 1;

struct op {
	void (*opf)();
	int code;
};

void
stm_init(int *_code, int *_data)
{
	//code = _code;
	//data = _data;
	ip = 0;
	sp = 0;
	bp = 0;
	stack[0] = 0;
}

static void
jmp()
{
	ip = stack[sp];
	sp --;
}

static void
jnz()
{
	if (stack[sp - 1]) {
		ip = stack[sp];
		sp -= 2;
	} else {
		ip ++;
		sp -= 2;
	}
}

static void
lt()
{
	sp --;
	stack[sp] = (stack[sp] < stack[sp + 1]);
	
	ip ++;
}

static void
mul()
{
	sp --;
	stack[sp] = (stack[sp] * stack[sp + 1]);
	
	ip ++;
}

static void
lea()
{
	stack[sp] = (stack[sp] + bp);
	
	ip ++;
}

static void
load()
{
	stack[sp] = data[stack[sp]];
	
	ip ++;
}

static void
store()
{
	data[stack[sp]] = stack[sp - 1];
	sp -= 2;
	
	ip ++;
}

static void
li()
{
	sp ++;
	stack[sp] = code[ip + 1];
	
	ip += 2;
}

static void
add()
{
	sp --;
	stack[sp] = (stack[sp] + stack[sp + 1]);
	
	ip ++;
}

static void
sub()
{
	sp --;
	stack[sp] = (stack[sp] - stack[sp + 1]);
	
	ip ++;
}

static void
mdiv()
{
	sp --;
	stack[sp] = (stack[sp] / stack[sp + 1]);
	
	ip ++;
}

static void
and()
{
	sp --;
	stack[sp] = (stack[sp] & stack[sp + 1]);
	
	ip ++;
}

static void
or()
{
	sp --;
	stack[sp] = (stack[sp] | stack[sp + 1]);
	
	ip ++;
}

static void
not()
{
	stack[sp] =  ~ stack[sp];
	
	ip ++;
}

static void
eq()
{
	sp --;
	stack[sp] = (stack[sp] == stack[sp + 1]);
	
	ip ++;
}

static void
mt()
{
	sp --;
	stack[sp] = (stack[sp] > stack[sp + 1]);
	
	ip ++;
}

static void
dup()
{
	sp ++;
	stack[sp] = stack[sp - 1];
	
	ip ++;
}

static void
down()
{
	sp --;
	
	ip ++;
}

static void
neg()
{
	stack[sp] *= -1;
	
	ip ++;
}

static void
stop()
{
	running = 0;
	int i;
	for (i = 0; i <= sp; i ++)
		printf("reg %d == %d\n", i, stack[i]);
}

static void
lz()
{
	sp ++;
	stack[sp] = 0;
	
	ip += 1;
}

static void
ne()
{
	sp --;
	stack[sp] = (stack[sp] != stack[sp + 1]);
	
	ip ++;
}


struct op opcodes[] = {
	{jmp, 1},
	{jnz, 2},
	{lt, 3},
	{mul, 4},
	{lea, 5},
	{load, 6},
	{store, 7},
	//{leave, 8},
	//{enter, 9},
	//{ret, 10},
	{li, 11},
	{add, 12},
	{sub, 13},
	{mdiv, 14},
	{and, 15},
	{or, 16},
	{not, 17},
	{eq, 18},
	{mt, 19},
	{dup, 20},
	{down, 21},
	{neg, 22},
	{stop, 23},
	{lz, 24},
	{ne, 25},
	{NULL, 0},	
};

void
stm_run()
{
	printf("STM started\n");
	struct op *o;
	while (running) {
		o = opcodes;
		while (o->code != code[ip])
			o ++;
		o->opf();
	}
}
