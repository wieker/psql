#include <stdlib.h>
#include <string.h>

typedef unsigned int uint32_t;
typedef int int32_t;

struct mstate {
	int mem_len;
	
	unsigned char* mem;
	
	int32_t r[32];
	
	float fr[32];
	
	int32_t ip;
	
	char halt;
};

typedef void (*opexec)(struct mstate* mstate, void* input, void* closure);

opexec ops[256];

static void
add_int(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] / mstate->r[src2];
	mstate->ip += 8;
}

static void
add_float(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->fr[dst] = mstate->fr[src1] / mstate->fr[src2];
	mstate->ip += 8;
}

static void
sub_int(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] / mstate->r[src2];
	mstate->ip += 8;
}

static void
sub_float(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->fr[dst] = mstate->fr[src1] / mstate->fr[src2];
	mstate->ip += 8;
}

static void
mul_int(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] / mstate->r[src2];
	mstate->ip += 8;
}

static void
mul_float(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->fr[dst] = mstate->fr[src1] / mstate->fr[src2];
	mstate->ip += 8;
}

static void
div_int(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] / mstate->r[src2];
	mstate->ip += 8;
}

static void
div_float(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->fr[dst] = mstate->fr[src1] / mstate->fr[src2];
	mstate->ip += 8;
}

static void
load_word(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char dst = mstate->mem[mstate->ip + 1];
	uint32_t addr = *((uint32_t *) &(mstate->mem[mstate->ip + 2]));
	
	mstate->r[dst] = *((uint32_t *) &(mstate->mem[addr]));
	mstate->ip += 8;
}

static void
store_word(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char src = mstate->mem[mstate->ip + 1];
	uint32_t addr = *((uint32_t *) &(mstate->mem[mstate->ip + 2]));
	
	*((uint32_t *) &(mstate->mem[addr])) = mstate->r[src];
	mstate->ip += 8;
}

static void
and(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] & mstate->r[src2];
	mstate->ip += 8;
}

static void
or(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] | mstate->r[src2];
	mstate->ip += 8;
}

static void
xor(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] ^ mstate->r[src2];
	mstate->ip += 8;
}

static void
nor(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = ~ (mstate->r[src1] | mstate->r[src2]);
	mstate->ip += 8;
}

static void
less(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] < mstate->r[src2];
	mstate->ip += 8;
}

static void
shr(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] >> mstate->r[src2];
	mstate->ip += 8;
}

static void
shl(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	char dst = mstate->mem[mstate->ip + 2];
	
	mstate->r[dst] = mstate->r[src1] << mstate->r[src2];
	mstate->ip += 8;
}

static void
beq(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	char reg = mstate->mem[mstate->ip + 1];
	char src1 = reg & ~(0xff << 4);
	char src2 = reg & (0xff << 4);
	uint32_t addr = *((uint32_t *) &(mstate->mem[mstate->ip + 2]));
	
	if (mstate->r[src1] == mstate->r[src2])
		mstate->ip = addr;
	
	mstate->ip += 8;
}

static void
jmp(struct mstate* mstate, void* input, void* closure)
{
	char opcode = mstate->mem[mstate->ip];
	uint32_t addr = *((uint32_t *) &(mstate->mem[mstate->ip + 2]));
	
	mstate->ip = addr;
}

static void
hlt(struct mstate* mstate, void* input, void* closure)
{
	mstate->halt = 1;
}

void
init_state(struct mstate* mstate)
{
	mstate->mem_len = 1024 * 1024;
	mstate->mem = malloc(mstate->mem_len);
	mstate->halt = 0;
	mstate->ip = 0;
	int i;
	for (i = 0; i < 32; i ++) {
		mstate->r[i] = 0;
		mstate->fr[i] = 0;
	}
	ops[1] = add_int;
	ops[2] = sub_int;
	ops[3] = mul_int;
	ops[4] = div_int;
	
	ops[5] = load_word;
	ops[6] = store_word;
	
	
	ops[7] = and;
	ops[8] = or;
	ops[9] = xor;
	ops[10] = nor;
	
	ops[11] = less;
	
	ops[12] = shr;
	ops[13] = shl;
	
	ops[14] = beq;
	ops[15] = jmp;
	ops[16] = hlt;
}

void
loop(struct mstate* mstate, void* input, void* closure)
{
	while (!mstate->halt) {
		ops[mstate->mem[mstate->ip]](mstate, input, closure);
	}
}

