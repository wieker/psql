#include <math.h>
#include "root.h"
#include "utils.h"

struct complex *
get_roots(int ord, struct complex* c)
{
	struct complex *roots = mem_alloc(ord * sizeof(*roots));
	int i = 0;
	
	float r = pow(c->r, 1 / (float) ord);
	for (i = 0; i < ord; i ++) {
		roots[i].r = r;
		roots[i].angle = (c->angle + i * 2 * 3.14) / (float) ord;
	}
	
	return roots;
}

struct complex *
tostd(struct complex *a)
{
	struct complex *r = mem_alloc(sizeof(*r));
	
	r->r = a->r * cosf(a->angle);
	r->angle = a->r * sinf(a->angle);
	
	return r;
}

struct complex *
fromstd(struct complex *a)
{
	struct complex *r = mem_alloc(sizeof(*r));
	
	r->r = powf(a->r * a->r + a->angle * a->angle, 1 / (float) 2);
	r->angle = asinf(a->angle / r->r);
	if (a->r < 0)
		r->angle += 3.14;
	
	return r;
}

struct complex *
cpmul(struct complex *a, struct complex *b)
{
	struct complex *r = mem_alloc(sizeof(*r));
	
	r->r = a->r * b->r;
	r->angle = a->angle + b->angle;
	
	return r;
}

struct complex *
cpdiv(struct complex *a, struct complex *b)
{
	struct complex *r = mem_alloc(sizeof(*r));
	
	r->r = a->r / b->r;
	r->angle = a->angle - b->angle;
	
	return r;
}

struct complex *
cpadd(struct complex *a, struct complex *b)
{
	struct complex *r = mem_alloc(sizeof(*r));
	struct complex *stdr = mem_alloc(sizeof(*r));
	struct complex *stda = tostd(a);
	struct complex *stdb = tostd(b);
	
	stdr->r = stda->r + stdb->r;
	stdr->angle = stda->angle + stdb->angle;
	
	r = fromstd(stdr);
	
	mem_free(stda);
	mem_free(stdb);
	mem_free(stdr);
	
	return r;
}

struct complex *
cpsub(struct complex *a, struct complex *b)
{
	struct complex *r = mem_alloc(sizeof(*r));
	struct complex *stdr = mem_alloc(sizeof(*r));
	struct complex *stda = tostd(a);
	struct complex *stdb = tostd(b);
	
	stdr->r = stda->r - stdb->r;
	stdr->angle = stda->angle - stdb->angle;
	
	r = fromstd(stdr);
	
	mem_free(stda);
	mem_free(stdb);
	mem_free(stdr);
	
	return r;
}

