
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FPNilpotentGroupRep class.
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

#include "FPNilpotentGroupRep.h"

#include "NilpotentGroup.h"
#include "SGOfNilpotentGroup.h"
#include "PolyWordIterator.h"
#include <stdio.h>
#include "conversions.h"
#include "Presentation.h"
#include "GCD.h"
#include "AbelianSGPresentation.h"

//------------------------------------------------------------
//  Special helper stuff
//------------------------------------------------------------

const Type FPNilpotentGroupRep::theFPNilpotentGroupType =
Type( Type::unique() );

Type FPNilpotentGroupRep::type( ) { 
  return theFPNilpotentGroupType; 
}
// dominates FGGroupRep::type()

//----------------------------------------------------------------
//  Constructors and initializers
//----------------------------------------------------------------

// To construct a group with named generators, given nilpotency class
FPNilpotentGroupRep::
FPNilpotentGroupRep(const VectorOf<Chars>& gennames, 
		    int nilclass,
		    const VectorOf<Word>& rels,
		    enum NilpotentCollector::Type collectorType
		    )
  : NilpotentGroupRep(gennames),
    nilClass(nilclass),
    theCollector(gennames.length(), nilclass, collectorType),
    theRelators(rels),
    LCScomputed(false)
{  }


// initializes the group (computes LCS)
// ** Time consuming
void FPNilpotentGroupRep::initialize() const {
  computeLCSQuotients();
}


//-------------------------------------------------------------------
// 	Accessors
//-------------------------------------------------------------------


VectorOf<PolyWord> FPNilpotentGroupRep::basis() const {

  if( ! LCScomputed )
    error("FPNilpotentGroupRep::basis invoked before the basis is computed");

  return theBasis;
  
}
// basis of the group

VectorOf<Chars> FPNilpotentGroupRep::basisNames() const {

  if( ! LCScomputed )
    error("FPNilpotentGroupRep::basisNames invoked before the basis is computed");

  return theBasisNames;
  
}
// names of the basis words


// Returns the generators of the relation subgroup
VectorOf<Word> FPNilpotentGroupRep::relationSubgroupGenerators() const {

  MalcevSet ms(theRelators, theCollector);
  ms.makeNormalClosure();
  return ms.getWords();
}

//-------------------------------------------------------------------
// 	Methods dealing with the properties of the group:
//-------------------------------------------------------------------


int FPNilpotentGroupRep::order() const {

  if(! LCScomputed )
    error("FPNilpotentGroupRep::order invoked before the LCS is computed");

  if( !abelianization().isFinite()) return 0;
  int order = 1;

  for(int k = 1; k < LCS.length(); k++) {
    order *= LCS.constref(k).abelianization.orderOfTheTorsionSubgroup().as_long();
  }
  return order;  
}


Trichotomy FPNilpotentGroupRep::isTrivial( ) const {
  return abelianization().isTrivial();
}


Trichotomy FPNilpotentGroupRep::isFinite( ) const {
  return abelianization().isFinite();
}


Trichotomy FPNilpotentGroupRep::isInfinite( ) const { 
  return ! isFinite(); 
}


Trichotomy FPNilpotentGroupRep::isAbelian( ) const {
  return minimalNilpotencyClass() == 1;
}


Trichotomy FPNilpotentGroupRep::isFree( ) const { 
  if( theNumberOfGenerators < 2 ) return yes;
  else return no;
}


// returns the Hirsch number (the number of basic commutators of infinite order)
int FPNilpotentGroupRep::theHirschNumber() const {

  if( ! LCScomputed )
    error("FPNilpotentGroup::theHirschNumber() invoked before the LCS computed");

  return hirschNumber;
}


// returns true iff the group is free nilpotent.
// in this case also returns its rank and nilpotency class.
bool FPNilpotentGroupRep::isFreeNilpotent( int* freeRank, int* freeClass )  const {

  if( ! LCScomputed )
    error("FPNilpotentGroup::isFreeNilpotent invoked before the LCS computed");

  if( abelianization().orderOfTheTorsionSubgroup() != 1 )
    return false;

  if( abelianization().isTrivial()  ) {
    *freeRank = 0;
    *freeClass = 0;
    return true;
  }

  int supposedRank = abelianization().rankOfFreeAbelianFactor();
  BasicCommutators BC(supposedRank, nilClass);
    
  for(int k = 2; k <= nilClass; k++) {

    const AbelianGroup& Q = LCS.constref(k).abelianization;

    if( Q.isTrivial() ) {
      *freeRank = supposedRank;
      *freeClass = k - 1;
      return true;
    }

    if( Q.orderOfTheTorsionSubgroup() != 1 )
      return false;

    if( Q.rankOfFreeAbelianFactor() != BC.numberOfWeight(k) )
      return false;
  }    
  *freeRank = supposedRank;
  *freeClass = nilClass;
  return true;
}
  

int FPNilpotentGroupRep::minimalNilpotencyClass( ) const {

  if( ! LCScomputed )
    error("FPNilpotentGroup::minimalNilpotencyClass invoked before the group is initialized");

  for(int k = 1; k <= nilClass; k++) {
    if( LCS.constref(k).abelianization.isTrivial() ) return k - 1;
  }
  return nilClass;
}


// returns true iff the basis found in computeLCSQuotients() 
// is the Malcev basis
bool FPNilpotentGroupRep::isMalcevBasis() const {
  
  if(! LCScomputed )
    error("FPNilpotentGroup::isMalcevBasis invoked before the LCS is computed");

  return theHirschNumber() == theBasis.length();
}


// Returns order of the torsion subgroup.
Integer FPNilpotentGroupRep::orderOfTheTorsionSubgroup() const {

  if(! LCScomputed )
    error("FPNilpotentGroup::orderOfTheTorsionSubgroup invoked before the LCS is computed");
  Integer theOrder;
  const AbelianGroup& abelOfQuot1 = LCS.constref(1).abelianization;
  VectorOf<PolyWord> torsionSG(abelOfQuot1.invariants().length());
  // Decompose generators of torsion subgroup of 1st factor
  // in generators of the group
  VectorOf<AbelianWord> tmpNewToOld = abelOfQuot1.newToOldGens();
  for (int i=0;i<torsionSG.length();i++)
    torsionSG[i] = decomposeFromQuotientPresentation
      (tmpNewToOld[i+abelOfQuot1.rankOfFreeAbelianFactor()],1);
  theOrder = abelOfQuot1.orderOfTheTorsionSubgroup();
  
  for (int i=2;i<nilpotencyClass()+1;i++){
    const AbelianGroup& abelOfQuot = LCS.constref(i).abelianization;
    // Raise all generators of torsionSG to power = order of torsion
    // part in i-1 th factor and make presentation of this group
    // in i th quotient
    VectorOf<Word> tmpVec(torsionSG.length());
    for (int j=0;j<tmpVec.length();j++)
      tmpVec[j] = 
	decomposeInQuotientPresentation(torsionSG[j].raiseToPower
					(theOrder.as_long()),
					i);
    AbelianSGPresentation tmpAbPres = 
      abelOfQuot.makeSubgroupPresentation(tmpVec);
    // extract generators of torsion part of subgroup 
    tmpAbPres.computeCyclicDecomposition();
    tmpNewToOld = tmpAbPres.newToOldGens();
    VectorOf<AbelianWord> newTorsionPart(tmpAbPres.invariants().length());
    for (int j=0;i<newTorsionPart.length();j++){
      Word tmpWord =
	tmpNewToOld[j+tmpAbPres.rankOfFreeAbelianFactor()].getWord();
      VectorOf<Integer> tmpAbPowers = 
	abelOfQuot.oldInAbelianForm
	(tmpAbPres.fromSGPGensToSGGens(tmpWord)).getPowers();
      for (int k=0;k<tmpAbPowers.length();k++)
	tmpAbPowers[k] = tmpAbPowers[k]/theOrder;
      newTorsionPart[j] = AbelianWord(tmpAbPowers);
    }
    // extract generators of torsion subgroup of ith factor
    // and join them with `newTorsionPart` then torsionSG, generates
    // torsion subgroup of i th factor of the grou (if i=nilpotency clas
    // torsion subgroup of the group)
    torsionSG = VectorOf<PolyWord>(newTorsionPart.length()+
				   abelOfQuot.invariants().length());
    for (int j=0;j<newTorsionPart.length();j++)
      torsionSG[j] = decomposeFromQuotientPresentation(newTorsionPart[i],i);
    
    tmpNewToOld = abelOfQuot.newToOldGens();
    for (int j=newTorsionPart.length();j<torsionSG.length();j++)
      torsionSG[j] = decomposeFromQuotientPresentation
	(tmpNewToOld[j+abelOfQuot.rankOfFreeAbelianFactor()-
		    newTorsionPart.length()],i);
    
    theOrder *= abelOfQuot.orderOfTheTorsionSubgroup();
    
  }
  for (int i=0;i<torsionSG.length();i++){
    cout << endl;
    printWord(cout,torsionSG[i]);
  }
  cout << endl;
  return theOrder;
}


//-------------------------------------------------------------------
//  Methods dealing with group elements
//-------------------------------------------------------------------


Trichotomy FPNilpotentGroupRep::areEqual(const Elt& w1, const Elt& w2) const {
  Word W1 = (Word&)w1;
  Word W2 = (Word&)w2;
  Word W = W1 * W2.inverse();
  return wordProblem(W.freelyReduce());
}

Trichotomy FPNilpotentGroupRep::wordProblem(const Word& w) const {

  if(! LCScomputed )
    error("FPNilpotentGroup::wordProblem invoked before the LCS is computed");

  return weightOf(w) == nilClass + 1;
}


//------------------------------------------------------------
//  Conversions from one basis to another:
//------------------------------------------------------------


// decomposes the given word in terms of the group basis
// input: word in terms of basic commutators (not collected)
// output: word in terms of group basis.

PolyWord FPNilpotentGroupRep::decompose(const PolyWord& pw ) const {

  if(! LCScomputed )
    error("FPNilpotentGroupRep::decompose invoked before the LCS is computed");

  PolyWord rest = collector().collect(pw);
  PolyWord decomp;

  for(int k = 1; k < LCS.length(); k++) {

    //cout << "Decompose: rest = " << rest << " decomp = " << decomp << " k = " << k << endl;

    decomp = decomp * decomposeInQuotient(rest, k);
  }

  return decomp;
}

//input: word in terms of the group basis
//output: word in terms of the basic commutators
PolyWord FPNilpotentGroupRep::toCommutatorWord(const PolyWord& pw) const {
  return evaluateWord(pw, basis());
}


//-----------------------------------------------------------
//  Properties of the group elements:
//-----------------------------------------------------------


// determines in which term of the lower central series w lies
// nilclass + 1 if the word is trivial
int FPNilpotentGroupRep::weightOf( const Word& w) const {

  if(! LCScomputed )
    error("FPNilpotentGroupRep::order invoked before the LCS is computed");

  PolyWord rest = w;
  PolyWord decomp;

  for(int k = 1; k < LCS.length(); k++) {
    decomp = decomposeInQuotient(rest, k);
    if( ! decomp.isEmpty() ) return k;
  }
  return nilClass + 1;
}


//Returns order of the basic word corresponding to the generator g.
//0 means infinity.
int FPNilpotentGroupRep::orderOfBasic(Generator g) const {

  if( ! LCScomputed )
    error("FPNilpotentGroupRep::orderOfBasic: LCS not computed");
  
  return theBasisOrders.val( abs(g) - 1 );
}
  

//Returns the order of the word given in terms of group generators
int FPNilpotentGroupRep::orderOf(const Word& w) const {
  if( ! LCScomputed )
    error("FPNilpotentGroupRep::orderOfBasic: LCS not computed");
  Word tmpWord = w;
  Integer currentOrder = 1;
  for (int i=1;i<nilpotencyClass()+1;i++){
    const AbelianGroup& Q = LCS.constref(i).abelianization;
    Word wordInQ = decomposeInQuotientPresentation(tmpWord,i);
    Integer oderInAbGroup = Q.orderOfElt(wordInQ);
    if (oderInAbGroup == 0) 
      return 0;
    currentOrder *= oderInAbGroup;
    tmpWord = tmpWord.raiseToPower(oderInAbGroup);
  }
  return currentOrder.as_long();
}


//Returns the order of the word given in terms of group basis
int FPNilpotentGroupRep::orderOf(const PolyWord& pw) const {
  
  int answer = 1;

  ConstPolyWordIterator iter(pw);

  for(iter.startFromLeft(); ! iter.done(); iter.stepRight() ) {
    
    int orderOfGen = orderOfBasic(iter.thisLetter().gen);
    if(orderOfGen == 0) return 0;

    int orderOfComponent = orderOfGen / GCD( orderOfGen, 
				abs(iter.thisLetter().power) );

    answer = LCM( answer, orderOfComponent);

    //cout << "Order of: pw = " << pw << "  orderOfGen = " << orderOfGen
    //<< " orderOfComponent = " << orderOfComponent << "  answer = " << answer << endl;
  }

  return 0;
}


 
// finds maximal root of pw.
void FPNilpotentGroupRep::maximalRoot(const PolyWord& pw, 
				      PolyWord& root, int& power ) const {

/****************

  PolyWord theWord = decompose(pw);

  if( theWord.isEmpty() ) {
    root = PolyWord();
    power = 1;
    return;
  }

  // ----- do the first step
  // find the root in the class firstClass

  
  int firstClass = weightOfDecomposed(theWord);

  PolyWord restrictedWord = theWord;

  //mapToClass(restrictedWord, firstClass);
  //@ep mapToClass does not fit in well, unwind it

  PolyWordIterator iter(restrictedWord);
  
  for( iter.startFromLeft(); ! iter.done(); ) {
    const PolyWord& basisWord = theBasis.constref(ord( iter.thisLetter().gen) - 1); 
    if( commutators().weightOf( ord( basisWord.firstLetter().gen ) ) > firstClass )
      iter.removeThisLetter();
    else
      iter.stepRight();
  }

  int curPower;
  PolyWord curRoot;

  //zdes' nuzhno otobrazhat' v abelianizaciju.

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

***************/

}




//-------------------------------------------------------------------
//   I/O:
//-------------------------------------------------------------------

void FPNilpotentGroupRep::printBasis( ostream& s ) const {

  if( ! LCScomputed )
    error("FPNilpotentGroupRep::printLCSQuotients invoked before LCS computed");
  s << "Basic commutators of free preimage:"<<endl;
  commutators().print(s, theNamesOfGenerators);
  s << endl;
  int iGen = 0;

  for(int theWeight = 1; theWeight <= nilpotencyClass(); theWeight++) {
 
    // print banner

    s << "Generators of " << theWeight << ordinalPostfix(theWeight)
      << " quotient:" << endl << endl;

    // print generators

    VectorOf<Chars> commNames = commutators().commutatorNames();

    for( int i = 0; i < LCS.val(theWeight).generators.length(); i++ ) {

      s << theBasisNames.val(iGen) << " = "
	<< theBasis.val(iGen).toChars(commNames)
	<< endl;
      ++iGen;
    }

    s << endl << "The cyclic decomposition is " 
      << LCS.val(theWeight).abelianization << endl << endl;
  }


  s << "The orders: " << theBasisOrders;

}


//IPC tools

void FPNilpotentGroupRep::write( ostream& ostr ) const
{
  NilpotentGroupRep::write(ostr);
  ostr < nilClass < theCollector;
  ostr < theRelators;
  ostr < LCS;
  ostr < LCScomputed;
  ostr < hirschNumber;
  ostr < theBasis;
  ostr < theBasisNames;
  ostr < theBasisOrders;
}

void FPNilpotentGroupRep::read( istream& istr ) {
  NilpotentGroupRep::read(istr);

  /*  FPNilpotentGroupRep * This = (FPNilpotentGroupRep *) this;

  istr > This->nilClass > This->theCollector;
  istr > This->theRelators;
  istr > This->LCS;
  istr > This->LCScomputed;
  istr > This->hirschNumber;
  istr > This->theBasis;
  istr > This->theBasisNames;
  istr > This->theBasisOrders;
  */
  istr > nilClass > theCollector;
  istr > theRelators;
  istr > LCS;
  istr > LCScomputed;
  istr > hirschNumber;
  istr > theBasis;
  istr > theBasisNames;
  istr > theBasisOrders;
}
  


//-------------------------------------------------------------------
// Internal methods
//-------------------------------------------------------------------

void FPNilpotentGroupRep::computeLCSQuotients( ) const {

  if(LCScomputed) return;

  FPNilpotentGroupRep *This = (FPNilpotentGroupRep *)this;
  //to break physical constness

  int theBasisLength = 0;
  This->LCS = VectorOf<LCSQuotient> (nilClass + 1);

  // compute quotients

  for(int k = 1; k <= nilpotencyClass(); k++) {

    //cout << k << ".." << flush;

    This->computeQuotient(k);

    This->LCS.ref(k).numerationShift = theBasisLength;
    theBasisLength += LCS.constref(k).generators.length();
  }

  // form the basis

  This->hirschNumber = 0;

  This->theBasis = VectorOf<PolyWord> (theBasisLength);
  This->theBasisNames = VectorOf<Chars> (theBasisLength);
  This->theBasisOrders = VectorOf<int> (theBasisLength);

  int cur = 0;

  for(int k = 1; k <= nilpotencyClass(); k++) {
    LCSQuotient& Q = This->LCS.ref(k);

    for(int i = 0; i < Q.generators.length(); i++ ) {
      This->theBasis[cur] = Q.generators.val(i).theWord;
      Q.generators.ref(i).theGlobalNumber = cur;
      This->theBasisNames[cur] = Q.generators.val(i).theName;
      This->theBasisOrders[cur++] = Q.generators.val(i).theOrder;
    }

    This->hirschNumber += Q.infNumber;
  }

  This->LCScomputed = true;
}


void FPNilpotentGroupRep::computeQuotient( int theWeight ) {

  //cout << "making presentation group.." << flush;

  // make nilpotent group (the same but nilpotency class is restricted to theWeight)

  NilpotentGroup presGroup( theNamesOfGenerators, theWeight, theRelators );

  // make subgroup consisting of commutators of weight theWeight

  VectorOf<Word> gens;

  const BasicCommutators &BC = presGroup.commutators();

  for(int i = BC.theFirstOfWeight(theWeight); 
      i < BC.theFirstOfWeight(theWeight + 1); 
      i++) {
    gens.append( BC.wordForm().toWord(i) );
  }

  //cout << "making presentation subgroup.." << flush;

  SGOfNilpotentGroup presSubgroup( presGroup, gens );
  presSubgroup.initPreimage();

  LCSQuotient& Q = LCS.ref(theWeight);

  Q.basis = presSubgroup.preimageBasis();
  VectorOf<PolyWord> oldGens = Q.basis.getPolyWords();

  //cout << "The weight: " << theWeight << "  oldgens: " << oldGens << endl;
  
  // find FP presentation
  
  //cout << "finding FP presentation.." << flush;

  PresentationForSNG pres = presSubgroup.makePresentation();
  pres.build();

  //pres.print(cout);

  FPGroup presFP = pres.makeGroup();

  

  // abelianize it
  
  //cout << "making abelian presentation.." << flush;

  Q.abelianization = AbelianGroup(presFP);
  Q.abelianization.computeCyclicDecomposition();

  // form new generators

  Q.infNumber = Q.abelianization.rankOfFreeAbelianFactor();
  int numOfNewGens = Q.abelianization.newToOldGens().length();
  
  Q.generators = VectorOf<BasisWord>( numOfNewGens );


  int infCount = 1;
  int finCount = 1;

  for( int i = 1; i <= numOfNewGens; i++ ) {
    AbelianWord inNew( numOfNewGens, Word(Generator(i)) );
    Word inOld = Q.abelianization.newToOldGens(inNew).getWord();
    PolyWord inGlobal = collector().collect( evaluateWord( inOld, oldGens) );

    BasisWord& bw = Q.generators.ref(i-1);
    bw.theWord = inGlobal;
    bw.theWeight = theWeight;
    bw.theOrder = Q.abelianization.orderOfElt(inOld).as_long();
    bw.theLocalNumber = (bw.theOrder == 0) ? infCount++ : finCount++;
    //bw.theGlobalNumber is set within the caller (computeLCSQuotients)
    bw.theName = (bw.theOrder == 0) ? Chars("f(") : Chars("t(");
    bw.theName += Chars(theWeight) + "," + Chars(bw.theLocalNumber) + ")";
  }

  //Q.numerationShift is set within the caller (computeLCSQuotients)
}

LCSQuotient FPNilpotentGroupRep::getLCSQuotient( int N ) const
{
  if(!isInitialized()) 
    error("FPNilpotentGroupRep::getLCSQuotient invoked before the LCS is computed");
  
  return LCS[N];
}

// helper procedure for decompose
// Decomposes rest in LCS quotient k.
// Returns the decomposition; rest is reduced and prepared for the next quotient

PolyWord FPNilpotentGroupRep::decomposeInQuotient(PolyWord& rest, int k ) const {

  const LCSQuotient& Q = LCS.constref(k);

  // map the word to the quotient:

  //cout << "rest = " << rest << endl;

  theCollector.collectingProcess(rest);
  PolyWord restReduced = rest;
  mapToClass(restReduced, k);  // remove generators of weight > k

  //cout << "restReduced = " << restReduced << endl;

  // map the word into the presentation

  PolyWord presPolyWord;
    
  if( ! Q.basis.decomposeWord(restReduced, presPolyWord) ) {

    //cout << "Decompose: restReduced = " << restReduced << "  presPolyWord = "
    //<< presPolyWord << endl;

    error("FPNilpotentGroupRep::decompose: cannot map a word to presentation");
  }


  //cout << "The factor basis: " << endl;
  //Q.basis.printOn(cout);
  //cout << "*End." << endl;

  //cout << "presPolyWord = " << presPolyWord << endl;
  

  if(presPolyWord.isEmpty() ) return PolyWord();

  Word presWord = presPolyWord;

  // decompose the word in the abelianization

  AbelianWord oldInAbelian = Q.abelianization.oldInAbelianForm(presWord);
  AbelianWord newInAbelian = Q.abelianization.oldToNewGens(oldInAbelian);

  //cout << "oldInAbelian = " << oldInAbelian <<" newInAbelian = " << newInAbelian << endl;

  PolyWord decomp = newInAbelian;

  // decomp is written in terms of local basis. Rewrite.

  PolyWordIterator iter(decomp);

  for( iter.startFromLeft(); ! iter.done(); iter.stepRight() ) {
    Letter& p = iter.thisLetter();
    p.gen = ord(p.gen) + Q.numerationShift;
  }

  // reduce the rest
  //@ep this is evaluateWord() put here in explicit form.
  //The reason is that Q.generators is vector of BasisWords, not PolyWords.

  PolyWord reductor;
  Word w = newInAbelian.getWord();

  for(int i = 0; i < w.length(); i++) {

    int gen = ord(w[i]);

    if( gen > 0 )
      reductor.append( Q.generators.val(gen - 1).theWord );
    else
      reductor.append( Q.generators.val(-gen - 1).theWord.inverse() );
  }

  rest = reductor.inverse() * rest;

  return decomp;
}


const AbelianGroup& FPNilpotentGroupRep::abelianization() const {

  if( ! LCScomputed )
    error("FPNilpotentGroupRep::abelianization invoked before the LCS is computed");
  
  return LCS.constref(1).abelianization;
}
// Returns an abelianization of the group ( LCS[1] )



// Returns the weight of the word decomposed in terms of group basis.
int FPNilpotentGroupRep::weightOfDecomposed( const PolyWord& pw) const {
  int firstComponent = ord( pw.firstLetter().gen ) - 1;
  const PolyWord& bw = theBasis.constref(firstComponent);
  return commutators().weightOf( ord( bw.firstLetter().gen ) );
}

Trichotomy FPNilpotentGroupRep::conjugacyProblem( const Word& u, 
						const Word& v ) const
{
  if (!isInitialized())
    error("Trichotomy NilpotentGroupRep::conjugacyProblem( const Word& u," 
	  "const Word& v ) const : Group was not initialized!");
  Word conjugator;
  Word h = u; Word g = v;
  // If words are not equal in first quotient, then they are not conjugate
  if (!LCS.constref(1).abelianization.areEqual(u,v) ) return no;
  
  for (int weight=2;weight<=nilpotencyClass();weight++){
    // Generators of centralizer of ith quotient 
    VectorOf<Word> centralizerGens(3);

    // z - it's a new part in equality h = g^f(weight-1)*z, which appears
    // when we moved to the current quotient. 
    Word z = conjugator.inverse()*g.inverse()*conjugator*h;
    // Conjugator in current quotient f = f(weight-1)*w^-1
    //where w can be represent as w1^a1 w2^a2 ... wn^an, where
    // w1 ... wn are generators of centralazer of weight-1 quotient ???????
    // so we have to solve such equation in weight-th LCS quotient :
    // z =  [h,w1]^a1 ... [h,wn]^an
    //    AbelianWord leftPart = decomposeInQuotientAbelianization(z,weight);
    VectorOf<AbelianWord> rightPart(centralizerGens.length());
    for (int i = 0;i<rightPart.length();i++){
      Word commut = h*centralizerGens[i]*h.inverse()*centralizerGens[i].inverse();
      //      rightPart[i] = decomposeInQuotientAbelianization(commut,weight);
    }

    VectorOf<int> solutionKoeff(3); 
    //  VectorOf<int> solutionKoeff = 
    //                    solveEquationInAbelianGroup(leftPart,rightPart);
    for (int i = 0;i<rightPart.length();i++)
      conjugator = conjugator * centralizerGens[i].raiseToPower(solutionKoeff[i]);
  }
  //  return conjugator;
}

VectorOf<Word> FPNilpotentGroupRep::centralizer( const Word& w) const{
  if (!isInitialized())
    error("Trichotomy NilpotentGroupRep::conjugacyProblem( const Word& u," 
	  "const Word& v ) const : Group was not initialized!");

  VectorOf<Word> centraliserOfQuotient(theNumberOfGenerators);
  for (int i = 0;i<theNumberOfGenerators;i++)
    centraliserOfQuotient[i] = Generator(i+1);

  for (int weight=2;weight<=nilpotencyClass();weight++){

    AbelianWord leftPart;
    VectorOf<AbelianWord> rightPart(centraliserOfQuotient.length());
    for (int i = 0;i<rightPart.length();i++){
      Word commut = w*centraliserOfQuotient[i]*
	w.inverse()*centraliserOfQuotient[i].inverse();
      //      rightPart[i] = decomposeInQuotientAbelianization(commut,weight);
    }
    VectorOf<int> solutionKoeff; 
   //  VectorOf<int> solutionKoeff = 
    //                    solveEquationInAbelianGroup(leftPart,rightPart);
    // centraliserOfQuotient = something;
  }
  return centraliserOfQuotient;
}

Word FPNilpotentGroupRep::
decomposeInQuotientPresentation(const Word& w, int k ) const {
  PolyWord theWord(w);
  theCollector.collectingProcess(theWord);
 
  PolyWord restReduced = w;
  mapToClass(restReduced, k);  // remove generators of weight > k

  PolyWord presPolyWord;
  const LCSQuotient& Q = LCS.constref(k);
    
  if( ! Q.basis.decomposeWord(restReduced, presPolyWord) ) {
    error("FPNilpotentGroupRep::decomposeInQuotientAbelianization: cannot map a word to presentation");
  }

  if(presPolyWord.isEmpty() ) return Word();

  Word presWord = presPolyWord;

  // decompose the word in the abelianization

  //  AbelianWord oldInAbelian = Q.abelianization.oldInAbelianForm(presWord);
  //  AbelianWord newInAbelian = Q.abelianization.oldToNewGens(oldInAbelian);
  return presWord;
}

PolyWord FPNilpotentGroupRep::
decomposeFromQuotientPresentation(const PolyWord& w, int k ) const {
  const LCSQuotient& Q = LCS.constref(k);
  VectorOf<Word> tmpBasis = Q.basis.getWords(); 
  VectorOf<PolyWord> basisGens(tmpBasis.length());
  for (int i = 0;i<basisGens.length();i++)
    basisGens[i] = tmpBasis[i];
  return evaluateWord(w, basisGens);
}
