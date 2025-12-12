#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define ALPHA 0.259921049895
//need to find underlying issue
#define LIM21_t 2000000
#define CBLIM21_t 8000000000000000000

// functions from factor64
int initfactor64(const char*);
int factor64(uint64_t*, int*, uint64_t);
int isprime64(uint64_t);

typedef struct intentry{
	uint64_t x;
	struct intentry* next;
}intentry;

static inline uint64_t absl(int64_t x) {
	return x>=0 ? x : -x;
}

static inline uint64_t sq(uint32_t x) {
	return (int64_t)x * x;
}

// need to find underlying issue
static inline int64_t cb(int32_t x){
	return x > LIM21_t ? (int64_t)CBLIM21_t : (int64_t)x * x * x;
}

int8_t sgn(int64_t x) {
	if (x > 0)
		return 1;
	else if (x == 0)
		return 0;
	else // x < 0
		return -1;
}

int64_t calcdisc(uint64_t d, int64_t k, int32_t z) {
	return 4 * (d + absl(6*k - cb(z) + z)) - cb(d);
}

uint8_t calcsqrt(uint64_t* restrict _sqrt, uint64_t d, int64_t k, int32_t z) {
	int64_t disc = calcdisc(d, k, z);
	const uint64_t _3d = 3 * d;
	uint32_t sqrt;
	if (disc < 0)
		return 0;
	if (disc % _3d != 0)
		return 0;
	const uint64_t sqrtand = disc/_3d;
	sqrt = sqrtl(sqrtand);
	if (sq(sqrt) == sqrtand) {
		*_sqrt = sqrt;
		return 1;
	}
	return 0;
}

uint8_t calcxy(int64_t* restrict x, int64_t* restrict y, uint64_t d, int64_t k, int32_t z) {
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

static inline uint64_t get_divbound(int32_t z, int64_t k) {
  if (z == 0)
     return 3*k;
  else if (abs(z) >= sqrtl(6*k)) {
    return (abs(z) >> 2) + 1;
	}
  else
		return 3*k>LIM21_t ? LIM21_t : 3*k;

}

//getting some unwanted divisors
intentry* get_divisors(int32_t z, uint64_t x, int64_t k) {
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
				if (p[i] == start->x && e[i] == 0)
					break;
				nextint = p[i] * start->x;
				cur = start;
				start = start->next;
				if (nextint >= DIVBOUND) {
					goto cont;
				}
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
				cont:;
			}
		}
	return first;
}

static inline int64_t canonicize(int64_t x) {
	return x == 0 || x == 1 || x == 2 ? 0 : x;
}

int basic(int32_t z, int64_t k/*, FILE* f*/) {
	uint64_t _6kmcbzpz = absl(6*k - cb(z) + z);
	int64_t x = 0, y = 0;
	uint8_t r = 0;

	if (_6kmcbzpz == 0)
		return 1;
	intentry* head = get_divisors(z, _6kmcbzpz, k), * temp;
	while (head != NULL) {
		if (calcxy(&x, &y, head->x, k, z) && !r) {
			//fprintf(f, "%ld = %ldC3 + %ldC3 + %ldC3\n", k, canonicize(x+1), canonicize(y+1), canonicize(z+1));
		 	r = 1;
		}
		temp = head->next;
		free(head);
		head = temp;
	}
	return r;
}

uint8_t zloop(int64_t k, int32_t zLIM/*, FILE* f*/) {
	if (basic(0, k/*, f*/))
		return 1;
	for (int32_t z = 1; z <= zLIM; ++z) {
		if (basic(z, k/*, f*/))
			return 1;
		if (basic(-z, k/*, f*/))
			return 1;
	}
	return 0;
}

void kloop(int64_t kMIN, int64_t kMAX, int32_t zLIM, FILE* f/*, FILE* e*/) {
	for (int64_t k = kMIN; k <= kMAX; ++k)
		if (!zloop(k, zLIM/*, e*/))
			fprintf(f, "%ld\n", k);
}

int main(int argc, char** argv) {
	if (argc != 4)
		return -1;
	if (initfactor64("factor.bin") < 0) {
		fprintf(stderr, "Cannot read factor data\n");
		return -1;
	}
	char hardname[100];
	//char repname[100];
	const int64_t kMIN = atoi(argv[1]), kMAX = atoi(argv[2]);
	const int32_t zLIM = atoi(argv[3]);
	snprintf(hardname, 100, "hards/hard%ld.txt", kMAX);
	//snprintf(repname, 100, "reps/rep%ld.txt", kMAX);
	FILE* f = fopen(hardname, "w");
	//FILE* e = fopen(repname, "w");
	kloop(kMIN, kMAX, zLIM, f/*, e*/);
	fclose(f);
	//fclose(e);
	return 0;
}