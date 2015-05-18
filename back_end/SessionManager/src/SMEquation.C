// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMEquation
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "SMEquation.h"



EIC::EIC( OID equation_oid )
  : equationOID( equation_oid ),
    foundAllBasicSolutions( false ),
    foundAllRegStabGenerators( false ),
    solvedInAbelian( false ),
    haveSolInAbelian( false )
{ }


EIC::~EIC( )
{ }


bool EIC::haveAllBasicSolutions( ) const 
{  
  return haveProperty( AllBasicSolutionsProperty::type() ) == yes; 
}

void EIC::putHaveAllBasicSolutions( bool found ) 
{ 
  putProperty( AllBasicSolutionsProperty(), found );
  // foundAllBasicSolutions = found; 
}

bool EIC::haveAllRegStabGenerators( ) const 
{ 
  return haveProperty( AllRegStabGeneratorsProperty::type() ) == yes; 
  //  return foundAllRegStabGenerators;
}

void EIC::putHaveAllRegStabGenerators( bool found ) 
{ 
  putProperty( AllRegStabGeneratorsProperty(), found );
  //foundAllRegStabGenerators = found; 
}


Trichotomy EIC::haveBasicSolutions() const
{
  return haveProperty( BasicSolutionsProperty::type() );
}

ListOf<Endomorphism> EIC::getBasicSolutions( ) const 
{ 
  if( haveBasicSolutions() != yes )
    return ListOf<Endomorphism>();

  return ( (const BasicSolutionsProperty*)
	   getKnownProperty(BasicSolutionsProperty::type())) -> value(); 
  //  return theBasicSolutions; 
}

void EIC::addBasicSolutions( const ListOf<Endomorphism>& basicSolutions ) 
{ 
  ListOf<Endomorphism> theBasicSolutions = getBasicSolutions();
  ListIterator< ListOf<Endomorphism> > I(basicSolutions);
  for( ; !I.done(); I.next() ) {
    theBasicSolutions.append( I.value() ); 
  }
  setKnownProperty( BasicSolutionsProperty(theBasicSolutions) );
}

Trichotomy EIC::haveRegStabGenerators() const
{
  return haveProperty( RegStabGeneratorsProperty::type() );
}

ListOf<Automorphism> EIC::getRegStabGenerators( ) const 
{
  if( haveRegStabGenerators() != yes )
    return ListOf<Automorphism>();

  return ( (const RegStabGeneratorsProperty*)
	   getKnownProperty(RegStabGeneratorsProperty::type())) -> value(); 
  //return theRegStabGenerators; 
}

void EIC::addRegStabGenerators( const ListOf<Automorphism>& regStabGens ) 
{ 
  ListOf<Automorphism> theRegStabGenerators = getRegStabGenerators();
  ListIterator< ListOf<Automorphism> > I(regStabGens);
  for( ; !I.done(); I.next() ) {
    theRegStabGenerators.append( I.value() ); 
  }
  setKnownProperty( RegStabGeneratorsProperty(theRegStabGenerators) );
}

//---------------------------------------------------------------------------//
//-------------------- Static members of new properties ---------------------//
//---------------------------------------------------------------------------//

const PropertyType AllBasicSolutionsProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType AllRegStabGeneratorsProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType BasicSolutionsProperty::theTypeID = 
  GenericProperty::unique();
const PropertyType RegStabGeneratorsProperty::theTypeID = 
  GenericProperty::unique();

//---------------------------------------------------------------------------//
//-------------------------- SMEquation -------------------------------------//
//---------------------------------------------------------------------------//

SMEquation::SMEquation(SMFPGroup& G, FreeGroup F, const Word w, int nvar,
		       const Chars heritage)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theFreeGroup( F ),
    theWord( w ),
    numOfVar( nvar ),
    eic( oid() )
{ }


void SMEquation::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theGroup ) );
}


void SMEquation::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}


void SMEquation::printDefinition(ostream& ostr) const
{
  theFreeGroup.printWord( ostr, theWord );
  //@rn Note that the output format does not match the input format.
}


//@db temporary class SMEquation2


//---------------------------------------------------------------------------//
//------------------------- SMEquation2 -------------------------------------//
//---------------------------------------------------------------------------//

SMEquation2::SMEquation2(SMFPGroup& G, FreeGroup F, const Word w, int nvar,
		       const Chars heritage)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theFreeGroup( F ),
    theWord( w ),
    numOfVar( nvar ),
    eic( oid() )
{ }


void SMEquation2::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theGroup ) );
}


void SMEquation2::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}


void SMEquation2::printDefinition(ostream& ostr) const
{
  theFreeGroup.printWord( ostr, theWord );
  //@rn Note that the output format does not match the input format.
}
