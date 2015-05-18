// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes TTArcer, TTCM, TTProblem.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "stdio.h"
#include "string.h"
#include "TTProblem.h"
#include "OutMessages.h"
#include "viewStructure.h"


// ------------------------------ TTArcer ---------------------------------- //


Chars TTArcer::getFileName( ) {

#if SAFETY > 0
  if( !ttp )
    error("Chars TTArcer::getFileName( ) : "
	  "The file has not been created.");
#endif
  
  return ttp->getFileName();
}
  

void TTArcer::setArguments( const FPGroup& G )
{
  theGroup = G;

  delete ttp;
  ttp = new TTP(G);
}

void TTArcer::writeResults( ostream& out ) 
{
  out < bHappy;
}


void TTArcer::readResults( istream& in )
{
  in > bHappy;
}


void TTArcer::runComputation( )
{
  if( !ttp->run() )
    bHappy = false;
}


// ----------------------------- KBSupervisor ------------------------------ //


TTCM::TTCM( class TTProblem& PO )
  : theGroup( PO.getGroup() ),
    arcer( *this ),
    inPos( 0 ),
    bInstructions( true )
{
  adminStart();
  in = fopen( arcer.getFileName(), "r" );
}

TTCM::~TTCM( ) 
{
  fclose(in);
}

void TTCM::start( )
{
  arcer.setArguments( theGroup.getFPGroup() );
}


void TTCM::takeControl( )
{
  // infinite process
  if ( freeARCs() > 0 ) {

    arcer.takeControl();
    usedOneARC();
    
    fseek(in,0,SEEK_END);
    long sz;
    if( ( sz = ftell(in)) > inPos ) {
      
      char s[1000];
      fseek(in,inPos,SEEK_SET);
      
      while( fgets(s,1024,in) != 0 ) {

	if( bInstructions ) 
	  if( strchr(s,'#') == 0 ) continue;
	  else 
	    bInstructions = false;
	
	int len = strlen(s);
	for( int i = 0; i < len; ++i )
	  if( s[i] == '#' ) s[i] = 'G';
	
	out << s << ready;
      }
      
      inPos = sz;
    }
  }
}


// -------------------------- TTProblem ----------------------------------- //


//@njz: renamed the argument 'G' to 'smG' to avoid conflicting with local var 'G'
//TTProblem::TTProblem(class SMFPGroup& G)
//  : Supervisor( true ),
//    theGroup( G ),
//    tt( *this ),
//    linkHasBeenSent( false )
TTProblem::TTProblem(class SMFPGroup& smG)
  : Supervisor( true ),
    theGroup( smG ),
    tt( *this ),
    linkHasBeenSent( false )
//
{ 
  FPGroup G = theGroup.getFPGroup(); 
  SetOf<Word> S = G.getRelators();
  SetIterator<Word> I(S);
  
  for( int i = 0; i < G.numberOfGenerators(); ++i ) {
    
    for( I.reset(); !I.done(); I.next() )
      if( Word(I.value()).numberOfOccurrences( Generator(i+1) ) != 0 )
	break;
    
    if( I.done() ) {
      
      LogMessage msg( *this );
      msg << "The current version of the Tietze Trasformation Package does"
	  << " not allow us to have a free generator in a group presentation.";
      msg.send();
      resultIsFast();
      return;
    }
  }
  
  //@njz
  //  ostrstream ostr;
  std::ostrstream ostr;
  //
  
  int i;
  for( I.reset(), i = 0; !I.done(); I.next(), ++i ) {
    
    G.printWord(ostr, I.value());
    if( i < S.cardinality() - 1 )
      ostr << ',';
    else
      ostr << '!' << endl;
  }
  
  if( ostr.pcount() > 254 ) {

    LogMessage msg( *this );
    msg << "The current version of the Tietze Trasformation Package does"
	<< " not allow us to have an input line longer than 254 characters.";

    msg.send();
    resultIsFast();
    return;
  }
}


void TTProblem::takeControl( )
{
  if( fastResult() ) {
    adminTerminate();
    return;
  }

  if( !tt->isHappy() ) {
    
    LogMessage msg( *this );
    msg << "The Tietze Trasformation Package is unable to work with"
	<< " your presentation; probably it is to long for it.";
    
    msg.send();
    adminTerminate();
    return;
  }
  
  if( !linkHasBeenSent ) {
    
    linkHasBeenSent = true;
    
    LogMessage msg( *this );
    msg << Link(Chars("Click here to see other presentations for ")
		+ Text(Name( theGroup )), "TTProblem", 
		tt->getFileName(), true);
    msg << ".";
    msg.send();
  }
  
  // infinite process
}


void TTProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Presenations for the group"),
		  Text("Seek other presentations for") 
		  + Name( theGroup ),
		  helpID("TTProblem", theGroup),
		  "Present"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), tt.arcSlotID(), 100);

  pv.done();
}
