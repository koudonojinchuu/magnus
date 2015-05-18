// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:  
//            
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//
// TO DO:
//
//

#include "SubgroupOfOneRelatorGroup.h"
#include "QuickAssociations.h"
#include "HNNExtOfORGroup.h"
#include "MagnusBreakdown.h"
#include "CONDITION.h"
#include "conversions.h"
#include "Associations.h"
#include "SymmetricRelators.h"
#include "ShortenByRelators2.h"

#define DEBUG_ORGSUBGROUP

#ifdef DEBUG_ORGSUBGROUP
#  include "Margin.h"
#endif


ORGSubgroup::ORGSubgroup( const VectorOf<Word>& gens )
  : theGenerators( gens )
{ }


bool ORGSubgroup::isMagnusSubgroup( ) const
{
  const Word relator = parentGroup().relator();

  SetOf<Generator> relGens;
  for( int i = 0; i < relator.length(); ++i ) 
    relGens |= Generator(abs(ord(relator[i])));

  for( int i = theGenerators.length()-1; i >= 0; --i ) {
    const Word gen = theGenerators[i];

    if( gen.length() == 0 )
      continue;

    if( gen.length() > 1 )
      return false;
    
    relGens -= Generator(abs(ord(gen[0])));
  }

  if( relGens.cardinality() > 0 )
    return true;

  return false;
}

bool ORGSubgroup::lexicallyContains( const Word& w ) const 
{
  Word dummy;
  return lexicallyContains(w, dummy);
}
  
bool ORGSubgroup::lexicallyContains( const Word& w, Word& wInBasis ) const
{
  //  SetOf<Word> setOfGens = makeSetOf( theGenerators );

  // @dp implementation of this function is not finished:
  //       !!!  may required back tracing !!! 
  // @dp Is there algorithm to solve this problem in general ? 
  // @dp for word problem in 1-relator group this should be enough.

  /*Quick*/ AssociationsOf<Word, int> gens;
  for( int i = theGenerators.length()-1; i >= 0; --i )
    gens.bind( theGenerators[i], i+1 );

  bool found;
  Word u = w;
  wInBasis = Word();
  do {
    found = false;
    int uLen = u.length();
    int subwordLen;
    for( subwordLen = 1; !found && subwordLen <= uLen ; ++subwordLen ) {
      const Word subword = u.initialSegment( subwordLen );
      if( gens.bound(subword) ) {
	const Generator g = gens.valueOf(subword);
	wInBasis = wInBasis * g;
	found = true;
	break;
      }
      else if( gens.bound(subword.inverse()) ) {
	const Generator g = inv(Generator( gens.valueOf(subword.inverse()) ));
	wInBasis = wInBasis * g;
	found = true;
	break;
      }
    }
    if( found )
      u = u.subword( subwordLen, uLen );
  } while( found );
  
  if( u.length() > 0 ) {
    wInBasis = Word();
    return false;
  }

  return true;
}

Word ORGSubgroup::rewriteFromSubgroupToGroupGenerators( const Word& w ) const
{

#ifdef DEBUG_ORGSUBGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << " Request to recode S->G: " << endl
       << margin << " group = ";
  DEBUG_PRINT( cerr, parentGroup() );
  cerr << '(' << parentGroup().relator() << ')' << endl
       << margin << " subgroup = " << theGenerators << endl
       << margin << " word in S = " << w << endl;
#endif


  Word result;
  result = w.replaceGenerators( theGenerators );

#ifdef DEBUG_ORGSUBGROUP
  cerr << margin << " word in G = " << result << endl;
#endif

  return result;
}



void ORGSubgroup::write( ostream& ostr ) const
{
  ostr < theGenerators;
}

void ORGSubgroup::read( istream& istr )
{
  istr > theGenerators;
}

void ORGSubgroup::printOn( ostream& ostr ) const
{
  ostr << "gp(";
  parentGroup().printVectorOfWords(ostr, theGenerators);
  ostr << "; " << parentGroup() << ")";
}




SubgroupOfOneRelatorGroup::SubgroupOfOneRelatorGroup(
  const OneRelatorGroup& G, const VectorOf<Word>& gens)
  : ORGSubgroup(gens), theParentGroup(G), containsResult( )
{
}


void SubgroupOfOneRelatorGroup::setContainmentProblemData( 
  const Word& w, bool f, const Word& wInBasis, bool keepDetails, 
  const ProductOfRelatorConjugates& prodDeco ) const
{
  SubgroupOfOneRelatorGroup *This = (SubgroupOfOneRelatorGroup *)this;
  This->containsResult = 
    ContainmentProblemData( w, f, wInBasis, keepDetails, prodDeco );
}


// This is GENERAL PROBLEM.

static bool recodeGenerators( int oldGens, const Word& relator, 
			      const VectorOf<Word> subGens, const Word& w,
			      int& numOfNewGens, VectorOf<Word>& toNewGens, 
			      VectorOf<Word>& toOldGens )
{
  SetOf<Generator> allGens, RelGens, SubGens, WordGens;

  for( int i = relator.length() - 1; i >= 0; --i ) 
    RelGens |= Generator(abs(ord(relator[i])));
  for( int i = subGens.length() - 1; i >= 0; --i )
    SubGens |= Generator(abs(ord(subGens[i][0])));
  for( int i = w.length() - 1; i >= 0; --i )
    WordGens |= Generator(abs(ord(w[i])));

  allGens = SubGens | RelGens | WordGens;

  SetOf<Generator> nonUsedSubGens = SubGens - ( RelGens | WordGens );
  
  allGens -= nonUsedSubGens;
  
  if( allGens.cardinality() == oldGens )
    return false;

  toNewGens = VectorOf<Word>( oldGens );
  toOldGens = VectorOf<Word>( allGens.cardinality() );
  numOfNewGens = 0;
  for( int i = 1; i <= oldGens; ++i ) {
    if( allGens.contains( Generator(i) ) ) {
      toOldGens[numOfNewGens] = Generator(i);
      toNewGens[i-1] = Generator( ++numOfNewGens );
    }
  }

  return true;
}

bool SubgroupOfOneRelatorGroup::contains( const Word& w ) const
{
  const bool keepDetails = true;
  return contains(w, !keepDetails );
}

static int calculateShift( const Word& w, const Word& relator )
{
  Word u = w;
  for( int i = 0; i < u.length(); ++i ) {
    if( u == relator )
      return i;
    u = u.cyclicallyPermute(1);
  }

  return -1;
}

bool SubgroupOfOneRelatorGroup::contains( const Word& w, bool keepDetails ) 
  const
{

  // @dp IMPORTANT NOTE:
  // Now the algorithm is only implemented for subgroups defined 
  // by set of generators of an one-relator group.
  //
  // quick checking of the condition described above:
  for( int i = generators().length() - 1; i >= 0; --i ) {
    if( generators()[i].length() != 1 ) 
      error("Sorry, the general case for SubgroupOfOneRelatorGroup::contains()"
	    " is not implemented yet.");
  }

#ifdef DEBUG_ORGSUBGROUP
  Margin margin;
  margin.set( globalMargin );
#endif

  Word u = w.freelyReduce();
  const ProductOfRelatorConjugates emptyProdDeco;

  setContainmentProblemData( u, false, Word(), false, emptyProdDeco );

  // Is the word lexically contained in the subgroup? The super fast check.
  Word uInBasis;
  if( lexicallyContains(u, uInBasis) ) {
    setContainmentProblemData( u, true, uInBasis, true, emptyProdDeco );
    return true;
  }

  // Try to find a common piece of relator and the given word,
  // replace the with complement of the relator and check for lexically
  // containing in the subgroup.
  // The check is quick enough as I think.
  const SymmetricRelators symRels( theParentGroup.relator() );
  SymmetricRelatorsIterator symRelsIter( symRels );
  int relLen = theParentGroup.relator().length();
  for( int i = 0; i < u.length(); ++i ) {
    const Word z = u.subword( i, u.length() );
    for( symRelsIter.reset(); !symRelsIter.done(); symRelsIter.next() ) {
      const Word relator = symRelsIter.value();
      const int common = z.agreementLength( relator );
      if( common > 0 ) {
	const Word t = 
	  u.initialSegment(i) 
	  * relator.terminalSegment( relLen-common ).inverse()
	  * z.terminalSegment( z.length()-common );
	Word tBasis;
	if( lexicallyContains(t, tBasis) ) {
#ifdef DEBUG_ORGSUBGROUP
	  cerr << margin << "quick decomposition of " << u 
	       << " on " << tBasis << endl;
#endif

	  ProductOfRelatorConjugates prodDeco;
	  if( keepDetails ) {
	    RelatorConjugate conj;
	    int shift = calculateShift( relator, theParentGroup.relator() );
	    if( shift >= 0 )
	      ; // conj.relator = relator;
	    else {
	      shift = 
		calculateShift( relator.inverse(), theParentGroup.relator() );
	      // conj.relator = relator.inverse();
	    }
	    conj.relator = relator; //++

	    Word shift_conj = conj.relator.terminalSegment( shift );

	    conj.conjugator = 
	      ( shift_conj 
		* relator.terminalSegment( relLen-common ).inverse()
		* z.terminalSegment( z.length()-common )
	      ).freelyReduce();

	    cerr << "conj = " << conj << endl; 

	    prodDeco.append( conj );
	  }

	  setContainmentProblemData( u, true, tBasis, keepDetails, prodDeco );
	  return true;
	}
      }
    }
    if( !lexicallyContains( Word(z[0]) ) )
      break;
  }

  // strip out unused letters from the parent group.
  int numOfNewGens;
  VectorOf<Word> toNewGens, toOldGens;
  if( !recodeGenerators( theParentGroup.numberOfGenerators(),
			 theParentGroup.relator(), generators(), u, 
			 numOfNewGens, toNewGens, toOldGens ) ) {    

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " Solve PURE problem " << endl;
#endif

    return pureContainmentProblem( u, keepDetails );
  }
  else {

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " Solve STRIPPED problem " << endl;
#endif

    const Word newRelator = 
      theParentGroup.relator().replaceGenerators( toNewGens );
    const Word newTest = u.replaceGenerators( toNewGens );
    VectorOf<Word> newSubgroupGens;
    VectorOf<Word> toOldSubGens;
    for( int i = 0; i < generators().length() ; ++i ) {
      const Generator g = generators()[i][0];
      if( toNewGens[ ord(g)-1 ].length() > 0 ) {
	newSubgroupGens.append( toNewGens[ ord(g)-1 ] );
	toOldSubGens.append( Generator(i+1) );
      }
    }

    VectorOf<Chars> names;
    for( int i = theParentGroup.numberOfGenerators(); i > 0; --i ) {
      if( toNewGens[i-1].length() > 0 )
	names.prepend( theParentGroup.namesOfGenerators()[i-1] );
    }
    const OneRelatorGroup G( names, newRelator );
    const SubgroupOfOneRelatorGroup S(G, newSubgroupGens);

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " Remove nonused generators by " << toNewGens << endl
	 << margin << " old subgroup is " << "(" << theParentGroup.relator() 
	 << ')';
    DEBUG_PRINT( cerr, *this );
    cerr << endl << margin << " new subgroup is " << "(" << newRelator << ")";
    DEBUG_PRINT( cerr, S );
    cerr << endl << margin << " old word is ";
    DEBUG_PRINT_WORD( cerr, theParentGroup, u );
    cerr << ' ' << u << endl
	 << margin << " new word is ";
    DEBUG_PRINT_WORD(cerr, G, newTest );
    cerr << margin << ' ' << newTest << endl;
#endif
    
    const bool answer = S.pureContainmentProblem( newTest, keepDetails );

    if( answer ) {

      const Word inNewSubGens = 
	S.rewriteFromGroupToSubgroupGenerators(newTest);      
      const Word inOldSubGens = inNewSubGens.replaceGenerators( toOldSubGens);

#ifdef DEBUG_ORGSUBGROUP
      cerr << margin << "in new (stripped) generators" << inNewSubGens << endl
	   << margin << "in old (unstrip)  generators" << inOldSubGens << endl;
#endif

      ProductOfRelatorConjugates prodDeco;
      if( keepDetails ) {
	ProductOfRelatorConjugates oldProdDeco = 
	  S.getIdentityProductDecomposition(newTest);
	for( int i = oldProdDeco.length()-1; i >= 0; --i ) {
	  RelatorConjugate rc;
	  rc.relator = oldProdDeco[i].relator.replaceGenerators( toOldGens );
	  rc.conjugator = 
	    oldProdDeco[i].conjugator.replaceGenerators(toOldGens);
	  prodDeco.prepend( rc );
	}
#ifdef DEBUG_ORGSUBGROUP
	cerr << margin << "identity product decomosition is:" << endl
	     << margin << "in new (stripped) generators " << oldProdDeco <<endl
	     << margin << "in old (unstrip)  generators " << prodDeco << endl;
#endif
      }

      setContainmentProblemData( u,true,inOldSubGens,keepDetails,prodDeco );
    }
    return answer;
  }

}

MagnusBreakdown SubgroupOfOneRelatorGroup::buildMagnusBreakdown( 
  const Word& u, SetOf<Generator>& subGens ) const
{
  // form outside generators?
  for( int i = generators().length()-1; i >= 0; --i ) {
    subGens |= Generator(abs(ord(generators()[i][0]))); 
  }
  
  const Word relator = theParentGroup.relator();
  SetOf<Generator> relGens;
  for( int i = relator.length()-1; i >= 0; --i )
    relGens |= Generator(abs(ord(relator[i])));
  
  ListOf<Generator> outsideGens;
  for( int i = u.length()-1; i >= 0; --i ) {
    const Generator uGen = abs(ord(u[i]));
    if( !subGens.contains( uGen ) && !relGens.contains( uGen ) ) 
      outsideGens.append( uGen );
  }

  MagnusBreakdown M( theParentGroup );
  // Select a stable letter for HNN-extension.
  Generator stableGen;
  
  const SetOf<Generator> diff = subGens - relGens;
  if( diff.cardinality() > 0 ) {
    // case 1. inside the target subgroup and not appearing in the relator.
    SetIterator<Generator> I(diff);
    M.findHNNPresentation( I.value() );
  }
  else {
    bool stableFound = false;
    
    for( int i = 0; i < generators().length(); ++i ) {
      stableGen = generators()[i][0];
      if( M.getExponentSumOf( stableGen ) == 0 ) {
	// case 2. inside the target subgroup with zero exponent sum "
	//         in the relator.
	M.findHNNPresentation( stableGen );
	stableFound = true;
	break;
      }
    }

    if( !stableFound ) {
      if( M.getGeneratorsWithNonzeroExpSum().length() > 1 ) {
	// case 3. inside the target subgroup, with nonzero exponent sum in r,
	// and accompanying generator with nonzero exponent sum in r.
	Generator accompGen = M.getGeneratorsWithNonzeroExpSum()[0];
	if( stableGen == accompGen ) 
	  accompGen = M.getGeneratorsWithNonzeroExpSum()[1];
	M.findHNNPresentation( stableGen, accompGen );
      }
      else {
	// case 4. The target subgroup is generated by a single element, with
	// nonzero exponent sum in r, and every other generator of G has zero
	// exponent sum in r. Then choose t to be one of the latter generators.
	M.findHNNPresentation( M.getGeneratorsWithZeroExpSum()[0] );
      }
    }
  }	

#ifdef DEBUG_ORGSUBGROUP
  Margin margin;
  margin.set( globalMargin );
  
  cerr << margin << "the group is ";
  DEBUG_PRINT(cerr, theParentGroup);
  cerr << endl
       << margin << " all gens count = " 
       << theParentGroup.numberOfGenerators() << endl
       << margin << " relator = " << relator << endl
       << margin << " theSubGens = " << generators() << endl
       << margin << " test word = " << u << endl
       << margin << " subGens = " << makeVectorOf(subGens) << endl
       << margin << " relGens = " << makeVectorOf(relGens) << endl
       << margin << " outsideGens = " << outsideGens << endl
       << margin << " names = " << theParentGroup.namesOfGenerators() << endl;
#endif


  CONDITION( ERROR, 
	     outsideGens.length() + ( subGens | relGens ).cardinality() 
	     == theParentGroup.numberOfGenerators() 
	     ) ;
  
  return M;
}



bool SubgroupOfOneRelatorGroup::pureContainmentProblem( 
  const Word& u, bool keepDetails ) const 
{

  if( generators().length() == 0 ) {
    ProductOfRelatorConjugates prodDeco;
    const bool answer = theParentGroup.wordProblem( u, keepDetails, prodDeco );
    setContainmentProblemData( u, answer, Word(), keepDetails, prodDeco );
    return containsResult.inSubgroup;
  }
  
  Word uInBasis;
  const Word emptyWord;
  const ProductOfRelatorConjugates emptyProdDeco;
  setContainmentProblemData( u, false, emptyWord, false, emptyProdDeco );
  
  // every generator of the parent group appears in the relator, the test word
  // or subgroup generators.

  // The subgroup is not trivial.
  SetOf<Generator> subGens;
  MagnusBreakdown M = buildMagnusBreakdown( u, subGens );
  Generator stableGen = M.stableGenerator();
  HNNExtOfORGroup H = M.getHNNPresentation();
  OneRelatorGroup G0 = H.basisGroup();
  Word rewrittenTest =  M.rewriteWordInNewGenerators(
			   M.embeddingOfORGroups().imageOf(u).freelyReduce() 
			);

#ifdef DEBUG_ORGSUBGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << " new test word is ";
  DEBUG_PRINT_WORD(cerr, H.getPresentation(), rewrittenTest );
  cerr << ' ' << rewrittenTest << endl;
#endif

  if( !subGens.contains(stableGen) ) {
    CONDITION( ERROR, M.getExponentSumOf(stableGen) == 0 );
    // the subgroup is cyclic, generated by x_1 say.
    // find x_1 in relator r of G, conjugate r to start with x_1
    // rewritten relator is started with x_1,0 in G_0.
    // So if rewrittenWord in T then it belongs to the subgroup T of G_0
    // and length M of rewrittenWord is 0.
    // I.e. rewrittenWord must belong to subgroup T of G_0.

    // make new relator.
    const Word relator = theParentGroup.relator();
    int shift;
    for( shift = 0; shift < relator.length(); ++shift )
      if( abs(ord(relator[shift])) == stableGen ) {
	//@dp:!!!	relator.cyclicallyPermute( shift );
	break;
      }
    const Word newRelator = relator.cyclicallyPermute(shift);

    // make new presentation of the parent one-relator group.
    const OneRelatorGroup G( theParentGroup.namesOfGenerators(), 
			     newRelator 
			     );
    M = MagnusBreakdown( G );
    M.findHNNPresentation( stableGen );
    H = M.getHNNPresentation();

    rewrittenTest = M.rewriteWordInNewGenerators( 
		      M.embeddingOfORGroups().imageOf(u).freelyReduce() 
		    );

    ProductOfRelatorConjugates prodDeco;
    VectorOf<Word> deco = 
      H.reducedDecompositionOf( rewrittenTest, keepDetails, prodDeco );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " prodDeco in HNN gens:" << prodDeco << endl;
#endif

    prodDeco = liftUpProduct( prodDeco, M, G );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " prodDeco (lifted up) in OR gens:" << prodDeco << endl;
#endif

    const Word relatorInv = relator.inverse();
    const Word newRelatorInv = newRelator.inverse();
    const Word rConj = newRelator.terminalSegment( shift );
    const Word rConjInv = newRelator.inverse().terminalSegment( shift );
    for( int i = 0; i < prodDeco.length(); ++i ) {
      if( prodDeco[i].relator == newRelator ) {
	prodDeco[i] = 
	  RelatorConjugate( relator, 
			    (rConj.inverse() * prodDeco[i].conjugator)
			      .freelyReduce()
			  );
      }
      else if( prodDeco[i].relator == newRelatorInv ) {
	prodDeco[i] = 
	  RelatorConjugate( relatorInv, 
			    (rConjInv.inverse() * prodDeco[i].conjugator)
			      .freelyReduce()
			  );
      }
      else 
	error("Internal error in SubgroupOfOneRelatorGroup::"
	      "pureContainmentProblem: couldn't lift up relator conjugates "
	      "decomposition of the given word." );
    }

    if( deco.length() > 1 ) { 
      setContainmentProblemData( u, false, emptyWord, keepDetails, prodDeco );
    }
    else {
      G0 = H.basisGroup();
      VectorOf<Word> newSubgens( generators().length() );
      for( int i = generators().length()-1; i >= 0; --i ) {
	newSubgens[i] = 
	  M.rewriteWordInNewGenerators( 
	    M.embeddingOfORGroups().imageOf(generators()[i]).freelyReduce() 
	  );
      }
      const SubgroupOfOneRelatorGroup T( G0, newSubgens );
      const bool cont = T.contains( rewrittenTest );
      Word uInBasis;
      if( cont ) {
	
	if( keepDetails ) {
	  ProductOfRelatorConjugates someProdDeco = 
	    T.getIdentityProductDecomposition( rewrittenTest );
	  CONDITION( ERROR, someProdDeco.length() == 0 );
	}

	uInBasis = T.rewriteFromGroupToSubgroupGenerators( rewrittenTest );
	Word tmp  = T.rewriteFromSubgroupToGroupGenerators( uInBasis );
	tmp = M.rewriteWordInOldGenerators( tmp );
	if( !lexicallyContains( tmp, uInBasis ) )
	  error("Internal error 1 in SubgroupOfOneRelatorGroup::"
		"pureContainmentProblem()");
      }
      setContainmentProblemData( u, cont, uInBasis, keepDetails, prodDeco );
      return cont;
    }
  }
  else {
    // Subgroup contains stable generator.

    ProductOfRelatorConjugates prodDeco;
    VectorOf<Word> deco = 
      H.reducedDecompositionOf( rewrittenTest, keepDetails, prodDeco );
    
    if( keepDetails ) {
      prodDeco = liftUpProduct( prodDeco, M, theParentGroup );
    }

    const VectorOf<Range> subscriptsTable = M.getSubscriptsTable();
    ListOf<Word> t0Gens;
    for( int i = 0; i < generators().length(); ++i ) {
      const int gen = ord(generators()[i][0]);
      if( abs(gen) == abs(ord(stableGen)) ) 
	continue;

      CONDITION( WARNING, !subscriptsTable[ord(gen)].Default );

      for( int subscript = subscriptsTable[gen].low; 
	   subscript <= subscriptsTable[gen].high; 
	   ++subscript ) {
	t0Gens.append( M.adjoinSubscript(gen, subscript) );
      }
    }
    
    const SubgroupOfOneRelatorGroup T0(H.basisGroup(), makeVectorOf(t0Gens));

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " Subgroups decomposition. " << endl
	 << margin << " H = ";
    DEBUG_PRINT( cerr, H);
    cerr << endl
	 << margin << " A_H = " 
	 << H.subgroup(HNNExtOfORGroupGeneric::A).generators() << endl
	 << margin << " B_H = " 
	 << H.subgroup(HNNExtOfORGroupGeneric::B).generators() << endl
	 << margin << " T0  = " << T0.generators() << ", ";
    DEBUG_PRINT( cerr, T0);
    cerr << endl
	 << margin << " new relator is " << H.basisGroup().relator() << endl
	 << margin << " input deco = " << deco << endl << endl;
#endif

    typedef HNNExtOfORGroupGeneric::NumberOfSubgroup NumberOfSubgroup;
    
    Word wordInBasis, carry;
    const VectorOf<Word> gensOfA = 
      H.subgroup(HNNExtOfORGroupGeneric::A).generators();
    const VectorOf<Word> gensOfB = 
      H.subgroup(HNNExtOfORGroupGeneric::B).generators();

    for( int component = 0; component < deco.length()-1; component += 2 ) {

      const Word h0 = (carry * deco[component]).freelyReduce();
      Word t0, s;

#ifdef DEBUG_ORGSUBGROUP
      cerr << margin << " Trying decompose for:" << endl
	   << margin << "   carry = " << carry << endl
	   << margin << " deco[ " << component << " ] = " << deco[component] 
	   << endl
	   << margin << " h0 (carry*deco[" << component << "]) = " << h0 
	   << endl;

      CONDITION( WARNING, 
		 abs(ord(deco[component+1][0]))
		 == H.basisGroup().numberOfGenerators() + 1
	       );
#endif

      const Generator nextStableGen = deco[component+1][0];
      const NumberOfSubgroup S = ( ord(nextStableGen) < 0
				   ? HNNExtOfORGroupGeneric::A 
				   : HNNExtOfORGroupGeneric::B 
				   );
      Word h0basis;
      
      if( H.subgroup(S).lexicallyContains( h0, h0basis ) ) {
#ifdef DEBUG_ORGSUBGROUP
	cerr << margin << " !YES! h0 (" << component 
	     << ") is lexically contained by subgroup " << S << endl;
#endif
      }
      else {
	ProductOfRelatorConjugates joinProdDeco;
	if( !subgroupsDecomposition( 
	       h0, T0, (SubgroupOfOneRelatorGroup&)H.subgroup(S), 
	       t0, h0basis, keepDetails, joinProdDeco
	     ) 
	  )
	  return false;

	prodDeco = concatenate( joinProdDeco, prodDeco );
#ifdef DEBUG_ORGSUBGROUP
	cerr << margin << " !YES! extract word " 
	     << margin << nextStableGen << " * " << t0 << " of T0 " << endl;
#endif
      }

      carry = H.mappingFromSubgroup( S, h0basis );

      if( keepDetails ) { 
	// Write down relator conjugates' decomposition associated with
	// this rewriting process.
	VectorOf<Word> c = deco;
	c.shrink( component+2, c.length()-component-2 );
	const Word tail = compose( c );
	prodDeco = 
	  concatenate( H.mappingDecompositionOf(S, h0, h0basis, tail ), 
		       prodDeco 
		     );
      }
	
      wordInBasis = wordInBasis * T0.rewriteFromSubgroupToGroupGenerators(t0) 
                    * nextStableGen;

#ifdef DEBUG_ORGSUBGROUP
      cerr << margin << " wordInBasis is " << wordInBasis << endl << endl;
#endif
    }

    const Word h0 =  (carry * deco[deco.length()-1]).freelyReduce();

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " h0 (last) = " << h0 << endl;
#endif
    
    if( !T0.contains( h0 ) )
      return false;

    if( keepDetails ) {
      ProductOfRelatorConjugates someProdDeco = 
	T0.getIdentityProductDecomposition( rewrittenTest );
      CONDITION( ERROR, someProdDeco.length() == 0 );
    }

    const Word t0 = T0.rewriteFromGroupToSubgroupGenerators( h0 );
    wordInBasis = wordInBasis * T0.rewriteFromSubgroupToGroupGenerators( t0 );
    const Word tmp = M.rewriteWordInOldGenerators( wordInBasis );
    prodDeco = liftUpProduct( prodDeco, M, theParentGroup );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << " wordInBasis (in new gens) = " << wordInBasis << endl
	 << margin << " tmp (in old gens) = " << tmp << endl
	 << margin << " HNN -> OR (embedd) map: " 
	 << M.embeddingOfORGroups().inverse() << endl;
#endif

    if( !lexicallyContains( tmp, uInBasis ) )
      error( "Internal error 2 in SubgroupOfOneRelatorGroup::"
	     "pureContainmentProblem( )" );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin 
	 << " rewritten word in old subgroup generators is (uInBasis) " 
	 << uInBasis << endl
	 << margin << " product decomposition is " << prodDeco << endl;
#endif
    setContainmentProblemData( u, true, uInBasis, keepDetails, prodDeco );
  }
  
  return containsResult.inSubgroup;
  }

ProductOfRelatorConjugates 
SubgroupOfOneRelatorGroup::getIdentityProductDecomposition( const Word& w ) 
  const
{
  const bool keepDetails = true;
  if( w.freelyReduce() != containsResult.wordInParentGroupGenerators 
      || !containsResult.keepDetails )
    contains(w, keepDetails);

  CONDITION( ERROR, containsResult.keepDetails );
  return containsResult.idProductDeco;
}

Word SubgroupOfOneRelatorGroup::rewriteFromGroupToSubgroupGenerators( 
  const Word& w ) const
{
  const bool keepDetails = true;
  if( w.freelyReduce() != containsResult.wordInParentGroupGenerators )
    contains(w, !keepDetails);

  CONDITION( ERROR, containsResult.inSubgroup );
  return containsResult.wordInSubgroupGenerators;
}

SubgroupOfOneRelatorGroup 
SubgroupOfOneRelatorGroup::join( const SubgroupOfOneRelatorGroup& sg ) const
{
  if( theParentGroup != sg.theParentGroup )
    error("SubgroupOfOneRelatorGroup::join : Parent groups are different.");

  VectorOf<Word> joinGens( generators().length(), true, generators() );

  for( int j = 0; j < sg.generators().length(); ++j ) {
    const Generator g = sg.generators()[j][0];
    if( !contains( sg.generators()[j] ) ) {
      joinGens.append( sg.generators()[j] );
    }
  }

  return SubgroupOfOneRelatorGroup( theParentGroup, joinGens );
}

bool SubgroupOfOneRelatorGroup::subgroupsDecomposition( 
  const Word& w,
  const SubgroupOfOneRelatorGroup& T,
  const SubgroupOfOneRelatorGroup& A,
  Word& t_part, Word& a_part,
  const bool keepDetails, 
  ProductOfRelatorConjugates& prodDeco ) const
{

  t_part = a_part = Word();
  
  const SubgroupOfOneRelatorGroup TA = A.join(T);

  if( !TA.contains(w) )
    return false;

  if( keepDetails ) {
    prodDeco = TA.getIdentityProductDecomposition( w );
  }

#ifdef DEBUG_ORGSUBGROUP
  Margin margin;
  margin.set( globalMargin );
#endif

  const Word wInBasis = TA.rewriteFromGroupToSubgroupGenerators( w );

  AssociationsOf<Word, int> tBasis, aBasis;
  for( int i = 0; i < T.generators().length(); ++i ) {
    tBasis.bind( T.generators()[i], i+1 );
    tBasis.bind( T.generators()[i].inverse(), -(i+1) );
  }
  for( int i = 0; i < A.generators().length(); ++i ) {
    aBasis.bind( A.generators()[i], i+1 );
    aBasis.bind( A.generators()[i].inverse(), -(i+1) );
  }
  
  int left = 0;
  int wComponents = wInBasis.length();
  ListOf<Generator> t_partList;
  while( left < wComponents  
	 && tBasis.bound( TA.generators()[ abs(ord(wInBasis[left]))-1 ] ) ) {
    const int sgn = ( ord(wInBasis[left]) > 0 ? +1 : -1 );
    const Word gen = TA.generators()[ abs(ord(wInBasis[left]))-1 ];
    const Generator newgen = sgn * tBasis.valueOf( gen );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << "decomposed from (T)A " << gen << " to " 
	 << newgen << endl;
#endif

    t_partList.append( newgen );
    ++left;
  }

  int right = wComponents - 1;
  ListOf<Generator> a_partList;
  while( right >= left 
	 && aBasis.bound( TA.generators()[ abs(ord(wInBasis[right]))-1 ] ) ) {
    const int sgn = ( ord(wInBasis[right]) > 0 ? +1 : -1 );
    const Word gen = TA.generators()[ abs(ord(wInBasis[right]))-1 ];
    const Generator newgen = sgn * aBasis.valueOf( gen );

#ifdef DEBUG_ORGSUBGROUP
    cerr << margin << "decomposed from T(A) " << gen << " to " 
	 << newgen << endl;
#endif

    a_partList.append( newgen );    
    --right;
  }
  
  if( left <= right ) {
    a_part = t_part = Word();
    return false;
  }
  
  t_part = t_partList;
  a_part = a_partList;

  return true;
}

void SubgroupOfOneRelatorGroup::write( ostream& ostr ) const
{
  ORGSubgroup::write( ostr );
  ostr < theParentGroup;
}

void SubgroupOfOneRelatorGroup::read( istream& istr )
{
  ORGSubgroup::read( istr );
  istr > theParentGroup;
}


///            SubgroupOfORGroupWithTorsion


SubgroupOfORGroupWithTorsion::SubgroupOfORGroupWithTorsion( 
  const OneRelatorGroupWithTorsion& G, const VectorOf<Word>& gens)
  : ORGSubgroup(gens), theParentGroup(G)
{
}

bool SubgroupOfORGroupWithTorsion::contains( const Word& w ) const
{
  Word wInBasis;
  return contains( w, wInBasis );
}

Word SubgroupOfORGroupWithTorsion::rewriteFromGroupToSubgroupGenerators( 
  const Word& w ) const
{
  Word wInBasis;
  if( !contains(w, wInBasis) ) 
    error("Word does not belong the subgroup in "
	  "SubgroupOfORGroupWithTorsion::"
	  "rewriteFromGroupToSubgroupGenerators");

  return wInBasis;
}

void SubgroupOfORGroupWithTorsion::write( ostream& ostr ) const
{
  ORGSubgroup::write( ostr );
  ostr < theParentGroup;
}

void SubgroupOfORGroupWithTorsion::read( istream& istr )
{
  ORGSubgroup::read( istr );
  istr > theParentGroup;
}
  
bool SubgroupOfORGroupWithTorsion::contains( const Word& w, Word& wInBasis ) 
  const
{
  ShortenByRelators2 S( theParentGroup.relator() );

  const Word u = S.getShortenWord( w.freelyReduce() );

  if( lexicallyContains( u, wInBasis ) ) 
    return true;

  if( isMagnusSubgroup() )
    return false;

  return false; 
  //@dp:@dp change return type to Trichotomy, 
  //        the function must return `dontknow'.
}


