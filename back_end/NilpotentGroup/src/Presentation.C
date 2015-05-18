// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes NilpotentRelator, NilpotentPresentation,
//           PresentationForFNG
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Special Notes:
//
//
// Revision History:
//


#include "Presentation.h"
#include "PolyWordIterator.h"

#include "FPGroup.h"


ostream& operator << ( ostream& s, const NilpotentRelator& rel ) {
  s << "(" << rel.left1 << "," << rel.left2 << ") = " << rel.right << endl;
}





///////////////////// NilpotentPresentation /////////////////////////////////


//-------------------------------------------------------------------------
//   User interface
//-------------------------------------------------------------------------


void NilpotentPresentation::build() {

  if(built) return;
  
  NilpotentPresentation* This = (NilpotentPresentation*) this;
  // breaking physical const-ness

  int numGen = theGenerators.length();


  //  build commutators 

  //for( int left = 0; left < numGen; left++ ) {
  for( int left = 1; left < numGen; left++ ) {

    //for( int right = 0; right < numGen; right++ ) {
    for( int right = 0; right < left; right++ ) {
    
      //if(left == right) continue;  // always 1

      // compute commutator

      PolyWord comm = commutator( theGenerators.val(left), theGenerators.val(right) );
      
      NilpotentRelator rel;
      rel.setLeft( left+1, right+1 );
      rel.right = decompose(comm);      
      This->theRelators.append( rel );
    }
  }

  buildKernelRelators();

  This->built = true;
}


void NilpotentPresentation::print( ostream& s ) const {

  //  improvePresentation();
  printGenerators(s);

  // print the relators

  s << "Relators of the presentation:" << endl << endl;

  for( int i = 0; i < theRelators.length(); i++) {
    if (theRelators[i].isCommutation() || !theRelators[i].right.isEmpty())
      s << printRelator(i) << endl;
  }
  s << flush;
}


// produces a group using the presentation
FPGroup NilpotentPresentation::makeGroup() const {

  if( ! built )
    error("NilpotentPresentation::makeGroup: presentation is not built.");

  SetOf<Word> rels = convertRelators();
  return FPGroup( theNames, rels );
}



ostream& NilpotentPresentation::write(ostream& s) const {
  return s < theGenerators < theNames < theRelators < built < theGroupRelators;
}
// IPC output


istream& NilpotentPresentation::read(istream& s) {
  return s > theGenerators > theNames > theRelators > built > theGroupRelators;
}
// IPC input


//---------------------------------------------------------------------
//  Helper methods to form and use the presentation
//---------------------------------------------------------------------


SetOf<Word> NilpotentPresentation::convertRelators() const {

  SetOf<Word> res;

  for( int i = 0; i < theRelators.length(); i++) {

    NilpotentRelator rel = theRelators.val(i);
    PolyWord r = rel.right;

    if( rel.isCommutation() ) {
      r = commutator( PolyWord(Letter(rel.left2)), 
		      PolyWord(Letter(rel.left1)) ) * r;
    }

    res.adjoinElement(Word(r));
  }
  return res;
}


// outputs the ith relator in terms of theGenerators
Chars NilpotentPresentation::printRelator(int i) const {

  NilpotentRelator rel = theRelators.val(i);
  Chars result;

  // check if it is a commutation relator

  if( rel.isCommutation() ) {
    result = Chars("[") + theNames.val(rel.left1 - 1) 
      + ", " + theNames.val(rel.left2 - 1)
      + "] = " + rel.right.toChars(theNames);
  }
  else {
    result = rel.right.toChars(theNames) + " = 1";
  }

  return result;
}


void NilpotentPresentation::buildKernelRelators() {

  if( theGroupRelators.length() == 0 )
    return;

  MalcevSet basis(theGroupRelators, theCollector);
  MalcevSet closure = basis.normalClosure();

  VectorOf<PolyWord> kernelRels = closure.getPolyWords();

  NilpotentRelator rel;
  rel.setLeft(0,0);

  for(int i=0; i < kernelRels.length(); i++) {
    rel.right = decompose(kernelRels.val(i));
    theRelators.append(rel);
  }
}

void NilpotentPresentation::improvePresentation()
  //VectorOf<Word>& newGenerators,
  //VectorOf<NilpotentRelator>& newRelators;
 {
  bool wasChanged = false;
  int numOfOldRelators = theRelators.length();
  VectorOf<int> extractedGens;
  // Looking for generators, equal to 1. If found, then empty 
  // this generator in theGenerators.
  for (int i = 0;i < numOfOldRelators;i++){
    NilpotentRelator& rel = theRelators[i];
    int extractGen=0;
    if (rel.right.isEmpty() &&  !rel.isCommutation()){
      if (rel.left1 != 0){
	extractGen = abs(rel.left1);
	rel.left1 = 0;
      }else{
	extractGen = abs(rel.left2);
 	rel.left2 = 0;
      }
    }
    if (rel.right.numberOfLetters()==1 &&  rel.left1==0 && rel.left2==0){
      extractGen = abs(rel.right.firstLetter().gen);
      rel.right.clear();
    }
    if (extractGen){
      
      theGenerators[extractGen-1].clear();
      theNames[extractGen-1] = "";
    }
  }
  //cout << theNames << endl;
  //cout << theRelators << endl;
  // Find the last of identity generators in relations set.
  for (int i = 0;i < numOfOldRelators;i++){
    NilpotentRelator& rel = theRelators[i];
    if ( rel.left1 != 0 )
      if (theNames[abs(rel.left1)-1].length() == 0){
	rel.left1 = 0;
	wasChanged = true;
      }
    if ( rel.left2 != 0 )
      if (theNames[abs(rel.left2)-1].length() == 0){
	rel.left2 = 0;    
 	wasChanged = true;
      }
   //cout << rel.right << endl;
    PolyWordIterator I(rel.right);
    I.startFromLeft();
    int wordLen = rel.right.numberOfLetters();
    for (int j=0;j<wordLen;j++){
      if (theNames[abs(I.thisLetter().gen)-1].length() == 0){
	I.removeThisLetter();
      }
      else
	I.stepRight();
    }
    //    if (wasChanged)
    //   improvePresentation();
    
    //cout << rel.right << endl;
  }
}

//////////////////////////// PresentationForNG //////////////////////////


PresentationForNG::PresentationForNG(const NGCollector& coll, 
				     const VectorOf<Word>& rels)
  : NilpotentPresentation(coll, rels)      
{

  theCollector.initialize();

  int genNum = theCollector.commutators().theHirschNumber();
  
  theGenerators = VectorOf<PolyWord> (genNum);
  theNames = VectorOf<Chars> (genNum);

  for( int i = 0; i < genNum; i++) {
    theGenerators.ref(i) = Letter(i+1);
    theNames.ref(i) = theCollector.commutators().commutatorName(i+1);
  }
}


PolyWord PresentationForNG::decompose( const PolyWord& pw ) const {
  return theCollector.collect(pw);
}
// decomposes the word given in terms of the group generators
// the result is the word in terms of theGenerators.



//////////////////////////// PresentationForSNG //////////////////////////


PresentationForSNG::PresentationForSNG(const MalcevSet& preimage, 
				       const VectorOf<Word>& rels,
				       const NGCollector& coll)
  : NilpotentPresentation(coll, rels), thePreimageBasis(preimage)
{
  theGenerators = preimage.getPolyWords();
  int genNum = theGenerators.length();
  theNames = VectorOf<Chars> (genNum);

  for( int i = 0; i < genNum; i++) {
    theNames.ref(i) = Chars("h") + Chars(i+1);
  }
}


PolyWord PresentationForSNG::decompose( const PolyWord& pw ) const {
  PolyWord decomp;
  if( ! thePreimageBasis.decomposeWord(pw, decomp) )
    error("PresentationForSNG::decompose: internal error");
  return decomp;
}
// decomposes the word given in terms of the group generators
// the result is the word in terms of theGenerators.


void PresentationForSNG::printGenerators( ostream& s ) const {

  s << "Generators of the presentation:" << endl << endl;

  for( int i = 0; i < theGenerators.length(); i++ ) {
    if (theNames.val(i).length() > 0 )
      s << theNames.val(i) << " = " 
	<< theCollector.commutators().wordForm().asCommutatorWord( theGenerators.val(i) )
	<< endl;
  }
}


