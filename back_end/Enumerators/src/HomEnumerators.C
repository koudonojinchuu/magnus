// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the HomEnumeratorARCer1, RandHomEnumeratorProblem
//           HomEnumeratorARCer2 TotalHomEnumeratorProble classes.
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


#include "HomEnumerators.h"
#include "SMEnumerator.h"
#include "SMListIterator.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "SMMap.h"
#include "viewStructure.h"
#include <signal.h>


// ------------------------ HomEnumeratorARCer1 ---------------------------- //


void HomEnumeratorARCer1::setArguments( FPGroup g , FPGroup h ,
					int avgNumbers , int n )
{
  number = n;
  G = g;
  H = h;
  avgLength = avgNumbers;
}

Map HomEnumeratorARCer1::getMap()
{
  int HRank = H.numberOfGenerators();
  int GRank = G.numberOfGenerators();
  
  VectorOf<Word> images(GRank);
  
  for( int i1 = 0 ; i1 < images.length() ; i1++ )
    {
      int length;
      
      do {
	length = lengthPicker.rand(avgLength, avgLength >> 1);
      } while ( length < 1 );
      
      Word w;
      
      for( int j = 0 ; j < length ; j++ )
	while( true )
	  {
	    int gen = genPicker.rand( 1 , 2 * HRank );
	    
	    Word tmp;
	    if( gen % 2 )
	      tmp = Word( Generator( gen / 2 + 1 ) );
	    else
	      tmp = Word( Generator( gen / 2 ) ).inverse(); 
	    
	    if( (w * tmp).freelyReduce().length() > w.length() )
	      {
		w *= tmp;
		break;
	      }
	  }	
      
      images[i1] = w;
    }

  return Map(G,H,images);
}

bool HomEnumeratorARCer1::extendToHom( Map& m )
{
  int numOfGensG = G.numberOfGenerators();
  int numOfGensH = H.numberOfGenerators();
  
  SetOf<Word> GRels = G.getRelators();
  SetIterator<Word> I(GRels);
  
  if ( rangeGIC.haveConfluentKBMachine() ) 
    for( I.reset() ; !I.done() ; I.next() ) 
      {
	Word tmp( ( m.imageOf( I.value() ) ).freelyReduce() );
	rangeGIC.getConfluentKBMachine().rewrite( tmp );
	if( tmp.length() != 0 )
	  return false;
      }
  
  if ( rangeGIC.haveAutomatic() ) 
    for( I.reset() ; !I.done() ; I.next() ) 
      {
	Word tmp( ( m.imageOf( I.value() ) ).freelyReduce() );
	rangeGIC.getDiffMachine().rewrite( tmp );
	if( tmp.length() != 0 )
	  return false;
      }
  
  return true;
}

void HomEnumeratorARCer1::enumerate( )
{
  Map m;
  
  while (current < number) 
    {
      if (submitSignalRecieved()){
	return;
      }
      
      m = getMap();
      
      if( extendToHom( m ) )
	{
	  current++;
	  
	  theData << current << ": ";
	  theData << EnumWriteMap( m ) << "\n";
	}
    }
  
  theData << "\n\n";
  
}


void HomEnumeratorARCer1::writeResults( ostream& out ) 
{
  out < current < theData;
}


void HomEnumeratorARCer1::readResults( istream& in )
{
  in > current > theData;
}


// ------------------------ RandEnumeratorProblem ------------------------- //

RandHomEnumeratorProblem::RandHomEnumeratorProblem( SMFPGroup& g,
						    SMFPGroup& h )
  : EnumeratorProblem< Map >( g ),
    G( g ),
    H( h ),
    kbSupervisor( *this, H.gcm().kbSupervisor ),
    agSupervisor( *this, H.gcm().agSupervisor ),
    init( false ),
    arcer( *this , theData , H.gic )
{ }

void RandHomEnumeratorProblem::start( ) 
{   
  LogMessage msg( *this , G , H );
  
  if( G.getFreePreimage().numberOfGenerators() == 0 )
    {
      msg <<  Name( G ) << " is trivial, so it has only one homomorphism to " << Name( H ) << " : 1->1.";
      
      msg.send();
      resultIsFast();
    }
  else
    if( H.getFreePreimage().numberOfGenerators() == 0 )
      { 
	msg <<  Name( G ) << " has only one homomorphism to " << Name( H ) << " that maps all generators to 1.";
	
	msg.send();
	resultIsFast();
      }
    else
      if( !init && 
	  ( H.gic.haveConfluentKBMachine() || H.gic.haveAutomatic() ) )
	{
	  if( H.gic.haveConfluentKBMachine() )
	    agSupervisor.getWorker()->adminTerminate();
	  else
	    kbSupervisor.getWorker()->adminTerminate();	      
	  
	  int num = getParameters().getValue("P1");
	  int avgNumGens = getParameters().getValue("P2");
	  
	  arcer.setArguments( G.getFPGroup() , H.getFPGroup() ,
			      avgNumGens , num );
	  
	  msg << Link( Chars( "Click here to see the random homomorphisms of ") + Text( Name( G ) ) + Chars( " to " ) + Text( Name( H ) ) ,
		       "RandomHomomorphisms" ,
		       theData.getDataFileName() , true );
	  
	  msg.send();
	  
	  init = true;
	}
}

void RandHomEnumeratorProblem::takeControl( )
{
  if( !init && 
      ( H.gic.haveConfluentKBMachine() || H.gic.haveAutomatic() ) )
    {
      if( H.gic.haveConfluentKBMachine() )
	agSupervisor.getWorker()->adminTerminate();
      else
	kbSupervisor.getWorker()->adminTerminate();
      
      int num = getParameters().getValue("P1");
      int avgNumGens = getParameters().getValue("P2");
      
      arcer.setArguments( G.getFPGroup() , H.getFPGroup() ,
			  avgNumGens , num );
      
      LogMessage msg( *this , G , H );
      
      msg << Link( Chars( "Click here to see the random homomorphisms of ") + Text( Name( G ) ) + Chars( " to " ) + Text( Name( H ) ) ,
		   "RandomHomomorphisms" ,
		   theData.getDataFileName() , true );
      
      msg.send();
      
      init = true;
    }
  
  if( !fastResult() && init )
    if( freeARCs() > 0 )
      {
	if( arcer.takeControl() )
	  {
	    adminSuspend();
	    return;	
	  }
	usedOneARC();
      }
    else
      adminTerminate();

}

void RandHomEnumeratorProblem::viewStructure(ostream& ostr) const 
{
  
   EnumertatorProblemView pv(oid(),"Homomorphism enumerator",
			     Text("Randomly enumerate homomorphisms from ")+
			     Name( G ) + Text(" to ") + Name( H ),
			     helpID("RandHomEnumeratorProblem",G),
			     Text("Hom en")
			     );

   pv.addParameter(EditInteger("P1","Number of homomorphisms",1,10,0,10000));
   pv.addParameter(EditInteger("P2","Average length of image",0,10,0,50));

   pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			     Text("For the use of enumerator"),100));
   
   pv.addProblem( Subproblem("Sp2",kbSupervisor.arcSlotID(),
			     Text("Seek a rewriting system for") + Name( H ),
			     100));
   
   pv.addProblem( Subproblem("Sp3",agSupervisor.arcSlotID(),
			     Text("Seek a ShortLex automatic structure for") + Name( H ),
			     100));
   
   submit(ostr, pv);
   
}

// ------------------------ HomEnumeratorARCer2 ---------------------------- //


void HomEnumeratorARCer2::setArguments( FPGroup g, 
					FPGroup h,
					int n )
{
  number = n;
  G = g;
  H = h;
  
  we = WordEnumerator( FreeGroup( H.namesOfGenerators() ) , 
		       G.numberOfGenerators() ); 
}


Map HomEnumeratorARCer2::nextMap()
{
  VectorOf<Word> images = we.getWords();
  we.next();
  
  return Map(G,H,images);  
}

bool HomEnumeratorARCer2::extendToHom( Map& m )
{
  int numOfGensG = G.numberOfGenerators();
  int numOfGensH = H.numberOfGenerators();
  
  SetOf<Word> GRels = G.getRelators();
  SetIterator<Word> I(GRels);
  
  if ( rangeGIC.haveConfluentKBMachine() ) 
    for( I.reset() ; !I.done() ; I.next() ) 
      {
	Word tmp( ( m.imageOf( I.value() ) ).freelyReduce() );
	rangeGIC.getConfluentKBMachine().rewrite( tmp );
	if( tmp.length() != 0 )
	  return false;
      }
  
  if ( rangeGIC.haveAutomatic() ) 
    for( I.reset() ; !I.done() ; I.next() ) 
      {
	Word tmp( ( m.imageOf( I.value() ) ).freelyReduce() );
	rangeGIC.getDiffMachine().rewrite( tmp );
	if( tmp.length() != 0 )
	  return false;
      }
  
  return true;
}

void HomEnumeratorARCer2::enumerate( )
{
  Map m;
  
  while (current < number) 
    {
      if (submitSignalRecieved()){
	return;
      }
      
      m = nextMap();
      
      if( extendToHom( m ) )
	{
	  current++;
	  
	  theData << current << ": ";
	  theData << EnumWriteMap( m ) << "\n";
	}
    }
  
 theData << "\n\n";
  
}


void HomEnumeratorARCer2::writeResults( ostream& out ) 
{
  out < current < theData;
}


void HomEnumeratorARCer2::readResults( istream& in )
{
  in > current > theData;
}


// ----------------------- RandHomEnumeratorProblem ------------------------ //

TotalHomEnumeratorProblem::TotalHomEnumeratorProblem( SMFPGroup& g,
						      SMFPGroup& h )
  : EnumeratorProblem< Map >( g ),
    G( g ),
    H( h ),
    kbSupervisor( *this, H.gcm().kbSupervisor ),
    agSupervisor( *this, H.gcm().agSupervisor ),
    init( false ),
    arcer( *this , theData , H.gic )
{ }

void TotalHomEnumeratorProblem::start( ) 
{   
  LogMessage msg( *this , G , H );
  
  if( G.getFreePreimage().numberOfGenerators() == 0 )
    {
      msg <<  Name( G ) << " is trivial, so it has only one homomorphism to " << Name( H ) << " : 1->1.";
      
      msg.send();
      resultIsFast();
    }
  else
    if( H.getFreePreimage().numberOfGenerators() == 0 )
      { 
	msg <<  Name( G ) << " has only one homomorphism to " << Name( H ) << " that maps all generators to 1.";
	
	msg.send();
	resultIsFast();
      }
    else
      if( !init && 
	  ( H.gic.haveConfluentKBMachine() || H.gic.haveAutomatic() ) )
	{
	  if( H.gic.haveConfluentKBMachine() )
	    agSupervisor.getWorker()->adminTerminate();
	  else
	    kbSupervisor.getWorker()->adminTerminate();	      
	  
	  int num = getParameters().getValue("P");
	  
	  arcer.setArguments( G.getFPGroup() , 
			      H.getFPGroup() , num );
	  
	  msg << Link( Chars( "Click here to see the homomorphisms of ") + 
		       Text( Name( G ) ) + Chars( " to " ) + 
		       Text( Name( H ) ) ,
		       "TotalHomomorphisms" ,
		       theData.getDataFileName() , true );
	  msg.send();
	  
	  init = true;
	}
}

void TotalHomEnumeratorProblem::takeControl( )
{
  if( ( H.gic.haveConfluentKBMachine() || H.gic.haveAutomatic() ) 
      && !init )
    {
      if( H.gic.haveConfluentKBMachine() )
	agSupervisor.getWorker()->adminTerminate();
      else
	kbSupervisor.getWorker()->adminTerminate();
      
      int num = getParameters().getValue("P");
      
      arcer.setArguments( G.getFPGroup() , 
			  H.getFPGroup() , num );
      
      LogMessage msg( *this , G , H );
      
      msg << Link( Chars( "Click here to see the homomorphisms of ") + 
		   Text( Name( G ) ) + Chars( " to " ) + 
		   Text( Name( H ) ) ,
		   "TotalHomomorphisms" ,
		   theData.getDataFileName() , true );
      msg.send();
      
      init = true;
    }
  
  if( !fastResult() && init )
    if( freeARCs() > 0 )
      {
	if( arcer.takeControl() )
	  {
	    adminSuspend();
	    return;	
	  }
	usedOneARC();
      }
    else
      adminTerminate();
  
}

void TotalHomEnumeratorProblem::viewStructure(ostream& ostr) const
{
  
   EnumertatorProblemView pv(oid(),"Homomorphism enumerator",
			     Text("Totally enumerate homomorphisms from ")+
			     Name( G ) + Text(" to ") + Name( H ),
			     helpID("TotalHomEnumeratorProblem",G),
			     Text("Hom en")
			     );

   pv.addParameter(EditInteger("P","Number of homomorphisms",1,10,0,10000));
   
   pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			     Text("For the use of enumerator"),100));
   
   pv.addProblem( Subproblem("Sp2",kbSupervisor.arcSlotID(),
			     Text("Seek a rewriting system for") + Name( H ),
			     100));
   
   pv.addProblem( Subproblem("Sp3",agSupervisor.arcSlotID(),
			     Text("Seek a ShortLex automatic structure for") + Name( H ),
			     100));
   
   submit(ostr, pv);
}



