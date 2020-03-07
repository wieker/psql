#ifndef POW_H
#define POW_H

struct complex {
	float r;
	float angle;
	
	float dev;
};

struct poly_closure {
	
};

struct poly_state {
	int N;
	struct complex* roots;
};

struct complex *get_roots(int ord, struct complex* c);

struct complex *tostd(struct complex *a);

struct complex *fromstd(struct complex *a);

struct complex *cpmul(struct complex *a, struct complex *b);

struct complex *cpdiv(struct complex *a, struct complex *b);

struct complex *cpadd(struct complex *a, struct complex *b);

struct complex *cpsub(struct complex *a, struct complex *b);

typedef void (*draw_func)(float x, float y);

#endif

