// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class GIC
//
// Principal Author: Roger Needham, Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
//
// 1/9/96 @db changed
//
// void GIC::putIsOneRelatorWithTorsion(const Word& relator)
// void GIC::putIsOneRelator(const Word& relator)
//
// 10/98 @dp implemented properties.

#include "GIC.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "HNNExtOfFreeGroup.h"
#include "DecomposeInSubgroup.h"

//---------------------------------------------------------------------------//
//------------------------------ GIC ----------------------------------------//
//---------------------------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Constructors:                                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
  
GIC::GIC(OID group_oid) :
  groupOID( group_oid )
{ 
  setKnownProperty( NilpotencyClassProperty(0) );
  setKnownProperty( 
    NilpotentQuotientsProperty(AssociationsOf<int, NilpotentGroup*>())
  );
}


GIC::~GIC( )
{
  AssociationsIterator<int, NilpotentGroup*> I( getNilpotentQuotients() );
  while ( !I.done() ) {
    delete I.value();
    I.next();
  }
}


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Reporting Information:                                              //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

//======================================================================//
//                                                                      //
//  Property: GroupOrder                                                //
//                                                                      //
//======================================================================//

bool GIC::haveOrder( ) const 
{ 
  return haveProperty(GroupOrderProperty::type()) == yes;
}

void GIC::putHaveOrder(Integer order, Chars explanation, bool showExplanation)
{
  if( order == -1 || haveProperty( GroupOrderProperty::type() ) == yes )
    return;

  setKnownProperty( GroupOrderProperty(order,explanation) );

  Chars orderMessage = getOrderMessage();

  if( showExplanation )
    LogMessage(groupOID, orderMessage).send();
  
  if( order == 0 )
    putIsFinite(false, explanation, false);
  else
    putIsFinite(true, explanation, false);

/*@db   
  if ( order == 1 ) {
	 abelian = yes;
	 //@rn Others?
	 //@rn Make these function calls
  }
*/  
  // As primality testing can be expensive, we leave that to, e.g.,
  // the abelian class membership determiner.
}

const Integer& GIC::getOrder( ) const
{ 
  GroupOrderProperty* pr = 
    (GroupOrderProperty*)getKnownProperty( GroupOrderProperty::type() );
  return pr->value(); 
}

const Chars GIC::getOrderExplanation( ) const 
{
  //GroupOrderProperty* pr =  (GroupOrderProperty*)getKnownProperty(  );
  return getDescription( GroupOrderProperty::type() ); 
}

const Chars GIC::getOrderMessage( ) const 
{ 
  Integer order       = getOrder();
  Chars   explanation = getOrderExplanation();

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //

  msg << Name( groupOID );
  if ( order == 0 )
    msg << " is infinite";
  else
    msg << " has order " << order;
  
  if( explanation.length() > 0 )
    msg << ": " << explanation;
  msg << "." << ends;
  
  Chars orderMessage = msg.str();
  
  msg.freeze(0);

  return orderMessage; 
}

/*

//======================================================================//
//                                                                      //
//  Property: GroupOrderUpperBound                                      //
//                                                                      //
//======================================================================//

void GIC::putHaveOrderUpperBound(Integer the_bound, Chars explanation)
{
  Integer orderUpperBound = -1;
  if( haveProperty(GroupOrderUpperBoundProperty::type()) == yes ) {
    orderUpperBound = 
      ( (const GroupOrderUpperBoundProperty*)
	getKnownProperty(GroupOrderUpperBoundProperty::type()) ) -> value();
  }

  if ( the_bound != orderUpperBound ) {     // This can be reported many times
    if ( the_bound == 1 )
      putHaveOrder( 1, explanation );
    else {
      setKnownProperty( GroupOrderUpperBoundProperty(the_bound,explanation) );
      
      LogMessage msg( groupOID );
      
      msg << Name( groupOID ) << " has order at most "
	  << the_bound << ": " << explanation << ".";
      
      msg.send();
    }
  }
}

// const Integer& getOrderUpperBound( ) const {
//   return ( order != -1 ? order : orderUpperBound );
// }
//
// const Integer& getOrderLowerBound( ) const {
//   return ( order != -1 ? order : orderLowerBound );
// }

*/

//======================================================================//
//                                                                      //
//  Property:  SolvedWordProblem                                        //
//                                                                      //
//======================================================================//

bool GIC::haveSolvedWordProblem( ) const 
{ 
  return haveProperty(SolvedWordProblemProperty::type()) == yes; 
}

void GIC::putHaveSolvedWordProblem( Chars explanation, bool bShowExplanation )
{
  setKnownProperty( SolvedWordProblemProperty(explanation) );

  if( bShowExplanation ) {

    LogMessage msg( groupOID );
    
    msg << "Solved the word problem for " << Name( groupOID ) << ": "
	<< explanation << ".";
    
    msg.send();
  }
}

//======================================================================//
//                                                                      //
//  Property: FastWordProblem                                           //
//                                                                      //
//======================================================================//

bool GIC::haveFastWordProblem( ) const 
{ 
  return haveProperty(FastWordProblemProperty::type()) == yes; 
}

void GIC::putHaveFastWordProblem( Chars explanation, bool bShowExplanation )
{
  setKnownProperty( FastWordProblemProperty(explanation) );

  if( bShowExplanation ) {

    LogMessage msg( groupOID );
    
    msg << "Have a fast solution of the word problem for " << Name( groupOID )
	<< ": " << explanation << ".";
    
    msg.send();
  }
}

//======================================================================//
//                                                                      //
//  Property: CompleteCayleyGraph                                       //
//                                                                      //
//======================================================================//

bool GIC::haveCompleteCayleyGraph( ) const 
{ 
  return haveProperty(CompleteCayleyGraphProperty::type()) == yes;
}

void GIC::putHaveCompleteCayleyGraph(SubgroupGraph CG)
{
  setKnownProperty( CompleteCayleyGraphProperty(CG) );
  // Sneaky way of getting a pointer to our own copy of CG.
  
  LogMessage msg( groupOID );

  msg << "Have a complete Cayley graph for " << Name( groupOID ) << ".";
  msg.send();
  
  putHaveOrder( CG.vertexCount(),
		"by counting vertices in the complete Cayley graph"
		);
  putHaveFastWordProblem("have a complete Cayley graph");
}

SubgroupGraph GIC::getCompleteCayleyGraph( ) const 
{
#if SAFETY > 0
  if ( !haveCompleteCayleyGraph() )
    error("called GIC::getCompleteCayleyGraph before graph exists");
#endif
  
  return ( (const CompleteCayleyGraphProperty*)
	   getKnownProperty(CompleteCayleyGraphProperty::type()) ) -> value();
}

//======================================================================//
//                                                                      //
//  Property: ConfluentKBMachine                                        //
//                                                                      //
//======================================================================//

bool GIC::haveConfluentKBMachine( ) const 
{ 
  return haveProperty(ConfluentKBMachineProperty::type()) == yes; 
}

void GIC::putHaveConfluentKBMachine(KBMachine kbm)
{
  setKnownProperty( ConfluentKBMachineProperty(kbm) );
  
  LogMessage msg( groupOID );
  msg << "Have a confluent rewriting system for " << Name( groupOID )
      << ", discovered by the Rutgers Knuth-Bendix Package, Version 1.25.";
  msg.send();

  // Can deduce the order of the group:

  Integer size = kbm.sizeLanguage();
  if ( size == -1 )
    putHaveOrder( 0, "by counting the language of irreducible words" );
  else
    putHaveOrder( size, "by counting the language of irreducible words" );
  // An automaton may accept only the empty language, so
  // FSA::sizeLanguage() uses -1 for infinity.

  putHaveFastWordProblem("the confluent rewriting system");
}

const KBMachine& GIC::getConfluentKBMachine( ) const 
{ 
  return ( (const ConfluentKBMachineProperty*)
	   getKnownProperty(ConfluentKBMachineProperty::type()) ) -> value();
}

//======================================================================//
//                                                                      //
//  Property: IsAutomatic                                               //
//                                                                      //
//======================================================================//

Trichotomy GIC::isAutomatic( ) const 
{ 
  	return haveProperty(IsAutomaticProperty::type());
}

void GIC::putIsAutomatic( Trichotomy bAutomatic ) 
{
  if( haveProperty( IsAutomaticProperty::type() ) != dontknow )
    return;

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //

  if( bAutomatic == yes ) 
    msg << Name( groupOID ) << " is automatic";
  else if( bAutomatic == no )
    msg << "The algorithm fails to prove " << Name(groupOID) << " automatic";
  else
    msg << "The algorithm is unable to chech whether " << Name(groupOID)
	<< " is automatic (probably through lack of memory)";
  
  msg << "." << ends;
  Chars automaticMessage = msg.str();
  msg.freeze(0);

  putProperty( IsAutomaticProperty(automaticMessage), bAutomatic );
  //return automaticMessage;
}

const Chars GIC::getAutomaticMessage( ) const 
{
  return getDescription( IsAutomaticProperty::type() );
  //  return ( (const IsAutomaticProperty*)
  //	   getKnownProperty() ) -> getDescription();
}

//========================================================================//
//                                                                        //
//  Property: Automatic_GroupDFSAProperty, Automatic_DiffMachineProperty  //
//                                                                        //
//========================================================================//

bool GIC::haveAutomatic( ) const 
{ 
  return haveProperty(Automatic_GroupDFSAProperty::type()) == yes;
}

void GIC::putHaveAutomatic(GroupDFSA wa, DiffMachine dm)
{
#if SAFETY > 0
  if( isAutomatic() != yes )
    error("void GIC::putHaveAutomatic( GroupDFSA, DiffMachine ) : "
	  "The group is not automatic.");
#endif
  
  setKnownProperty( Automatic_GroupDFSAProperty(wa) );
  setKnownProperty( Automatic_DiffMachineProperty(dm) );

  /*
  LogMessage msg( groupOID );

  msg << "Have a ShortLex automatic structure for " << Name( groupOID )
		<< ", discovered by KBMAG, Version 1.2.";

  msg.send();
  */

  // Can deduce the order of the group:

  Integer size = wa.sizeLanguage();
  if ( size == -1 )
    putHaveOrder( 0, "by counting the language of the word acceptor" );
  else
    putHaveOrder( size, "by counting the language of the word acceptor" );
  // An automaton may accept only the empty language, so
  // FSA::sizeLanguage() uses -1 for infinity.

  putHaveFastWordProblem("the automatic structure");
}

const GroupDFSA& GIC::getWordAcceptor( ) const 
{
  return ( (const Automatic_GroupDFSAProperty*)
	   getKnownProperty(Automatic_GroupDFSAProperty::type()) ) -> value();
}

const DiffMachine& GIC::getDiffMachine( ) const 
{ 
  return ( (const Automatic_DiffMachineProperty*)
	   getKnownProperty(Automatic_DiffMachineProperty::type())) -> value();
}


//======================================================================//
//                                                                      //
//  Property: OneRelator                                                //
//                                                                      //
//======================================================================//

bool GIC::isOneRelator( ) const 
{
  return haveProperty(OneRelatorProperty::type()) == yes;
}

void GIC::putIsOneRelator(const Word& relator)
{
  setKnownProperty( OneRelatorProperty(relator) );

  if ( haveProperty( OneRelatorWithTorsionProperty::type() ) != yes )
    putHaveSolvedWordProblem("the presentation is one-relator", false);

  LogMessage msg( groupOID );
  msg << Name( groupOID )
      << " is a one-relator group and so it has a solvable word problem.";
  msg.send();
}

Word GIC::getOneRelator( ) const 
{
  return ( (const OneRelatorProperty*)
	   getKnownProperty(OneRelatorProperty::type()) ) -> value();
}

//======================================================================//
//                                                                      //
//  Property: OneRelatorWithTorsion                                     //
//                                                                      //
//======================================================================//

bool GIC::isOneRelatorWithTorsion( ) const 
{
  return haveProperty(OneRelatorWithTorsionProperty::type()) == yes;
}

void GIC::putIsOneRelatorWithTorsion(const Word& relator)
{
  setKnownProperty( OneRelatorWithTorsionProperty() );
  setKnownProperty( OneRelatorProperty(relator) );
  putHaveFastWordProblem("presentation is one relator with torsion", false);

  LogMessage msg( groupOID );
  msg << Name( groupOID )
      << " is a one-relator group with torsion and so it has a solvable"
      << " word and conjugacy problem and is also hyperbolic.";
  msg.send();
}

//======================================================================//
//                                                                      //
//  Property: AbelianGroup                                              //
//                                                                      //
//======================================================================//

bool GIC::haveCyclicDecomposition( ) const 
{ 
  return haveProperty( AbelianPresentationProperty::type()) == yes;
}

bool GIC::havePrimaryDecomposition( ) const 
{
  return haveCyclicDecomposition() &&
    getCyclicDecomposition().havePrimaryDecomposition();
}

void GIC::putHaveCyclicDecomposition(const AbelianGroup& A)
{
  if( haveProperty( AbelianPresentationProperty::type() ) != dontknow )
    return;

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //
  msg << "Found the canonical decomposition of " << Name( groupOID );
  if ( isAbelian() != yes ) msg << " abelianized";
  msg << ": " << A << ". " << ends;
  Chars theCyclicDecompositionMessage = msg.str();
  msg.freeze(0);

  LogMessage(groupOID, theCyclicDecompositionMessage).send();

  setKnownProperty( 
    AbelianPresentationProperty(A,theCyclicDecompositionMessage)
  );

  if ( isAbelian() == yes ) {

    LogMessage msg1( groupOID );
    msg1 << "The canonical decomposition allows one to solve a number"
	  " of algorithmic problems, for example the word problem,"
	" the order problem and so on.";
    msg1.send();

    LogMessage msg2( groupOID );
    msg2 << 
      "We use multiplicative notation when dealing with abelian groups,"
      " denoting the generators of"
      " the infinite cyclic factors in the canonical decomposition"
      " of a finitely generated abelian group, by f1, f2, ... , and those"
      " of the finite cyclic factors, by t1, t2, ... .";
    msg2.send();

    LogMessage msg3( groupOID );
    msg3 << Link( Chars("Click here to see the generators of the canonical "
			"decomposition of ") + Text( Name(groupOID) ),
		  "CyclicDecomposition",
		  A.getFileName() );
    msg3.send();
  }
   
  // We may be able to deduce the order.
  // We certainly can if the group itself is abelian:
  
  if ( isAbelian() == yes ) {
    
    /*	   putHaveOrder( theAbelianGroup->order(),
	   Text("this follows from the cyclic decomposition of")
	   + Name( groupOID)
	   );
    */
    putHaveFastWordProblem("from canonical decomposition", false);
    
  } else if ( A.isInfinite() ) {
    //		putHaveOrder(0, "from torsion free rank of abelianization");
  }
}

void GIC::putHavePrimaryDecomposition(const AbelianGroup& abGroup)
{
  if( haveCyclicDecomposition() ) {
    ( (AbelianPresentationProperty*)
      getKnownProperty(AbelianPresentationProperty::type()))->ref() = abGroup;
  }
#if SAFETY > 0
  else
    error("void putHavePrimaryDecomposition(const AbelianGroup& abGroup) : "
	  "Tried to put primary decomposition into uninitialized object.");
#endif
}

void GIC::putHaveCanonicalSmithPresentation( const AbelianGroup& A )
{
  if( haveProperty( AbelianPresentationProperty::type() ) == dontknow ) {
    
    //@njz
    //  ostrstream msg;
    std::ostrstream msg;
    //
    msg << "Found the canonical decomposition of " << Name( groupOID );
    msg << ": " << A << ". " << ends;

    setKnownProperty( AbelianPresentationProperty(A, msg.str()) );
    putHaveFastWordProblem("from canonical decomposition", false);

    msg.freeze(0);
  }
}

const AbelianGroup& GIC::getCyclicDecomposition( ) const 
{
#if SAFETY > 0
  if ( !haveCyclicDecomposition() )
    error("called GIC::getCyclicDecomposition before "
	  "the cyclic decomposition exists");
#endif
  
  return ( (const AbelianPresentationProperty*)
	   getKnownProperty(AbelianPresentationProperty::type()) ) -> value();
}

const Chars GIC::getCyclicDecompositionMessage( ) const 
{ 
  return getDescription( AbelianPresentationProperty::type() );
  //return ( 
  //	   (const AbelianPresentationProperty*)
  //	   getKnownProperty(AbelianPresentationProperty::type())
  //	 ) -> getDescription();
}

//======================================================================//
//                                                                      //
//  Property: IsAbelian                                                 //
//                                                                      //
//======================================================================//

Trichotomy GIC::isAbelian( ) const 
{ 
  return haveProperty( IsAbelianProperty::type() );
}

void GIC::putIsAbelian( ) 
{ 
  setKnownProperty( IsAbelianProperty() );

  //@db 2.95.2 workaround
  CheckinType ct( groupOID );
  FEDataUpdate( ct, SMFPGroup::ABELIAN ).send();
  //FEDataUpdate( CheckinType( groupOID ), SMFPGroup::ABELIAN ).send();
}

//======================================================================//
//                                                                      //
//  Property: IsFree                                                    //
//                                                                      //
//======================================================================//

Trichotomy GIC::isFree( ) const 
{
  return haveProperty(IsFreeProperty::type());
}

void GIC::putIsFree( bool IsFree, Chars explanation ) 
{
  putProperty( IsFreeProperty(explanation), IsFree );
}

//======================================================================//
//                                                                      //
//  Property: IsFinite                                                  //
//                                                                      //
//======================================================================//

Trichotomy GIC::isFinite( ) const 
{ 
  return haveProperty(IsFiniteProperty::type());
}

const Chars GIC::getFiniteMessage( ) const
{
  Chars explanation = getFiniteExplanation();

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //

  msg << Name(groupOID) << " is " << (isFinite()? "finite":"infinite") << ": ";
  if( explanation.length() > 0 )
    msg << explanation;
  msg << "." << ends;

  Chars finiteMessage = msg.str();

  msg.freeze(0);

  return finiteMessage;
}


void GIC::putIsFinite(bool IsFinite, Chars explanation, bool showExplanation )
{
  if( isFinite() != dontknow ) return;

  putProperty( IsFiniteProperty(explanation), IsFinite );
  
  if( !IsFinite )
    putHaveOrder(0, explanation, false);

  if( showExplanation ) {
    LogMessage(groupOID, getFiniteMessage()).send();
  }
}

const Chars GIC::getFiniteExplanation( ) const 
{
  return getDescription( IsFiniteProperty::type() );
}

//const Chars& GIC::getFiniteMessage( ) const
//{
//  return finiteMessage; 
//}

//======================================================================//
//                                                                      //
//  Property: NilpotencyClass                                           //
//                                                                      //
//======================================================================//

Trichotomy GIC::isNilpotent( ) const
{ 
  if( getNilpotentcyClass() == 0 ) return dontknow;
  return haveProperty( NilpotencyClassProperty::type() ) ;
}

void GIC::putIsNilpotent(int theClass) 
{
  setKnownProperty( NilpotencyClassProperty(theClass) );

  //@db 2.95.2 workaround
  CheckinType ct( groupOID );
  FEDataUpdate( ct, SMFPGroup::NILPOTENT ).send();
  //FEDataUpdate( CheckinType( groupOID ), SMFPGroup::NILPOTENT ).send();
}

int GIC::getNilpotentcyClass( ) const 
{
  return ( (const NilpotencyClassProperty *)
	   getKnownProperty( NilpotencyClassProperty::type()) ) -> value(); 
}

//======================================================================//
//                                                                      //
//  Property: IsFreeNilpotent                                           //
//                                                                      //
//======================================================================//

Trichotomy GIC::isFreeNilpotent( ) const 
{
  return haveProperty( IsFreeNilpotentProperty::type() );
}

void GIC::putIsFreeNilpotent(const FreeGroup& F, int theClass)
{
  // putIsNilpotent( theClass );
  setKnownProperty( NilpotencyClassProperty(theClass) );
  setKnownProperty( IsFreeNilpotentProperty() );
  NilpotentGroup fng(F.namesOfGenerators(), theClass);
  putHaveNilpotentQuotInited(fng, theClass);
}

//======================================================================//
//                                                                      //
//  Property: ActualNilpotencyClass                                     //
//                                                                      //
//======================================================================//

bool GIC::haveActualNilpotencyClass( ) const 
{ 
  return haveProperty( ActualNilpotencyClassProperty::type() ) == yes;
}

void GIC::putActualNilpotencyClass(int c ) 
{ 
#if SAFETY > 0
  if ( c < 0 )
    error("called GIC::putActualNilpotentcyClass() with argument less then 1");
#endif
  setKnownProperty( ActualNilpotencyClassProperty(c) );
}

int GIC::getActualNilpotencyClass( ) const 
{ 
#if SAFETY > 0
  if ( !haveActualNilpotencyClass() )
    error("called GIC::getActualNilpotentcyClass() before class was computed");
#endif

  return ( (const ActualNilpotencyClassProperty*)
	   getKnownProperty(ActualNilpotencyClassProperty::type()))->value(); 
}

//======================================================================//
//                                                                      //
//  Property: NilpotentQuotients                                        //
//                                                                      //
//======================================================================//

// @dp -- TO BE REWRITTEN.

bool GIC::haveNilpotentQuotInited(int theClass) const
{
  return getNilpotentQuotients().bound(theClass);
}

bool GIC::haveNilpotentGroupInited() const 
{
  return haveNilpotentQuotInited( getNilpotentcyClass() );
}


void GIC::putHaveNilpotentQuotInited(
  const NilpotentGroup& theQuot, int theClass )
{
  //@am error if group was not initialized
#if SAFETY > 0
  if( !theQuot.isInitialized() )
    error("void GIC::putHaveNilpotentQuotInited(const NilpotentGroup& theQuot,"
	  "int theClass) : Tried to put not initialized nilpotent quotient.");
#endif  

  //@rn Avoid redundance:
  AssociationsOf<int,NilpotentGroup*>& nilpQuot = 
    (AssociationsOf<int,NilpotentGroup*> &) getNilpotentQuotients();

  if ( ! nilpQuot.bound(theClass) ) {
    //@am to build exact objects
    nilpQuot.bind( theClass, new NilpotentGroup(theQuot));
  }
}

void GIC::putHaveNilpotentGroupInited(const NilpotentGroup& theGroup,
				      Chars filename)
{
  if( BCfileName.length() == 0 )
    BCfileName = filename;

  if( haveNilpotentQuotInited( getNilpotentcyClass() ) )  
    return;

  putHaveNilpotentQuotInited( theGroup, getNilpotentcyClass() );

  putActualNilpotencyClass( theGroup.minimalNilpotencyClass() );
}

AssociationsOf<int,NilpotentGroup*>& GIC::getNilpotentQuotients() const
{
  return ((NilpotentQuotientsProperty*)
	  getKnownProperty(NilpotentQuotientsProperty::type()) ) -> ref();
}
  
const NilpotentGroup& GIC::getNilpotentQuotInited(int theClass) const
{
  return *getNilpotentQuotients().valueOf( theClass );
}

const NilpotentGroup& GIC::getNilpotentGroupInited( ) const
{
  return getNilpotentQuotInited( getNilpotentcyClass() );
}

//======================================================================//
//                                                                      //
//  Property: IsFreeByCyclic                                            //
//                                                                      //
//======================================================================//

bool GIC::isFreeByCyclic( ) const 
{
  return haveProperty( IsFreeByCyclicProperty::type() ) == yes;
}

void GIC::putIsFreeByCyclic(const FreeGroup& F, const Map& aut)
{
  if( haveProperty(IsFreeByCyclicProperty::type()) == dontknow ) {

    setKnownProperty( IsFreeByCyclicProperty( FreeByCyclic(F,aut) ) );

    putHaveFastWordProblem("the explicit free-by-cyclic structure");
  }
}

const FreeByCyclic& GIC::getFreeByCyclic( ) const 
{
#if SAFETY > 0
  if ( !isFreeByCyclic() )
    error("called GIC::getFreeByCyclic() when it doesn't exist");
#endif

  return ( (const IsFreeByCyclicProperty*)
	   getKnownProperty(IsFreeByCyclicProperty::type())) -> value();
}

//======================================================================//
//                                                                      //
//  Property: MSC                                                       //
//                                                                      //
//======================================================================//

bool GIC::haveMSC( ) const 
{
  return haveProperty(MSCProperty::type()) == yes;
}

void GIC::putHaveMSC(FPGroup G, int lambda)
{
  if( haveProperty(MSCProperty::type()) == dontknow ) {
    MSCGroup MSC(G,lambda);
    setKnownProperty( MSCProperty(G,lambda) );
	 
    //@rn Report that we have small canc?
    
    putHaveFastWordProblem("by Dehn's algorithm");
    //@rn putHaveSolvedConjugacyProblem
    
    putHaveOrder( MSC.order(), "by a theorem of Greendlinger" );
    
    SMFPGroup* pGroup = (SMFPGroup*)(TheObjects::get(groupOID));

    if( pGroup->getCheckinType() == SMFPGroup::FP ) {
      CheckinType CT( groupOID );
      FEDataUpdate( CT, SMFPGroup::SMALL_CANCELLATION ).send();
    }
  }
}

const MSCGroup& GIC::getMSC( ) const 
{
#if SAFETY > 0
  if ( !haveMSC() )
    error("called GIC::getMSC() before MSC presentation exists");
#endif

  const MSCGroupPtr& ptr =   
    ((const MSCProperty*)getKnownProperty(MSCProperty::type())) -> value();
  return ptr.getMSCG();
  
}

//======================================================================//
//                                                                      //
//  Property: MSCLambda                                                 //
//                                                                      //
//======================================================================//

bool GIC::haveMSCLambda( ) const 
{ 
  return haveProperty(MSCLambdaProperty::type()) == yes;
}

void GIC::putHaveMSCLambda( int lambda )
{
  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //

  if( lambda < 6 && lambda != 0 ) {

    msg << "The presentation of " << Name( groupOID )
	<< " is not metric small cancellation; it is only C'(1/"
	<< lambda << ')';
    
  } else {
    
    msg << "The presentation of " << Name( groupOID ) << " is C'(";
    if( lambda != 0 ) msg << "1/";
    msg << lambda << ')';
  }

  msg << "." << ends;

  setKnownProperty( MSCLambdaProperty(lambda, msg.str()) );

  msg.freeze(0);
}

int GIC::getMSCLambda( ) const 
{
#if SAFETY > 0
  if( !haveMSCLambda() )
    error(" bool getMSCLambda( ) const : MSC lambda is not computed");
#endif
  
  return ( (const MSCLambdaProperty*)
	   getKnownProperty(MSCLambdaProperty::type())) -> value();
}

//@dp
//Chars GIC::getMSCMessage() const
//{
//  return MSCMessage; 
//}

//======================================================================//
//                                                                      //
//  Property: APOfFree                                                  //
//                                                                      //
//======================================================================//

bool GIC::haveAPOfFree( ) const 
{
  return haveProperty( APofFreeProperty::type() ) == yes;
}

void GIC::putHaveAPOfFree(const AmalgProductOfFreeGroups& AP)
{
  if( !haveAPOfFree() ) 
    setKnownProperty( APofFreeProperty(AP) );
}

const AmalgProductOfFreeGroups& GIC::getAPOfFree( ) const
{
#if SAFETY > 0
  if( !haveAPOfFree() )
    error("called GIC::getAPOfFree() before AP presentation exists");
#endif

  return ( (const APofFreeProperty*)
	   getKnownProperty(APofFreeProperty::type()) ) -> value();
}


//======================================================================//
//                                                                      //
//  Property: HNNofFree                                                 //
//                                                                      //
//======================================================================//

bool GIC::haveHNNofFree( ) const 
{
  return haveProperty(HNNofFreeProperty::type()) == yes;
}

void GIC::putHaveHNNofFree(const HNNExtOfFreeGroup& H) 
{
  if ( !haveHNNofFree() ) 
    setKnownProperty( HNNofFreeProperty(H) );
}

const HNNExtOfFreeGroup& GIC::getHNNofFree( ) const 
{
#if SAFETY > 0
  if( !haveHNNofFree() )
    error("called GIC::getHNNofFree() before HNN presentation exists");
#endif

  return ( (const HNNofFreeProperty*)
	   getKnownProperty(HNNofFreeProperty::type()) ) -> value();
}


//======================================================================//
//                                                                      //
//  Property: SchreierTransversal                                       //
//                                                                      //
//======================================================================//

bool GIC::haveSchreierTransvl() const 
{
  return haveProperty(SchreierTransversalProperty::type()) == yes;
}

void GIC::putHaveSchreierTransvl(const PermutationRepresentation& pr)
{
  if (pr.getRepresentatives().length()==0)
    error("void GIC::putHaveSchreierTransvl(const PermutationRepresentation&):"
	  " wrong transversal.");

  setKnownProperty( SchreierTransversalProperty(pr) );
}

const PermutationRepresentation& GIC::getSchreierTransvl() const 
{ 
  if( !haveSchreierTransvl() )
    error("const PermutationRepresentation& getSchreierTransvl()const :"
	  "transversal has not been computed yet");

  return ( (const SchreierTransversalProperty*)
	   getKnownProperty(SchreierTransversalProperty::type())) -> value();
}

//======================================================================//
//                                                                      //
//  Property: WordDecomposer                                            //
//                                                                      //
//======================================================================//

bool GIC::haveWordDecomposer( ) const 
{
  return haveProperty(WordDecomposerProperty::type()) == yes;
}

void GIC::putHaveWordDecomposer(const DecomposeInSubgroupOfFPGroup& decomposer)
{
  setKnownProperty( WordDecomposerProperty(decomposer) );
}

const DecomposeInSubgroupOfFPGroup& GIC::getWordDecomposer( ) const
{
#if SAFETY > 0
  if( !haveWordDecomposer() )
    error("called  GIC::getWordDecomposer() before a word decomposer exists");
#endif

  return ( (const WordDecomposerProperty*)
	   getKnownProperty( WordDecomposerProperty::type()) ) -> value();
}

//======================================================================//
//                                                                      //
//  U N S O R T E D                                                     //
//                                                                      //
//======================================================================//

/*void GIC::putHaveLCSQuotients(const NilpotentGroup& group)
{
#if SAFETY > 0
  if ( !haveNilpotentQuotInited(nilpotencyClass) )
  error("called void GIC::putHaveLCSQuotients(const NilpotentGroup& group)
            before nilpotent presentation exists");
#endif
  NilpotentGroup* ng = theNilpotentQuotients.valueOf(nilpotencyClass);
  ng->putLCSQuotients(group);
  putActualNilpotencyClass(group.actualClass());
  theNilpotentQuotients.unbind(nilpotencyClass);
  theNilpotentQuotients.bind(nilpotencyClass,ng);
}
*/

/*bool GIC::haveLCSQuotients() const
{
  if (!haveNilpotentQuotInited(nilpotencyClass))
    return false;
  return getNilpotentQuotInited(nilpotencyClass).isLCSComputed();
}
*/
/*
bool GIC::haveActualNilpotencyClass( ) const { 
  return actualNilpotencyClass != -1;
}
int GIC::getActualNilpotencyClass( ) const { 
#if SAFETY > 0
  if ( actualNilpotencyClass == -1 )
    error("called GIC::getActualNilpotentcyClass()
              before class was computed");
#endif
  return actualNilpotencyClass; 
}

const AmalgProductOfFreeGroups& GIC::getAPOfFree( ) const
{
#if SAFETY > 0
  if ( theAPOfFree == NULL )
    error("called GIC::getAPOfFree() before AP presentation exists");
#endif
  return *theAPOfFree;
}


const HNNExtOfFreeGroup& GIC::getHNNofFree( ) const 
{
#if SAFETY > 0
  if ( theHNNofFree == NULL )
    error("called GIC::getHNNofFree() before HNN presentation exists");
#endif
  return *theHNNofFree;
}

void GIC::putIsAbelian( ) 
{ 
  abelian = yes; 
  CheckinType CT( groupOID );
  FEDataUpdate( CT, SMFPGroup::ABELIAN ).send();
}

void GIC::putIsNilpotent(int theClass) 
{
  nilpotent = yes;
  nilpotencyClass = theClass;
  CheckinType CT( groupOID );
  FEDataUpdate(CT, SMFPGroup::NILPOTENT ).send();
}

const DecomposeInSubgroupOfFPGroup& GIC::getWordDecomposer( ) const
{
#if SAFETY > 0
  if( !theWordDecomposer )
    error( "called  GIC::getWordDecomposer() before a word decomposer exists");
#endif
  return *theWordDecomposer;
}
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Reporting Secondary Information:                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////


void GIC::postSecondaryLogMessage(Chars message)
{
  LogMessage msg(groupOID, message, 2);
  msg.send();
}

//---------------------------------------------------------------------------
//------------------ Static data members ------------------------------------
//---------------------------------------------------------------------------

#define UNIQUE() GenericProperty::unique()

const PropertyType GroupOrderProperty           :: theTypeID = UNIQUE();
//const PropertyType GroupOrderUpperBoundProperty:: theTypeID = UNIQUE();
const PropertyType SolvedWordProblemProperty    :: theTypeID = UNIQUE();
const PropertyType FastWordProblemProperty      :: theTypeID = UNIQUE();
const PropertyType CompleteCayleyGraphProperty  :: theTypeID = UNIQUE();
const PropertyType ConfluentKBMachineProperty   :: theTypeID = UNIQUE();
const PropertyType IsAutomaticProperty          :: theTypeID = UNIQUE();
const PropertyType Automatic_GroupDFSAProperty  :: theTypeID = UNIQUE();
const PropertyType Automatic_DiffMachineProperty:: theTypeID = UNIQUE();
const PropertyType OneRelatorWithTorsionProperty:: theTypeID = UNIQUE();
const PropertyType OneRelatorProperty           :: theTypeID = UNIQUE();

const PropertyType AbelianPresentationProperty  :: theTypeID = UNIQUE();
//const PropertyType CyclicDecompositionProperty  :: theTypeID = UNIQUE();
//const PropertyType PrimaryDecompositionProperty :: theTypeID = UNIQUE();
//const PropertyType CanonicalSmithDecompositionProperty::theTypeID = UNIQUE();

const PropertyType IsAbelianProperty            :: theTypeID = UNIQUE();
const PropertyType IsFreeProperty               :: theTypeID = UNIQUE();
const PropertyType IsFiniteProperty             :: theTypeID = UNIQUE();

const PropertyType NilpotencyClassProperty      :: theTypeID = UNIQUE();
const PropertyType NilpotentQuotientsProperty   :: theTypeID = UNIQUE();
const PropertyType IsFreeNilpotentProperty      :: theTypeID = UNIQUE();
const PropertyType ActualNilpotencyClassProperty:: theTypeID = UNIQUE();

const PropertyType IsFreeByCyclicProperty       :: theTypeID = UNIQUE();
//const PropertyType IsFreeByCyclic_MapProperty :: theTypeID = UNIQUE();
const PropertyType MSCProperty                  :: theTypeID = UNIQUE();
const PropertyType MSCLambdaProperty            :: theTypeID = UNIQUE();
const PropertyType APofFreeProperty             :: theTypeID = UNIQUE();
const PropertyType HNNofFreeProperty            :: theTypeID = UNIQUE();
const PropertyType SchreierTransversalProperty  :: theTypeID = UNIQUE();
const PropertyType WordDecomposerProperty       :: theTypeID = UNIQUE();

