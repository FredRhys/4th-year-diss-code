#include <stdio.h>
#include <stdlib.h>

int is_even(int x) {
  return (x & 0b1) == 0b0;
}

int is_3mod4(int x) {
  return (x & 0b11) == 0b11;
}

int get2pwr(int* restrict S, int* restrict Q, int x) {
  register int i;
  for (i = 0; is_even(x); ++i) {
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
  register int i = 0;
  while (t != 1){
    t = mod_pow(t, 2, p);
    ++i;
  }
  return i;
}

// calculates the square root of n modulo p if it exists and returns affirmative. o/w returns negative
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
      i = find_i(t, p);
      b = mod_pow(c, _pow(2, M - i - 1), p);
      M = i;
      c = (b * b) % p;
      t = (t * b * b) % p;
      R = (R * b) % p;
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    return 0;
  }
  const int n = atoi(argv[1]), p = atoi(argv[2]);
  int r;
  if (mod_sqrt(&r, n, p))
    printf("%d\n", r);
  return 0;
}