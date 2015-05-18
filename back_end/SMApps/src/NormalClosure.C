// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of classes NormalClosureARCer, NormalClosure
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
//

#include "NormalClosure.h"
#include "GCM.h"
#include "SMSubgroup.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "Timer.h"
#include "DecomposeInSubgroup.h"

// ------------------------ NormalClosureARCer ----------------------------- //

NormalClosureARCer::NormalClosureARCer( ComputationManager& boss ) 
  : ARCer2( boss ), theDecomposer( 0 ), tempDecomposer( 0 )
{ } 

NormalClosureARCer::~NormalClosureARCer( ) 
{ 
  delete theDecomposer; 
  delete tempDecomposer; 
}


void NormalClosureARCer::setArguments( const FPGroup& G, 
				       const VectorOf<Word>& sgens )
{
  theDecomposer = new DecomposeInSubgroupOfFPGroup( G, sgens );
}


void NormalClosureARCer::runComputation( )
{
  Timer t(delay()*1000);
  while( !t.expired() ) {
    theDecomposer->expandGraph();
  }
}


void NormalClosureARCer::writeResults( ostream& out ) 
{
  out < *theDecomposer;
}


bool NormalClosureARCer::readResults2( istream& in )
{
  if( tempDecomposer == 0 ) {
    tempDecomposer = 
      new DecomposeInSubgroupOfFPGroup( FPGroup(), VectorOf<Word>() );
  }

  Timer timer(1000);
  bool ret = tempDecomposer->readPiece(in,timer);
  
  if( ret ) {
    delete theDecomposer;
    theDecomposer = tempDecomposer;
    tempDecomposer = 0;
  }

  return ret;
}


// ---------------------------- NormalClosure ------------------------------ //


NormalClosure::NormalClosure(GCM& gcm)
  : arcer(*this),
    theCM( gcm ),
    theGroup( gcm.getSMFPGroup() ),
    theObject( gcm.getSMFPGroup() )
{
  adminStart();
  arcer.setArguments( theGroup.getFPGroup(), VectorOf<Word>() );
}


NormalClosure::NormalClosure(SCM& scm)
  : arcer(*this),
    theCM( scm ),
    theGroup( scm.getSubgroup().getParent() ),
    theObject( scm.getSubgroup() )
{
  adminStart();
  arcer.setArguments( theGroup.getFPGroup(), 
		      scm.getSubgroup().getSubgroup().generators() 
		      );
}

DecomposeInSubgroupOfFPGroup& NormalClosure::getDecomposer( )
{
  return arcer.getDecomposer();
}

SubgroupGraph NormalClosure::getSubgroupGraph( ) const 
{ 
  return arcer.getDecomposer().graph(); 
}

Trichotomy NormalClosure::isTrivial(const Word& w) const
{
  DecomposeInSubgroupOfFPGroup& D = arcer.getDecomposer();

  if ( D.graph().isComplete() )
    return D.contains(w);
  else
    return ( D.contains(w) ? yes : dontknow );
}


void NormalClosure::takeControl( )
{
  if ( freeARCs() > 0 ) {
    
    if( arcer.takeControl() ) {
      
      DecomposeInSubgroupOfFPGroup& D = arcer.getDecomposer();

      putHaveWordDecomposer( D );

      LogMessage msg( theObject ); //.oid(), 2 );
      if( theObject.typeID() == SMFPGroup::type() ) {
	msg << "The trivial word acceptor has ";
      }
      else {
	msg << "The word acceptor has ";
      }
      msg << D.graph().vertexCount() << " vertices.";
      msg.send();
      
      if ( D.graph().isComplete() ) {
	putHaveCompleteCayleyGraph( D.graph() );
	adminTerminate();
      }
      
    }
    
    usedOneARC();
  }
}

void NormalClosure::putHaveWordDecomposer( 
  const DecomposeInSubgroupOfFPGroup& D )
{
  if( theObject.typeID() == SMFPGroup::type() ) {
    ((SMFPGroup &)theObject).gic.putHaveWordDecomposer( D );
  }
  else {
    ((SMSubgroup &)theObject).sic.putHaveWordDecomposer( D );
  }
}


void NormalClosure::putHaveCompleteCayleyGraph( 
  const SubgroupGraph& S )
{
  if( theObject.typeID() == SMFPGroup::type() ) {
    ((SMFPGroup &)theObject).gic.putHaveCompleteCayleyGraph( S );
  }
  else {
    ((SMSubgroup &)theObject).sic.putHaveCompleteCayleyGraph( S );
  }
}
