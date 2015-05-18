// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the PrimeNumbers and IntProblems classes
//
// Principal Author:  Alexey Myasnikov
//
// Status: in progress
//
// Usage:
//
// Special Notes:
//
// Revision History:
//

#ifndef _PRIMENUMBERS_H_
#define _PRIMENUMBERS_H_

#include "Integer.h"
#include "Vector.h"
#include "DArray.h"

// IntProblems involces some methods to deal with Integers
struct IntProblems
{
  Integer gcdOfVector(const VectorOf<Integer>& vc)const;
  // Computes and returns the gcd of integers in
  // vector vc.

  Integer powerOf(const Integer& o, const Integer& p)const;
  // Returns the power e if p^e = o, if not returns -1

  void findCoeff(Integer p, Integer q, Integer& x, Integer& y, Integer mod)const;
  // Computes the coefficients x, y such that xp + yq = 1 (modulo mod)
};

// Class with methods in which prime numbers are involved
class PrimeNumbers {
public:
   PrimeNumbers() : currentNumber(2) {}
   const Integer& current() const {
       return currentNumber;
   }

   // Gives thee next prime number from current
   const Integer&  nextNumber(){
     while (true){
      currentNumber+=1;
      if (isPrime(currentNumber))
          return currentNumber;
      }
   }

   // Sets the current number 
   void setCurrent(const Integer& num){
       currentNumber = num;
   }

   // True if number is prime
   bool isPrime(Integer num) const
     {
       for (int i=2;i<=sqrt(num);i++){
           if (num%i==0) 
                return false;
       }
       return true;
     } 

   // returns primary decomposition of num. First column - primes,
   // second their powers.
   DArray<Integer> getPrimeDecom(const Integer& num);
   // Prints primary decomposition of num in ostr. 2^3 + 3^2 + 5 ...
   void printPrimeDecom(ostream& ostr,const Integer& num);
private:
   Integer currentNumber;
   // current prime number
};
#endif
