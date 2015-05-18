// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the AmalgamatedProductWithOneRelatorRep
//           (APwithOneRelatorRep) class.
//           The name is abbreviated to fit in a library.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: draft
//
// Revision History:
//
//   02-10-96: the preliminary release.
//   04-07-96: maximalRoot is rewritten to fix some bugs. Comments added.
//
// Discussion:
//
// Bugs:
//
//
// Special Notes:
//

#include "APwithOneRelatorRep.h"
#include "AmalgamatedProductParser.h"
#include "Associations.h"
#include "APwithOneRelator.h"

//#define DEBUG_APORelator
//#define DEBUG_APORelator_conj

inline VectorOf<Word> createVectorOfWords(const Word& w)
{
  VectorOf<Word> result(1);
  result[0] = w;
  return result;
}

APwithOneRelatorRep::APwithOneRelatorRep(const FreeGroup& g1,
					 const FreeGroup& g2,
					 const Word& gen1,
					 const Word& gen2 )
: AmalgProductOfFreeGroupsRep(g1, g2, createVectorOfWords(gen1),
			      createVectorOfWords(gen2))
{ }


//
// Methods dealing with the properties of the group:
//

// Determine whether the product is a hyperbolic group
// (only when the amalgamating relation is a(x)^k = b(y)^s and k,s > 1).

Trichotomy APwithOneRelatorRep::isHyperbolic() const
{
  Word dummyRoot;
  int power1, power2;
  maximalRootInFreeGroup(assocWord(0), dummyRoot, power1);
  maximalRootInFreeGroup(assocWord(1), dummyRoot, power2);
  if( power1 > 1  ||  power2 > 1 )
    return no;
  return yes;
}


VectorOf<int> APwithOneRelatorRep::computeInvariants() const
{
  // for ONE relator amalgamated products only!

  int m1 = factor[0].rank();
  int m2 = factor[1].rank();
  bool allZeros = true;

  Word relator = SetIterator<Word>(relators).value();
  VectorOf<int> exponents = exponentSum( relator );
  int gcd = 0;
  for(int i = 0; i < m1+m2 && gcd != 1; ++i)
    gcd = GCD2(gcd, exponents[i]);

  if( gcd == 0 ) return VectorOf<int>(0);

  VectorOf<int> res(1);
  res[0] = gcd;
  return res;

}


//
// Methods dealing with group elements:
//


//----------- compute the maximal root of the given word ------

// This is a large procedure divided into three parts, maximalRoot,
// maximalRoot_case1 and maximalRoot_case2. Two latter ones are
// intended for internal use only!
// There is also static function rewriteVectorAsWord.

// We suppose that amalgamating relation is a(x)^k = b(y)^s

void
APwithOneRelatorRep::maximalRoot(const Word& w, Word& root, int& power) const
{
  Word wFreeReduced = w.freelyReduce();

  if( wFreeReduced.length() == 0 ) {
    root = wFreeReduced;
    power = 1;
    return;
  }

  // For further computing, we need the word to be cyclically reduced.
  // We can do it via conjugating by some word uConj; u is the reduced
  // form.

  Word uConj;
  VectorOf<Word> uDeco;

  cyclicDecomposition(wFreeReduced, uDeco, uConj);

  Word u = compose(uDeco);

  if( uDeco.length() < 2 )
    // a case with word from just one (free) factor.
    maximalRootInFreeGroup(u, root, power);
  else {
    // Find k and s, the powers of a(x) and b(y)
    Word aRoot, bRoot;   // a(x) , b(y)
    int aPower, bPower;  // k, s

    maximalRootInFreeGroup(assocWord(0), aRoot, aPower);
    maximalRootInFreeGroup(assocWord(1), bRoot, bPower);
    bRoot = localToGlobal( LocalWord(bRoot, RIGHT_FACTOR) );

    Word aRootInv = aRoot.inverse();
    Word bRootInv = bRoot.inverse();

    // check whether u is a combination of a(x) and b(y)

    VectorOf<int> uPowers(uDeco.length());
    bool inH = true;
    Word pieceRoot;
    int piecePower;

    // all components in decomposition must be powers of aRoot
    // or bRoot
    for(int i = 0; i < uDeco.length(); ++i) {
      maximalRootInFreeGroup(uDeco[i], pieceRoot, piecePower);
      if( pieceRoot != aRoot && pieceRoot != bRoot ) {
	if( pieceRoot == aRootInv || pieceRoot == bRootInv )
	  piecePower = -piecePower;
	else {
	  inH = false;
	  break;
	}
      }
      uPowers[i] = piecePower;
    }

    if( inH ) {
      // Case 2  (the numbering is historical :)

      // rewriting u as a combination of formal generators
      // representing a(x) and b(y)

      // construct AP group for this case
      FreeGroup Fa(1);
      FreeGroup Fb(1);
      VectorOf<Word> Ra(1);
      VectorOf<Word> Rb(1);
      Ra[0] = Fa.raiseToPower(Word(Generator(1)), aPower);
      Rb[0] = Fb.raiseToPower(Word(Generator(1)), bPower);

      APwithOneRelatorRep Gab( Fa, Fb, Ra, Rb );
      // constuct only *rep-level* object, not true AP-group.

#ifdef DEBUG_APORelator
      cerr << "Fa is " << Fa << ", Ra is " << Ra << endl
	   << "Fb is " << Fb << ", Rb is " << Rb << endl
	   << "rep-level object: " << APwithOneRelator(Fa,Fb,Ra,Rb) << endl;
#endif

      // rewriting the word
      Word uInGab;
      Word mult;
      for(int i = 0; i < uDeco.length(); ++i) {
	Generator gen = 1+factorOfFormalWord(uDeco[i]);
	uInGab *= raiseToPower(Word(gen), uPowers[i]);
      }

      // find the root using Case 2
      Word uRoot;
      Gab.maximalRoot_case2(uInGab, uRoot, power);

      // inverse transformation
      VectorOf<Word> images(2);
      images[0] = aRoot;
      images[1] = bRoot; /*localToGlobal( LocalWord(bRoot, RIGHT_FACTOR) );*/
      root = uRoot.replaceGenerators(images);
    }
    else {
      // Solve maximal root problem as for hyperbolic group
      // (Case 1).
      maximalRoot_case1(u, root, power);
    }
  }

  //don't forget about cyclical decomposition
  root = Word(root.conjugateBy(uConj)).freelyReduce();
}

// Case 1: a(x) = b(y), a and b are root words (and the group is
// huperbolic torsion free),
//         OR
// the given word cannot be expressed in terms of a(x) and b(y)
//
//@ep VNR says the algorithm is the same for both these cases.
//    This is to be carefully proved.

void APwithOneRelatorRep::maximalRoot_case1(const Word& w, Word& root, 
  int& power) const
{
  VectorOf<Word> normalDeco = normalDecomposition(w);

  int aFactor = factorOfElement(normalDeco[0]);
  // what is the factor of the first component

  int decoLen = normalDeco.length();
  // decomposition length (number of components)
  // the word is already cyclically reduced, so the length
  // is even (the case of decoLen==1 proceeded by the caller)

  // Let the normal decomposition be like this:
  // a^k * s_1 * t_1 * ... * s_n * t_n, where t_n = rep1^-1. Then
  // t_n belongs to the subgroup generated by bword, and s_n
  // to the one generated by aWord.

  Word init = normalDeco[0];
  LocalWord s_1 = rightSchreierRepresentativeOf(globalToLocal(init));
  normalDeco[0] = localToGlobal(s_1);
  Word ak = (init * Word(normalDeco[0]).inverse()).freelyReduce();


#ifdef DEBUG_APORelator
  cerr << "normalDeco[0] = "; 
  printWord(cerr, (Word)normalDeco[0]); cerr << endl;
  cerr << "ak = "; printWord(cerr, ak); cerr << endl;
  cerr << "aFactor = " << aFactor << endl
       << "assocSubgroup[aFactor] = "<<(SGofFreeGroup)assocSubgroup[aFactor] << endl;
#endif

  int k = assocSubgroup[aFactor].inNielsenWords(globalToLocal(ak)).length();

  Word aWord = assocWord(aFactor);
  Word bWord = assocWord(1-aFactor);

  int maxPowerOfFirstElt = 2*(k+1)+1;
  VectorOf<Word> aReps(maxPowerOfFirstElt);
  VectorOf<Word> bReps(maxPowerOfFirstElt);
  for(int repCount = 0; repCount < maxPowerOfFirstElt; ++repCount) {
    // maps repCount into powerOfFirstElt as follows:
    //    0  1  2  3  4 ...
    //    0 -1  1 -2  2 ...
    int powerOfFirstElt = ( repCount & 1 ? -((repCount+1)>>1) : repCount >> 1);
    aReps[repCount] = raiseToPower(aWord, powerOfFirstElt);
    bReps[repCount] = raiseToPower(bWord, powerOfFirstElt);
  }

  // the length of the root should be even and divide decoLen

  for(int rootLen = 2; rootLen < decoLen; rootLen += 2) {
    if( decoLen % rootLen != 0 )
      continue;

#ifdef DEBUG_APORelator
    cerr << "trying length of root = " << rootLen << endl;
#endif

    int rootPower = decoLen / rootLen;
    VectorOf<Word> rootNormalDeco = normalDeco;
    rootNormalDeco.shrink(decoLen-rootLen, rootLen);
    Word rootNormalForm = compose(rootNormalDeco);

    for(int p = 0; p < maxPowerOfFirstElt; ++p) {
      Word rootWord = (aReps.ref(p) * rootNormalForm).freelyReduce();

      // checking the pretender
      if( areEqual(w,raiseToPower(rootWord,rootPower)) == yes ) {
	root = rootWord;
	power = rootPower;
#ifdef DEBUG_APORelator
	cerr << "root found: ";
	printWord(cerr, root);
	cerr << " with power " << power << endl
	     << "leaving algorithm 1 for maximal root" << endl;
#endif
	return;
      }
    }
  }
  // nothing is found -- this is a root word
  root = w;
  power = 1;

#ifdef DEBUG_APORelator
  cerr << "the word is prime." << endl
       << "leaving algorithm 1 for maximal root" << endl;
#endif
}


// Case 2: group <x,y | x^k = y^s >

static Word rewriteVectorAsWord(const VectorOf<Word>& vec);

void APwithOneRelatorRep::maximalRoot_case2(const Word& w, Word& root, 
  int& power) const
{
  // find normal decomposition
  VectorOf<Word> normalDeco = normalDecomposition(w);
  int decoLength = normalDeco.length();

  // the first element of decomposition is x^kt * x^p
  // p != 0, otherwise w is a power of a(x) and proceeded by the caller

  Word init = normalDeco[0];
  int initPower = init.length();

  NumberOfFactor whatFactor = factorOfElement(init);

  // letter x
  Generator initRootGen = init[0];

  LocalWord firstElement = rightSchreierRepresentativeOf(globalToLocal(init));
  // x^p

  normalDeco[0] = localToGlobal(firstElement);
  Word subgroupElement = (init * Word(normalDeco[0]).inverse()).freelyReduce();

  int abskt = subgroupElement.length();
  // absolute value of kt

  // Now searching for maximal root of the remainder.

  Word wrd = rewriteVectorAsWord(normalDeco);

#ifdef DEBUG_APORelator
  cerr << "case2: rewrite vector: ";
  printVectorOfWords(cerr, normalDeco);
  cerr << "rewritten word is " << wrd << endl;
#endif

  int len = wrd.length();
  int halfWrd = len / 2;
  for( int rootLen = 1; rootLen <= halfWrd; ++rootLen) {
    if ( len % rootLen == 0 ) {
      power = len / rootLen;
      if( abskt % power == 0 ) {
	// check by raising
	root = wrd.initialSegment(rootLen);
	if( raiseToPower(root, power) == wrd) {
	  normalDeco.shrink(rootLen);
	  root = (init.initialSegment(abskt / power) *
		  compose(normalDeco)).freelyReduce();
	  return;
	}
      }
    }
  }
  // nothing was found -- the root word
  power = 1;
  root = w;
}

// Rewrites given vector of words replacing words with formal letters.
// The same words are replaced with the same letters, and vice versa.

static Word rewriteVectorAsWord(const VectorOf<Word>& vec)
{
  Word result;
  AssociationsOf<Word,Generator> indices;
  Word tmpWord;
  Generator gen;
  int last_index = 0;
  for(int i = 0; i < vec.length(); i++) {
    Word tmpWord = vec[i];
    if( indices.bound(tmpWord) )
      gen = indices.valueOf(tmpWord);
    else {
      ++last_index;
      indices.bind(tmpWord, Generator(last_index));
      gen = Generator(last_index);
    }
    result *= gen;
  }
  return result;
}


VectorOf<Word> APwithOneRelatorRep::centralizerOf(const Word& w) const
{
  Word u = w.freelyReduce();

  if( u.length() == 0 ) {

    // for identity word, the centralizer is the whole group

    int subgroupGens = theNumberOfGenerators;
    VectorOf<Word> answer(subgroupGens);
    for(int i=0; i<subgroupGens; i++)
      answer[i] = Word(Generator(i+1));
    return answer;
  }

  // Find k and s, the powers of a(x) and b(y)
  Word aRoot, bRoot;   // a(x) , b(y)
  Word aRootInv, bRootInv;
  int aPower, bPower;  // k, s

  maximalRootInFreeGroup(assocWord(0), aRoot, aPower);
  maximalRootInFreeGroup(assocWord(1), bRoot, bPower);
  bRoot = localToGlobal( LocalWord(bRoot, RIGHT_FACTOR) );

  aRootInv = aRoot.inverse();
  bRootInv = bRoot.inverse();

  // maximal root of u always commutes with u

  int uPower;
  Word uRoot;
  maximalRoot(u, uRoot, uPower);
  VectorOf<Word> answer(1);
  answer[0] = uRoot;

  // Check whether u is a combination of a(x) and b(y).
  // If so, then u also commutes with a(x).

  VectorOf<Word> uDeco;
  Word uConj;
  cyclicDecomposition(u, uDeco, uConj);

  // all components in decomposition must be powers of aRoot
  // or bRoot
  for(int i = 0; i < uDeco.length(); ++i) {
    Word pieceRoot;
    int piecePower;
    maximalRootInFreeGroup(uDeco[i], pieceRoot, piecePower);
    if( pieceRoot != aRoot && pieceRoot != bRoot &&
       pieceRoot != aRootInv && pieceRoot != bRootInv )
      return answer; // no more centralizer generators
  }

  // yes, u is a combination of a(x) and b(y).

  uRoot = Word(uRoot.conjugateBy(uConj.inverse())).freelyReduce();
  if( uRoot != aRoot && uRoot != bRoot && uRoot != aRootInv &&
     uRoot != bRootInv )
    answer.append( Word(assocWord(0).conjugateBy(uConj)).freelyReduce());
  // if uRoot generates the amalgamated subgroup <a(x)> then
  // there's no need to include a(x) to the set of generators

  return answer;
}


// This is a special function to adjust maximal root.
// Changes root to min(root, root^-1), adjusting power.

static void adjustRoot(Word& root, int& power) {
  Word rootInv = root.inverse();
  if( rootInv < root ) {
    root = rootInv;
    power = -power;
  }
}


GroupRep*
APwithOneRelatorRep::readFrom(istream& istr, Chars& errMesg) const
{
  AmalgamatedProductParser P(istr);
  return P.parseAPwithOneRelator(errMesg);
}



// --------------------------------------------------------------------//
//                                                                     //
//                         Conjugacy problem                           //
//                                                                     //
// --------------------------------------------------------------------//


// Here goes set of methods solving conjugacy problem in
// G = < F(X) * F(Y) | a(x) = b(y).
//
// The algorithm was provided by V.N.Remeslennikov. It rests on a
// paper of S.Lipshutz "Generalization of Dehn's result on the
// conjugacy problem", Proc.Amer.Math.Soc. 17(1966), 759-762).


// Special helper method for solving conjugacy problem.
// If amalgamated words a(x) and b(y) can be cyclically reduced
// such that a = aRed^aConj and b = bRed^bConj, then we can switch
// from the current presentation to another one, using isomorphism:
//
// s -> s^aConjInv if s belongs to F(X),
// s -> s^bConjInv if s belongs to F(Y).
//
// This method translates the given word w to this "reduced"
// presentation, using given aConjInv and bConjInv. It is also
// used to perform inverse translation.

Word APwithOneRelatorRep::translateToAnotherPresentation(const Word& w,
  const Word& aConjInv, const Word& bConjInv) const
{
  Word res;
  Word aConj = aConjInv.inverse();
  Word bConj = bConjInv.inverse();

  int len = w.length();
  for(int i = 0; i < len; i++) {
    if(factorOfGenerator(w[i]) == LEFT_FACTOR)
      res *= aConjInv * Word(w[i]) * aConj;
    else res *= bConjInv * Word(w[i]) * bConj;
  }
  return res.freelyReduce();
}

// The main method for solving conjugacy problem. The other are
// helper ones and never should be called directly by user.

Trichotomy APwithOneRelatorRep::conjugacyProblem( const Word& u, const Word& v,
  Word& conjugator ) const
{
  // Determine whether a(x) and b(y) can be cyclically reduced.

  Word aConj;
  Word bConj;
  Word aRed = cyclicallyReduce(assocWord(0), aConj);
  Word bRed = cyclicallyReduce(assocWord(1), bConj);

  // If amalgamated words a(x) and b(y) can be cyclically reduced
  // such that a = aRed^aConj and b = bRed^bConj, then we can switch
  // from the current presentation to another one, using isomorphism:
  //
  // s -> s^aConjInv if s belongs to F(X),
  // s -> s^bConjInv if s belongs to F(Y).

  // Translating u, v.

  Word u1 = translateToAnotherPresentation(u, aConj.inverse(),bConj.inverse());
  Word v1 = translateToAnotherPresentation(v, aConj.inverse(),bConj.inverse());

  // Make new presentation (rep-level object).
  APwithOneRelatorRep AP1(factor[0], factor[1], aRed, bRed);

#ifdef DEBUG_APORelator_conj
  cerr << "\nReady to enter reduced case.";
  cerr << "\naRed = ";  printWord(cerr, aRed);
  cerr << "\nbRed = ";  printWord(cerr, bRed);
  cerr << "\naConj = ";  printWord(cerr, aConj);
  cerr << "\nbConj = ";  printWord(cerr, bConj);
  cerr << "\nu1 = ";  printWord(cerr, u1);
  cerr << "\nv1 = ";  printWord(cerr, v1);
  cerr << "\nEntering reduced case.\n";
#endif

  // Solve conjugacy problem in this presentation.
  Trichotomy answer = AP1.conjugacyProblem_reduced(u1, v1, conjugator);

#ifdef DEBUG_APORelator_conj
  cerr << "\nLeaving reduced case." << endl << "\nReduced conjugator = ";  
  printWord(cerr, conjugator);
#endif

  if( answer == yes)
    conjugator = translateToAnotherPresentation(conjugator, aConj, bConj);
  else
    conjugator = Word();

#ifdef DEBUG_APORelator_conj
  cerr << "\nTrue conjugator = ";  printWord(cerr, conjugator);
  cerr << endl;
#endif

  return answer;
}

// Conjugacy problem for special case: amalgamated words are cyclically
// reduced.

Trichotomy APwithOneRelatorRep::conjugacyProblem_reduced( const Word& u,
  const Word& v, Word& conjugator ) const
{
  // Find cyclic decomposition of u and v.

  conjugator = Word();

  VectorOf<Word> uDec, vDec;
  Word uConj, vConj;

  cyclicDecomposition(u, uDec, uConj);
  cyclicDecomposition(v, vDec, vConj);

  Word uRed = compose(uDec);
  Word vRed = compose(vDec);

  Trichotomy answer = dontknow;

  if(uDec.length() != vDec.length() ) {
    answer = no;
  }
  // The two cases depending on words length.
  else {

#ifdef DEBUG_APORelator_conj
    cerr << "\nReduced case: ready to analyze decompositions.";
    cerr << "\nuDec = ";  printVectorOfWords(cerr, uDec);
    cerr << "\nvDec = ";  printVectorOfWords(cerr, vDec);
    cerr << "\nuConj = ";  printWord(cerr, uConj);
    cerr << "\nvConj = ";  printWord(cerr, vConj);
    cerr << endl;
#endif
    if(uDec.length() == 0) {
      answer = yes;
    }
    else if(uDec.length() == 1) {
      answer = conjugacyProblem_case1(uRed, vRed, conjugator);
    }
    else {
      answer = conjugacyProblem_case2(uDec, vDec, conjugator);
    }
  }

  if( answer == yes) {

#ifdef DEBUG_APORelator_conj
    cerr << "\nReduced case: conjugator found.";
    cerr << "\nRaw conjugator = ";  printWord(cerr, conjugator);
    cerr << endl;
#endif

    conjugator =  (uConj.inverse() * conjugator * vConj).freelyReduce();
  }
  return answer;
}


// Conjugacy problem for special case: amalgamated words are cyclically
// reduced, and every of u, v belongs to a factor

Trichotomy APwithOneRelatorRep::conjugacyProblem_case1( const Word& u,
  const Word& v, Word& conjugator ) const
{
  LocalWord uLoc = globalToLocal(u);
  LocalWord vLoc = globalToLocal(v);

#ifdef DEBUG_APORelator_conj
  cerr << "\nEntering case 1.";
  cerr << "\nuLoc = "; printWord(cerr, uLoc.theWord);
  cerr << "\nvLoc = "; printWord(cerr, vLoc.theWord);
  cerr << endl;
#endif

  if( uLoc.theFactor == vLoc.theFactor) {
    // case 1.1: both elements belong to the same factor

    Trichotomy answer = ::conjugacyProblem(factor[uLoc.theFactor], uLoc.theWord,
					   vLoc.theWord, conjugator);
#ifdef DEBUG_APORelator_conj
    cerr << "\nCase 1.1.";
    cerr << "\nLocal conjugator ="; printWord(cerr, conjugator);
    cerr << endl;
#endif

    conjugator = localToGlobal(conjugator, uLoc.theFactor);
    return answer;
  }
  else {
    // case 1.2

    // Cyclically permute words in uLoc.theWord, trying to
    // get a word from the amalgamated subgroup.

#ifdef DEBUG_APORelator_conj
    cerr << "\nCase 1.2.";
    cerr << endl;
#endif

    int uLocLen = uLoc.theWord.length();

    for(int i = 0; i < uLocLen; i++) {
      Word perm = uLoc.theWord.cyclicallyPermute(i);  // local word
      if(isElementOfSubgroup(LocalWord(perm, uLoc.theFactor))) {
	
#ifdef DEBUG_APORelator_conj
	cerr << "\nWord perm = ";
	printWord(cerr, perm);
	cerr << " is element of subgroup." << endl;
#endif

	// Searching for m.

	Word dummy;
	int permPower;
	maximalRootInFreeGroup(perm, dummy, permPower);
	adjustRoot(dummy, permPower);	

	Word aWord = assocWord(uLoc.theFactor);
	int aPower;
	maximalRootInFreeGroup(aWord, dummy, aPower);
	adjustRoot(dummy, aPower);

	int m = permPower / aPower;

	Word bWord = assocWord(vLoc.theFactor);
	Word bPow = raiseToPower(bWord, m);

	Word vLocConj;
	Trichotomy answer = ::conjugacyProblem(factor[vLoc.theFactor],
					       bPow, vLoc.theWord, vLocConj);
	//@dp invokes fixup of conjugacyProblem for FreeGroup.

#ifdef DEBUG_APORelator_conj
	cerr << "\nm = " << m << "\nSolving conjugacy problem for words bPow = ";
	printWord(cerr, bPow);
	cerr << "   vLoc = ";
	printWord(cerr, vLoc.theWord);
	cerr << "\nLocal conjugator is ";
	printWord(cerr, vLocConj);
	cerr << endl;
#endif

	if(answer == yes) {
	  // Find the word mapping uLoc to perm by conjugation
	  Word uLocConj = uLoc.theWord.initialSegment(i);
	  conjugator = localToGlobal(uLocConj, uLoc.theFactor);
	  vLocConj = localToGlobal(vLocConj, vLoc.theFactor);
	  conjugator = (conjugator * vLocConj).freelyReduce();

#ifdef DEBUG_APORelator_conj
	  cerr << "\nCase 1.2: conjugator found." << "\nConjugator = ";
	  printWord(cerr, conjugator);
	  cerr << endl;
#endif

	}
	return answer;
      }
    }
    return no;
  }
}

// Conjugacy problem for special case: amalgamated words are cyclically
// reduced, and length of u is greater than 1.

Trichotomy APwithOneRelatorRep::conjugacyProblem_case2(
  VectorOf<Word>& uDec, VectorOf<Word>& vDec, Word& conjugator ) const
{
  conjugator = Word();
  int start = 0;

  if( factorOfFormalWord(uDec[0]) != factorOfFormalWord(vDec[0]) )
    start = 1;

#ifdef DEBUG_APORelator_conj
  cerr << "Entering case 2. start =" << start << endl;
#endif

  // Pre-compute maximal roots of associated words.

  VectorOf<Word> assocRoot(2);        // local coding
  int assocPower[2];

  Word root;
  maximalRootInFreeGroup(assocWord(0), root, assocPower[0]);
  adjustRoot(root, assocPower[0]);	
  assocRoot[0] = root;
  maximalRootInFreeGroup(assocWord(1), root, assocPower[1]);
  adjustRoot(root, assocPower[1]);
  assocRoot[1] = root;

  Trichotomy answer = dontknow;

  int decLen = uDec.length();

#ifdef DEBUG_APORelator_conj
  cerr << "\nMaximal roots of a and b.";
  cerr << "\naRoot = ";  printWord(cerr, assocRoot[0]);
  cerr << "\nbRoot = ";  printWord(cerr, assocRoot[1]);
  cerr << "\naPow = " << assocPower[0] << "  bPow = " << assocPower[1] << endl;
#endif

  for(int i = start; i < decLen; i += 2) {
    VectorOf<Word> vPerm = cyclicallyPermute(vDec, i);

#ifdef DEBUG_APORelator_conj
    cerr << "\nComparing decompositions.";
    cerr << "\nuDec = ";  printVectorOfWords(cerr, uDec);
    cerr << "\nvPerm = ";  printVectorOfWords(cerr, vPerm);
    cerr << endl;
#endif

    // Check whether uDec and vPerm are conjugated by element of
    // amalgamated subgroup.

    // Checking component-wise, left to right.

    for(int j = 0; j < decLen; j++) {

      NumberOfFactor fact = factorOfFormalWord(uDec[j]);
      Word aWord = assocWord(fact);

      Word uComp = globalToLocal(uDec[j]).theWord;
      Word vComp = globalToLocal(vPerm[j]).theWord;

#ifdef DEBUG_APORelator_conj
      cerr << "\n Trying local components uDec[" << j << "] = ";
      printWord(cerr, uComp);
      cerr << ", vDec[" << j << "] = ";
      printWord(cerr, vComp);
      cerr << endl;
#endif

      Word comm = commutator(aWord, vComp);

      if( comm.length() > 0 ) {
	// Case 2.1
	// The answer is yes or no (and the loop is canceled)

#ifdef DEBUG_APORelator_conj
	cerr << "\nCase 2.1.\n";
#endif

	int mMax = uComp.length() + vComp.length() + 1;

	Word uCompInv = uComp.inverse();
	for(int m = -mMax; m <= mMax; m++) {

#ifdef DEBUG_APORelator_conj
	  cerr << "\n Trying m = " << m;
#endif

	  Word aPower = raiseToPower(aWord, m); // local coding
	  Word prod = (uCompInv * aPower * vComp).freelyReduce();


#ifdef DEBUG_APORelator_conj
	  cerr << "\naPower = ";  printWord(cerr, aPower);
	  cerr << "\nprod = ";  printWord(cerr, prod);
#endif


	  if( isElementOfSubgroup(LocalWord(prod, fact)) ) {
	    // Check whether m satisfies


	    VectorOf<Word> rest = uDec;
	    rest.shrink(j, decLen-j);
	    Word uRest = compose(rest);

	    rest = vPerm;
	    rest.shrink(j, decLen-j);
	    Word vPermRest = compose(rest);

	    aPower = localToGlobal(aPower, fact);

#ifdef DEBUG_APORelator_conj
	    cerr << "\nChecking aPower to be conjugator.";
	    cerr << "\naPower = ";  printWord(cerr, aPower);
	    cerr << "\nuRest = "; printWord(cerr, uRest);
	    cerr << "\nvPermRest = "; printWord(cerr, vPermRest);
#endif

	    uRest = uRest.conjugateBy(aPower);
	    if(areEqual(uRest, vPermRest)) {
	      answer = yes;
	      conjugator = aPower;
	      break;
	    }
	  }
	}
	if(answer != yes) answer = no;
	break;
      }
      else {
	// Case 2.2

#ifdef DEBUG_APORelator_conj
	cerr << "\nCase 2.2.\n";
#endif


	Word root;          // local coding
	int  vPower;
	maximalRootInFreeGroup(vComp, root, vPower);
	adjustRoot(root, vPower);

	// if SAFETY .....
	if( root != (Word)assocRoot[fact] ) {
	  error("Internal error in APwithOneRelatorRep::ConjugacyProblem_case2.");
	}

	int uPower;
	maximalRootInFreeGroup(uComp, root, uPower);
	adjustRoot(root, uPower);

	int q = vPower - uPower;

	if( q % assocPower[fact] != 0 || root != (Word)assocRoot[fact] ) {
	  answer = no;
	  break;
	}

	// "Reduce" j-th components

	q /= assocPower[fact];				

	// If this is the last component, then the words are conjugated
	// iff q == 0.

	if(j == decLen-1) {
	  answer = ( q == 0 );
	  break;
	}

	Word rest = raiseToPower(assocWord(1-fact), q);  // local word
	rest = localToGlobal(rest, NumberOfFactor(1-fact));


#ifdef DEBUG_APORelator_conj
	cerr << "\nReducing this component with rest = ";
	printWord(cerr, rest);
#endif				

	vPerm[j+1] = ( rest * vPerm[j+1] ).freelyReduce();
	answer = dontknow;
      } // end of Case 2.2

      if(answer != dontknow) break;
    }

    // If answer == yes then search is completed.
    // If answer == no then make next iteration.

    if(answer == yes) {
      // Now conjugator maps uDec to vPerm.

      VectorOf<Word> vConjDec = vDec;
      vConjDec.shrink(i, decLen-i);
      Word vConj = compose(vConjDec);
      conjugator = (conjugator * vConj).freelyReduce();

#ifdef DEBUG_APORelator_conj
      cerr << "\nFound conjugator.";
      cerr << "\nconjugator = ";  printWord(cerr, conjugator);
#endif

      return answer;
    }
  }
  return no;
}

//
//     End of conjugacy problem.
//

const Type APwithOneRelatorRep::theAPwithOneRelatorType = Type::unique();

