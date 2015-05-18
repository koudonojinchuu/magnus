// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SGofFreeGroup and SGofFreeGroupRep classes.
//
// Principal Author: Sergei Lyutikov
//
// Status: Draft
//
// Revision History:
//
// 1/96 Dmitry B. added
//      bool conjugateTo(const SetOf<Word>& S)
//
// 10/96 @dp added the function wrapped from code original by Denis Serbin.
//      bool SGofFreeGroupRep::isMalnormal()
//      static VectorOf<Word> ::conjugateBy(const VectorOf<Word>&, const Word&)
// 05/98 @dp ::conjugateBy(...) is a global function now.


#include "SGofFreeGroup.h"
#include <stdio.h>
#include "File.h"

SGofFreeGroupRep::SGofFreeGroupRep( const FreeGroup& parent, const VectorOf<Word>& gens ) :
  computedNielsenBasis(false),
  theSubgroupGraph(0,SetOf<Word>()),
  builtSubgroupGraph(false),
  theParentGroup(parent)
{
  // This was made to exclude empty words from the vector of generators.
  VectorOf<Word> tmpV(gens.length());
  int numberOfGens = 0;
  // Use tmpV to avoid appending elements to the vector, because it can be time consuming
  for (int i=0;i<tmpV.length();i++){
    Word tmpW = gens[i].freelyReduce();
    if (tmpW.length()>0){
      tmpV[numberOfGens] = tmpW;
      numberOfGens++;
    }
  }

  theGenerators = VectorOf<Word>(numberOfGens);
  for (int i=0;i<numberOfGens;i++)
    theGenerators[i] = tmpV[i];

}
Elt SGofFreeGroupRep::eval( const Word& w ) const {
  #if SAFETY > 0
    if ( w.maxOccurringGenerator() > theParentGroup.rank() )
	   error("SGofFreeGroupRep::eval: attempt to evaluate word with no "
				"interpretation in parent group");
  #endif

  return w.replaceGenerators(theGenerators);
}

// To test whether a subgroup of free group is malnormal we build words
// of all initial and terminal subwords taken from basis of subgroup;
// then we conjugate subgroup that we test with constructed words;
// if the subgroup and conjugated ones don't intersect then subgroup is
// malnormal,otherwise it isn't. 
//
// @dp This function is implemented by Denis Serbin.
//
// @dp `::conjugateBy(VectorOf<Word>,Word)' is local function 
//     which is used only by `isMalnormal()' now.
//     But it may be need to change the location.

VectorOf<Word> conjugateBy( const VectorOf<Word>& V, const Word& w );

bool SGofFreeGroupRep::isMalnormal( Word& conjugator ) 
{
  //  SGofFreeGroup H(F,V); // subgroup being tested  
  // VectorOf<Word> nielsenBasis = H.nielsenBasis();
	
  nielsenBasis(); // make sure that Nielsen basis is computed.

  SetOf<Word> Sinit, Sterm;
	
  int lengthOfNielsenBasis = NielsenBasis.length();
  int lenOfw, lenOfu;	
		
  Word w, u, invw, invu;     // pair words and their inverses that we take from basis of H
  Word termOfu, termOfinvu,  // initial and terminal subwords of u and w  
       initOfw, initOfinvw; 
  Word symp1, symp2, symp3;  // sympathetic words

  int parentGroupRank = theParentGroup.rank();
                          		
  for( int i = 0; i < lengthOfNielsenBasis; ++i ) { 

    // select a word from Nielsen basis.

    w = NielsenBasis[i]; 
    invw = w.inverse();
    lenOfw = w.length();
    
    for(int j = 1; j <= lenOfw/2 ; ++j ) { 

      // checks all its initial and terminal segments.

      conjugator = initOfw = w.initialSegment(j);

      if( !contains(conjugator) && !Sinit.contains(conjugator)
	  && theSubgroupGraph.intersection( 
                SubgroupGraph(parentGroupRank,
			      ::conjugateBy(NielsenBasis,conjugator)) 
	     ).rank() != 0 )
	return false;

      conjugator = initOfinvw = invw.initialSegment(j);
      
      if( !contains(initOfinvw) && !Sinit.contains(initOfinvw)
	  && theSubgroupGraph.intersection( 
                SubgroupGraph(parentGroupRank, 
			      ::conjugateBy(NielsenBasis,initOfinvw)) 
	     ).rank() != 0 )
	return false;
      
      for( int k = 0; k < lengthOfNielsenBasis; ++k ) {

	// select another word  from Nielsen basis.

	u = NielsenBasis[k]; 
	invu = u.inverse();
	lenOfu = u.length();
	
	for( int l = 1; l <= lenOfu/2; ++l ) {

	  // checks all initial and terminal sergments of the second word.

	  termOfu = u.terminalSegment(l);
	  termOfinvu = invu.terminalSegment(l);
	  
	  conjugator = (termOfu * initOfw).freelyReduce();
	  
	  if( symp1.length() > 0 && !contains(conjugator) 
	      && ( !Sinit.contains(initOfw) || !Sterm.contains(termOfu) )
	      && theSubgroupGraph.intersection( 
                    SubgroupGraph(parentGroupRank, 
				  ::conjugateBy(NielsenBasis,conjugator)) 
		    ).rank() != 0 )
	    return false;
	  
	  if( k >= i ) { 
	    
	    conjugator = ( termOfu * initOfinvw ).freelyReduce(); 

	    if( conjugator.length() > 0 && !contains(conjugator) 
		&& ( !Sinit.contains(initOfinvw) || !Sterm.contains(termOfu) ) 
		&& theSubgroupGraph.intersection( 
                      SubgroupGraph(parentGroupRank, 
				    ::conjugateBy(NielsenBasis,conjugator)) 
		   ).rank() != 0 )
	      return false;
	    
	    conjugator = ( termOfinvu * initOfw ).freelyReduce();

	    if( conjugator.length() > 0 && !contains(symp3)
		&& ( !Sinit.contains(initOfw) || !Sterm.contains(termOfinvu) ) 
		&& theSubgroupGraph.intersection( 
                      SubgroupGraph(parentGroupRank, 
				    ::conjugateBy(NielsenBasis,conjugator)) 
		   ).rank() != 0 ) 
	      return false;
	  }

	  Sterm.adjoinElement(termOfu);
	  Sterm.adjoinElement(termOfinvu);
	}
      }
      Sinit.adjoinElement(initOfw);
      Sinit.adjoinElement(initOfinvw);
    }
  }

  conjugator = Word();
  return true;
}


VectorOf<Word> SGofFreeGroupRep::nielsenBasis( ) {
  if ( computedNielsenBasis ) return NielsenBasis;
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  NielsenBasis = theSubgroupGraph.nielsenBasis( );
  computedNielsenBasis = true;
  return NielsenBasis;

  // Using a member of the FreeGroup class nielsenBasis is dangerous.
}

Word SGofFreeGroupRep::nielsenWord(int i) 
{ 
  if ( computedNielsenBasis ) return (Word)NielsenBasis[i];
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  NielsenBasis = theSubgroupGraph.nielsenBasis( );
  computedNielsenBasis = true;
  return (Word)NielsenBasis[i];
}

Word SGofFreeGroupRep::inNielsenWords(const Word& w) 
{ 
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  if ( !computedNielsenBasis ) {
    NielsenBasis = theSubgroupGraph.nielsenBasis( );
    computedNielsenBasis = true;
  }
  return theSubgroupGraph.inNielsenWords(w.freelyReduce());
}

int SGofFreeGroupRep::rank()
{
  if ( computedNielsenBasis ) return NielsenBasis.length();
  if ( builtSubgroupGraph ) return theSubgroupGraph.rank();
  makeSubgroupGraph();
  return theSubgroupGraph.rank();
}

Bool SGofFreeGroupRep::wordProblem( const Word& w ) const
{
  Word temp = w.replaceGenerators(theGenerators);
  if ( theParentGroup.wordProblem(temp) == yes )
	 return 1;
  else
	 return 0;
  //@rn Without the temp, g++ 2.6.0 does not call the dtor!
}

SGofFreeGroupRep* SGofFreeGroupRep::join(SGofFreeGroupRep& SGR)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  if ( !SGR.builtSubgroupGraph ) SGR.makeSubgroupGraph();
  SubgroupGraph join = theSubgroupGraph.join(SGR.theSubgroupGraph);
  SGofFreeGroupRep* res = new SGofFreeGroupRep(theParentGroup,join.nielsenBasis());
  res->theSubgroupGraph = join;
  res->builtSubgroupGraph = true;

  return res;
}

SGofFreeGroupRep* SGofFreeGroupRep::intersection(SGofFreeGroupRep& SGR)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  if ( !SGR.builtSubgroupGraph ) SGR.makeSubgroupGraph();
  SubgroupGraph intersect = theSubgroupGraph.intersection(SGR.theSubgroupGraph);
  SGofFreeGroupRep* res = new SGofFreeGroupRep(theParentGroup,intersect.nielsenBasis());
  res->theSubgroupGraph = intersect;
  res->builtSubgroupGraph = true;

  return res;
}


Bool SGofFreeGroupRep::conjugacyProblem( const Word& u, const Word& v ) const
{
  Word tempU = u.replaceGenerators(theGenerators);
  Word tempV = v.replaceGenerators(theGenerators);
  Word conjugator;
  if ( theParentGroup.conjugacyProblem(tempU,tempV,conjugator) == yes )
	 return 1;
  else
	 return 0;
}

Bool SGofFreeGroupRep::contains(const Word& w)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.contains(w.freelyReduce());
}

// For all generators of the free group and their inverses check
// if the subgroup contains its conjugate.
Bool SGofFreeGroupRep::isNormal( )
{
  for ( int i = 0; i < theGenerators.length(); i++ ) {
    for ( int j = 1; j <= theParentGroup.rank(); j++ ) {
      Generator g(j);
      Word w1 = (Word)theGenerators[i];
      w1 = w1.conjugateBy(Word(g));
      if ( !contains(w1) ) return FALSE;
      w1 = (Word)theGenerators[i];
      w1 = w1.conjugateBy(Word(inv(g)));
      if ( !contains(w1) ) return FALSE;
    }
  }

  return TRUE;
}

VectorOf<Word> SGofFreeGroupRep::normalizer()
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.normalizer();
}

Bool SGofFreeGroupRep::contains(const SetOf<Word>& S)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  SetOf<Word> tmp;
  SetIterator<Word> I(S);
  while ( !I.done() ) {
    tmp.adjoinElement(I.value().freelyReduce());
    I.next();
  }
  return theSubgroupGraph.contains(tmp);
}

Bool SGofFreeGroupRep::contains(const VectorOf<Word>& V)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  VectorOf<Word> tmp(V.length());
  for ( int i = V.length()-1; i >= 0; i-- )
    tmp[i] = ((Word)V[i]).freelyReduce();
  return theSubgroupGraph.contains(tmp);
}

Bool SGofFreeGroupRep::contains(const SGofFreeGroupRep& SGR)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  VectorOf<Word> tmp(SGR.theGenerators.length());
  for ( int i = SGR.theGenerators.length()-1; i >= 0; i-- )
    tmp[i] = ((Word)SGR.theGenerators[i]).freelyReduce();
  return theSubgroupGraph.contains(tmp);
}

Bool SGofFreeGroupRep::equalTo(const SetOf<Word>& S)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  SetOf<Word> tmp;
  SetIterator<Word> I(S);
  while ( !I.done() ) {
    tmp.adjoinElement(I.value().freelyReduce());
    I.next();
  }

  return theSubgroupGraph.equalTo(tmp);
}

Bool SGofFreeGroupRep::conjugateInSubgroup(const Word& w, Word& conjugator)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  //@njz
  //  return theSubgroupGraph.conjugateInSubgroup(w.freelyReduce(),conjugator);
  SetOf<Word> tmp;
  tmp.adjoinElement(w.freelyReduce());
  return theSubgroupGraph.conjugateInSubgroup(tmp,conjugator);
  //
}

Bool SGofFreeGroupRep::conjugateInSubgroup(const SetOf<Word>& S, Word& conjugator)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();

  SetOf<Word> tmp;
  SetIterator<Word> I(S);
  while ( !I.done() ) {
    tmp.adjoinElement(I.value().freelyReduce());
    I.next();
  }

  return theSubgroupGraph.conjugateInSubgroup(tmp,conjugator);
}

bool SGofFreeGroupRep::conjugateTo(const SetOf<Word>& S)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();

  SetOf<Word> tmp;
  SetIterator<Word> I(S);
  while ( !I.done() ) {
    tmp.adjoinElement(I.value().freelyReduce());
    I.next();
  }

  return theSubgroupGraph.conjugateTo(tmp);
}

long SGofFreeGroupRep::powerInSubgroup( const Word& w )
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.powerInSubgroup(w.freelyReduce());
}

int SGofFreeGroupRep::findIndex()
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.findIndex();
}

VectorOf<Word> SGofFreeGroupRep::findWhiteheadBasis()
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.findWhiteheadBasis();
}

Bool SGofFreeGroupRep::isAFreeFactor()
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.isAFreeFactor();
}

Bool SGofFreeGroupRep::generatesTheFreeGroup()
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.generatesTheFreeGroup();
}

Word SGofFreeGroupRep::rightSchreierRepresentative(const Word& w)
{
  if ( !builtSubgroupGraph ) makeSubgroupGraph();
  return theSubgroupGraph.rightSchreierRepresentative(w.freelyReduce());
}


SGofFreeGroupRep* SGofFreeGroupRep::MHallCompletion( ) {

  if ( !builtSubgroupGraph ) makeSubgroupGraph();

  return new SGofFreeGroupRep(theParentGroup,
		  	      	theSubgroupGraph.MHallCompletion()
								   );
}

void SGofFreeGroupRep::makeSubgroupGraph( )
{
  if ( !builtSubgroupGraph ) {
    VectorOf<Word> tmp(theGenerators.length());
    for ( int i = theGenerators.length()-1; i >= 0; i-- )
      tmp[i] = ((Word)theGenerators[i]).freelyReduce();
    theSubgroupGraph = SubgroupGraph( theParentGroup.rank(), tmp );
    builtSubgroupGraph = true;
  }
}

////////////////// IO:

void SGofFreeGroupRep::printOn(ostream& ostr) const
{
  ostr << "< ";
  printGenerators( ostr );
  ostr << " >";
}

void SGofFreeGroupRep::printGenerators(ostream& ostr) const
{
  int len = theGenerators.length();

  if ( len > 0 ) theParentGroup.printWord(ostr,theGenerators[0]);

  for ( int i = 1; i < len; i++ ) {
    ostr << ", ";
    theParentGroup.printWord(ostr,theGenerators[i]);
  }
}

void SGofFreeGroupRep::printGenerator(ostream& ostr, int n) const
{
  theParentGroup.printWord(ostr,theGenerators[n]);
}

void SGofFreeGroupRep::printWord(ostream& ostr, const Word& w) const
{
  Word temp = w.replaceGenerators(theGenerators);
  theParentGroup.printWord(ostr,temp);
}

/////////////////////////////////////////////////////////////////////////////
//
//    Local functions
//
/////////////////////////////////////////////////////////////////////////////

// Conjugates each word of the given vector with the word `w'.

VectorOf<Word> conjugateBy(const VectorOf<Word>& V, const Word& w)
{ 
  VectorOf<Word> result(V.length());
  for( int i = 0; i < V.length(); ++i ) 
    result[i] = Word( Word(V[i]).conjugateBy(w) ).freelyReduce();
  return result;
}


//----------------------- SGofFreeGroupRep -------------------------//
//------------------------- static members -------------------------//

const Type SGofFreeGroupRep::theSGofFreeGroupType = Type( Type::unique() );




//======================================================================


VectorOf<Word> makeMapBetweenNielsens( SGofFreeGroup& H,
				       FreeGroup::NielsenBasis& N )
{
  VectorOf<Word> sgNielsen = H.nielsenBasis();
  VectorOf<Word> myNielsen = N.newGenerators();

#if SAFETY > 0
  if( sgNielsen.length() != myNielsen.length() )
    error("makeMapBetweenNielsens(): Basises must have same length.");
#endif

  VectorOf<Word> result( sgNielsen.length() );

  for( int i = myNielsen.length() - 1; i >= 0; --i ) {
    Word n = myNielsen[i];
    Word s = H.inNielsenWords( n );
    if( s.length() != 1 ) {
      char msg[200];
      sprintf( msg, "makeMapBetweenNielsens(): %d is not a nielsen word.\n");
      warn( msg );
    } 
    else {
      Generator g = s[0];
      int gabs = abs(ord(g));
      if( ord(g) > 0 ) {
	result[gabs-1] = Generator(i+1);
      }
      else {
	result[gabs-1] = Generator(-i-1);
      }
    }
  }

#if SAFETY > 0
  int emptyLinks = 0;
  for(int i = myNielsen.length() - 1; i >= 0; --i ) {
    if( result[i].length() == 0 ) {
      char msg[200];
      sprintf( msg, "makeMapBetweenNielsens(): link %d is empty.\n", i);
      warn( msg );
      ++emptyLinks;
    }
  }
  if( emptyLinks ) {
    char msg[200];
    sprintf( msg, "makeMapBetweenNielsens(): total %d links are empty.\n", 
	     emptyLinks);
    error( msg );
  }
#endif

  return result;
} 


Word expressWordInSubgroupGenerators( const SGofFreeGroup& H, const Word& w )
{
  SGofFreeGroup S = H;
  FreeGroup F = S.parentGroup();

  bool dontKeepDetails = false;
  FreeGroup::NielsenBasis slowNielsen = 
    F.nielsenBasis( S.generators(), dontKeepDetails );

  VectorOf<Word> images = makeMapBetweenNielsens( S, slowNielsen );
  VectorOf<Word> nielsenInOldGens = slowNielsen.expressNewGensInOldGens();

  for( int j = 0; j < images.length(); ++j ) {
    images[j] = images[j].replaceGenerators( nielsenInOldGens );
  }

  FreeGroup G( images.length() );
  Map map( G, G, images );
  Word wInNielsenWords = S.inNielsenWords( w );
  Word wInSubgroupGens = map.imageOf( wInNielsenWords );


  return wInSubgroupGens;
}


bool SGofFreeGroup::operator == ( const SGofFreeGroup& g) const 
{
  if (parentGroup().rank()!=g.parentGroup().rank()) return false;
  SGofFreeGroup ng1( parentGroup(), generators() );
  SGofFreeGroup ng2( parentGroup(), g.generators() );

  if( !ng1.contains( ng2.generators() ) ) return false;
  if( !ng2.contains( ng1.generators() ) ) return false;
  return true;
}


int SGofFreeGroupRep::hash() const
{
  int t, j;
  int result = 0;
  const int rank = theParentGroup.rank();
  const int p = exp(14*log(2)/rank);
  int* dg = new int[rank];
  
  for( t=0;t<rank;++t ) dg[t] = 0;

  VectorOf<Word> words( (SGofFreeGroup(theParentGroup, theGenerators)).nielsenBasis() );

  for( j=0 ; j<words.length() ; ++j ) 
    for( t=0 ; t<words[j].length() ; ++t ) {
      dg[abs(words[j][t])-1]++;
      dg[abs(words[j][t])-1] %= p;
    }
 
  j = 1;
  for( t=0 ; t<rank ; ++t ) {
    result += dg[t]*j;
    j *= p;
  }

  delete []dg;
  return result;
}


template <> int SetData< SGofFreeGroup >::
hashElement( const SGofFreeGroup& t) const
{
  return t.hash();
}
