// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of class RandomAutoInFree
//
// Principal Author:Alexey Myasnikov
//
// Status: in progress
//
// Revision History:
//

#ifndef _RANDOMAUTOINFREE_H_
#define _RANDOMAUTOINFREE_H_


#include "RandomNumbers.h"
#include "FreeGroup.h"
#include "Map.h"


//---------------------------------------------------------------------------//
//--------------------------- RandomAutoInFree ------------------------------//
//---------------------------------------------------------------------------//


class RandomAutoInFree
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandomAutoInFree(const FreeGroup& F, int avgNumGens);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  Map getAutomorphism();
  // Returns random automorhism.
  Map getFiniteAutomorphism(int& order);
  // Returns random automorhism of finite order. "order" is equal to
  // order of automorpism.
  Trichotomy isFiniteAutomorphism(const Map& m,int& order)const;
  // Returns true if automorhism of finite order, no if not and dontknow, if
  // it can't solve this problem. "order" is equal to order of automorpism if it finite.

 
private:

  bool isTooLong(const VectorOf<Word>& images )const;
  int sumOfImagesLens(const VectorOf<Word>& images)const;
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const short MAXLENGTH = 32767; //??????????????????
  FreeGroup theGroup;

  int numberOfGroupGens;

  NormalRandom numGensPicker;
  UniformRandom typeGenPicker;

  int avgNumGens;

};

#endif
