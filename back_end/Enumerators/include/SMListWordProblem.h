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

#ifndef _SMLIST_WORD_PROBLEM_H_
#define _SMLIST_WORD_PROBLEM_H_

// --------------------------- SMListGeneticWPArcer ------------------------ //


class SMListGeneticWPArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMListGeneticWPArcer( ComputationManager& );

  ~SMListGeneticWPArcer( ) { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( SMList<Word>* );

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
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  SMListIterator<Word>* theIterator;
  SMList<Word>* theList;
};



// ---------------------------- SMListGeneticWPCM ------------------------- //


class SMListGeneticWPCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMListGeneticWPCM( class SMListSupervisor& sms );

  //  ~GeneticWPCM( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
   
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

  SMListGeneticWPArcer arcer;
};

// ----------------------- SMListWPCheckARCer ------------------ //

class SMListWPCheckARCer : public ARCer
{
public:
  SMListWPCheckARCer( ComputationManager& boss ) 
    : ARCer( boss ),
      theIterator( NULL ),
      theGroup( NULL ) { } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( SMList<Word>*, const SMFPGroup*  );

  void runComputation( );
  void writeResults( ostream& ) { };
  void readResults( istream& ) { };

private:
  SMListIterator<Word>* theIterator;
  const class SMFPGroup* theGroup;
  class SMList<Word>* theList;
};


// --------------------- SMListWPCheckComputation  ---------------------- //


class SMListWPCheckComputation  : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListWPCheckComputation( SMListSupervisor& );
  
  ~SMListWPCheckComputation(); 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


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

  void start( ) { }

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList<Word>& theList;

  bool started;
  SMListWPCheckARCer arcer;
};

// --------------------------- SMListWPPrinResultArcer --------------------- //


class SMListWPPrinResultArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMListWPPrinResultArcer( ComputationManager& b) :
    ARCer( b ),
    theList( NULL ) { }

  ~SMListWPPrinResultArcer( ) { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( SMList<Word>* );

  Chars getFileOfNonTrivial() const { return fileOfNonTrivial.getFileName(); }
  const SMListData& getTrivialData() const { return trivialData; }



  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
  void writeResults( ostream& o ) { o < trivialData;}

  void readResults( istream& i ) { i > trivialData; }

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  SMList<Word>* theList;
  File fileOfNonTrivial;
  SMListData trivialData;
};

// --------------------- SMListExtractTrivialWords  ---------------------- //


class SMListExtractTrivialWords  : public SMListSupervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractTrivialWords( SMList<Word>& );
  
  ~SMListExtractTrivialWords() {

  }
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

  bool finished( );
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

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate normalClosure;
  Subordinate<SMListExtractTrivialWords, SMListGeneticWPCM> genetic;

  Subordinate<SMListExtractTrivialWords, SMListWPCheckComputation> checker;

  SMListWPPrinResultArcer arcer;

};


// ----------------------- SMListWordIsCentralARCer ------------------ //

class SMListWordIsCentralARCer : public ARCer
{
public:
  SMListWordIsCentralARCer( ComputationManager& boss );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( SMList<Word>*);

  void runComputation( );
  void writeResults( ostream& o ) { 
    o < tCounter; 
    o < ntCounter;
    o < centralData;
  };
  void readResults( istream& i ) { 
    i > tCounter; 
    i > ntCounter;
    i > centralData;
  };

  Chars getFileOfNonCentral() const { return fileOfNonCentral.getFileName(); }
  const SMListData& getCentralData() const { return centralData; }
 
private:
  SMListIterator<Word>* theIterator;
  const class SMFPGroup* theGroup;
  class SMList<Word>* theList;
  File fileOfNonCentral;
  SMListData centralData;
  int tCounter;
  int ntCounter;
};


// --------------------- SMListWordIsCentralComputation -------------------- //


class SMListWordIsCentralComputation  : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListWordIsCentralComputation( class SMListExtractCentralWords& );
  
  ~SMListWordIsCentralComputation(); 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars getFileOfNonCentral() const { return arcer.getFileOfNonCentral(); }
  const SMListData& getCentralData() const { return arcer.getCentralData(); }

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

  void start( ) { }

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList<Word>& theList;

  bool started;
  SMListWordIsCentralARCer arcer;
};

// --------------------- SMListExtractCentralWords -------------------- //


class SMListExtractCentralWords  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractCentralWords( SMList<Word>& );
  
  ~SMListExtractCentralWords() {

  }
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

  SMList<Word>& getList() { return theList; }

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

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate normalClosure;

  Subordinate<SMListExtractCentralWords,SMListWordIsCentralComputation> checker;

};

#endif
