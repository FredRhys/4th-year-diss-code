#include <stdio.h>
#include <stdlib.h>

int is_even(int n) {
  return (n & 0b1) == 0b0;
}

int is_odd(int n) {
  return (n & 0b1) == 0b1;
}

int is_3mod4(int n) {
  return (n & 0b11) == 0b11;
}

int get2pwr(int* restrict S, int* restrict Q, int x) {
  register int i;
  for (i = 0; is_odd(x); ++i) {
    x = x >> 1;
  }
  *S = i;
  *Q = x;
  return 0;
}

int _pow(int x, int n) {
  return n == 0 ? 1 : x * _pow(x, n - 1);
}

int mod_pow(int x, int n, int p) {
  return (_pow(x, n)) % p;
}

int legendre(int a, int p) {
  return mod_pow(a, (p-1)>>1, p);
}

int find_z(int p) {
  int z;
  for (z = 2; z < p; ++z)
    if (legendre(z, p) == p - 1)
      return z;
  return 0;
}

int find_i(int t, int p) {
  t = mod_pow(t, 2, p);
  int run = 1, i = 1;
  while (run){
    ++i;
    t = mod_pow(t, 2, p);
    if (t == 1)
      run = 0;
  }
  return i;
}

int mainloop(int p, int n) {
  if (is_3mod4(n))
    return mod_pow(n, (p+1)>>2, p);
  int S, Q, z = find_z(p);
  get2pwr(&S, &Q, p-1);
  // p is 1 mod 4
  int M = S;
  int c =  mod_pow(z, Q, p);
  int t = mod_pow(n, Q, p);
  int R = mod_pow(n, (Q + 1) >> 1, p);
  int i, b;
  while (t >= 0) {
    if (t == 0)
      return 0;
    if (t == 1)
      return R;
    i = find_i(t, p);
    M = i;
    b = mod_pow(c, _pow(2, M - i - 1), p);
    c = (b * b) % p;
    t = (t * b * b) % p;
    R = (R * b) % p;
  }
  return 0;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    return 0;
  }
  const int n = atoi(argv[1]), p = atoi(argv[2]);
  if (legendre(n, p) == p - 1) {
    printf("%d is not a quadratic residue modulo %d.\n", n, p);
    return 0;
  }
  printf("%d\n", mainloop(p, n));
  return 0;
}