// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMWord
//
// Principal Author: Roger Needham, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#include "SMWord.h"

// ---------------------- WIC ----------------------------------------------//
WIC::WIC(OID word_oid) :
  wordOID( word_oid )
  //,
  //  ispower( dontknow ),
  //  is_trivial( dontknow ),
  //  collected( NULL ),
  //  order( -1 )
{
}

WIC::~WIC( ) {
  //  if(collected != NULL) delete collected;
}

//============================================================
//
// Property: IsTrivial
//
//============================================================

Trichotomy WIC::isTrivial() const 
{
  return haveProperty( IsTrivialProperty::type() );
}

// Yes if word is trivial, no if it is not.
Trichotomy WIC::isTrivial(Chars& expl)
{
  Trichotomy havep = haveProperty( IsTrivialProperty::type() );

  if( havep == yes ) 
    setKnownProperty( IsTrivialProperty(expl) );
  else if( havep == no ) 
    setHaveNotProperty( IsTrivialProperty::type(), expl ); //???

  return havep;
  // Old code:
  //  expl = explOfIsTrivial;
  //  return is_trivial;
}

// Yes if word is trivial, no if it is not.
void WIC::putIsTrivial(bool istrivial, const Chars& expl)
{
  if( istrivial )
    setKnownProperty( IsTrivialProperty(expl) );
  else 
    setHaveNotProperty( IsTrivialProperty::type(), expl );  

  if( istrivial )
    setKnownProperty( WordOrderProperty(1) );

  //  is_trivial = istrivial;
  //  explOfIsTrivial = expl;
  //  if (is_trivial)
  //    order = 1;
}

//============================================================
//
// Property: Have Order 
//
//============================================================

bool WIC::haveOrder( ) const 
{ 
  return haveProperty( WordOrderProperty::type() ) == yes;
}

void WIC::putHaveOrder (const Integer& o) 
{
  setKnownProperty( WordOrderProperty(o) );
  if( o == 1 ) {
    setKnownProperty( IsTrivialProperty(" because it's order is 1") );
  }
  else {
    setHaveNotProperty( IsTrivialProperty::type(),
			( o > 0
			  ? " because it's order is bigger, than 1"
			  : " because it's order is infinite"
			)
		      );

  }
}

Integer WIC::getOrder( ) const 
{
  if( !haveOrder() ) 
    return -1;

  return ((const WordOrderProperty *)
	  getKnownProperty( WordOrderProperty::type() ) ) -> value();
}

//============================================================
//
// Property: Maximal Root
//
//============================================================

// True if maximal root problem, was solved
Trichotomy WIC::isPower() const
{
  return haveProperty( MaximalRootProperty::type() );
}

// True if word has the maximal root
bool WIC::haveMaximalRoot() const
{ 
  return haveProperty( MaximalRootProperty::type() ) == yes;
}

// Puts the solution of maximal root problem
void WIC::putHaveMaximalRoot(const Word& root, const Integer& power)
{
  if( power == 0 ) {
    setHaveNotProperty( MaximalRootProperty::type(), "" );
  }
  else {
    setKnownProperty( MaximalRootProperty (root)  );
    setKnownProperty( MaximalPowerProperty(power) );
  }
}


void WIC::getMaximalRoot(Word& root, Integer& power) const
{
#if SAFETY > 0
  {
    Trichotomy havep = haveProperty(MaximalRootProperty::type());
    if( havep == dontknow ) {
      error( "void getMaximalRoot(Word&, Integer&) const : "
	     "maximal root was not computed yet." );
    }
    if( havep == no ) {
      error( "void getMaximalRoot(Word&, Integer&) const : "
	     "Word is not a power." );
    }
  }
#endif

  root = ((const MaximalRootProperty*)
	  getKnownProperty( MaximalRootProperty::type() ) ) -> value();

  power = ((const MaximalPowerProperty*)
	   getKnownProperty( MaximalPowerProperty::type() ) ) -> value();
}

Integer WIC::getMaximalExponent() const 
{ 
  return ((const MaximalPowerProperty *)
	  getKnownProperty( MaximalPowerProperty::type())) -> value();
}

//============================================================
//
// Property: Collected Form
//
//============================================================


// True if have cannonical decomposition in nilpotent
bool WIC::haveCollectedForm() const 
{
  return haveProperty( CollectedFormProperty::type() ) == yes;
  //return collected != NULL; 
}

void WIC::putHaveCollectedForm( const PolyWord& p )
{
  if( haveCollectedForm() )
    error("WIC::putHaveCollectedForm(): collected form already computed");

  setKnownProperty( CollectedFormProperty(p) );
}

const PolyWord& WIC::getCollectedForm() const
{
  if( !haveCollectedForm() )
    error("WIC::getCollectedForm(): collected form not computed");

  return ((const CollectedFormProperty *)
	  getKnownProperty(CollectedFormProperty::type())) -> value();
}

//---------------------------------------------------------------------------//
//-------------------- Static members of new properties ---------------------//
//---------------------------------------------------------------------------//

const PropertyType IsTrivialProperty::theTypeID = GenericProperty::unique();
const PropertyType WordOrderProperty::theTypeID = GenericProperty::unique();
const PropertyType MaximalRootProperty::theTypeID = GenericProperty::unique();
const PropertyType MaximalPowerProperty::theTypeID = GenericProperty::unique();
const PropertyType CollectedFormProperty::theTypeID=GenericProperty::unique();

//---------------------------------------------------------------------------//
//---------------------------- SMWord ---------------------------------------//
//---------------------------------------------------------------------------//

SMWord::SMWord(SMFPGroup& G, const Word w, const Chars heritage)
  : AlgebraicObject( heritage ),
    wic( oid() ),
    theGroup( G ),
    theWord( w ),
    theWCM( new WCM( *this ) )
{
  addDependent( *theWCM );
}


void SMWord::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theGroup ) );
}


void SMWord::viewStructure(ostream& ostr) const
{
  ostr
	 << "viewBuilder objectView viewParameters {viewID "
	 << oid()
	 << " heritage {"
	 << heritage()
	 << "}}";
}


void SMWord::printDefinition(ostream& ostr) const
{
  theGroup.getFPGroup().printWord( ostr, theWord );
}
// ------------------------------- WCM ------------------------------------- //


WCM::WCM( class SMWord& W )
  : Supervisor( false ),
    ngDecomposeWord(*this),
    theWord( W )
{
  adminStart();
}


void WCM::takeControl( )
{
  if ( ! didFastChecks ) {
    
    didFastChecks = true;
    
    // Cheat and do some quickie checks here without reporting ARC consumption.
  }
}
