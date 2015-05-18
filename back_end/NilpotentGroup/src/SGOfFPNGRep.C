
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SGOfFPNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//


#include "SGOfFPNGRep.h"

#include "PolyWordIterator.h"
#include "FreeGroup.h"
#include "conversions.h"

#include <stdio.h>


//=======================================================================
//=============== class SGOfFPNilpotentGroupRep =========================
//=======================================================================


//---------------------------------------------------------------------
//    Constructors / initializers:
//---------------------------------------------------------------------

SGOfFPNilpotentGroupRep::
SGOfFPNilpotentGroupRep(const NilpotentGroup& ng, const VectorOf<Word>& gens)
  : thePreimage(), preimageInitialized(false), theBasisSet(gens, ng)
{ }
// creates a subgroup


// the initialization of thePreimageBasis
void SGOfFPNilpotentGroupRep::initPreimage( ) const {    // logical const !

  if( preimageInitialized ) return;

  SGOfFPNilpotentGroupRep *This = (SGOfFPNilpotentGroupRep *)this;
  //to break physical constness

  VectorOf<Word> fpGens = concatenate( parentGroup().relationSubgroupGenerators(),
				       generators() );
  
  This->thePreimage = MalcevSet(fpGens, parentGroup().collector());
  thePreimage.makeFull();
  This->preimageInitialized = true;
}


void SGOfFPNilpotentGroupRep::initParent() const {
  parentGroup().initialize();
}


void SGOfFPNilpotentGroupRep::initBasis() const {
  parentGroup().initialize();
  initPreimage();
  theBasisSet.initialize();

  SGOfFPNilpotentGroupRep* This = (SGOfFPNilpotentGroupRep*) this;

  This->theBasis = theBasisSet.asGroupWords();

  int len =  theBasis.length();
  This->theBasisNames = VectorOf<Chars>(len);

  for(int i = 0; i < len; i++)
    This->theBasisNames.ref(i) = Chars("b") + Chars(i+1);

}


//---------------------------------------------------------------------
//  Accessors:
//---------------------------------------------------------------------

// returns the preimage of subgroup
const MalcevSet& SGOfFPNilpotentGroupRep::preimageBasis() const {

  if( ! preimageIsInitialized() )
    error("SGOfFPNilpotentGroupRep::preimageBasis: preimage not initialized");

  return thePreimage;
}


VectorOf<PolyWord> SGOfFPNilpotentGroupRep::basis() const {
  
  if( ! theBasisSet.isFull())
    error("SGOfFPNilpotentGroupRep::basis: basis not initialized");

  return theBasis;
}


VectorOf<Chars> SGOfFPNilpotentGroupRep::basisNames() const {
  
  if( ! theBasisSet.isFull())
    error("SGOfFPNilpotentGroupRep::basisNames: basis not initialized");

  return theBasisNames;
}


// returns the order of subgroup (0 means infinite).
// Fast, requires initialization of the parent group
int SGOfFPNilpotentGroupRep::order() const {

  if( ! theBasisSet.isFull())
    error("SGOfFPNilpotentGroupRep::order: basis not initialized");

  int ord = 1;

  for(int i = 0; i < theBasis.length() && ord != 0; i++)
    ord *= parentGroup().orderOf(theBasis.val(i));

  return ord;
}


//---------------------------------------------------------------------
//  Methods producing related structures
//---------------------------------------------------------------------

//------------------- normalClosure -----------------------------
 

// returns the normal closure of the subgroup

VectorOf<Word> SGOfFPNilpotentGroupRep::normalClosureBasis() const {

  SubgroupBasis newBasis = theBasisSet.normalClosure();

  // The basis is found. Now we try to reduce it to make further
  // processing easier.

  VectorOf<Word> basisWords = newBasis.asWords();

  // Now reduce this vector

  FreeGroup F1( parentGroup().numberOfGenerators() );
  basisWords = F1.nielsenBasis(basisWords);

  return basisWords;
}


//---------------------------------------------------------------------
// Methods dealing with group elements:
//---------------------------------------------------------------------


//---------------------- decompose ----------------------------------

bool SGOfFPNilpotentGroupRep::decompose(const PolyWord& w, PolyWord& decomp) const {

  if( ! basisIsInitialized() )
    error("SGOfFPNilpotentGroupRep::decompose: basis must be initialized");

  return theBasisSet.decomposeWord(w, decomp); 
}


//---------------------------------------------------------------------
// I/O:
//---------------------------------------------------------------------


void SGOfFPNilpotentGroupRep::printBasis(ostream& s) const {

  if( ! basisIsInitialized() )
    error("SGOfFPNilpotentGroupRep::printBasis: basis must be initialized");

  for(int i = 0; i < theBasis.length(); i++) {
    s << theBasisNames[i] << " = " 
      << parentGroup().asDecomposition(theBasis[i]) << endl;
  }
}    

// Outputs the given decomposition in terms of basis generators
Chars SGOfFPNilpotentGroupRep::asDecomposition(const PolyWord& decomposition) const {

  if(! basisIsInitialized() )
    error("FPNilpotentGroupRep::asDecomposition: basis must be initialized");

  return decomposition.toChars( theBasisNames );
}


//---------------------- IPC tools -----------------------------------

// IPC output
ostream& SGOfFPNilpotentGroupRep::writeIPC(ostream& s) const {

  SGOfNilpotentGroupRep::writeIPC(s);

  s < thePreimage < preimageInitialized 
    < theBasisSet < theBasis < theBasisNames;
  return s;
}

// IPC intput
istream& SGOfFPNilpotentGroupRep::readIPC( istream& s) const {

  SGOfFPNilpotentGroupRep* This = (SGOfFPNilpotentGroupRep*) this;

  SGOfNilpotentGroupRep::readIPC(s);
  s > This->thePreimage > This->preimageInitialized > This->theBasisSet
    > This->theBasis > This->theBasisNames;

  return s;
}

GenericRep* SGOfFPNilpotentGroupRep::clone( ) const {
  return new SGOfFPNilpotentGroupRep(*this);
}

const Type SGOfFPNilpotentGroupRep::theSGOfFPNilpotentGroupType =
Type( Type::unique() );



