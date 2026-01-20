#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define ALPHA 0.259921049895
//need to find underlying issue
#define LIM21_t (int32_t)2097151
#define CBLIM21_t 9223358842721533951

// functions from factor64
int initfactor64(const char*);
int factor64(uint64_t*, int*, uint64_t);
int isprime64(uint64_t);

// linked list structure used for storing divisors
typedef struct intentry{
	uint32_t x;
	struct intentry* next;
}intentry;

// absolute function for long integers
static inline uint64_t absl(int64_t x) {
	return x>=0 ? x : -x;
}

// squaring function
static inline uint64_t sq(uint32_t x) {
	return (int64_t)x * x;
}

// cubing function; input integers must fit into 21 bits.
static inline int64_t cb(int32_t x){
	return (int64_t)x * x * x;
}

// returns sign of input integer
int8_t sgn(int64_t x) {
	if (x > 0)
		return 1;
	else if (x == 0)
		return 0;
	else // x < 0
		return -1;
}

// returns the discriminant of the quadratic that gives potential values for x and y
int64_t calcdisc(uint32_t d, int64_t k, int32_t z) {
	return 4 * (d + absl(6*k - cb(z) + z)) - cb(d);
}

// calculates the square root required for calculating potential values for x and y
// returns affirmative if no issues
uint8_t calcsqrt(uint64_t* restrict _sqrt, uint32_t d, int64_t k, int32_t z) {
	int64_t disc = calcdisc(d, k, z);
	const uint64_t _3d = 3 * d;
	uint32_t sqrt;
	if (disc < 0) // i.e. no real square root exists
		return 0;
	if (disc % _3d != 0) // i.e. the discriminant is not divisible by 3d
		return 0;
	const uint64_t sqrtand = disc/_3d;
	sqrt = sqrtl(sqrtand);
	if (sq(sqrt) == sqrtand) { // i.e., sqrtand is a square number
		*_sqrt = sqrt;
		return 1;
	}
	return 0;
}

// calculates values for x and y
// returns affirmative if they are satisfactory
uint8_t calcxy(int64_t* restrict x, int64_t* restrict y, uint32_t d, int64_t k, int32_t z) {
	uint64_t sqrt;
	if (!calcsqrt(&sqrt, d, k, z)) // i.e., no satisfactory square root exists
		return 0;
	const int8_t SGN = sgn(6*k - cb(z) + z);
	*x = SGN * (d + sqrt);
	*y = SGN * (d - sqrt);
	if ((*x & 0b1) == 0b1 || (*y & 0b1) == 0b1) // i.e., they're not both even
	 	return 0;
	*x >>= 1;
	*y >>= 1;
	return 1;
}

// need to improve bound
// returns an upper bound on the divisors in k
static inline uint64_t get_divbound(int32_t z, int64_t k) {
  if (z == 0)
     return 0;
  else if (abs(z) >= sqrtl(6*k)) {
    return ALPHA * z + 1;
	}
  else
		return 3*k;

}

// returns the first entry in a linked list of the divisors of k
// potential optimisation: sort the list whilst inserting elements
// i am scared to touch it
intentry* get_divisors(int32_t z, uint64_t x, int64_t k) {
	int l, e[15];
  uint64_t p[15];
	uint32_t nextint;
	const uint64_t DIVBOUND = get_divbound(z, k);

	// creates and initializes the first entry
  intentry* first = malloc(sizeof(intentry)), * cur, * start, * next;
	first->x = 1;
	first->next = NULL;
	if (x==1)
		return first;
	// store the prime divisors in p, their multiplicity in e, and the number of unique prime divisors in l
  l = factor64(p, e, x);
	// iterate through all unique prime divisors
	for (int i = 0; i < l; ++i)
		// iterate as per multiplicity of the divisor.
		while (--e[i] >= 0) {
			// this is equivalent to S <- S append S * d
			start = first;
			// iterates through each existing entry in the list
			while (start != NULL) {
				if (p[i] == start->x && e[i] == 0)
					break;
				nextint = p[i] * start->x; // set the next potential entry
				cur = start; // set the current entry
				start = start->next; // increment the start entry ready for the next iteration
				if (nextint >= LIM21_t || nextint >= DIVBOUND) { // skip this iteration if the next potential entry is too large
					goto cont;
				}
				while(cur->next != NULL) { // check if the linked list contains the next potential entry already.
					if (cur->x == nextint) // skip this start-iteration if the next potential entry is already present
						goto cont;
					cur = cur->next;
				}
				// append the next entry as it is small enough and not already present
				if (cur->x != nextint) {
					next = malloc(sizeof(intentry));
					next->x = nextint;
					next->next = NULL;
					cur->next = next;
				}
				cont:; // goto label to skip the iteration
			}
		}
	return first;
}

// sets x to 0 if x=1,2 as these all give 0 in the tetrahedral number formula
static inline int64_t canonicize(int64_t x) {
	return x == 1 || x == 2 ? 0 : x;
}

// the basic algorithm
// returns affirmative if we have found a representation for k with z as the third term
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
	for (int64_t k = kMIN; k <= kMAX; ++k) {
		if (!zloop(k, zLIM/*, e*/))
			fprintf(f, "%ld\n", k);
	}
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
	if (zLIM > LIM21_t)
		return -1;
	snprintf(hardname, 100, "hards/hard%ld.txt", kMAX);
	//snprintf(repname, 100, "reps/rep%ld.txt", kMAX);
	FILE* f = fopen(hardname, "w");
	//FILE* e = fopen(repname, "w");
	kloop(kMIN, kMAX, zLIM, f/*, e*/);
	fclose(f);
	//fclose(e);
	return 0;
}