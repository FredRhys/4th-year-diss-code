#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pair{
  uint64_t fst, snd;
}pair;

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
static inline int8_t is_quad_res(uint64_t a, uint64_t p) {
  return pow_mod(a, (p-1)>>1, p) == p-1 ? 0 : 1;
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
    if (!is_quad_res(z, p))
      return z;
  return 0;
}

// finds a representation for positive a as S*2^Q
pair find_2_pwr(uint64_t a) {
  register uint64_t i = 0;
  for (i = 0; (a & 0b1) == 0; ++i) {
    a = a >> 1;
  }
  return (pair){i, a};
}

// runs the tonelli-shanks algorithm to calculate the square root of a
// quadratic residue, a, modulo a prime congruent to 1 modulo 4, p
uint64_t tonelli_shanks(uint64_t a, uint64_t p) {
  register uint64_t z, c, t, R, i, b;
  pair MQ = find_2_pwr(a);
  z = find_non_res_mod(p);
  c = pow_mod(z, MQ.snd, p);
  t = pow_mod(a, MQ.snd, p);
  R = pow_mod(a, (MQ.snd+1)>>1, p);
  while (t > 0) {
    if (t == 1)
      return R;
    else {
      i = lst_pwr_2_mod(a, p, MQ.fst);
      b = pow_mod(c, pow_mod(2, MQ.fst-i-1, p-1), p);
      MQ.fst = i;
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

int main (int argc, char** argv) {
  if (argc != 3)
    return 0;
  const int a = atoi(argv[1]), p = atoi(argv[2]);
  if (!is_quad_res(a, p)) {
    printf("%d is not a quadratic residue modulo %d\n", a, p);
    return 0;
  }
  printf("sqrt(%d) modulo %d is %ld\n", a, p, sqrt_mod(a, p));
  return 0;
}