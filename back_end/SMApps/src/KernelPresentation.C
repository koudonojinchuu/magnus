// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPNewPresentationARCer, 
//           FPNewPresentationProblem, FPImagePresentationARCer, 
//           FPImagePresentationCM, FPKernelPresentationARCer,
//           FPKernelPresentationCM, KBSupervisorCM,
//           FPKernelPresentationProblem
//           classes.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//

#include "KernelPresentation.h"
#include "Supervisor.h"
#include "FEData.h"
#include "Word.h"
#include "FPGroup.h"
#include "CosetEnumerator.h"
#include "PresentationProblems.h"
#include "Chars.h"
#include "OutMessages.h"


//---------------------------FPNewPresentationARCer--------------------------//

void FPNewPresentationARCer::setArguments( const KBMachine& kb , 
					   const FPGroup& g , 
					   const VectorOf<Word>& y )
{
  G = g;
  newGens = y;
  N = NewPresentation( g , kb , y );
}

void FPNewPresentationARCer::runComputation( )
{
  CosetEnumerator ce( G , newGens );
  int ind = ce.enumerate();
  
  if( ind != 1 )
    {
      dntg = true;
      return;
    }
  
  retValue = N.getNewPresentation( );
}
 
void FPNewPresentationARCer::writeResults( ostream& out )
{
  out < retValue;
}

void FPNewPresentationARCer::readResults( istream& in )
{
  in > retValue;
}


// -----------------------FPNewPresentationProblem-------------------------- //

FPNewPresentationProblem::FPNewPresentationProblem( class SMVectorOfWords& v )
  : Supervisor( true ), 
    theVector( v ),
    init( false ),
    kbSupervisor( *this , theVector.getParent().gcm().kbSupervisor ),
    arcer( *this )
{ 
  //adminStart();
}

void FPNewPresentationProblem::takeControl( )
{
  if( freeARCs() > 0 && theVector.getParent().gic.haveConfluentKBMachine() )
    {
      if( !init )
	{
	  arcer.setArguments( theVector.getParent().gic.getConfluentKBMachine(),
			      theVector.getParent().getFPGroup(),
			      theVector.getWords() );
	  init = true;
	}
       
      if(arcer.takeControl())
	{
	  LogMessage msg( *this , theVector );
	  
	  if( arcer.doesntGenerate() )
	    msg << Name( theVector ) << " doesn't generate " 
		<< Name( theVector.getParent() ) << " . ";
	  else
	    {
	      /*  
		  msg << " New presentation for " << Name( theVector.getParent() )
		  << " on new generators " << Name( theVector ) << " : ";
		  msg << arcer.getRetValue();
	      */
	      SMFPGroup* smo = new SMFPGroup( arcer.getRetValue() , Text("") + Name( theVector.getParent() ) + Text("presented on generators given by") + Name( theVector ) );
	      
	      ListOf<OID> dependencies( theVector.getParent() );
	      CheckinMessage( *smo, "", dependencies ).send();
	    }
	  
	  msg.send();
	  
	  adminTerminate();
	  return;
	}
      else
	usedOneARC();
    }
}

void FPNewPresentationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("New presentation problem"),
		  Text("Compute a presentation for") + Name( theVector.getParent() )
		  + Text("on generators given by") + Name( theVector ),
		  helpID("FPNewPresentationProblem", theVector.getParent()),
		  Text("NewPres") + Name( theVector )
		  );  
  
  pv.startItemGroup();
  
  pv.add(Text("Seek a rewriting system for") + Name( theVector.getParent() ),
	 kbSupervisor.arcSlotID(),
	 50
	 );

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);  
  
  pv.done();
}


//-------------------------FPImagePresentationARCer-------------------------//

void FPImagePresentationARCer::setArguments( const FPGroup& preimage, 
					     const FPGroup& image, 
					     const VectorOf<Word>& y )
{
  Im = ImageOfHom( preimage , image , y );
}
  
void FPImagePresentationARCer::runComputation( )
{
  retValue = Im.getImageOfHomo( newImages );
}

void FPImagePresentationARCer::writeResults( ostream& out )
{
  out < retValue < newImages;
}

void FPImagePresentationARCer::readResults( istream& in )
{
  in > retValue;
  in > newImages;
}


//---------------------------FPImagePresentationCM---------------------------//

FPImagePresentationCM::FPImagePresentationCM( class FPKernelPresentationProblem& kpp )
  : ComputationManager( false ), 
    theMap( kpp.getInitialHomo() ),
    G(),
    im(),
    arcer( *this )
{
  arcer.setArguments( theMap.getDomain().getFPGroup(),
		      theMap.getRange().getFPGroup(),
		      theMap.getMap().generatingImages() );  
  adminStart();
}

void FPImagePresentationCM::takeControl( )
{
  if ( freeARCs() > 0 )
    if (arcer.takeControl())
      {
	G = arcer.getRetValue();
	im = arcer.getVectorOfImages();
	
	adminTerminate();
	return;
      }
    else
      usedOneARC();
}


//-------------------------FPKernelPresentationARCer-------------------------//

void FPKernelPresentationARCer::setArguments( const KBMachine& kb , 
					      const FPGroup& preimage ,
					      const FPGroup& image ,
					      const VectorOf<Word>& y )
{
  K = KernelOfHom( preimage , image , kb , y );
}
  
void FPKernelPresentationARCer::runComputation( )
{
  retValue = K.getKernelPresentation( );
}
 
void FPKernelPresentationARCer::writeResults( ostream& out )
{
  out < retValue;
}

void FPKernelPresentationARCer::readResults( istream& in )
{
  in > retValue;
}


//---------------------------FPKernelPresentationCM--------------------------//

FPKernelPresentationCM::FPKernelPresentationCM( class FPKernelPresentationProblem& kpp )
  : ComputationManager( false ), 
    KPP( kpp ),
    result(),
    flag( false ),
    init( false ),
    arcer( *this )
{
  adminStart();
}

void FPKernelPresentationCM::takeControl( )
{
  if( flag )
    {
      if( !init )
	{
	  arcer.setArguments( KPP.getHomo().getRange().gic.getConfluentKBMachine(), 
			      KPP.getHomo().getDomain().getFPGroup(),
			      KPP.getHomo().getRange().getFPGroup(),
			      KPP.getHomo().getMap().generatingImages() );
	  init = true;
	}
      
      if ( freeARCs() > 0 )
	if (arcer.takeControl())
	  {
	    result = arcer.getRetValue();
	    
	    adminTerminate();
	    return;
	  }
	else
	  usedOneARC();
    }
}


//-----------------------------KBSupervisor----------------------------------//

KBSupervisorCM::KBSupervisorCM( class FPKernelPresentationProblem& kpp )
  : ComputationManager( false ), 
    KPP( kpp ),
    flag( false ),
    init( false ),
    arcer( *this )
{
  adminStart();
}

void KBSupervisorCM::takeControl( )
{
  if( flag )
    {
      if( !init )
	{
	  arcer.setArguments( KPP.getHomo().getRange().getFPGroup() );
	  init = true;
	}
      
      if ( freeARCs() > 0 )
	if (arcer.takeControl())
	  {
	    KPP.getHomo().getRange().gic.putHaveConfluentKBMachine( arcer.getKBMachine() );
	    
	    adminTerminate();
	    return;
	  }
	else
	  usedOneARC();
    }
}


// -----------------------FPKernelPresentationProblem----------------------- //

FPKernelPresentationProblem::FPKernelPresentationProblem( class SMHomomorphism2& m )
  : Supervisor( true ), 
    theMap( m ),
    theMap2( 0 ),
    kernel( *this ),
    image( *this ),
    kbs( *this ),
    foundImage( false ),
    foundKB( false ) 
{
  if ( ! displayInFE() ) adminStart();
}
  
void FPKernelPresentationProblem::takeControl( )
{
  if( image->state() == ComputationManager::TERMINATED && !foundImage )
    {
      LogMessage msg1( *this , theMap.getRange() );
	    
      msg1 << "Found the image of " << Name( theMap.getDomain() )
	  << " in " << Name( theMap.getRange() ) << " under " 
	  << Name( theMap ) << " . ";
      msg1.send();
      
      if( image->getImage() == theMap.getRange().getFPGroup() )
	{
	  LogMessage msg2( *this, theMap.getRange() );
	  
	  msg2 << "It coinsides with " << Name( theMap.getRange() ) << " . ";
	  msg2.send();
	
	  theMap2 = &theMap;
	}
      else
	{
	  FPGroup G = image->getImage();
	  VectorOf<Chars> genNames( G.numberOfGenerators() );
	  
	  for( int i1 = 0 ; i1 < genNames.length() ; i1++ )
	    genNames[i1] = Chars( "x" ) + Chars( i1 + 1 );
	  
	  FPGroup newG( genNames , G.getRelators() );
	  
	  SMFPGroup* smo1 = new SMFPGroup( newG , Text("The image of") + Name( theMap.getDomain() ) + " in " + Name( theMap.getRange() ) );
	  
	  ListOf<OID> dependencies1( theMap.getRange() );
	  CheckinMessage( *smo1, "", dependencies1 ).send();
	  
	  Map m( theMap.getDomain().getFPGroup() , image->getImage() ,
		 image->getVectorOfImages() );
	  
	  SMHomomorphism2* smo2 = new SMHomomorphism2( theMap.getDomain(), *smo1, m , Text("The restriction of") + Name( theMap ) + " to " + Name( *smo1 ) );
	  
	  ListOf<OID> dependencies2;
	  dependencies2.insert( theMap.getDomain().oid() , 0 );
	  dependencies2.insert( (*smo1).oid() , 0 );
	  CheckinMessage( *smo2, "", dependencies2 ).send();
	  
	  theMap2 = smo2;
	}
      
      foundImage = true;
      kbs->setFlag( true );
      
      //adminSuspend();
    }
  
  if( foundImage && theMap2->getRange().gic.haveConfluentKBMachine() &&
      !foundKB )
    {
      kernel->setFlag( true );
      foundKB = true;
    }
  
  if( kernel->state() == ComputationManager::TERMINATED )
    {
      /*
	LogMessage msg3( *this , theMap2.getRange() );
	
	msg3 << " Presentation for the kernel of " << Name( theMap2 )
	<< " : " << kernel.getResult();
	
	msg3.send();
      */
      
      SMFPGroup* smo3 = new SMFPGroup( kernel->getResult() , Text("The kernel of") + Name( *theMap2 ) );
      
      ListOf<OID> dependencies3( theMap2->getDomain() );
      CheckinMessage( *smo3, "", dependencies3 ).send();
      
      adminTerminate();
      return;
    }
}

void FPKernelPresentationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Kernel presentation problem"),
		  Text("Compute a presentation for the kernel of") + Name( theMap ),
		  helpID("FPKernelPresentationProblem", theMap.getDomain()),
		  Text("KernelPresentation") + Name( theMap )
		  );  
  
  pv.startItemGroup();
 
  pv.add( Text("Computing the image of ") + Name( theMap.getDomain() ) , 
	  image.arcSlotID(),
	  50 
	  );
  
  pv.add(Text("Seek a rewriting system for the image of ") + Name( theMap.getDomain() ) + " in " + Name( theMap.getRange() ),
	 kbs.arcSlotID(),
	 50
	 );

  pv.add( Text("Computing the kernel of ") + Name( theMap ) , 
	  kernel.arcSlotID(),
	  50 
	  );
  
  pv.done();
}


  
