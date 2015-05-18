// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the PrimeNumbers class
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


#include "PrimeNumbers.h"

// ---------------------- IntProblems -----------------------------//
Integer IntProblems::gcdOfVector(const VectorOf<Integer>& vc) const
{
   #if SAFETY > 0
     if (vc.length() == 0 )
        error("Could not compute gcd of empty vector in  gcdOfVector(const VectorOf<Integer>& vc) ");
   #endif        
  
  if (vc.length() == 1) 
         return abs(vc[0]);
  Integer gcdV = gcd(vc[0],vc[1]);
  for (int i=2;i<vc.length();i++)
          gcdV = gcd(gcdV,vc[i]);

  return gcdV;    
}

Integer IntProblems::powerOf(const Integer& o, const Integer& p) const
{
   Integer io=o, power=0;
   if (io == 1)
            return 0;
   if (io<p) return -1;
     while (io>1){
        if ((io%p)!=0)
             return -1;
        io = io/p;
        power+=1;
   }
   return power;
}

void IntProblems::findCoeff(Integer p, Integer q, Integer& x, Integer& y, Integer mod) const
{
   for (x=0;x<mod;x+=1)
       for(y=0;y<mod;y+=1)
           if ((p*x + q*y)%mod == 1) 
                     return;
}


// ------------------- PrimeNumbers::printPrimeDecom ---------------------//
void  PrimeNumbers::printPrimeDecom(ostream& ostr,const Integer& num)
{
   #if SAFETY > 0
     if ( num < 1 )
        error("the prime decompozition could not be done for this number in PrimeNumbers::getPrimeDecom(ostream& ostr,const Integer& num) ");
   #endif        
   if (num < 4){
      ostr << num;
      return;
   }
   Integer savNum = num;
   Integer powers,primes;
   bool firstDone = false;
         while (!isPrime(savNum)){
           powers = 0;
           primes = 0;
           if (savNum%current()==0){
             while (savNum%current()==0){
                 savNum = savNum / current();
                 powers+=1;
                 primes=current();
             }
	    if (firstDone)
                 ostr << " x ";
            else
                 firstDone = true;
            ostr << primes;
            if (powers > 1)
                 ostr << "^" << powers;
           }
            nextNumber();
	 }
         if (savNum>1){
             if (firstDone)
               ostr << " x ";
             ostr << savNum;
         }
   return;
}

// ------------------- PrimeNumbers::getPrimeDecom ---------------------//
DArray<Integer> PrimeNumbers::getPrimeDecom(const Integer& num)
{
#if SAFETY > 0
  if ( num < 1 )
    error("the prime decompozition could not be done for this number in "
	  "PrimeNumbers::getPrimeDecom(const Integer& num) ");
#endif     

  DArray<Integer> retDArray(1,2);   
  if (num < 4){
    retDArray[0][0] =  num;
    retDArray[0][1] =  1;
    return retDArray;
  }
  Integer savNum = num;
  int i=0;
  while (!isPrime(savNum)){
    if (savNum%current()==0){
      retDArray[i][0] =  0;
      retDArray[i][1] =  0;
      while (savNum%current()==0){
	savNum = savNum / current();
	retDArray[i][1] += 1;
	retDArray[i][0]=current();
      }
      i++;
      retDArray.rowInsAfter(i-1,1);
    }
      nextNumber();
  }
  if (savNum>1){
      retDArray[i][0] =  savNum;
      retDArray[i][1] =  1;
  }
  return retDArray;
}





