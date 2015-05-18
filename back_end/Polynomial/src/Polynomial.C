// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of classes Monomial, Polynomial. 
//
// Principal Author: Dmitry Bormotov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "memory.h"
#include "Polynomial.h"


//-------------------------------- Monomial ---------------------------------//


template <class R>
Monomial<R>::Monomial( const char *s )
{
  numberOfVariables = 0;
  //@njz
  //  istrstream istr(s);
  std::istrstream istr(s);
  //
  istr >> *this;
}


template <class R>
//@njz
//Monomial<R>::Monomial( R coef = 0, int numOfVars = 0,
//		       const int *powersOfVars = NULL ) :
//    theCoefficient(coef), numberOfVariables(numOfVars)
Monomial<R>::Monomial( R coef, int numOfVars,
		       const int *powersOfVars ) :
    theCoefficient(coef), numberOfVariables(numOfVars)
//
{
#if SAFETY > 0
  if( numberOfVariables < 0 )
    error("Monomial<R>::Monomial( R, int, int * ) : "
	  "The number of variables can't be negative.");
//@db What about ( numberOfVariables > 0 && powersOfVars == NULL ) ?
#endif
  if( numberOfVariables > 0 ) {
    powersOfVariables = new int[numberOfVariables];
    memcpy( powersOfVariables, powersOfVars, numberOfVariables * sizeof(int) );
  }
}


template <class R>
Monomial<R>::Monomial( const Monomial& M ) 
  : theCoefficient(M.theCoefficient),
    numberOfVariables(M.numberOfVariables)
{
  if( numberOfVariables > 0 ) {
    powersOfVariables = new int[numberOfVariables];
    memcpy( powersOfVariables, M.powersOfVariables,
	    numberOfVariables * sizeof(int) );
  }
}


template <class R>
Monomial<R>& Monomial<R>::operator = ( const Monomial& M )
{
  if( numberOfVariables != M.numberOfVariables ) {
    if( numberOfVariables > 0 )
      delete [] powersOfVariables;
    numberOfVariables = M.numberOfVariables;
    if( numberOfVariables > 0 )
      powersOfVariables = new int[numberOfVariables];
  }

  theCoefficient = M.theCoefficient;
  memcpy( powersOfVariables, M.powersOfVariables,
	  numberOfVariables * sizeof(int) );

  return *this;
}


template <class R>
bool Monomial<R>::operator == ( const Monomial& M ) const
{
  if( numberOfVariables != M.numberOfVariables ||
      theCoefficient != M.theCoefficient )
    return false;
  for( int i = 0; i < numberOfVariables; i++ )
    if( powersOfVariables[i] != M.powersOfVariables[i] )
      return false;
  return true;

  //return compare(M) == 0;
}

template <class R>
Monomial<R> Monomial<R>::operator + ( const Monomial& M ) const
{
  //@db Can there be a situation in Polynomial when we 
  // add two incompatible monomials? We don't have to worry about
  // other classes since Monomial is for the use in Polynomial only.

  Monomial<R> result(*this);
  result.theCoefficient += M.theCoefficient;
  return result;
}

template <class R>
Monomial<R>& Monomial<R>::operator += ( const Monomial& M )
{
  theCoefficient += M.theCoefficient;
  return *this;
}

template <class R>
Monomial<R> Monomial<R>::operator - () const
{
  Monomial<R> result(*this);
  result.theCoefficient = - result.theCoefficient;
  return result;
}


template <class R>
Monomial<R> Monomial<R>::operator * ( const Monomial& M ) const
{
  const Monomial *M1 = this;
  const Monomial *M2 = &M;
  if( M1->numberOfVariables < M2->numberOfVariables ) {
    const Monomial *R1 = M1;
    M1 = M2;
    M2 = R1;
  }

  Monomial result(*M1);
  result.theCoefficient *= M2->theCoefficient;
  if( result.theCoefficient == R(0) )
    return Monomial(0);

  for( int i = 0; i < M2->numberOfVariables; ++i )
    result.powersOfVariables[i] += M2->powersOfVariables[i];

  // Because of the negative powers we need to check for zeros

  int numOfVars = result.numberOfVariables;
  if( !result.powersOfVariables[numOfVars-1] == 0 )
    return result;
  else {

    for( int i = numOfVars-1; i>=0; --i )
      if( result.powersOfVariables[i] != 0 ) 
	break;
      else
	--numOfVars;

    if( numOfVars == 0 )
      return Monomial( result.theCoefficient );

    int powers[numOfVars];
    for( int i = 0; i < numOfVars; ++i )
      powers[i] = result.powersOfVariables[i];
    
    return Monomial( result.theCoefficient, numOfVars, powers );
  }
}


template <class R>
int Monomial<R>::compare( const Monomial& M ) const
{
  if( numberOfVariables < M.numberOfVariables )
    return -1;
  else
    if( numberOfVariables > M.numberOfVariables )
      return 1;
  else 
    for( int i = 0; i < numberOfVariables; ++i ) {
      int num1 = powersOfVariables[i];
      int num2 = M.powersOfVariables[i];
      if( abs(num1) < abs(num2) )
	return -1;
      else
	if( abs(num1) > abs(num2) )
	  return 1;
	else
	  if( num1 < 0 && num2 > 0 )
	    return -1;
	  else
	    if( num1 > 0 && num2 < 0 )
	      return 1;
     }

  return 0;
}


template <class R>
void Monomial<R>::printOn(ostream& ostr) const
{
  if( theCoefficient == R(-1))
    if( numberOfVariables == 0)
      ostr << " - 1";
    else 
      ostr << " -";
  else
    if( theCoefficient != R(1) || numberOfVariables == 0)
      ostr << " " << theCoefficient;
  
  for( int i = 0; i < numberOfVariables; ++i ) {
    int power = powersOfVariables[i];
    if( power != 0 ) {
      ostr << " x" << i+1;
      if( power != 1 )
	ostr << '^' << power;
    }
  }
}

template <class R>
int Monomial<R>::getPowerOfVariable( int i ) const
{
  if( i < 0 || i >= numberOfVariables )
    error("int Monomial<R>::getPowerOfVariable( int i ) const : "
	  "the index i is out of range");
  return powersOfVariables[i];
}

template <class R>
void Monomial<R>::setPowerOfVariable( int i, int v )
{
  if( i < 0 || i >= numberOfVariables )
    error("int Monomial<R>::setPowerOfVariable( ... ) : "
	  "the index i is out of range");
  powersOfVariables[i] = v;
}


//------------------------------- Polynomial --------------------------------//


template <class R>
Polynomial<R>::Polynomial( const Polynomial& P ) 
  : PBTree< Monomial<R>, Monomial<R> > ( )
{
  PBTreeIterator< Monomial<R>, Monomial<R> > I(P);
  for( ; !I.done(); I.next() )
    this->insert(I.getValue(), I.getValue());
}


template <class R>
Polynomial<R>::Polynomial( const Monomial<R>& M ) 
  : PBTree< Monomial<R>, Monomial<R> > ( )
{
  this->insert(M,M);
}


template <class R>
Polynomial<R>::Polynomial( const char *s )
  : PBTree< Monomial<R>, Monomial<R> > ( )
{
  if( s == 0 ) return;
  int sLen = strlen(s);
  char t[sLen+2];
  for( int i = 0; i < sLen; ++i )
    t[i] = s[i];
  t[sLen] = ',';
  t[sLen+1] = 0;

  //@njz  
  //  istrstream istr(t);
  std::istrstream istr(t);
  //
  istr >> *this;
}

template <class R>
Polynomial<R>::Polynomial( const R& r ) 
  : PBTree< Monomial<R>, Monomial<R> > ( )
{
  Monomial<R> M = Monomial<R>(r);
  this->insert(M,M);
}

template <class R>
Polynomial<R>& Polynomial<R>::operator = ( const Polynomial& P )
{
  this->deleteAll();
  PBTreeIterator< Monomial<R>, Monomial<R> > I(P);
  for( ; !I.done(); I.next() ) {
    //cout << "New Monomial: " << I.getValue() << endl;
    this->insert(I.getValue(), I.getValue());
    //cout << "The tree: " << endl;
    //printAll();
  }
  return *this;
}


template <class R>
bool Polynomial<R>::operator == ( const Polynomial& P ) const
{
  Polynomial result( *this - P );
  PBTreeIterator< Monomial<R>, Monomial<R> > I(result);
  return I.done();
}


template <class R>
bool Polynomial<R>::operator != ( const Polynomial& P ) const
{
  return !(*this == P);
}


template <class R>
Polynomial<R>& Polynomial<R>::operator += ( const Polynomial& P )
{
  PBTreeIterator< Monomial<R>, Monomial<R> > I(P);
  for( ; !I.done(); I.next() )
    this->insert(I.getValue(), I.getValue());
  
  return *this;
}


template <class R>
Polynomial<R>& Polynomial<R>::operator -= ( const Polynomial& P )
{
  (*this) += -P;
  return *this;
}


template <class R>
Polynomial<R>& Polynomial<R>::operator *= ( const Polynomial& P )
{
  Polynomial<R> result = (*this) * P;
  *this = result;
  return *this;
}


template <class R>
Polynomial<R> Polynomial<R>::operator + (const Polynomial& P) const
{
  Polynomial<R> result(*this);
  PBTreeIterator< Monomial<R>, Monomial<R> > I(P);
  for( ; !I.done(); I.next() )
    result.insert(I.getValue(), I.getValue());
 
  return result;
}


template <class R>
Polynomial<R> Polynomial<R>::operator - () const
{
  Polynomial<R> result;
  PBTreeIterator< Monomial<R>, Monomial<R> > I(*this);
  for( ; !I.done(); I.next() )
    result.insert(-I.getValue(), -I.getValue());

  return result;
}


template <class R>
Polynomial<R> Polynomial<R>::operator - (const Polynomial& P) const
{
  return (*this) + (-P);
}


template <class R>
Polynomial<R> Polynomial<R>::operator * (const Polynomial& P) const
{
  Polynomial<R> result;
  PBTreeIterator< Monomial<R>, Monomial<R> > I(*this);
  PBTreeIterator< Monomial<R>, Monomial<R> > J(P);
  for( ; !I.done(); I.next() )
    for( J.reset(); !J.done(); J.next() ) {
      //cout << "New Monomial: " << I.getValue() * J.getValue() << endl;
      result.insert( I.getValue() * J.getValue(), 
		     I.getValue() * J.getValue() );
      //cout << "The tree: " << endl;
      //result.printAll();
      //cout << "The tree has been printed" << endl;
    }
  return result;
}


template <class R>
void Polynomial<R>::printOn(ostream& ostr) const
{
  bool bFirst = true;
  PBTreeIterator< Monomial<R>, Monomial<R> > I(*this);
  if( I.done() ) {
    cout << "0";
    return;
  }
  for( ; !I.done(); I.next() ) {
    if( !bFirst && I.getValue().theCoefficient > R(0) )
      cout << "+";
    cout << I.getValue() << " ";
    bFirst = false;
  }
  //cout << endl;
}


template <class R>
int Polynomial<R>::degree( ) const
{
  PolynomialIterator<R> I(*this);
  int degree = 0;
  for( ; !I.done(); I.next() ) {
    Monomial<R> M = I.getValue();
    int s = 0;
    for( int i = 0; i < M.getNumberOfVariables(); ++i )
      s += abs(M.getPowerOfVariable(i));
    if( s > degree )
      degree = s;
  }
  
  return degree;
}

template <class R>
int Polynomial<R>::numberOfMonomials( ) const
{
  PolynomialIterator<R> I(*this);
  int n = 0;
  for( ; !I.done(); I.next() )
    ++n;
  return n;
}



template class Monomial<Integer>;
template class Monomial<Rational>;
template class Polynomial<Integer>;
template class Polynomial<Rational>;

#include "Int2.h"
template class Monomial<Int2>;
template class Polynomial<Int2>;





