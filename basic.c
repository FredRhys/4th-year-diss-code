#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const int ZLIM = 10;

int abs(int x) {
	return x >= 0 ? x : -x;
}

int cb(int x){
	return x * x * x;
}

int sgn(int x) {
	if (x > 0)
		return 1;
	else if (x == 0)
		return 0;
	else // x < 0
		return -1;
}

int calcdisc(int d, int k, int z) {
	return (4*d + 4*abs(6*k - cb(z) + z)) - cb(d);
}

int calcsqrt(int* restrict _sqrt, int d, int k, int z) {
	int disc = calcdisc(d, k, z);
	if (disc < 0)
		return 0;
	*_sqrt = sqrt(disc / (3.0 * d));
	return 1;
}

int calcxy(int* restrict x, int* restrict y, int d, int k, int z) {
	int sqrt;
	if (!calcsqrt(&sqrt, d, k, z))
		return 0;
	const float hlfsgnkz = 0.5 * sgn(k - cb(z) + z);
	*x = hlfsgnkz * (d + sqrt);
	*y = hlfsgnkz * (d - sqrt);
	return 1;
}

int checkxy(int x, int y, int z, int k) {
	return (cb(x) - x + cb(y) - y + cb(z) - z == 6*k);
}

int _6xC3(int x) {
	return x * (x - 1) * (x - 2);
}

int basic(int z, int k) {
	const int kmcbzpz = abs(k - cb(z) + z);
	int x, y;
	for (int d = 1; d <= kmcbzpz; ++d) {
		if (kmcbzpz % d != 0)
			continue;
		if (!calcxy(&x, &y, d, k, z-1))
			continue;
		if (!checkxy(x, y, z-1, k))
			continue;
		printf("(%d)C3 + (%d)C3 + (%d)C3 = %d\n", x+1, y+1, z, k);
		return 1;
	}
	return 0;
}

int zloop(int k) {
	if (basic(0, k))
		return 1;
	for (int z = 1; z <= ZLIM; ++z) {
		if (basic(z, k))
			return 1;
		if (basic(-z, k))
			return 1;
	}
	return 0;
}

int main(int argc, char** argv) {
	if (argc != 2)
		return -1;
	const int k = atoi(argv[1]);
	zloop(k);
	return 0;
}