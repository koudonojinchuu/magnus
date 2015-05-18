// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of ObjectSmith methods outputCheckinMenuDefns,
//           outputToolsMenuDefns
//
// Principal Author: Dmitry Bormotov
//
// Status: Always being added to.
//
// Revision History:
//
// * 1/96 Dmitry B. completely rewrote almost all tools menus.
//

#include "ObjectSmith.h"

/////////////////////////////////////////////////////////////////////////
// Include the header for everything that can be made by choosing a
// menu item here:
//

#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMEquation.h"
#include "SMEnumerator.h"
#include "SMHomomorphism.h"
#include "SMSetOfWords.h"
#include "SMEqSystem.h"
#include "SMPermutation.h"

#include "fastProblems.h"
#include "WordProblem.h"
#include "KBModule.h"
#include "AbelianProblems.h"
#include "NilpotentQuotients.h"
#include "QuadEquationSolver.h"
#include "HomologyProblem.h"
#include "FreeProblems.h"
#include "OrderProblem.h"
#include "ConjugacyProblem.h"
#include "NilpotentProblems.h"
//#include "FNGroupProblems.h"
#include "FreeIsPartOfBasisProblem.h"
#include "IsTrivialProblem.h"
#include "IsFreeProblem.h"
#include "IsFiniteProblem.h"
#include "ExtendToHomProblem.h"
#include "IsAbelianProblem.h"
#include "IsNilpotentProblem.h"
#include "IsEltCentral.h"
#include "AreEltsEqual.h"
#include "OrderOfElt.h"
#include "SubgroupProblems.h"
#include "HNNProblems.h" 
#include "OneRelatorProblems.h"
#include "TTProblem.h"
#include "ToddCoxeter.h"
#include "GAEquations.h"
#include "ACConjecture.h"
#include "WhiteheadMinimal.h"
#include "KernelPresentation.h"
#include "PresentationProblems.h"
#include "GAConjProblemForORGroup.h"
#include "RankOfSubgroup.h"
#include "RankOfElt.h"
#include "IsWordAPE.h"
#include "ACE.h"
#include "MakeRandomPresentation.h"

// Lists and Enumerators

#include "SGREnumerator.h"
#include "WEnumerator.h"
#include "REnumerator.h"
#include "AutoEnumerator.h"
#include "SGREnumerator.h"
#include "SubgroupEnumerator.h"
#include "ORConsequencesEnumerator.h"
#include "SubgroupContainmentProblem.h"
#include "Rewrites.h"
#include "HomEnumerators.h"
#include "EquationsInFPProblem.h"
#include "ListProblems.h"
#include "SMListWordProblem.h"
#include "SMListSubgroupProblems.h"
#include "FreeListProblems.h"
#include "PreAbelianRepProblem.h"

// --------------- ObjectSmith::outputCheckinMenuDefns --------------------- //


void ObjectSmith::outputCheckinMenuDefns(ostream& ostr)
{

  
// ----------------------------- Groups ------------------------------------ //

  {
    Menu0 M( Menu::CHECKIN );
	 


    M.startItemGroup();

    M.add( ReadSMAbelianGroup(), Text("Abelian Group"), DefineAbelianGroup() );

    //M.addDisabled( "Automatic Group" );

    M.add( Ctor0<CheckinAPOfFree>(), Text("Amalgamated Product") );

    M.add( ReadSMFPGroup(), Text("Finitely Presented Group"), DefineFPGroup() );

    M.add( ReadSMFreeGroup(), Text("Free Group"), DefineFreeGroup() );

    //M.addDisabled( "Hyperbolic Group" );

    //M.addDisabled( "Matrix Group" );

    //M.addDisabled( "Metabelian Group" );

    M.add( ReadSMFreeNilpotentGroup(), Text("Free Nilpotent Group"),
	   DefineFreeNilpotentGroup() );

    M.add( Ctor0<CheckinHNNofFreeGroup>(), Text("HNN-extension") );
    
    M.add( ReadSMNilpotentGroup(), Text("Nilpotent Group"),
	   DefineNilpotentGroup() );

    M.add( ReadSMORGroup(), Text("One Relator Group"), DefineORGroup() );

    M.addDisabled( Text("Permutation Group") );

    //M.addDisabled( "Polycyclic Group" );

    M.add( ReadSMSmallCancGroup(), Text("Small Cancellation Group"),
	   DefineSmallCancGroup() );

    //M.separator();

    //M.addDisabled( "Direct Product" );

    //M.addDisabled( "Semidirect Product" );
	 
    //M.addDisabled( "Free Product" );

    //M.addDisabled( "Tree Product" );

    //M.addDisabled( "Graph Product" );

    M.separator();
    
    M.startCascade( Text("Group definition templates") );

    M.add( ReadSymmetricGroup(), Text("Symmetric group"), 
	   DefineInt("Enter the number N", 2) );

    M.closeCascade();

    M.separator();
    
    M.add( ReadSMPermutation(), Text("Permutation"), DefinePermutation() );
	 
    M.done();
  }

//----------------------------- Substructures -------------------------------//

  {
    Menu1<SMFPGroup> M( Menu::CHECKIN );

    M.startItemGroup();
	 
    M.add(ReadSMWord(), Text("Word in the generators of") + Name(1),
	  DefineWord());

    M.add(ReadSMSubgroup(), Text("Subgroup of") + Name(1),
	  DefineSubgroup());

    M.add(ReadSMMap(),
	  Text("Map from the generators of") + Name(1) + "to" + Name(1),
	  DefineMap() );

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FP ||
		      CheckinType( 1 ) == SMFPGroup::SMALL_CANCELLATION ||
		      CheckinType( 1 ) == SMFPGroup::ONE_RELATOR ||
		      CheckinType( 1 ) == SMFPGroup::ONE_RELATOR_WITH_TORSION);
    
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FREE );

    M.add(ReadSMEquation2(),
	  Text("Equation in") + Name(1),
	  DefineEquation2() );

    M.add(ReadSMEquation(),
	  Text("Quadratic equation in") + Name(1),
	  DefineEquation() );

    M.add(ReadSMEqSystem(),
	  Text("System of equations in") + Name(1),
	  DefineEqSystem() );

    M.add(ReadSMSetOfWords(), Text("Set of words in") + Name(1),
	  DefineSetOfWords());

    M.add(ReadSMVectorOfWords(), Text("Tuple of words in") + Name(1),
	  DefineVectorOfWords());

 	 
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FP ||
		      CheckinType( 1 ) == SMFPGroup::ABELIAN );
    
    M.add(ReadSMEquation2(), Text("Equation in") + Name(1),
	  DefineEquation2());
    
    M.add(ReadSMEqSystem(), Text("System of equations in") + Name(1),
	  DefineEqSystem());	 
    
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FP );
    
    M.add(ReadSMVectorOfWords(), Text("Tuple of words in") + Name(1),
	  DefineVectorOfWords());

    M.done();
  }

  {
    Menu2<SMFPGroup,SMFPGroup> M( Menu::CHECKIN );
    
    M.startItemGroup();
    
    M.add(ReadSMMap2(),
	  Text("Map from the generators of") + Name(1) + "to" + Name(2),
	  DefineMap2() );
    
    M.add(ReadSMMap2(true),
	  Text("Map from the generators of") + Name(2) + "to" + Name(1),
	  DefineInverseMap2() );
    
    M.done();
  }
}

//---------------------------------------------------------------------------//
//----------------- ObjectSmith::outputToolsMenuDefns -----------------------//
//---------------------------------------------------------------------------//

void ObjectSmith::outputToolsMenuDefns(ostream& ostr)
{
  {
    Menu1<SMFPGroup> M( Menu::TOOLS );

    /* template
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::TYPE );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    M.closeCascade();

    M.startCascade( Text("Compute ...") );
    M.closeCascade();

    M.startCascade( Text("Find ...") );
    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );
    M.closeCascade();
    */

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::ABELIAN );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor1<IsTrivialProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.add( Ctor1<IsFiniteProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("finite ?") );
    
    M.add( Ctor1<IsFreeProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("free abelian ?") );
    
    M.add( Ctor1<AbelianIsAutomatic, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.add( Ctor1<IsAbelianHyperbolic, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("hyperbolic ?") );
  
    M.closeCascade();

    M.startCascade( Text("Compute ...") );
    
    M.add( Ctor1<AbelianOrderProblem, SMFPGroup>(),
	   Text("Compute the order of") + Name(1) );

    M.add( Ctor1<AbelianComputeTorsionSubgroup, SMFPGroup>(),
	   Text("Compute the torsion subgroup of") + Name(1) );

    M.add( Ctor1<AbelianOrderOfTheTorsionSubgroupProblem, SMFPGroup>(),
	   Text("Compute the order of the torsion subgroup of") + Name(1) );

    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1) );

    M.add( Ctor1<AbelianPrimeDecomp,SMFPGroup>(),
	   Text("Compute the primary decomposition of") + Name(1) );

    M.add( Ctor1<AbelianTorsionFreeRankProblem, SMFPGroup>(),
	   Text("Compute the torsion-free rank of") + Name(1) );

    M.add( ReadHomologyItem1(),
	   Text("Compute an integral homology group of") + Name(1),
	   DefineInt("What dimension?", 2) );

    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Compute a short-lex system of representatives for")
	   + Name(1) );

    M.closeCascade();
 
    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMFPGroup>(), 
	   Text("Find a permutation representation of") + Name(1)
	   + Text("when") + Name(1) + Text("is finite"));

    M.add( Ctor1<AbelianIsConfluent, SMFPGroup>(), 
	   Text("Find a finite rewriting system for") + Name(1));

    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );
    
    M.add( Ctor1<WordEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate words of ") + Name(1));
    
    M.add( Ctor1<SGEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate subgroups of ") + Name(1));

    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.add( Ctor1<AutoEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate automorphisms of ") + Name( 1 ));
    
    M.add( Ctor1<FiniteAutoEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate automorphisms of ") + Name( 1 )
	   + Text("of finite order"));

    M.closeCascade();
 
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::AP_FREE ||
		      CheckinType( 1 ) == SMFPGroup::AP_FREE_CYCLIC );

    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<APOfFreeIsAbelian, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );

    M.add( Ctor1<APofFreeIsPerfect, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("perfect ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );
    
    M.add( Ctor1<APOfFreeIsFinite, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("finite ?") );

    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );
 
    M.add( Ctor1<APofFreeIsFree, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("free ?") );
 
    M.add( Ctor1<FPIsMSC,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("metric small cancellation ?") );
    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<APOfFreeOrder,SMFPGroup>(),
	   Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );
    
    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );
 
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute Schreier representatives for") + Name(1) );
    
    M.add( ReadHomologyItem(),
	   Text("Compute an integral homology group of") + Name(1),
	   DefineInt("What dimension?", 2) );
    
    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.addDisabled( Text("Find an HNN-extension decomposition of") + Name(1) );

    M.separator();

    M.add( Ctor1<KBProblem,SMFPGroup>(),
	   Text("Find a finite rewriting system for") + Name(1) );

    M.separator();
    
    M.addDisabled( Text("Find an endomorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) );

    M.addDisabled( Text("Find an IA-automorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) 
		   + Text("of finite order"));

    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate words of") + Name(1) );
   
    M.addDisabled( Text("Enumerate subgroups of") + Name(1) );

    M.addDisabled( Text("Enumerate relators of") + Name(1) );
  
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
 		   + Text("of finite order") );

    M.closeCascade();
 

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FP );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<IsTrivialProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("trivial ?")
	   );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );
    
    M.add( Ctor1<IsAbelianProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("abelian ?")
	   );

    M.add( ReadIsNilpotentProblemItem(),
	   Text("Is") + Name(1) + Text("nilpotent ?"),
	   DefineInt("Enter the nilpotency class: ", 1)
	   );
    
    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );
    
    M.add( Ctor1<IsFiniteProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("finite ?") );

    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.add( Ctor1<FPIsMSC,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("metric small cancellation ?")
	   );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<OrderProblem,SMFPGroup>(),
	   Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );
    
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.add( Ctor1<SchreierTransversal,SMFPGroup>(),
	   Text("Compute Schreier representatives for") + Name(1) );

    
    M.add( ReadHomologyItem(),
	   Text("Compute an integral homology group of") + Name(1),
	   DefineInt("What dimension?", 2) );
    M.closeCascade();

  
    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMFPGroup>(),
	   Text("Find a permutation representation of") + Name(1)
	   + Text("when") + Name(1) + Text("is finite") );
    
    M.separator();

    M.add( Ctor1<KBProblem,SMFPGroup>(),
	   Text("Find a finite rewriting system for") + Name(1) );

    M.add( Ctor1<PreAbelianRepProblem, SMFPGroup>(),
	   Text("Find a pre-Abelian presentation for") + Name(1)
	   );

    M.separator();
   
    M.addDisabled( Text("Find an endomorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) );

    M.addDisabled( Text("Find an IA-automorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) 
		   + Text("of finite order"));

    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );
    
    M.add( Ctor1<WordEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate words of ") + Name( 1 )
	   );
    
    M.add( Ctor1<SGEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate subgroups of ") + Name( 1 )
	   );

    M.add( Ctor1<NCRelatorEnumerator, SMFPGroup>(), 
	   Text("Enumerate relators of ") + Name( 1 )
	   );
    
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
		   + Text("of finite order"));

    M.closeCascade();
  
    /*@db buggy!
    M.startCascade( Text("Run ...") );
    
    M.add( Ctor1<ACEProblem,SMFPGroup>(),
	   Text("Run Todd-Coxeter for") + Name( 1 )
	   );
    
    M.closeCascade();
    */

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FREE );

    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<FreeIsAutomatic, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.add( Ctor1<FreeIsHyperbolic, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.closeCascade();
    
    M.startCascade( Text("Compute ...") );
    
    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized") );

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute Schreier representatives for") + Name(1) );
    
    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );
    
    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.addDisabled( Text("Find a finite rewriting system for") + Name(1) );
    
    M.add( ReadFreeGetN_thElementItem(),
	   Text("Find the n-th element of") + Name(1),
	   DefineInt("Enter the number n: ", 0) );

    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );

    M.add( Ctor1<WordEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate words of ") + Name( 1 ) );
    
    M.add( Ctor1<SGEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate subgroups of ") + Name( 1 )
	   );
    
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.add( Ctor1<AutoEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate automorphisms of ") + Name( 1 )
	   );
    
    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );
    
    M.add( Ctor1<FiniteAutoEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate automorphisms of ") + Name( 1 )
	   + Text("of finite order")
	   );

    M.closeCascade();
    
 
    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FREE_NILPOTENT );
    

    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.addDisabled( Text("Is") + Name(1) + Text("abelian ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the torsion subgroup of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("abelianized") );
    M.addDisabled( Text("Compute the class of") + Name(1) );

    M.addDisabled( Text("Compute the derived length of") + Name(1) );

    M.add( ReadLCStermProblem(),
	   Text("Compute a term of the lower central series of") + Name(1),
	   DefineInt("Enter the number of the term ", 1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.add( Ctor1<NGHirschNumberProblem, SMFPGroup>(), 
	   Text("Compute the Hirsch number of") + Name(1) );
    
    M.add( Ctor1<NGcomputeBasisProblem, SMFPGroup>(),
	   Text("Compute a basis of ") + Name(1) );

    M.addDisabled( Text("Compute a short-lex system of representatives for")
		   + Name(1) );
  
    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );
    
    M.add( Ctor1<NGbuildPresentationProblem, SMFPGroup>(),
	   Text("Find a presentation for ") + Name(1) );

    M.addDisabled( Text("Find a polycyclic presentation of") + Name(1) );

    M.addDisabled( Text("Find a permutation presentation of") + Name(1) );
	
    M.separator();
    
    M.addDisabled( Text("Find a finite rewriting system for") + Name(1) );
    
    M.closeCascade();
 
    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate words of") + Name(1) );
   
    M.addDisabled( Text("Enumerate subgroups of") + Name(1) );

    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
 		   + Text("of finite order") );

    M.closeCascade();
 

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::HNN_FREE );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<HNNofFreeGroup_IsAbelian, SMFPGroup>(), 
	   Text("Is") + Name(1) + Text("abelian ?")
	   );

    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );
    
    M.add( Ctor1<HNNofFreeGroup_IsFinite, SMFPGroup>(), 
	   Text("Is") + Name(1) + Text("finite ?") );

    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.add( Ctor1<HNNofFreeIsFree, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("free ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("metric small cancellation ?"));

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );
    
    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );
 
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute Schreier representatives for") + Name(1) );
    
    M.add( ReadHomologyItem(),
	   Text("Compute an integral homology group of") + Name(1),
	   DefineInt("What dimension?", 2) );
    
    M.closeCascade();


    M.startCascade( Text("Find ...") );


    M.addDisabled( Text("Find an amalgamated product decomposition of") 
		   + Name(1) );

    M.addDisabled( Text("Find an HNN-extension decomposition of") + Name(1) );

    M.addDisabled( Text("Find a permutation representation of") + Name(1) 
		   + Text("when") + Name(1) + Text("is finite") );

    M.separator();

    M.add( Ctor1<KBProblem,SMFPGroup>(),
	   Text("Find a finite rewriting system for") + Name(1)
	   );

    M.separator();
    
    M.addDisabled( Text("Find an endomorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) );

    M.addDisabled( Text("Find an IA-automorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) 
		   + Text("of finite order"));

    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate words of") + Name(1) );
   
    M.addDisabled( Text("Enumerate subgroups of") + Name(1) );

    M.addDisabled( Text("Enumerate relators of") + Name(1) );
  
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
 		   + Text("of finite order") );

    M.closeCascade();
 

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::NILPOTENT );
    

    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<IsTrivialProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("trivial ?")
	   );

    M.add( Ctor1<IsFiniteProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("finite ?")
	   );

    M.add( Ctor1<IsAbelianProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("abelian ?")
	   );
   
    M.add( Ctor1<NGisFreeNilpotentProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("free nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<OrderProblem,SMFPGroup>(),
	   Text("Compute the order of") + Name(1)
	   );

    M.addDisabled( Text("Compute the center of") + Name(1) );
    
    M.addDisabled( Text("Compute the torsion subgroup of") + Name(1) );

    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );

    M.add( Ctor1<NGcomputeNClassProblem, SMFPGroup>(),
	   Text("Compute the class of") + Name(1) );
    
    M.addDisabled( Text("Compute the derived length of") + Name(1) );

    M.add( ReadLCStermProblem(),
	   Text("Find a term of the lower central series of") + Name(1),
	   DefineInt("Enter the number of the term ", 1)
	   );
    
    M.add( Ctor1<NGcomputeBasisProblem,SMFPGroup>(),
	   Text("Compute lower central quotients of") + Name(1)
	   );

    M.addDisabled( Text("Compute the derived series of") + Name(1) );


    M.addDisabled( Text("Compute quotients of the derived series of")
			+ Name(1) );

    M.add( Ctor1<NGHirschNumberProblem,SMFPGroup>(),
	   Text("Compute the Hirsch number of") + Name(1)
	   );

    M.add( Ctor1<NGcomputeBasisProblem,SMFPGroup>(),
	   Text("Compute a basis of ")
	   + Name(1)
	   );

    M.addDisabled( Text("Compute a short-lex system of representatives for")
			+ Name(1) );
  
    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );

    M.closeCascade();


    M.startCascade( Text("Find ...") );

    M.add( Ctor1<NGbuildPresentationProblem, SMFPGroup>(),
	   Text("Find a polycyclic presentation of") + Name(1)
	   );

    M.separator();
 
    M.addDisabled( Text("Find a finite rewriting system for") + Name(1) );
 
    M.addDisabled( Text("Find a permutation presentation of") + Name(1) );
	
    M.closeCascade();
 

    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate words of") + Name(1) );
   
    M.addDisabled( Text("Enumerate subgroups of") + Name(1) );

    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );
    
    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
 		   + Text("of finite order") );

    M.closeCascade();


    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::ONE_RELATOR ||
		      CheckinType( 1 ) == SMFPGroup::ONE_RELATOR_WITH_TORSION
		      );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<ORIsTrivial, SMFPGroup>(), 
	   Text("Is") + Name(1) + Text("trivial ?")
	   );

    M.add( Ctor1<ORIsAbelian, SMFPGroup>(), 
	   Text("Is") + Name(1) + Text("abelian ?")
	   );

    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.add( Ctor1<ORIsFinite, SMFPGroup>(), 
	   Text("Is") + Name(1) + Text("finite ?") );
    
    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.add( Ctor1<ORIsFreeProblem, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("free ?") );

    M.add( Ctor1<FPIsMSC,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("metric small cancellation ?") );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<OROrder,SMFPGroup>(),
	   Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );
    
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute Schreier representatives for") + Name(1) );

    M.add( Ctor1<FastHomology,SMFPGroup>(),
	   Text("Compute the integral homology of") + Name(1) );
 
    M.closeCascade();

  
    M.startCascade( Text("Find ...") );

    M.addDisabled( Text("Find an amalgamated product decomposition of") 
		   + Name(1) );

    M.add( ReadSMMagnusBreakdown(),
	   Text("Find an HNN-extension decomposition of") + Name(1) );
    
    M.addDisabled( Text("Find a permutation representation of") + Name(1) 
		   + Text("when") + Name(1) + Text("is finite") );
  
    M.separator();

    M.add( Ctor1<KBProblem,SMFPGroup>(),
	   Text("Find a finite rewriting system for") + Name(1) );

    M.separator();
   
    M.addDisabled( Text("Find an endomorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) );

    M.addDisabled( Text("Find an IA-automorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) 
		   + Text("of finite order") );

    M.closeCascade();
  

    M.startCascade( Text("Enumerate ...") );
    
    M.add( Ctor1<WordEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate words of ") + Name( 1 ) );
    
    M.add( Ctor1<SGEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate subgroups of ") + Name( 1 ) );

    M.add( Ctor1<NCRelatorEnumerator, SMFPGroup>(), 
	   Text("Enumerate relators of ") + Name( 1 ) );

    M.add( Ctor1<ORConsequencesEnumerator,SMFPGroup>(),
	   Text("Enumerate consequences of the relator of ") + 
	   Name( 1 ) );
    
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
		   + Text("of finite order") );

    M.closeCascade();
  

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::SMALL_CANCELLATION );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<MSCIsTrivial, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );
 
    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );
 
    M.add( Ctor1<MSCIsAbelian, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );

    M.add( ReadIsNilpotentProblemItem(),
	   Text("Is") + Name(1) + Text("nilpotent ?"),
	   DefineInt("Enter the nilpotency class: ", 1) );
    
    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.add( Ctor1<MSCIsFinite, SMFPGroup>(),
	   Text("Is") + Name(1) + Text("finite ?") );

    M.add( Ctor1<AGProblem,SMFPGroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<MSCOrder,SMFPGroup>(),
	   Text("Compute the order of") + Name(1) );
  
    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.add( Ctor1<AbelInvariantsProb,SMFPGroup>(),
	   Text("Compute the canonical decomposition of") + Name(1)
	   + Text("abelianized") );
    
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.add( Ctor1<SchreierTransversal,SMFPGroup>(),
	   Text("Compute Schreier representatives for") + Name(1)
	   );
    
    M.add( ReadHomologyItem(),
	   Text("Compute an integral homology group of") + Name(1),
	   DefineInt("What dimension?", 2)
	   );
    
    M.closeCascade();

  
    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMFPGroup>(),
	   Text("Find a permutation representation of") + Name(1) );
    
    M.separator();

    M.add( Ctor1<KBProblem,SMFPGroup>(),
	   Text("Find a finite rewriting system for") + Name(1) );

    M.separator();
   
    M.addDisabled( Text("Find an endomorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) );

    M.addDisabled( Text("Find an IA-automorphism of") + Name(1) );

    M.addDisabled( Text("Find an automorphism of") + Name(1) 
		   + Text("of finite order") );

    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );
    
    M.add( Ctor1<WordEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate words of ") + Name( 1 ) );
    
    M.add( Ctor1<SGEnumeratorProblem, SMFPGroup>(), 
	   Text("Enumerate subgroups of ") + Name( 1 ) );

    M.add( Ctor1<NCRelatorEnumerator, SMFPGroup>(), 
	   Text("Enumerate relators of ") + Name( 1 ) );
    
    M.addDisabled( Text("Enumerate endomorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate IA-automorphisms of") + Name(1) );

    M.addDisabled( Text("Enumerate automorphisms of") + Name(1)
		   + Text("of finite order") );

    M.closeCascade();

    M.done();
  }

  {
    Menu1<SMWord> M( Menu::TOOLS );

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );

    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor1<FastAbelianForm, SMWord>(), Text("Rewrite") + Name(1) 
	   + Text("in reduced form as an element of a free abelian group") );

    M.add( Ctor1<FastInverseInAbelianForm, SMWord>(),
	   Text("Rewrite the inverse of") + Name(1) 
	   + Text("in reduced form as an element of a free abelian group") );

    M.separator();
    
    M.add( ReadNthPower(),
	   Text("Rewrite") + Name(1) + Text("as an n-th power"),
	   DefineInt("n = ", 2) );

    M.closeCascade();
    
    M.startCascade( Text("Compute ...") );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the reduced form of") 
	   + Name(1) + Text("as an element of a free abelian group") );

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the reduced form of") 
	   + Name(1) + Text("as an element of a free abelian group"),
	   DefineInt("Enter length of the initial segment: ", 1) );

    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the reduced form of") 
	   + Name(1) + Text("as an element of a free abelian group"),
	   DefineInt("Enter length of the terminal segment: ", 1) );
    
    M.separator();

    M.add( Ctor1<WordRepresentativeProblem, SMWord>(),
	   Text("Compute the representative of") 
	   + Name(1) + Text("in a Schreier system of representatives for") 
	   + Name( Parent(1) ) );

    M.separator();
    
    M.add( Ctor1<AbelianWordProblem,SMWord>(),
	   Text("Compute the canonical decomposition of") + Name(1) );
    
    M.add( Ctor1<EltPrimeForm, SMWord>(),
	   Text("Compute the primary decomposition of ") + Name(1) );

    M.separator();

    M.add( Ctor1<AbelianOrderOfEltProblem, SMWord>(),
	   Text("Compute the order of ") + Name(1) );
    
    M.add( ReadPHeight(),
	   Text("Compute the p-height of ") + Name(1),
	   DefineInt("Enter the p  ", 2) );
    
    M.add( Ctor1<AbelianMaximalRoot, SMWord>(),
	   Text("Compute the maximal root of ") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?");
    
    M.add( Ctor1<AbelianPowerProblem, SMWord>(),
	   Text("Is ") + Name(1) + Text("a proper power ?") );
    
    M.add( Ctor1<AbelianSGGenedByWordPurityProblem, SMWord>(),
	   Text("Is the subgroup generated by")
	   + Name(1) + Text("pure ?") );
    
    M.closeCascade();
        
    M.startCascade( Text("Does") + Name(1) + Text("...") );
    
    M.add( Ctor1<AbelianDoesGensSummand, SMWord>(),
	   Text("Does ") + Name(1) + Text("generate a direct summand ?") );
    
    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::AP_FREE_CYCLIC );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.addDisabled( Text("Compute the amalgamated product length of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of")
	   + Name(1), DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") 
	   + Name(1), DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.addDisabled( Text("Compute the representative of") + Name(1) 
		   + Text("in a Schreier system of representatives for")
		   + Name( Parent(1) )  );

    M.separator();
    
    M.add( Ctor1<APOfFreeReducedForm,SMWord>(),
	   Text("Compute the reduced form of") + Name(1) );

    M.add( Ctor1<APOfFreeNormalForm,SMWord>(),
	   Text("Compute the normal form of") + Name(1) );

    M.add( Ctor1<APOfFreeCyclicNormalForm,SMWord>(),
	   Text("Compute the cyclic normal form of") + Name(1) );

    M.add( Ctor1<APOfFreeCyclic_MaximalRoot, SMWord>(),
	   Text("Compute the maximal root of") + Name(1) );

    M.separator();

    M.add( Ctor1<APOfFreeCyclic_CentralizerOfElt, SMWord>(),
	   Text("Compute the centralizer of") + Name(1) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );
    
    M.add( Ctor1<SquareRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of squares") );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );
 
    M.add( Ctor1<CommutatorRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of commutators") );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators") );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a strictly alternating product") );
		   
    M.add( Ctor1<APOfFreeNormalForm,SMWord>(),
	   Text("Rewrite") + Name(1)
	   + Text("in the normal form for amalgamated products") );
    
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor1<APOfFreeWordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?" );

    M.add( Ctor1<APOfFreeCyclic_IsEltAProperPower, SMWord>(),
	   Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::AP_FREE );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") +Name(1));
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.addDisabled( Text("Compute the amalgamated product length of") +Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.addDisabled( Text("Compute the representative of") + Name(1) 
		   + Text("in a Schreier system of representatives for")
		   + Name( Parent(1) ) );

    M.separator();
    
    M.add( Ctor1<APOfFreeReducedForm,SMWord>(),
	   Text("Compute the reduced form of") + Name(1) );

    M.add( Ctor1<APOfFreeNormalForm,SMWord>(),
	   Text("Compute the normal form of") + Name(1) );

    M.add( Ctor1<APOfFreeCyclicNormalForm,SMWord>(),
	   Text("Compute the cyclic normal form of") + Name(1) );

    M.addDisabled( Text("Compute the maximal root of") + Name(1) );

    M.separator();

    M.addDisabled( Text("Compute the centralizer of") + Name(1) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.closeCascade();


    M.startCascade( Text("Rewrite ...") );
    
    M.addDisabled( Text("Rewrite") + Name(1)
		   + Text("as a product of squares") );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );
 
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of commutators") );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators") );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a strictly alternating product") );
		   
    M.add( Ctor1<APOfFreeNormalForm,SMWord>(),
	   Text("Rewrite") + Name(1)
	   + Text("in the normal form for amalgamated products") );
    
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor1<APOfFreeWordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?" );

    M.addDisabled( Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::HNN_FREE );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.addDisabled( Text("Compute the representative of") + Name(1) 
		   + Text("in a Schreier system of representatives for")
		   + Name( Parent(1) ) );

    M.separator();
    
    M.addDisabled( Text("Compute the centralizer of") + Name(1) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor1<SquareRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of squares") );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );

    M.add( Ctor1<CommutatorRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of commutators") );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators")  );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product without pinches") );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("in HNN-extension normal form") );
		   
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<HNNofFreeGroup_WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?" );
    
    M.addDisabled( Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FP );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1)  );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.add( Ctor1<WordRepresentativeProblem, SMWord>(),
	   Text("Compute the representative of") + Name(1) 
	   + Text("in a Schreier system of representatives for")
	   + Name( Parent(1) ) );
    
    M.separator();
    
    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.separator();

    M.addDisabled( Text("Compute the centralizer of") + Name(1) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor1<SquareRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of squares") );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );

    M.add( Ctor1<CommutatorRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of commutators") );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators") );
	   
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?" );

    M.add( Ctor1<IsEltCentral,SMWord>(),
	   Text("Is") + Name(1) + Text("central in")
	   + Name( Parent(1) ) + Text("?") );
    
    M.add( Ctor1<OrderOfElt, SMWord>(),
	   Text("Is") + Name(1) + Text("of finite order in")
	   + Name( Parent(1) ) + Text("?") );

    M.addDisabled( Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.add( Ctor1<FreeCentolizerOfElement, SMWord>(),
	   Text("Compute the centralizer of") + Name(1) + "in"
	   + Name( Parent(1) ) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );
    
    M.separator();
 
    M.add( Ctor1<RankOfElementProblem,SMWord>(),
	   Text("Compute a rank of ") + Name(1) + " in " 
	   + Name( Parent(1) ) + "?" );
    
    M.add( Ctor1<FreeMaximalRootOfElement,SMWord>(),
	   Text("Compute the maximal root of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );
        
    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );
  
    M.add( Ctor1<ProductOfSquares,SMWord>(),
	   Text("Rewrite") + Name(1) + "as a product of squares" 
	   + Name( Parent(1) ) );

    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );

    M.add( Ctor1<ProductOfCommutators,SMWord>(),
	   Text("Rewrite") + Name(1) + "as a product of commutators" 
	   + Name( Parent(1) ) );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators") );
	   
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor1<WordProblemInFree,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?" );

    M.add( Ctor1<FreeIsElementAProperPower,SMWord>(),
	   Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.add( Ctor1<CommutatorInFree,SMWord>(),
	   Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.add( Ctor1<TwoCommProblem,SMWord>(),
	   Text("Is") + Name(1) + "a product of two commutators ?" );

    M.add( Ctor1<IsWordAPEProblem,SMWord>(),
	   Text("Is ") + Name(1) + "almost primitive ?" );

    M.add( Ctor1<FreeIsPartOfBasisProblem,SMWord>(),
	   Text("Is") + Name(1) + "part of a free basis for" +
	   Name( Parent(1) ) + "?" );
    
    M.add( Ctor1<FreeInCommutatorSG,SMWord>(),
	   Text("Is") + Name(1) + "in the commutator subgroup of"
	   + Name( Parent(1) ) + Text("?") );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE_NILPOTENT );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.add( Ctor1<NGcentralizer, SMWord>(),
	   Text("Compute the centralizer of ") + Name(1) );

    M.addDisabled( Text("Compute the normalizer of ") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.separator();

    M.add( Ctor1<NGMaximalRootProblem, SMWord>(),
	   Text("Compute the maximal root of ") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );
  
    M.addDisabled( Text("Rewrite") + Name(1) + "as a product of squares" );

    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );

    M.addDisabled( Text("Rewrite") + Name(1) + "as a product of commutators" );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators") );
    
    M.separator();
   
    M.add( Ctor1<NGdecomposeWordProblem,SMWord>(),
	   Text("Rewrite") + Name(1)
	   + Text("as a product of basic commutators") );
    
    M.closeCascade();
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem, SMWord>(),
	   Text("Is") + Name(1) + Text("trivial in")
	   + Name( Parent(1) ) + Text("?") );

    M.add( Ctor1<NGIsProperPower, SMWord>(),
	   Text("Is ") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("a commutator ?") );	

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.addDisabled( Text("Is") + Name(1) + "part of a free basis for" +
		   Name( Parent(1) ) + "?" );
    
    M.separator();
    
    M.add( Ctor1<NGisCentralProblem, SMWord>(),
	   Text("Is") + Name(1) + Text("central ?") );
    
    M.add( Ctor1<NGisWordInCommutatorSGProblem, SMWord>(),
	   Text("Is") + Name(1) 
	   + Text("an element of the commutator subgroup of")
	   + Name( Parent(1) ) + Text("?") );	 

    M.closeCascade();
       
    M.startCascade( Text("Find ...") );
    
    M.add( Ctor1<NGweightOfWordProblem,SMWord>(),
	   Text("Find the term of the lower central series in which") 
	   + Name(1) + Text("lies") );
    
    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT );
    
    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1) );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1) );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1) );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1));

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1) );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1) );

    M.separator();

    M.addDisabled( Text("Compute the centralizer of") + Name(1) );

    M.addDisabled( Text("Compute the normalizer of ") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) ) );

    M.separator();

    M.add( Ctor1<NGorderOfEltProblem, SMWord>(),
	   Text("Compute the order of ") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );
  
    M.addDisabled( Text("Rewrite") + Name(1) + "as a product of squares" );

    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group") );

    M.addDisabled( Text("Rewrite") + Name(1) + "as a product of commutators" );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of basic commutators") );
    
    M.add( Ctor1<NGdecomposeWordProblem,SMWord>(),
	   Text("Rewrite") + Name(1)
	   + Text("in canonical form as a product of elements in a")
	   + Text("Malcev basis for") + Name( Parent(1) )
	   );

    M.closeCascade();

    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?");

    M.addDisabled( Text("Is ") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("a commutator ?") );	

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.addDisabled( Text("Is") + Name(1) + "part of a free basis for" +
		   Name( Parent(1) ) + "?"
		   );

    M.separator();

    M.add( Ctor1<NGisCentralProblem, SMWord>(),
	   Text("Is") + Name(1) + Text("central ?")
	   );
    
    M.add( Ctor1<NGisWordInCommutatorSGProblem, SMWord>(),
	   Text("Is") + Name(1) + Text("an element of the commutator subgroup of")
	   + Name( Parent(1) ) + Text("?")
	   );	

 
    M.closeCascade();
      

    M.startCascade( Text("Find ...") );
    
    M.add( Ctor1<NGweightOfWordProblem,SMWord>(),
	   Text("Find the term of the lower central series in which") + Name(1)
	   + Text("lies")
	   );
    
    M.closeCascade();

    //@@@@@@

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::ONE_RELATOR );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1)
	   );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1)
	   );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1)
	   );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1)
	   );

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of")
	   + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1)
	   );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") 
	   + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1)
	   );

    M.separator();

    M.add( Ctor1<WordRepresentativeProblem, SMWord>(),
	   Text("Compute the representative of") + Name(1) 
	   + Text("in a Schreier system of representatives for")
	   + Name( Parent(1) )
	   );
    
    M.separator();
    
    M.add( Ctor1<OrderOfElt, SMWord>(),
	   Text("Compute the order of") + Name(1)
	   );

    M.separator();

    M.addDisabled( Text("Compute the centralizer of") + Name(1) );
	 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) )
		   );

    M.closeCascade();


    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor1<SquareRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of squares")
	   );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group")
		   );
    M.add( Ctor1<CommutatorRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of commutators")
	   );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators")
		   );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("in HNN-extension normal form")
		   );
	   
    M.closeCascade();

    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?"
	   );

    M.add( Ctor1<IsEltCentral,SMWord>(),
	   Text("Is") + Name(1) + Text("central in")
	   + Name( Parent(1) ) + Text("?")
	   );

    M.addDisabled( Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();


    M.startItemGroup( CheckinType( Parent(1) ) == 
		      SMFPGroup::ONE_RELATOR_WITH_TORSION );

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<FreelyReduceWord,SMWord>(),
	   Text("Compute the freely reduced form of") + Name(1)
	   );
    
    M.add( Ctor1<CyclicallyReduceWord,SMWord>(),
	   Text("Compute a cyclically reduced form of") + Name(1)
	   );

    M.add( Ctor1<FormalInverseOfWord,SMWord>(),
	   Text("Compute the freely reduced form of the inverse of") 
	   + Name(1)
	   );
    
    M.add( Ctor1<WordLength,SMWord>(),
	   Text("Compute the length of the freely reduced form of") + Name(1)
	   );

    M.add( ReadInitialSegmentItem(),
	   Text("Compute the i-th initial segment of the freely reduced form of")
	   + Name(1),
	   DefineInt("Enter length of the initial segment: ", 1)
	   );
    
    M.add( ReadTerminalSegmentItem(),
	   Text("Compute the j-th terminal segment of the freely reduced form of") 
	   + Name(1),
	   DefineInt("Enter length of the terminal segment: ", 1)
	   );

    M.separator();

    M.add( Ctor1<WordRepresentativeProblem, SMWord>(),
	   Text("Compute the representative of") + Name(1) 
	   + Text("in a Schreier system of representatives for")
	   + Name( Parent(1) )
	   );
    
    M.separator();
    
    M.add( Ctor1<ORWithTorsionEltFiniteOrder, SMWord>(),
	   Text("Compute the order of") + Name(1)
	   );

    M.separator();

    M.add( Ctor1<ORWithTorsionCentralizerOfElt, SMWord>(),
	   Text("Compute the centralizer of") + Name(1)
	   );
 
    M.addDisabled( Text("Compute the normalizer of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("modulo the derived group of") + Name( Parent(1) )
		   );

    M.closeCascade();


    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor1<SquareRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of squares")
	   );
    
    M.addDisabled( Text("Rewrite") + Name(1) + Text("as a product of its "
       "canonical form modulo the derived group and an element "
       "of the derived group")
		   );
    M.add( Ctor1<CommutatorRewriteProblem,SMWord>(),
	   Text("Rewrite") + Name(1) + Text("as a product of commutators")
	   );
    
    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("as a product of n-fold commutators")
		   );

    M.addDisabled( Text("Rewrite") + Name(1) 
		   + Text("in HNN-extension normal form")
		   );
	   
    M.closeCascade();

    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor1<WordProblem,SMWord>(),
	   Text("Is") + Name(1) + "trivial ?"
	   );

    M.add( Ctor1<IsEltCentral,SMWord>(),
	   Text("Is") + Name(1) + Text("central in")
	   + Name( Parent(1) ) + Text("?")
	   );

    M.addDisabled( Text("Is") + Name(1) + Text("a proper power ?") );
    
    M.addDisabled( Text("Is") + Name(1) + "a commutator ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of squares ?" );

    M.addDisabled( Text("Is") + Name(1) + "a product of commutators ?" );

    M.closeCascade();

    M.done();
  }

  {
    Menu1<SMSubgroup> M( Menu::TOOLS );

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor1<AbelianIsSGEqualToTheGroup, SMSubgroup>(),
	   Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.add( Ctor1<AbelianSGIndexProblem, SMSubgroup>(),
	   Text("Is") + Name(1) + "of finite index in" 
	   + Name( Parent(1) ) + "?" );

    M.add( Ctor1<AbelianSGPurityProblem, SMSubgroup>(),
	   Text("Is") + Name(1) + "pure in" + Name( Parent(1) ) + "?" );

    M.add( Ctor1<AbelianIsSGIsolatedProblem, SMSubgroup>(),
	   Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant in" 
		   + Name( Parent(1) ) + "?" );

    M.separator();

    M.add( Ctor1<IsSGTrivial, SMSubgroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.add( Ctor1<AbelianIsSGFinite, SMSubgroup>(),
	   Text("Is") + Name(1) + Text("finite ?") );
 
    M.add( Ctor1<AbelianIsSGFreeAbelian, SMSubgroup>(), 
	   Text("Is") + Name(1) + Text("free abelian ?") );

    M.add( Ctor1<AbelianIsAutomatic, SMSubgroup>(),
	   Text("Is") + Name(1) + Text("automatic ?") );

    M.add( Ctor1<AbelianIsSGHyperbolic, SMSubgroup>(), 
	   Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );
    
    M.add( Ctor1<AbelianSGIndexProblem, SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );

    M.add( Ctor1<SchreierTransversal, SMSubgroup>(),
	   Text("Compute a Schreier transversal of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );

    M.add( Ctor1<SubgroupIsolator, SMSubgroup>(),
	   Text("Compute the isolator of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );

    M.add( Ctor1<virtualFreeComp, SMSubgroup>(),
	   Text("Compute a virtual free complement of") + Name(1) 
	   + Text("in") + Name( Parent(1) ) );
    
    M.separator();

    M.add( Ctor1<AbelianSGOrder, SMSubgroup>(),
	   Text("Compute the order of") + Name(1) );
 
    M.add( Ctor1<AbelianComputeTorsionSubgroupOfSG, SMSubgroup>(),
	   Text("Compute the torsion subgroup of") + Name(1) );
    
    M.add( Ctor1<AbelianOrderOfTheTorsionSubgroupOfSG, SMSubgroup>(),
           Text("Compute the order of the torsion subgroup of") + Name(1) );

    M.add( Ctor1<AbelianSGCyclicDecomposition, SMSubgroup>(),
	   Text("Compute the canonical decomposition of") + Name(1) );

    M.add( Ctor1<AbelianSGPrimesDecomposition, SMSubgroup>(),
	   Text("Compute the primary decomposition of") + Name(1) );
    
    M.add( Ctor1<AbelianTorsionFreeRankOfSG, SMSubgroup>(),
           Text("Compute the torsion-free rank of") + Name(1) );

    M.add( Ctor1<AbelianSGMinGens, SMSubgroup>(),
	   Text("Compute the minimal number of generators of") + Name(1) );   
  
    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );

    M.addDisabled( Text("Compute the first n terms for a short-lex system of "
			"representatives for the elements of") + Name(1) );
    
    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMSubgroup>(),
	   Text("Find a permutation representation of") + Name( Parent(1) ) 
	   + Text("modulo") + Name(1) + Text("when the index of ") 
	   + Name(1) + " in " + Name( Parent(1) ) + Text("is finite") );

    M.add( Ctor1<AbelianIsConfluent, SMSubgroup>(),
	   Text("Find a finite rewriting system for") + Name(1) );

    M.addDisabled( Text("Find a ShortLex automatic structure for") + Name(1) );

    M.closeCascade();
 
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::AP_FREE ||
		      CheckinType( Parent(1) ) == SMFPGroup::AP_FREE_CYCLIC );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.addDisabled( Text("Is") + Name(1) + "central in" + Name( Parent(1) ) 
		   + "?" );
    
    M.add( Ctor1<IsSGNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal in") + Name(Parent(1)) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "subnormal in" + Name( Parent(1) ) 
		   + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant in" 
		   + Name( Parent(1) ) + "?" );

    M.separator();
 
    M.add( Ctor1<APOfFreeIsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + "trivial ?" );
 
    M.add( Ctor1<APOfFreeIsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.closeCascade();


    M.startCascade( Text("Compute ...") );
    
    M.addDisabled( Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );
    
    M.addDisabled( Text("Compute a Schreier transversal of") + Name(1)
		   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized") );

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );

    M.closeCascade();


    M.startCascade( Text("Find ...") );
    
    M.addDisabled( Text("Find a permutation representation of")
		   + Name( Parent(1) ) + Text("modulo") + Name(1) 
		   + Text("when the index of ") + Name(1) + " in " 
		   + Name( Parent(1) ) + Text("is finite") );

    M.separator();
    
    M.add( Ctor1<SGPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation for") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate relators of") + Name(1) );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::HNN_FREE );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.addDisabled( Text("Is") + Name(1) + "central in" + Name( Parent(1) ) 
		   + "?" );
    
    M.add( Ctor1<IsSGNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal in") + Name( Parent(1) ) 
	   + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "subnormal in" + Name( Parent(1) ) 
		   + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant in" 
		   + Name( Parent(1) ) + "?" );

    M.separator();
    
    M.add( Ctor1<HNNofFreeGroup_IsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + " trivial ?" );
    
    M.add( Ctor1<HNNofFreeGroup_IsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );
    
    M.addDisabled( Text("Compute the index of") + Name(1) + Text("in")
		   + Name( Parent(1) ) );
    
    M.addDisabled( Text("Compute a Schreier transversal of") + Name(1)
		   + Text("in") + Name( Parent(1) )  );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized") );

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );
    
    M.addDisabled( Text("Find a permutation representation of")
		   + Name( Parent(1) ) + Text("modulo") + Name(1) 
		   + Text("when the index of ") + Name(1) + " in "
		   + Name( Parent(1) ) + Text("is finite")  );

    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );

    M.addDisabled( Text("Enumerate relators of") + Name(1) );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FP );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.add( Ctor1<IsSGCentral,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("central ?") );
 
    M.add( Ctor1<IsSGNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal ?") );
 
    M.addDisabled( Text("Is") + Name(1) + "subnormal ?");
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic ?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant ?" );

    M.separator();
    
    M.add( Ctor1<IsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.add( Ctor1<IsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );

    M.add( ReadIsSGNilpotentItem(),
	   Text("Is") + Name(1) + Text("nilpotent ?"),
	   DefineInt("Enter the nilpotency class: ", 1) );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<SGIndexProblem, SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );

    M.add( Ctor1<SchreierTransversal, SMSubgroup>(),
	   Text("Compute a Schreier transversal of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized"));

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMSubgroup>(),
	   Text("Find a permutation representation of")
	   + Name( Parent(1) ) + Text("modulo") + Name(1) 
	   + Text("when the index of ") + Name(1) + " in " + Name( Parent(1) )
	   + Text("is finite") );

    M.separator();

    M.add( Ctor1<SGPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation for") + Name(1) );
    
    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );
 
    M.add( Ctor1<SGRelatorEnumerator,SMSubgroup>(),
	   Text("Enumerate relators of ") + Name( 1 ) );

    M.closeCascade();

    M.startCascade( Text("Run ...") );
 
    M.add( Ctor1<ACEProblem,SMSubgroup>(),
	   Text("Run Todd-Coxeter for") + Name( 1 )  );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.addDisabled( Text("Is") + Name(1) + Text("central ?") );

    M.add( Ctor1<SGOfFreeIsNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal ?") );
 
    M.addDisabled( Text("Is") + Name(1) + "subnormal ?");
 
    M.add( Ctor1<SGOfFreeIsMalnormal,SMSubgroup>(),
	   Text("Is") + Name(1) + "malnormal ?" );
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic ?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant ?" );

    M.separator();

    M.add( Ctor1<FreeIsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.addDisabled( Text("Is") + Name(1) + " abelian ?");

    M.addDisabled( Text("Is") + Name(1) + " nilpotent ?");

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("a free factor of") 
		   + Name( Parent(1) ) + " ?" );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<SGOfFreeIndex,SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );

    M.add( Ctor1<SGOfFreeRank,SMSubgroup>(),
	   Text("Compute the rank of") + Name(1) + " ?" );

    M.addDisabled( Text("Compute a Schreier transversal of") + Name(1)
		   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized"));
 
    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );
    
    M.separator();
    
    M.add( Ctor1<SGOfFreeNormaliser,SMSubgroup>(),
	   Text("Compute the normaliser of") + Name(1) + "in"
	   + Name( Parent(1) ) + "?"  );
 
    M.add( ReadNormalApproximation(),
	   Text("Compute the n-th normal approximation of ") + Name(1),
	   DefineInt("Enter the n  ", 1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor1<SGOfFreeHallCompletion,SMSubgroup>(),
	   Text("Find a virtual free factor of") + Name( Parent(1) )
	   + "containing" + Name(1) + "as a subgroup of finite index" );

    M.add( Ctor1<SGOfFreeNielsenBasis,SMSubgroup>(),
	   Text("Find a Nielsen basis for") + Name(1) );

    M.addDisabled( Text("Find a permutation representation of")
	   + Name( Parent(1) ) + Text("modulo") + Name(1) 
	   + Text("when the index of ") + Name(1) + " in " + Name( Parent(1) )
	   + Text("is finite") );
      
    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );
 
    M.addDisabled( Text("Enumerate relators of ") + Name( 1 ) );

    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE_NILPOTENT );
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.addDisabled( Text("Is") + Name(1) + "abelian ?");

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.separator();

    M.add( Ctor1<IsSGCentral,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("central ?") );

    M.add( Ctor1<SGOfNGisNormalProblem,SMSubgroup>(),
	   Text("Is") + Name(1) + "normal ?" );

    M.closeCascade();
 
    M.startCascade( Text("Compute ...") );

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the torsion subgroup of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("abelianized") );
    M.addDisabled( Text("Compute the class of") + Name(1) );

    M.addDisabled( Text("Compute the derived length of") + Name(1) );

    M.addDisabled( Text("Compute the lower central series of") + Name(1) );

    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute the derived series of") + Name(1) );

    M.addDisabled( Text("Compute quotients of the derived series of")
		   + Name(1) );

    M.add( Ctor1<SGOfNGhirschNumberProblem,SMSubgroup>(),
	   Text("Compute the Hirsch number of") + Name(1) );

    M.addDisabled( Text("Compute a Malcev basis for") + Name(1) );

    M.addDisabled( Text("Compute a short-lex system of representatives for")
		   + Name(1) );

    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );

    M.separator();
	
    M.add( Ctor1<SGOfNGindexProblem,SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + "in" + Name( Parent(1) ) );
    
    M.add( Ctor1<SGOfNGcomputeBasisProblem,SMSubgroup>(),
	   Text("Compute a basis for") + Name(1) );

    M.add( Ctor1<SGOfNGnormalClosureGensProblem,SMSubgroup>(),
	   Text("Compute the normal closure of") + Name(1) +
	   Text("in") + Name( Parent(1) ) );
 
    M.closeCascade();

    
    M.startCascade( Text("Find ...") );

    M.addDisabled( Text("Find a polycyclic presentation of") + Name(1) );

    M.separator();

    M.addDisabled( Text("Find a finite rewriting system for") + Name(1) );

    M.addDisabled( Text("Find a permutation presentation of")
		   + Name( Parent(1) ) + Text("modulo") + Name(1) 
		   + Text("when the index of ") + Name(1) + " in "
		   + Name( Parent(1) ) + Text("is finite") );

    M.separator();

    M.add( Ctor1<SGOfNGbuildPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation of") + Name(1) );
    
    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );
 
    M.add( Ctor1<IsSGCentral,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("central in") + Name( Parent(1) )
	   + "?" );

    M.add( Ctor1<SGOfNGisNormalProblem,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal in") + Name( Parent(1) )
	   + "?" );
 
    M.addDisabled( Text("Is") + Name(1) + "subnormal ?");
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic ?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant ?" );

    M.separator();

    M.add( Ctor1<IsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + "trivial ?" );
    
    M.addDisabled( Text("Is") + Name(1) + "finite ?" );

    M.add( Ctor1<IsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + "abelian ?" );

    M.addDisabled( Text("Is") + Name(1) + Text("free nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.addDisabled( Text("Compute the index of") + Name(1) + "in" 
		   + Name( Parent(1) ) );

    M.addDisabled( Text("Compute a Schreier transversal of") + Name(1)
		   + Text("in") + Name( Parent(1) ) );
    
    
    M.addDisabled( Text("Compute the isolator of ") + Name(1)
		   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the torsion subgroup of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1) 
		   + Text("abelianized") );
 
    M.add( Ctor1<SGOfNGcomputeNClassProblem,SMSubgroup>(),
	   Text("Compute the class of") + Name(1) );
    
    M.addDisabled( Text("Compute the derived length of") + Name(1) );

    M.addDisabled( Text("Compute the lower central series of") + Name(1) );

    M.addDisabled( Text("Compute lower central quotients of") + Name(1) );

    M.addDisabled( Text("Compute the derived series of") + Name(1) );

    M.addDisabled( Text("Compute quotients of the derived series of")
		   + Name(1) );

    M.add( Ctor1<SGOfNGhirschNumberProblem,SMSubgroup>(),
	   Text("Compute the Hirsch number of") + Name(1) );

    M.addDisabled( Text("Compute a Malcev basis for") + Name(1) );

    M.addDisabled( Text("Compute a short-lex system of representatives for")
		   + Name(1) );

    M.addDisabled( Text("Compute an integral homology group of") + Name(1) );

    M.separator();

    M.add( Ctor1<SGOfNGcomputeBasisProblem,SMSubgroup>(),
	   Text("Compute a basis for") + Name(1) );

    M.add( Ctor1<SGOfNGnormalClosureProblem,SMSubgroup>(),
	   Text("Compute the normal closure of") + Name(1) +
	   Text("in") + Name( Parent(1) ) );
  
    M.closeCascade();
    
    M.startCascade( Text("Find ...") );

    M.addDisabled( Text("Find a polycyclic presentation of") + Name(1) );

    M.separator();

    M.addDisabled( Text("Find a permutation presentation of")
		   + Name( Parent(1) ) + Text("modulo") + Name(1) 
		   + Text("when the index of ") + Name(1) + " in " 
		   + Name( Parent(1) ) + Text("is finite") );

    M.addDisabled( Text("Find a finite rewriting system for") + Name(1) );

    M.separator();

    M.add( Ctor1<SGOfNGbuildPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation of") + Name(1) );
 
    M.closeCascade();

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::ONE_RELATOR ||
		      CheckinType( Parent(1) ) == SMFPGroup::ONE_RELATOR_WITH_TORSION
		      );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.add( Ctor1<IsSGCentral,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("central ?") );
 
    M.add( Ctor1<IsSGNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal ?") );
 
    M.addDisabled( Text("Is") + Name(1) + "subnormal ?");
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic ?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant ?" );

    M.separator();
    
    M.add( Ctor1<IsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.add( Ctor1<IsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("nilpotent ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.separator();

    M.add( Ctor1<ORIsMagnusSubgroup,SMSubgroup>(),
	   Text("Is") + Name(1) + "a Magnus subgroup ?" );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<SGIndexProblem, SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );
    
    M.add( Ctor1<SchreierTransversal, SMSubgroup>(),
	   Text("Compute a Schreier transversal of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized") );

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMSubgroup>(),
	   Text("Find a permutation representation of")
	   + Name( Parent(1) ) + Text("modulo") + Name(1) 
	   + Text("when the index of ") + Name(1) + " in " + Name( Parent(1) )
	   + Text("is finite") );

    M.separator();

    M.add( Ctor1<SGPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation for") + Name(1) );
    
    M.closeCascade();

    M.startCascade( Text("Enumerate ...") );
 
    M.add( Ctor1<SGRelatorEnumerator,SMSubgroup>(),
	   Text("Enumerate relators of ") + Name( 1 ) );

    M.closeCascade();

    M.startItemGroup(CheckinType(Parent(1)) == SMFPGroup::SMALL_CANCELLATION );

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + "=" + Name( Parent(1) ) + "?" );
    
    M.addDisabled( Text("Is") + Name(1) + "of finite index in" 
		   + Name( Parent(1) ) + "?" );

    M.addDisabled( Text("Is") + Name(1) + "isolated in" + Name( Parent(1) ) 
		   + "?" );

    M.add( Ctor1<IsSGCentral,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("central ?") );
 
    M.add( Ctor1<IsSGNormal,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("normal ?") );
 
    M.addDisabled( Text("Is") + Name(1) + "subnormal ?");
    
    M.addDisabled( Text("Is") + Name(1) + "characteristic ?" );

    M.addDisabled( Text("Is") + Name(1) + "fully invariant ?" );

    M.separator();
    
    M.add( Ctor1<IsSGTrivial,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("trivial ?") );

    M.add( Ctor1<IsSGAbelian,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("abelian ?") );

    M.add( ReadIsSGNilpotentItem(),
	   Text("Is") + Name(1) + Text("nilpotent ?"),
	   DefineInt("Enter the nilpotency class: ", 1) );

    M.addDisabled( Text("Is") + Name(1) + Text("polycyclic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("solvable ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("perfect ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finite ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("finitely related ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("automatic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("hyperbolic ?") );

    M.addDisabled( Text("Is") + Name(1) + Text("free ?") );

    M.closeCascade();

    M.startCascade( Text("Compute ...") );

    M.add( Ctor1<SGIndexProblem, SMSubgroup>(),
	   Text("Compute the index of") + Name(1) + Text("in")
	   + Name( Parent(1) ) );

    M.add( Ctor1<SchreierTransversal, SMSubgroup>(),
	   Text("Compute a Schreier transversal of") + Name(1)
	   + Text("in") + Name( Parent(1) ) );

    M.separator();

    M.addDisabled( Text("Compute the order of") + Name(1) );

    M.addDisabled( Text("Compute the center of") + Name(1) );

    M.addDisabled( Text("Compute the canonical decomposition of") + Name(1)
		   + Text("abelianized") );

    M.addDisabled( Text("Compute a nilpotent quotient of") + Name(1) );

    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor1<PermutationRepProblem, SMSubgroup>(),
	   Text("Find a permutation representation of")
	   + Name( Parent(1) ) + Text("modulo") + Name(1) 
	   + Text("when the index of ") + Name(1) + " in " + Name( Parent(1) )
	   + Text("is finite") );

    M.separator();

    M.add( Ctor1<SGPresentationProblem,SMSubgroup>(),
	   Text("Find a presentation for") + Name(1) );
    
    M.closeCascade();


    M.startCascade( Text("Enumerate ...") );
 
    M.add( Ctor1<SGRelatorEnumerator,SMSubgroup>(),
	   Text("Enumerate relators of ") + Name( 1 ) );

    M.closeCascade();
 
    M.done();
  }
  
  {
    Menu1<SMSetOfWords> M( Menu::TOOLS );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );

    M.add( Ctor1<FreeGeneralIsPartOfBasisProblem,SMSetOfWords>(),
	   Text("Is") + Name(1) + "part of a basis of" +
	   Name( Parent(1) ) + "?" );
    
    M.done();
  }

  {
    Menu1<SMVectorOfWords> M( Menu::TOOLS );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );
    /*
    M.add( Ctor1<SGOfFreeWhiteheadReduction,SMVectorOfWords>(),
	   Text("Find Whitehead reduction of") + Name(1) 
	   );
    */  
    M.add( Ctor1<ACConjectureProblem,SMVectorOfWords>(),
	   Text("Find Andrews-Curtis reduction of") + Name(1) );

    M.add( Ctor1<FindWhiteheadMinimalProblem,SMVectorOfWords>(),
	   Text("Find Whitehead reduction of") + Name(1) + "(genetic)" );

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FP ||
		      CheckinType( Parent(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor1<FPNewPresentationProblem,SMVectorOfWords>(),
	   Text("Compute a presentation for") + Name( Parent(1) )
	   + Text("on new generators given by") + Name(1) );
  
    M.done();
  }

  {
    Menu1<SMMap> M( Menu::TOOLS );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::FREE &&
		      CheckinType( Range(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor1<ExtendToHomProblem,SMMap>(),
	   Text("Extend") + Name( 1 ) + "to a homomorphism" );

    M.startItemGroup( (CheckinType( Domain(1) ) != SMFPGroup::FREE ||
		      CheckinType( Range(1) ) != SMFPGroup::FREE ) );
    
    M.add( Ctor1<ExtendToHomProblem,SMMap>(),
	   Text("Does") + Name( 1 ) + "extend to a homomorphism ?" );

/*
	 M.startItemGroup( ( Domain(1) == Range(1) ) &&
			   ( CheckinType( Domain(1) ) == SMFPGroup::FREE ||
			     IsHomo( 1 ) )
			   );
	 
	 M.add( ReadPowerOfMapItem(),
		Text("Compute a power of") + Name(1),
		DefineInt("What power?", 2)
		);
*/	 
/*
	 M.startItemGroup( Domain(1) == Range(1) &&
			   CheckinType( Domain(1) ) == SMFPGroup::FREE
			   );

	 M.add( Ctor1<EndoOnFreeIsAut,SMMap>(),
		Text("Is") + Name(1) + "an automorphism of"
		+ Name( Domain(1) ) + "?"
		);

	 M.add( Ctor1<EndoOnFreeIsInner,SMMap>(),
		Text("Is") + Name(1) + "an inner automorphism of"
		+ Name( Domain(1) ) + "?"
		);
	 
	 M.startItemGroup( IsAuto( 1 ) &&
			   CheckinType( Domain(1) ) == SMFPGroup::FREE
			   );


         M.add( Ctor1<InverseAuto,SMMap>(),
	 Text("Compute the inverse of") + Name( 1 )
	 );
	 
	 M.separator();
	 
	 M.add( Ctor1<ExtendFreeByAut,SMMap>(),
	 Text("Extend") + Name( Domain(1) ) + "by" + Name(1)
	 );
*/	 

    M.done();
  }

  {
    Menu1<SMHomomorphism> M( Menu::TOOLS );

    M.startItemGroup( );

    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::ABELIAN 
		      && CheckinType( Domain(1) ) != SMFPGroup::FREE );
 
    M.addDisabled( Text("Is") + Name( 1 ) + "a monomorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::ABELIAN &&
		      CheckinType( Domain(1) ) != SMFPGroup::NILPOTENT &&
		      CheckinType( Domain(1) ) != SMFPGroup::FREE_NILPOTENT );

    M.addDisabled( Text("Is") + Name( 1 ) + "an epimorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::ABELIAN );

    M.add( Ctor1<AbelianHomIsMono,SMHomomorphism>(),
	   Text("Is") + Name(1) + "a monomorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::ABELIAN ||
		      CheckinType( Domain(1) ) == SMFPGroup::NILPOTENT ||
		      CheckinType( Domain(1) ) == SMFPGroup::FREE_NILPOTENT );

    M.add( Ctor1<AbelianHomIsEpi,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an epimorphism ?" );

    M.add( Ctor1<AbelianHomIsAuto,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an automorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::FREE );

    M.add( Ctor1<EndoOnFreeIsMono,SMHomomorphism>(),
	   Text("Is") + Name( 1 ) + "a monomorphism ?" );

    M.add( Ctor1<EndoOnFreeIsEpi,SMHomomorphism>(),
	   Text("Is") + Name( 1 ) + "an epimorphism ?" );

    M.add( Ctor1<EndoOnFreeIsAut,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an automorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::FREE &&
		      CheckinType( Domain(1) ) != SMFPGroup::ABELIAN &&
		      CheckinType( Domain(1) ) != SMFPGroup::NILPOTENT &&
		      CheckinType( Domain(1) ) != SMFPGroup::FREE_NILPOTENT );
	 
    M.addDisabled( Text("Is") + Name( 1 ) + "an automorphism ?" );

    M.startItemGroup( IsAuto(1) &&
		      CheckinType( Domain(1) ) == SMFPGroup::ABELIAN );
    M.separator();

    M.add( Ctor1<AbelianOrderOfAuto,SMHomomorphism>(),
	   Text("Compute the order of ") + Name(1) );
    
    M.add( Ctor1<AbelianInverseAuto,SMHomomorphism>(),
	   Text("Find the inverse of ") + Name(1) );

    M.add( Ctor1<AbelianFixedPointsOfAutoProblem,SMHomomorphism>(),
	   Text("Compute the fixed points of ") + Name(1) );

    M.startItemGroup( IsAuto(1) &&
		       CheckinType( Domain(1) ) == SMFPGroup::FREE_NILPOTENT );
    M.separator();

    M.add( Ctor1<AutoInFreeIsFinite,SMHomomorphism>(),
	   Text("Is") + Name(1) + "of finite order?" );
      
    M.add( Ctor1<FNGAutoIsIAAut,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an IA-automorphism ?" );
	
    M.add( Ctor1<NGInverseAuto,SMHomomorphism>(),
	   Text("Find the inverse of ") + Name(1) );
	
    M.startItemGroup( IsAuto(1) &&
		      CheckinType( Domain(1) ) == SMFPGroup::NILPOTENT );
    M.separator();
    
    M.add( Ctor1<NGAutoIsIAAut,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an IA-automorphism ?" );

    M.startItemGroup( IsAuto(1) &&
		      CheckinType( Domain(1) ) == SMFPGroup::FREE );
    M.separator();
    
    M.add( Ctor1<AutoInFreeIsFinite,SMHomomorphism>(),
	   Text("Is") + Name(1) + "of finite order?" );   

    M.add( Ctor1<EndoOnFreeIsIAAut,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an IA-automorphism ?" );

 
    M.add( Ctor1<EndoOnFreeIsInner,SMHomomorphism>(),
	   Text("Is") + Name(1) + "an inner automorphism ?" );

    M.add( Ctor1<InverseAuto,SMHomomorphism>(),
	   Text("Compute the inverse of") + Name( 1 ) );

    M.separator();

    M.add( Ctor1<ExtendFreeByAut,SMHomomorphism>(),
	   Text("Extend") + Name( Domain(1) ) + "by" + Name(1) );

    M.add( Ctor1<AutoWhiteheadDecomposition,SMHomomorphism>(),
	   Text("Find Whitehead decomposition of") + Name(1) );
	 	 
    M.startItemGroup( !IsAuto(1) ||
		      (CheckinType( Domain(1) ) != SMFPGroup::FREE &&
		       CheckinType( Domain(1) ) != SMFPGroup::FREE_NILPOTENT &&
		       CheckinType( Domain(1) ) != SMFPGroup::NILPOTENT &&
		       CheckinType( Domain(1) ) != SMFPGroup::ABELIAN) );

    M.addDisabled( Text("Is") + Name( 1 ) + "an IA-automorphism ?" );

    M.startItemGroup( !IsAuto(1) ||
		     CheckinType( Domain(1) ) != SMFPGroup::FREE );

    M.addDisabled( Text("Is") + Name( 1 ) + "an inner automorphism ?"  );

    M.done();
  }

  {
    Menu1<SMHomomorphism2> M( Menu::TOOLS );

    M.startItemGroup( );

    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::ABELIAN );

    M.addDisabled( Text("Is") + Name( 1 ) + "a monomorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::ABELIAN &&
		      CheckinType( Domain(1) ) != SMFPGroup::FREE_NILPOTENT &&
		      CheckinType( Domain(1) ) != SMFPGroup::NILPOTENT );
    
    M.addDisabled( Text("Is") + Name( 1 ) + "an epimorphism ?" );
    
    M.startItemGroup( CheckinType( Domain(1) ) != SMFPGroup::ABELIAN );

    M.addDisabled( Text("Is") + Name( 1 ) + "an isomorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::ABELIAN );

    M.add( Ctor1<AbelianHomIsMono,SMHomomorphism2>(),
	   Text("Is") + Name(1) + "a monomorphism ?" );
	 
    M.add( Ctor1<AbelianHomIsEpi,SMHomomorphism2>(),
	   Text("Is") + Name(1) + "an epimorphism ?" );

    M.add( Ctor1<AbelianHomIsIso,SMHomomorphism2>(),
	   Text("Is") + Name(1) + "an isomorphism ?" );

    M.startItemGroup( CheckinType( Domain(1) ) == SMFPGroup::NILPOTENT ||
		      CheckinType( Domain(1) ) == SMFPGroup::FREE_NILPOTENT );

    M.add( Ctor1<AbelianHomIsEpi,SMHomomorphism2>(),
	   Text("Is") + Name(1) + "an epimorphism ?" );

    M.startItemGroup( IsAuto( 1 ) &&
		      CheckinType( Domain(1) ) == SMFPGroup::FREE );

    M.add( Ctor1<InverseAuto,SMHomomorphism2>(),
	   Text("Compute the inverse of") + Name( 1 ) );
	 
    M.startItemGroup( ( CheckinType( Domain(1) ) == SMFPGroup::FP ) &&
		      ( CheckinType( Range(1) ) == SMFPGroup::FP  ) );
    
    M.add( Ctor1<FPKernelPresentationProblem,SMHomomorphism2>(),
	   Text("Compute a presentation for the kernel of") + Name( 1 ) );
	
    M.separator();
	 
    M.add( Ctor1<ExtendFreeByAut,SMHomomorphism2>(),
	   Text("Extend") + Name( Domain(1) ) + "by" + Name(1) );
	 	 
    M.done();
  }

  {
    Menu1<SMEquation2> M( Menu::TOOLS );

    M.startItemGroup( CheckinType( Parent( 1 ) ) == SMFPGroup::FREE );

    M.add( Ctor1<GAEquationProblem, SMEquation2>(),
	   Text("Find a solution for") + Name(1) );

    M.startItemGroup( CheckinType( Parent( 1 ) ) == SMFPGroup::FP ||
		      CheckinType( Parent( 1 ) ) == SMFPGroup::ABELIAN );
    
    M.add( Ctor1<EquationInFPProblem , SMEquation2>(),
	   Text("Find all solutions for") + Name(1) );

    M.done();
  }

  {
    Menu1<SMEquation> M( Menu::TOOLS );

    M.startItemGroup();

    M.add( Ctor1<QuadEquationSurfaceForm, SMEquation>(),
	   Text("Find a surface form of") + Name(1) );

	 
    M.add( Ctor1<EquationProblem,SMEquation>(),
	   Text("Solve the equation") + Name(1) + " = 1" );
	 	 
    M.add( Ctor1<QuickEquationProblem,SMEquation>(),
	   Text("Find basic solutions of the equation") + Name(1) + " = 1" );

    /* @dp Debug only menu items.
       M.separator();

       M.add( Ctor1<QEquationInFreeBasicSolutions,SMEquation>(),
       Text("View known basic solutions of ") + Name(1)
       );

       M.add( Ctor1<QEquationInFreeRegStabGenerators,SMEquation>(),
       Text("View known generators of regular stabilizer of") + Name(1)
       );
    */

    M.done();
  } 

  {
    Menu1<SMEqSystem> M( Menu::TOOLS );

    M.startItemGroup();

    M.add( Ctor1<EqSystemInFPProblem , SMEqSystem>(),
	   Text("Find solutions for") + Name(1) );

    M.done();
  }

  {
    Menu2<SMWord,SMWord> M( Menu::TOOLS );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );
    
    M.add( Ctor2<ProductInAbelianForm, SMWord, SMWord>(),
	   Text("Compute the product") + Name(1) + Name(2)
	   + Text(", in abelian form") );

    M.add( Ctor2<ProductInAbelianForm, SMWord, SMWord>(true),
	   Text("Compute the product") + Name(2) + Name(1)
	   + Text(", in abelian form") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) != SMFPGroup::ABELIAN );
    
    M.add( Ctor2<FormalProductOfWords,SMWord,SMWord>(),
	   Text("Compute the product") + Name(1) + " * " + Name(2) );
    
    M.add( Ctor2<FormalProductOfWords,SMWord,SMWord>(true),
	   Text("Compute the product") + Name(2) + " * " + Name(1) );
 
    M.add( Ctor2<ConjugateOfWord,SMWord,SMWord>(),
	   Text("Compute the conjugate of") + Name(1) + Text("by") + Name(2) );
    
    M.add( Ctor2<ConjugateOfWord,SMWord,SMWord>(true),
	   Text("Compute the conjugate of") + Name(2) + Text("by") + Name(1) );
    
    M.add( Ctor2<CommutatorOfWords,SMWord,SMWord>(),
	   Text("Compute the commutator of") + Name(1) + Text("and") +Name(2));

    M.add( Ctor2<CommutatorOfWords,SMWord,SMWord>(true),
	   Text("Compute the commutator of") + Name(2) + Text("and") +Name(1));
    
    M.startItemGroup();
 
    M.separator();
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );
 
    M.add( Ctor2<AbelianEqualityProblem, SMWord, SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );

    M.add( Ctor2<IsAbelianWordPowerOfSecond,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("a power of") + Name(2) + Text("?") );

    M.add( Ctor2<IsAbelianWordPowerOfSecond,SMWord,SMWord>(true),
	   Text("Is") + Name(2) + Text("a power of") + Name(1) + Text("?") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor2<WordsAreEqual,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
   
    M.add( Ctor2<ConjugacyProblemInFree,SMWord,SMWord>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate in"
	   + Name( Parent(1) ) + "?" );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::FREE_NILPOTENT );
  
    M.add( Ctor2<AreEltsEqual, SMWord, SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    /* 
       M.addDisabled( Text("Are") + Name(1) + Text("and") + Name(2)
       + Text("conjugate in") + Name( Parent( 1 ) ) + Text("?")
       );
    */
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      (CheckinType( Parent(1) ) == SMFPGroup::AP_FREE ||
		       CheckinType( Parent(1) ) == SMFPGroup::AP_FREE_CYCLIC));
    
    M.add( Ctor2<APOfFreeAreEqual,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::HNN_FREE );
    
    M.add( Ctor2<HNNofFreeGroup_AreEqual,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      (CheckinType( Parent(1) ) == SMFPGroup::FP ||
		       CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT) );
    
    M.add( Ctor2<AreEltsEqual,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT );
    
    M.addDisabled( Text("Are") + Name(1) + Text("and") + Name(2)
		   + Text("conjugate in") + Name( Parent( 1 ) ) + Text("?") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::ONE_RELATOR_WITH_TORSION );
    
    M.add( Ctor2<ORWithTorsionAreEltsEqual,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    
    M.add( Ctor2<ORWithTorsionConjugacyProblem,SMWord,SMWord>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate in"
	   + Name( Parent(1) ) + "?" );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) != SMFPGroup::FREE &&
		      CheckinType( Parent(1) ) != SMFPGroup::ABELIAN &&
		      CheckinType( Parent(1) ) != SMFPGroup::NILPOTENT &&
		      CheckinType( Parent(1) ) != SMFPGroup::FREE_NILPOTENT &&
		      CheckinType( Parent(1) ) != SMFPGroup::AP_FREE_CYCLIC &&
		      CheckinType( Parent(1) ) != SMFPGroup::HNN_FREE &&
		      CheckinType( Parent(1) ) != SMFPGroup::FREE );
    
    M.add( Ctor2<ConjugacyProblem,SMWord,SMWord>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate in"
	   + Name( Parent(1) ) + "?"  );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::AP_FREE_CYCLIC );
    
    M.add( Ctor2<APOfFreeCyclic_ConjugacyProblem,SMWord,SMWord>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate in"
	   + Name( Parent(1) ) + "?" );
    
 
    M.add( Ctor2<APOfFreeCyclic_IsEltAProperPowerOfSecond,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("a proper power of")
	   + Name(2) + Text("?") );
    
    M.add( Ctor2<APOfFreeCyclic_IsEltAProperPowerOfSecond,SMWord,SMWord>(true),
	   Text("Is") + Name(2) + Text("a proper power of")
	   + Name(1) + Text("?") );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::HNN_FREE );
    
    M.add( Ctor2<HNNofFreeGroup_ConjugacyProblem,SMWord,SMWord>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate in"
	   + Name( Parent(1) ) + "?" );
    
 
    M.add( Ctor2<HNNofFreeGroup_IsProperPowerOfSecond,SMWord,SMWord>(),
	   Text("Is") + Name(1) + Text("a proper power of")
	   + Name(2) + Text("?") );
    
    M.add( Ctor2<HNNofFreeGroup_IsProperPowerOfSecond,SMWord,SMWord>(true),
	   Text("Is") + Name(2) + Text("a proper power of")
	   + Name(1) + Text("?") );
   
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) != SMFPGroup::FREE &&
		      CheckinType( Parent(1) ) != SMFPGroup::FREE_NILPOTENT &&
		      CheckinType( Parent(1) ) != SMFPGroup::AP_FREE_CYCLIC &&
		      CheckinType( Parent(1) ) != SMFPGroup::ABELIAN );
    
    M.addDisabled( Text("Is") + Name(1) + Text("a proper power of")
		   + Name(2) + Text("?") );
    
    M.addDisabled( Text("Is") + Name(2) + Text("a proper power of")
		   + Name(1) + Text("?") );

    M.done();
  }
  
  {
    Menu2<SMHomomorphism,SMHomomorphism> M( Menu::TOOLS );
  
    M.startItemGroup( Range( 1 ) == Domain( 2 ) );
    
    M.add( Ctor2<ComposeMaps,SMHomomorphism,SMHomomorphism>(),
	   Text("Form the composition") + Name(2) + Text(Name(1)) );
    
    M.startItemGroup( Range( 2 ) == Domain( 1 ) );
    
    M.add( Ctor2<ComposeMaps,SMHomomorphism,SMHomomorphism>(true),
	   Text("Form the composition") + Name(1) + Text(Name(2)) );
 
    M.startItemGroup( Domain( 1 ) == Domain( 2 ) &&
		      CheckinType( Domain(1) ) != SMFPGroup::FREE );
    
    M.addDisabled( Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    
    
    M.startItemGroup( Domain( 1 ) == Domain( 2 ) &&
		      CheckinType( Domain(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor2<FreeAreHomsEqual,SMHomomorphism,SMHomomorphism>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    
    M.done();
  }
  
  {
    Menu2<SMHomomorphism2,SMHomomorphism2> M( Menu::TOOLS );
    
    M.startItemGroup( Range( 1 ) == Domain( 2 ) );
    
    M.add( Ctor2<ComposeMaps,SMHomomorphism2,SMHomomorphism2>(),
	   Text("Form the composition") + Name(2) + Text(Name(1))  );
    
    M.startItemGroup( Range( 2 ) == Domain( 1 ) );
    
    M.add( Ctor2<ComposeMaps,SMHomomorphism2,SMHomomorphism2>(true),
	   Text("Form the composition") + Name(1) + Text(Name(2)) );
    
    M.startItemGroup( Domain(1) == Domain(2) &&
		      Range(1) == Range(2) &&
		      CheckinType( Domain(1) ) != SMFPGroup::FREE );
	 
    M.addDisabled( Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
	 
    M.startItemGroup( Domain(1) == Domain(2) &&
		      Range(1) == Range(2) &&
		      CheckinType( Domain(1) ) == SMFPGroup::FREE );
	 
    M.add( Ctor2<FreeAreHomsEqual,SMHomomorphism2,SMHomomorphism2>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );
    
    M.done();
  }

  {
    Menu2<SMHomomorphism,SMHomomorphism2> M( Menu::TOOLS );
    
    M.startItemGroup( Range( 1 ) == Domain( 2 ) );
	 
    M.add( Ctor2<ComposeMaps,SMHomomorphism,SMHomomorphism2>(),
	   Text("Form the composition") + Name(2) + Text(Name(1)) );
	 
    M.startItemGroup( Range( 2 ) == Domain( 1 ) );
	 
    M.add( Ctor2<ComposeMaps,SMHomomorphism,SMHomomorphism2>(true),
	   Text("Form the composition") + Name(1) + Text(Name(2)) );
    
    M.done();
  }

  {
    Menu2<SMHomomorphism, SMWord> M( Menu::TOOLS );

    M.startItemGroup( Parent( 2 ) == Domain( 1 ) );
	 
    M.add( Ctor2<ImageUnderMap,SMHomomorphism,SMWord>(),
	   Text("Compute the image of") + Name(2) + "under" + Name(1) );
	 
    M.done();
  }

  {
    Menu2<SMHomomorphism2, SMWord> M( Menu::TOOLS );

    M.startItemGroup( Parent( 2 ) == Domain( 1 ) );
	 
    M.add( Ctor2<ImageUnderMap,SMHomomorphism2,SMWord>(),
	   Text("Compute the image of") + Name(2) + "under" + Name(1) );
	 
    M.done();
  }
  
  {
    Menu2<SMHomomorphism, SMSubgroup> M( Menu::TOOLS );

    M.startItemGroup( Parent( 2 ) == Domain( 1 ) );
	 
    M.add( Ctor2<SGImageUnderMap,SMHomomorphism,SMSubgroup>(),
	   Text("Compute the image of") + Name(2) + "under" + Name(1) );
	 
    M.done();
  }

  {
    Menu2<SMHomomorphism2, SMSubgroup> M( Menu::TOOLS );

    M.startItemGroup( Parent( 2 ) == Domain( 1 ) );
	 
    M.add( Ctor2<SGImageUnderMap,SMHomomorphism2,SMSubgroup>(),
	   Text("Compute the image of") + Name(2) + "under" + Name(1) );
	 
    M.done();
  }

  //!!!!!!!!!!!!

  {
    Menu2<SMSubgroup,SMWord> M( Menu::TOOLS );

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );


    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor2<WordInSGOfAbelian, SMSubgroup, SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?" );
 
    M.add( Ctor2<WordPowerInSGOfAbelian, SMSubgroup, SMWord>(),
	   Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.add( Ctor2<WordInSGBasisOfAbelian, SMSubgroup, SMWord>(),
	   Text("Is") + Name(2) + "part of a canonical basis for" 
	   + Name(1) + "?" );
    
    M.closeCascade();

    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor2<AbelianSGWordProblem,SMSubgroup,SMWord>(),
	   Text("Rewrite") + Name(2) + "in terms of a canonical basis for"
	   + Name(1) );
    
    M.closeCascade();
    
    M.startCascade( Text("Compute ...") );

    M.add( Ctor2<AbelianEltPowerSubgr,SMSubgroup,SMWord>(),
	   Text("Compute the order of") + Name(2) + "in the factor group" 
	   + Name( Parent(1) ) + "/" + Name(1) );

    M.add( Ctor2<AbelianQuotientWordProblem,SMSubgroup,SMWord>(),
	   Text("Compute the the canonical decomposition of") 
	   + Name(2) + "in terms of a canonical basis for" 
	   + Name( Parent(1) ) + "/" + Name(1) );

    M.closeCascade();

    M.startItemGroup( Parent(1) == Parent(2) && 
		      ( CheckinType(Parent(1)) == SMFPGroup::AP_FREE ||
			CheckinType(Parent(1)) == SMFPGroup::AP_FREE_CYCLIC ||
			CheckinType(Parent(1)) == SMFPGroup::HNN_FREE ) );
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
 
    M.add( Ctor2<SubgroupContainmentProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?" );
 
    M.addDisabled( Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.closeCascade();



    M.startItemGroup( Parent(1) == Parent(2) && 
		      (CheckinType(Parent(1))==SMFPGroup::FP || 
		       CheckinType(Parent(1))==SMFPGroup::SMALL_CANCELLATION));


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor2<SubgroupContainmentProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?" );
 
    M.addDisabled( Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is") + Name(2) + "part of a canonical basis for" 
		   + Name(1) + "?" );
    
    M.closeCascade();


    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor2<SGRewriteWordProblem,SMSubgroup,SMWord>(),
	   Text("Rewrite") + Name(2) + " in terms of Schreier generators of " 
	   + Name(1) );	 
    
    M.closeCascade();

    
    M.startCascade( Text("Find ...") );

    M.add( Ctor2<WordRepresentativeProblem, SMSubgroup,SMWord>(),
	   Text("Find a Schreier representative of ") + Name(2) +
	   " modulo " + Name(1) );	 

    M.closeCascade();



    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::FREE );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor2<WordInSGOfFree,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?" );

    M.add( Ctor2<PowerOfWordInSGOfFree,SMSubgroup,SMWord>(),
	   Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );
 
    M.add( Ctor2<ConjugateOfWordInSGOfFree,SMSubgroup,SMWord>(),
	   Text("Is a conjugate of") + Name(2) + "in" + Name(1) + "?" );
 
    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is") + Name(2) + "primitive in" + Name(1) + "?" );
    
    M.closeCascade();


    M.startCascade( Text("Rewrite ...") );

    M.add( Ctor2<WordInNielsenBasisSGOfFree,SMSubgroup,SMWord>(),
	   Text("Rewrite") + Name(2) + "in term of a Nielsen basis for" 
	   + Name(1) );

    M.closeCascade();

    
    M.startCascade( Text("Compute ...") );

    M.addDisabled( Text("Compute the length of") + Name(2) 
		   + "when rewritten as a reduced word in a Nielsen basis for" 
		   + Name(1) );

    M.addDisabled( Text("Compute the representative of") + Name(2)
	   + "in a left Schreier transversal of" + Name(1) 
	   + "in" + Name( Parent(1) ) );

    M.add( Ctor2<SchreierRepOfWordInSGOfFree,SMSubgroup,SMWord>(),
	   Text("Compute the representative of") + Name(2)
	   + "in a right Schreier transversal of" + Name(1) 
	   + "in" + Name( Parent(1) ) );

    M.closeCascade();
     
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) && 
		      (CheckinType( Parent(1) ) == SMFPGroup::FREE_NILPOTENT ||
		       CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT));
    
   
    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor2<SGOfNGWordContainProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?"
	   );
    
    M.addDisabled( Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is") + Name(2) + "part of a canonical basis for" 
		   + Name(1) + "?" );
    
    M.closeCascade();



    M.startCascade( Text("Rewrite ...") );

    M.addDisabled( Text("Rewrite") + Name(2) 
		   + "in terms of a Malcev basis for" + Name(1) );

    M.separator();

    M.add( Ctor2<SGOfNGDecomposeWordProblem,SMSubgroup,SMWord>(),
	   Text("Rewrite") + Name(2) 
	   + "in terms of a basis for" + Name(1)
	   );
	
    M.closeCascade();



    M.startItemGroup( Parent( 1 ) == Parent( 2 ) && 
		      CheckinType( Parent(1) ) == SMFPGroup::ONE_RELATOR
		      );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor2<SubgroupContainmentProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?"
	   );
 
    M.addDisabled( Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is") + Name(2) + "part of a canonical basis for" 
		   + Name(1) + "?" );
    
    M.separator();

    M.add( Ctor2<ORExtendedWordProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "an element of Magnus subgroup"
	   + Name(1) + "?"
	   );

    M.closeCascade();



    M.startItemGroup( Parent( 1 ) == Parent( 2 ) && 
		      CheckinType( Parent(1) ) 
		      == SMFPGroup::ONE_RELATOR_WITH_TORSION
		      );


    M.startCascade( Text("Is") + Name(1) + Text("...") );

    M.add( Ctor2<SubgroupContainmentProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "in" + Name(1) + "?"
	   );
 
    M.addDisabled( Text("Is a power of") + Name(2) + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a power of an automorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );

    M.addDisabled( Text("Is a non-trivial endomorphic image of") + Name(2) 
		   + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is a power of a non-trivial endomorphic image of") 
		   + Name(2) + "in" + Name(1) + "?" );
    
    M.addDisabled( Text("Is") + Name(2) + "part of a canonical basis for" 
		   + Name(1) + "?" );
    
    M.separator();

    M.add( Ctor2<ORWithTorsionExtendedWordProblem,SMSubgroup,SMWord>(),
	   Text("Is") + Name(2) + "an element of Magnus subgroup"
	   + Name(1) + "?"
	   );

    M.closeCascade();

    M.done();
  }
  
  {
    Menu2<SMSubgroup,SMSubgroup> M( Menu::TOOLS );
	 
    //@@@@@@@@@@@@

    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::ABELIAN );
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor2<AbelianSGEqualityProblem,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );

    M.add( Ctor2<AbelianSGContainmentProblem,SMSubgroup,SMSubgroup>(true),
	   Text("Is") + Name(1) + "contained in" + Name(2) + "?"  );

    M.add( Ctor2<AbelianSGContainmentProblem,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(2) + "contained in" + Name(1) + "?"  );

    M.addDisabled( Text("Is an automorphic image of") + Name(1) 
		   + "contained in" + Name(2) + "?"  );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "contained in" + Name(1) + "?" );

    M.add( Ctor2<AbelianSGDirectFactorProblem,SMSubgroup,SMSubgroup>(true),
	   Text("Is") + Name(1) + "a direct factor of" + Name(2) + "?" );

    M.add( Ctor2<AbelianSGDirectFactorProblem,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(2) + "a direct factor of" + Name(1) + "?" );

    M.separator();

    M.add( Ctor2<AbelianIsIsomorphicSG,SMSubgroup,SMSubgroup>(),
	   Text("Is ") + Name(1) + "isomorphic to" + Name(2) + "?" );
    
    M.closeCascade();
    
    M.startCascade( Text("Find ...") );

    M.add( Ctor2<AbelianSGJoin,SMSubgroup,SMSubgroup>(),
	   Text("Find the join of") + Name(1) + "and" + Name(2) );
    
    M.add( Ctor2<AbelianSGIntersectionProblem,SMSubgroup,SMSubgroup>(),
	   Text("Find the intersection of") + Name(1) + "and" + Name(2) );
    
    M.closeCascade();
    
    M.startItemGroup( Parent( 1 ) == Parent(2) && 
		      (CheckinType(Parent(1))==SMFPGroup::AP_FREE ||
		       CheckinType(Parent(1))==SMFPGroup::AP_FREE_CYCLIC ||
		       CheckinType(Parent(1))==SMFPGroup::HNN_FREE ||
		       CheckinType(Parent(1))==SMFPGroup::FP ||
		       CheckinType(Parent(1))==SMFPGroup::SMALL_CANCELLATION ||
		       CheckinType(Parent(1))==SMFPGroup::ONE_RELATOR ||
		       CheckinType(Parent(1))==SMFPGroup::ONE_RELATOR_WITH_TORSION));
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.addDisabled( Text("Is") + Name(1) + Text("=") + Name(2) + Text("?") );

    M.addDisabled( Text("Is") + Name(1) + "contained in" + Name(2) + "?" );

    M.addDisabled( Text("Is") + Name(2) + "contained in" + Name(1) + "?" );

    M.addDisabled( Text("Is an automorphic image of") + Name(1) 
		   + "contained in" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "contained in" + Name(1) + "?"
		   );

    M.addDisabled( Text("Is") + Name(1) 
		   + "a free factor of" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is") + Name(2) 
		   + "a free factor of" + Name(1) + "?"
		   );
    
    M.addDisabled( Text("Is the intersection of") + Name(1) + "and" + Name(2) 
		   + "finitely generated ?"
		   );
    
    M.closeCascade();


    M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
		      CheckinType( Parent(1) ) == SMFPGroup::FREE
		      );
    
    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor2<SGOfFreeAreEqual,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?")
	   );

    M.add( Ctor2<SGOfFreeContainment,SMSubgroup,SMSubgroup>(true),
	   Text("Is") + Name(1) + "contained in" + Name(2) + "?"
	   );

    M.add( Ctor2<SGOfFreeContainment,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(2) + "contained in" + Name(1) + "?"
	   );

    M.add( Ctor2<SGOfFreeContainsConjugate,SMSubgroup,SMSubgroup>(true),
	   Text("Is a conjugate of") + Name(1) + "contained in" + Name(2) + "?"
	   );

    M.add( Ctor2<SGOfFreeContainsConjugate,SMSubgroup,SMSubgroup>(),
	   Text("Is a conjugate of") + Name(2) + "contained in" + Name(1) + "?"
	   );

    M.addDisabled( Text("Is an automorphic image of") + Name(1) 
		   + "contained in" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "contained in" + Name(1) + "?"
		   );

    M.addDisabled( Text("Is") + Name(1) 
		   + "a free factor of" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is") + Name(2) 
		   + "a free factor of" + Name(1) + "?"
		   );
    
    M.closeCascade();
    

    M.startCascade( Text("Compute ...") );
    
    M.addDisabled( Text("Compute the rank of the join of") + Name(1) + "and" + Name(2) );
 
    M.addDisabled( Text("Compute the rank of the intersection of") + Name(1) + "and" + Name(2) );
 
    M.closeCascade();

    M.startCascade( Text("Find ...") );

    M.add( Ctor2<SGOfFreeJoin,SMSubgroup,SMSubgroup>(),
	   Text("Find the join of") + Name(1) + "and" + Name(2) );

    M.add( Ctor2<SGOfFreeIntersection,SMSubgroup,SMSubgroup>(),
	   Text("Find the intersection of") + Name(1) + "and" + Name(2) );

    M.closeCascade();

    M.startCascade( Text("Are ...") );
    
    M.add( Ctor2<SGOfFreeConjugateTo,SMSubgroup,SMSubgroup>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "conjugate ?"
	   );

    M.closeCascade();


    M.startItemGroup( Parent( 1 ) == Parent( 2 ) && 
		      ( CheckinType( Parent(1) ) == SMFPGroup::FREE_NILPOTENT
		      || CheckinType( Parent(1) ) == SMFPGroup::NILPOTENT
			)
		      );
    

    M.startCascade( Text("Is") + Name(1) + Text("...") );
    
    M.add( Ctor2<SGOfNGequalSubgroupProblem,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(1) + Text("=") + Name(2) + Text("?")
	   );

    M.add( Ctor2<SGOfNGcontainSubgroupProblem,SMSubgroup,SMSubgroup>(true),
	   Text("Is") + Name(1) + "contained in" + Name(2) + "?"
	   );

    M.add( Ctor2<SGOfNGcontainSubgroupProblem,SMSubgroup,SMSubgroup>(),
	   Text("Is") + Name(2) + "contained in" + Name(1) + "?"
	   );

    M.addDisabled( Text("Is an automorphic image of") + Name(1) 
		   + "contained in" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is an automorphic image of") + Name(2) 
		   + "contained in" + Name(1) + "?"
		   );

    M.addDisabled( Text("Is") + Name(1) 
		   + "a free factor of" + Name(2) + "?"
		   );

    M.addDisabled( Text("Is") + Name(2) 
		   + "a free factor of" + Name(1) + "?"
		   );
    
    M.closeCascade();
    

    M.startCascade( Text("Compute ...") );
    
    M.addDisabled( 
      Text("Compute the minimal number of generators of the join of") 
      + Name(1) + "and" + Name(2) 
      );
 
    M.addDisabled( 
      Text("Compute the minimal number of generators of the intersection of") 
      + Name(1) + "and" + Name(2) 
      );
 
    M.closeCascade();


    M.startCascade( Text("Find ...") );
 
    M.add( Ctor2<SGOfNGjoinSubgroupProblem,SMSubgroup, SMSubgroup>(),
	   Text("Find a Malcev basis for the join of") 
	   + Name(1) + "and" + Name(2)
	   );

    M.addDisabled( Text("Find a Malcev basis for the intersection of") 
		   + Name(1) + "and" + Name(2)
		   );

    M.closeCascade();

    M.done();
  }

  {
    Menu2<SMFPGroup,SMFPGroup> M( Menu::TOOLS );

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::ABELIAN &&
		      CheckinType( 2 ) == SMFPGroup::ABELIAN
		      );

    M.add( Ctor2<AbelianIsomProblem,SMFPGroup,SMFPGroup>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "isomorphic ?"
	   );
   
    M.startItemGroup( ( CheckinType( 1 ) == SMFPGroup::NILPOTENT &&
			CheckinType( 2 ) == SMFPGroup::NILPOTENT ) ||
		      ( CheckinType( 1 ) == SMFPGroup::FP &&
			CheckinType( 2 ) == SMFPGroup::FP )
		      );
    
    M.add( Ctor2<NGIsomorphismProblem,SMFPGroup,SMFPGroup>(),
	   Text("Are") + Name(1) + "and" + Name(2) + "isomorphic ?"
	   );

    M.startItemGroup( ( CheckinType( 1 ) == SMFPGroup::FP ||
			CheckinType( 1 ) == SMFPGroup::FREE ) &&
		      ( CheckinType( 2 ) == SMFPGroup::FP ||
			CheckinType( 2 ) == SMFPGroup::FREE )
		      );
    
    M.add( Ctor2<TotalHomEnumeratorProblem,SMFPGroup,SMFPGroup>(),
	   Text("Totally enumerate homomorphisms from ") + Name(1) +
	   "to" + Name(2)
	   );
    
    M.add( Ctor2<TotalHomEnumeratorProblem,SMFPGroup,SMFPGroup>(true),
	   Text("Totally enumerate homomorphisms from ") + Name(2) +
	   "to" + Name(1)
	   );
    
    M.add( Ctor2<RandHomEnumeratorProblem,SMFPGroup,SMFPGroup>(),
	   Text("Randomly enumerate homomorphisms from ") + Name(1) +
	   "to" + Name(2)
	   );
    
    M.add( Ctor2<RandHomEnumeratorProblem,SMFPGroup,SMFPGroup>(true),
	   Text("Randomly enumerate homomorphisms from ") + Name(2) +
	   "to" + Name(1)
	   );

    M.done();
  }


  {
    Menu1< SMList<Word> > M( Menu::TOOLS );
    
    M.startItemGroup( );
    
    M.add( Ctor1<WordsOfLength, SMList<Word> >(),
	   Text("Extract words of a given length from ") + Name(1)
	   );	 
   
    M.add( Ctor1<SMListExtractTrivialWords, SMList<Word> >(),
	   Text("Extract trivial words from ") + Name(1)
	   );	  

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor1<SMListExtractProperPowerInFree, SMList<Word> >(),
	   Text("Extract proper powers from ") + Name(1)
	   );	  
  
    M.add( Ctor1<SMListExtractCommutatorsInFree, SMList<Word> >(),
	   Text("Extract commutators from ") + Name(1)
	   );	  

    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FP );
    
    M.add( Ctor1<SMListExtractCentralWords, SMList<Word> >(),
	   Text("Extract central words from ") + Name(1)
	   );	    

    M.done();
  }

  {
    Menu1< SMList<Subgroup> > M( Menu::TOOLS );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE ||
		      CheckinType( Parent(1) ) == SMFPGroup::FP );
    
    M.add( Ctor1<SGListExtractOfIndex, SMList<Subgroup> >(),
	   Text("Extract subgroups of a given index from ") + Name(1)
	   );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor1<SGListExtractNormal, SMList<Subgroup> >(),
	   Text("Extract normal subgroups from ") + Name(1)
	   );	 
    
    M.add( Ctor1<SGListExtractMalnormal, SMList<Subgroup> >(),
	   Text("Extract malnormal subgroups from ") + Name(1)
	   );	 
    
    M.add( Ctor1<SGListExtractOfRank, SMList<Subgroup> >(),
	   Text("Extract subgroups of a given rank from ") + Name(1)
	   );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FP );
    
    M.add( Ctor1<SMListExtractTrivialSubgroups, SMList<Subgroup> >(),
	   Text("Extract trivial subgroups from ") + Name(1)
	   );	  

     M.add( Ctor1<SMListExtractAbelianSubgroups, SMList<Subgroup> >(),
	   Text("Extract abelian subgroups from ") + Name(1)
	   );	    

     M.add( Ctor1<SMListExtractCentralSubgroups, SMList<Subgroup> >(),
	    Text("Extract central subgroups from ") + Name(1)
	    );	    
     
     M.done();
  }

 {
    Menu1< SMList<Map> > M( Menu::TOOLS );
    
    M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE );
    
    M.add( Ctor1<MapListExtractIAauto, SMList<Map> >(),
	   Text("Extract IA-automorphisms from ") + Name(1)
	   );
    
   
    M.add( Ctor1<MapListExtractInner, SMList<Map> >(),
	   Text("Extract inner automorphisms from ") + Name(1)
	   );

    M.done();
  }

  {
    Menu2< SMList<Word>, SMList<Word>  > M( Menu::TOOLS );
    
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) );
    
    M.add( Ctor2<SMListJoin<Word>, SMList<Word>, SMList<Word> >(),
	   Text("Compute the join of ") + Name(1) + Text(" and ") + Name(2)
	   );	 
    
    M.done();
  }

 {
    Menu2< SMList<Subgroup>, SMList<Subgroup>  > M( Menu::TOOLS );
    
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) );
    
    M.add( Ctor2<SMListJoin<Subgroup>, SMList<Subgroup>, SMList<Subgroup> >(),
	   Text("Compute the join of ") + Name(1) + Text(" and ") + Name(2)
	   );	 

    M.done();
  }

  {
    Menu2< SMList<Map>, SMList<Map>  > M( Menu::TOOLS );
    
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) );
    
    M.add( Ctor2<SMListJoin<Map>, SMList<Map>, SMList<Map> >(),
	   Text("Compute the join of ") + Name(1) + Text(" and ") + Name(2)
	   );	 
    
    M.done();
  }
 
 {
   Menu2<SMList<Word>,SMWord> M( Menu::TOOLS );
    
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) );
    
    M.add( Ctor2<IsWordInList, SMList<Word>,SMWord>(),
	   Text("Determine whether") + Name(2)
	   + Text("lies in") + Name(1)
	   );	 
    
    M.done();
  }
 {
    Menu2<SMList<Subgroup>,SMSubgroup> M( Menu::TOOLS );
    
    
    M.startItemGroup( Parent( 1 ) == Parent( 2 ) );
    
    M.add( Ctor2<IsSubgroupInList, SMList<Subgroup>,SMSubgroup>(),
	   Text("Determine whether") + Name(2)
	   + Text("lies in") + Name(1)
	   );	 
    
    M.done();
  }
 {
    Menu2<SMList<Map>,SMMap> M( Menu::TOOLS );
    
    
    M.startItemGroup(  Domain(2) == Parent( 1 ) );
    
    M.add( Ctor2<IsMapInList, SMList<Map>,SMMap>(),
	   Text("Determine whether") + Name(2)
	   + Text("lies in") + Name(1)
	   );	 
    
    M.done();
  }
 {
    Menu2<SMList<Map>,SMHomomorphism> M( Menu::TOOLS );
    
    
     M.startItemGroup(  Domain(2) == Parent( 1 ) );
    
    M.add( Ctor2<IsMapInList, SMList<Map>,SMHomomorphism>(),
	   Text("Determine whether") + Name(2)
	   + Text("lies in") + Name(1)
	   );	 
    
    M.done();
  }

}


// ----------------- ObjectSmith::outputMakeMenuDefns ---------------------- //


void ObjectSmith::outputMakeMenuDefns(ostream& ostr)
{
  /* @am 
  {
    Menu0 M( Menu::MAKE );
    
    
    M.startItemGroup();
    
    M.add( Ctor0<MakeRandomPresentation>( ),
	   Text("Random presentation")
	   );
    
    M.done();
  }
  */

 {
    Menu1<SMFPGroup> M( Menu::MAKE );
    


    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::ABELIAN );
    
    M.add( Ctor1<MakeCyclicDecomposition, SMFPGroup>( ),
	   Text("Canonical presentation of") + Name(1)
	   );


    
    M.startItemGroup();
    
    M.add( ReadMakeQuotientItem(),
	   Text("Quotient of") + Name(1),
	   DefineSetOfRelators()
	   );



    M.startItemGroup( CheckinType( 1 ) != SMFPGroup::ABELIAN );

    M.add( Ctor1<MakeAbelianQuotient, SMFPGroup>(),
	   Text("Abelian Quotient of") + Name(1)
	   );



    M.startItemGroup( CheckinType( 1 ) != SMFPGroup::ABELIAN
		      );
    
    M.add( ReadMakeNilpotentQuotientItem(),
	   Text("Nilpotent Quotient of") + Name(1),
	   DefineInt("Enter the nilpotency class: ", 1)
	   );


    
    M.startItemGroup();

    M.separator();

    /*@db
    M.addDisabled( Text("Another Presentation for") + Name(1)
		   );
    */

    M.add( Ctor1<TTProblem, SMFPGroup>(),
	   Text("Another Presentation of") + Name(1)
	   );

    M.add( Ctor1<MakeRipsConstruction, SMFPGroup>(),
	   Text("Rips construction of") + Name(1)
	   );
    
    
    M.done();
 }
  
 {
    Menu1<SMSubgroup> M( Menu::MAKE );

    M.startItemGroup();

    M.add( Ctor1<MakeQuotientFromSubgroup, SMSubgroup>(),
	   Text("Form the quotient group") + Name(Parent(1))+Text("/")+Name(1)
	   );
    
    M.done();
 }
 
 {
   Menu2<SMSubgroup, SMWord> M( Menu::MAKE );
   
   M.startItemGroup();
   
   M.add( Ctor2<SubgroupConjugateBy,SMSubgroup,SMWord>(),
	  Text(Name(1)) + Text("conjugated by") + Name(2)
	  );
   M.done();
 }

 {
   Menu2<SMSubgroup, SMSubgroup> M( Menu::MAKE );
   
   M.startItemGroup( CheckinType( Parent(1) ) == SMFPGroup::FREE &&
		     Parent( 1 ) == Parent( 2 )
		     ); 
   
   M.add( Ctor2<SGOfFreeJoin,SMSubgroup,SMSubgroup>(),
			      Text("The join of") + Name(1) + "and" + Name(2)
			      );
   
   M.add( Ctor2<SGOfFreeIntersection,SMSubgroup,SMSubgroup>(),
	  Text("The intersection of") + Name(1) + "and" + Name(2)
	  );
   
   M.startItemGroup( CheckinType( Parent(1) ) != SMFPGroup::FREE &&
		     Parent( 1 ) == Parent( 2 )
		     );
   
   M.add( Ctor2<SubgroupJoin,SMSubgroup,SMSubgroup>(),
	  Text("The join of") + Name(1) + "and" + Name(2)
	  ); 
   
   M.done();
 }
 {
   Menu4<SMFPGroup, SMFPGroup, SMSubgroup, SMSubgroup> M( Menu::MAKE );
   
   
   
   M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FREE &&
		     CheckinType( 2 ) == SMFPGroup::FREE &&
		     ( Parent(3) == Oid(1) || Parent(3) == Oid(2) ) &&
		     ( Parent(4) == Oid(1) || Parent(4) == Oid(2) ) &&
		     Parent(3) != Parent(4)
		     );

   
    M.add( Ctor4<MakeAPOfFree, SMFPGroup, SMFPGroup,
	   SMSubgroup, SMSubgroup> (),
	   Text("Amalgamated Product")
	   );

    
    
    M.done();
  }

  { 
    Menu3<SMFPGroup, SMSubgroup, SMSubgroup> M( Menu::MAKE );

    M.startItemGroup( CheckinType( 1 ) == SMFPGroup::FREE
		      && Parent(2) == Oid(1) && Parent(3) == Oid(1) 
		      );

    M.add( Ctor3<MakeHNNExtOfFreeGroup,SMFPGroup,SMSubgroup,SMSubgroup>(), 
	   Text("HNN-extension")
	   );

    M.done();
  }

  {
    Menu2<SMFPGroup, SMFPGroup> M( Menu::MAKE );
    
    M.startItemGroup( CheckinType( 1 ) != SMFPGroup::NILPOTENT &&
		      CheckinType( 2 ) != SMFPGroup::NILPOTENT &&
		      CheckinType( 1 ) != SMFPGroup::FREE_NILPOTENT &&
		      CheckinType( 2 ) != SMFPGroup::FREE_NILPOTENT
		      );

    M.add( Ctor2<MakeFreeProduct,SMFPGroup,SMFPGroup>(), 
	   Text("Make free product ")+Name(1)+Text("*")+Name(2)
	   );
    
    M.add( Ctor2<MakeDirectProduct,SMFPGroup,SMFPGroup>(), 
	   Text("Make direct product")+Name(1)+Text("X")+Name(2)
	   );
    
    M.done();
    
  }

  {
    Menu1< EnumeratorProblem< Word > > M( Menu::MAKE );
    
    M.startItemGroup( );
    //@njz
    //    ostrstream o;
    std::ostrstream o;
    //
    o << "Make list of words from \\\"" 
      << Name(1) 
      << "\\\"" << ends;
  
    M.add( Ctor1<MakeSMListOf<Word>,EnumeratorProblem< Word > >(), 
	   Text( o.str() )
	   );
    
    M.done();
    
  }
  {
    Menu1< EnumeratorProblem< Subgroup > > M( Menu::MAKE );
    
    M.startItemGroup( );
    
    //@njz
    //    ostrstream o;
    std::ostrstream o;
    //
    o << "Make list of subgroups from \\\"" 
      << Name(1) 
      << "\\\"" << ends;
    M.add( Ctor1<MakeSMListOf<Subgroup>,EnumeratorProblem< Subgroup > >(), 
	   Text(o.str())
	   );
    
    M.done();
    
  }
  {
    Menu1< EnumeratorProblem< Map > > M( Menu::MAKE );

    M.startItemGroup( );
    
    //@njz
    //    ostrstream o;
    std::ostrstream o;
    //
    o << "Make list of maps from \\\"" 
      << Name(1) 
      << "\\\"" << ends;
    M.add( Ctor1<MakeSMListOf<Map>,EnumeratorProblem< Map > >(), 
	   Text(o.str())
	   );
    
    M.done();
    
  }
  /* @db buggy !
  {
    Menu2<SMFPGroup, SMSubgroup> M( Menu::MAKE );
    
    M.startItemGroup( ( CheckinType( 1 ) == SMFPGroup::FREE
			|| CheckinType( 1 ) == SMFPGroup::FP
			|| CheckinType( 1 ) == SMFPGroup::ABELIAN 
			|| CheckinType( 1 ) == SMFPGroup::AP_FREE
			|| CheckinType( 1 ) == SMFPGroup::AP_FREE_CYCLIC
			|| CheckinType( 1 ) == SMFPGroup::SMALL_CANCELLATION
			|| CheckinType( 1 ) == SMFPGroup::ONE_RELATOR 
			|| CheckinType( 1 ) == SMFPGroup::ONE_RELATOR_WITH_TORSION
			|| CheckinType( 1 ) == SMFPGroup::HNN_FREE
		      )
		      && Parent(2) == Oid(1) );

    M.add( Ctor2<MakeFactorGroup,SMFPGroup,SMSubgroup>(), 
	   Text("Make a factor group ")+Name(1)+Text("/")+Name(2)
	   );

    M.done();
    
  }
  */
}


// --------------- ObjectSmith::outputTestingMenuDefns -------------------- //


void ObjectSmith::outputTestingMenuDefns(ostream& ostr)
{
  {
    Menu0 M( Menu::TESTING );

    M.startItemGroup( );
    
    M.addDisabled( Text("Nothing") );

    M.done();
  }
}

