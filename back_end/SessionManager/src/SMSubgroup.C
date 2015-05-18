// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of classes SMSubgroup, SIC, SCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "SMSubgroup.h"
#include "DecomposeInSubgroup.h"

// --------------------------- SMSubgroup ---------------------------------- //


SMSubgroup::SMSubgroup(SMFPGroup& G, const SGofFreeGroup& S,
		       const Chars heritage, Chars def)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theSubgroup( S ),
    sic( oid() ),
    definition(def),
    theSCM( new SCM( *this ) )
{
  addDependent( *theSCM );
}


void SMSubgroup::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( *this ), Object( theGroup ) );
}


void SMSubgroup::viewStructure(ostream& ostr) const
{
  ostr
	 << "viewBuilder objectView viewParameters {viewID "
	 << oid()
	 << " heritage {"
	 << heritage()
	 << "}}";
}


void SMSubgroup::printDefinition(ostream& ostr) const
{

   if( definition.length() == 0) {
  //@db
    ostr << "gp ";

    theGroup.getFPGroup().printVectorOfWords( ostr, theSubgroup.generators(),
					      "(",")" );
  }
  else {
    ostr << definition;
  }
}


// ------------------------------ SIC ------------------------------------- //

SIC::SIC(OID group_oid) :
  subgroupOID( group_oid )
{ 
  //  setKnownProperty( SGNilpotencyClassProperty(0) );
  setKnownProperty( 
    SGNilpotentQuotientsProperty(AssociationsOf<int, SGOfNilpotentGroup*>())
  );
}


SIC::~SIC( )
{
  AssociationsIterator<int,SGOfNilpotentGroup*> I(getSGNilpotentQuotients());
  while ( !I.done() ) {
    delete I.value();
    I.next();
  }
}

/*
SIC::SIC(OID subgroup_oid) :
  subgroupOID( subgroup_oid ),
  //  theAbelianGroup( NULL ),
  //  theAbelianSGPresentation( NULL ),
  //  theSGOfNilpotentGroup( NULL ),
  //  isPureSubgroup( dontknow ),
  //  isCentralSubgroup( dontknow ),
  //  isNormalSubgroup( dontknow ),
  //  isAbelianSubgroup( dontknow ),
  //  isTrivialSubgroup( dontknow ),
  //  permutationRepresentation(NULL),
  //  theIndex( -1 ),
  //  theCompleteCayleyGraph( NULL ),
  //  theWordDecomposer( NULL )
{ }


SIC::~SIC( )
{
  //  if (theAbelianGroup)
  //    delete theAbelianGroup;
  //  if (theSGOfNilpotentGroup)
  //    delete theSGOfNilpotentGroup;
  //  if (theAbelianSGPresentation)
  //    delete theAbelianSGPresentation;
  //  if (permutationRepresentation)
  //    delete permutationRepresentation;

  AssociationsIterator<int, SGOfNilpotentGroup*> I( theSGNilpotentQuotients );
  while ( !I.done() ) {
    delete I.value();
    I.next();
  }

  //  delete theCompleteCayleyGraph;
  //  delete theWordDecomposer;
}
*/

//======================================================================//
//                                                                      //
//  Property: CyclicDecompositionOfFactor                               //
//                                                                      //
//======================================================================//

bool SIC::haveCyclicDecompositionOfFactor( ) const 
{ 
  return haveProperty( CyclicDecompositionOfFactorProperty::type()) == yes;
}

void SIC::putHaveCyclicDecompositionOfFactor(const AbelianGroup& A)
{
  if( haveProperty(CyclicDecompositionOfFactorProperty::type()) == dontknow )
    setKnownProperty( CyclicDecompositionOfFactorProperty(A) );
}

const AbelianGroup& SIC::getCyclicDecompositionOfFactor( ) const 
{
#if SAFETY > 0
  if ( !haveCyclicDecompositionOfFactor() )
   error("called SIC::getCyclicDecomposition() before abelianization exists");
#endif
  return ( (const CyclicDecompositionOfFactorProperty*)
	   getKnownProperty(CyclicDecompositionOfFactorProperty::type()) 
	 ) -> value();
  //  return *theAbelianGroup;
}

//======================================================================//
//                                                                      //
//  Property: AbelianSubgroupPresentation                               //
//                                                                      //
//======================================================================//

bool SIC::haveCyclicDecomposition( ) const 
{ 
  return haveProperty( AbelianSubgroupPresentationProperty::type()) == yes;
}

bool SIC::havePrimaryDecomposition( ) const 
{ 
  return haveCyclicDecomposition() &&
    getSGPresentation().havePrimaryDecomposition();
}

void SIC::putHaveCyclicDecomposition(const AbelianSGPresentation& aP)
{
  if( haveProperty( AbelianPresentationProperty::type() ) == dontknow )
    setKnownProperty( AbelianSubgroupPresentationProperty(aP) );
}

void SIC::putHavePrimaryDecomposition(const AbelianSGPresentation& aP)
{
  if( haveCyclicDecomposition() ) {
    ( (AbelianSubgroupPresentationProperty*) 
      getKnownProperty(AbelianSubgroupPresentationProperty::type())
    ) -> ref() = aP;
  }

#if SAFETY > 0
  else
    error("void SIC::putHavePrimaryDecomposition(const AbelianSGPresentation& abGroup) : "
	  "Tried to put primary decomposition into uninitialized object.");
#endif
 
}

const AbelianSGPresentation& SIC::getSGPresentation( ) const 
{
#if SAFETY > 0
  if ( !haveCyclicDecomposition() )
    error("called SIC::getSGPresentation() before presentation exists");
#endif
  return ( (const AbelianSubgroupPresentationProperty*)
	   getKnownProperty(AbelianSubgroupPresentationProperty::type()) 
	 ) -> value();
}

//======================================================================//
//                                                                      //
//  Property: SchreierTransversalProperty                               //
//                                                                      //
//======================================================================//

bool SIC::haveSchreierTransvl() const 
{
  return haveProperty(SchreierTransversalProperty::type()) == yes;
}

void SIC::putHaveSchreierTransvl(const PermutationRepresentation& pr)
{
  if (pr.getRepresentatives().length()==0)
    error("void SIC::putHaveSchreierTransvl(const PermutationRepresentation&):"
	  " wrong transversal.");

  setKnownProperty( SchreierTransversalProperty(pr) );
}

const PermutationRepresentation& SIC::getSchreierTransvl() const 
{ 
  if( !haveSchreierTransvl() )
    error("const PermutationRepresentation& getSchreierTransvl()const :"
	  "transversal has not been computed yet");

  return ( (const SchreierTransversalProperty*)
	   getKnownProperty(SchreierTransversalProperty::type())) -> value();
}

//======================================================================//
//                                                                      //
//  Property: IsPureSubgroupProperty                                    //
//                                                                      //
//======================================================================//

Trichotomy SIC::isPure() const 
{ 
  return haveProperty( IsPureSubgroupProperty::type() );
}

void SIC::putIsPure( bool ispure) 
{ 
  putProperty( IsPureSubgroupProperty(), ispure );
}

//======================================================================//
//                                                                      //
//  Property: IsCentralSubgroupProperty                                 //
//                                                                      //
//======================================================================//

Trichotomy SIC::isCentral() const
{ 
  return haveProperty( IsCentralSubgroupProperty::type() );
}

void SIC::putIsCentral( bool iscentral)
{ 
  putProperty( IsCentralSubgroupProperty(), iscentral );
}

//======================================================================//
//                                                                      //
//  Property: IsNormalSubgroupProperty                                  //
//                                                                      //
//======================================================================//

Trichotomy SIC::isNormal() const
{ 
  return haveProperty( IsNormalSubgroupProperty::type() );
}

void SIC::putIsNormal(bool isnormal)
{ 
  putProperty( IsNormalSubgroupProperty(), isnormal );
}

//======================================================================//
//                                                                      //
//  Property: IsAbelianSubgroupProperty                                 //
//                                                                      //
//======================================================================//

Trichotomy SIC::isAbelian() const 
{ 
  return haveProperty( IsAbelianSubgroupProperty::type() );
}

void SIC::putIsAbelian(bool isabelian) 
{ 
  putProperty( IsAbelianSubgroupProperty(), isabelian );
}

//======================================================================//
//                                                                      //
//  Property: IsTrivialSubgroupProperty                                 //
//                                                                      //
//======================================================================//

Trichotomy SIC::isTrivial() const
{ 
  return haveProperty( IsTrivialSubgroupProperty::type() );
}

void SIC::putIsTrivial(bool istrivial)
{ 
  putProperty( IsTrivialSubgroupProperty(), istrivial );
}

//======================================================================//
//                                                                      //
//  Property: IndexOfSubgroupProperty                                   //
//                                                                      //
//======================================================================//

bool SIC::haveIndex() const 
{
  return haveProperty( IndexOfSubgroupProperty::type() ) == yes;
}

void SIC::putIndex(const int& index) 
{ 
  setKnownProperty( IndexOfSubgroupProperty(index) );
}

int SIC::index() const 
{
  if( !haveIndex() ) {
    // error("Called SIC::index() before the index is computed.");
    return -1;
  }

  return ( (const IndexOfSubgroupProperty*)
	   getKnownProperty(IndexOfSubgroupProperty::type())) -> value();
}


// Methods for nilpotent subgroups

//======================================================================//
//                                                                      //
//  Property: SubgroupOfNilpotentGroupProperty                          //
//                                                                      //
//======================================================================//

bool SIC::haveSGOfNGPreimageInitialized( ) const 
{
  return haveProperty( SubgroupOfNilpotentGroupProperty::type() ) == yes && 
    getSGOfNilpotentGroup().preimageIsInitialized(); 
}

bool SIC::haveSGOfNGinitialized( ) const 
{ 
  return haveProperty( SubgroupOfNilpotentGroupProperty::type() ) == yes && 
    getSGOfNilpotentGroup().basisIsInitialized(); 
  //  if (theSGOfNilpotentGroup == NULL) return false;
  //  return theSGOfNilpotentGroup->basisIsInitialized(); 
}

void SIC::putHaveSGOfNGPreimageInitialized( const SGOfNilpotentGroup& SGN)
{
  if( !haveSGOfNGPreimageInitialized() ){
#if SAFETY > 0
    if ( !SGN.preimageIsInitialized() )
      error("called void SIC::putHaveSGOfNGPrimageInitialized "
	    "( const SGOfNilpotentGroup& SGN):before basis of preimage computed");
#endif
    setKnownProperty( SubgroupOfNilpotentGroupProperty(SGN) );
    //if (theSGOfNilpotentGroup) delete theSGOfNilpotentGroup;
    //theSGOfNilpotentGroup = new SGOfNilpotentGroup( SGN );
  }
}

void SIC::putHaveSGOfNGinitialized( const SGOfNilpotentGroup& SGN)
{
  if( !haveSGOfNGinitialized() ){
#if SAFETY > 0
    if ( !SGN.basisIsInitialized() )
      error("called void SIC::putHaveSGOfNGinitialized "
	    "( const SGOfNilpotentGroup& SGN):before basis computed");
#endif
    setKnownProperty( SubgroupOfNilpotentGroupProperty(SGN) );
    //    if (theSGOfNilpotentGroup) delete theSGOfNilpotentGroup;
    //    theSGOfNilpotentGroup = new SGOfNilpotentGroup( SGN );
  }
}

const SGOfNilpotentGroup& SIC::getSGOfNilpotentGroup( ) const
{
#if SAFETY > 0
  if ( haveProperty(SubgroupOfNilpotentGroupProperty::type()) != yes )
    error("called const SGOfNilpotentGroup& sic::getSGOfNilpotentGroup( )" 
	  " const before basis exists");
#endif  
  return ( (const SubgroupOfNilpotentGroupProperty*)
	   getKnownProperty( SubgroupOfNilpotentGroupProperty::type() )
	 ) -> value();
  //  return *theSGOfNilpotentGroup;  
}


//======================================================================//
//                                                                      //
//  Property: SGNilpotentQuotientsProperty                              //
//                                                                      //
//======================================================================//

bool SIC::haveSGNilpotentQuotInited(int theClass) const
{
  return getSGNilpotentQuotients().bound(theClass);
}

void SIC::putHaveSGNilpotentQuotInited(
  const SGOfNilpotentGroup& theQuot, int theClass)

{
  //@dp error if group was not initialized
#if SAFETY > 0
  if( !theQuot.preimageIsInitialized() )
    error("putHaveSGNilpotentQuotInited(const SGOfNilpotentGroup& theQuot,"
          "int theClass) : Tried to put not initialized nilpotent quotient.");
#endif  

  //@dp Avoid redundance:
  AssociationsOf<int,SGOfNilpotentGroup*>& sgNilpQuot = 
    (AssociationsOf<int,SGOfNilpotentGroup*> &) getSGNilpotentQuotients();

  if ( ! sgNilpQuot.bound(theClass) ) {
    //@dp to build exact objects
    sgNilpQuot.bind( theClass, new SGOfNilpotentGroup(theQuot));
  }
}

AssociationsOf<int,SGOfNilpotentGroup*>& SIC::getSGNilpotentQuotients() const
{
  return ((SGNilpotentQuotientsProperty*)
	  getKnownProperty(SGNilpotentQuotientsProperty::type()) ) -> ref();
}

const SGOfNilpotentGroup& SIC::getSGNilpotentQuotInited(int theClass) const
{
  return *getSGNilpotentQuotients().valueOf( theClass );
}

//======================================================================//
//                                                                      //
//  Property: CompleteCayleyGraphProperty                               //
//                                                                      //
//======================================================================//

bool SIC::haveCompleteCayleyGraph( ) const 
{ 
  return haveProperty(CompleteCayleyGraphProperty::type()) == yes;
}

void SIC::putHaveCompleteCayleyGraph(const SubgroupGraph& S)
{
  setKnownProperty( CompleteCayleyGraphProperty(S) );
}

const SubgroupGraph& SIC::getCompleteCayleyGraph( ) const 
{
#if SAFETY > 0
  if ( !haveCompleteCayleyGraph() )
    error("called SIC::getCompleteCayleyGraph before graph exists");
#endif
  
  return ( (const CompleteCayleyGraphProperty*)
	   getKnownProperty(CompleteCayleyGraphProperty::type()) ) -> value();
}


//======================================================================//
//                                                                      //
//  Property: WordDecomposerProperty                                    //
//                                                                      //
//======================================================================//

bool SIC::haveWordDecomposer( ) const 
{
  return haveProperty(WordDecomposerProperty::type()) == yes;
}

void SIC::putHaveWordDecomposer(const DecomposeInSubgroupOfFPGroup& decomposer)
{
  setKnownProperty( WordDecomposerProperty(decomposer) );
}

const DecomposeInSubgroupOfFPGroup& SIC::getWordDecomposer( ) const
{
#if SAFETY > 0
  if( !haveWordDecomposer() )
    error("called  SIC::getWordDecomposer() before a word decomposer exists");
#endif

  return ( (const WordDecomposerProperty*)
	   getKnownProperty( WordDecomposerProperty::type()) ) -> value();
}


//---------------------------------------------------------------------------//
//-------------------- Static members of new properties ---------------------//
//---------------------------------------------------------------------------//

const PropertyType CyclicDecompositionOfFactorProperty::theTypeID =  
  GenericProperty::unique();
const PropertyType AbelianSubgroupPresentationProperty::theTypeID =
  GenericProperty::unique();
const PropertyType IsPureSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType IsCentralSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType IsNormalSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType IsAbelianSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType IsTrivialSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType IndexOfSubgroupProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType SGNilpotentQuotientsProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType SubgroupOfNilpotentGroupProperty::theTypeID = 
  GenericProperty::unique();

//---------------------------------------------------------------------------//
// ------------------------------- SCM ------------------------------------- //
//---------------------------------------------------------------------------//


SCM::SCM( class SMSubgroup& S )
: Supervisor( false ),
  theSubgroup( S ),
  abelianInvariantsOfFactor( *this ),
  initializeSGOfNG( *this ),
  initPreimageSGOfNG( *this ),
  sgToddCoxeter( *this ),
  sgGHToddCoxeter( *this ),
  abelianSGInvariants( *this ),
  sgNilpotentQuotients( *this ),
  normalClosure( *this )
{
  adminStart();
}


void SCM::takeControl( )
{
  if ( ! didFastChecks ) {
    
    didFastChecks = true;
    
    //Cheat and do some quickie checks here without reporting ARC consumption.
  }
}
