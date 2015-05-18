// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class PermutationParser
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Usage:
//
// Revision History:
//


#ifndef _PermutationParser_h_
#define _PermutationParser_h_

#include "Permutation.h"


//--------------------------- PermutationParser -----------------------------//


class PermutationParser
{

  // We represent permutation by it's bottom row, i.e., { 3 1 2 }
  // means 1 -> 3, 2 -> 1, 3 -> 2. It should be a list of integers
  // separated by spaces and enclosed in braces. The list must define
  // a permutation, i.e., it must be a list of integers without holes,
  // contain no integer twice, contain no integer not in the range
  // from 1 to the length of the list. Also, we assume that i -> i for
  // all i greater than the length of the list.

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  PermutationParser(istream &str) : istr(str) { }
  // Initialize the parser with the istream from which to read.

  // Destructor, cc and op = supplied by compiler.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Accessors:                                               //
  //                                                          //
  //////////////////////////////////////////////////////////////
    
  Permutation parsePermutation( Chars& errMesg );
  // Read permutation from istream; errMesg explain errors if any, or
  // empty otherwise.

  int cursorPos( ) const { return pos; }
  // returns the current cursor position while parsing; useful
  // when error has occured to show where the error was

private:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Data members:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  istream& istr;  // istream from which to read
  int pos;        // error position when possible


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  bool isDigit( char c );
  // true if c is digit and not zero, i.e., 1..9

  bool isSpace( char c );
  // returns true if c is a space character or \t or \n

  void eatWhite( );
  // skips white space
};

#endif



