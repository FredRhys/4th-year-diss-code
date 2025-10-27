#include <stdio.h>
#include <math.h>

const int ZLIM = 100;

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
	const int sgnkz = sgn(k - cb(z) + z);
	int sqrt;
	if (calcsqrt(&sqrt, d, k, z)) {
		if (sgnkz >= 0) {
			*x = 0.5 * sgnkz * (d + sqrt);
			*y = 0.5 * sgnkz * (d - sqrt);
		} else {
			*x = 0.5 * sgnkz * (d - sqrt);
			*y = 0.5 * sgnkz * (d + sqrt);
		}
		return 1;
	}
	return 0;
}

int basic(int z, int k) {
	int x, y;
	for (int d = 1; d < k; ++d)
		if (k % d == 0)
			if (calcxy(&x, &y, d, k, z))
				if (d == abs(x + y)) {
					printf("%dC3 + %dC3 + %dC3 = %d\n", x, y, z, k);
					return 1;
				}
	return 0;
}

int zloop(int k) {
	for (int z = -ZLIM; z <= ZLIM; ++z)
		if (basic(z, k))
			return 1;
	return 0;
}

int main(int argc, char** argv) {
	if (argc != 2)
		return -1;
	const int k = argv[1];
	zloop(k);
	return 0;
}