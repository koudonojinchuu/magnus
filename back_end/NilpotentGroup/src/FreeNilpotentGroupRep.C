
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FreeNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//
// Special Notes:
//
//

#include "FreeNilpotentGroupRep.h"
#include "PolyWordIterator.h"
#include "Letter.h"
#include <stdio.h>
#include "GCD.h"
#include "Map.h"

//=====================================================================
//================== struct FreeNilpotentGroupRep =====================
//=====================================================================

//-------------------------------------------------------------------
// 	Accessors
//-------------------------------------------------------------------


// The basis of the group
VectorOf<PolyWord> FreeNilpotentGroupRep::basis() const {
  VectorOf<PolyWord> res( theHirschNumber() );
  for(int c = 0; c < theHirschNumber(); c++)
    res.ref(c) = PolyWord( Letter(c) );
  return res;
}

// The names of the basic commutators
VectorOf<Chars> FreeNilpotentGroupRep::basisNames() const {
  VectorOf<Chars> res( theHirschNumber() );
  for(int c = 0; c < theHirschNumber(); c++)
    res.ref(c) = commutators().commutatorName(c+1);
  return res;
}


//------------------------------------------------------------
//  Special helper stuff
//------------------------------------------------------------

const Type FreeNilpotentGroupRep::theFreeNilpotentGroupType =
Type( Type::unique() );


//-------------------------------------------------------------------
// 	Methods dealing with the properties of the group:
//-------------------------------------------------------------------

//@ep Cannot make them inline due to compiler bugs

int FreeNilpotentGroupRep::order( ) const { return -1; }
Trichotomy FreeNilpotentGroupRep::isTrivial( ) const { return no; }
Trichotomy FreeNilpotentGroupRep::isFinite( ) const { return no; }
Trichotomy FreeNilpotentGroupRep::isInfinite( ) const { return ! isFinite(); }
Trichotomy FreeNilpotentGroupRep::isAbelian( ) const { return nilClass == 1; }
Trichotomy FreeNilpotentGroupRep::isFree( ) const { return theNumberOfGenerators == 1; }


//-------------------------------------------------------------------
//  Methods dealing with group elements
//-------------------------------------------------------------------

//--------------- areEqual ------------------------------

Trichotomy FreeNilpotentGroupRep::areEqual(const Elt& e1, const Elt& e2) const {
  PolyWord w1 = (const Word&)e1;
  PolyWord w2 = (const Word&)e2;

  for(int c = 1; c <= theHirschNumber(); c++) {
    bool presentInFirst  = theCollector.collectLetter(w1, c);
    bool presentInSecond = theCollector.collectLetter(w2, c);

    if( ! presentInFirst && ! presentInSecond ) continue;

    if( presentInFirst != presentInSecond ) return false;

    if( w1.firstLetter() != w2.firstLetter() ) return false;

    w1.removeFirstLetter();
    w2.removeFirstLetter();
  }    

  if(w1.numberOfLetters() != 0 || w2.numberOfLetters() != 0)
      error("Some letters lost in FreeNilpotenrGroup::areEqual.");

  return true;
}

//------------------ wordProblem -----------------------------------

Trichotomy FreeNilpotentGroupRep::wordProblem( const Word& w ) const {
  PolyWord pw = w;
  return theCollector.weightOf(pw) == nilClass+1;
}

//------------------ maximalRoot -----------------------------------

void FreeNilpotentGroupRep::maximalRoot(const PolyWord& pw, PolyWord& root,
					int& power ) const {

  PolyWord theWord = decompose(pw);

  if( theWord.isEmpty() ) {
    root = PolyWord();
    power = 1;
    return;
  }

  // ----- do the first step
  // find the root in the class firstClass

  int firstClass = theCollector.weightOf(theWord);

  PolyWord restrictedWord = theWord;
  mapToClass(restrictedWord, firstClass);

  int curPower;
  PolyWord curRoot;

  findAbelianRoot( restrictedWord, curRoot, curPower );

  if( curPower == 1 ) {
    root = pw;
    power = 1;
    return;
  }

  // --- do next steps

  for( int k = firstClass + 1; k <= nilClass; k++ ) {

    PolyWord preimage = curRoot.raiseToPower(-curPower);

    PolyWord centerWord = theWord;
    mapToClass(centerWord, k);
    centerWord = theCollector.multiply(preimage, centerWord);
    mapToClass(centerWord, k);

    int newPower = GCDofExponents(centerWord);
    newPower = GCD( newPower, curPower );  

    if( newPower == 1 ) {
      root = pw;
      power = 1;
      return;
    }

    PolyWordIterator iter(centerWord);

    for( iter.startFromLeft(); ! iter.done(); iter.stepRight() )
      iter.thisLetter().power /= newPower;
    
    curRoot = theCollector.raiseToPower(curRoot, curPower / newPower );
    curRoot = theCollector.multiply(curRoot, centerWord);
    mapToClass(curRoot, k);
    curPower = newPower;
  }

  root = curRoot;
  power = curPower;
}


//-------------------------------------------------------------------
// Internal methods
//-------------------------------------------------------------------


//------------------- findAbelianRoot ------------------------------

// finds the root of the given PolyWord assuming that all
// the word generators commute and the word is collected

void FreeNilpotentGroupRep::findAbelianRoot(const PolyWord& pw, PolyWord& root,
					int& power ) {

  power = GCDofExponents(pw);
  root = pw;

  PolyWordIterator iter(root);

  for( iter.startFromLeft(); ! iter.done(); iter.stepRight() )
    iter.thisLetter().power /= power;
}


int FreeNilpotentGroupRep::GCDofExponents(const PolyWord& pw) {

  int gcd = 0;

  ConstPolyWordIterator iter(pw);

  for( iter.startFromLeft(); ! iter.done(); iter.stepRight() )
    gcd = GCD( gcd, iter.thisLetter().power );

  return abs(gcd);
}


// Returns the weight of the word decomposed in terms of group basis.
int FreeNilpotentGroupRep::weightOfDecomposed( const PolyWord& pw) const {
  return commutators().weightOf( ord( pw.firstLetter().gen ) );
}


//---------------------------------------------------------------------------
//    I/O
//---------------------------------------------------------------------------


void FreeNilpotentGroupRep::write(ostream& s) const {

  NilpotentGroupRep::write(s);
  s < nilClass < theCollector;
}

void FreeNilpotentGroupRep::read(istream& s) {

  //  FreeNilpotentGroupRep *This = (FreeNilpotentGroupRep *) this;
  NilpotentGroupRep::read(s);
  s > nilClass > theCollector;
}


VectorOf<Word> FreeNilpotentGroupRep::inverseAuto(const VectorOf<Word>& V) const
{
#if SAFETY > 0
  if ( V.length() != theNumberOfGenerators ) 
    error("Wrong argument in FreeNilpotentGroupRep::inverseAuto");
#endif
  FPGroup G(theNamesOfGenerators);
  AbelianGroup abg(G);
  Map IAAuto(G,G,abg.inverseAuto(V));
  Map inverseMap(IAAuto);
  Map fiAuto(composition( Map(G,G,V),IAAuto) );
  for (int i=1;i<nilClass;i++){
    VectorOf<Word> tmpVec(fiAuto.generatingImages());
    for (int j=0;j<theNumberOfGenerators;j++)
      tmpVec[j] = Generator(j+1)*(inv(Generator(j+1))*tmpVec[j]).
	inverse().freelyReduce();
    IAAuto.setGeneratingImages(tmpVec);
    inverseMap = composition(inverseMap,IAAuto);
    fiAuto = composition(fiAuto,IAAuto);
  }
  return inverseMap.generatingImages();  
}
VectorOf<Word> FreeNilpotentGroupRep::centralizer( const Word& w) const
{
  int t = weightOf(w);
  int weightOfQ = nilpotencyClass()+1-t;
  NGWordForms bcForms(commutators());
  // Helper class for conversion to words

  VectorOf<Word> theCentralizer(getLCSterm(weightOfQ));
  
  if (t < weightOfQ){ 
    PolyWord root;
    int power;
    maximalRoot( decompose(w), root, power);
    theCentralizer.append(bcForms.toWord(root));
  }
  return theCentralizer;
}
