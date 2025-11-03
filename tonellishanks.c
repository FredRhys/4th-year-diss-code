#include <stdio.h>
#include <stdlib.h>

int get2pwr(int* restrict S, int* restrict Q, int x) {
  register int i;
  for (i = 0; (x & 0b1) == 1; ++i) {
    x = x >> 1;
  }
  *S = i;
  *Q = x;
  return 0;
}

int pow(int x, int n) {
  return n == 0 ? 1 : x * pow(x, n-1);
}

int mainloop(int z, int Q, int S, int n) {
  int M = S, c = pow(z, Q), t = pow(n, Q), R = pow(n, (Q + 1) >> 1);
  return 0;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    return 0;
  }
  const int p = atoi(argv[1]), n = atoi(argv[2]);
  int S, Q, z = 0;
  get2pwr(&S, &Q, p-1);
  mainloop(z, Q, S, n);
  return 0;
}