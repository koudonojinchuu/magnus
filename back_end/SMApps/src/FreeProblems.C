// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes AutEnumerator, FiniteAutEnumerator
//
// Principal Author: Roger Needham, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "FreeProblems.h"
#include "SMFPGroup.h"
#include "SMSubgroup.h"
#include "SMMap.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"
#include <unistd.h>

// ---------------------- AutoInFreeIsFinitARCer --------------------------- //

void AutoInFreeIsFinitARCer::runComputation( )
{
  RandomAutoInFree tempF(theGroup, 1);
  int order;
  retValue = tempF.isFiniteAutomorphism(*theMap,order);
  theOrder = order;
}


void AutoInFreeIsFinitARCer::setArguments( const Map& map,const FreeGroup& group )
{
 theGroup = group;
 theMap = new Map( map );
}


void AutoInFreeIsFinitARCer::writeResults( ostream& out ) 
{
  out < retValue < theOrder;
}

void AutoInFreeIsFinitARCer::readResults( istream& in )
{
  in > retValue > theOrder;
}
//---------------------------------------------------------------------------//
//-------------------- AutoInFreeIsFinit ---------- -------------------------//
//---------------------------------------------------------------------------//


AutoInFreeIsFinite::AutoInFreeIsFinite(const SMMap& hom)
  : Supervisor( true ),
    theHom( hom ),
    arcer( *this )
{
 arcer.setArguments( theHom.getMap(),theHom.getDomain().getFreePreimage()); 
}


void AutoInFreeIsFinite::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (arcer.takeControl()){
      LogMessage msg( *this,theHom);
      Trichotomy answer = arcer.getRetValue();
      if (answer == dontknow){
	msg << "Can't solve problem is "<<Name(theHom)<<" finite.";
      }
      else{
	msg  << Name( theHom ) << " is";
	if (answer == no) msg << " not finite";
	else
	  msg << " finite, and it's order: " << arcer.getOrder() <<".";
      }
      msg.send();
      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}


void AutoInFreeIsFinite::start( ) { }


void AutoInFreeIsFinite::terminate( ) { }


void AutoInFreeIsFinite::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is finite automorphism"),
		  Text("Is") + Name( theHom ) + "of finite order? ",
		  helpID("AutoInFreeIsFinite", theHom.getDomain()),
		  "Is finite"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.done();
}

// ---------------------- SGOfFreeContainsConjugateARCer --------------------------- //

void SGOfFreeContainsConjugateARCer::runComputation( )
{
  retValue = theSubgroup->conjugateInSubgroup(  generators, conjugator );
}


void SGOfFreeContainsConjugateARCer::setArguments( const SGofFreeGroup& subgroup, 
					    const VectorOf<Word>& gens )
{
  theSubgroup = new SGofFreeGroup(subgroup);
  generators = makeSetOf(gens);
}


void SGOfFreeContainsConjugateARCer::writeResults( ostream& out ) 
{
  out < retValue;
  out < conjugator;
}

void SGOfFreeContainsConjugateARCer::readResults( istream& in )
{
  in > retValue;
  in > conjugator;
}
//---------------------------------------------------------------------------//
//-------------------- SGOfFreeContainsConjugate -------------------------//
//---------------------------------------------------------------------------//


SGOfFreeContainsConjugate::SGOfFreeContainsConjugate(const SMSubgroup& S1, const SMSubgroup& S2)
  : Supervisor( true ),
    subgroup1( S1 ),
    subgroup2( S2 ),
    arcer( *this )
{
 arcer.setArguments( subgroup1.getSubgroup(), subgroup2.getSubgroup().generators()); 
}


void SGOfFreeContainsConjugate::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (arcer.takeControl()){
      LogMessage msg( *this,subgroup1);
      Word conjugator;
      if ( arcer.getRetValue( conjugator ))
	{
	  msg << Name( subgroup2 ) << " conjugated by ";
	  subgroup1.getParent().getFPGroup().printWord( msg, conjugator );
	}
      else {
	msg << "No conjugate of " << Name( subgroup2 );
      }
      
      msg << " is contained in " << Name( subgroup1 ) << ".";
      
      msg.send();
      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}


void SGOfFreeContainsConjugate::start( ) { }


void SGOfFreeContainsConjugate::terminate( ) { }


void SGOfFreeContainsConjugate::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Does contain a conjugate"),
		  Text("Does") + Name( subgroup1 ) + "contain a conjugate of"
		  + Name(subgroup2 ),
		  helpID("SGOfFreeContainsConjugate", subgroup1.getParent()),
		  "Cont conj"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.done();
}

// ---------------------- SGOfFreeConjugateToARCer --------------------------- //

void SGOfFreeConjugateToARCer::runComputation( )
{
  retValue = theSubgroup->conjugateTo( generators );
}


void SGOfFreeConjugateToARCer::setArguments( const SGofFreeGroup& subgroup, 
					    const VectorOf<Word>& gens )
{
  theSubgroup = new SGofFreeGroup(subgroup);
  generators = makeSetOf(gens);
}


void SGOfFreeConjugateToARCer::writeResults( ostream& out ) 
{
  out < retValue;
}

void SGOfFreeConjugateToARCer::readResults( istream& in )
{
  in > retValue;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfFreeConjugateTo -------------------------//
//---------------------------------------------------------------------------//


SGOfFreeConjugateTo::SGOfFreeConjugateTo(const SMSubgroup& S1, const SMSubgroup& S2)
  : Supervisor( true ),
    subgroup1( S1 ),
    subgroup2( S2 ),
    arcer( *this )
{
 arcer.setArguments( subgroup1.getSubgroup(), subgroup2.getSubgroup().generators()); 
}


void SGOfFreeConjugateTo::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (arcer.takeControl()){
      LogMessage msg( *this,subgroup1,subgroup2);
      msg << Name( subgroup1 ) << " and " << Name( subgroup2 ) << " are";
      if ( ! arcer.getRetValue())
	msg << " not";
      msg << " conjugate.";
      
      msg.send();
      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}


void SGOfFreeConjugateTo::start( ) { }


void SGOfFreeConjugateTo::terminate( ) { }


void SGOfFreeConjugateTo::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Are conjugate ?"),
		  Text("Are") + Name( subgroup1 ) + "and"
		  + Name(subgroup2 ) + "congugate",
		  helpID("SGOfFreeConjugateTo", subgroup1.getParent()),
		  "Are conjugate"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.done();
}


// ---------------------- AutEnumeratorARCer --------------------------- //


void AutEnumeratorARCer::setArguments( const FreeGroup& group, int avgNumbers, 
                                                                 bool isfinite)
{
  randAuto = new RandomAutoInFree( group, avgNumbers );
  isFinite = isfinite;
}


void AutEnumeratorARCer::runComputation( )
{
#if SAFETY > 0
  if( randAuto == 0 )
    error( "void AutEnumeratorARCer::runComputation( ) : "
	   "The enumerator has not been initialized yet");
#endif
  Map m;
  int order;
  int i = 1;
  while (true){
    if (!isFinite){
      m = randAuto->getAutomorphism();

      #ifdef SYSV
        sleep(1); // @db: don't have usleep under Solaris
      #else    
	usleep(30000);
      #endif   
    }
    else
      m = randAuto->getFiniteAutomorphism(order);
    file << "a" << i << " = ";
    m.printOn(file);
    if (isFinite){
      file.setColor(titleColor);
      file << endl << "The order of a" << i << ": " << order;
      file.setColor(mainColor);
    }
    file << ready << endl << endl;
    i++;
  }
}


void AutEnumeratorARCer::writeResults( ostream& out ) 
{

}


void AutEnumeratorARCer::readResults( istream& in )
{

}


Chars AutEnumeratorARCer::getFileName() 
{
  return file.getFileName();
}

//---------------------------------------------------------------------------//
//--------------------------- AutEnumerator ---------------------------------//
//---------------------------------------------------------------------------//


AutEnumerator::AutEnumerator(SMFPGroup& F, int avgNumGens, bool is_finite)
  : ComputationManager( true ),
	 theGroup( F ),
	 avgNumGens( avgNumGens ),
	 isFinite( is_finite ),
	 isStarted( false ),
         arcer(*this)
{ 
  LogMessage msg(*this,theGroup);
  if (theGroup.gic.isAbelian()==yes && theGroup.getFPGroup().getRelators().cardinality() > 0){
    Message msg;
    msg << " This group is not free abelian, can't built automorphisms.";
    msg.send();
    resultIsFast();
  }else
    if (theGroup.getFreePreimage().numberOfGenerators() < 2){
      LogMessage msg(*this,theGroup);
      if (theGroup.getFreePreimage().numberOfGenerators()==0)
	msg <<  Name(theGroup) << " is trivial, so it has only one automorphism: 1->1.";
      else
	{
	  Chars nameOfGen = theGroup.getFreePreimage().nameOfGenerator(0);
	  msg <<  Name(theGroup) << " with one generator, so it has only two automorphisms: " <<
	    nameOfGen << "->"<<nameOfGen <<" and "<<nameOfGen<<"->"<<nameOfGen<<"^-1.";
	}
      msg.send();
      resultIsFast();
    }
    else{
      arcer.setArguments(theGroup.getFreePreimage(),avgNumGens,isFinite);
      adminStart();
    }
}


AutEnumerator::~AutEnumerator( )
{

}


void AutEnumerator::start( )
{

}


void AutEnumerator::takeControl( )
{
  if (fastResult()){
    adminTerminate();
    return;
  }
  if ( freeARCs() == 0 ) {
    adminSuspend();
    return;
  }
  if (freeARCs() > 0){
    if (!isStarted){
      LogMessage msg(*this,theGroup);
      if (isFinite)
	msg << Link(Chars( "Click here to see the random automorphisms of finite order of ")+
		    Text( Name(theGroup) ),
		    "RandomAutomorphisms",
		    arcer.getFileName(), true);
      else
	msg << Link(Chars( "Click here to see the random automorphisms  of ")+
		    Text( Name(theGroup) ),
		    "RandomAutomorphisms",
		    arcer.getFileName(),true );
      msg.send();        
      isStarted = true;
    }
    
    if (arcer.takeControl()){
      adminTerminate();
      return;
    }
  usedOneARC();
  }
}


void AutEnumerator::terminate( )
{

}


const IconID AutEnumerator::iconID( ) const
{
  return IconID::enumerator_problem;
}


void AutEnumerator::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Enumerate automorphisms"),
		  Text("Randomly enumerate automorphisms of")
		  + Name( theGroup )
		  + ( isFinite ? "of finite order" : "" ),
		  helpID("AutEnumerator", theGroup),
		  "Enum aut"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this enumerator"), ThisARCSlotID(), 50);

  pv.done();
}
