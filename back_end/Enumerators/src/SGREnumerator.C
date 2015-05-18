// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class RelatorEnumerator
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "SGREnumerator.h"
#include "SMFPGroup.h"
#include "Subgroup.h"
#include "OneRelatorGroup.h"
#include "SetOfWordsChecker.h"
#include "OutMessages.h"
#include "viewStructure.h"

//--------------------------------------------------------------------------//
//--------------------- SGRelatorEnumeratorARCer ---------------------------//
//--------------------------------------------------------------------------//



void SGRelatorEnumeratorARCer::setArguments(const  SMFPGroup* group,
					  const SMSubgroup* subgroup )
{
  theGroup = group;
  theSMSubgroup = subgroup;
  if (firstStart){
    file << "For this enumeration, name the generators of " 
	 << "the subgroup as follows: ";
    //      << Name( *theSMSubgroup ) << " as follows: ";
    
    VectorOf<Word> gens = theSMSubgroup->getSubgroup().generators();
    for( int i1 = 0; i1 < gens.length(); ++i1 ) {
      if ( i1 > 0 ) file << ", ";
      file << "x" << i1 + 1 << " = ";
      theGroup->getFPGroup().printWord( file, gens[i1] );
    }
    
    file << "." << endl << ready;
    firstStart = false;
  }
}

void SGRelatorEnumeratorARCer::writeResults( ostream& out ) 
{
  out < possibleRelator < counter;
}

void SGRelatorEnumeratorARCer::readResults( istream& in )
{
  in > possibleRelator > counter;
}

void SGRelatorEnumeratorARCer::enumerate()
{
  
  // We use this to enumerate all words in the subgroup generators.
  int numberOfSubgroupGens = 
    theSMSubgroup->getSubgroup().generators().length();
  Subgroup theSubgroup( theGroup->getFPGroup(),
			theSMSubgroup->getSubgroup().generators()
			);
  
  while ( counter <= numberOfAll ) {

    if (submitSignalRecieved()){
      return;
    }
    // returns if terminate was called in parent process

    possibleRelator =
      possibleRelator.nextCyclicallyReduced( numberOfSubgroupGens );
    
    bool ORmethod = false;
    Trichotomy answer( dontknow );
    Word wordToCheck(theSubgroup.eval(possibleRelator));
    
    if (theGroup->gic.haveFastWordProblem()){
      SetOfWordsChecker theChecker( wordToCheck, *theGroup );
      
      answer = theChecker.isTrivial();
    }
    else {
      if (theGroup->gic.isOneRelator()){
	answer = ORisTrivial(wordToCheck);
	ORmethod = true;
      }
    }
    if ( answer == yes ) {
      
      // Use this crufty old Subgroup member:
      
      if ( ! theSubgroup.redundantRelator(possibleRelator) ) {
	file << counter << ": ";
	theSubgroup.printWord(file, possibleRelator );

	file << endl << endl << ready;
	counter++;
      }
    }
  } 
  
}

bool SGRelatorEnumeratorARCer::ORisTrivial(const Word&  theTestWord)
{
  if (!theGroup->gic.isOneRelator())
    error("bool RelatorEnumeratorARCer::ORisTrivial(const Word&  theTestWord) :"
	  " group is not one relator.");
  Generator g1 = theGroup->gic.getOneRelator().maxOccurringGenerator();
  Generator g2 = theTestWord.maxOccurringGenerator();
  int numOfGens = max( abs(ord(g1)), abs(ord(g2)) );
  
  OneRelatorGroup G( numOfGens, theGroup->gic.getOneRelator() );
  return G.wordProblem( theTestWord );
}

//--------------------------------------------------------------------------//
//----------------------- SGRelatorEnumerator ------------------------------//
//--------------------------------------------------------------------------//


SGRelatorEnumerator::SGRelatorEnumerator(SMSubgroup& s)
  : EnumeratorProblem< NoType >( s ),
    theSMSubgroup( s ),
    theGIC( s.getParent().gic ),
    arcer(*this, theData),
    kbSupervisor( *this, s.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, s.getParent().gcm().agSupervisor ),
    firstStart(true),
    started(false),
    resumed( false ),
    useORwordProblem(false)
{
   if ( ! theGIC.haveFastWordProblem() ) {
     if (theGIC.isOneRelator())
       useORwordProblem = true;
     else {
       LogMessage msg( theSMSubgroup );
       msg <<
	 "The system has no fast, complete solution of the word problem "
	 "for " << Name( theSMSubgroup.getParent() ) << 
	 " yet. It will not try to enumerate "
	 "relators of " << Name( theSMSubgroup ) << " until it finds one "
	 "(a more general enumerator is in the works).";
       msg.send();
     }
   }
}


void SGRelatorEnumerator::takeControl( )
{
  if ( (theGIC.haveFastWordProblem() || theGIC.isOneRelator() ) && 
       freeARCs() > 0 ) {
    if (firstStart){
      LogMessage msg( *this, theSMSubgroup );
      msg << Link(Text("Click here to view the relators of ")+ 
		  Name(theSMSubgroup),
		  "RelatorEnumerator", arcer.getFileName(),true);
      msg.send();
      firstStart = false;
    }
    if (!started){
      arcer.setArguments(&(theSMSubgroup.getParent()),
			 &theSMSubgroup);
      started = true;
    }
    // @am if fast word problem was found and we were using one
    // relator method then reset ARCer
    if (theGIC.haveFastWordProblem() && useORwordProblem && started){
      arcer.submitSignal();
      useORwordProblem = false;
      started = false;
    }

    if (arcer.takeControl()){
      if (started && !resumed){
	
	adminSuspend();
	return;	
      }
      if (resumed)
	resumed = false;
    }
 
    usedOneARC();
  }
}


void SGRelatorEnumerator::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv( oid(), "Relator enumerator",
			     Text("Enumerate relators of") + 
			     Name( theSMSubgroup ),
			     helpID("RelatorEnumerator", 
				    theSMSubgroup.getParent()),
			     "SG Rel Enum"
			     );
  
  pv.addParameter(EditInteger("numberOfAll","Number of relators",1,100,0,1000000));

  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of this enumerator"),50));
  
  //@rn Dodge updating the FE database for now:

  if ( ! theGIC.haveFastWordProblem() ) {
    
    pv.addProblem( Subproblem("Sp2",kbSupervisor.arcSlotID(),
			      Text("Seek a rewriting system for") +
			      Name( theSMSubgroup.getParent() ),
			      50));
    
    pv.addProblem( Subproblem("Sp2",agSupervisor.arcSlotID(),
			      Text("Seek a ShortLex automatic structure for") +
			      Name( theSMSubgroup.getParent() ),
			      50));
    
  }

  submit(ostr, pv);
}
