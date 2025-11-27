#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t _abs(int64_t x) {
	return x >= 0 ? x : -x;
}

// might change to a 96 bit type
__int128_t cb(int64_t x){
	return (__int128_t)x * (__int128_t)x * (__int128_t)x;
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

int basic(int64_t z, int64_t k) {
	const uint64_t kmcbzpz = _abs(k - cb(z) + z);
	int64_t x, y;
	for (uint64_t d = 1; d <= kmcbzpz; ++d) {
		if (kmcbzpz % d != 0)
			continue;
		if (!calcxy(&x, &y, d, k, z-1))
			continue;
		if (!checkxy(x, y, z-1, k))
			continue;
		//printf("(%ld)C3 + (%ld)C3 + (%d)C3 = %ld\n", x+1, y+1, z, k);
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

void kloop(int64_t kLIM) {
	/*const uint16_t BUFFER_LIM = 100;
	int64_t buffer[BUFFER_LIM];
	uint64_t end = 0;
	FILE* f = fopen("hardks.txt", "wb");*/
	const uint16_t cLIM = 40;
	uint16_t c = 0;
	for (int64_t k = 1; k <= kLIM; ++k) {
		if (!zloop(k)) {
			printf("%ld,", k);
			if (++c == cLIM) {
				putchar('\n');
				c = 0;
			}
		}
		/*if (end == BUFFER_LIM) {
			fwrite(buffer, 64, BUFFER_LIM, f);
			end = 0;	
		}*/
	}
	/*fwrite(buffer, 64, end, f);
	fclose(f);*/
	putchar('\n');
}

int main(int argc, char** argv) {
	if (argc != 2)
		return -1;
	const int64_t k_LIM = atoi(argv[1]);
	kloop(k_LIM);
	return 0;
}