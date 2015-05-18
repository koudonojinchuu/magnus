// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class SetOfWordsChecker
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#ifndef _SetOfWordsChecker_h_
#define _SetOfWordsChecker_h_

#include "SMFPGroup.h"
#include "Word.h"


// ----------------------- SetOfWordsChecker ------------------------------- //


class SetOfWordsChecker
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SetOfWordsChecker( const SetOf<Word>&, const class SMFPGroup& );

  SetOfWordsChecker( const VectorOf<Word>&, const class SMFPGroup& );

  SetOfWordsChecker( const class SMFPGroup& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial( );

  Chars getExplanation( ) const { return explanation; }
  
  GIC::AlgorithmID getAlgorithm( ) const { return solutionAlgorithm; }

  void replaceTheSet( const VectorOf<Word>& V);
  //  void replaceTheSet( const VectorOf<Word>& V) { theWords = V; }

  void replaceTheSet( const SetOf<Word>& );

  void enablePutDetailsToFile( );
  void disablePutDetailsToFile( );
  bool haveDetails( ) const;
  Chars getDecompositionFileName( ) const;
  Chars getDehnTransformationFileName( ) const;

private:

  void init( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> theWords;

  FPGroup G;

  const class GIC& gic;

  class GCM& gcm;


  // Housekeeping members:

  bool triedAbelianization;

  GIC::AlgorithmID solutionAlgorithm;

  Chars explanation;

  VectorOf<bool> theTrivialWords;
  bool keepDetails;
  DetailedReport dehnsDetails;
  DetailedReport wordsDecomposition;
};


//
//  Class SetOfWordsChecker -- inline functions
//

inline
void SetOfWordsChecker::enablePutDetailsToFile( ) 
{ 
  keepDetails = true; 
}

inline
void SetOfWordsChecker::disablePutDetailsToFile( ) 
{ 
  keepDetails = false; 
}

inline
bool SetOfWordsChecker::haveDetails ( ) const 
{ 
  return wordsDecomposition.haveDetails(); 
}

inline
Chars SetOfWordsChecker::getDecompositionFileName( ) const 
{ 
  return wordsDecomposition.getFileName(); 
}

inline
Chars SetOfWordsChecker::getDehnTransformationFileName( ) const 
{ 
  return dehnsDetails.getFileName(); 
}

#endif
