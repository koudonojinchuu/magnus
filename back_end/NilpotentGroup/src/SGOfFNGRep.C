
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SGOfFreeNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//


#include "SGOfFNGRep.h"

#include <stdio.h>
#include "PolyWordIterator.h"
#include "FreeGroup.h"
#include "conversions.h"
#include "File.h"

//=======================================================================
//=============== class SGOfFreeNilpotentGroupRep =======================
//=======================================================================


//---------------------------------------------------------------------
//    Constructors / Initializers:
//---------------------------------------------------------------------

SGOfFreeNilpotentGroupRep::
SGOfFreeNilpotentGroupRep( const NilpotentGroup& parent, 
			   const VectorOf<Word>& gens ) :
  theParentGroup(parent),
  theGenerators(gens),
  theBasisSet()
{ }

// Finding the basis of the subgroup.
void SGOfFreeNilpotentGroupRep::initBasis( ) const {     // logical const !

  if(theBasisSet.isMalcevBasis()) return;

  SGOfFreeNilpotentGroupRep *This = (SGOfFreeNilpotentGroupRep *)this;

  if( theBasisSet.cardinality() == 0 )
    This->theBasisSet = MalcevSet(theGenerators, theParentGroup.collector() );

  This->theBasisSet.makeFull(); 

  This->theBasis = theBasisSet.getPolyWords();

  // name the generators
  // now standard names as h1, h2, h3 ......

  int hirsch = theBasis.length();

  This->theBasisNames = VectorOf<Chars> ( hirsch );

  for(int n = 0; n < hirsch; n++)
    This->theBasisNames[n] = (Chars)"h" + Chars(n+1);

}


//---------------------------------------------------------------------
//  Accessors:
//---------------------------------------------------------------------

// returns the basis of subgroup
const class MalcevSet& SGOfFreeNilpotentGroupRep::preimageBasis() const {
  if(! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::preimageBasis(): basis is not initialized");

  return theBasisSet;
}


// returns the basis of the subgroup in terms of the basis of the parent
// Fast, requires the basis
VectorOf<PolyWord> SGOfFreeNilpotentGroupRep::basis() const {
  if(! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::basis(): basis is not initialized");

  return theBasis;
}


// returns the names of basis words
// Fast, requires the basis
VectorOf<Chars> SGOfFreeNilpotentGroupRep::basisNames() const {
  if(! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::basisNames(): basis is not initialized");
  
  return theBasisNames;
}


// returns the order of subgroup (0 means infinite).
// Fast, requires initialization of the parent group
int SGOfFreeNilpotentGroupRep::order() const {
  if( isTrivial() ) return 1;
  else return 0;
}





//---------------------------------------------------------------------
//  Methods producing related structures
//---------------------------------------------------------------------

//------------------- normalClosure -----------------------------
 

// returns the normal closure of the subgroup

VectorOf<Word> SGOfFreeNilpotentGroupRep::normalClosureBasis() const {

  //@ep I have found that the process is slower with the full basis,
  // so I form it from the scratch.

  MalcevSet newBasis(theGenerators, theParentGroup.collector());
  newBasis.makeNormalClosure();

  // The basis is found. Now we try to reduce it to make further
  // processing easier.

  VectorOf<Word> basis = newBasis.getCommutatorWords();

  // First, we reduce words in terms of basic commutators.
  // We could proceed without this step, but it helps
  // to reduce the second one greatly.
  
  FreeGroup F( theParentGroup.theHirschNumber() );  
  basis = F.nielsenBasis(basis);
  
  // Convert words in terms of basic commutators to group
  // generators.

  for(int i = 0; i < basis.length(); i++) {
    PolyWord pw = basis.val(i);
    basis[i] = theParentGroup.commutators().wordForm().toWord(pw);
  }

  // Now reduce this vector

  FreeGroup F1( theParentGroup.numberOfGenerators() );
  basis = F1.nielsenBasis(basis);
  

  return basis;
}


//---------------------------------------------------------------------
// Methods dealing with group elements:
//---------------------------------------------------------------------

// Finds decomposition of the subgroup element w in terms of
// Malcev generators
// returns true iff w is an element of the subgroup
// Slow, requires initialization
bool SGOfFreeNilpotentGroupRep::decompose(const PolyWord& w, PolyWord& decomp) const {

  if( ! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::decompose: the basis must be built");

  return theBasisSet.decomposeWord(w,decomp);
}

//---------------------------------------------------------------------
// I/O:
//---------------------------------------------------------------------


//@njz
//void SGOfFreeNilpotentGroupRep::printBasis( class ostream& s ) const {
void SGOfFreeNilpotentGroupRep::printBasis( ostream& s ) const {
//
  if( ! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::printBasis: the basis must be built");

  for(int i = 0; i < theHirschNumber(); i++) {
    s << theBasisNames.val(i) << " = " 
      << theBasis.val(i).toChars(theParentGroup.basisNames())
      << endl;
  }
}


ostream& SGOfFreeNilpotentGroupRep::writeIPC( ostream& s) const {
  SGOfNilpotentGroupRep::writeIPC(s);
  s < theParentGroup < theGenerators < theBasis 
    < theBasisNames < theBasisSet;
}

istream& SGOfFreeNilpotentGroupRep::readIPC( istream& s) const {

  SGOfFreeNilpotentGroupRep* This = (SGOfFreeNilpotentGroupRep*) this;

  SGOfNilpotentGroupRep::readIPC(s);
  s > This->theParentGroup > This->theGenerators > This->theBasis 
    > This->theBasisNames > This->theBasisSet;
  
  return s;
}


Chars SGOfFreeNilpotentGroupRep::asDecomposition( const PolyWord& w ) const {

  if( ! basisIsInitialized() )
    error("SGOfFreeNilpotentGroupRep::asDecomposition: the basis must be built");

  return w.toChars( theBasisNames );
}


GenericRep* SGOfFreeNilpotentGroupRep::clone( ) const {
  return new SGOfFreeNilpotentGroupRep(*this);
}


const Type SGOfFreeNilpotentGroupRep::theSGOfFreeNilpotentGroupType =
Type( Type::unique() );

