// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes NilpotentCollector
//
//
// Principal Author: Eugene Paderin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//


#include "PolyWordIterator.h"
#include "NilpCollectors.h"


//===========================================================================
//============================ class NGCollector ============================
//===========================================================================

ostream& NGCollector::writeIPC(ostream& s) const {
  s < (int)type();
  return NC->writeIPC(s);
}

istream& NGCollector::readIPC(istream& s) const {
  int t;
  s > t;
  NilpotentCollector::Type theType = (enum NilpotentCollector::Type) t;

  if(theType != NC->type()) 
    error("Attempt to change collector representation via IPC tools.");

  return NC->readIPC(s);
}


//==========================================================================
//=================== class NilpotentCollector =============================
//==========================================================================

//------------------------------------------------------------------------
//   Constructors and initializers
//------------------------------------------------------------------------

// To construct the set of basic commutators which constitute a Mal'cev
// basis for free nilpotent group of `numgen' generators and nilpotency
// class `nilclass'.
// Flag initialize shows whether the initialization should be
// performed immediately or delayed

NilpotentCollector::NilpotentCollector(int numgen, int nilclass, 
				       bool initialize)
  : BC(numgen, nilclass, initialize)
{}


NilpotentCollector::NilpotentCollector(const BasicCommutators& bc)
  : BC(bc)
{ }
  
void NilpotentCollector::initialize() const {    // Logical const!
  BC.initialize();
}


//------------------------------------------------------------------------
//   Common problems related with the collecting process
//------------------------------------------------------------------------


// collect the word
// Input word is given in generators of the nilpotent group.
// The output is a PolyWord in generators of Malcev basis.

PolyWord NilpotentCollector::collect(const Word& w) const {
  PolyWord pw = w;
  collectingProcess(pw);
  return pw;
}  

// collects the PolyWord
// The output is a PolyWord in generators of Malcev basis.

PolyWord NilpotentCollector::collect(const PolyWord& pw) const {
  PolyWord ppw = pw;
  collectingProcess(ppw);
  return ppw;
}

// collects one letter moving it to the left
// Returns true if the letter presents in the word after collection,
// and false if the letter was eliminated or not found

bool NilpotentCollector::collectLetter(PolyWord& w, Generator c) const {

  //cout << "Collecting letter " << c << " of weight " << BC.weightOf(ord(c)) << endl;

  if( ord(c) < 0 ) c = inv(c);
  
  PolyWordIterator iter(w);
  iter.startFromRight();

  //w.debugInfo();
  
  while( iter.searchToLeft(c) ) {

    //cout << w << endl;

    // move it left
    while(1) {


      // on the edge of the world
      if( iter.isFirst() ) {

	//cout << "Letter collected with power " << iter.thisLetter().power << endl;

	return true;
      }

      Letter leftLetter = iter.leftLetter();

      // cannot move further
      if( leftLetter.gen < c ) {

	//cout << "Letter collected with power " << iter.thisLetter().power << endl;
	return true;
      }
      
      //otherwise, commute them

      PolyWord commuted = BC.commuteLetters(leftLetter, iter.thisLetter());

      //cout << "Commuted: " << commuted << endl;

      iter.removeLeftLetter();

      if(commuted.isEmpty()) {
	iter.removeThisLetter();
	break;
      }
      else if( commuted.lastLetter().gen != c ) {
	iter.insertRight(commuted);
	iter.removeThisLetter();
      }
      else {
	iter.insertLeft(commuted);
	iter.stepLeft();
	iter.removeRightLetter();
      }
    }
    // arrive here if the letter was eliminated and we must find
    // another one
  }

  // arrive here if there is no such letter in the word

  //cout << "Letter eliminated." << endl;

  return  false;
}

// returns the weight of the word (the weight of the first letter
// in the word's collected form)
// 0 means word is trivial

int NilpotentCollector::weightOf(const Word& w) const {
  PolyWord ppw = w;
  for(int c = 1; c <= BC.theHirschNumber(); c++)
    if( collectLetter(ppw, c) )
      return BC.weightOf(c);
  return BC.nilpotencyClass() + 1;
}

int NilpotentCollector::weightOf(const PolyWord& pw) const {
  PolyWord ppw = pw;
  for(int c = 1; c <= BC.theHirschNumber(); c++)
    if( collectLetter(ppw, c) )
      return BC.weightOf(c);
  return  BC.nilpotencyClass() + 1;
}

// returns a collected product of two words
PolyWord NilpotentCollector::multiply(const PolyWord& pw1, const PolyWord& pw2) const {
  PolyWord res = pw1 * pw2;
  collectingProcess(res);
  return res;
}

// returns a collected power of the word
PolyWord NilpotentCollector::raiseToPower(const PolyWord& pw, int power) const {
  PolyWord res = pw.raiseToPower(power);
  collectingProcess(res);
  return res;
}



// returns collected inverse of the word
PolyWord NilpotentCollector::inverse(const PolyWord& pw) const {
  PolyWord res = pw.inverse();
  collectingProcess(res);
  return res;
}


//------------------------------------------------------------------------
//   IPC tools
//------------------------------------------------------------------------

ostream& NilpotentCollector::writeIPC(ostream& s) const {
  return s < BC;
}

istream& NilpotentCollector::readIPC(istream& s) const {
  return s > BC;
}
// Changes the current collector type.


// makes a collector of given type
//@njz
//NilpotentCollector * NilpotentCollector::make(int numgen, int nilclass,
//					      Type colType = STANDARD, 
//					      bool initialize = true)
NilpotentCollector * NilpotentCollector::make(int numgen, int nilclass,
					      Type colType = STANDARD, 
					      bool initialize)
//
{
  switch(colType) {

  case TO_THE_LEFT:
    return new CollectorToTheLeft(numgen, nilclass, initialize);
    
  case FROM_THE_LEFT:
    return new CollectorFromTheLeft(numgen, nilclass, initialize);
    
  default:
    error("NilpotentCollector::make: collector not implemented.");
    return NULL;
  }

}


