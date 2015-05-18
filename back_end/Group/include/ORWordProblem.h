// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of utility class ORWordProblem, which wraps
//           a BlackBox connected to Dion Gildenhuys' one relator
//           word problem program.
//
// Principal Author: Roger Needham
//
// Status: Under development
//
// Usage:  (@rn out of date)
//
// You need only one object in this class, since the program it talks to
// accepts arbitrarily many pairs of relators and test words.
//
// Multiplexing is not a problem now, since BlackBox communication is
// strictly synchronous.
//
// First call findAnswer(const Word& relator, const Word& testWord)
// to compute the result.
// Then call goodStatus( ) to see if anything went wrong.
// If that returns true, get the result from isTrivial( ),
// otherwise this effectively shuts down forever.
//
// Revision History:
//


#ifndef _OR_WORD_PROBLEM_H_
#define _OR_WORD_PROBLEM_H_


#include "BlackBox.h"
#include "Word.h"


class ORWordProblem {

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  ORWordProblem(const Word& relator);

  ~ORWordProblem( ) { delete orwp; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void findAnswer(const Word& testWord);
  // This will not return until it has the answer.

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Accessors:                                                       //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  bool goodStatus( ) { return status; }

  bool isTrivial( );

private:

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Data Members:                                                    //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  BlackBox* orwp;
  // Wraps the orwp executable.

  const Word theRelator;

  bool status;
  // True until something goes wrong, then we give up altogether.

  bool result;
  // The last answer computed.

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Private Members:                                                 //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  void printWord(const Word& w, ostream& ostr);
  // Output w in Dion's format.

};

#endif
