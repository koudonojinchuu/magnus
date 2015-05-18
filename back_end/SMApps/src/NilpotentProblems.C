// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes NilpotentWP, NilpotentCenterProblem,
//           NilpotentCentralizersProblem,
//           NilpotentIsAbelianProblemARCer, NilpotentIsAbelianProblem,
//           NilpotentOrderProblemARCer, NilpotentOrderProblem,
//           NilpotentOrderOfEltProblemARCer, NilpotentOrderOfEltProblem,
//           NilpotentIsSGTrivialProblemARCer, NilpotentIsSGTrivialProblem,
//           NilpotentIsSGFiniteProblemARCer, NilpotentIsSGFiniteProblem,
//           NilpotentIsSGAbelianProblemARCer, NilpotentIsSGAbelianProblem
//
// Principal Author: Roger Needham, Dmitry Bormotov, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// * 2/96 Dmitry B. added new classes:
//
//   NilpotentIsAbelianProblemARCer, NilpotentIsAbelianProblem,
//   NilpotentOrderProblemARCer, NilpotentOrderProblem,
//   NilpotentOrderOfEltProblemARCer, NilpotentOrderOfEltProblem,
//   NilpotentIsSGTrivialProblemARCer, NilpotentIsSGTrivialProblem,
//   NilpotentIsSGFiniteProblemARCer, NilpotentIsSGFiniteProblem,
//   NilpotentIsSGAbelianProblemARCer, NilpotentIsSGAbelianProblem
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "NilpotentProblems.h"
#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMMap.h"
#include "SMSubgroup.h"
#include "SMHomomorphism.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"

// ------------------- NGOrderOfEltProblemARCer --------------------- //


void NGOrderOfEltProblemARCer::runComputation( )
{
  retValue = theNG->orderOf(theWord);
}


void NGOrderOfEltProblemARCer::setArguments( const NilpotentGroup& NG,
						    const Word& w )
{
  theNG = new NilpotentGroup(NG);
  theWord = w;
}


void NGOrderOfEltProblemARCer::writeResults( ostream& out ) 
{
  out < retValue;
}

void NGOrderOfEltProblemARCer::readResults( istream& in )
{
  in > retValue;
}

// ---------------------- NGorderOfEltProblem ----------------------- //


NGorderOfEltProblem::NGorderOfEltProblem(SMWord& w)
  : Supervisor( true ),
    theWord( w ),
    theGIC( w.getParent().gic ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    started( false ),
    arcer( *this )
{ 
  if (theWord.wic.haveOrder())
    resultIsFast();
}


void NGorderOfEltProblem::takeControl( )
{
  if (!fastResult())
    if (freeARCs() > 0){
      if (theGIC.haveNilpotentGroupInited()){
	if (!started){
	  arcer.setArguments(theGIC.getNilpotentGroupInited(),
			     theWord.getWord());
	  started = true;
	}
	if (arcer.takeControl()){
	  theWord.wic.putHaveOrder(arcer.getOrder());
	}
      }
      usedOneARC();
    }
  if (theWord.wic.haveOrder()){
    LogMessage msg( *this, theWord );
    Integer theOrder = theWord.wic.getOrder();
    msg << "The order of " << Name( theWord ) << " is ";

    if( theOrder == 0 )
      msg << "infinite";
    else
      msg << theOrder;
    
    msg << ".";
    msg.send();
    adminTerminate();
  }
}


void NGorderOfEltProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find the order"),
		  Text("Find the order of ") + Name( theWord ),
		  helpID("NGorderOfEltProblem", theWord.getParent()),
		  Text("order") 
		  );
  
  pv.startItemGroup();

  if (!theGIC.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    );
  pv.add(Text("Compute the order of ") + Name( theWord ), ThisARCSlotID(), 100);
  pv.done();
}




// ---------------------- NGHirschNumberProblem ----------------------- //


NGHirschNumberProblem::NGHirschNumberProblem(SMFPGroup& group)
  : Supervisor( true ),
    theGroup( group ),
    theGIC( group.gic ),
    computeBasis( *this, group.gcm().computeBasis )
{ 
  if (theGIC.haveNilpotentGroupInited())
    resultIsFast();
}


void NGHirschNumberProblem::takeControl( )
{
  if (theGIC.haveNilpotentGroupInited()){
    LogMessage msg( *this, theGroup );
    msg << Text("The Hirsch number of the group ") << Name(theGroup) 
	<< " is " << theGIC.getNilpotentGroupInited().theHirschNumber() << ".";
    msg.send();
    
    adminTerminate();
  }
}


void NGHirschNumberProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the Hirsch number"),
		  Text("Compute the Hirsch number of") + Name( theGroup ),
		  helpID("NGHirschNumberProblem", theGroup),
		  Text("Hir num") 
		  );
  
  pv.startItemGroup();
  
  pv.add( Text("Compute a basis of ")
	  + Name( theGroup ),
	  computeBasis.arcSlotID(),
	  100
	  );
  pv.done();
}



// ---------------------- NGcomputeNClassProblem ----------------------- //


NGcomputeNClassProblem::NGcomputeNClassProblem(SMFPGroup& group)
  : Supervisor( true ),
    theGroup( group ),
    theGIC( group.gic ),
    abelianInvariants( *this, group.gcm().abelianInvariants ),
    computeBasis( *this, group.gcm().computeBasis )
    //    computeLCSQuotientsComp( *this, group.gcm().computeLCSQuotientsComp )
{ 
  if (theGIC.haveCyclicDecomposition()) {
    const AbelianGroup& ag = theGIC.getCyclicDecomposition();
    if ((ag.rankOfFreeAbelianFactor() == 0 && ag.invariants().length() ==1) ||
	(ag.rankOfFreeAbelianFactor() == 1 && ag.invariants().length() ==0))
      theGIC.putActualNilpotencyClass(1);
  }
  if (theGIC.haveActualNilpotencyClass())
      resultIsFast();
}


void NGcomputeNClassProblem::takeControl( )
{
  if (theGIC.haveCyclicDecomposition()) {
    const AbelianGroup& ag = theGIC.getCyclicDecomposition();
    if ((ag.rankOfFreeAbelianFactor() == 0 && ag.invariants().length() ==1) ||
	(ag.rankOfFreeAbelianFactor() == 1 && ag.invariants().length() ==0))
      theGIC.putActualNilpotencyClass(1);
  }  
  if (theGIC.haveActualNilpotencyClass()){
      LogMessage msg( *this, theGroup);
      msg << Text("The nilpotency class of the group ") << Name(theGroup)  << " is " 
	  << theGIC.getActualNilpotencyClass() << ".";
      msg.send();
      adminTerminate();
  }
}


void NGcomputeNClassProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the nilpotency class"),
		  Text("Compute the nilpotency class of")
		  + Name( theGroup ),
		  helpID("NGcomputeNClassProblem", theGroup),
		  Text("class of ")+Name(theGroup)
		  );
  
  pv.startItemGroup();
  if (!theGIC.haveCyclicDecomposition())
    pv.add( Text("Compute abelianization of")
	    + Name( theGroup ),
	    abelianInvariants.arcSlotID(),
	    50
	    );  
  
  if (!theGIC.haveNilpotentQuotInited(theGIC.getNilpotentcyClass()))
    pv.add( Text("Compute a basis of ")
	    + Name( theGroup ),
	    computeBasis.arcSlotID(),
	    100
	    );
  
  /*  if (!theGIC.haveLCSQuotients())
    pv.add( Text("Compute the lower central factors of")
	    + Name( theGroup ),
	    computeLCSQuotientsComp.arcSlotID(),
	    100
	    );
	    */
  pv.done();
}
// ---------------------- NGisFreeNilpotentProblem ----------------------- //


NGisFreeNilpotentProblem::NGisFreeNilpotentProblem(SMFPGroup& group)
  : Supervisor( true ),
    theGroup( group ),
    theGIC( group.gic ),
    abelianInvariants( *this, group.gcm().abelianInvariants ),
    computeBasis( *this, group.gcm().computeBasis ),
    //    computeLCSQuotientsComp( *this, group.gcm().computeLCSQuotientsComp ),
    answer(dontknow)
{ 
  if (theGIC.haveNilpotentGroupInited()){
    answer = theGIC.getNilpotentGroupInited().
      isFreeNilpotent(&freeRank,&freeClass );
    resultIsFast();
    return;
  }
  if (theGIC.haveCyclicDecomposition())
    if (!theGIC.getCyclicDecomposition().isFree()){
      answer = no;
      resultIsFast();
    }
}


void NGisFreeNilpotentProblem::takeControl( )
{
  Chars explanation;
  if (!fastResult()){
    if (theGIC.haveNilpotentGroupInited())
      answer = theGIC.getNilpotentGroupInited().
	isFreeNilpotent(&freeRank,&freeClass );
    if (theGIC.haveCyclicDecomposition())
      if (!theGIC.getCyclicDecomposition().isFree()){
	explanation = Chars(": abelianization of ")+ Text(Name(theGroup))+ " is not free abelian group";
	answer = no;
      }
  }
  if (answer != dontknow){
    LogMessage msg( *this, theGroup );
    msg << "The group " << Name( theGroup ) << " is";
    if ( answer == no ){
      msg << " not free nilpotent";
      msg << explanation;
      msg << ".";
    }    
    else {
      if( freeRank == 0 && freeClass == 0 )
	msg << " trivial.";
      else
	msg << " free nilpotent of rank " << freeRank 
	    << " and class " << freeClass << ".";
    }
    msg.send();    
    adminTerminate();
  }
}


void NGisFreeNilpotentProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is the group free nilpotent?"),
		  Text("Is") + Name( theGroup ) + "free nilpotent ?",
		  helpID("NGisFreeNilpotentProblem", theGroup),
		  Text(Name(theGroup)) + "free nilp?" 
		  );
  
  pv.startItemGroup();
  
  if (!theGIC.haveNilpotentQuotInited(theGIC.getNilpotentcyClass()))
    pv.add( Text("Compute a basis of ")
	    + Name( theGroup ),
	    computeBasis.arcSlotID(),
	    100
	    );

  pv.add( Text("Compute abelianization of")
	  + Name( theGroup ),
	  abelianInvariants.arcSlotID(),
	  50
	  );  
  /*  pv.add( Text("Compute the lower central factors of")
	  + Name( theGroup ),
	  computeLCSQuotientsComp.arcSlotID(),
	  100
	  );
	  */
  pv.done();
}

// ---------------------- NGdecomposeWordProblem ----------------------- //


NGdecomposeWordProblem::NGdecomposeWordProblem(SMWord& word)
  : Supervisor( true ),
    theWord( word ),
    theWIC( word.wic ),
    computeBasis( *this,  word.getParent().gcm().computeBasis ),
    ngDecomposeWord(*this, word.wcm().ngDecomposeWord)
{ 
  if (theWIC.haveCollectedForm())
    resultIsFast();
}


void NGdecomposeWordProblem::takeControl( )
{

  if (theWIC.haveCollectedForm()){
    LogMessage msg1( *this, theWord );
    LogMessage msg2( *this, theWord );
    const NilpotentGroup& G = theWord.getParent().gic.getNilpotentGroupInited();
    if (theWord.getParent().gic.isFreeNilpotent() == yes){
      msg1 << "The canonical form of " << Name( theWord ) 
       << ", expressed as product of basic commutators: ";
      msg1 << G.asDecomposition(theWIC.getCollectedForm());
      msg2 << "The canonical form of " << Name( theWord ) 
	   << ", where the basic commutators are expressed directly "
	   << "in terms of the given generators: ";
      G.printWord(msg2,G.toWord(theWIC.getCollectedForm()));
      //      msg << G.toBracketedWordSL(theWIC.getCollectedForm());
    }
    else{
      msg1 << "Decomposition of " << Name( theWord ) << " in terms of basis: ";
      msg1 <<G.asDecomposition( theWIC.getCollectedForm() );
      msg2 << "The canonical form of " << Name( theWord ) 
	   << ", where the basic commutators are expressed directly "
	   << "in terms of the given generators: ";
      G.printWord(msg2,G.toWord(theWIC.getCollectedForm()));
    }
    msg1 << ".";
    msg2 << ".";
    msg1.send();    
    msg2.send();    
    adminTerminate();
  }
}



void NGdecomposeWordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Decompose word in terms of a basis"),
		  Text("Decompose ") + Name( theWord ) + 
		  Text("in terms of a basis of ")+
		  Name(theWord.getParent()),
		  helpID("NGdecomposeWordProblem", theWord.getParent()),
		  Text("decomp") +Name(theWord) 
		  );
  
  pv.startItemGroup();
  
  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    );

  pv.add( Text("Decompose ")
	  + Name( theWord ),
	  ngDecomposeWord.arcSlotID(),
	  100
	  );
  pv.done();
}

// ---------------------- NGisWordInCommutatorSGProblem ----------------------- //


NGisWordInCommutatorSGProblem::NGisWordInCommutatorSGProblem(SMWord& word)
  : Supervisor( true ),
    theWord( word ),
    theGIC( word.getParent().gic ),
    abelianInvariants( *this, word.getParent().gcm().abelianInvariants )
{ 
  if (theGIC.haveCyclicDecomposition())
      resultIsFast();
}


void NGisWordInCommutatorSGProblem::takeControl( )
{
  if (theGIC.haveCyclicDecomposition()){
    LogMessage msg( *this,theWord );
    msg << Name( theWord ) << " is "; 
    if(!theGIC.getCyclicDecomposition().isTrivial(theWord.getWord()))  msg << "not ";
    msg << "in the commutator subgroup of " << Name( theWord.getParent() ) << 
      ": its image in the abelianization of "<< Name(theWord.getParent()) << " is ";
    if(!theGIC.getCyclicDecomposition().isTrivial(theWord.getWord()))  msg << "non-";
    msg << "trivial.";
    msg.send();
    adminTerminate();
  }
}


void NGisWordInCommutatorSGProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Decide if word is in the commutator subgroup"),
		  Text("Is") + Name(theWord) + Text("an element of the commutator subgroup in")
		  + Name( theWord.getParent() ),
		  helpID("NGisWordInCommutatorSGProblem", theWord.getParent()),
		  Text(Name(theWord)) + "in comm subg?" 
		  );
  
  pv.startItemGroup();
  
  pv.add( Text("Compute abelianization of")
	  + Name( theWord.getParent() ),
	  abelianInvariants.arcSlotID(),
	  50
	  );  
  pv.done();
}
// ------------------- NGweightOfWordARCer --------------------- //


void NGweightOfWordARCer::runComputation( )
{
  weight = theNG->weightOf(theWord);
}


void NGweightOfWordARCer::setArguments( const NilpotentGroup& NG,
					 const Word& w )
{
  theNG = new NilpotentGroup(NG);
  theWord = w;
}


void NGweightOfWordARCer::writeResults( ostream& out ) 
{
  out < weight;
}

void NGweightOfWordARCer::readResults( istream& in )
{
  in > weight;
}

// ---------------------- NGweightOfWordProblem ----------------------- //


NGweightOfWordProblem::NGweightOfWordProblem(SMWord& word)
  : Supervisor( true ),
    theWord( word ),
    theWIC( word.wic ),
    computeBasis( *this,  word.getParent().gcm().computeBasis ),
    arcer(*this),
    weight(-1),
    started(false)
{ 
  if (theWIC.haveCollectedForm())
    resultIsFast();
}


void NGweightOfWordProblem::takeControl( )
{
  if (fastResult()){
    weight = theWord.getParent().gic.getNilpotentGroupInited()
      .weightOf(theWIC.getCollectedForm());
  }
  else
    if (freeARCs() > 0){
      if (theWord.getParent().gic.haveNilpotentGroupInited()){
	if (!started){
	  arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			     theWord.getWord());
	  started = true;
	}
	if (arcer.takeControl() )
	  weight = arcer.getWeight();
      }
      usedOneARC();
    }
  
  if (weight >-1){
    LogMessage msg( *this, theWord );
    msg << Name( theWord ) << " lies in the " << weight << ordinalPostfix(weight) 
	<< " term of the lower central series: " 
	<<"this follows from an inspection of its canonical form.";
    msg.send();
    adminTerminate();
  }
}



void NGweightOfWordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Term of the lower central series problem"),
		  Text("In which term of the lower central series does") +
		  Name( theWord ) + "lies.",
		  helpID("NGweightOfWordProblem", theWord.getParent()),
		  Text("Term of l.c.s. ?")
		  );
  
  pv.startItemGroup();
  
  if (!theWord.getParent().gic.haveNilpotentQuotInited
      (theWord.getParent().gic.getNilpotentcyClass()))
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    );

   pv.add( Text("Determine in which term of the lower central series ")
	  + Name( theWord ),
	  ThisARCSlotID(),
	  100
	  );
  pv.done();
}
// ---------------------- NGorderOfTorsionSubgroupProblem ----------------------- //


NGorderOfTorsionSubgroupProblem::NGorderOfTorsionSubgroupProblem(SMFPGroup& group)
  : Supervisor( true ),
    theGroup( group ),
    theGIC( group.gic ),
    computeBasis( *this, group.gcm().computeBasis )
    //    computeLCSQuotientsComp( *this, group.gcm().computeLCSQuotientsComp )
{ 
  if (theGIC.haveNilpotentGroupInited())
    resultIsFast();
}


void NGorderOfTorsionSubgroupProblem::takeControl( )
{
  if (theGIC.haveNilpotentGroupInited()){
      LogMessage msg( *this, theGroup );
      msg << "The order of the torsion subgroup of  " << Name( theGroup ) << " is ";
      msg << theGIC.getNilpotentGroupInited().
	orderOfTheTorsionSubgroup( ) << ".";
      msg.send();    
      adminTerminate();
  }
}


void NGorderOfTorsionSubgroupProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find the order of the torsion subgroup"),
		  Text("Find the order of the torsion subgroup of ")
		  + Name( theGroup ), 
		  helpID("NGorderOfTorsionSubgroupProblem", 
			 theGroup), "Ord tor subgp"
		  );

  pv.startItemGroup();
  
  if (!theGIC.haveNilpotentQuotInited(theGIC.getNilpotentcyClass()))
    pv.add( Text("Compute a basis of ")
	    + Name( theGroup ),
	    computeBasis.arcSlotID(),
	    100
	    );

  /*  pv.add( Text("Compute the lower central factors of")
	  + Name( theGroup ),
	  computeLCSQuotientsComp.arcSlotID(),
	  100
	  );
	  */
  pv.done();
}
//---------------------------------------------------------------------------//
//-------------------- NGbuildPresentationProblem --------------------------//
//---------------------------------------------------------------------------//


NGbuildPresentationProblem::NGbuildPresentationProblem( SMFPGroup& group)
  : Supervisor( true ),
    theGroup( group ),
    arcer( *this),
    started(false),
    computeBasis( *this, group.gcm().computeBasis )
{
  
}


void NGbuildPresentationProblem::takeControl( )
{
   if (freeARCs() > 0){
     if (theGroup.gic.haveNilpotentGroupInited()){
       if (!started){
	 arcer.setArguments(theGroup.gic.getNilpotentGroupInited());    
	 started = true;
       }
       if (arcer.takeControl() ){
	 File file;
	 arcer.getPresentation().print(file);
	 LogMessage msg( *this, theGroup);
	 if (theGroup.gic.isFreeNilpotent() == yes)
	   msg << Link(Text("Click here to view the structure constants of ") + 
			Name(theGroup) + "." ,
			"FNGStructureConstants", 
			file.getFileName());	   
	 else
	   msg << Link(Text("Click here to view the presentation of ") +
		       Name(theGroup) + "." ,
		       "FNGStructureConstants", file.getFileName());
	 msg.send();
	 adminTerminate();
	 return;
       }
     }
     usedOneARC();
   }
}  


void NGbuildPresentationProblem::start( ) { }


void NGbuildPresentationProblem::terminate( ) { }

void NGbuildPresentationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the presentation"),
		  Text("Compute the presentation of")
		  + Name( theGroup ),
		  helpID("NGbuildPresentationProblem", theGroup),
		  Text("Pres of") + Name( theGroup )
		  );

  pv.startItemGroup();
  if (!theGroup.gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theGroup ),
	    computeBasis.arcSlotID(),
	    100
	    );
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 100);
  pv.done();
}

// ---------------------- NGAutoIsIAAut ----------------------- //


NGAutoIsIAAut::NGAutoIsIAAut(SMMap& map)
  : Supervisor( true ),
    theMap( map ),
    theGIC( map.getDomain().gic ),
    abelianInvariants( *this, map.getDomain().gcm().abelianInvariants )
{ 
  if (theGIC.haveCyclicDecomposition())
    resultIsFast();
}


void NGAutoIsIAAut::takeControl( )
{
  if (theGIC.haveCyclicDecomposition()){
    int theNumberOfGenerators = theMap.getDomain().getFPGroup().numberOfGenerators();
    VectorOf<Word> V =  theMap.getMap().generatingImages();
    bool answer = true;
    const AbelianGroup& abGroup = theGIC.getCyclicDecomposition();
    for( int i = theNumberOfGenerators-1; i >= 0; --i ) {
      if( !abGroup.isTrivial(inv(Generator(i+1)) * V[i]) )
	answer = false;
    }
    LogMessage msg( theMap );
    msg << Name( theMap ) << " is ";    
    if( !answer )
      msg << "not ";
    
    msg << "an IA-automorphism of " << Name( Domain( theMap ) ) << ".";
    msg.send();
    adminTerminate();
  }
}

void NGAutoIsIAAut::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Test for an automorphism being an IA-automorphism"),
		  Text("Is") + Name( theMap  ) + "an IA-automorphism?",
		  helpID("NGAutoIsIAAut", theMap.getDomain()),
		  "Is an IA-auto?" 
		  );
  
  pv.startItemGroup();
  
  pv.add( Text("Compute abelianization of")
	  + Name( theMap.getDomain() ),
	  abelianInvariants.arcSlotID(),
	  50
	  );  
  pv.done();
}
 
// ----------------- NGisCentralARCer ------------------------ //

void NGisCentralARCer::runComputation( )
{
  theAnswer = theNG->isCentral( theWord );  
}


void NGisCentralARCer::setArguments( const NilpotentGroup& NG,
					     const Word& w)
{
  theNG = new NilpotentGroup( NG );
  theWord = w;
}


void NGisCentralARCer::writeResults( ostream& out ) 
{
  out < theAnswer;
}

void NGisCentralARCer::readResults( istream& in )
{
  in > theAnswer;
}

//---------------------------------------------------------------------------//
//-------------------- NGisCentralProblem ---------------------------//
//---------------------------------------------------------------------------//


NGisCentralProblem::NGisCentralProblem(SMWord& w)
  : Supervisor( true ),
    theWord( w ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    answer(dontknow),
    started( false ),
    arcer( *this )
{
  if( w.getWord().freelyReduce().length() == 0) {
    answer = true;
    resultIsFast();
    return;
  }
  //  if( w.wic.haveCollectedForm() ) resultIsFast();
}


void NGisCentralProblem::takeControl( )
{
  if( fastResult() ) {
    if(answer == dontknow) {
      /*      const NilpotentGroup& NGroup = 
	theWord.getParent().gic.getNilpotentGroupInited();
      Generator firstLetter = theWord.wic.getCollectedForm().firstLetter().gen;
      answer = ( ord(firstLetter) >= 
		 NGroup.firstOfWeight( NGroup.nilpotencyClass() ) 
		 );
		 */
    }
  }
  else {
    if ( freeARCs() <= 0 ) return;
    if 	(theWord.getParent().gic.haveNilpotentGroupInited()){
      if (!started){
	arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			   theWord.getWord());    
	started = true;
      }
      if( arcer.takeControl() ) {
	answer = arcer.getAnswer();
      }
      else {
	usedOneARC();
	return;
      }
    }  
  }
  LogMessage msg( *this, theWord );
  msg << Name( theWord ) << " is ";
  if ( answer == no ) msg << "not ";
  msg << "central in " << Name( Parent( theWord ) ) << ".";
  msg.send();

  adminTerminate();

}

void NGisCentralProblem::start( ) { }


void NGisCentralProblem::terminate( ) { }


void NGisCentralProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Central element problem"),
		  Text("Is") + Name( theWord ) + "central in"
		  + Name( Parent( theWord ) ) + "?",
		  helpID("NGisCentralProblem", theWord.getParent()),
		  Text("Is") + Name(theWord) 
		  + "central ?"
		  );

  pv.startItemGroup();
  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    ); 
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 100);
  pv.done();
}

 
// ---------------------- NGMaximalRootARCer --------------------------- //


void NGMaximalRootARCer::setArguments( const NilpotentGroup& group,
					const Word& w)
{
  NG = new NilpotentGroup( group );
  theWord = w;
}


void NGMaximalRootARCer::runComputation( )
{
#if SAFETY > 0
  if( NG == 0 )
    error( "void NGMaximalRootARCer::runComputation( ) : "
	   "You have not initialized the group");
#endif
 PolyWord root;
 NG->maximalRoot(theWord, root, thePower);
 theRoot = NG->toWord(root);
}


void NGMaximalRootARCer::writeResults( ostream& out ) 
{
  out < theRoot;
  out < thePower;
}


void NGMaximalRootARCer::readResults( istream& in )
{
  in > theRoot;
  in > thePower;
}


int NGMaximalRootARCer::getPower() const
{
#if SAFETY > 0
  if( NG == 0 )
    error( "int NGMaximalRootARCer::getPower() const : "
	   "You have not initialized the group");
#endif
  
  return thePower;
}

Word NGMaximalRootARCer::getRoot() const
{
#if SAFETY > 0
  if( NG == 0 )
    error( "Word NGMaximalRootARCer::getRoot() const : "
	   "You have not initialized the group");
#endif
  
  return theRoot;
}

//---------------------------------------------------------------------------//
//-------------------- NGMaximalRootProblem----- ---------------------------//
//---------------------------------------------------------------------------//


NGMaximalRootProblem::NGMaximalRootProblem(SMWord& w)
  : Supervisor( true ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    theWord( w ),
    started(false),
    arcer( *this )
{
  if( theWord.wic.isPower() != dontknow )
    resultIsFast();
}


void NGMaximalRootProblem::takeControl( )
{
  if(!fastResult() ) {
    if ( freeARCs() > 0 ) {
      if (theWord.getParent().gic.haveNilpotentGroupInited()){
	if (!started){
	  arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			     theWord.getWord());    	  
	  started = true;
	}
	if( arcer.takeControl() ) {
	  theWord.wic.putHaveMaximalRoot(arcer.getRoot(),arcer.getPower());
	}
      }
      usedOneARC();
    }
  }  
  
  // reporting result
  if (theWord.wic.isPower() != dontknow){
    LogMessage msg( *this, theWord);
    if (!theWord.wic.isPower()){
      msg << Name(theWord) <<" is not a power, so it does not have maximal root.";
    }else {
      Word root;
      Integer exp;
      theWord.wic.getMaximalRoot(root,exp);
      msg << "Maximal root of "<<Name(theWord ) << " is ";
      theWord.getParent().getFPGroup().printWord(msg,root);
      msg << " and " << exp << ordinalPostfix(exp.as_long())<<" power of it is "
	  << Name(theWord );
    }
    msg.send(); 
    
    adminTerminate();
  }
}

void NGMaximalRootProblem::start( ) { }


void NGMaximalRootProblem::terminate( ) { }

void NGMaximalRootProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the maximal root of element"),
		  Text("Compute the maximal root of ")
		  + Name( theWord )  ,
		  helpID("NGMaximalRootProblem", theWord.getParent()), 
		  "Max root"
		  );
  pv.startItemGroup();
  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    ); 
  pv.add(Text("Compute maximal root"), ThisARCSlotID(), 100);
  pv.done();
}

//---------------------------------------------------------------------------//
//-------------------- NGIsProperPower  ------------------------------------//
//---------------------------------------------------------------------------//


NGIsProperPower::NGIsProperPower(SMWord& w)
  : Supervisor( true ),
    theWord( w ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    started(false),
    arcer( *this )
{
  if( theWord.wic.isPower() != dontknow )
    resultIsFast();
}


void NGIsProperPower::takeControl( )
{
  if(!fastResult() ) {
    if ( freeARCs() <= 0 ) return;
    if (theWord.getParent().gic.haveNilpotentGroupInited()){
      if (!started){
	arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			   theWord.getWord());    	
	started = true;
      }
      if( arcer.takeControl() ) {
	theWord.wic.putHaveMaximalRoot(arcer.getRoot(),arcer.getPower());
      }
      else {
	usedOneARC();
	return;
      }
    }
  }  
  
  // reporting result

  if( theWord.wic.isPower() != dontknow ){
    LogMessage msg( *this, theWord);
    bool answer = false;
    if (theWord.wic.isPower()){
      Word root;
      Integer exp;
      theWord.wic.getMaximalRoot(root,exp);
      if (exp > 1)
	answer = true;
    }
    msg << Name (theWord) << " is ";
    if (!answer) msg << " not ";
    msg << "a proper power.";
    msg.send(); 
    
    adminTerminate();
  }
}

void NGIsProperPower::start( ) { }


void NGIsProperPower::terminate( ) { }

void NGIsProperPower::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is element a proper power"),
		  Text("Find is the ") + Name( theWord ) 
		  + Text(" a proper power"),
		  helpID("NGIsProperPower", theWord.getParent()),
		  Text("Powr prob")
		  );

  pv.startItemGroup();
  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name(theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    ); 
  pv.add(Text("For use of this problem"), ThisARCSlotID(), 100);
  pv.done();
}

// ---------------------- NGInverseAutoARCer --------------------------- //


void NGInverseAutoARCer::setArguments( const NilpotentGroup& group, 
					    const VectorOf<Word>& images)
{
  NG = new NilpotentGroup( group );
  theImages = images;
}


void NGInverseAutoARCer::runComputation( )
{
#if SAFETY > 0
  if( NG == 0 )
    error( "void NGInverseAutoARCer::runComputation( ) : "
	   "Nilpotent group has not been computed yet");
#endif

  result = NG->inverseAuto(theImages);
}


void NGInverseAutoARCer::writeResults( ostream& out ) 
{
  out < result;
}


void NGInverseAutoARCer::readResults( istream& in )
{
  in > result;
}

// ------------------- NGInverseAuto -----------------------//

NGInverseAuto::NGInverseAuto(const SMHomomorphism& aut)
  : Supervisor( true ),
    theAuto(aut ),
    arcer(*this)
{
  if (theAuto.mic.isEpi()!=yes)
    error("Don't know is it automorphism!!!");
  else{
    FPGroup group = theAuto.getDomain().getFPGroup();
    NilpotentGroup  ng(group.namesOfGenerators(),
		       theAuto.getDomain().gic.getNilpotentcyClass(),
		       makeVectorOf(group.getRelators()));   
    arcer.setArguments(ng,theAuto.getMap().generatingImages());
  }    
}


void NGInverseAuto::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()){
      Map inverseMap(theAuto.getDomain().getFPGroup(),
		     theAuto.getDomain().getFPGroup(),
		     arcer.getInverse());
      SMHomomorphism* smo = new SMHomomorphism( theAuto.getDomain(), 
						inverseMap,
						Text("The inverse of ")+Name(theAuto));
      IsAuto IA( *smo );
      FEDataUpdate( IA, True() ).send();
      smo->mic.putIsEpi(true);

      ListOf<OID> dependencies( theAuto.getDomain() );
      CheckinMessage( *smo, "", dependencies ).send();
      adminTerminate();
      return;
      }
    usedOneARC();
  }
}


void NGInverseAuto::viewStructure(ostream& ostr) const
{
  char *temp;
    
  ProblemView pv( ostr, oid(), Text("Find inverse of an automorphism"),
		  Text("Find inverse of ")+Name(theAuto),
		  helpID("NGInverseAuto", theAuto.getDomain()),
		  Text("inverse")
		 );
  
  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.done();
}

 
// ---------------------- NGcentralizerARCer --------------------------- //


void NGcentralizerARCer::setArguments( const NilpotentGroup& group,
					const Word& w)
{
  NG = new NilpotentGroup( group );
  theWord = w;
}


void NGcentralizerARCer::runComputation( )
{
#if SAFETY > 0
  if( NG == 0 )
    error( "void NGcentralizerARCer::runComputation( ) : "
	   "You have not initialized the group");
#endif
 result = NG->centralizer(theWord);
}


void NGcentralizerARCer::writeResults( ostream& out ) 
{
  out < result;
}


void NGcentralizerARCer::readResults( istream& in )
{
  in > result;
}


const VectorOf<Word>& NGcentralizerARCer::getCentralizer() const
{
#if SAFETY > 0
  if( NG == 0 )
    error( " NGcentralizerARCer::getCentralizer() const : "
	   "You have not initialized the group");
#endif
  
  return result;
}

//---------------------------------------------------------------------------//
//-------------------- NGcentralizer ----------------------------------------//
//---------------------------------------------------------------------------//


NGcentralizer::NGcentralizer(SMWord& w)
  : Supervisor( true ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    theWord( w ),
    started(false),
    arcer( *this )
{

}


void NGcentralizer::takeControl( )
{
  if(!fastResult() ) {
    if ( freeARCs() > 0 ) {
      if (theWord.getParent().gic.haveNilpotentGroupInited()){
	if (!started){
	  arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			     theWord.getWord());    	  
	  started = true;
	}
	if( arcer.takeControl() ) {
	  SGofFreeGroup F(theWord.getParent().getFreePreimage(),
			  arcer.getCentralizer() );
	  
	  SMObject* smo = 
	    new SMSubgroup( theWord.getParent(), F,
			    Text("The centralizer of") + Name( theWord )
			    ); 
	  
	  ListOf<OID> dependencies( theWord.getParent() );
	  CheckinMessage( *smo, "", dependencies ).send();
	  
	  adminTerminate();
	  return;
	}
      }
      usedOneARC();
    }
  }  
}

void NGcentralizer::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the centralizer of element"),
		  Text("Compute the centralizer of ")
		  + Name( theWord )  ,
		  helpID("NGcentralizer", theWord.getParent()), 
		  "centralizer"
		  );
  pv.startItemGroup();
  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    ); 
  pv.add(Text("Compute the centralizer "), ThisARCSlotID(), 100);
  pv.done();
}

// ---------------------- NGIsomorphismARCer --------------------------- //

void NGIsomorphismARCer::setArguments( const FPGroup& gr1 ,
				       const FPGroup& gr2 , 
				       int n1 , int n2 )
{
  g1 = gr1;
  g2 = gr2;
  nc1 = n1;
  nc2 = n2;
}

void NGIsomorphismARCer::runComputation( )
{
  if( nc1 && nc2 )
    {
      NilpotentGroup ng1( g1.namesOfGenerators() , nc1 ,
			  makeVectorOf(g1.getRelators()) );
      NilpotentGroup ng2( g2.namesOfGenerators() , nc2 ,
			  makeVectorOf(g2.getRelators()) );
      
      if( !ng1.isInitialized() ) ng1.initialize();
      if( !ng2.isInitialized() ) ng2.initialize();
      
      int i;
      
      for( i = 1 ; i <= nc1 && i <= nc2 ; i++ )
	if( !ng1.getLCSQuotient(i).abelianization.isomorphicTo(ng2.getLCSQuotient(i).abelianization) )
	  {
	    ans = no;
	    return;
	  }
      
      if( nc1 > nc2 )
	{
	  if( !ng1.getLCSQuotient( i ).abelianization.isTrivial() )
	    ans = no;
	}
      else
	if( nc1 < nc2 )
	  if( !ng2.getLCSQuotient( i ).abelianization.isTrivial() )
	    ans = no;
    }
  else
    for( int nc = 1 ; ; nc++ )
      {
	NilpotentGroup ng1( g1.namesOfGenerators() , nc ,
			    makeVectorOf(g1.getRelators()) );
	NilpotentGroup ng2( g2.namesOfGenerators() , nc ,
			    makeVectorOf(g2.getRelators()) );
	
	if( !ng1.isInitialized() ) ng1.initialize();
	if( !ng2.isInitialized() ) ng2.initialize();
	
	if( !ng1.getLCSQuotient(nc).abelianization.isomorphicTo(ng2.getLCSQuotient(nc).abelianization) )
	  {
	    ans = no;
	    break;
	  }
      }
}
 
void NGIsomorphismARCer::writeResults( ostream& out ) 
{
  out < ans;
}


void NGIsomorphismARCer::readResults( istream& in )
{
  in > ans;
}

// ------------------------- NGIsomorphismProblem ------------------------- //

NGIsomorphismProblem::NGIsomorphismProblem( SMFPGroup& gr1 , SMFPGroup& gr2 )
  : ComputationManager( true ), 
    theGroup1( gr1 ),
    theGroup2( gr2 ),
    arcer( *this )
{
  if( theGroup1.gic.haveActualNilpotencyClass() &&
      theGroup2.gic.haveActualNilpotencyClass() &&
      theGroup1.gic.getActualNilpotencyClass() != 
      theGroup2.gic.getActualNilpotencyClass() )
   {
     flag = 0;
     resultIsFast();
     return;
   }
  
  if( theGroup1.getFPGroup().numberOfGenerators() == 
      theGroup2.getFPGroup().numberOfGenerators() &&
      theGroup1.getFPGroup().getRelators() ==
      theGroup2.getFPGroup().getRelators() )
    if( theGroup1.getFPGroup().namesOfGenerators() ==
	theGroup2.getFPGroup().namesOfGenerators() )
      {
	flag = 1;
	resultIsFast();
	return;
      }
    else
      {
	flag = 2;
	resultIsFast();
	return;
      }
  
  int nc1,nc2;
  
  if( theGroup1.gic.isNilpotent() == yes )
    nc1 = theGroup1.gic.getNilpotentcyClass();
  else
    nc1 = 0;
  
  if( theGroup2.gic.isNilpotent() == yes )
    nc2 = theGroup2.gic.getNilpotentcyClass();
  else
    nc2 = 0;

  arcer.setArguments( theGroup1.getFPGroup() , 
		      theGroup2.getFPGroup() , 
		      nc1 , nc2 );
}

void NGIsomorphismProblem::takeControl( )
{
  if( fastResult() )
    {
      LogMessage msg1( *this , theGroup1 , theGroup2 );
      
      if( !flag )
	msg1 << Name( theGroup1 ) << " and " << Name( theGroup2 ) << " are not isomorphic because they are of different nilpotency classes.";
      else
	if( flag == 1 )
	  msg1 << Name( theGroup1 ) << " and " << Name( theGroup2 ) << " are isomorphic because they have the same presentation.";
	else
	  {
	    msg1 << Name( theGroup1 ) << " and " << Name( theGroup2 ) << " are isomorphic.";
	    msg1.send();
	    
	    LogMessage msg3( *this , theGroup1 , theGroup2 );
	    msg3 << "The isomorphism is: ";
	    
	    FPGroup G1 = theGroup1.getFPGroup();
	    FPGroup G2 = theGroup2.getFPGroup();
	    
	    for( int i = 0 ; i < G1.numberOfGenerators() ; i++ )
	      {
		msg3 << G1.nameOfGenerator( i ) << " -> " << G2.nameOfGenerator( i );
		if( i != G1.numberOfGenerators() - 1 )
		  msg3 << " , ";
	      }
	    
	    msg3 << " . ";
	    msg3.send();
	    
	    adminTerminate();
	    return;
	  }
      msg1.send();
      
      adminTerminate();
      return;
    }
  else
     if ( freeARCs() > 0 )
       if( arcer.takeControl() ) 
	 {
	   Trichotomy answer = arcer.getAnswer();
	   
	   if( answer == no )
	     {
	       LogMessage msg2( *this , theGroup1 , theGroup2 );
      	       msg2 << Name( theGroup1 ) << " and " << Name( theGroup2 ) << " are not isomorphic because they have nonisomorphic LCS quotients.";
	       msg2.send();
	     }
	   else
	     {
	       LogMessage msg3( *this , theGroup1 , theGroup2 );
      	       msg3 << "This algorithm can't give an exact answer.";
	       msg3.send();
	     }
	   
	   adminTerminate();
	   return;
	 }
       else
	 usedOneARC();
  
}
  
void NGIsomorphismProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Nilpotent isomorphism problem"),
		  Text("Are ") + Name( theGroup1 ) + " and " + 
		  Name( theGroup2 ) + " isomorphic? ",
		  helpID("IsomorphismProblem", theGroup1),
		  "Isomorphic ?" 
		  );
  
  pv.startItemGroup();
  
  pv.add( Text("Are ")+Name( theGroup1 ) + " and " + Name( theGroup2 ) + 
	  " isomorphic?",
	  ThisARCSlotID(),
	  100);
  
  pv.done();
}
