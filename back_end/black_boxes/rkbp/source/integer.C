// integer.C

/*
   The functions in this file perform various basic
   number-theoretic tasks.

   is_prime     determines whether an integer is prime.

   next_prime   returns the first prime larger than its argument.

   prev_prime   returns the first prime, if any, smaller than its
                argument.

   zp_prod      performs multiplication modulo a positive
                integer.
*/


#include <iostream.h>
#include <limits.h>
#include "portable.h"
#include "utility.h"
#include "integer.h"


/*
   The vector small_primes lists the first 10 primes and the
   vector residues lists representatives for the residue classes
   modulo 30 and prime to 30.
*/


static long  small_primes[10] = {2,3,5,7,11,13,17,19,23,29};
static long  nsmall           = 10;
static long  residues[8]      = {1,7,11,13,17,19,23,29};
static long  nres             = 8;



/////////////////////////////////////////////////////////////////
//
//                        is_prime
//
/////////////////////////////////////////////////////////////////

// Return 1 if n is a prime and 0 otherwise.  Negative numbers
// are not considered primes.

long is_prime(
  long  n
)
{
  long   i,k,m,p;

  if (n <= 1) return 0;

  // See if n is divisible by a small prime.

  for (i = 0; i < nsmall; i++)
    if (0 == (n%small_primes[i]) )
      if (n == small_primes[i]) return 1;
      else return 0;

  // If not and n < 31*31, then n is prime.

  if (n < 961) return 1;

  // Let m be an integer slightly bigger than the square root of
  // n.

  m = n;

  for (;;) {
    k = (n+m-1)/m;
    if (k >= m-1) break;
    m = (m+k+1)/2;
  }

  /* Divide n by the integers up to m which are greater than 30
     and relatively prime to 30. */

  p = 30;

  while (p < m) {
    for (i = 0; i < nres; i++)
      if ( 0 == (n % (p+residues[i]) ) ) return 0;

    p += 30;
  }

  return 1;
}



/////////////////////////////////////////////////////////////////
//
//                      next_prime
//
/////////////////////////////////////////////////////////////////

// Return the next prime greater than n.  If there is no prime
// larger than n and not exceeding LONG_MAX, then an error is
// reported and computation stops.

long next_prime(
  long   n
)
{
  if (n < 2) return 2;

  if (LONG_MAX == n) goto too_big;

  if (0 == (n&1) ) n++;
  else n += 2;

  while ( !is_prime(n) ) {
    if (LONG_MAX == n) goto too_big;
    n += 2;
  }

  return n;

 too_big:
  error("Argument too large in next_prime");
  return n;  // This line is never reached!
}



/////////////////////////////////////////////////////////////////
//
//                      previous_prime
//
/////////////////////////////////////////////////////////////////

// Return the first prime smaller than n, or 0 if n <= 2.

long previous_prime(
  long    n
)
{
  if (n <= 2) return 0;
  if (n == 3) return 2;

  if (0 == (n&1) ) n--;
  else n -= 2;

  while ( !is_prime(n) ) n -= 2;

  return n;
}



/////////////////////////////////////////////////////////////////
//
//                       zp_prod
//
/////////////////////////////////////////////////////////////////

// Compute the product of a and b modulo p.  Shifting is used to
// avoid overflow.  The smaller factor should be b.

long zp_prod(
  long   a,
  long   b,
  long   p
)
{
  long  c,u;

  if (p == 0) error("invalid modulus in zp_prod");

  /* Reduce a and b modulo p. */

  if (p < 0) p = -p;

  a = a%p;
  if (a < 0) a += p;

  b = b%p;
  if (b < 0) b += p;

  if (a == 0) return 0;

  /* Multiply a by each binary digit in b. */

  c = 0;
  u = a;

  while (b != 0) {
    if ( 1 == (b&1) ) {
      c += u;
      if (c >= p) c -= p;
    }

    u = u+u;
    if (u >= p) u -= p;
    b >>= 1;
  }

  return c;
}

// end of integer.C
