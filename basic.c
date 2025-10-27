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
	*_sqrt = sqrt(disc / 3.0);
	return 1;
}

int calcxy(int* restrict x, int* restrict y, int d, int k, int z) {
	const float hlfsgnkz = 0.5 * sgn(k - cb(z) + z);
	int sqrt;
	if (!calcsqrt(&sqrt, d, k, z))
		return 0;
	*x = hlfsgnkz * (d + sqrt);
	*y = hlfsgnkz * (d - sqrt);
	return 1;
}

int checkxy(int x, int y, int z, int k) {
	return (cb(x) - x + cb(y) - y + cb(z) - z == 6*k) && abs(x) >= 2 && abs(y) >= 2;
}

int _6xC3(int x) {
	return x * (x - 1) * (x - 2);
}

int checkxm1ym1(int x, int y, int z, int k) {
	return (_6xC3(x) + _6xC3(y) + _6xC3(z) == 6*k);
}

int basic(int z, int k) {
	const int kmcbzpm = abs(k - cb(z) + z);
	int x, y;
	for (int d = 1; d <= kmcbzpm; ++d) {
		if (kmcbzpm % d != 0)
			continue;
		if (!calcxy(&x, &y, d, k, z))
			continue;
		if (!checkxy(x, y, z, k))
			continue;
		/*
		if(!checkxy(x+1, y+1, z+1, k))
			continue; */
		printf("%dC3 + %dC3 + %dC3 = %d; done w/ divisor %d\n", x, y, z, k, d);
	}
	return 0;
}

int zloop(int k) {
	for (int z = -ZLIM; z <= ZLIM; ++z) {
		if (-3 <= z && z <= 3)
			continue;
		basic(z, k);
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