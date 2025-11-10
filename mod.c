#include <stdio.h>

int modpwr2(int x, int k) {
	int mask = (0b1 << k) - 1;
	return x & mask;
}

int is_even(int x) {
  return modpwr2(x, 1) == 0b0;
}

int is_3mod4(int x) {
  return modpwr2(x, 2) == 0b11;
}

void get2pwr(int* restrict S, int* restrict Q, int x) {
  register int i;
  for (i = 0; is_even(x); ++i) {
    x = x >> 1;
  }
  *S = i;
  *Q = x;
}

int _pow(int x, int n) {
  return n == 0 ? 1 : x * _pow(x, n - 1);
}

int mod_pow(int x, int n, int p) {
  while (n > 1) {
    x = x*x;
    if (x > p)
      x = x % p;
    --n;
  }
  return x;
}

int legendre(int a, int p) {
  return mod_pow(a, (p-1)>>1, p);
}

int find_z(int p) {
  int z;
  for (z = 2; z < p; ++z)
    if (legendre(z, p) != 1)
      return z;
  return 0;
}

int find_i(int t, int p, int M) {
  register int i = 0;
  while (t != 1 && i < M){
    t = mod_pow(t, 2, p);
    ++i;
  }
  return i;
}

int mod_sqrt(int* restrict r, int n, int p) {
  if (legendre(n, p) == p - 1)
    return 0;
  else if (is_3mod4(p)) {
    *r = mod_pow(n, (p+1)>>2, p);
    return 1;
  }
  int M, Q, z = find_z(p);
  get2pwr(&M, &Q, p-1);
  int c =  mod_pow(z, Q, p);
  int t = mod_pow(n, Q, p);
  int R = mod_pow(n, (Q + 1) >> 1, p);
  //printf("%d\t%d\t%d\t%d\t%d\t%d\n", M, Q, z, c, t, R);
  int i, b;
  while (t >= 0) {
    if (t == 0) {
      *r = 0;
      return 1;
    }
    else if (t == 1) {
      *r = R;
      return 1;
    }
    else {
      i = find_i(t, p, M);
      b = mod_pow(c, mod_pow(2, M-i-1, p-1), p);
      M = i;
      c = (b * b) % p;
      t = (t * b * b) % p;
      R = (R * b) % p;
    }
  }
  return 0;
}

int find_k(int p) {
	register int R = 1;
	register int k = 0;
	for (k = 0; R <= p; ++k)
		R = R << 1;
	return k;
}

void par_ass(int* restrict old_x, int* restrict x, int q) {
	int t = *x;
	*x = *old_x - q * t;
	*old_x = t;
}

// returns s s.t. as === 1 (mod b)
int mod_inv(int a, int b) {
	register int q;
	int old_r, r, old_s, s;
  if (a >= b) {
    old_r = a, r = b, old_s = 1, s = 0;
  }
  else {
    old_r = b, r = a, old_s = 0, s = 1;
  }
	while (r != 0) {
		q = old_r/r; // would like to replace
		par_ass(&old_r, &r, q);
		par_ass(&old_s, &s, q);
	}
	while (old_s < 0)
		old_s += b;
	return old_s;
}

void get3pwr(int* restrict k, int* restrict t, int* restrict e, int p) {
  int q = p - 1;
  while (q % 3 == 0) {
    ++(*k);
    q = q / 3;
  }
  // q = p-1/(3^k) by the end.
  *e = q % 3 == 1 ? -1 : 1;
  *t = (q + *e) / 3;
}

void find_u(int* restrict u, int* restrict r, int k, int t, int e, int p) {
 while (*r != 1) {
    *r = mod_pow(*u, mod_pow(3, k-1, p-1) * (3 * t - e), p);
    ++(*u);
  }
}

int find_m(int b, int p) {
  register int i = 0;
  printf("Here now!\n");
  printf("%d\n", b);
  while (b != 1) {
    b = (b * b * b) % p;
    ++i;
  }
  return i;
}

// problem here for p = 29
int mod_cbrt(int* restrict res, int a, int p) {
	if (p % 3 == 2) {// barret reduction
		*res = mod_pow(a, ((2*p-1)/3)%(p-1), p);
		return 1;
	}
  // p === 1 mod 3
  int k = 0, t, e, u = 2, r = 0;
  get3pwr(&k, &t, &e, p);
  find_u(&u, &r, k, t, e, p);
  int x = mod_pow(a, t, p);
  int b = mod_pow(a, 3 * t - e, p);
  int z = mod_pow(u, 3 * t - e, p);
  int sigma = 1;
  int m, s, d, tau, f;
  while (b % p != 1) {
    m = find_m(b, p);
    s = b == r ? 1 : -1;
    d = mod_pow(z, mod_pow(3, k-m-1, p-1), p);
    tau = (sigma * s) % 3;
    f = mod_pow(d, tau, p);

    //Update
    z = mod_pow(f, 3, p);
    b = (b * z) % p;
    x = (x * f) % p;
    k = m;
    sigma = s;
    //printf("%d\t%d\t%d\t%d\t%d\n", m, s, d, tau, f);
  }
  if (e == 1)
    *res = x;
  else
    *res = mod_inv(x, p);
  return 1;
}

int main(void) {
	const int k = 1, p = 29;
	int r1, r2, r3, z;
	if (!mod_sqrt(&r1, 9 * k * k - mod_inv(27, p), p)) {
    printf("No square root.\n");
		return 0;
  }
	if (!mod_cbrt(&r2, 3 * k + r1, p)) {
    printf("No cube root (1).\n");
		return 0;
  }
  printf("%d\n", r2);
	if (!mod_cbrt(&r3, 3 * k - r1, p)) {
    printf("No cube root (2).\n");
		return 0;
  }
  printf("%d\n", r3);
	z = r2 + r3;
	if (z > p)
		z -= p;
	printf("z=%d\n", z);
	return 0;
}