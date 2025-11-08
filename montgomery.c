#include <stdio.h>
#include <stdlib.h>

typedef struct quotrem {
	int q;
	int r;
	struct quotrem* prev;
} quotrem;

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

// creates new quotrem and returns its pointer.
quotrem* save_quotrem(quotrem* end, int q, int r) {
	quotrem* new = malloc(sizeof(quotrem));
	new->q = q;
	new->r = r;
	new->prev = end;
	return new;
}

// creates all the quotrems and returns the endpoint.
quotrem* calc_quotrems(quotrem* end, int a, int b) {
	register int q = 0;
	register int bdec_c= 0;
	while (a != b) {
		if (a > b) {
			a -= b;
			++q;
			bdec_c = 0;
		}
		else {
			if (bdec_c == 0) {
				end = save_quotrem(end, q, a);
				q = 0;
			}
			else if (bdec_c > 1)
				++q;
			++bdec_c;
			b -= a;
		}
	}
	return save_quotrem(end, q, a);
}

int find_q(int p, int k) {
	quotrem* end = NULL, * temp;
	end = calc_quotrems(end, 0b1<<k, p);
	while (end != NULL) {
		printf("%d, %d\n", end->q, end->r);
		temp = end;
		end = end->prev;
		free(temp);
	}
	return 0;
}

int modmult(int a, int b, int p) {
	return a + b + p;
}

int main(void) {
	find_q(13, 4);
	return 0;
}