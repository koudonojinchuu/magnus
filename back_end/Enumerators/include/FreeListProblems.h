#include "Supervisor.h"
#include "SMList.h"
#include "File.h"
#include "FPGroup.h"
#include "OutMessages.h"
#include "SMListIterator.h"
#include "ListProblems.h"

#ifndef _FREE_LIST_PROBLEMS_H_
#define _FREE_LIST_PROBLEMS_H_

// ------------------- SMListProperPowerInFreeARCer ------------------ //

class SMListProperPowerInFreeARCer : public ARCer
{
public:
  SMListProperPowerInFreeARCer( ComputationManager& boss ) 
    : ARCer( boss ),
      theIterator( NULL ) { } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( const SMList<Word>& );
  const SMListData& getData() const { return theData; }

  void runComputation( );

  void writeResults( ostream& o ) { o < theData; };
  void readResults( istream& i) { i > theData; };
 
 
private:
  SMListIterator<Word>* theIterator;
  FPGroup theGroup;
  SMListData theData;
};

// --------------------- SMListExtractProperPowerInFree  ------------------ //


class SMListExtractProperPowerInFree  : public SMListSupervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractProperPowerInFree( SMList<Word>& );
  
  ~SMListExtractProperPowerInFree() {

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

  SMListProperPowerInFreeARCer arcer;

};



// ------------------- SMListCommutatorsInFreeARCer ------------------ //

class SMListCommutatorsInFreeARCer : public ARCer
{
public:
  SMListCommutatorsInFreeARCer( ComputationManager& boss ) 
    : ARCer( boss ),
      theIterator( NULL ) { } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );

  void setArguments( const SMList<Word>& );
  const SMListData& getData() const { return theData; }

  void runComputation( );

  void writeResults( ostream& o ) { o < theData; };
  void readResults( istream& i) { i > theData;};
 
 
private:
  SMListIterator<Word>* theIterator;
  FreeGroup theGroup;
  SMListData theData;
};

// --------------------- SMListExtractCommutatorsInFree  ------------------ //


class SMListExtractCommutatorsInFree  : public SMListSupervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListExtractCommutatorsInFree( SMList<Word>& );
  
  ~SMListExtractCommutatorsInFree() {

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

  SMListCommutatorsInFreeARCer arcer;

};
// ---------------------- SGListExtractOfRankARCer --------------------- //


class SGListExtractOfRankARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SGListExtractOfRankARCer( ComputationManager& boss ) 
    : ARCer( boss ),
    minRank( 1 ), 
    maxRank( 0 ) { }
  
  ~SGListExtractOfRankARCer( ) { 
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
  void setMinimalRank(int  min ) { minRank = min; }
  void setMaximalRank(int  max ) { maxRank = max; }
  int indexInFreeGroup( const VectorOf<Word>& s )const;
 
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
  int minRank; 
  int maxRank;
};



// --------------------- SGListExtractOfRank ---------------------- //


class SGListExtractOfRank  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 SGListExtractOfRank( SMList<Subgroup>& );


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
  SGListExtractOfRankARCer arcer;
};

// ---------------------- SGListExtractNormalARCer --------------------- //


class SGListExtractNormalARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SGListExtractNormalARCer( ComputationManager& boss ) 
    : ARCer( boss ) { } 
  
  ~SGListExtractNormalARCer( ) { 
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
  FreeGroup theGroup;
};



// --------------------- SGListExtractNormal ---------------------- //


class SGListExtractNormal  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 SGListExtractNormal( SMList<Subgroup>& );


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
  SGListExtractNormalARCer arcer;
};
// ---------------------- SGListExtractMalnormalARCer --------------------- //


class SGListExtractMalnormalARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SGListExtractMalnormalARCer( ComputationManager& boss ) 
    : ARCer( boss ) { } 
  
  ~SGListExtractMalnormalARCer( ) { 
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
  //void setMinimalLength(int  min ) { minLength = min; }
  //void setMaximalLength(int  max ) { maxLength = max; }
 
 
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
  FreeGroup theGroup;
};



// --------------------- SGListExtractMalnormal ---------------------- //


class SGListExtractMalnormal  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 SGListExtractMalnormal( SMList<Subgroup>& );


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
  SGListExtractMalnormalARCer arcer;
};

// ---------------------- MapListExtractIAautoARCer --------------------- //


class MapListExtractIAautoARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  MapListExtractIAautoARCer( ComputationManager& boss ) 
    : ARCer( boss ) { } 
  
  ~MapListExtractIAautoARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Map>&);

  const SMListData& getData() const { return theData; }
  //void setMinimalLength(int  min ) { minLength = min; }
  //void setMaximalLength(int  max ) { maxLength = max; }
 
 
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

  SMListIterator<Map>* theIterator;
  SMListData theData;
  FreeGroup theGroup;
};



// --------------------- MapListExtractIAauto ---------------------- //


class MapListExtractIAauto  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 MapListExtractIAauto( SMList<Map>& );


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

  SMList<Map>& theList;
  bool started;
  MapListExtractIAautoARCer arcer;
};


// ---------------------- MapListExtractInnerARCer --------------------- //


class MapListExtractInnerARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  MapListExtractInnerARCer( ComputationManager& boss ) 
    : ARCer( boss ) { } 
  
  ~MapListExtractInnerARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Map>&);

  const SMListData& getData() const { return theData; }
  //void setMinimalLength(int  min ) { minLength = min; }
  //void setMaximalLength(int  max ) { maxLength = max; }
 
 
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

  SMListIterator<Map>* theIterator;
  SMListData theData;
  FreeGroup theGroup;
};



// --------------------- MapListExtractInner ---------------------- //


class MapListExtractInner  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 MapListExtractInner( SMList<Map>& );


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

  SMList<Map>& theList;
  bool started;
  MapListExtractInnerARCer arcer;
};

#endif
