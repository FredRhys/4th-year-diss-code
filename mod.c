#include <stdint.h>
#include <stdio.h>

// reduces a modulo m;
static inline uint64_t red_mod(uint64_t a, uint64_t m) {
  return a >= m ? a % m : a;
}

// calculates a plus b modulo m
static inline uint64_t add_mod(uint64_t a, uint64_t b, uint64_t m) {
  const uint64_t SUM = a + b;
  return SUM >= m ? SUM - m : SUM;
}

// calculates a minus b modulo m
static inline uint64_t sub_mod(uint64_t a, uint64_t b, uint64_t m) {
  return a < b ? m + a - b : a - b;
}

// calculates a times b modulo m
static inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t m) {
  return red_mod((__uint128_t) a * b, m);
}

// calculates a to the power n modulo m
static inline uint64_t pow_mod(uint64_t a, uint64_t n, uint64_t m) {
  register uint64_t r = 1;
  while (n > 0) {
    r = mul_mod(r, a, m);
    --n;
  }
  return r;
}

// calculates the inverse of 2 modulo an odd prime, p
static inline uint64_t half_mod(uint64_t p) {
  return (p>>1) + 1;
}

// calculates the legendre symbol of a and b.
static inline int8_t legendre(uint64_t a, uint64_t p) {
  printf("%ld\n", pow_mod(a, (p-1)>>1, p));
  return pow_mod(a, (p-1)>>1, p) == 1 ? 1 : -1;
}

// calculates the least t s.t. a^(2^t) is congruent to 1 modulo a prime
// congruent to 1 modulo 4; returns 0 if this least is M.
static inline uint64_t lst_pwr_2_mod(uint64_t a, uint64_t p, uint64_t M) {
  for (uint64_t t = 0; t < M; ++t) {
    a = mul_mod(a, a, p);
    if (a == 1)
      return t;
  }
  return 0;
}

// returns a quadratic non-residue modulo an odd prime, p, if it exists (which
// it does); returns 0 if it doesn't exist (which it does).
static inline uint64_t find_non_res_mod(uint64_t p) {
  for (register uint64_t z = 2; z < p; ++z)
    if (legendre(z, p) == -1)
      return z;
  return 0;
}

// finds a representation for positive a as S*2^Q
void set_2_pwr(uint64_t* restrict S, uint64_t* restrict Q, int a) {
  register int i = 0;
  for (i = 0; (a & 0b1) == 0; ++i) {
    a = a >> 1;
  }
  *S = i;
  *Q = a;
}

// runs the tonelli-shanks algorithm to calculate the square root of a
// quadratic residue, a, modulo a prime congruent to 1 modulo 4, p
uint64_t tonelli_shanks(uint64_t a, uint64_t p) {
  uint64_t M, Q;
  register uint64_t z, c, t, R, i, b;
  set_2_pwr(&M, &Q, a);
  z = find_non_res_mod(p);
  c = pow_mod(z, Q, p);
  t = pow_mod(a, Q, p);
  R = pow_mod(a, (Q+1)>>1, p);
  while (t > 0) {
    if (t == 1)
      return R;
    else {
      i = lst_pwr_2_mod(a, p, M);
      b = pow_mod(c, pow_mod(2, M-i-1, p-1), p);
      M = i;
      c = pow_mod(b, 2, p);
      t = mul_mod(t, c, p);
      R = mul_mod(R, b, p);
    }
  }
  return 0;
}

// calculates the square root of a quadratic residue, a, modulo an odd prime,
// p.
uint64_t sqrt_mod(uint64_t a, uint64_t p) {
  if (a == 0)
    return 0;
  if ((p & 0b11) == 0b11) // i.e., p is congruent to 3 modulo 4
    return pow_mod(a, (p+1)>>2, p);
  // hence, p is congruent to 1 modulo 4
  return tonelli_shanks(a, p);
}

int main (void) {
  const int P = 7, A = 5;
  if (legendre(A, P) == 1)
    printf("%ld\n", sqrt_mod(A, P));
  return 0;
}