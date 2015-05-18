#include "Supervisor.h"
#include "SMEnumerator.h"
#include "File.h"
#include "Word.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "RandomNumbers.h"

#ifndef _REnumerator_H_
#define _REnumerator_H_

// ------------------ NCEnumerateTotalARCer ---------------------- //


class NCEnumerateTotalARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NCEnumerateTotalARCer( ComputationManager& boss, SMListData& d,
			 const FPGroup& group ) 
    : EnumeratorARCer( boss, d ), 
      theGroup(group),
      numberOfAll(100),
      counter(0) { } 
  
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );
  // Inherited from EnumeratorARCer:
  // void runComputation( );
  
  void setTotalNumber(int n) { numberOfAll = n; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void writeResults( ostream& ostr ) 
    {
      ostr < currentWord < counter;
      ostr < theData;
    }

  void readResults( istream& istr) 
    {
      istr > currentWord > counter;
      istr > theData;
    }
  
protected:
  void enumerate();
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup theGroup;

  int numberOfAll;
  Word currentWord;
  int counter;
};

// ------------------ NCEnumerateRandomARCer ---------------------- //


class NCEnumerateRandomARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NCEnumerateRandomARCer( ComputationManager& boss, SMListData& d,
			  const FPGroup& group ) 
    : EnumeratorARCer( boss, d ), 
    theGroup(group),
    numberOfAll(100),
    maxConjLength(100),
    typeIsUniform( true ),
    numberOfFactors( 10 ),
    counter(0) { } 
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );
  
  void setTotalNumber(int n) { numberOfAll = n; }
  void setConjL(int n) { maxConjLength = n; }
  void setNumOfFactors(int n) { numberOfFactors = n; }
  
  void setTypeUniform() { typeIsUniform = true; }
  void setTypeNormal() { typeIsUniform = false; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // void runComputation( );
 
  void writeResults( ostream& o ) { 
    o < counter; 
    o < theData;
  }

  void readResults( istream& i ) { 
    i > counter; 
    i > theData;
  }

protected:
  Word getRelConj();
  void enumerate();
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup theGroup;

  int numberOfAll;
  int maxConjLength;
  bool typeIsUniform;
  int counter;
  int numberOfFactors;
  
  UniformRandom ur; 
  NormalRandom nr; 
};


class NCRelatorEnumerator : public EnumeratorProblem< Word >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NCRelatorEnumerator(SMFPGroup& );
  
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

  void start( ) ;                // overrides ComputationManager  
  void terminate( ) { }          // overrides ComputationManager
  
  void resume( );                // overrides ComputationManager
    
  
   
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  EnumeratorARCer* arcer;
};



#endif
