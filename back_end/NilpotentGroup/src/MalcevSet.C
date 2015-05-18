
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of MalcevSet class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Special Notes:
//
//  Almost all algorithms of this class are time consuming.
//
// Revision History:
//

#include "MalcevSet.h"
#include "PolyWordIterator.h"


//@db 2.91 this function is defined at the end of this file 
//    (template instantiation)

//@njz: removed
//int SetData< QuickAssociation<Generator,PolyWord> >::
//  hashElement( const QuickAssociation<Generator,PolyWord>& t) const;
#if GCC_VERSION < 29600
int SetData< QuickAssociation<Generator,PolyWord> >::
  hashElement( const QuickAssociation<Generator,PolyWord>& t) const;
#endif
//

//@db


//================================================================
//------------------- MalcevSet ----------------------------------
//================================================================


//-----------------------------------------------------
//   Constructors  
//-----------------------------------------------------


// construct empty basis
//MalcevSet::MalcevSet(const NGCollector& nc)
//  : isBasis(false), isNormal(dontknow), theCollector(nc)
//{ }


// a placeholder
MalcevSet::MalcevSet()
  : isBasis(false), isNormal(dontknow), theCollector(1,1)
{ }
  
// put the words to the set
MalcevSet::MalcevSet(const VectorOf<Word>& v, const NGCollector& nc)
  : isBasis(false), isNormal(dontknow), theCollector(nc)
{
  for(int i = 0; i < v.length(); i++)
    addWord( v.val(i) );
}



//-----------------------------------------------------
//   Helper methods
//-----------------------------------------------------


//--------------------- reduceWords ---------------------------

// Reduces two words
// After reduction the leading term of pw1 is in minimal power,
// the leading term of pw2 is greater.
// Returns true iff pw1 was changed

bool MalcevSet::reduceWords(PolyWord& pw1, PolyWord& pw2) const {

  if(pw1.isEmpty() || pw2.isEmpty())
    error("MalcevSet::reduceWords: empty argument");

  bool firstChanged = false;

  int power1 = absPower(pw1);
  int power2 = absPower(pw2);

  // make both PolyWords to be of distinct signs

  if( sign(pw1) ^ sign(pw2) == 0)  // if they have the same sign
    pw2 = pw2.inverse();

  // in fact, this is Euclid algorithm for finding GCD
    
  do {

    if( power1 > power2 ) {
      // swapping two words
      PolyWord tmp = pw1;
      pw1 = pw2;
      pw2 = tmp;
      int t = power1;
      power1 = power2;
      power2 = t;	
      firstChanged = true;
    }      

    power2 -= power1;
    pw2 = theCollector.multiply(pw1, pw2);
 
  } while(power2 != 0);

  return firstChanged;
}  


//--------------------- makeCommutator --------------------------------

PolyWord MalcevSet::makeCommutator( PolyWord& pw1, PolyWord& pw2 ) {

  // commute words

  PolyWord comm = commutatorOfInverses(pw1, pw2);
  comm.freelyReduce();
  return comm;
}


//------------------ checkMembership ----------------------------------

void MalcevSet::checkMembership(PolyWord& remainder) const {

  while( ! remainder.isEmpty() && theSet.bound( leader(remainder) ) ) {
    PolyWord curWord = theSet.valueOf( leader(remainder) );
    if( absPower(remainder) % absPower(curWord) != 0 ) 
      break;

    // The rest can be reduced. Do it.
    reduceWords( curWord, remainder );
  }
}


//-----------------------------------------------------
//   Word containment problem
//-----------------------------------------------------


//------------------ contains ------------------------------------------

// returns true iff the word is a member of the set
// ** Time-consuming

bool MalcevSet::contains(const Word& w) const {
  PolyWord pw = theCollector.collect(w);
  checkMembership(pw);
  return pw.isEmpty();
}


//------------------ decomposeWord -------------------------

// determines whether w is an element of the subgroup
// if yes, returns its decomposition in terms of Malcev basis

bool MalcevSet::decomposeWord(const PolyWord& w, PolyWord& decomp) const {

  if( ! isBasis )
    error("Attempt to use MalcevSet::decomposeWord before the set is full.");

  PolyWord remainder = theCollector.collect(w);
  decomp = PolyWord();

  // try to decompose the remainder

  while( ! remainder.isEmpty() && theSet.bound( leader(remainder) ) ) {
    PolyWord divisor = theSet.valueOf( leader(remainder) );
    if( absPower(remainder) % absPower(divisor) != 0 ) 
      break;

    // The remainder can be reduced. Do it.
    int divPower = power(remainder) / power(divisor);
    decomp.append( Letter( leader(remainder), divPower ) );

    if( divPower < 0 ) {
      divPower = - divPower;
    }
    else {
      divisor = theCollector.inverse(divisor);
    }

    for(int i = 0; i < divPower; i++) {
      remainder = theCollector.multiply(divisor, remainder);
    }

  }

  // if the remainder cannot be decomposed

  if( ! remainder.isEmpty() ) {
    decomp = PolyWord();
    return false;
  }
  
  // if the remainder initially was empty

  if( decomp.isEmpty() ) return true;

  // initially decomp is a series of letters (gen, power),
  // where gen is a leader of decomposition component and
  // power is its power

  // now translate decomp to Malcev basis terms: replace
  // gen with index of basis word having leader gen

  int curElement = 0;   // index of current basis element

  PolyWordIterator iter(decomp);
  iter.startFromLeft();

  for( int c = 1; c <= theCollector.commutators().theHirschNumber(); c++ ) {
    Generator theLeader(c);
    if( ! theSet.bound(theLeader) ) continue;
    ++curElement;

    if( iter.thisLetter().gen == theLeader ) {
      iter.thisLetter().gen = Generator(curElement);
      iter.stepRight();
      if( iter.done() ) break;
    }
  }
  return true;
}


//-----------------------------------------------------
//   Accessors
//-----------------------------------------------------


//-------------------- isNormalClosure ----------------------------------

bool MalcevSet::isNormalClosure() const {

  if( isNormal != dontknow ) return isNormal;

  if( ! isBasis )
    error("Attempt to use MalcevSet::isNormalClosure before the set is full.");

  const BasicCommutators& BC = theCollector.commutators();
  MalcevSet* This = (MalcevSet *)this;    // to break physical constness

  //the subgroup is normal iff any w^x is in the set

  for(int i = 1; i < BC.theFirstOfWeight(BC.nilpotencyClass() ); i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord W = theSet.valueOf( Generator(i) );
    
    for(int j = 1; j <= BC.numberOfGenerators(); j++) {

      PolyWord conj = collect( Letter(j, -1) * W * Letter(j, 1) );

      checkMembership(conj);

      if(! conj.isEmpty() ) {
	This->isNormal = no;
	return false;
      }
    }
  }
  This->isNormal = yes;
  return true;
}


//-----------------------------------------------------
//   Methods to form the set
//-----------------------------------------------------


//----------------------- addWord ---------------------------------

// adds w to the set
// returns true iff the set was changed
bool MalcevSet::addWord( const Word& w ) {

  PolyWord remainder = theCollector.collect(w);
  checkMembership(remainder);
  return addWord(remainder);
}

// adds w to the set
// returns true iff the set was changed
bool MalcevSet::addWord( const PolyWord& pw ) {

  bool wasChanged = false;
  PolyWord remainder = collect(pw);

  while( ! remainder.isEmpty() ) {
    Generator key = leader(remainder);

    // if there is no word with the same leader, include the remainder to the set

    if( ! theSet.bound(key) ) {
      theSet.bind( key, remainder );
      isBasis = false;
      isNormal = dontknow;
      return true;
    }

    // reduce the remainder

    PolyWord currentWord = theSet.valueOf(key);

    if( reduceWords( currentWord, remainder) ) {
      theSet.bind( key, currentWord );
      isBasis = false;
      isNormal = dontknow;
      wasChanged = true;
    }
  }
  return wasChanged;
}


//-------------------- makeFull ----------------------------------

void MalcevSet::makeFull() const {

  if( isBasis ) return;

  MalcevSet *This = (MalcevSet *)this;  // to break physical constness
  const BasicCommutators& BC = theCollector.commutators();
  int nilClass = BC.nilpotencyClass();

  // add all commutators [Wj, Wi]

  int upper_i =  BC.theFirstOfWeight(nilClass);

  for(int i = 2; i < upper_i; i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord Wi = theSet.valueOf( Generator(i) );
    int upper_j = 
      BC.theFirstOfWeight( nilClass - BC.weightOf(i) + 1);

    if( upper_j > i ) upper_j = i;

    for(int j = 1; j < upper_j; j++) {

      if( ! theSet.bound( Generator(j) ) ) continue;

      PolyWord Wj = theSet.valueOf( Generator(j) );

      PolyWord comm = makeCommutator(Wj, Wi);
      This->addWord(comm);
    }
  }
  This->isBasis = true;
}


//-------------------- normalClosure ----------------------------------

MalcevSet MalcevSet::normalClosure() const {
  MalcevSet ncl = *this;
  ncl.makeNormalClosure();
  return ncl;
}


//-------------------- makeNormalClosure ----------------------------------

void MalcevSet::makeNormalClosure() {

  if(isNormal == yes) return;

  const BasicCommutators& BC = theCollector.commutators();
  int nilClass = BC.nilpotencyClass();

  int upper_i = BC.theFirstOfWeight(nilClass);

  for(int i = 1; i <= upper_i; i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord Wi = theSet.valueOf( Generator(i) );
    PolyWord WiInv = Wi.inverse();

    // trying generators of the group

    for(int j = 1; j <= BC.numberOfGenerators(); j++) {

      Generator g(j); 
      PolyWord comm = collect( Wi * Letter(g,-1) * WiInv * Letter(g,1) ); 

      addWord(comm);
    }

    // trying basis elements

    int upper_j = BC.theFirstOfWeight(nilClass - BC.weightOf(i) + 1);
    if(upper_j > i) upper_j = i;

    for(int j = 1; j < upper_j; j++) {

      if( ! theSet.bound( Generator(j) ) ) continue;
      PolyWord Wj = theSet.valueOf( Generator(j) );

      PolyWord comm = collect( Wi * Wj * WiInv * Wj.inverse() );

      addWord(comm);
    }
  }
  isBasis = true;
  isNormal = yes;
}


//----------------------- mapToQuotient -------------------------

// maps the subgroup generated by the set to the quotient 
// G_weight / G_{weight+1}
// Fast, the set must be full

AbelianGroup MalcevSet::mapToQuotient(int k) const {

  if( ! isBasis )
    error("MalcevSet::mapToQuotient: the set must be full");

  // The generators of the quotient are basic commutators of weight k.

  const BasicCommutators& bc = theCollector.commutators();

  int numGen = bc.numberOfWeight(k);
  int firstGen = bc.theFirstOfWeight(k) - 1;

  // The relators are Malcev basis words of weight k

  SetOf<Word> relsForAbelian;

  QuickAssociationsIterator< Generator, PolyWord > iter(theSet);
  for( ; ! iter.done(); iter.next() ) {

    // take a word from Malcev basis

    PolyWord pw = iter.value();
    Letter first = pw.firstLetter();
    if( ord(first.gen) != firstGen ) continue; 
    
    //Ok, this is a word from the quotient. Abelianize it.

    ConstPolyWordIterator iter( pw );
    Word w;
    for(iter.startFromLeft(); ! iter.done(); iter.stepRight() ) {

      Letter s = iter.thisLetter();
      int newgen =  ord(s.gen) - firstGen;
      if( newgen > numGen ) break;
      s.gen = Generator( newgen );
      w *= Word(s);
    }
    relsForAbelian.adjoinElement(w);
  }

  // make the abelian quotient
  
  AbelianGroup abel( FPGroup(numGen, relsForAbelian) );
  abel.computeCyclicDecomposition();
  return abel;
}






//-----------------------------------------------------
//   Conversions to other presentations
//-----------------------------------------------------


//---------------------- getWords ----------------------------

// returns the basis as a vector of words
VectorOf<Word> MalcevSet::getWords() const {

  VectorOf<Word> res( cardinality() );
  int cnt = 0;

  const BasicCommutators& BC = theCollector.commutators();

  for(int key = 1; key <= BC.theHirschNumber(); key++) {
    if( theSet.bound( Generator(key) ) ) {
      PolyWord pw = theSet.valueOf( Generator(key) );
      res[cnt++] = BC.wordForm().toWord(pw);
    }
  }
  return res;
}


//---------------------- getCommutatorWords -------------------

// returns the basis as a vector of commutator words
VectorOf<Word> MalcevSet::getCommutatorWords() const {

  VectorOf<Word> res( cardinality() );
  int cnt = 0;

  const BasicCommutators& BC = theCollector.commutators();

  for(int key = 1; key <= BC.theHirschNumber(); key++) {
    if( theSet.bound( Generator(key) ) ) {
      PolyWord pw = theSet.valueOf( Generator(key) );
      res[cnt++] = Word(pw);
    }
  }
  return res;
}


//---------------------- getPolyWords ----------------------------

// returns the basis as a vector of commutator words
VectorOf<PolyWord> MalcevSet::getPolyWords() const {

  VectorOf<PolyWord> res( cardinality() );
  int cnt = 0;

  const BasicCommutators& BC = theCollector.commutators();

  for(int key = 1; key <= BC.theHirschNumber(); key++) {
    if( theSet.bound( Generator(key) ) ) {
      PolyWord pw = theSet.valueOf( Generator(key) );
      res[cnt++] = pw;
    }
  }
  return res;
}


//-----------------------------------------------------
//   I/O
//-----------------------------------------------------


// ----------------------- printOn ----------------------------

void MalcevSet::printOn(ostream& s) const {

  const BasicCommutators& BC = theCollector.commutators();

  for(int key = 1; key <= BC.theHirschNumber(); key++) {
    if( theSet.bound( Generator(key) ) ) {
      PolyWord pw = theSet.valueOf( Generator(key) );
      s << BC.wordForm().asCommutatorWord(pw) << endl;
    }
  }
}


// ------------------ IPC routines --------------------------

// IPC output
ostream& operator < (ostream& s, const MalcevSet& b) {
  s < b.theCollector < b.isBasis < b.theSet.cardinality();

  QuickAssociationsIterator< Generator, PolyWord > iter(b.theSet);
  for( ; ! iter.done(); iter.next() ) {
    s < iter.key() < iter.value();
  }
  return s;
}

// IPC intput
istream& operator > (istream& s, MalcevSet& b) {
  s > b.theCollector > b.isBasis;

  int size;
  s > size;

  b.theSet = QuickAssociationsOf<Generator,PolyWord> ();
  for(int i=0; i<size; i++) {
    Generator key;
    PolyWord value;
    s > key > value;
    b.theSet.bind(key, value);
  }
  return s;
}


//
//  Some (temporary?!) hacks since gcc doesn't do templates right:
//

template <> int SetData< QuickAssociation<Generator,PolyWord> >::
hashElement( const QuickAssociation<Generator,PolyWord>& t) const
{ return t.hash(); }
