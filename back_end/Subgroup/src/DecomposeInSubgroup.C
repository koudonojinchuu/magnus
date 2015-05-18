// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Bugs:
//
// Special Notes:
//
// Next implementation steps:
//


#include "DecomposeInSubgroup.h"
#include "Timer.h"
#include "conversions.h"
#include "../AProducts/include/CONDITION.h"
#include "Timer.h"

//#define DEBUG_DECOMPOSE
//#define DEBUG_READ_PIECE

template <class T>
bool 
readPieceOfVector( VectorOf<T>& v, int& pos, istream& istr, const Timer& timer)
{
  while( !timer.expired() &&  pos < v.length() ) {
    istr > v[pos];

#ifdef DEBUG_READ_PIECE
    // cerr << v[pos] << ' ';
#endif

    ++pos;
  }
  return (pos == v.length());
}

template <class T>
void writeVector( ostream& ostr, const VectorOf<T>& V )
{
  ostr < V.length();
  for( int i = 0; i < V.length(); ++i ) {
    ostr < V[i] < ' ';
  }
}

template <class T>
VectorOf<T> concat( const VectorOf<T>& v1, const VectorOf<T>& v2 )
{
  const int v1len = v1.length();
  const int v2len = v2.length();
  VectorOf<Word> result( v1len + v2len, v1 );
  
  for( int i = 0; i < v2len; ++i ) {
    result [i+v1len] = v2 [i];
  }

  return result;
}

VectorOf<Word> conjugateBy(const VectorOf<Word>& V, const Word& w);

VectorOf<Word> replaceGenerators( const VectorOf<Word> origin, 
				  const VectorOf<Word> replace )
{
  VectorOf<Word> result( origin.length() );
  for( int i = origin.length()-1; i >= 0; --i ) {
    result[i] = origin[i].replaceGenerators( replace );
  }
  return result;
}

//
//  Class  DecomposeInSubgroup.
//

DecomposeInSubgroupOfFreeGroup::DecomposeInSubgroupOfFreeGroup( 
  const int ambientRank, const VectorOf<Word>& gens )
  : theAmbientRank( ambientRank ),
    theGraph( ambientRank, gens ),
    theGenerators( gens ), 
    readState( STOP )
{
  init();
}

DecomposeInSubgroupOfFreeGroup::DecomposeInSubgroupOfFreeGroup( 
   const SubgroupGraph& G, const VectorOf<Word>& gens )
  : theAmbientRank( G.rank() ), 
    theGraph( G ),
    theGenerators( gens ),
    readState( STOP )
{
  init();
}

Word DecomposeInSubgroupOfFreeGroup::decompose( const Word& w ) const
{
  if( !theGraph.contains( w ) ) {
    error("The given word does not contains in the subgroup in \n"
	  "Word DecomposeInSubgroupOfFreeGroup::decompose(const Word& w).");
  }

  Word inNielsen = theGraph.inNielsenWords( w );
  return inNielsen.replaceGenerators( nielsenInSubgroupGenerators );
}

void DecomposeInSubgroupOfFreeGroup::init( )
{
  const FreeGroup F( theAmbientRank );
  const bool dontKeepDetails = false;
  FreeGroup::NielsenBasis slowNielsen = 
    F.nielsenBasis( theGenerators, dontKeepDetails );

  VectorOf<Word> images = 
    makeMapBetweenNielsens( theGraph, slowNielsen.newGenerators() );
  VectorOf<Word> nielsenInOldGens = slowNielsen.expressNewGensInOldGens();

  nielsenInSubgroupGenerators = replaceGenerators( images, nielsenInOldGens );
}

void DecomposeInSubgroupOfFreeGroup::write( ostream& ostr ) const
{
#ifdef DEBUG
  ostr < Chars("DecomposeInSubgroupOfFreeGroup<") < ' ';
#endif

  ostr < theAmbientRank < '\n';
  writeVector( ostr, theGenerators );
  ostr < '\n' < theGraph < '\n';
  writeVector( ostr, nielsenInSubgroupGenerators );
  ostr < '\n';

#ifdef DEBUG
  ostr < ' ' < Chars(">DecomposeInSubgroupOfFreeGroup");
#endif
}

istream& operator > ( istream& istr, DecomposeInSubgroupOfFreeGroup& d )
{
  Timer timer(1000);
  while( !d.readPiece( istr, timer ) ) {
    timer.reset(1000);
  }
  return istr;
}

bool 
DecomposeInSubgroupOfFreeGroup::readPiece( istream& istr, const Timer& timer )
{
  bool read = true;
  switch( readState ) {

  case STOP: 
    // This should be done very quick
#ifdef DEBUG
    {
      Chars header;
      istr > header;
      if( header != Chars("DecomposeInSubgroupOfFreeGroup<")) {
	error("Cannot load DecomposeInSubgroupOfFreeGroup: bad header of data section");
      }
    }
#endif

    istr > theAmbientRank;
    int k;
    istr > k;
    theGenerators = VectorOf<Word>( k );
    readState = GENS;
    n = 0;

  case GENS:
    while( !timer.expired() && read )
      read = ! readPieceOfVector(theGenerators, n, istr, timer);
    if( read )
      break;
    
#ifdef DEBUG_READ_PIECE
    cerr << "\ntheAmbientRank = " << theAmbientRank 
	 << "\ntheGenerators = " << theGenerators;
#endif

    read = true;
    readState = GRAPH;
    theGraph = SubgroupGraph( theAmbientRank, VectorOf<Word>() ); 
    // Start reading the graph right now.

  case GRAPH: 
    while( !timer.expired() && read  )
      read = ! theGraph.readPiece( istr, timer );

    if( read ) 
      break;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theGraph = ";
    cerr < theGraph;
#endif

    readState = NIELSEN;
    read = true;
    n = 0;
    int numberOfNielsens;
    istr > numberOfNielsens;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "numberOfNielsens = " << numberOfNielsens << ' ';
#endif

    nielsenInSubgroupGenerators = VectorOf<Word>(numberOfNielsens);
    // start reading the vector component.

  case NIELSEN:
    while( !timer.expired() && read )
      read = ! readPieceOfVector(nielsenInSubgroupGenerators, n, istr, timer);
    if( read )
      break;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "nielsenInSubgroupGenerators = " 
	 << nielsenInSubgroupGenerators;
    cerr << endl;
#endif

#ifdef DEBUG
    {
      Chars footer;
      istr > footer;
      if( footer != Chars(">DecomposeInSubgroupOfFreeGroup")) {
	error("Cannot load DecomposeInSubgroupOfFreeGroup: bad footer of data section");
      }
    }
#endif

    readState = STOP;
    return true;
  }

  return false;
}


VectorOf<Word> DecomposeInSubgroupOfFreeGroup::makeMapBetweenNielsens( 
  SubgroupGraph& S, const VectorOf<Word>& words ) const
{
  VectorOf<Word> result( S.nielsenBasis().length() );

  for( int i = words.length() - 1; i >= 0; --i ) {
    const Word w = words[i];
    const Word s = S.inNielsenWords( w );
    CONDITION(ERROR,  s.length() == 1 );
    const Generator g = s[0];
    int gabs = abs(ord(g));
    if( ord(g) > 0 ) {
      result[gabs-1] = Generator(i+1);
    }
    else {
      result[gabs-1] = Generator(-i-1);
    }
  }

#if SAFETY > 0
  int emptyLinks = 0;
  for(int i = words.length() - 1; i >= 0; --i ) {
    if( result[i].length() == 0 ) {
      error("Found empty link");
      // char msg[200];
      // sprintf( msg, "makeMapBetweenNielsens(): link %d is empty.\n", i);
      // warn( msg );
      ++emptyLinks;
    }
  }
  if( emptyLinks ) {
    error("We have empty links");
    // char msg[200];
    // sprintf( msg, "makeMapBetweenNielsens(): total %d links are empty.\n", 
    // emptyLinks);
    // error( msg );
  }
#endif

  return result;
} 



Trichotomy DecomposeInSubgroupOfFreeGroup::checkDecomposition( const Word& w ) 
  const
{
  Word deco = decompose(w);

  if( w.freelyReduce()!=deco.replaceGenerators(theGenerators).freelyReduce() )
    return no;

  return yes;
}


//
//  Class  DecomposeInExpandingSubgroup.
//

DecomposeInExpandingSubgroup::DecomposeInExpandingSubgroup( 
  int ambientRank, const VectorOf<Word>& constantSubgroup, 
  const VectorOf<Word>& expandingSubgroup )
  : DecomposeInSubgroupOfFreeGroup( ambientRank, 
				    concat(constantSubgroup,expandingSubgroup) 
				    ),
    theConstantGraph( ambientRank, constantSubgroup ),
    theExpandingGraph( ambientRank, expandingSubgroup ),
    theConstantGenerators( constantSubgroup ),
    theExpandingGenerators( expandingSubgroup ),
    theNumberOfExpandedGenerators( expandingSubgroup.length() ),
    readState2( STOP )
{
  VectorOf<Word> C, E0, Ne0, Nce0InOld, H0, Nh0, Nh0InH0, Nh0InOld;

  C  = theConstantGenerators;
  Nc = theConstantGraph.nielsenBasis();

#ifdef DEBUG_DECOMPOSE
  cerr << "Nc = " << Nc << endl;
#endif

  DecomposeInSubgroupOfFreeGroup DC( theConstantGraph, C );

  NcInOld = DC.decomposeNielsenBasis( ); // !!

#ifdef DEBUG_DECOMPOSE
  cerr << "NcInOld = " << NcInOld << endl;
#endif

  E0  = theExpandingGenerators;
  Ne0 = theExpandingGraph.nielsenBasis();

#ifdef DEBUG_DECOMPOSE
  cerr << "E0 = " << E0 << endl
       << "Ne0 = " << Ne0 << endl;
#endif

  DecomposeInSubgroupOfFreeGroup DE0( theExpandingGraph, E0 );
  Ne0InOld = DE0.decomposeNielsenBasis( );

#ifdef DEBUG_DECOMPOSE
  cerr << "Ne0InOld = " << Ne0InOld << endl;
#endif

  Ne0InOld = shiftGenerators( Ne0InOld, theConstantGenerators.length() ); //!!

#ifdef DEBUG_DECOMPOSE
  cerr << "Ne0InOld = " << Ne0InOld << endl;
#endif

  Nce0InOld = concat( NcInOld, Ne0InOld );

#ifdef DEBUG_DECOMPOSE
  cerr << "Nce0InOld = " << Nce0InOld << endl;
#endif

  H0 = concat( Nc, Ne0 );
  
#ifdef DEBUG_DECOMPOSE
  cerr << "H0 = " << H0 << endl;
#endif

  SubgroupGraph theH0graph( rankOfFreeGroup(), H0 );
  Nh0 = theH0graph.nielsenBasis();

#ifdef DEBUG_DECOMPOSE
  cerr << "Nh0 = " << Nh0 << endl;
#endif

  DecomposeInSubgroupOfFreeGroup DH0( theH0graph, H0 );
  Nh0InH0 = DH0.decomposeNielsenBasis( );

#ifdef DEBUG_DECOMPOSE
  cerr << "Nh0InH0 = " << Nh0InH0 << endl;
#endif

  nielsenInSubgroupGenerators = replaceGenerators( Nh0InH0, Nce0InOld );

#ifdef DEBUG_DECOMPOSE
  cerr << "nielsenInSubgroupGenerators = "
       << nielsenInSubgroupGenerators << endl;
#endif
 }


static Word shiftGenerators( const Word& w, const int shift )
{
  Word result = w;
  for( int i =  w.length()-1; i >= 0; --i ) {
    int g = abs(ord(w[i])) + shift;
    result[i] = ( ord(w[i]) > 0 ? g : -g );
  }
  return result;
}

VectorOf<Word> DecomposeInExpandingSubgroup::shiftGenerators( 
  const VectorOf<Word>& v, int shift )
{
  VectorOf<Word> result( v.length() );
  for( int i = v.length()-1; i >= 0; --i ) {
    result[i] = ::shiftGenerators( v[i], shift );
  }
  return result;
}


void DecomposeInExpandingSubgroup::expandSubgroupByConjugation(
  const Generator& conjugator )
{
  theExpandingConjugators.append( conjugator );

  VectorOf<Word> E1, E1InOld, Ne0, Ne1, Ne1InNe0, Ne1InOld, Nce1InOld;
  VectorOf<Word> H1, Nh1, Nh1InH1, Nh1InOld;

  Ne0 = theExpandingGraph.nielsenBasis();
  E1 = concat( Ne0, conjugateBy(Ne0, conjugator));
  VectorOf<Word> expConjNielsenBasis = 
    shiftGenerators( Ne0InOld, theNumberOfExpandedGenerators );
  E1InOld = concat(Ne0InOld,expConjNielsenBasis);

#ifdef DEBUG_DECOMPOSE
  cerr << "Ne0 = " << Ne0 << endl
       << "E1 = " << E1 << endl
       << "E1InOld = " << E1InOld << endl;
#endif

  theExpandingGraph.joinConjugate( inv(conjugator) );
  theNumberOfExpandedGenerators *= 2;

  Ne1 = theExpandingGraph.nielsenBasis();
  DecomposeInSubgroupOfFreeGroup DE1( theExpandingGraph, E1 );
  Ne1InNe0 = DE1.decomposeNielsenBasis( );

#ifdef DEBUG_DECOMPOSE
  cerr << "Ne1 = " << Ne1 << endl
       << "Ne1InNe0 = " << Ne1InNe0 << endl;
#endif

  Ne1InOld = replaceGenerators( Ne1InNe0, E1InOld );

#ifdef DEBUG_DECOMPOSE
  cerr << "Ne1InOld = " << Ne1InOld << endl;
#endif

  Nce1InOld = concat( NcInOld, Ne1InOld );

#ifdef DEBUG_DECOMPOSE
  cerr << "Nce1InOld = " << Nce1InOld << endl;
#endif

  H1 = concat( Nc, Ne1 );

#ifdef DEBUG_DECOMPOSE
  cerr << "H1 = " << H1 << endl;
#endif

  SubgroupGraph theH1graph( rankOfFreeGroup(), H1 );
  Nh1 = theH1graph.nielsenBasis();

#ifdef DEBUG_DECOMPOSE
  cerr << "Nh1 = " << Nh1 << endl;
#endif

  DecomposeInSubgroupOfFreeGroup DH1( theH1graph, H1 );
  Nh1InH1 = DH1.decomposeNielsenBasis( );

#ifdef DEBUG_DECOMPOSE
  cerr << "Nh1InH1 = " << Nh1InH1 << endl;
#endif

  Ne0InOld = Ne1InOld;
  nielsenInSubgroupGenerators = replaceGenerators( Nh1InH1, Nce1InOld );

#ifdef DEBUG_DECOMPOSE
  cerr << "nielsenInSubgroupGenerators = " 
       << nielsenInSubgroupGenerators << endl;
#endif

  theGraph = theConstantGraph.join( theExpandingGraph );

  VectorOf<Word> sync = makeMapBetweenNielsens( theGraph, Nh1 );
  nielsenInSubgroupGenerators = 
    replaceGenerators( sync, nielsenInSubgroupGenerators );

#ifdef DEBUG_DECOMPOSE
  cerr << "after basises' synchronizing: " << endl;
  cerr << "nielsenInSubgroupGenerators = " 
       << nielsenInSubgroupGenerators << endl;
#endif
}


VectorOf<int> binaryDecomposition( int n )
{
  VectorOf<int> deco(0,true);

  n = abs( n );
  while( n > 0 ) {
    deco.append( n % 2 );
    n /= 2;
  }

  return deco;
}


void DecomposeInExpandingSubgroup::expressAsConjugateOfOriginalGenerator( 
  const Generator& g, Generator& og, Word& conjugator ) const
{
  const int numberOfConstantGenerators = theConstantGenerators.length();
  const int numberOfExpandingGenerators = theExpandingGenerators.length();

  int absg = abs(ord(g));
  if( absg <= numberOfConstantGenerators ) {
    og = g;
    conjugator = Word();
    return;
  }
  absg -= numberOfConstantGenerators+1;

  og = absg % numberOfExpandingGenerators + 1 + numberOfConstantGenerators;
  if( ord(g) < 0 )
    og = inv(og);

  absg /= numberOfExpandingGenerators;

  VectorOf<int> byPowerOfTwo = binaryDecomposition( absg );

#ifdef DEBUG_DECOMPOSE
  cerr << "byPowerOfTwo = " << byPowerOfTwo << endl; 
  cerr << "theExpandingConjugators = " << theExpandingConjugators << endl;
#endif

  ListOf<Generator> cVec;
  for( int i = 0; i < byPowerOfTwo.length(); ++i ) {
    if( byPowerOfTwo[i] == 1 )
      cVec.append( theExpandingConjugators[i] );
  }
  
  conjugator = Word( cVec );
}


ProductOfRelatorConjugates DecomposeInExpandingSubgroup::fullDecomposition(
  const Word& w ) const 
{
  Word deco = decompose( w );
  const int numberOfConstantGenerators = theConstantGenerators.length();
  ProductOfRelatorConjugates result;

  for( int i = 0; i < deco.length(); ++i ) {

    Generator g;
    Word conj;
    expressAsConjugateOfOriginalGenerator( deco[i], g, conj );
    Word gen = g;
    int o = abs(ord(g));

    if( o <= numberOfConstantGenerators ) {
      gen = theConstantGenerators[o-1];
    }
    else {
      o -= numberOfConstantGenerators;
      gen = theExpandingGenerators[o-1]; 
    }

    if( ord(g) < 0 ) {
      gen = gen.inverse();
    }

    result.append( RelatorConjugate(gen, conj) );
  }

  return result;
}

static Word compose( const ProductOfRelatorConjugates& product )
{
  Word result;
  for(int i = 0; i < product.length(); ++i ) {
    result *= product[i].relator.conjugateBy( product[i].conjugator );
  }
  return result;
}

Trichotomy DecomposeInExpandingSubgroup::checkDecomposition( const Word& w )
  const 
{
  Word tmp = compose( fullDecomposition(w) );

  if( w.freelyReduce() != tmp.freelyReduce() )
    return no;

  return yes;
}

void DecomposeInExpandingSubgroup::write( ostream& ostr ) const
{
  DecomposeInSubgroupOfFreeGroup::write( ostr );
  ostr < theNumberOfExpandedGenerators < '\n' < theExpandingConjugators < '\n';
  //       < theConstantGenerators.length() < theExpandingGenerators.length()
  //       < Nc.length() < NcInOld.length() < Ne0InOld.length();
  ostr < theConstantGraph < theExpandingGraph < '\n';
  writeVector( ostr, theConstantGenerators ); ostr < '\n';
  writeVector( ostr, theExpandingGenerators ); ostr < '\n';
  writeVector( ostr, Nc ); ostr < '\n';
  writeVector( ostr, NcInOld ); ostr < '\n';
  writeVector( ostr, Ne0InOld ); ostr < '\n';

}

bool DecomposeInExpandingSubgroup::readPiece(istream& istr, const Timer& timer)
{
  bool read = true;
  switch( readState2 ) {

  case STOP:
    // reading base class.
    while( !timer.expired() && read )
      read = ! DecomposeInSubgroupOfFreeGroup::readPiece( istr, timer);
    if( read )
      break;
    readState2 = BASECLASS;
    read = true;

  case BASECLASS:
    // continue to read new components of the class.
    istr > theNumberOfExpandedGenerators > theExpandingConjugators;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theNumberOfExpandedGenerators = " 
	 << theNumberOfExpandedGenerators 
	 << endl << "theExpandingConjugators = " 
	 << theExpandingConjugators;
#endif

    readState2 = CONSTGRAPH;

  case CONSTGRAPH:
    while( !timer.expired() && read )
      read = !theConstantGraph.readPiece(istr, timer);
    if( read )
      break;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theConstantGraph = ";
    cerr < theConstantGraph;
#endif

    readState2 = EXPANDGRAPH;
    read = true;

  case EXPANDGRAPH:
    while( !timer.expired() && read )
      read = !theExpandingGraph.readPiece(istr, timer);
    if( read )
      break;

    readState2 = CONSTGENS;
    read = true;
    int c;
    istr > c;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theExpandingGraph = ";
    cerr < theExpandingGraph;
    cerr << c << ' ';
#endif

    theConstantGenerators = VectorOf<Word>( c );
    n = 0;

  case CONSTGENS:
    while( !timer.expired() && read )
      read = !readPieceOfVector( theConstantGenerators, n, istr, timer );
    if( read )
      break;

    readState2 = EXPANDGENS;
    read = true;
    int e;
    istr > e;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theConstantGenerators = " << theConstantGenerators;
    cerr << endl << "e = " << e << ' ';
#endif

    theExpandingGenerators =  VectorOf<Word>( e );
    n = 0;

  case EXPANDGENS:
    while( !timer.expired() && read )
      read = !readPieceOfVector( theExpandingGenerators, n, istr, timer );
    if( read )
      break;

    readState2 = READNC;
    read = true;
    int nc;
    istr > nc;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theExpandingGenerators = " << theExpandingGenerators;
    cerr << endl << "read nc = " << nc << ' ';
#endif

    Nc = VectorOf<Word>( nc );
    n = 0;

  case READNC:
    while( !timer.expired() && read )
      read = !readPieceOfVector( Nc, n, istr, timer );
    if( read )
      break;

    readState2 = READNCINOLD;
    read = true;
    int ncinold;
    istr > ncinold;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "Nc = " << Nc;
    cerr << endl << "ncinold = " << ncinold << ' ';
#endif

    NcInOld = VectorOf<Word>( ncinold );
    n = 0;

  case READNCINOLD: 
    while( !timer.expired() && read )
      read = !readPieceOfVector( NcInOld, n, istr, timer );
    if( read )
      break;

    readState2 = READNE0INOLD;
    read = true;
    int neinold;
    istr > neinold;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "NcInOld = " << NcInOld;
    cerr << endl << "neinold = " << neinold << ' ';
#endif

    Ne0InOld = VectorOf<Word>( neinold );
    n = 0;

  case READNE0INOLD:
    while( !timer.expired() && read )
      read = !readPieceOfVector( Ne0InOld, n, istr, timer );
    if( read )
      break;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "NeInOld = " << Ne0InOld;
    cerr << endl;
#endif

    readState2 = STOP;


    return true;

  } // switch( readState2 )

  return false;
}

DecomposeInSubgroupOfFPGroup::DecomposeInSubgroupOfFPGroup( 
  const FPGroup& G, const VectorOf<Word>& gens )
  : DecomposeInExpandingSubgroup( 
      G.numberOfGenerators(), gens, makeVectorOf(G.getRelators())
    ),
    theGroup( G ), g( 1 ), theNumberOfIterations(0), readState3( STOP )
{ 
}


void DecomposeInSubgroupOfFPGroup::expandGraph( )
{
#ifdef DEBUG_DECOMPOSE
  cerr << "++ expanding graph by " << g << endl;
#endif

  expandSubgroupByConjugation( g );

  int o = ord(g);
  o = ( o > 0 ? -o : 1-o );
  if ( o > theGroup.numberOfGenerators() ) 
    o = 1;

  g = Generator(o);

  ++theNumberOfIterations;
}


void DecomposeInSubgroupOfFPGroup::write( ostream& ostr ) const
{
  DecomposeInExpandingSubgroup::write( ostr );
  ostr
    < theGroup < '\n'
    < g < '\n'
    < theNumberOfIterations < '\n';
}

bool DecomposeInSubgroupOfFPGroup::readPiece(istream& istr, const Timer& timer)
{
  bool read = true;
  switch( readState3 ) {

  case STOP:
    while( !timer.expired() && read )
      read = ! DecomposeInExpandingSubgroup::readPiece( istr, timer );
    if( read )
      break;
    readState3 = NEWDATA;
    // start reading new data of the class right now.

  case NEWDATA:
    istr > theGroup > g > theNumberOfIterations;
    readState3 = STOP;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "theGroup = " << theGroup 
	 << endl << "g = " << g
	 << endl << "theNumberOfIterations = " << theNumberOfIterations 
	 << endl;
#endif

    return true;
  }
  return false;
}

/*
#include <iostream.h>
//#include "CONDITION.h"

void debugMessage( int msgType, bool condition, const char *condDescription, 
		   const char *fileName, int lineno )
{
  if( !condition ) {

    cerr << endl;

    switch( msgType ) {
    case WARNING:
      cerr << "WARNING: ";
      break;
    case ERROR:
      cerr << "ERROR: ";
      break;
    default:
      cerr << "UNIDENTIFIED ERROR: ";
    }

    cerr << fileName << ":" << lineno << ": " 
	 << condDescription << " is false." << endl << endl;

    if( msgType != WARNING ) {
      while( 1 )
	;
      exit(1);
    }
  }
}

*/
