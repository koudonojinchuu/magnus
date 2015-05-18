// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of classes LCSQuotient, LCSQuotientSeries
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
// The LCSQuotient of weight k is an abelian factor G_k / G_{k+1},
// where G_i is an i-th term of the lover central series (LCS).
//
// Special Notes:
//
//
// Revision History:
//

#ifndef _LCS_QUOTIENTS_H_
#define _LCS_QUOTIENTS_H_

#include "PolyWord.h"
#include "AbelianGroup.h"
#include "MalcevSet.h"
//#include <iostream.h>


//========================================================================
//====================== BasisWord =======================================
//========================================================================

//@ep maybe it will be a real class some day. Now it is designed for 
//    internal use only.


struct BasisWord {

  //-------- Fictive operators for VectorOf templates ----------------

  friend bool operator == (const BasisWord& w1, const BasisWord& w2);
  friend ostream& operator << (ostream& s, const BasisWord& w);


  //-------- IPC tools -----------------------------------------------

  friend ostream& operator < ( ostream& s, const BasisWord& w );
  // IPC output

  friend istream& operator > ( istream& s, BasisWord& w );
  // IPC input

  //-------- Data ----------------------------------------------------

  PolyWord theWord;
  //the word in terms of basic commutators

  int theWeight;
  //its weight in the group

  int theOrder;
  //its order (0 means infinite)

  int theLocalNumber;
  // its ordinal number in the LCS quotient, 1-based
  // (basic words of finite and infinite orders are numerated separately) 

  int theGlobalNumber;
  // its ordinal number in the group basis, 1-based

  Chars theName;
  // the name of the word

};


//========================================================================
//==================== LCSQuotient =======================================
//========================================================================


class LCSQuotient {

public:

  //----------------------------------------------------------------------
  //  Constructors / initializers
  //----------------------------------------------------------------------
  
  LCSQuotient() : abelianization( FPGroup() ) { }
  //default constructor 


  friend bool operator == (const LCSQuotient& q1, const LCSQuotient& q2);
  // comparison operator (fictive to fool the VectorOf templates

  friend ostream& operator << (ostream& s, const LCSQuotient& q);
  // to fool the VectorOf templates

  friend ostream& operator < ( ostream& s, const LCSQuotient& q );
  // IPC output

  friend istream& operator > ( istream& s, LCSQuotient& q );
  // IPC input

  //--------------------------------------------------------------------
  //   Data
  //--------------------------------------------------------------------

  AbelianGroup abelianization;
  VectorOf<BasisWord> generators;
  int infNumber;
  int numerationShift;
  MalcevSet basis;
};


#endif


