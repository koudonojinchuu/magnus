// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of one-relator problems.
//
// Principal Author: 
//
// Status: in progress
//
// Revision History:
//

#include "OutMessages.h"
#include "OneRelatorProblems.h"
#include "MagnusBreakdown.h"
#include "SMHomomorphism.h"
#include "SMWord.h"
#include "SMSubgroup.h"
#include "viewStructure.h"

FPGroup makePresentationOf( const OneRelatorGroup& G )
{
  return FPGroup( G.namesOfGenerators(), G.relator() );
}

void ORFindHNNPresentation::takeControl( )
{

  SMFPGroup *theGroup1 = (SMFPGroup *)&theGroup;
  SMFPGroup *group = theGroup1;

  FPGroup FP = theGroup.getFPGroup();
  Word relator =  theGroup.gic.getOneRelator();
  VectorOf<Chars> oldGeneratorsNames = FP.namesOfGenerators();

  OneRelatorGroup G( oldGeneratorsNames, relator );  
  ListOf<OID> dependencies;

  // Step I.. Simplify the one-relator presentation.

  // Step 1a. Seach for cyclically reduced relator of the group.

  if( relator.cyclicallyReduce().length() != relator.length() ) {
    relator = relator.cyclicallyReduce();
    LogMessage msg( *this, theGroup );
    msg << "Made cyclically reduced relator ";
    FP.printWord(msg, relator );
    msg << " of the group " << Name(theGroup) << ".";
    msg.send();
  }
  
  // Step 1b. Search for free part of group: 

  SetOf<Generator> relatorGens;
  for( int i = relator.length()-1; i >= 0; --i )
    relatorGens |= Generator( abs(ord(relator[i])) );
  if( numOfGens > 0 ) {
    // user can specify a generator not occured in the relator.
    relatorGens |= stableGen; 
  }
  
  int oldGen = FP.numberOfGenerators();
  int newGen =  relatorGens.cardinality();
  int freeGen = oldGen - newGen;

  if( freeGen > 0 ) {
    VectorOf<Word> imagesOldToNew( oldGen );
    VectorOf<Word> imagesNewToOld( newGen );
    VectorOf<Chars> newGeneratorsNames( newGen );
    VectorOf<Chars> freeGeneratorsNames( freeGen );
    for( int i = oldGen-1, f = freeGen, n = newGen ; i >= 0; --i ) {
      if( !relatorGens.contains( Generator(i+1) ) ) 
	freeGeneratorsNames[ --f ] = oldGeneratorsNames[i];
      else {
	imagesOldToNew[ i ] = Word( Generator(n) );
	imagesNewToOld[ --n ] = Word( Generator(i+1) );
	newGeneratorsNames[ n ]  = oldGeneratorsNames[i];
      }
    }

    // form factors of free product.
    
    FreeGroup F(freeGeneratorsNames);
    relator = relator.replaceGenerators( imagesOldToNew );
    G = OneRelatorGroup( newGeneratorsNames, relator );
    if( numOfGens > 0 )
      stableGen = imagesOldToNew[ ord(stableGen)-1 ][0];
    if( numOfGens > 1 ) 
      accompGen = imagesOldToNew[ ord(accompGen)-1 ][0]; 

    // Checkin non-free part of one-relator group.
    
    SMFPGroup *theGroup2 = 
      new SMFPGroup( makePresentationOf(G),
		     Text("The group of non-free part of one-relator group ") 
		     + Name( theGroup ),
		     SMFPGroup::ONE_RELATOR );

    CheckinMessage( *theGroup2, "" /*"G"*/, ListOf<OID>() ).send();

    group = theGroup2;

    // Make mappings between G1 and G2:
    FPGroup Gfp = makePresentationOf( G ); 
    Map mapOldToNew(FP, Gfp, imagesOldToNew);
    Map mapNewToOld( Gfp, FP, imagesNewToOld);

    dependencies = ListOf<OID>( *theGroup1 );
    dependencies.append( *theGroup2 );

    SMHomomorphism2 *smmap1 = 
      new SMHomomorphism2( *theGroup1, *theGroup2, mapOldToNew, 
			   Text("The restricting map of group ") 
			   + Name( *theGroup1 ) + " onto the group "
			   + Name( *theGroup2 ) 
			   );
    CheckinMessage( *smmap1, "", dependencies ).send();

    SMHomomorphism2 *smmap2 = 
      new SMHomomorphism2( *theGroup2, *theGroup1, mapNewToOld, 
			   Text("The embedding map of the group ") 
			   + Name( *theGroup2 ) + " into the group "
			   + Name( *theGroup1 ) 
			   );
    CheckinMessage( *smmap2, "", dependencies ).send();
  }

  // Step 2. Make a Magnus breakdown on given generators.

  MagnusBreakdown M(G);
  bool found = false;

  if( numOfGens == 0 || G.numberOfGenerators() == 1 ) {
    found = M.findHNNPresentation( );

    if( !found ) {
      int order = relator.length();
      
      if( order > 0 && FP.raiseToPower( Word(relator[0]), order ) != relator )
	error("Interal error: bad order of group "
	      "in ORFindHNNPresentation::takeControl");
      
      LogMessage msg( *this, *group );
      msg << "Magnus breakdown completed. The one-relator group " 
	  << Name(*group) << " is ";
      switch ( order ) {
      case 0:
	msg << "a cyclic infinite group" ;
	break;
      case 1:
	msg << "a trivial group";
	break;
      default: 
	msg << "a cyclic group of order " << order;
      }
      msg << '.';
      msg.send( );
      return;
    }
    
    stableGen = M.stableGenerator();
    numOfGens = 1;
    if( M.getExponentSumOf( stableGen ) != 0 ) {
      ++numOfGens;
      accompGen = M.accompGenerator();
    }
  }
  else if( numOfGens == 1 )
    found = M.findHNNPresentation( stableGen );
  else
    found = M.findHNNPresentation( stableGen, accompGen );

  // Put HNN presentation and its base group  into workspace.
   
  if( !found ) 
    error("Internal error in ORFindHNNPresentation::takeControl()");

  HNNExtOfORGroup H = M.getHNNPresentation();
    
  if( numOfGens == 2 ) {
    FPGroup FP2( M.getORGroup().namesOfGenerators(), 
		 M.embeddingOfORGroups().imageOf( M.getORGroup().relator() ) 
		 );

    // Make embedding.

    //@njz
    //    ostrstream heritage;
    std::ostrstream heritage;
    //
    heritage << "Embed " << Name(theGroup) 
	     << " in the group obtained by adjoining to " << Name(theGroup)
	     << " a "
	     << M.getExponentSumOf(accompGen) << "-th root of ";
    FP2.printWord( heritage, stableGen );
    heritage << " and set ";
    FP2.printWord( heritage, accompGen );
    heritage << " = ";
    FP2.printWord( heritage, 
	  M.embeddingOfORGroups().generatingImages( abs(ord(accompGen))-1 )
    );
    heritage << " " << ends;
	       
    SMFPGroup *theGroup3 = 
      new SMFPGroup( FP2, heritage.str(), SMFPGroup::ONE_RELATOR);
      
    CheckinMessage( *theGroup3, "", ListOf<OID>() ).send();

    dependencies = ListOf<OID>( *group );
    dependencies.append( *theGroup3 );

    SMHomomorphism2 *embedding = 
      new SMHomomorphism2( *group, *theGroup3, 
			   M.embeddingOfORGroups(),
			   Text(Name( *group )) + "is embedded in" +
			   Name( *theGroup3 ) + "via the mapping"
			   );
    CheckinMessage( *embedding, "", dependencies ).send();

    group = theGroup3;
  }

  // Check in HNN presentation and corresponding mappings.
    
  FPGroup OFP = group->getFPGroup();
  FPGroup HFP = H.getPresentation();

  //@njz
  //  ostrstream heritage;
  std::ostrstream heritage;
  //
  heritage << Name( *group ) << " can be expressed as an HNN presentation"
	   << " with stable letter t = ";
  OFP.printWord( heritage, stableGen );
/*@db 
  if( numOfGens == 2 ) {
    heritage << " and accompanying generator ";
    OFP.printWord( heritage, accompGen );
  }
*/
  heritage << ends;

  SMFPGroup *HNN = new SMFPGroup( HFP, heritage.str() );      
    
  CheckinMessage( *HNN, "" /*"H"*/, ListOf<OID>() ).send();

  // Forms  isomorphic mappings between an one-relator group and 
  // its HNN-presentation.

  dependencies = ListOf<OID>( *group );
  dependencies.append( *HNN );

  SMHomomorphism2 *ORG2HNN = 
    new SMHomomorphism2( *group, *HNN, M.toHNNPresentation(), 
			 Text("The isomorphism from") + Name( *group )
			 + "into" + Name( *HNN )
			 + Text("is defined by the mapping")
			 );
  CheckinMessage( *ORG2HNN, "", dependencies).send();


  SMHomomorphism2 *HNN2ORG = 
    new SMHomomorphism2( *HNN, *group, M.toORGroup(),
			 Text("The inverse isomorphism from") + Name( *HNN )
			 +"to" + Name( *group ) + Text("is defined by")
			 );
  CheckinMessage( *HNN2ORG, "", dependencies).send();


  // Check in an basis group of HNN-presentation and corresponding mappings:

  FPGroup BFP = makePresentationOf( H.basisGroup() );
    
  //@njz
  //  ostrstream heritage2;
  std::ostrstream heritage2;
  //
  heritage2 << "The base group of the HNN extension " << Name( *HNN )
	    << " has presentation" << ends;
/*@db
  OFP.printWord( heritage2, stableGen );
  if( numOfGens == 2 ) {
    heritage2 << " and accompanying generator ";
    OFP.printWord( heritage2, accompGen );
  }
  heritage2 << " and presentation" << ends;
*/        
  SMFPGroup *basisGroup = 
    new SMFPGroup( BFP, heritage2.str(), SMFPGroup::ONE_RELATOR );

  CheckinMessage( *basisGroup, "" /*"B"*/, ListOf<OID>() ).send();
    
  dependencies = ListOf<OID>( *HNN );
  dependencies.append( *basisGroup );

  int numberOfNewGenerators = M.numberOfNewGenerators();
  VectorOf<Word> images( numberOfNewGenerators );
  for( int i = 0; i < numberOfNewGenerators-1 ; ++i )
    images[i] = Generator(i+1);
  Map hnn2basis( HFP, BFP, images );
 
/*@db
  SMHomomorphism2 *HNN2BASIS = 
    new SMHomomorphism2( *HNN, *basisGroup, hnn2basis, 
			 Text("The isomorphism from") + Name( *group )
			 + Text(", thought of a subgroup of") + Name( *HNN )
			 + ", to" + Name( *basisGroup )
			 + Text("is defined by")
			 );
  CheckinMessage( *HNN2BASIS, "", dependencies ).send();
*/    

  images.shrink( numberOfNewGenerators - 1 );
  Map basis2hnn( BFP, HFP, images );

  SMHomomorphism2 *BASIS2HNN = 
    new SMHomomorphism2( *basisGroup, *HNN, basis2hnn, 
			 Text("The monomorphism from")
			 + Name( *basisGroup )
			 + "into" + Name( *HNN )
			 + Text("is defined by")
			 );
  CheckinMessage( *BASIS2HNN, "", dependencies ).send();
}    




// ========================================================================= //


// ------------------------ ORWordProblemARCer ----------------------------- //


ORExtendedWordProblemARCer::ORExtendedWordProblemARCer( ComputationManager& boss )
  : ARCer( boss ), answer(dontknow)
{ } 


void ORExtendedWordProblemARCer::setArguments( const Word& relator,
					       const Word& testWord,
					       const VectorOf<Word>& target )
{
  theRelator  = relator;
  theTestWord = testWord;
  theTarget   = target;
}

void ORExtendedWordProblemARCer::runComputation( ) 
{
  Generator g1 = theRelator.maxOccurringGenerator();
  Generator g2 = theTestWord.maxOccurringGenerator();
  int numOfGens = max( abs(ord(g1)), abs(ord(g2)) );

  for(int i = 0; i < theTarget.length(); ++i ) {
    Word gen = theTarget[i];
    numOfGens = max( numOfGens, ord(gen.maxOccurringGenerator()) );
  }

  OneRelatorGroup G( numOfGens, theRelator );
  SubgroupOfOneRelatorGroup S( G, theTarget );
  
  answer = S.contains( theTestWord );
  if( answer == yes ) 
    theWordInBasis = S.rewriteFromGroupToSubgroupGenerators( theTestWord );
}

void  ORExtendedWordProblemARCer::writeResults( ostream& out )
{
  out < answer;
  if( answer == yes ) 
    out < ' ' < theWordInBasis;
}


void ORExtendedWordProblemARCer::readResults( istream& in )
{
  in > answer;
  if( answer == yes ) {
    retValue = true;
    in > theWordInBasis;
  }
  else if( answer == no )
    retValue = false;
  else 
    error( "Internal error in OneRelatorGroup::wordProblem() " );
}


// ----------------------- ORExtendedWordProblemCM ------------------------ //


ORExtendedWordProblemCM::ORExtendedWordProblemCM( ORExtendedWordProblem& PO )
: theRelator( PO.getTestWord().getParent().gic.getOneRelator() ),
  theTestWord( PO.getTestWord().getWord() ),
  theTarget( PO.getTarget().getSubgroup().generators() ),
  itIsExpressed( dontknow ),
  theWordInBasis( ),
  bStarted( false ),
  arcer( *this )
{
  adminStart();
}

void ORExtendedWordProblemCM::terminate( )
{
  arcer.terminate();
}

void ORExtendedWordProblemCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if( !bStarted ) {
      arcer.setArguments( theRelator, theTestWord, theTarget );
      bStarted = true;
    }
    if( arcer.takeControl() ) {
      itIsExpressed = arcer.getRetValue();
      theWordInBasis = arcer.wordInBasis();
      return;
    }
    else
      usedOneARC();
  }
}


// -------------------------- ORExtendedWordProblem ------------------------ //


ORExtendedWordProblem::ORExtendedWordProblem( SMSubgroup& target, SMWord& w )
  : Supervisor( true ),
    theWord( w ),
    theTarget( target ),
    theAnswer( dontknow ),
    theWordInBasis( ),
    orwp( *this )
{
  OneRelatorGroup G( w.getParent().getFPGroup().namesOfGenerators(), 
		     w.getParent().gic.getOneRelator() );
  SubgroupOfOneRelatorGroup S( G, target.getSubgroup().generators() );
  if( !S.isMagnusSubgroup() ) {
    resultIsFast();
    theAnswer = no;
    Message msg("Warning");
    msg << "The given subgroup is not a Magnus one.";
    msg.send();
  }
}

void ORExtendedWordProblem::takeControl( )
{
  if ( ( theAnswer = orwp->isExpressed() ) != dontknow ) {
    
    LogMessage msg( *this, theTarget, theWord );
    msg << Name( theWord ) << " is ";
    if ( theAnswer == no ) msg << "not ";
    msg << "expressed in generators of " << Name( theTarget );
    
    if( theAnswer == yes ) {
      theWordInBasis =  orwp->wordInBasis();
      msg << " : " << Name(theWord) << " = ";
      theTarget.getSubgroup().printWord( msg, theWordInBasis );
    }
    msg << ".";
    msg.send();
    adminTerminate();
  }
}


void ORExtendedWordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Extended word problem"),
		  Text("Is") + Name( theWord ) + "expressed in basis of"
		  + Name( theTarget ) + "?",
		  helpID( "ExtendedWordProblem", theWord.getParent() ), 
		  Text(Name(theWord)) + " in " + Text(Name(theTarget)) + "?"
		  );
  

  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), orwp.arcSlotID(), 50);

  pv.done();
}


void ORIsMagnusSubgroup::takeControl( )
{
  OneRelatorGroup G( theSubgroup.getParent().getFPGroup().namesOfGenerators(), 
		     theSubgroup.getParent().gic.getOneRelator() );
  SubgroupOfOneRelatorGroup S( G, theSubgroup.getSubgroup().generators() );

  bool answer = S.isMagnusSubgroup();

  LogMessage msg( theSubgroup );
  msg << Name(theSubgroup) << " is ";
  if( !answer )
    msg << "not ";
  msg << "a Magnus subgroup.";
  msg.send();  
}
