// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definitions of classes RingEltParser, MatrixParser,
//           MonomialParser, PolynomialParser.
//
// Principal Author: Dmitry Bormotov
//
// Status: In development
//
// Usage:
//
// Revision History:
//


#ifndef _RING_PARSER_H_
#define _RING_PARSER_H_

#include "RingEltParser.h"
#include "Polynomial.h"



//---------------------------- MonomialParser -------------------------------//


template <class R> class MonomialParser : public RingEltParser<R> 
{
  
  friend class PolynomialParser<R>;

public:


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  MonomialParser(istream &istr) : RingEltParser<R>(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Parser function:                                         //
  //                                                          //
  //////////////////////////////////////////////////////////////
    
  Monomial<R> parseMonomial( Chars& errMesg )
  {
    bool isLastMonomial;
    return getMonomial(errMesg, isLastMonomial);
  }

private:


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Data members:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  static const maxNumberOfVariables = 100;


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  Monomial<R> getMonomial( Chars& errMesg, bool& isLastMonomial );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  friend int main( );

#endif

};


//--------------------------- PolynomialParser ------------------------------//


template <class R> class PolynomialParser : public MonomialParser<R> 
{
  
public:
  
  
  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  PolynomialParser(istream &istr) : MonomialParser<R>(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Parser function:                                         //
  //                                                          //
  //////////////////////////////////////////////////////////////
    
  Polynomial<R> parsePolynomial( Chars& errMesg );


private:


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Data members:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  friend int main( );

#endif

};

#endif



