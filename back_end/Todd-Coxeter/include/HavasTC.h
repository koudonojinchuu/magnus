// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class PermutationRepresentation, CosetEnumerator.
//
// Principal Author: Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Revision History:
//
//


#ifndef _HAVAS_TC_H_
#define _HAVAS_TC_H_

#include "BlackBox.h"
#include "FPGroup.h"

enum TC_STATE {NSTARTED, RUNNING, SUCCESS, NOMEM};

// ------------------------- HavasTC --------------//

class HavasTC {
public:

  HavasTC(const FPGroup& group); 
  // Constructor for order of a group problem
  HavasTC(const FPGroup& group,const VectorOf<Word>& subgroup);
  // Constructor for index of a subgroup problem


  ~HavasTC();
  /////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Methods                                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  void start();

  bool finished();

  int getIndex()const { return theIndex;}

  TC_STATE getState() const { return theState; }

  void printWord( ostream& ostr, const Word& w ) const;

  void setWorkspace(int w) { theWorkspace = w; } 

private:
  bool isInitSegment(Chars str, Chars seg ) const;
  
  int theIndex;
  BlackBox tc;
  FPGroup theGroup;
  VectorOf<Word> theSubgroup;

  TC_STATE theState;
  int theWorkspace;
};
#endif































































