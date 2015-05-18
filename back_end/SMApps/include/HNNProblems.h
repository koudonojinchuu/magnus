// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes 
// 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#ifndef _HNN_PROBLEMS_H_
#define _HNN_PROBLEMS_H_

#include "IsFreeProblem.h"
#include "fastProblems.h"

// ========================================================================= //
// ============================= FASTPROBLEMS.H ============================ //
// ========================================================================= //

class MakeHNNExtOfFreeGroup : public FastComputation
{
public:

  MakeHNNExtOfFreeGroup( const SMFPGroup& G,
			 const class SMSubgroup& S1, const class SMSubgroup& S2 )
    : theStableLetter( "t" /*stableLetter*/ ), 
      subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  const Chars theStableLetter;
  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};


class HNNofFreeGroup_IsTrivial : public FastComputation 
{
public:

  HNNofFreeGroup_IsTrivial( const SMFPGroup& G )
    : group( G )
  { }

  void takeControl( );

private:

  const SMFPGroup& group;
};

class HNNofFreeGroup_IsFinite : public FastComputation 
{
public:

  HNNofFreeGroup_IsFinite( const SMFPGroup& G )
    : group( G )
  { }

  void takeControl( );

private:

  const SMFPGroup& group;
};

class HNNofFreeGroup_IsAbelian : public FastComputation 
{
public:
  HNNofFreeGroup_IsAbelian( const SMFPGroup& G )
    : group( G )
  { }

  void takeControl( );

private:

  const SMFPGroup& group;
};


class HNNofFreeGroup_ConjugacyProblem : public FastComputation 
{
public:

  HNNofFreeGroup_ConjugacyProblem( const SMWord& x, const SMWord& y )
    : word1( x ), word2( y ) 
  { }

  void takeControl( );

private:

  const SMWord& word1;
  const SMWord& word2;
};

class HNNofFreeGroup_WordProblem : public FastComputation 
{
public:

  HNNofFreeGroup_WordProblem( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};

class HNNofFreeGroup_ReducedForm : public FastComputation 
{
public:

  HNNofFreeGroup_ReducedForm( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};

class HNNofFreeGroup_NormalForm : public FastComputation 
{
public:

  HNNofFreeGroup_NormalForm( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};

class HNNofFreeGroup_CyclicallyReducedForm : public FastComputation 
{
public:

  HNNofFreeGroup_CyclicallyReducedForm( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};

class HNNofFreeGroup_IsProperPowerOfSecond : public FastComputation 
{
public:

  HNNofFreeGroup_IsProperPowerOfSecond( const SMWord& x, const SMWord& y )
    : word1( x ), word2( y )
  { }

  void takeControl( );

private:

  const SMWord& word1;
  const SMWord& word2;
};

class HNNofFreeGroup_IsProperPower : public FastComputation 
{
public:

  HNNofFreeGroup_IsProperPower( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};

class HNNofFreeGroup_MaximalRoot : public FastComputation 
{
public:

  HNNofFreeGroup_MaximalRoot( const SMWord& x ) : word( x ) { }

  void takeControl( );

private:

  const SMWord& word;
};


class HNNofFreeGroup_AreEqual : public FastComputation 
{
public:

  HNNofFreeGroup_AreEqual( const SMWord& x, const SMWord& y )
    : word1( x ), word2( y )
  { }

  void takeControl( );

private:

  const SMWord& word1;
  const SMWord& word2;
};


class HNNofFreeGroup_IsSGTrivial : public FastComputation 
{
public:

  HNNofFreeGroup_IsSGTrivial( const SMSubgroup& S )
    : subgroup( S )
  { }

  void takeControl( );

private:

  const SMSubgroup& subgroup;
};


class HNNofFreeGroup_IsSGAbelian : public FastComputation 
{
public:

  HNNofFreeGroup_IsSGAbelian( const SMSubgroup& S )
    : subgroup( S )
  { }

  void takeControl( );

private:

  const SMSubgroup& subgroup;
};

class CheckinHNNofFreeGroup : public FastComputation
{
public:

  CheckinHNNofFreeGroup( ) { }

  void takeControl( );              // overrides ComputationManager
};

// +++++++++++++++++++++++++++++ FASTPROBLEMS.H ++++++++++++++++++++++++++++ //



/*
// ---------------------- HNNofFreeIsFreeArcer --------------------------- //


class HNNofFreeIsFreeArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNofFreeIsFreeArcer( ComputationManager& boss ) 
    : ARCer( boss ), theAnswer( dontknow ), theGroup( 0 ) 
  { }

  ~HNNofFreeIsFreeArcer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const HNNExtOfFreeGroup& H );

  // Inherited from ARCer:
  // bool takeControl( );

  Trichotomy answer() const { return theAnswer; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& ostr );

  void readResults( istream& istr );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNExtOfFreeGroup *theGroup;
  Trichotomy theAnswer;
};
*/

// --------------------------------------------------------------------- //
// ------------------------- HNNofFreeIsFree --------------------------- //
// --------------------------------------------------------------------- //


class HNNofFreeIsFree : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNofFreeIsFree( class SMFPGroup& F );

  ~HNNofFreeIsFree();
    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

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

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theGroup;
  
  ORProblems* theORProblems;
  ORIsFreeProblemARCer arcer;
  
  //HNNofFreeIsFreeArcer arcer;
};


// ====================================================================== //
//                                                                        // 
//                                                                        // 
//       Amalgamated product of free groups's problems                    // 
//                                                                        // 
//                                                                        // 
// ====================================================================== //


// ------- Freeness of an amalgamated product of free groups  -------- //

class APofFreeIsFreeArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  APofFreeIsFreeArcer( ComputationManager& boss ) 
    : ARCer( boss ), theAnswer( dontknow ), theGroup( 0 ) 
  { }

  ~APofFreeIsFreeArcer( ) { delete theGroup; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const AmalgProductOfFreeGroups& G );

  // Inherited from ARCer:
  // bool takeControl( );

  Trichotomy answer() const { return theAnswer; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& ostr );

  void readResults( istream& istr );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AmalgProductOfFreeGroups *theGroup;
  Trichotomy theAnswer;
};

class APofFreeIsFree : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  APofFreeIsFree( class SMFPGroup& F );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

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

  void start( ) { }              // overrides ComputationManager
  void terminate( ) { }          // overrides ComputationManager

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theGroup;
  APofFreeIsFreeArcer arcer;
};



// --------- Is an amalgamated product of free groups perfect  --------- //

class APofFreeIsPerfect : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  APofFreeIsPerfect( class SMFPGroup& F );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

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

  void start( ) { };              // overrides ComputationManager
  void terminate( ) { };          // overrides ComputationManager

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theGroup;
  MirrorSubordinate abelianInvariants;
};


// --------- Homology of an amalgamated product of free groups  -------- //

class APofFreeHomologyArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  APofFreeHomologyArcer( ComputationManager& boss ) 
    : ARCer( boss ), theGroup( 0 ), theRankOfH2(-1)
  { }

  ~APofFreeHomologyArcer( ) { delete theGroup; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const AmalgProductOfFreeGroups& G );

  // Inherited from ARCer:
  // bool takeControl( );

  int rankOfH2() const { return theRankOfH2; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& ostr );

  void readResults( istream& istr );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AmalgProductOfFreeGroups *theGroup;
  int theRankOfH2;
};

class APofFreeHomologyProblem : public Supervisor //ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  APofFreeHomologyProblem( class SMFPGroup& F );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

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

  void start( ) { }              // overrides ComputationManager
  void terminate( ) { }          // overrides ComputationManager

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theGroup;
  APofFreeHomologyArcer arcer;
  MirrorSubordinate abelianInvariants;
  bool cyclicSubgroup;
  bool abDone;
  bool arcerStarted;
};





#endif
