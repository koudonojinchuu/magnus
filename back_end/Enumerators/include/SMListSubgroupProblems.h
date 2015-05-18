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

#ifndef _SMLIST_SUBGROUP_PROBLEMS_H_
#define _SMLIST_SUBGROUP_PROBLEMS_H_


// ----------------------- SMListSGTrivialARCer ------------------ //

class SMListSGTrivialARCer : public ARCer
{
public:
  SMListSGTrivialARCer( ComputationManager& boss );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( SMList<Subgroup>*);

  void runComputation( );
  void writeResults( ostream& o) { o < tCounter; o < ntCounter; o<trivialData;};
  void readResults( istream& i) { i > tCounter; i > ntCounter; i>trivialData;};
  
  Chars getFileOfNonTrivial() const { return fileOfNonTrivial.getFileName(); }
  const SMListData& getTrivialData() const { return trivialData; }
 
private:
  SMListIterator<Subgroup>* theIterator;
  const class SMFPGroup* theGroup;
  class SMList<Subgroup>* theList;
  int tCounter;
  int ntCounter;
  File fileOfNonTrivial;
  SMListData trivialData;
};


// --------------------- SMListSGTrivialComputation  ---------------------- //


class SMListSGTrivialComputation  : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListSGTrivialComputation( class SMListExtractTrivialSubgroups& );
  
  ~SMListSGTrivialComputation(); 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars getFileOfNonTrivial() const { return arcer.getFileOfNonTrivial(); }
  const SMListData& getTrivialData() const { return arcer.getTrivialData(); }

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

  SMList<Subgroup>& theList;

  bool started;
  SMListSGTrivialARCer arcer;

};


// --------------------- SMListExtractTrivialSubgroups -------------------- //


class SMListExtractTrivialSubgroups  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractTrivialSubgroups( SMList<Subgroup>& );
  
  ~SMListExtractTrivialSubgroups() {

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

  SMList<Subgroup>& getList() { return theList; }


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

  SMList<Subgroup>& theList;

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate normalClosure;

  Subordinate<SMListExtractTrivialSubgroups,SMListSGTrivialComputation> checker;
};


// ----------------------- SMListSGAbelianARCer ------------------ //

class SMListSGAbelianARCer : public ARCer
{
public:
  SMListSGAbelianARCer( ComputationManager& boss );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( SMList<Subgroup>*);

  void runComputation( );
  void writeResults( ostream& o ) { o < tCounter; o < ntCounter;};
  void readResults( istream& i ) { i > tCounter; i > ntCounter;};
  Chars getFileOfNonAbelian() const { return fileOfNonAbelian.getFileName(); }
  const SMListData& getAbelianData() const { return abelianData; }
 
private:
  SMListIterator<Subgroup>* theIterator;
  const class SMFPGroup* theGroup;
  class SMList<Subgroup>* theList;
  File fileOfNonAbelian;
  SMListData abelianData;
  int tCounter;
  int ntCounter;
};


// --------------------- SMListSGAbelianComputation  ---------------------- //


class SMListSGAbelianComputation  : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListSGAbelianComputation( class SMListExtractAbelianSubgroups& );
  
  ~SMListSGAbelianComputation(); 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars getFileOfNonAbelian() const { return arcer.getFileOfNonAbelian(); }
  const SMListData& getAbelianData() const { return arcer.getAbelianData(); }

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

  SMList<Subgroup>& theList;

  bool started;
  SMListSGAbelianARCer arcer;
};

// --------------------- SMListExtractAbelianSubgroups -------------------- //


class SMListExtractAbelianSubgroups  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractAbelianSubgroups( SMList<Subgroup>& );
  
  ~SMListExtractAbelianSubgroups() {

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

  SMList<Subgroup>& getList() { return theList; }

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

  SMList<Subgroup>& theList;

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate normalClosure;

  Subordinate<SMListExtractAbelianSubgroups,SMListSGAbelianComputation> checker;
};


// ----------------------- SMListSGCentralARCer ------------------ //

class SMListSGCentralARCer : public ARCer
{
public:
  SMListSGCentralARCer( ComputationManager& boss );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( SMList<Subgroup>*);

  void runComputation( );
  void writeResults( ostream& o ) { o < tCounter; o < ntCounter;};
  void readResults( istream& i ) { i > tCounter; i > ntCounter;};
  Chars getFileOfNonCentral() const { return fileOfNonCentral.getFileName(); }
  const SMListData& getCentralData() const { return centralData; }
 
private:
  SMListIterator<Subgroup>* theIterator;
  const class SMFPGroup* theGroup;
  class SMList<Subgroup>* theList;
  File fileOfNonCentral;
  SMListData centralData;
  int tCounter;
  int ntCounter;
};


// --------------------- SMListSGCentralComputation  ---------------------- //


class SMListSGCentralComputation  : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListSGCentralComputation( class SMListExtractCentralSubgroups& );
  
  ~SMListSGCentralComputation(); 

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

  SMList<Subgroup>& theList;

  bool started;
  SMListSGCentralARCer arcer;
};

// --------------------- SMListExtractCentralSubgroups -------------------- //


class SMListExtractCentralSubgroups  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractCentralSubgroups( SMList<Subgroup>& );
  
  ~SMListExtractCentralSubgroups() {

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

  SMList<Subgroup>& getList() { return theList; }

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

  SMList<Subgroup>& theList;

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate normalClosure;

  Subordinate<SMListExtractCentralSubgroups,SMListSGCentralComputation> checker;

};

#endif
