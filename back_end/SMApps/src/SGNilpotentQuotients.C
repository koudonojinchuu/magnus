// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes 
//            
//            
//            

// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#include "SGNilpotentQuotients.h"
//#include "CommutatorIterator.h"
#include "SMFPGroup.h"
#include "SMSubgroup.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"

// ---------------------- SCPNilpotentARCer --------------------------- //
SCPNilpotentARCer::SCPNilpotentARCer( ComputationManager& boss ) 
  : ARCer( boss ), 
    theSubgroup(0), 
    theAnswer( dontknow ) 
{ 
} 

SCPNilpotentARCer::~SCPNilpotentARCer( ) 
{
  delete theSubgroup; 
} 


void SCPNilpotentARCer::setArguments( const SGOfNilpotentGroup& subgroup,
				      const SetOf<Word>& testWords )
{
  if( theSubgroup )
    delete theSubgroup;
  theSubgroup = new SGOfNilpotentGroup(subgroup);
  theTestWords = testWords;
}


void SCPNilpotentARCer::runComputation( )
{
#if SAFETY > 0
  if( theSubgroup == 0 )
    error( " NilpotentSCPARCer::runComputation() : "
	   "The subgroup containment problem has not been started yet");
#endif

  if( !theSubgroup->preimageIsInitialized() )
    error("theSubgroup.preimageIsInitialized() is false.");

  SetIterator<Word> I( theTestWords );

  for( I.reset(); !I.done(); I.next() ) {
    if( !theSubgroup->contains(I.value()) ) {
      theAnswer = false;
      return;
    }
  }

  theAnswer = true;
}


void SCPNilpotentARCer::writeResults( ostream& out ) 
{
  out < theAnswer;
}


void SCPNilpotentARCer::readResults( istream& in )
{
  in > theAnswer;
}


Trichotomy SCPNilpotentARCer::contains() const
{
#if SAFETY > 0
  if( theSubgroup == 0 )
    error( "bool SCPNilpotentSCPARCer::contains() const : "
	   "The containment problem has not been computed yet");
#endif
  return theAnswer;
}


//---------------------------------------------------------------------------//
//---------------------------- NilpotentSCPInQuotients -----------------------//
//---------------------------------------------------------------------------//


SCPinNilpotentQuotients::SCPinNilpotentQuotients( Supervisor& boss )
: 
  theSubgroup( 0 ),
  theAnswer( dontknow ),
  currentClass(2),
  upperBound( -1 ), // delayed initialization
  arcer( *this ),
  arcerStarted( false )
{

}

int getNilpotencyUpperBound( const SMFPGroup& G )
{
  int upperBound = 10;
  if( G.gic.isNilpotent() == yes || G.gic.isFreeNilpotent() == yes )
    upperBound = G.gic.getNilpotentcyClass() - 1;
  return upperBound;
}


void SCPinNilpotentQuotients::initialize( const SetOf<Word>& testWords, 
					  const SMSubgroup* subgroup, 
					  int startClass)
{
  theTestWords = testWords;
  theSubgroup = subgroup;
  currentClass = startClass;
  upperBound = getNilpotencyUpperBound( subgroup->getParent() );

  adminStart();
}

Trichotomy SCPinNilpotentQuotients::contains( int& nClass ) const
{
  if( theAnswer != dontknow )
    nClass = currentClass;
  return theAnswer;
}

void SCPinNilpotentQuotients::takeControl( )
{
  if( freeARCs() > 0 ) {

    if( currentClass > upperBound ){
      adminTerminate();
      return;
    }   
    if( !arcerStarted 
	&& theSubgroup->sic.haveSGNilpotentQuotInited(currentClass) ) {
      arcer.setArguments( 
        theSubgroup->sic.getSGNilpotentQuotInited(currentClass), theTestWords
      );
      arcerStarted = true;
    }
    
    if( arcerStarted ) {
      if( arcer.takeControl() ){
	if( !arcer.contains() ){
	  theAnswer = no;
	  adminTerminate();
	  return;
	}
	currentClass++;
	arcerStarted = false;
      }
      usedOneARC();
    }

  } // if( freeARCs() > 0 )
}


// ---------------------- SGofNGinitPreimageARCer ------------------------- //

SGofNGinitPreimageARCer::SGofNGinitPreimageARCer( ComputationManager& boss )
  : ARCer( boss ), theSubgroup( 0 ) 
{ 
} 

SGofNGinitPreimageARCer::~SGofNGinitPreimageARCer( )
{
  if (theSubgroup) 
    delete theSubgroup; 
} 


void SGofNGinitPreimageARCer::setArguments( const NilpotentGroup& NG, 
					    const VectorOf<Word>& gens )
{
  if( theSubgroup )
    delete theSubgroup;
  // This ARCer can be called for several times, so we have to delete
  // init, if it exists.

  theSubgroup =  new SGOfNilpotentGroup( NG, gens );
}


void SGofNGinitPreimageARCer::runComputation( )
{
#if SAFETY > 0
  if( theSubgroup == 0 )
    error( "void SGofNGinitPreimageARCer::runComputation( ): "
	   "The subgroup in quotient has not been initialized");
#endif
  //  if( !theSubgroup->preimageIsInitialized() )
  theSubgroup->initPreimage();
}


void SGofNGinitPreimageARCer::writeResults( ostream& out ) 
{
  out < *theSubgroup;
}


void SGofNGinitPreimageARCer::readResults( istream& in )
{
  in > *theSubgroup;
}


const SGOfNilpotentGroup& SGofNGinitPreimageARCer::getSubgroup() const
{
#if SAFETY > 0
  if( theSubgroup == 0 )
    error( "SGOfNilpotentGroup& SGofNGinitPreimageARCer::getGroup() : "
	   "The quotient has not been computed yet");
#endif
  
  return *theSubgroup;
}


//---------------------------------------------------------------------------//
//------------------------ SGNilpotentQuotients -----------------------------//
//---------------------------------------------------------------------------//


SGNilpotentQuotients::SGNilpotentQuotients( class SCM& scm, bool internal )
  : ComputationManager( !internal ),
    theSCM( scm ),
    theSubgroup( scm.getSubgroup() ),
    arcer(*this),
    currentClass( 2 )
{
  upperBound = getNilpotencyUpperBound( theSubgroup.getParent() );
  
  if ( increaseCurrentClass() )
    setNewSubgroup();
  // Looking for computed quotients.

  adminStart();
}

void SGNilpotentQuotients::setNewSubgroup( ) 
{
  SMFPGroup& theGroup = theSubgroup.getParent();
  if( theGroup.gic.haveNilpotentQuotInited( currentClass ) ) {
    arcer.setArguments( theGroup.gic.getNilpotentQuotInited( currentClass ),
			theSCM.getSubgroup().getSubgroup().generators());
  }
  else {
    const FPGroup& G = theGroup.getFPGroup();
    NilpotentGroup NG( G.namesOfGenerators(), 
		       currentClass, //theGroup.gic.getNilpotentcyClass(),
		       makeVectorOf( G.getRelators() ) );
    
    arcer.setArguments( NG, theSubgroup.getSubgroup().generators() );
  }
}

SGNilpotentQuotients::~SGNilpotentQuotients( )
{
}


void SGNilpotentQuotients::takeControl( )
{
  if( freeARCs() > 0 ) {
    if( currentClass > upperBound ){
      adminTerminate();
      return;
    }
    
    if( arcer.takeControl() ) {
      theSubgroup.sic.putHaveSGNilpotentQuotInited( arcer.getSubgroup(), 
						    currentClass );
      LogMessage msg( *this,theSubgroup );
      msg << "Computed the subgroup " 
	  << " in the nilpotent quotient of class " << currentClass
	  << " of " << Name( theSubgroup.getParent() ) << ".";
      msg.send();     

      currentClass++;
      if( !increaseCurrentClass() ){
	adminTerminate();
	return;
      }

      // Compute preimage for next nilpotency class.
      setNewSubgroup();
    } 
    usedOneARC();
  }
}


void SGNilpotentQuotients::start( )
{

}


void SGNilpotentQuotients::terminate( ) { }

bool SGNilpotentQuotients::increaseCurrentClass()
{
  while( currentClass <= upperBound && 
	 theSubgroup.sic.haveSGNilpotentQuotInited( currentClass ) )
    currentClass++;     
  return !(currentClass > upperBound);
}

