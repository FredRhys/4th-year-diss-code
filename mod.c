#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// struct to represent ax^2 + bx + c with a, b, c residues of some prime, p
typedef struct quad{
  uint64_t a, b, c;
}quad;

typedef struct coeff{
  uint64_t degree, coeff;
  coeff* next;
}coeff;

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
  if (n == 0)
    return 1;
  while (n > 0) {
    r = mul_mod(r, a, m);
    n -= 1;
  }
  return r;
}

// calculates the inverse of 2 modulo an odd prime, p
static inline uint64_t half_mod(uint64_t p) {
  return (p>>1) + 1;
}

// calculates the legendre symbol of a and b.
static inline uint8_t is_quad_res(uint64_t a, uint64_t p) {
  return pow_mod(a, (p-1)>>1, p) == p-1 ? 0 : 1;
}

// calculates the least t s.t. a^(2^t) is congruent to 1 modulo a prime
// congruent to 1 modulo 4; returns 0 if this least is M.
static inline uint64_t lst_pwr_2_mod(uint64_t a, uint64_t p, uint64_t M) {
  for (register uint64_t t = 1; t < M; ++t) {
    a = mul_mod(a, a, p);
    if (a == 1) {
      return t;
    }
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

// finds a representation for positive a as Q*2^S
void find_2_pwr(uint64_t* restrict S, uint64_t* restrict Q, uint64_t a) {
  register uint64_t i = 0;
  for (i = 0; (a & 0b1) == 0; ++i) {
    a = a >> 1;
  }
  *S = i;
  *Q = a;
}

// runs the tonelli-shanks algorithm to calculate the square root of a
// quadratic residue, a, modulo a prime congruent to 1 modulo 4, p
uint64_t tonelli_shanks(uint64_t a, uint64_t p) {
  register uint64_t z, c, t, R, i, b;
  uint64_t M, Q;
  find_2_pwr(&M, &Q, p-1);
  z = find_non_res_mod(p);
  if (z == 0)
    return 0; // this will never happen; here for completeness
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

static inline quad add_quad(quad f, quad g, uint64_t p) {
  return (quad){add_mod(f.a, g.a, p),
    add_mod(f.a, g.a, p),
    add_mod(f.a, g.a, p)};
}

static inline quad sub_quad(quad f, quad g, uint64_t p) {
  return (quad){sub_mod(f.a, g.a, p),
    sub_mod(f.a, g.a, p),
    sub_mod(f.a, g.a, p)};
}

static inline quad mul_quad(quad f, quad g, uint64_t m, uint64_t k) {
  const uint64_t AD = mul_mod(f.a, g.a, m),
                 BE = mul_mod(f.b, g.b, m),
                 CF = mul_mod(f.c, g.c, m),
                 _6k = mul_mod(6, k, m),
                 _6kAD = mul_mod(_6k, AD, m),
                 ApBmDpE = mul_mod(add_mod(f.a, f.b, m), add_mod(g.a, g.b, m), m),
                 BpCmEpF = mul_mod(add_mod(f.b, f.c, m), add_mod(g.b, g.c, m), m),
                 ApCmDpF = mul_mod(add_mod(f.a, g.a, m), add_mod(g.a, g.c, m), m),
                 _6kconstant = mul_mod(_6k, sub_mod(sub_mod(ApBmDpE, AD, m), BE, m), m);
  return (quad){sub_mod(add_mod(BE, ApCmDpF, m), CF, m),
                add_mod(_6kAD, add_mod(sub_mod(sub_mod(ApBmDpE, AD, m), BE, m), sub_mod(sub_mod(BpCmEpF, BE, m), CF, m), m), m),
                add_mod(_6kconstant, CF, m)};
}


// need to check if coprime
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