#include "Supervisor.h"
#include "SMEnumerator.h"
#include "SMList.h"
#include "File.h"
#include "Word.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "RandomNumbers.h"
#include "ListProblems.h"
#include "SMListIterator.h"

#ifndef _WEnumerator_H_
#define _WEnumerator_H_

// ------------------ WordEnumeratorARCer ---------------------- //


class WordEnumeratorARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordEnumeratorARCer( ComputationManager& boss, SMListData& d, SMFPGroup& g ) 
    : EnumeratorARCer( boss, d ), 
    group(g),
    numberOfAll(100),
    enumerateAll( true ),
    isNormalRandom( false ),
    numberOfsampleGenerators(1),
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
  void setSample(Word w) { 
    theSample = w; 
    numberOfsampleGenerators = ord(w.maxOccurringGenerator());
  }
  
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

  void enumerateAllWords( );
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
  
  int numberOfsampleGenerators;
  int numberOfAll;
  int maxLength;
  
  int counter;
  Word theSample;
  UniformRandom ur; 
  NormalRandom nr; 
};

// ----------------------------------------------------------- //

class WordEnumeratorProblem : public EnumeratorProblem< Word >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordEnumeratorProblem(SMFPGroup& );
  
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
  WordEnumeratorARCer arcer;
};

// ---------------------- IsWordInListARCer ------------------------ //

class IsWordInListARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  IsWordInListARCer( ComputationManager& boss ) 
  : ARCer( boss ), result(0) { } 
  
  ~IsWordInListARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Word>&, const Word&);
  
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

  SMListIterator<Word>* theIterator;
  Word theWord;
  bool result;
};



// --------------------- IsWordInList  ------------------------ //


class IsWordInList  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 IsWordInList( SMList<Word>&, const class SMWord& );


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

  const class SMWord& theWord;
  SMList<Word>& theList;

  IsWordInListARCer arcer;

};

// ---------------------- WordsOfLengthARCer ------------------------ //


class WordsOfLengthARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  WordsOfLengthARCer( ComputationManager& boss ) 
    : ARCer( boss ), 
    minLength( 1 ),
    maxLength( 30000 ) { } 
  
  ~WordsOfLengthARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Word>&);

  const SMListData& getData() const { return theData; }
  void setMinimalLength(int  min ) { minLength = min; }
  void setMaximalLength(int  max ) { maxLength = max; }
 
 
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

  SMListIterator<Word>* theIterator;
  int minLength;
  int maxLength;
  SMListData theData;
  FPGroup theGroup;
};



// --------------------- WordsOfLength  ------------------------ //


class WordsOfLength  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 WordsOfLength( SMList<Word>& );


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

  void start( ); 

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList<Word>& theList;
  bool started;
  WordsOfLengthARCer arcer;
};

#endif
