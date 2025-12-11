#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// functions from factor64
int initfactor64(const char*);
int factor64(uint64_t*, int*, uint64_t);
int isprime64(uint64_t);

typedef struct intentry{
	uint64_t x;
	struct intentry* next;
}intentry;

static inline uint64_t _abs(int64_t x) {
	return x >= 0 ? x : -x;
}

static inline __int128_t cb(int64_t x){
	return (__int128_t)x * x * x;
}

int8_t sgn(int64_t x) {
	if (x > 0)
		return 1;
	else if (x == 0)
		return 0;
	else // x < 0
		return -1;
}

int64_t calcdisc(uint64_t d, int64_t k, int64_t z) {
	return 4 * (d + _abs(6*k - cb(z) + z)) - cb(d);
}

uint8_t calcsqrt(uint64_t* restrict _sqrt, uint64_t d, int64_t k, int64_t z) {
	int64_t disc = calcdisc(d, k, z);
	if (disc < 0)
		return 0;
	*_sqrt = sqrtl(disc / (3 * d));
	return 1;
}

uint8_t calcxy(int64_t* restrict x, int64_t* restrict y, uint64_t d, int64_t k, int64_t z) {
	uint64_t sqrt;
	if (!calcsqrt(&sqrt, d, k, z))
		return 0;
	const int8_t SGN = sgn(6*k - cb(z) + z);
	*x = SGN * (d + sqrt);
	*y = SGN * (d - sqrt);
	if ((*x & 0b1) == 0b1 || (*y & 0b1) == 0b1)
	 	return 0;
	*x >>= 1;
	*y >>= 1;
	return 1;
}

static inline uint8_t checkxy(int64_t x, int64_t y, int64_t z, int64_t k) {
 	return (cb(x) - x + cb(y) - y + cb(z) - z == 6*k);
}

uint8_t check_d(int64_t* restrict x, int64_t* restrict y, int64_t z, int d, int64_t k) {
	if (!calcxy(x, y, d, k, z))
		return 0;
	if (!checkxy(*x, *y, z, k))
	 	return 0;
	return 1;
}

static inline uint64_t get_divbound(int64_t z, int64_t k) {
  switch (z) {
    case 0:
      return 3*k; // if y is nonzero
    default:
      return sqrtl(6*k);
  }
}

intentry* get_divisors(int64_t z, uint64_t x, int64_t k) {
	int l, e[15];
  uint64_t p[15], nextint;
	const uint64_t DIVBOUND = get_divbound(z, k);

  intentry* first = malloc(sizeof(intentry)), * cur, * start, * next;
	first->x = 1;
	first->next = NULL;
	if (x==1)
		return first;
  l = factor64(p, e, x);
	for (int i = 0; i < l; ++i)
		while (--e[i] >= 0) {
			start = first;
			while (start != NULL) {
				nextint = p[i] * start->x;
				if (nextint > DIVBOUND)
					goto end;
				cur = start;
				start = start->next;
				while(cur->next != NULL) {
					if (cur->x == nextint)
						goto cont;
					cur = cur->next;
				}
				// this only runs if cur is the last item in the linked list
				if (cur->x != nextint) {
					next = malloc(sizeof(intentry));
					next->x = nextint;
					next->next = NULL;
					cur->next = next;
				}
				cont: continue;
			}
		}

	end: return first;
}

int basic(int64_t z, int64_t k/*, FILE* f*/) {
	uint64_t _6kmcbzpz = _abs(6*k - cb(z) + z);
	int64_t x = 0, y = 0;
	uint8_t r = 0;

	if (_6kmcbzpz == 0)
		return 1;
	else if (_6kmcbzpz == 1) {
		if (!check_d(&x, &y, z-1, 1, k))
      return 0;
   return 1;
	}
	intentry* head = get_divisors(z, _6kmcbzpz, k), * temp;
	while (head != NULL) {
		if (check_d(&x, &y, z-1, head->x, k) && !r) {
			r = 1;
			//printf("%ld %ld %ld %ld\n", x+1, y+1, z, k);
		}
		temp = head->next;
		free(head);
		head = temp;
	}
	return r;
}

uint8_t zloop(int64_t k/*, FILE* f*/) {
	if (basic(0, k/*, f*/))
		return 1;
	const int zLIM = (int64_t)(sqrtl(6*k));
	for (int64_t z = 1; z <= zLIM; ++z) {
		if (basic(z, k/*, f*/))
			return 1;
		if (basic(-z, k/*, f*/))
			return 1;
	}
	return 0;
}

void kloop(int64_t kMIN, int64_t kMAX, FILE* f/*, FILE* e*/) {
	for (int64_t k = kMIN; k <= kMAX; ++k)
		if (!zloop(k/*, e*/))
			fprintf(f, "%ld\n", k);
}

int main(int argc, char** argv) {
	if (argc != 3)
		return -1;
	if (initfactor64("factor.bin") < 0) {
		fprintf(stderr, "Cannot read factor data\n");
		return -1;
	}
	char hardname[100];
	//char repname[100];
	const int64_t kMIN = atoi(argv[1]), kMAX = atoi(argv[2]);
	snprintf(hardname, 100, "hards/hard%ld.txt", kMAX);
	//snprintf(repname, 100, "reps/rep%ld.txt", kMAX);
	FILE* f = fopen(hardname, "w");
	//FILE* e = fopen(repname, "w");
	kloop(kMIN, kMAX, f/*, e*/);
	fclose(f);
	//fclose(e);
	return 0;
}