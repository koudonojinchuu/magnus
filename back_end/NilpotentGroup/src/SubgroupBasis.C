
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SubgroupBasis class.
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

#include "SubgroupBasis.h"
#include "PolyWordIterator.h"
#include "GCD.h"

//================================================================
//------------------- SubgroupBasis ------------------------------
//================================================================


//-----------------------------------------------------
//   Constructors / initializers
//-----------------------------------------------------


SubgroupBasis::SubgroupBasis(const VectorOf<Word>& v, const NilpotentGroup& ng)
  : theParent(ng), theGenerators(v), basisIsFull(false), isNormal(dontknow)
{ }


void SubgroupBasis::initialize() const {     //Logical const!

  theParent.initialize();

  SubgroupBasis *This = (SubgroupBasis *)this;

  // add words to the basis set

  for(int i = 0; i < theGenerators.length(); i++)
    This->addWord( theGenerators.val(i) );

  //make the set to be full

  This->makeFull();

  //compute the Hirsch number

  This->hirschNumber = 0;

  QuickAssociationsIterator< Generator, PolyWord > iter(theSet);
  for( ; ! iter.done(); iter.next() ) {
    PolyWord pw = iter.value();
    if( theParent.orderOf(pw) == 0 )
      ++This->hirschNumber;
  }
}



//-----------------------------------------------------
//   Helper methods
//-----------------------------------------------------


//---------- operations in parent group ---------------

//Everywhere it is assumed that arguments and return value
//are expressed in terms of the parent basis

// multiplies two words
PolyWord SubgroupBasis::
parentMultiply(const PolyWord& pw1, const PolyWord& pw2) const {

  //cout << "Multiply: " << pw1 << "   " << pw2 << endl; 

  PolyWord bcpw1 = toBasicCommutators(pw1);
  PolyWord bcpw2 = toBasicCommutators(pw2);
  PolyWord bcres = theParent.collector().multiply(bcpw1, bcpw2);
  return toParentBasis(bcres);
}


// inverts the word
PolyWord SubgroupBasis::parentInvert(const PolyWord& pw) const {
  PolyWord bcpw = toBasicCommutators(pw);
  PolyWord bcres = theParent.collector().inverse(bcpw);
  return toParentBasis(bcres);
}

// computes pw^power
PolyWord SubgroupBasis::parentRaiseToPower(const PolyWord& pw, int power) const {
  PolyWord bcpw = toBasicCommutators(pw);
  PolyWord bcres = theParent.collector().raiseToPower(bcpw, power);
  return toParentBasis(bcres);
}

// makes a commutator of two words
PolyWord SubgroupBasis::
parentCommute(const PolyWord& pw1, const PolyWord& pw2) const {
  PolyWord bcpw1 = toBasicCommutators(pw1);
  PolyWord bcpw2 = toBasicCommutators(pw2);
  PolyWord bcres = bcpw1.inverse() * bcpw2.inverse() * bcpw1 * bcpw2;
  return toParentBasis(bcres);
}

// returns the order of the word, 0 means infinity
int SubgroupBasis::leaderOrder(const PolyWord& pw) const {
  return theParent.orderOfBasic(leader(pw));
}


//The argument is expressed in terms of basic commutators.
//Result is in terms of parent basis.
PolyWord SubgroupBasis::toParentBasis(const PolyWord& pw) const {
  return theParent.decompose(pw);
}


//The argument is expressed in terms of parent basis.
//Result is in terms of basic commutators.
PolyWord SubgroupBasis::toBasicCommutators(const PolyWord& pw) const {
  PolyWord res = theParent.toCommutatorWord(pw);
  return theParent.collector().collect(res);
}


//--------------------- reduceWords ---------------------------

// Reduces two words
// After reduction the leading term of pw1 is in minimal power,
// the leading term of pw2 is greater.
// Returns true iff pw1 was changed
// pw1 and pw2 are expressed in terms of the parent basis

bool SubgroupBasis::reduceWords(PolyWord& pw1, PolyWord& pw2) const {

  if(pw1.isEmpty() || pw2.isEmpty())
    error("SubgroupBasis::reduceWords: empty argument");

  bool firstChanged = false;

  int power1 = absPower(pw1);
  int power2 = absPower(pw2);

  // make both PolyWords to be of distinct signs

  if( sign(pw1) ^ sign(pw2) == 0)  // if they have the same sign
    pw2 = parentInvert(pw2);

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
    pw2 = parentMultiply(pw1, pw2);
 
  } while(power2 != 0);

  return firstChanged;
}  


//------------------ checkMembership ----------------------------------

void SubgroupBasis::checkMembership(PolyWord& remainder) const {

  while( ! remainder.isEmpty() && theSet.bound( leader(remainder) ) ) {
    PolyWord curWord = theSet.valueOf( leader(remainder) );
    if( absPower(remainder) % absPower(curWord) != 0 ) 
      break;

    // The rest can be reduced. Do it.
    reduceWords( curWord, remainder );
  }
}


//----------------------- addWord ---------------------------------

// adds w to the set
// returns true iff the set was changed
bool SubgroupBasis::addWord( const Word& w ) {

  PolyWord remainder = theParent.decompose(w);
  checkMembership(remainder);
  return addWord(remainder);
}

// adds w to the set
// returns true iff the set was changed
bool SubgroupBasis::addWord( const PolyWord& pw ) {

  bool wasChanged = false;
  PolyWord remainder = pw;

  while( ! remainder.isEmpty() ) {
    Generator key = leader(remainder);

    // if there is no word with the same leader, include the remainder to the set

    if( ! theSet.bound(key) ) {
      theSet.bind( key, remainder );
      basisIsFull = false;
      isNormal = dontknow;
      return true;
    }

    // reduce the remainder

    PolyWord currentWord = theSet.valueOf(key);

    if( reduceWords( currentWord, remainder) ) {
      theSet.bind( key, currentWord );
      basisIsFull = false;
      isNormal = dontknow;
      wasChanged = true;
    }
  }
  return wasChanged;
}


//-------------------- makeFull ----------------------------------

void SubgroupBasis::makeFull() {

  if( basisIsFull ) return;

  int nilClass = theParent.nilpotencyClass();


  // add all commutators [Wj, Wi]

  int upper_i =  theParent.basis().length();

  for(int i = 2; i < upper_i; i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord Wi = theSet.valueOf( Generator(i) );

    // if leader(Wi) is of finite order, add Wi^p

    int orderWi = leaderOrder(Wi);
    
    if(orderWi != 0) {

      int powerWi = power(Wi);

      while( orderWi % powerWi != 0) {

	int p, q;
	GCD(powerWi, orderWi, p, q);
	PolyWord Wip = parentRaiseToPower(Wi, p);
	addWord(Wip);
      
	Wi = theSet.valueOf( Generator(i) );
	powerWi = power(Wi);
      }
      
      addWord(parentRaiseToPower(Wi, orderWi / powerWi));

    }

    // add commutators

    for(int j = 1; j < i; j++) {

      if( ! theSet.bound( Generator(j) ) ) continue;

      PolyWord Wj = theSet.valueOf( Generator(j) );

      PolyWord comm = parentCommute(Wj, Wi);
      addWord(comm);
    }

    // change the sign

    if(power(Wi) < 0) {
      Wi = parentInvert(Wi);
      theSet.bind(leader(Wi), Wi);
    }
  }
  basisIsFull = true;
}


//-------------------- makeNormalClosure ----------------------------------

void SubgroupBasis::makeNormalClosure() {

  // @am it's look like basis has to be initialized first, but
  // Evgeniy did not notice this, may be he forgot.
  if( !basisIsFull ) 
    error("void SubgroupBasis::makeNormalClosure() : Basis was not initialized");

  if( isNormal == yes ) return;

  int nilClass = theParent.nilpotencyClass();

  int upper_i =  theParent.basis().length();

  for(int i = 1; i < upper_i; i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord Wi = theSet.valueOf( Generator(i) );

    // if leader(Wi) is of finite order, add Wi^p

    int orderWi = leaderOrder(Wi);
    
    if(orderWi != 0) {

      int powerWi = power(Wi);

      while( orderWi % powerWi != 0) {

	int p, q;
	GCD(powerWi, orderWi, p, q);
	PolyWord Wip = parentRaiseToPower(Wi, p);
	addWord(Wip);
      
	Wi = theSet.valueOf( Generator(i) );
	powerWi = power(Wi);
      }
      
      addWord(parentRaiseToPower(Wi, orderWi / powerWi));

    }

    // trying generators of the group

    for(int j = 1; j <= upper_i; j++) {

      Generator g(j); 
      PolyWord comm = parentCommute(Wi, Letter(g,1) ); 
      addWord(comm);
    }

    // add commutators

    for(int j = 1; j < i; j++) {

      if( ! theSet.bound( Generator(j) ) ) continue;

      PolyWord Wj = theSet.valueOf( Generator(j) );

      PolyWord comm = parentCommute(Wj, Wi);
      addWord(comm);
    }

    // change the sign

    if(power(Wi) < 0) {
      Wi = parentInvert(Wi);
      theSet.bind(leader(Wi), Wi);
    }
  }
  basisIsFull = true;
  isNormal = yes;
}


//-----------------------------------------------------
//   Accessors
//-----------------------------------------------------

//---------------- theHirschNumber --------------------

int SubgroupBasis::theHirschNumber() const {

  if( ! basisIsFull )
    error("SubgroupBasis::theHirschNumber: the set must be full");

  return hirschNumber;
}


//------------------ isNormalClosure ------------------

bool SubgroupBasis::isNormalClosure() const {

  if( isNormal != dontknow ) return isNormal;

  if( ! basisIsFull )
    error("Attempt to use SubgroupBasis::isNormalClosure before the set is full.");

  SubgroupBasis* This = (SubgroupBasis *)this;    // to break physical constness

  int nilClass = theParent.nilpotencyClass();

  int upper_i =  theParent.basis().length();

  //the subgroup is normal iff any w^x is in the set

  for(int i = 1; i < upper_i; i++) {

    if( ! theSet.bound( Generator(i) ) ) continue;
    PolyWord W = theSet.valueOf( Generator(i) );
    
    for(int j = 1; j <= upper_i; j++) {

      PolyWord conj = parentMultiply(Letter(j, -1), W);
      conj = parentMultiply(conj, Letter(j, 1) );

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

//-------------------- normalClosure ------------------

SubgroupBasis SubgroupBasis::normalClosure() const {
  SubgroupBasis ncl = *this;
  ncl.makeNormalClosure();
  return ncl;
}

//-----------------------------------------------------
//   Word containment
//-----------------------------------------------------


//------------------ contains -------------------------

// returns true iff the word is a member of the set
// Slow

bool SubgroupBasis::contains(const Word& w) const {
  PolyWord pw = theParent.decompose(w);
  checkMembership(pw);
  return pw.isEmpty();
}


//------------------ decomposeWord --------------------

// determines whether w is an element of the subgroup
// if yes, returns its decomposition in terms of the subgroup basis
// w is given in terms of the parent group

bool SubgroupBasis::decomposeWord(const PolyWord& w, PolyWord& decomp) const {

  if( ! basisIsFull )
    error("Attempt to use SubgroupBasis::decomposeWord before the set is full.");

  PolyWord remainder = w;
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
      divisor = parentInvert(divisor);
    }

    for(int i = 0; i < divPower; i++) {
      remainder = parentMultiply(divisor, remainder);
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

  // now translate decomp to the subgroup basis terms: replace
  // gen with index of basis word having leader gen

  int curElement = 0;   // index of current basis element

  PolyWordIterator iter(decomp);
  iter.startFromLeft();

  int numgen = theParent.basis().length();

  for( int c = 1; c <= numgen; c++ ) {
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
//   Conversions to other presentations
//-----------------------------------------------------


//---------------------- asWords ---------------------

// returns the basis as a vector of words in terms of
//the group generators
VectorOf<Word> SubgroupBasis::asWords() const {

  if( ! basisIsFull )
    error("SubgroupBasis::asWords: basis must be initialized.");

  VectorOf<Word> res( cardinality() );
  int cnt = 0;

  int numgen = theParent.basis().length();

  for(int key = 1; key <= numgen; key++) {
    if( theSet.bound( Generator(key) ) ) {
      PolyWord pw = theSet.valueOf( Generator(key) ); //in the group basis
      pw = theParent.toCommutatorWord(pw);  // in terms of basic commutators
      res[cnt++] = theParent.commutators().wordForm().toWord(pw);
    }
  }
  return res;
}


//----------------- asGroupWords -----------------------

// returns the basis as a vector of PolyWords in terms of the group basis
VectorOf<PolyWord> SubgroupBasis::asGroupWords() const {

  if( ! basisIsFull )
    error("SubgroupBasis::asGroupWords: basis must be initialized.");

  VectorOf<PolyWord> res( cardinality() );
  int cnt = 0;

  int numgen = theParent.basis().length();

  for(int key = 1; key <= numgen; key++) {
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


// IPC output
ostream& operator < (ostream& s, const SubgroupBasis& b) {
  s < b.theParent < b.theGenerators < b.basisIsFull 
    < b.isNormal < b.hirschNumber;

  s < b.theSet.cardinality();

  QuickAssociationsIterator< Generator, PolyWord > iter(b.theSet);
  for( ; ! iter.done(); iter.next() ) {
    s < iter.key() < iter.value();
  }
  
  return s;
}

// IPC intput
istream& operator > (istream& s, const SubgroupBasis& b) {

  SubgroupBasis& sb = (SubgroupBasis&)b;

  s > sb.theParent > sb.theGenerators > sb.basisIsFull
    > sb.isNormal > sb.hirschNumber;

  int size;
  s > size;

  sb.theSet = QuickAssociationsOf<Generator,PolyWord> ();
  for(int i = 0; i < size; i++) {
    Generator key;
    PolyWord value;
    s > key > value;
    sb.theSet.bind(key, value);
  }
  return s;
}

