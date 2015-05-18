// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definitions of classes Monomial, Polynomial.
//
// Principal Author: Dmitry Bormotov
//
// Status: In development
//
// Usage:
//
// Revision History:
//


#ifndef _Polynomial_H_
#define _Polynomial_H_

#include "Integer.h"
#include "Rational.h"
#include "RingParser.h"
#include "IStreamPoll.h"
#include "PBTree.h"


template <class R> class Polynomial;
template <class R> class MonomialParser;
template <class R> class PolynomialParser;


//--------------------------------- Monomial --------------------------------//


//@db 2.91

template <class R> class Monomial;

template <class R>
ostream& operator << ( ostream& ostr, const Monomial<R>& M )
{
  M.printOn(ostr);
  return ostr;
}

template <class R>
IStreamPoll operator >> ( istream& istr, Monomial<R>& M )
{
  Chars errMesg;
  M = M.readFrom(istr, errMesg);
  return IStreamPoll( istr, errMesg );
}

// 2.91 temporary hacks

template <class R>
ostream& operator < ( ostream& ostr, const Monomial<R>& M )
{
  //o.write(ostr);
  return ostr;
}

template <class R>
istream& operator > ( istream& istr, Monomial<R>& M )
{
  //o.read(istr);
  return istr;
}

//@db end 


template <class R> class Monomial
{

  friend class Polynomial<R>;

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Monomial( const char * );

  Monomial( R coef = 0, int numOfVars = 0, const int *powersOfVars = NULL );

  Monomial( const Monomial& );

  ~Monomial( ) 
  {     
    if( numberOfVariables > 0 )
      delete [] powersOfVariables; 
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool operator == ( const Monomial& ) const;
  
  Monomial& operator = ( const Monomial& );

  Monomial operator - () const;

  Monomial operator * ( const Monomial& ) const;

  Monomial operator + ( const Monomial& ) const;

  Monomial& operator += ( const Monomial& );

  //bool operator < ( const Monomial& M ) const { return compare(M) < 0; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int compare( const Monomial& ) const;
  
  R getCoefficient( ) const { return theCoefficient; }

  void setCoefficient( const R& c ) { theCoefficient = c; }

  int getNumberOfVariables() const { return numberOfVariables; }

  int getPowerOfVariable( int i ) const;

  void setPowerOfVariable( int i, int v);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << <R> ( ostream& ostr, const Monomial<R>& M );

  friend IStreamPoll operator >> <R> ( istream& istr, Monomial<R>& M );


  //@db 2.91 temporary hacks:
  
  friend ostream& operator < <R> ( ostream& ostr, const Monomial<R>& M );
  
  friend istream& operator > <R> ( istream& istr, Monomial<R>& M );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  R theCoefficient;
  int numberOfVariables;
  int *powersOfVariables;


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  void printOn( ostream& ) const;

  Monomial readFrom( istream& istr, Chars& errMesg )
  {
    MonomialParser<R> P(istr);
    return P.parseMonomial(errMesg);
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main(...);

#endif

};


//-------------------------------- Polynomial -------------------------------//


//@db 2.91

template <class R> class Polynomial;

template <class R>
IStreamPoll operator >> ( istream& istr, Polynomial<R>& P )
{
  Chars errMesg;
  P = P.readFrom(istr, errMesg);
  return IStreamPoll( istr, errMesg );
}

//@db


template <class R> 
class Polynomial : public PBTree< Monomial<R>, Monomial<R> >
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Polynomial( ) : PBTree< Monomial<R>, Monomial<R> >( ) { }  

  Polynomial( const char*);

  Polynomial( const Monomial<R>& );

  Polynomial( const Polynomial& );

  Polynomial( const R& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool operator == ( const Polynomial& ) const;

  bool operator != ( const Polynomial& ) const;

  Polynomial& operator = ( const Polynomial& );

  Polynomial& operator += ( const Polynomial& );

  Polynomial& operator -= ( const Polynomial& );

  Polynomial& operator *= ( const Polynomial& );

  Polynomial operator + ( const Polynomial& ) const; 

  Polynomial operator - ( const Polynomial& ) const; 

  Polynomial operator * ( const Polynomial& ) const; 

  Polynomial operator - ( ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isIdentity( ) const;

  int degree( ) const;

  int numberOfMonomials( ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << ( ostream& ostr, const Polynomial& P )
  {
    P.printOn(ostr);
    return ostr;
  }

  friend IStreamPoll operator >> <R> ( istream& istr, Polynomial<R>& P );

private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void printOn( ostream& ) const;

  Polynomial readFrom( istream& istr, Chars& errMesg )
  {
    PolynomialParser<R> P(istr);
    return P.parsePolynomial(errMesg);
  }

  virtual void theKeyIsFound( const Monomial<R>& key, Monomial<R>& value )
  {
    value += key;
    if( value.getCoefficient() == R(0) )
      remove(value);
    //R c = value.getCoefficient();
    //value.setCoefficient( c + key.getCoefficient() ); 
  } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main( );

#endif

};

template <class R> 
struct PolynomialIterator : public PBTreeIterator< Monomial<R>, Monomial<R> >
{ 
  PolynomialIterator( const Polynomial<R>& P )
    : PBTreeIterator< Monomial<R>, Monomial<R> > ( P ) { }
};

#endif



