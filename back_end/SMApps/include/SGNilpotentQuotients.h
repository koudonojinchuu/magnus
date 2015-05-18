// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes  

//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#ifndef _SGNILPOTENT_QUOTIENTS_H_
#define _SGNILPOTENT_QUOTIENTS_H_


//@rn  This is specialized for the moment to work only on the specific class of
//@rn  a group which was checked in as nilpotent. It can, in principle, work
//@rn  on any nilpotent quotient of any FP group.


#include "Supervisor.h"
#include "NilpotentGroup.h"
#include "ComputationManager.h"
//#include "List.h"
//#include "File.h"
//#include "Presentation.h"

// ---------------------- SCPNilpotentARCer --------------------------- //


class SCPNilpotentARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SCPNilpotentARCer( ComputationManager& boss );

  ~SCPNilpotentARCer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const class SGOfNilpotentGroup& subgroup,
		     const SetOf<Word>& testWords );

  // Inherited from ARCer:
  //   bool takeControl( );

  Trichotomy contains() const;
  
  
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

  SetOf<Word> theTestWords;
  class SGOfNilpotentGroup* theSubgroup;
  Trichotomy theAnswer;
};

//---------------------------------------------------------------------------//
//---------------------------- SCPinNilpotentQuotients ----------------------//
//---------------------------------------------------------------------------//


class SCPinNilpotentQuotients : public ComputationManager
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SCPinNilpotentQuotients( Supervisor& boss );
  // Constructor gets abstract supervisor, so you can call this
  // computation manager from any supervisor.

  void initialize( const SetOf<Word>& testWords, 
		   const class SMSubgroup* subgroup,
		   int startClass = 2);
  // Initializing for checking of words set.
  // You have to call initialize, before running start, because we
  // need to initialize words and subgroup for running problem.
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy contains( int& nClass ) const;
  // Returns no if there is a word which doesn't lie in the subgroup.

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

  SetOf<Word> theTestWords; 
  // Set of words to be checked.

  const SMSubgroup* theSubgroup;

  Trichotomy theAnswer;

  int currentClass;
  // Class of quotient in which the word problem is solving.

  int upperBound;
  // Upper class for quotients. For nilpotent groups - nilpotency class-1,
  // for others - 10.

  SCPNilpotentARCer arcer;
  bool arcerStarted;
};

// ---------------------- NGcomputeBasisARCer --------------------------- //


class  SGofNGinitPreimageARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGofNGinitPreimageARCer( ComputationManager& boss );

  ~SGofNGinitPreimageARCer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  void setArguments( const NilpotentGroup& NG, const VectorOf<Word>& gens );

  // Inherited from ARCer:
  //   bool takeControl( );

  const SGOfNilpotentGroup& getSubgroup() const;

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

  SGOfNilpotentGroup* theSubgroup;
};


//---------------------------------------------------------------------------//
//------------------------ SGNilpotentQuotients -----------------------------//
//---------------------------------------------------------------------------//


class SGNilpotentQuotients : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGNilpotentQuotients(class SCM& scm, bool internal = true);
  
  ~SGNilpotentQuotients( );

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

  void viewStructure(ostream& ostr) const { }  // overrides SMObject

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

  bool increaseCurrentClass();

  void setNewSubgroup( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SCM& theSCM;

  SMSubgroup& theSubgroup;

  SGofNGinitPreimageARCer arcer;

  int currentClass;
  // This is the class of currently computing quotient.

  int upperBound; 
  // The hard-wired upper bound on the classes we'll compute.
  // For Nilpotent groups it Nilpotency class - 1, for others 10.
  
};


int getNilpotencyUpperBound( const SMFPGroup& G );

#endif
