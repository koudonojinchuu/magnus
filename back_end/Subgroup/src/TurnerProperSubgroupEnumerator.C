// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class TurnerProperSubgroupEnumerator
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#include "TurnerProperSubgroupEnumerator.h"
#include "Set.h"
//#include "OutMessages.h"
//#include "viewStructure.h"
//#include "GeneralWhitehead.h"
//#include "SMSubgroup.h"


//################################################

ProperSubgroupEnumerator::ProperSubgroupEnumerator(const FreeGroup& F, const VectorOf<Word>& words) :
  theGroup( F ),
  theWords( words ),
  subgroup( 0 ),
  rank( theGroup.rank() )
{
  int t, cur, w;

  genOfGroup = VectorOf<Word>(rank);

  for( t=0 ; t<rank ; ++t ) 
    genOfGroup[t] = Word(Generator(t+1));

  //qVert - number of vertices
  qVert = 1;
  for( t=0 ; t<words.length() ; ++t ) 
    qVert += words[t].length()-1;

  setToVert  = new int[qVert];
  vertToWord = new int[qVert];
  vertToPos  = new int[qVert];

  for( t=0,cur=0,w=0 ; t<qVert ; ++t,++cur ) {
    setToVert[t] = UNKNOWN;
    vertToWord[t] = w;
    vertToPos[t]  = cur;
    if( cur==words[w].length()-1 ) {
      ++w;
      cur = 0;
    }
  }

  partition  = new int[qVert];
  _partition = new int*[qVert];
  for( t=0 ; t<qVert ; ++t ) {
    partition[t] = UNKNOWN;
    _partition[t] = new int[qVert];
  }
  lCells = new LC[qVert];

  nVert = 0;
  nSet = 0;
  qSet = 0;

  qWord = theWords.length();
  //generators of new subgroup
  genOfSG = VectorOf<Word>( qVert+qWord , theWords );
  buildSG( );
}


ProperSubgroupEnumerator::~ProperSubgroupEnumerator()
{
  for( int t=0 ; t<qVert ; ++t ) 
    delete []_partition[t];
  delete []_partition;

  delete []setToVert;
  delete []vertToWord;
  delete []vertToPos;
  delete []lCells;
  if( subgroup )
    delete subgroup;
}


bool ProperSubgroupEnumerator::getNextProperSubgroup(SGofFreeGroup& sg)
{
  while( nVert>=0 ) {

    if( nSet<=qSet ) {
      if( stepTo() ) {
	if( nVert==qVert ) {
  
	  /*
	  static int _t = 0;	  
	  file << _t << ": {";
	  for( int t=0; t<qVert; ++t) 
	    file << partition[t] << " , ";
	  file << "}" << endl;
	  ++_t;
	  */

	  VectorOf<Word> vt = subgroup->nielsenBasis( );
	  sg = SGofFreeGroup( theGroup , vt );

	  stepBack();
	  ++nSet;
	  return true;
	}
      }
    }
    else {
      stepBack();
    }
    ++nSet;
  }
  return false;
}


bool ProperSubgroupEnumerator::stepTo()
{
  int t;

  if( partition[nVert]!=UNKNOWN ) {

    //
    if( nSet>partition[nVert] ) {
      nSet = qSet;
      return false;
    }
    nSet = partition[nVert];

    // debug checkout
    if( setToVert[nSet]==UNKNOWN ) {
      file << "Error !!! " << endl;
      //error
    }

    // remember step
    for( t=0 ; t<qVert ; ++t ) 
      _partition[nVert][t] = partition[t];
    partition[nVert] = nSet;
    
    lCells[nVert].qSet = qSet;
    
    nSet = -1;
    ++nVert;
    return true;  
  }
  else {

    // 
    for( t=0 ; t<qVert ; ++t ) 
      _partition[nVert][t] = partition[t];

    //
    partition[nVert] = nSet;

    // check references to vertice
    for( t=0 ; t<qVert ; ++t) 
      if( partition[t]==nVert )
	partition[t] = nSet;
    
    lCells[nVert].qSet = qSet;
    if( nSet==qSet ) // mem. first vertice of set
      setToVert[qSet++] = nVert;
    else {
      // add new generator
      genOfSG[qWord++] = getWord(setToVert[nSet], nVert);
      buildSG( );
    }
    nSet = -1;
    ++nVert;

    if( !setRefs() || subgroup->contains(genOfGroup) ) {
      stepBack();
      return false;
    }

    return true;
  }
}


void ProperSubgroupEnumerator::stepBack()
{
  int t;
  --nVert;

  if( nVert>=0 ) {

    nSet = partition[nVert];

    for( t=0 ; t<qVert ; ++t ) 
      partition[t] = _partition[nVert][t];

    if( qSet > lCells[nVert].qSet ) 
      setToVert[qSet] = UNKNOWN;
    else 
      if( partition[nVert]==UNKNOWN ) {
	--qWord;
	buildSG( );
      }

    qSet = lCells[nVert].qSet;
  }
}


Word ProperSubgroupEnumerator::getWord(int x, int y)
{
  int t;
  int w1 = vertToWord[x];
  int w2 = vertToWord[y];
  int p1 = vertToPos[x];
  int p2 = vertToPos[y];
  VectorOf<Generator> vect(p1 + theWords[w2].length()-p2 );

  for( t=0 ; t<p1 ; ++t ) 
    vect[t] = theWords[w1][t];
  for( t=0 ; t<theWords[w2].length()-p2 ; ++t )
    vect[p1+t] = theWords[w2][p2+t];

  /*
  file << "getWord(" << x << "," << y << ")" << " -> " ;
  theGroup.printWord(file, Word(vect) );
  file << endl;
  */

  return Word(vect);
}


void ProperSubgroupEnumerator::buildSG( )
{
  if( subgroup )
    delete subgroup;

  subgroup = new SGofFreeGroup( theGroup , VectorOf<Word>(qWord, genOfSG) );
}


bool ProperSubgroupEnumerator::setRefs( )
{
  int t, t1, t2;
  Word w;
  int ns1;
  int ns2;

  for( t1=0 ; t1<qVert; ++t1 ) {
    ns1 = partition[t1];
    if( ns1==UNKNOWN || setToVert[ns1]==t1 ) {

      for( t2=t1+1 ; t2<qVert; ++t2 ) {
	ns2 = partition[t2];
	if( ns2==UNKNOWN || setToVert[ns2]==t2 ) {
	  
	  w = getWord(t1, t2);
	  if( subgroup->contains(w) ) {

	    // 1.	    
	    if( ns1!=UNKNOWN && ns2!=UNKNOWN )
	      return false;
	    
	    // 2.
	    if( ns1!=UNKNOWN ) {
	      partition[t2] = ns1;
	      for( t=t2 ; t<qVert ; ++t )
		if( partition[t]==t2 )
		  partition[t]=ns1;
	      continue;
	    }
	    
	    // 3.
	    if( ns2!=UNKNOWN ) {
	      file << "Waring !!! 'ns2!=UNKNOWN'" << endl;
	      continue;
	    }
	    
	    // 4. 
	    // ns1==UNKNOWN && ns2==UNKNOWN
	    partition[t2] = ns1;
	    for( t=t2 ; t<qVert ; ++t )
	      if( partition[t]==t2 )
		partition[t]=ns1;
	    
	  }
	}
      }
    }
  }

  return true;
}

