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

#include "WordParser.h"
#include "Polynomial.h"


template <class R> class PolynomialParser;
template <class R> class Monomial;
template <class R> class Polynomial;


//------------------------------ RingEltParser ------------------------------//


template <class R> class RingEltParser : public WordParser 
{

public:


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  RingEltParser(istream &istr) : WordParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Accessors:                                               //
  //                                                          //
  //////////////////////////////////////////////////////////////

  void parseWhiteSpace( );
    
  R parseRingElt( Chars& errMesg );


//private:


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

  //friend int main(...);

#endif

};


//----------------------------- MatrixParser --------------------------------//


// The matrices must have the form like 
// [
//   [ x11, x12, x13 ],
//   [ x21, x22, x23 ],
//   [ x31, x32, x33 ]
// ]


template <class R> class MatrixParser : public RingEltParser<R> {

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  MatrixParser(istream &istr) : RingEltParser<R>(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Parser function:                                         //
  //                                                          //
  //////////////////////////////////////////////////////////////
    
  bool parseMatrix( Chars& errMesg, R ***M, int& MSize );


private:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  VectorOf<R> parseMatrixRow( Chars& errMesg );
  
  void deleteMatrix( R ***M, int MSize );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main( );

#endif

};


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

  static const int maxNumberOfVariables = 100;


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

  //friend int main( );

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

  //friend int main( );

#endif

};

#endif



