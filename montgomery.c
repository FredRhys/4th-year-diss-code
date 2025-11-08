#include <stdio.h>
#include <stdlib.h>

int modpwr2(int x, int k) {
	int mask = (0b1 << k) - 1;
	return x & mask;
}

int find_k(int p) {
	register int R = 1;
	register int k = 0;
	for (k = 0; R <= p; ++k)
		R = R << 1;
	return k;
}

void par_ass(int* old_x, int* x, int q) {
	int t = *x;
	*x = *old_x - q * t;
	*old_x = t;
}

// returns s s.t. as = 1 (mod b)
int modinv(int a, int b) {
	register int q;
	int old_r = a, r = b, old_s = 1, s = 0;
	while (r != 0) {
		q = old_r / r;
		par_ass(&old_r, &r, q);
		par_ass(&old_s, &s, q);
	}
	while (old_s < 0)
		old_s += b;
	return old_s;
}

int modmult(int a, int b, int p) {
	return a + b + p;
}

int main(void) {
	return 0;
}