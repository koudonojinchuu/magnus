// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of classes RingEltParser, MatrixParser,
//           MonomialParser, PolynomialParser.
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
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

//#include "WordParser.h"
#include "RingParser.h"



//--------------------------- MonomialParser -------------------------------//


template <class R>
Monomial<R> MonomialParser<R>::getMonomial( Chars& errMesg,
					    bool& isLastMonomial )
{
  int powersOfVars[maxNumberOfVariables];
  R coef = 1;
  int numOfVars = 0;
  isLastMonomial = false;

  for( int i = 0; i < maxNumberOfVariables; ++i )
    powersOfVars[i] = 0;

  parseWhiteSpace();
  char ch = peekCh();

  if( ch == '-' ) {
    getCh();
    coef = -1;
  }
  else
    if( ch == '+' )
      getCh();
  
  parseWhiteSpace();
  ch = tolower(peekCh());

  if ( ch >= '0' && ch <= '9' ) {
    getToken();

    coef *= parseRingElt(errMesg);
    if ( errMesg.length() > 0 ) {
      return Monomial<R>( );
    }

    parseWhiteSpace();

    if( ( ch = tolower(peekCh()) ) != 'x' ) {
      if( ch == ',')
	getToken();
      return Monomial<R>(coef);
    }
  }

  if( ch != 'x' ) {
    parseError("expected a 'x' here");
    errMesg = parseErrorMessage;
    return Monomial<R>( );
  }
  
  while( true ) {
    
    int index, power;
    getCh();
    getToken();
    
    if( curToken != INT || (index = tokenInt) <= 0 ) {
      parseError("expected a positive integer here");
      errMesg = parseErrorMessage;
      return Monomial<R>( );
    }
    
    parseWhiteSpace();
    ch = tolower(peekCh());

    if( ch == '^' ) {
      getCh();
      getToken();
      
      if( curToken !=  INT || (power = tokenInt) <= 0 ) {
	parseError("expected a positive integer here");
	errMesg = parseErrorMessage;
	return Monomial<R>( );
      }

      parseWhiteSpace();
      ch = peekCh();
    }
    else
      power = 1;
    
    powersOfVars[index-1] += power;
    if( numOfVars < index )
      numOfVars = index;

    if( ch != 'x' ) {
      if( coef == 0 )
	numOfVars = 0;
      if( ch == ',' ) {
	getToken();
	isLastMonomial = true;
      }
      return Monomial<R>(coef, numOfVars, powersOfVars);
    }
  }
}

template <class R>
Monomial<R> MonomialParser<R>::parseMonomial( Chars& errMesg )
{
  bool isLastMonomial;
  return getMonomial(errMesg, isLastMonomial);
}



//--------------------------- PolynomialParser ------------------------------//


template <class R>
Polynomial<R> PolynomialParser<R>::parsePolynomial( Chars& errMesg )
{
  Monomial<R> M;
  int koef = 1;
  bool isLastMonomial;
  Polynomial<R> result;

  while( true ) {
    M = getMonomial(errMesg, isLastMonomial);
    if ( errMesg.length() > 0 ) {
      return Polynomial<R>( );
    }

    result.insert(M);
    
    if( isLastMonomial )
      return result;
    
    char ch = peekCh();
    if( ch != '+' && ch != '-' ) {
      parseError("expected a sign here");
      errMesg = parseErrorMessage;
      return Polynomial<R>( );
    }
  }
}



template class MonomialParser<Integer>;
template class MonomialParser<Rational>;
template class PolynomialParser<Integer>;
template class PolynomialParser<Rational>;
