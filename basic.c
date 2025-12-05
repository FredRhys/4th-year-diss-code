#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// functions from factor64
int initfactor64(const char*);
int factor64(uint64_t*, int*, uint64_t);
int isprime64(uint64_t);

uint64_t _abs(int64_t x) {
	return x >= 0 ? x : -x;
}

__int128_t cb(int64_t x){
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
	if (!((*x & 0b1) == 0b0 && (*y & 0b1) == 0b0))
		return 0;
	*x >>= 1;
	*y >>= 1;
	return 1;
}

uint8_t checkxy(int64_t x, int64_t y, int64_t z, int64_t k) {
	return (cb(x) - x + cb(y) - y + cb(z) - z == 6*k);
}

uint8_t check_d(int64_t* restrict x, int64_t* restrict y, int64_t _z, int d, int64_t k) {
	if (!calcxy(x, y, d, k, _z))
		return 0;
	if (!checkxy(*x, *y, _z, k))
		return 0;
	return 1;
}


int basic(int64_t z, int64_t k) {
	uint64_t kmcbzpz = _abs(k - cb(z) + z), d = 1, p[15];
	int64_t x, y;
	int _k, e[15];
	if (kmcbzpz == 0)
		return 1;
	if (kmcbzpz == 1) {
		return check_d(&x, &y, z-1, 1, k);
	}
	_k = factor64(p, e, kmcbzpz);
	for (int64_t i = 0; i < 0b1<<_k; ++i) {
		d = 1;
		for (uint8_t j = 0; j < _k; ++j)
			d *= ((0b1<<j) & i) ? p[j] : 1;
		if (!check_d(&x, &y, z-1, d, k))
			continue;
		//printf("(%ld)C3 + (%ld)C3 + (%ld)C3 = %ld\n", x+1, y+1, z, k);
		return 1;
	}
	return 0;
}

uint8_t zloop(int64_t k) {
	if (basic(0, k))
		return 1;
	for (int64_t z = 1; z <= sqrt(6*k); ++z) {
		if (basic(z, k))
			return 1;
		if (basic(-z, k))
			return 1;
	}
	return 0;
}

void kloop(int64_t kLIM, FILE* f) {
	for (int64_t k = 1; k <= kLIM; ++k)
		if (!zloop(k))
			fprintf(f, "%ld\n", k);
}

int main(int argc, char** argv) {
	if (argc != 2)
		return -1;
	if (initfactor64("factor.bin") < 0) {
		fprintf(stderr, "Cannot read factor data\n");
		return -1;
	}
	const int64_t k_LIM = atoi(argv[1]);
	FILE* f = fopen("hard.txt", "w");
	kloop(k_LIM, f);
	fclose(f);
	return 0;
}