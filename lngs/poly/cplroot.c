#include <math.h>
#include "root.h"
#include "utils.h"
#include <stdio.h>

struct poly {
	int N;
	struct complex a;
	struct poly* next;
};

struct poly *
poly_derive(struct poly* poly)
{
	struct poly* result = mem_alloc(sizeof(*result)),* curr;
	curr = result;
	while (poly->next) {
		curr->N = poly->N - 1;
		curr->a = poly->a;
		curr->a.r *= poly->N;
		if (poly->next->next != NULL) {
			curr->next = mem_alloc(sizeof(*result));
		} else {
			curr->next = NULL;
		}
		curr = curr->next;
		poly = poly->next;
	}
	return result;
}

struct complex
poly_eval(struct poly* poly, struct complex x)
{
	struct complex result;
	result.angle = 0;
	result.r = 0;
	int i = 0;
	while (poly) {
		i = 0;
		struct complex work;
		work.angle = 0;
		work.r = 1;
		while (i < poly->N) {
			work =* cpmul(&work, &x);
			i ++;
		}
		work =* cpmul(&work, &poly->a);
		result =* cpadd(&work, &result);
		poly = poly->next;
	}
	return result;
}

struct complex
poly_next(struct poly* poly, struct poly* der, struct complex z)
{
	struct complex r1 = poly_eval(poly, z), r2 = poly_eval(der, z),* r3;
	printf("VAL %f %f\n", r1.r * cosf(r1.angle),
			r1.r * sinf(r1.angle));
	printf("DER %f %f\n", r2.r * cosf(r2.angle),
			r2.r * sinf(r2.angle));
	r3 = cpdiv(&r1, &r2);
	printf("DIF %f %f\n", r3->r * cosf(r3->angle),
			r3->r * sinf(r3->angle));
	return *cpsub(&z, r3);
}

struct complex
poly_iter(struct poly* poly, struct complex z0, draw_func draw)
{
	struct poly* der = poly_derive(poly);
	float delta;
	float eps = 0.5;
	struct complex z1;
	do {
		z1 = poly_next(poly, der, z0);
		struct complex val = poly_eval(poly, z1);
		delta = val.r;
		z0 = z1;
		draw(z0.r * cosf(z0.angle),
				z0.r * sinf(z0.angle));
		printf("%f %f\n", z0.r * cosf(z0.angle),
				z0.r * sinf(z0.angle));
	} while (delta > eps);
	return z0;
}

void
show_path(draw_func draw)
{
	struct complex c2 = {2, 0};
	struct complex c1 = {1, 0};
	struct poly A0 = {0, c2, NULL};
	struct poly A2 = {2, c1, &A0};
	struct complex z;
	z.r = 5;
	z.angle = 1.5;
	z = poly_iter(&A2, z, draw);
	draw(z.r * cosf(z.angle),
				z.r * sinf(z.angle));
}
