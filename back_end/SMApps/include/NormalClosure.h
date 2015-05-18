// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of classes NormalClosureARCer, NormalClosure
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
//

#ifndef _NORMALCLOSURE_H_
#define _NORMALCLOSURE_H_

#include "ComputationManager.h"
#include "SubgroupGraph.h"
#include "FPGroup.h"


// ------------------------ NormalClosureARCer ----------------------------- //


class NormalClosureARCer : public ARCer2
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NormalClosureARCer( ComputationManager& boss );

  ~NormalClosureARCer( );

    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FPGroup&, const VectorOf<Word>& sgens );

  // Inherited from ARCer:
  // bool takeControl( );

  class DecomposeInSubgroupOfFPGroup& getDecomposer() const { 
    return *theDecomposer; 
  }

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& ) { }

  bool readResults2( istream& );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class DecomposeInSubgroupOfFPGroup* theDecomposer;
  class DecomposeInSubgroupOfFPGroup* tempDecomposer;
};


// ---------------------------- NormalClosure ------------------------------ //


class NormalClosure : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NormalClosure(class GCM& gcm);

  NormalClosure(class SCM& scm);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial(const Word& w) const;

  DecomposeInSubgroupOfFPGroup& getDecomposer( );

  SubgroupGraph getSubgroupGraph( ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  void putHaveWordDecomposer( const DecomposeInSubgroupOfFPGroup& D );
  void putHaveCompleteCayleyGraph( const SubgroupGraph& S );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NormalClosureARCer arcer;

  class ComputationManager& theCM;
  class SMFPGroup& theGroup;
  class SMObject& theObject;
  //  class GCM& theGCM;

};

#endif
