
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SGOfNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//


#include "SGOfNilpotentGroupRep.h"

#include "NilpotentGroup.h"
#include "SGOfNilpotentGroup.h"

#include "PolyWordIterator.h"
#include "FreeGroup.h"
#include "conversions.h"

#include <stdio.h>


//=======================================================================
//================ class SGOfNilpotentGroupRep ==========================
//=======================================================================


// produces FP presentation of the subgroup
// requires the preimage basis to be built
PresentationForSNG SGOfNilpotentGroupRep::makePresentation() const {

  PresentationForSNG pres(preimageBasis(), parentGroup().relators(), 
			  parentGroup().collector());
  pres.build();
  return pres;
}


//------------------------ join ---------------------------------------

// Returns the generators for the join of this subgroup and the argument.
VectorOf<Word> 
SGOfNilpotentGroupRep::join(const SGOfNilpotentGroup& SGR) const {
  return join( SGR.generators() );
}

// Returns the generators for the join of this subgroup and the argument.
VectorOf<Word> 
SGOfNilpotentGroupRep::join( const VectorOf<Word>& G) const {

  int length1 = generators().length();
  int length2 = G.length();

  VectorOf<Word> newGens( length1 + length2, generators() );

  for(int i = 0; i < length2; i++)
    newGens[ length1 + i ] = G.val(i);

  FreeGroup F( parentGroup().numberOfGenerators() );
  return F.nielsenBasis(newGens);
}


//---------------------- isTrivial --------------------------------


bool SGOfNilpotentGroupRep::isTrivial() const {

  if( basisIsInitialized() )
    return basis().length() == 0;

  for(int i = 0; i < generators().length(); i++) {
    Word W = generators().constref(i);
    if( parentGroup().wordProblem(W) == no ) return false;
  }
  return true;
}


//---------------------- isCentral --------------------------------

bool SGOfNilpotentGroupRep::isCentral() const {

  for(int i = 0; i < generators().length(); i++) {

    Word W = generators().val(i);
    Word WInv = W.inverse();

    for(int j = 1; j <= parentGroup().numberOfGenerators(); j++) {

      Word comm = W * Generator(j) * WInv * Generator(-j);
      
      if( parentGroup().wordProblem(comm) == no ) return false;
    }
  }
  return true;
}

//---------------------- isNormal --------------------------------

bool SGOfNilpotentGroupRep::isNormal() const {

  for(int i = 0; i < generators().length(); i++) {
    Word theGenerator = generators().val(i);
    for (int j = 0;j<parentGroup().numberOfGenerators();j++){
      Generator groupGenerator(j+1);
      Generator groupGeneratorInv = inv(groupGenerator);
      if( ! contains(groupGenerator*theGenerator*groupGeneratorInv) ) 
	return false;
      if( ! contains(groupGeneratorInv*theGenerator*groupGenerator) ) 
	return false;    
    }
  }
  return true;
}

//---------------------- isAbelian --------------------------------

bool SGOfNilpotentGroupRep::isAbelian() const {

  for(int i = 1; i < generators().length(); i++) {

    Word W1 = generators().val(i);
    Word W1Inv = W1.inverse();

    for(int j = 0; j < i; j++) {

      Word W2 = generators().val(j);
      Word comm = W1 * W2 * W1Inv * W2.inverse();
            
      if( parentGroup().wordProblem( comm.freelyReduce() ) == no ) return false;
    }
  }
  return true;
}

const Type SGOfNilpotentGroupRep::theSGOfNilpotentGroupType = 
Type( Type::unique() );

//--------------------------- index -----------------------------

int SGOfNilpotentGroupRep::index( ) const {

  int theIndex = 1;

  MalcevSet preimage = preimageBasis();

  for( int k = 1; k <= parentGroup().nilpotencyClass(); k++ ) {
    AbelianGroup Ab = preimage.mapToQuotient(k);
    theIndex *= Ab.order().as_long();
    if(theIndex == 0) break;
  }
  return theIndex;
}
//-------------------------- subgroupClass -------------------------------

int SGOfNilpotentGroupRep::subgroupClass() const {
  
  if( ! parentIsInitialized() )
    error("SGOfFPNilpotentGroupRep::subgroupClass: the parent group must be is initialized");


  ListOf<Word> source = makeListOf(generators());

  int nilclass = parentGroup().nilpotencyClass();

  for(int k = 2; k <= nilclass; k++) {

    // make left-normed commutators of weight k

    ListOf<Word> dest;

    ListIterator< ListOf<Word> > iter(source);

    for( ; ! iter.done(); iter.next() ) {
      
      Word w = iter.value();
      Word wInv = w.inverse();

      int numgen = generators().length();

      for( int i = 0; i < numgen; i++ ) {
	Word comm = wInv * generators().constref(i).inverse() * w * generators().constref(i);
	comm = comm.freelyReduce();
	if( ! parentGroup().wordProblem(comm) ) 
	  dest.append(comm);
      }
    }

    if( dest.length() == 0 ) return k-1;
    source = dest;
  }
  return nilclass;
}

//---------------------- generators of normal closure ------------------------

VectorOf<Word> SGOfNilpotentGroupRep::normalClosureGens() const {
  VectorOf<Word> theGenerators = generators();
  int theClass = parentGroup().nilpotencyClass();
  int numberOfGens = parentGroup().numberOfGenerators();
  int lastOfLastWeight = theGenerators.length()-1;
  int firstOfLastWeight = 0;
  for (int i=1;i<theClass;i++){
    int numOfNew = 0;

    for (int j = firstOfLastWeight;j<=lastOfLastWeight;j++){
      Word Wi = theGenerators[j];
      Word WiInv = Wi.inverse();

      for (int k=0;k<numberOfGens;k++){
	Generator g(k+1); 
	Word comm = Wi * inv(g) * WiInv * g; 
	theGenerators.append(comm);
	numOfNew++;
      }
    }
    firstOfLastWeight = lastOfLastWeight+1;
    lastOfLastWeight += numOfNew;
  }
  FreeGroup F1( parentGroup().numberOfGenerators() );
  theGenerators = F1.nielsenBasis(theGenerators);  
  return theGenerators;
}

// Returns true iff this subgroup contains the subgroup generated by `V'.

bool SGOfNilpotentGroupRep::contains(const VectorOf<Word>& V) const {

  if( ! preimageIsInitialized() )
    error("SGOfNilpotentGroupRep::contains: preimage must be initialized");

  for(int i=0; i< V.length(); i++)
    if( ! preimageBasis().contains( V.val(i) ) ) return false;
  return true;
}

//---------------------- contains ----------------------------------

// Returns true iff this subgroup contains the word w

bool SGOfNilpotentGroupRep::contains(const Word& w) const {

  if( ! preimageIsInitialized() )
    error("SGOfNilpotentGroupRep::contains: preimage must be initialized");

  return preimageBasis().contains(w); 
}

//-------------------------------------------------------------------
// IPC tools:                                                          
//-------------------------------------------------------------------
    
ostream& SGOfNilpotentGroupRep::writeIPC( ostream& ostr ) const
{
  return ostr << actualType() << ' ';
}

istream& SGOfNilpotentGroupRep::readIPC( istream& istr ) const
{
  SGOfNilpotentGroupRep *This = (SGOfNilpotentGroupRep *)this;

  Type savedType = Type::notype();
  istr >> savedType;
  if(savedType != actualType())
    error("SGOfNilpotentGroupRep: wrong Rep type transferred via IPC");
  return istr;
}



