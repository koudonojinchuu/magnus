#include "Supervisor.h"
#include "SMEnumerator.h"
#include "SMList.h"
#include "SMListIterator.h"
#include "RandomAutoInFree.h"
#include "File.h"
#include "Word.h"
#include "SMFPGroup.h"
#include "fastProblems.h"
#include "OutMessages.h"

#ifndef _AutoEnumerator_H_
#define _AutoEnumerator_H_




// ---------------------- AutEnumeratorARCer --------------------------- //


class AutEnumeratorARCer2 : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutEnumeratorARCer2( ComputationManager& boss, SMListData& d ) 
    : EnumeratorARCer( boss, d ), 
    randAuto( 0 ), 
    current( 0 ) { }

  ~AutEnumeratorARCer2( ) { delete randAuto; } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( FreeGroup group, int avgNumbers, int n);

  // Inherited from ARCer:
  // bool takeControl( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void enumerate( );
 
  void writeResults( ostream& );

  void readResults( istream& );

  bool isAbelian, isFinite;

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int current;
  RandomAutoInFree* randAuto;
  int number;
};


class AutoEnumeratorProblem : public EnumeratorProblem< Map >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AutoEnumeratorProblem(SMFPGroup& );
  
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

  Chars getDataFileName() const { return theData.getDataFileName(); }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) {
    LogMessage msg(*this,theGroup);
    if (theGroup.gic.isAbelian()==yes && 
	theGroup.getFPGroup().getRelators().cardinality() > 0){
      msg << " This abelian group is not free, it is difficult to built automorphisms.";
      msg.send();
//      resultIsFast();
    } 
    else if (theGroup.getFreePreimage().numberOfGenerators() < 2) {
      if (theGroup.getFreePreimage().numberOfGenerators()==0)
	msg <<  Name(theGroup) << " is trivial, so it has only one automorphism: 1->1.";
      else {
	Chars nameOfGen = theGroup.getFreePreimage().nameOfGenerator(0);
	msg <<  Name(theGroup) 
	    << " with one generator, so it has only two automorphisms: " 
	    << nameOfGen << "->"<< nameOfGen << " and " << nameOfGen 
	    << "->" <<nameOfGen << "^-1.";
      }
      msg.send();
//      resultIsFast();
    }
    else {
      int num = getParameters().getValue("P1");
      int avgNumGens = getParameters().getValue("P2");
      arcer.setArguments(theGroup.getFreePreimage(), avgNumGens, num);
      msg << Link(Chars( "Click here to see the random automorphisms  of ")+
		  Text( Name(theGroup) ), "RandomAutomorphisms",
		  theData.getDataFileName(),true );
      msg.send();        
    }
  }

  void terminate( ) {  }        // overrides ComputationManager

  void resume( ) {              // overrides ComputationManager
    arcer.submitSignal();
    int num = getParameters().getValue("P1");
    int avgNumGens = getParameters().getValue("P2");
    arcer.setArguments(theGroup.getFreePreimage(),avgNumGens,num);
  }
   
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  AutEnumeratorARCer2 arcer;
};


// ---------------------  FiniteAutoEnumeratorProblem -------------------

class FiniteAutoEnumeratorProblem : public EnumeratorProblem< Map >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FiniteAutoEnumeratorProblem(SMFPGroup& );
  
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

  
  Chars getDataFileName() const { return theData.getDataFileName(); }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) {   
    int num = getParameters().getValue("P1");
    int avgNumGens = getParameters().getValue("P2");
    arcer.setArguments(theGroup.getFreePreimage(),avgNumGens,num);
    LogMessage msg(*this,theGroup);
    if (theGroup.gic.isAbelian()==yes && 
	theGroup.getFPGroup().getRelators().cardinality() > 0){
      msg << " This abelian group is not free, it is difficult to built automorphisms.";
      msg.send();
//      resultIsFast();
    }
    else if (theGroup.getFreePreimage().numberOfGenerators() < 2){
      if (theGroup.getFreePreimage().numberOfGenerators()==0)
	msg <<  Name(theGroup) << " is trivial, so it has only one automorphism: 1->1.";
      else {
	Chars nameOfGen = theGroup.getFreePreimage().nameOfGenerator(0);
	msg <<  Name(theGroup) 
	    << " with one generator, so it has only two automorphisms: " 
	    << nameOfGen << "->" << nameOfGen << " and " << nameOfGen 
	    << "->" << nameOfGen << "^-1.";
      }
      msg.send();
//      resultIsFast();
    }
    else {
      msg << Link(Chars( "Click here to see the random automorphisms  of ")+
		  Text( Name(theGroup) ) + Chars(" of finite order."),
		  "RandomAutomorphisms", theData.getDataFileName(),true );
      msg.send();        
    }
  }

  void terminate( ) {  }        // overrides ComputationManager

  void resume( ) {              // overrides ComputationManager
    arcer.submitSignal();
    int num = getParameters().getValue("P1");
    int avgNumGens = getParameters().getValue("P2");
    arcer.setArguments(theGroup.getFreePreimage(),avgNumGens,num);
  }
   
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  AutEnumeratorARCer2 arcer;
};

// ---------------------- IsMapInListARCer ------------------------ //


class IsMapInListARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  IsMapInListARCer( ComputationManager& boss ) 
  : ARCer( boss ), result(0) { } 
  
  ~IsMapInListARCer( ) { 
    if (theIterator)
      delete theIterator;
  }
 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<Map>&, const Map&);
  
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

  SMListIterator<Map>* theIterator;
  Map theMap;
  bool result;
};



// --------------------- IsMapInList  ------------------------ //


class IsMapInList  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 IsMapInList( SMList<Map>&, const class SMMap& );


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

  const class SMMap& theMap;
  SMList<Map>& theList;

  IsMapInListARCer arcer;

};

#endif
