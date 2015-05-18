// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:  
// 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#include "HNNProblems.h"
#include "SMSubgroup.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "FEData.h"
#include "viewStructure.h"
#include "HNNExtOfFreeGroup.h"


void MakeHNNExtOfFreeGroup::takeControl( )
{
  SGofFreeGroup S1 = subgroup1.getSubgroup( );
  SGofFreeGroup S2 = subgroup2.getSubgroup( );

  if( S1.generators().length() != S2.generators().length() ) {
    Message msg;
    msg << "The number of generators of the subgroups has to be equal.";
    msg.send();
    return;
  }
  
  // @dp The given name of stable letter must be checked.
  //     The letter must not belong to the free base group.

  FreeGroup F = subgroup1.getParent().getFPGroup().namesOfGenerators();
  HNNExtOfFreeGroup H(F, theStableLetter, S2, S1);

  // try to avoid HNN at all.
  //  SetOf<Word> relators;
  //  relators |= S1.generators()[0];
  //  relators |= S2.generators()[0];
  //  FPGroup G( 2, relators );

  FPGroup realH = H.getFPGroup();
  
  if( realH.getRelators().cardinality() == 1 )
    {
      SMFPGroup* smo = new SMFPGroup
	( realH, Text("User defined HNN-extension < ") + Text(theStableLetter) + "," + Name(Parent(subgroup1)) + "; " + Text(theStableLetter) + Text("^-1") + Name(subgroup2) + Text(theStableLetter) + " =" + Name(subgroup1) + ">" +
Text("which is actually is a one relator group"),
	  SMFPGroup::ONE_RELATOR
	  );
  
      SetIterator<Word> I(realH.getRelators());
      Word w;
      while( !I.done() )
	{
	  w = I.value();
	  I.next();
	}
      
      smo->gic.putIsOneRelator(w);

      ListOf<OID> dependencies;
      CheckinMessage( *smo, "", dependencies ).send();
    
      return;
    }
  
  SMFPGroup* smo = new SMFPGroup
    ( H.getFPGroup(),
      Text("User defined HNN-extension < ") + Text(theStableLetter) + ","
      + Name(Parent(subgroup1)) + "; "
      + Text(theStableLetter) + Text("^-1") + Name(subgroup2) + Text(theStableLetter) 
      + " =" + Name(subgroup1) + ">",
      SMFPGroup::HNN_FREE
      );
  
  smo->gic.putHaveHNNofFree(H);

  ListOf<OID> dependencies;
  CheckinMessage( *smo, "", dependencies ).send();
}


void HNNofFreeGroup_IsTrivial::takeControl( )
{
  bool answer = ( group.gic.getHNNofFree().isTrivial() == yes );
  
  LogMessage msg( *this, group );
  msg << Name( group ) << " is";
  if( !answer )
    msg << " not";
  msg << " trivial.";

  msg.send();
}

void HNNofFreeGroup_IsFinite::takeControl( )
{
  bool answer = ( group.gic.getHNNofFree().isFinite() == yes );
  
  LogMessage msg( *this, group );
  msg << Name( group ) << " is";
  if( !answer )
    msg << " not";
  msg << " finite.";

  msg.send();
}


void HNNofFreeGroup_IsAbelian::takeControl( )
{
  bool answer = ( group.gic.getHNNofFree().isAbelian() == yes );
  
  LogMessage msg( *this, group );
  msg << Name( group ) << " is";
  if( !answer )
    msg << " not";
  msg << " abelian.";

  msg.send();
}



void HNNofFreeGroup_ConjugacyProblem::takeControl( )
{
  const HNNExtOfFreeGroup& H = word1.getParent().gic.getHNNofFree();

  Word conjugator;

  Trichotomy answer = H.conjugacyProblem( word1.getWord(), word2.getWord(), conjugator );

  LogMessage msg( word1, word2 );

  if( answer == dontknow ) { 
    msg << "Magnus cannot determine whether " << Name( word1 ) << " and "
	<< Name( word2 ) << " are conjugate or not";
  }
  else {
    msg << Name( word1 ) << " and " << Name( word2 ) << " are";

    if( answer == no )
      msg << " not conjugate";
    else {
      msg << " conjugate: X " << Name( word1 ) << " x = "
	  << Name( word2 ) << ", where x = ";
      word1.getParent().getFPGroup().printWord( msg, conjugator );
    }
  }
  
  msg << ".";
  msg.send();
}

void HNNofFreeGroup_WordProblem::takeControl( )
{
  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Trichotomy answer = H.wordProblem( word.getWord() );

  LogMessage msg( word );

  if( answer == dontknow ) { 
    msg << "Magnus cannot determine whether " << Name( word ) << " is trivial or not.";
  }
  else {
    msg << Name( word ) << " is";
    if( answer == no )
      msg << " not";
    // @am changed wording
    msg << " trivial: follows from the normal form.";
  }
  
  msg.send();
}



void HNNofFreeGroup_ReducedForm::takeControl( )
{

  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Word reduced = H.reducedFormOf( word.getWord() );

  SMObject* smo = new SMWord( word.getParent(), reduced, 
			      Text("The reduced form of") + Name( word )
			      ); 
  ListOf<OID> dependencies( word.getParent() );
  CheckinMessage( *smo, "", dependencies ).send();
}

void HNNofFreeGroup_NormalForm::takeControl( )
{

  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Word normal = H.normalFormOf( word.getWord() );

  SMObject* smo = new SMWord( word.getParent(), normal, 
			      Text("The normal form of") + Name( word )
			      ); 
  ListOf<OID> dependencies( word.getParent() );
  CheckinMessage( *smo, "", dependencies ).send();
}


void HNNofFreeGroup_CyclicallyReducedForm::takeControl( )
{

  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Word conjugator;
  Word cycreduced = H.cyclicallyReducedFormOf( word.getWord(), conjugator );

  SMObject* smo = new SMWord( word.getParent(), cycreduced, 
			      Text("The cyclically reduced form of") + Name( word )
			      ); 
  ListOf<OID> dependencies( word.getParent() );
  CheckinMessage( *smo, "", dependencies ).send();
}

void HNNofFreeGroup_IsProperPower::takeControl( )
{
  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Word root;
  int power;

  Trichotomy answer = H.maximalRoot( word.getWord(), root, power );

  LogMessage msg( word );

  if( answer == dontknow ) { 
    msg << "Magnus cannot determine whether " << Name( word ) 
      // @am 'a' added 
	<< " is a proper power or not.";
  }
  else {
    if( power > 1 ) {
      msg << Name( word ) << " is a proper power. " << Name( word ) 
	  << " is equal ";
      H.printWord( msg, root );
      msg << " in power " << power;
    }
    else {
      msg << Name( word ) << " is not a proper power";
      if( power == 0 ) 
	msg << ": maximal root does not exist";
    }    
    msg << ".";
  }
  
  msg.send();
}

void HNNofFreeGroup_MaximalRoot::takeControl( )
{
  const HNNExtOfFreeGroup& H = word.getParent().gic.getHNNofFree();

  Word root;
  int power;
  Trichotomy answer = H.maximalRoot( word.getWord(), root, power );

  if( answer == dontknow ) { 
    LogMessage msg( word );
    msg << "Magnus cannot extract the maximal root of " << Name( word ) << ".";
    msg.send();
  }
  else {

    if( power == 0 ) {

      LogMessage msg( word );
      msg << "The maximal root of " << Name( word ) << " does not exist.";
      msg.send();
    }
    else if( power == 1 ) {

      LogMessage msg( word );
      msg << "The maximal root of " << Name( word ) << " is " 
	  << Name( word ) << ".";
      msg.send();
    }
    else {
      
      SMObject* smo =
	new SMWord( word.getParent(), root,
		    Text("The maximal root of") + Name( word )
		    );
      
      ListOf<OID> dependencies( word.getParent() );
      CheckinMessage( *smo, "", dependencies ).send();
    }
  }

}

void HNNofFreeGroup_AreEqual::takeControl( )
{
  const HNNExtOfFreeGroup& H = word1.getParent().gic.getHNNofFree();

  Trichotomy answer = H.areEqual( word1.getWord(), word2.getWord() );

  LogMessage msg( word1, word2 );

  if( answer == dontknow ) { 
    msg << "Magnus cannot determine whether " << Name( word1 ) << " is equal to "
	<< Name( word2 ) << " or not";
;
  }
  else {
    msg << Name( word1 ) << " is";
    if( answer == no )
      msg << " not";
    msg << " equal to " << Name( word2 );
  }

  msg << ".";
  msg.send();
}

void HNNofFreeGroup_IsProperPowerOfSecond::takeControl( )
{
  const HNNExtOfFreeGroup& H = word1.getParent().gic.getHNNofFree();

  LogMessage msg( word1, word2 );

  msg << Name( word1 ) << " is";
  int power = 0;
  bool answer = H.isProperPowerOfSecond( word1.getWord(), word2.getWord(), power );
  if( !answer )
    msg << " not";
  msg << " a proper power of " << Name( word2 );
  if( answer ) 
    msg << " ( " << Name(word2) << "^" << power << " = " << Name(word1) << " )";
  msg << ".";
  msg.send();
}


void HNNofFreeGroup_IsSGTrivial::takeControl( )
{
  const HNNExtOfFreeGroup& H = subgroup.getParent().gic.getHNNofFree();
  
  bool answer = H.isSubgroupTrivial( subgroup.getSubgroup().generators() );

  LogMessage msg( subgroup );

  msg << Name( subgroup ) << " is";
  if( !answer ) 
    msg << " not";
  msg << " trivial.";

  msg.send();
}



void HNNofFreeGroup_IsSGAbelian::takeControl( )
{
  const HNNExtOfFreeGroup& H = subgroup.getParent().gic.getHNNofFree();
  

  bool answer = H.isSubgroupAbelian( subgroup.getSubgroup().generators() );

  LogMessage msg( subgroup );

  msg << Name( subgroup ) << " is";
  if( !answer ) 
    msg << " not";
  msg << " abelian in " << Name( subgroup.getParent() ) << ".";
  
  msg.send();
}

void CheckinHNNofFreeGroup::takeControl( )
{
  Message msg;
  msg << 
    "Check-in the base group and two of its subgroups, which must "
    "have the same number of generators. The generators of the two "
    "subgroups must be arranged so that the mapping which sends the n-th "
    "generator of the first subgroup to the n-th generator of the second "
    "subgroup defines an isomorphism between them. Then go to the Make "
    "menu. (Note: HNN-extensions are, as of now, only implemented "
    "when the base group is checked in as free.)";
  msg.send();
}



// -------------------- Freeness of HNN-extension ---------------------- //

/*
HNNofFreeIsFreeArcer::~HNNofFreeIsFreeArcer( ) 
{ 
  delete theGroup;
}

void HNNofFreeIsFreeArcer::setArguments( const HNNExtOfFreeGroup& H )
{
  theGroup = new HNNExtOfFreeGroup(H);
}

void HNNofFreeIsFreeArcer::runComputation( )
{
  theAnswer = theGroup->isFree();
}
 
void HNNofFreeIsFreeArcer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
}

void HNNofFreeIsFreeArcer::readResults( istream& istr )
{
  istr > theAnswer;
}
*/

HNNofFreeIsFree::HNNofFreeIsFree( class SMFPGroup& F )
  : ComputationManager( true ), 
    theGroup( F ),
    theORProblems( 0 ),
    arcer( *this )
{
  if( !theGroup.gic.isOneRelator() ||
      theGroup.gic.isOneRelatorWithTorsion() )
    {
      resultIsFast();
      return;
    }
}

HNNofFreeIsFree::~HNNofFreeIsFree( )
{
  delete theORProblems;
  theORProblems = NULL;
}

void HNNofFreeIsFree::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is an HNN-extension of free group free"),
		  Text("Is an HNN-extension ") + Name( theGroup ) 
		  + Text(" of free group free"),
		  helpID("HNNofFreeFreeness", theGroup),
		  "Freeness"
		  );

  pv.startItemGroup();

  pv.add(Text("For use of this problem"), 
	 ThisARCSlotID(), 
	 50
	 );

  pv.done();
}

void HNNofFreeIsFree::takeControl( )
{
  if( fastResult() )
    {
      LogMessage msg( *this, theGroup );
      
      if( !theGroup.gic.isOneRelator() )
	msg << "There is no algorithm known at present which determines whether or not an HNN extension of free group is free.";
      else
	msg << Name( theGroup ) << " is one relator group with torsion, so it can't be free.";
      
      msg.send();
      adminTerminate();
      return;
    }
  
  if( freeARCs() > 0 ) {
    if( theORProblems == 0 ) 
      {
	theORProblems = 
	  new ORProblems( theGroup.getFPGroup().numberOfGenerators(),
			  theGroup.gic.getOneRelator() );
      
	arcer.setArguments(theORProblems);
      }
    else 
      if( arcer.takeControl() ) {
	
        bool answer = arcer.getRetValue();
	LogMessage msg( *this, theGroup );
	
	msg << Name( theGroup ) << " is ";
	
	if( !answer )
	  msg << "not ";
	
	msg << "free.";
	msg.send();
 	
	delete theORProblems;
	theORProblems = NULL;
	
	adminTerminate();
	return;
      }
      else
	usedOneARC();
  }
}


// ------- Freeness of an amalgamated product of free groups  -------- //

void APofFreeIsFreeArcer::setArguments( const AmalgProductOfFreeGroups& G )
{
  theGroup = new AmalgProductOfFreeGroups(G);
}

void APofFreeIsFreeArcer::runComputation( )
{
  theAnswer = theGroup->isFree();
}
 
void APofFreeIsFreeArcer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
}

void APofFreeIsFreeArcer::readResults( istream& istr )
{
  istr > theAnswer;
}


APofFreeIsFree::APofFreeIsFree( class SMFPGroup& F )
  : ComputationManager( true ), arcer( *this ), theGroup( F )
{
  arcer.setArguments( theGroup.gic.getAPOfFree() );
}

void APofFreeIsFree::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is an amalgamated product of free groups free"),
		  Text("Is an amalgamated product ") + Name( theGroup ) 
		  + Text(" of free groups free"),
		  helpID("APofFreeFreeness", theGroup),
		  "Freeness"
		  );

  pv.startItemGroup();

  pv.add(Text("For use of this problem"), ThisARCSlotID(), 50);

  pv.done();
  
}

void APofFreeIsFree::takeControl( )
{
  if( freeARCs() > 0 ) {

    if( arcer.takeControl() ) {

      Trichotomy answer = arcer.answer();
      LogMessage msg( *this, theGroup );
      if( answer == dontknow ) {
	msg << "Magnus cannot determine whether " << Name(theGroup) 
	    << " is free or not.";
      }
      else {
	theGroup.gic.putIsFree( answer );
	msg << Name( theGroup ) << " is";
	if( answer == no ) 
	  msg << " not";
	msg << " free.";
      }
      msg.send();
      adminTerminate();
      return;
    }

    usedOneARC();
  }
}

// ------- Is an amalgamated product of free groups perfect -------- //

APofFreeIsPerfect::APofFreeIsPerfect( class SMFPGroup& F )
  : Supervisor( true ),  theGroup( F ), 
    abelianInvariants( *this, F.gcm().abelianInvariants )
{
  if( theGroup.gic.haveCyclicDecomposition() ) // || theGroup.gic.isTrivial() ) 
    resultIsFast();
}

void APofFreeIsPerfect::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is an amalgamated product of free groups perfect"),
		  Text("Is an amalgamated product ") + Name( theGroup ) 
		  + Text(" of free groups perfect"),
		  helpID("APofFreePerfect", theGroup),
		  "Perfect"
		  );

  pv.startItemGroup();

  // pv.add("For use of this problem", ThisARCSlotID(), 50);
  pv.add( Text("Compute canonical decomposition of") + Name( theGroup ),
	  abelianInvariants.arcSlotID(),
	  50
	  );

  pv.done();
  
}

void APofFreeIsPerfect::takeControl( )
{
  if( theGroup.gic.haveCyclicDecomposition() ) {
    AbelianGroup A = theGroup.gic.getCyclicDecomposition();
    Trichotomy answer = A.isTrivial();
    
    LogMessage msg( *this, theGroup );
    msg << Name( theGroup ) << " is";
    if( answer == no ) 
      msg << " not";
    msg << " perfect.";
    msg.send();
    
    adminTerminate();
    return;
  }
}


// ------- Homology of an amalgamated product of free groups  -------- //

void APofFreeHomologyArcer::setArguments( const AmalgProductOfFreeGroups& G ) 
{
  theGroup = new AmalgProductOfFreeGroups(G);
}

void APofFreeHomologyArcer::runComputation( )
{
  int r = theGroup->factor( LEFT_FACTOR ).rank();
  int s = theGroup->factor( RIGHT_FACTOR ).rank();
  int m = theGroup->subgroup( LEFT_FACTOR ).generators().length();
  VectorOf<Word> relators( m );
  for( int i = 0; i < m; ++i ) {
    relators[i] = theGroup->subgroup( LEFT_FACTOR ).generators()[i] 
                  * theGroup->subgroup( RIGHT_FACTOR ).generators()[i].inverse();
  }
  AbelianGroup F( r+s );
  F.computeCyclicDecomposition();
  AbelianGroup H( F.makeSubgroupPresentation(relators) );
  H.computeCyclicDecomposition();
  theRankOfH2 = H.rankOfFreeAbelianFactor();
}

void APofFreeHomologyArcer::writeResults( ostream& ostr )
{
  ostr < theRankOfH2;
}

void APofFreeHomologyArcer::readResults( istream& istr )
{
  istr > theRankOfH2;
}


APofFreeHomologyProblem::APofFreeHomologyProblem( class SMFPGroup& F )
  : Supervisor( true ), theGroup( F ), cyclicSubgroup(false),
    arcer( *this ), arcerStarted( false ),
    abelianInvariants( *this, F.gcm().abelianInvariants ), abDone( false )
{
  cyclicSubgroup = ( theGroup.gic.getAPOfFree().subgroup(LEFT_FACTOR)
		     .generators().length() <= 1 
		     );
  if( theGroup.gic.haveCyclicDecomposition() ) {
    abDone = true;
    if( cyclicSubgroup ) 
      resultIsFast();
  }  

  adminStart();
}

void APofFreeHomologyProblem::takeControl( )
{
  if( freeARCs() > 0 ) {

    if( theGroup.gic.haveCyclicDecomposition() ) {

      int H2 = -1;

      if( cyclicSubgroup ) {
	const AmalgProductOfFreeGroups& G = theGroup.gic.getAPOfFree();
	AbelianWord rel( G.numberOfGenerators(), 
			 G.subgroup( LEFT_FACTOR ).generators()[0]
			 );
	H2 = ( rel.isTrivial() ? 0 : 1 );
      }
      else {

	if( !arcerStarted ) {
	  arcer.setArguments( theGroup.gic.getAPOfFree() );
	  arcerStarted = true;
	}
	if( arcer.takeControl() ) {
	  H2 = arcer.rankOfH2();
	}
      }
      
      if( H2 != -1 ) {

	const AbelianGroup& A = theGroup.gic.getCyclicDecomposition();
	LogMessage msg( *this, theGroup );
	msg << "Homology of the group " << Name(theGroup) << ": "
	    << " H_1(" << Name(theGroup) << ") = " << A
	    << ", H_2(" << Name(theGroup) << ") = ";
	switch( H2 ) {
	case 0:  msg << '0'; break;
	case 1:  msg << 'Z'; break;
	default: msg << "Z^" << H2;
	}
	msg << ",  H_n(" << Name(theGroup) << ") = 0, n>2.";
	msg.send();
	adminTerminate();
	return;
      }

      usedOneARC();
    }
  }

}


void APofFreeHomologyProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Integral homology of an amalgamated product of free groups"),
		  Text("Compute integral homology of the amalgamated product ") 
		  + Name( theGroup ) + Text(" of free groups"),
		  helpID("APofFreeHomology", theGroup),
		  "Homology"
		  );

  pv.startItemGroup();

  pv.add( Text("For use of this problem"), ThisARCSlotID(), 50 );

  if( !abDone )
    pv.add( Text("Compute abelian invariants of") + Name( theGroup ),
	    abelianInvariants.arcSlotID(), 50
	    );


  pv.done();
  
}


