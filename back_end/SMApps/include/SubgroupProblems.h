// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes IsSGAbelian, IsSGCentral, IsSGNilpotent,
//           IsSGTrivial.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _SubgroupProblems_h_
#define _SubgroupProblems_h_


#include "Supervisor.h"
#include "SetOfWordsChecker.h"
#include "CommutatorsChecker.h"
#include "NilpotentProblems.h"
#include "ToddCoxeter.h"
#include "Subgroup.h"
#include "GeneticProblems.h"


// --------------------------- IsSGAbelian --------------------------------- //


class IsSGAbelian : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsSGAbelian( class SMSubgroup& );

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

  void start( ) { }            // overrides ComputationManager

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& theSubgroup;

  SetOfWordsChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsSGAbelian,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate computeBasis;

  Subordinate<IsSGAbelian,NilpotentWP> nilpotentWP;

  Subordinate<IsSGAbelian, GeneticWPCM> genetic;
};


// --------------------------- IsSGCentral --------------------------------- //


class IsSGCentral : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsSGCentral( class SMSubgroup& );

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

  void start( ) { }            // overrides ComputationManager

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& theSubgroup;

  SetOfWordsChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsSGCentral,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate computeBasis;

  Subordinate<IsSGCentral,NilpotentWP> nilpotentWP;

  Subordinate<IsSGCentral, GeneticWPCM> genetic;
};


// ---------------------------- IsSGNilpotent ------------------------------ //


class IsSGNilpotent : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsSGNilpotent( const class SMSubgroup& S, int nilpClass = 2 );


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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& theSubgroup;

  int theClass;

  CommutatorsChecker theChecker;

  CommutatorsCheckerARCer theArcer;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsSGNilpotent,NilpotentWPInQuotients> nilpotentWPInQuotients;

};


// --------------------------- IsSGTrivial --------------------------------- //


class IsSGTrivial : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsSGTrivial(  class SMSubgroup& );

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

  void start( ) { }            // overrides ComputationManager

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& theSubgroup;

  SetOfWordsChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsSGTrivial,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate computeBasis;

  Subordinate<IsSGTrivial,NilpotentWP> nilpotentWP;

  Subordinate<IsSGTrivial, GeneticWPCM> genetic;
};


//---------------------------------------------------------------------------//
//--------------------------- SGIndexProblem --------------------------------//
//---------------------------------------------------------------------------//


class SGIndexProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGIndexProblem(SMSubgroup& subgroup);

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

  SMSubgroup& theSubgroup;
  
  MirrorSubordinate sgToddCoxeter;
  MirrorSubordinate sgGHToddCoxeter;
};


// ---------------------------SGPresentationProblem--------------------------//

class ApproxMethodARCer : public ARCer
{
 public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  ApproxMethodARCer( ComputationManager& boss ) 
    : ARCer( boss ) , sg( FPGroup() ) , tmpOutput( ) 
  { } 
  
  void setArguments( const FPGroup& g , const VectorOf<Word>& v );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getRetValue( );
  Chars tmpOutputFilename();

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

  Subgroup sg;
  File tmpOutput;
  FPGroup result;
};

class ApproxMethod : public ComputationManager
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ApproxMethod( class SGPresentationProblem& sgp );
     
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getResult();
  Chars tmpOutputFilename();  
   
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

  SMSubgroup& theSubgroup;
  FPGroup result;
  ApproxMethodARCer arcer;
};


class TCMethodARCer : public ARCer
{
 public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  TCMethodARCer( ComputationManager& boss ) 
    : ARCer( boss ) , sg( FPGroup() ) , 
      pr( ) , tmpOutput( ) 
  { } 

  void setArguments( const FPGroup& g , const VectorOf<Word>& v ,
		     const PermutationRepresentation& prep );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getRetValue( );
  Chars tmpOutputFilename();
  
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

  Subgroup sg;
  PermutationRepresentation pr;
  File tmpOutput;
  FPGroup result;
};

class TCMethod : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TCMethod( class SGPresentationProblem& sgp );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getResult();
  Chars tmpOutputFilename();
    
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

  SMSubgroup& theSubgroup;
  Bool init;
  FPGroup result;
  TCMethodARCer arcer;
};

class SGPresentationProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGPresentationProblem(SMSubgroup& s);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& getTestSubgroup(); 
  
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

  void start( );               // overrides ComputationManager

  void terminate( );           // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& theSubgroup;
  Bool done;
  Bool term;
  
  Subordinate<SGPresentationProblem,ApproxMethod> am;
  Subordinate<SGPresentationProblem,TCMethod> tcm;
  MirrorSubordinate sgToddCoxeter;
};


//--------------------------------SGRewriteWordProblem--------------------//

class RewriteWordARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  RewriteWordARCer( ComputationManager& boss ) 
    : ARCer( boss ),
      retValue(),
      theWord(),
      F(),
      rank( 0 ),
      tmpOutput(),
      pr(NULL)
    { } 
  
  void setArguments(const PermutationRepresentation&,
		    const FreeGroup&,const Word&);
  
  Chars tmpOutputFilename();
  
  int getRank();
  
  Word getRepresentative();
  
  Word rewrite(const PermutationRepresentation&,const VectorOf<Word>&,
	       const FreeGroup& F,const Word&,bool);
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getRetValue() const{ return retValue; }

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

  Word retValue;
  Word theWord;
  FreeGroup F;
  int rank;
  File tmpOutput;
  PermutationRepresentation* pr;
};

class SGRewriteWordProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGRewriteWordProblem(SMSubgroup& s,SMWord& w);

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

  SMWord& theWord;
  SMSubgroup& theSubgroup;
  bool init;
  
  RewriteWordARCer arcer;
  MirrorSubordinate sgToddCoxeter;
  
};

#endif





