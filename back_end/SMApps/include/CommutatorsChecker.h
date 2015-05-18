// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes CommutatorsChecker, CommutatorsCheckerARCer
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _CommutatorsChecker_h_
#define _CommutatorsChecker_h_


#include "SMFPGroup.h"
#include "ARCer.h"


// ------------------------- CommutatorsChecker ---------------------------- //


class CommutatorsChecker
{
public:
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  CommutatorsChecker( class SMFPGroup& group, int length );

  CommutatorsChecker( class SMFPGroup& group, int length,
		      const VectorOf<Word>& V );
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy areTrivial( );
  
  Chars getExplanation( ) { return explanation; }

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const CommutatorsChecker& o )
  {
    ostr < o.triedAbelianization < o.explanation;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, CommutatorsChecker& o )
  {
    istr > o.triedAbelianization > o.explanation;
    return istr;
  }

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup G;
  class GIC& gic;
  class GCM& gcm;

  int theLength;
  VectorOf<Word> generators;
  
  // Housekeeping members:

  bool triedAbelianization;
  Chars explanation;
};


// ---------------------- CommutatorsCheckerARCer -------------------------- //


class CommutatorsCheckerARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  CommutatorsCheckerARCer( ComputationManager& boss ) 
    : ARCer( boss ), retValue( dontknow ) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( CommutatorsChecker* );

  Trichotomy getRetValue() { return retValue; }

  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  CommutatorsChecker* theChecker;

  Trichotomy retValue;

};

#endif
