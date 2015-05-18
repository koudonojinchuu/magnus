// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes AutEnumerator, FiniteAutEnumerator
//
// Principal Author: Roger Needham, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#ifndef _FREE_PROBLEMS_H_
#define _FREE_PROBLEMS_H_


#include "ComputationManager.h"
#include "Supervisor.h"
#include "RandomAutoInFree.h"
#include "SGofFreeGroup.h"
#include "File.h"

// ---------------------- AutoInFreeIsFinitARCer --------------------------- //

class AutoInFreeIsFinitARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutoInFreeIsFinitARCer( ComputationManager& boss ) 
    : ARCer( boss ),theMap( 0 ),retValue(dontknow) { } 
  
  ~AutoInFreeIsFinitARCer( ) { delete theMap; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 void setArguments( const Map& map,const FreeGroup& group );

 Trichotomy getRetValue() const { return retValue;}
 int getOrder() const { return theOrder;}

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

  FreeGroup theGroup;
  Map* theMap;
  
  Trichotomy retValue;
  int theOrder;
};

//---------------------------------------------------------------------------//
//-------------------- AutoInFreeIsFinite  -----------------------------------//
//---------------------------------------------------------------------------//


class AutoInFreeIsFinite : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutoInFreeIsFinite(const class SMMap& hom);

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMMap& theHom;

  AutoInFreeIsFinitARCer arcer;

};

// ---------------------- SGOfFreeContainsConjugateARCer --------------------------- //

class SGOfFreeContainsConjugateARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGOfFreeContainsConjugateARCer( ComputationManager& boss ) 
    : ARCer( boss ),theSubgroup( 0 ) { } 
  
  ~SGOfFreeContainsConjugateARCer( ) { delete theSubgroup; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 void setArguments( const SGofFreeGroup& subgroup, const VectorOf<Word>& gens );

  bool getRetValue(Word& c) const{ 
    if (retValue)
      c = conjugator;
    return retValue; 
  }

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

  SGofFreeGroup* theSubgroup;
  
  bool retValue;
  Word conjugator;
  SetOf<Word> generators;
};
//---------------------------------------------------------------------------//
//-------------------- SGOfFreeContainsConjugate -- -------------------------//
//---------------------------------------------------------------------------//


class SGOfFreeContainsConjugate : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGOfFreeContainsConjugate(const class SMSubgroup& S1, const class SMSubgroup& S2);

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;

  SGOfFreeContainsConjugateARCer arcer;

};


// ---------------------- SGOfFreeConjugateToARCer --------------------------- //

class SGOfFreeConjugateToARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGOfFreeConjugateToARCer( ComputationManager& boss ) 
    : ARCer( boss ),theSubgroup( 0 ) { } 
  
  ~SGOfFreeConjugateToARCer( ) { delete theSubgroup; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 void setArguments( const SGofFreeGroup& subgroup, const VectorOf<Word>& gens );

  bool getRetValue( ) const{ 
    return retValue; 
  }

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

  SGofFreeGroup* theSubgroup;
  
  bool retValue;
  SetOf<Word> generators;
};
//---------------------------------------------------------------------------//
//-------------------- SGOfFreeConjugateTo ----------------------------------//
//---------------------------------------------------------------------------//


class SGOfFreeConjugateTo : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGOfFreeConjugateTo(const class SMSubgroup& S1, const class SMSubgroup& S2);

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;

  SGOfFreeConjugateToARCer arcer;

};

// ---------------------- AutEnumeratorARCer --------------------------- //


class AutEnumeratorARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutEnumeratorARCer( ComputationManager& boss ) 
    : ARCer( boss ), randAuto( 0 ), isFinite(0) { } 

  ~AutEnumeratorARCer( ) { delete randAuto; file << end;} 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FreeGroup& group,int avgNumbers, bool isfinite);

  // Inherited from ARCer:
  // bool takeControl( );

  Chars getFileName();
  
  
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

  RandomAutoInFree* randAuto;
  bool isFinite;
  File file;
};

//---------------------------------------------------------------------------//
//--------------------------- AutEnumerator ---------------------------------//
//---------------------------------------------------------------------------//


class AutEnumerator : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutEnumerator(class SMFPGroup& F, int avgNumGens = 1, bool is_finite = false);

  /*  AutEnumerator(class SMFPGroup& F)
    : ComputationManager( true ), theGroup( F ), numGensPicker(0),
		typeGenPicker(0)
  { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.
  */
  ~AutEnumerator( );

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

  const IconID iconID( ) const;             // overrides ComputationManager

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theGroup;

  AutEnumeratorARCer arcer;
  int avgNumGens;

  bool isFinite;
  bool isStarted;
};

#endif
