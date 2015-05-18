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


#include "RingParser.h"
#include "Int2.h"


//------------------------------ RingEltParser ------------------------------//


template <class R>
void RingEltParser<R>::parseWhiteSpace( )
{
  char ch;

  // Flush whitespace & comments.
  do {
    if ( this->peekCh() == '#' )
      while ( this->peekCh() != '\n' ) {
	istr.get();
      }
    if ( ( ch = this->peekCh() ) == ' ' || ch == '\t' || ch == '\n' ) {
      ch = ' ';
      this->getCh();
    }
  } while ( ch == ' ' );
}

template <> Integer RingEltParser<Integer>::parseRingElt( Chars& errMesg )
{
  Integer result;
  
  if ( curToken !=  INT ) {
    this->parseError("expected an integer here");
    errMesg = this->parseErrorMessage;
    return result;
  }
  
  result = tokenInt;
//  this->getToken();
  return result;
}

template <> Int2 RingEltParser<Int2>::parseRingElt( Chars& errMesg )
{
  Int2 result;
  
  if ( curToken !=  INT ) {
    this->parseError("expected an integer here");
    errMesg = this->parseErrorMessage;
    return result;
  }
  
  result = tokenInt;
//  this->getToken();
  return result;
}

template <> Rational RingEltParser<Rational>::parseRingElt( Chars& errMesg )
{
  Integer numerator, denominator;
  
  if ( curToken !=  INT ) {
    this->parseError("expected an integer here");
    errMesg = this->parseErrorMessage;
    return Rational();
  }

  numerator = tokenInt;
/*
  this->getToken();

  if ( curToken ==  COMMA || curToken ==  RSQUARE ) {
    denominator = 1;
    return Rational(numerator, denominator);
  }

  if ( curToken !=  BAD || this->getCh() != '/' ) {
    this->parseError("expected a '/' here");
    errMesg = this->parseErrorMessage;
    return Rational();
  }
*/
  
  this->parseWhiteSpace();
  char ch = this->peekCh();
  if( ch == ',' || ch == ']' ) {
    denominator = 1;
//    this->getCh();
    return Rational(numerator, denominator);
  }

  if ( this->getCh() != '/' ) {
    this->parseError("expected a '/' here");
    errMesg = this->parseErrorMessage;
    return Rational();
  }
  
  this->getToken();

  if ( curToken !=  INT ) {
    this->parseError("expected an integer here");
    errMesg = this->parseErrorMessage;
    return Rational();
  }

  denominator = tokenInt;
//  this->getToken();

  if( denominator == 0 ) {
    this->parseError("invalid rational: zero denominator here");
    errMesg = this->parseErrorMessage;
    return Rational();
   }
  
  return Rational(numerator, denominator);
}


//----------------------------- MatrixParser --------------------------------//


template <class R>
VectorOf<R> MatrixParser<R>::parseMatrixRow( Chars& errMesg )
{
  VectorOf<R> result; 

  if ( this->curToken == INIT ) this->getToken();

  if ( this->curToken !=  LSQUARE ) {
    this->parseError("expected a '[' here");
    errMesg = this->parseErrorMessage;
    return result;
  }

  this->getToken();

  if ( this->curToken !=  INT ) {
    this->parseError("expected an integer here");
    errMesg = this->parseErrorMessage;
    return result;
  }

  while( true ) {

    R elem = this->parseRingElt(errMesg);
    this->getToken();
    
    if( errMesg.length() > 0 )
      return result;
    result.append(elem);
    
    if ( this->curToken ==  RSQUARE ) {
      this->getToken();
      return result;
    }
    
    if ( this->curToken !=  COMMA ) {
      this->parseError("expected a ',' here");
      errMesg = this->parseErrorMessage;
      return result;
    }
    
    this->getToken();
  }
}


template <class R>
void MatrixParser<R>::deleteMatrix( R ***M, int MSize )
{
  for( int i = 0; i < MSize; i++ )
    delete [] M[i];
  delete [] M; 
}


template <class R>
bool MatrixParser<R>::parseMatrix( Chars& errMesg, R ***M, int& MSize )
{
  int numOfRows = 0;
  
  if ( this->curToken == INIT ) this->getToken();
  
  if ( this->curToken !=  LSQUARE ) {
    this->parseError("expected a '[' here");
    errMesg = this->parseErrorMessage;
    return false;
  }

  this->getToken();
  bool bFirstIteration = true;
  
  while( true ) {
    VectorOf<R> row = parseMatrixRow(errMesg);
    if( errMesg.length() > 0 ) {
      if( !bFirstIteration )
	deleteMatrix(M, MSize);
      return false;
    }
    
    if( bFirstIteration ) {
      bFirstIteration = false;
      MSize = row.length();
      //@njz
      //      *M = new (R*)[MSize];
      *M = new R*[MSize];
      //
      for( int i = 0; i < MSize; i++ ) {
	(*M)[i] = new R[MSize];
      }
    }
    else
      if( row.length() != MSize ) {
	this->parseError("incorrect length of this row");
	errMesg = this->parseErrorMessage;
	deleteMatrix(M, MSize);
	return false;
      }

    if( numOfRows >= MSize) {
      this->parseError("too many rows here");
      errMesg = this->parseErrorMessage;
      deleteMatrix(M, MSize);
      return false;
    }
    
    for( int i = 0; i < MSize; i++ ) 
      (*M)[numOfRows][i] = row[i];

    numOfRows++;

    if( this->curToken == RSQUARE )
      if( numOfRows == MSize )
	return true;
      else {
	this->parseError("not enough rows here");
	errMesg = this->parseErrorMessage;
	deleteMatrix(M, MSize);
	return false;
      }
    else
      if( this->curToken == COMMA )
	this->getToken();
      else {
	this->parseError("expected ',' or ']' here");
	errMesg = this->parseErrorMessage;
	deleteMatrix(M, MSize);
	return false;
      }
  }
}

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

  this->parseWhiteSpace();
  char ch = this->peekCh();

  if( ch == '-' ) {
    this->getCh();
    coef = -1;
  }
  else
    if( ch == '+' )
      this->getCh();
  
  this->parseWhiteSpace();
  ch = tolower(this->peekCh());

  if ( ch >= '0' && ch <= '9' ) {
    this->getToken();

    coef *= this->parseRingElt(errMesg);
    if ( errMesg.length() > 0 ) {
      return Monomial<R>( );
    }

    this->parseWhiteSpace();

    if( ( ch = tolower(this->peekCh()) ) != 'x' ) {
      if( ch == ',') {
	this->getToken();
	isLastMonomial = true;
      }
      return Monomial<R>(coef);
    }
  }

  if( ch != 'x' ) {
    this->parseError("expected a 'x' here");
    errMesg = this->parseErrorMessage;
    return Monomial<R>( );
  }
  
  while( true ) {
    
    int index, power;
    this->getCh();
    this->getToken();
    
    if( this->curToken != INT || (index = this->tokenInt) <= 0 ) {
      this->parseError("expected a positive integer here");
      errMesg = this->parseErrorMessage;
      return Monomial<R>( );
    }
    
    this->parseWhiteSpace();
    ch = tolower(this->peekCh());

    if( ch == '^' ) {
      this->getCh();
      this->getToken();
      
      if( this->curToken !=  INT ) {
	this->parseError("expected an integer here");
	errMesg = this->parseErrorMessage;
	return Monomial<R>( );
      }
      power = this->tokenInt;

      this->parseWhiteSpace();
      ch = this->peekCh();
    }
    else
      power = 1;
    
    powersOfVars[index-1] += power;
    if( numOfVars < index )
      numOfVars = index;

    if( ch != 'x' ) {
      if( coef == R(0) )
	numOfVars = 0;
      if( ch == ',' ) {
	this->getToken();
	isLastMonomial = true;
      }
      return Monomial<R>(coef, numOfVars, powersOfVars);
    }
  }
}
/*
template <class R>
Monomial<R> MonomialParser<R>::parseMonomial( Chars& errMesg )
{
  bool isLastMonomial;
  return getMonomial(errMesg, isLastMonomial);
}
*/


//--------------------------- PolynomialParser ------------------------------//


template <class R>
Polynomial<R> PolynomialParser<R>::parsePolynomial( Chars& errMesg )
{
  Monomial<R> M;
  int koef = 1;
  bool isLastMonomial;
  Polynomial<R> result;

  while( true ) {
    M = this->getMonomial(errMesg, isLastMonomial);
    if ( errMesg.length() > 0 ) {
      return Polynomial<R>( );
    }

    if( M.getCoefficient() != R(0) )
      result.insert(M, M);
    
    if( isLastMonomial )
      return result;
    
    char ch = this->peekCh();
    if( ch != '+' && ch != '-' ) {
      this->parseError("expected a sign here");
      errMesg = this->parseErrorMessage;
      return Polynomial<R>( );
    }
  }
}



template class MatrixParser<Integer>;
template class MatrixParser<Rational>;
template class MonomialParser<Integer>;
template class MonomialParser<Rational>;
template class PolynomialParser<Integer>;
template class PolynomialParser<Rational>;

template class MonomialParser<Int2>;
template class PolynomialParser<Int2>;

