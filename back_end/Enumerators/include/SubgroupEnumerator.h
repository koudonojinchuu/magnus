#include "Supervisor.h"
#include "SMEnumerator.h"
#include "SMList.h"
#include "Subgroup.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "RandomNumbers.h"
#include "SMListIterator.h" 

#ifndef _SubgroupEnumerator_H_
#define _SubgroupEnumerator_H_

// ------------------ SGEnumeratorARCer ---------------------- //


class SGEnumeratorARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGEnumeratorARCer( ComputationManager& boss,SMListData& d, SMFPGroup& g ) 
    : EnumeratorARCer( boss, d ), 
    group(g),
    numberOfAll(100),
    enumerateAll( true ),
    isNormalRandom( false ),
    numberOfGenerators(1),
    maxLength(100),
    counter(0) { theGroup = g.getFreePreimage(); } 
  
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );
  // Inherited from EnumeratorARCer:
  // void runComputation( );
  
  void setEnumerateAll() { enumerateAll = true; }
  void setEnumerateRandom() { enumerateAll = false; }
  void setNormalRandom() { isNormalRandom = true; }
  void setUniformRandom() { isNormalRandom = false; }
 
  void setTotalNumber(int n) { numberOfAll = n; }
  void setMaxLength(int n) { maxLength = n; }
  void setNumberOfGens( int n) { numberOfGenerators = n; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void writeResults( ostream& ostr ) 
    {
      ostr <  counter;
      ostr < theData;
    }

  void readResults( istream& istr) 
    {
      istr > counter;
      istr > theData;
    }
  
protected:
  void enumerate();
private:

  void enumerateAllSubgroups( );
  Word getRandomWord() ;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& group;
  FreeGroup theGroup;

  bool enumerateAll;
  bool isNormalRandom;
  
  int numberOfGenerators;
  int numberOfAll;
  int maxLength;
  
  int counter;
  UniformRandom ur; 
  NormalRandom nr; 
};

// ----------------------------------------------------------- //

class SGEnumeratorProblem : public EnumeratorProblem< Subgroup >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGEnumeratorProblem(SMFPGroup& );
  
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
  SGEnumeratorARCer arcer;
};


// ---------------------- IsSubgroupInListARCer ------------------------ //


class IsSubgroupInListARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  IsSubgroupInListARCer( ComputationManager& boss ) 
  : ARCer( boss ), result(0) { } 
  
  ~IsSubgroupInListARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Subgroup>&, const VectorOf<Word>&);
  
  // Inherited from ARCer:
  // bool takeControl( );
  
  bool isInList();
  
  
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

  SMListIterator<Subgroup>* theIterator;
  VectorOf<Word> theSubgroup;
  bool result;
};



// --------------------- IsSubgroupInList  ------------------------ //


class IsSubgroupInList  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 IsSubgroupInList( SMList<Subgroup>&, const class SMSubgroup& );


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

  const class SMSubgroup& theSubgroup;
  SMList<Subgroup>& theList;

  IsSubgroupInListARCer arcer;

};


// ---------------------- SGListExtractOfIndexARCer --------------------- //


class SGListExtractOfIndexARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SGListExtractOfIndexARCer( ComputationManager& boss ) 
    : ARCer( boss ),
    minIndex( 1 ), 
    maxIndex( 0 ) { }
  
  ~SGListExtractOfIndexARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Subgroup>&);

  const SMListData& getData() const { return theData; }
  void setMinimalIndex(int  min ) { minIndex = min; }
  void setMaximalIndex(int  max ) { maxIndex = max; }
  int indexInFreeGroup( const VectorOf<Word>& s )const;
  int indexInFPGroup( const VectorOf<Word>& s )const; 
 
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

  SMListIterator<Subgroup>* theIterator;
  SMListData theData;
  const SMFPGroup* theGroup;
  int minIndex; 
  int maxIndex;
};



// --------------------- SGListExtractOfIndex ---------------------- //


class SGListExtractOfIndex  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 SGListExtractOfIndex( SMList<Subgroup>& );


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

  void start( );            // overrides ComputationManager

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList<Subgroup>& theList;
  bool started;
  SGListExtractOfIndexARCer arcer;
};


#endif
