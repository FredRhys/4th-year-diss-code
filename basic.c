#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// functions from factor64
int initfactor64(const char*);
int factor64(uint64_t*, int*, uint64_t);
int isprime64(uint64_t);

static inline uint64_t _abs(int64_t x) {
	return x >= 0 ? x : -x;
}

static inline __int128_t cb(int64_t x){
	return (__int128_t)(x * (__int128_t)(x * x));
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

uint8_t calcsqrt(uint64_t* restrict _sqrt, uint64_t d, int64_t k, int32_t z) {
	int64_t disc = calcdisc(d, k, z);
	if (disc < 0)
		return 0;
	*_sqrt = sqrt(disc / (3 * d));
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


int basic(int64_t z, int64_t k/*, FILE* f*/) {
	uint64_t _6kmcbzpz = _abs(6*k - cb(z) + z), d, p[15], j = 0, l;
	int64_t x = 0, y = 0;
	int e[15];
	l = (uint64_t)factor64(p, e, _6kmcbzpz);

  uint64_t sum = 0;
  for (uint64_t i = 0; i < l; ++i)
    sum += e[i];
  int raw_factors[sum];

  //create array with a prime power factor in each position
  for (uint64_t i = 0; i < l; ++i) {
    while (--e[i] >= 0) {
      raw_factors[j++] = p[i];
    }
  }

	if (_6kmcbzpz == 0)
		goto success;
	if (_6kmcbzpz == 1) {
		if (!check_d(&x, &y, z-1, 1, k))
      return 0;
    goto success;
	}

	for (uint64_t i = 0; i < (uint64_t)0b1<<sum; ++i) {
		d = 1;
		for (uint64_t j = 0; j < sum; ++j)
      if ((0b1<<j) & i)
			  d *= raw_factors[j];
		if (!check_d(&x, &y, z-1, d, k))
			continue;
		success: //fprintf(f, "%ld: %ld %ld %ld\n", k, x+1, y+1, z);
		return 1;
	}
	return 0;
}

uint8_t zloop(int64_t k/*, FILE* f*/) {
	if (basic(0, k/*, f*/))
		return 1; 
	for (int64_t z = 1; z <= sqrt(6*k); ++z) {
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
	char repname[100];
	const int64_t kMIN = atoi(argv[1]), kMAX = atoi(argv[2]);
	snprintf(hardname, 100, "hards/hard%ld.txt", kMAX);
	snprintf(repname, 100, "reps/rep%ld.txt", kMAX);
	FILE* f = fopen(hardname, "w");
	//FILE* e = fopen(repname, "w");
	kloop(kMIN, kMAX, f/*, e*/);
	fclose(f);
	//fclose(e);
	return 0;
}