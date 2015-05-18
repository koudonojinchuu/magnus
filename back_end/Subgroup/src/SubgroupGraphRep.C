// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the SubgroupGraphRep class.
//
// Principal Authors: Sergei Lyutikov, Charles Miller III, Roger Needham
//
// Status: usable
//
// Revision History:
//
// * 9/94 Roger replaced the GNU data structures for nodes with homegrown,
//        re-organized the class interface, translated to the user class
//        system, and renamed everything.
//
// * 2/95 Roger simplified reduceGraph.
//
// * 2/95 Sergei
//   - fixed basisEdges.
//   - fixed void makeMaxlSubtree();
//   - fixed VectorOf<Word> nielsenBasis();
//   - added Bool loopSearch(const Word& w, VertexType v) const;
//   - added Bool contains(const SetOf<Word>& S) const;
//   - added Bool contains(const VectorOf<Word>& V) const;
//   - added Bool contains(SubgroupGraphRep& SGR);
//   - added Bool equalTo(const SetOf<Word>& S);
//   - added Bool equalTo(SubgroupGraphRep& SGR);
//   - added Word getLabel(VertexType v) const;
//   - added Word getInverseLabel(VertexType v) const;
//   - added Bool conjugateInSubgroup(const Word& w, Word& conjugator) const;
//   - added Bool conjugateInSubgroup(const SetOf<Word>& S, Word& ) const;
//   - added long powerInSubgroup( const Word& w ) const;
//
// * 3/95 Sergei
//
//   - added long findIndex( ) const;
//   - added VectorOf<Word> findWhiteheadBasis( );
//   - added Bool isAFreeFactor( );
//   - added long valenceAt(VertexType v) const;
//
// * 4/95 Sergei
//
//   - added Word nielsenWord(int i)
//   - added Word inNielsenWords(const Word& w)
//   - added int findBasisEdgeIndex(VertexType source,LabelType label)
//   - fixed Word rightSchreierRepresentative(const Word& w)
//
// * 5/95 Roger added void joinConjugate(int generator)
//
// * 12/95 Sergei fixed Word rightSchreierRepresentative(const Word& w) again.
//
// * 1/96  Dmitry B. added
//         bool conjugateTo(const SetOf<Word>& S)
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 05/97 Dmitry B. implemented IPC tools.
//
// Next implementation steps:
//



#include "SubgroupGraphRep.h"
#include "Timer.h"

//#define DEBUG_READ_PIECE

SubgroupGraphRep::SubgroupGraphRep(int whatValence, long howManyVertices)
{
#if SAFETY > 0
  if ( (whatValence < 1) || (whatValence & 1) || (howManyVertices < 0) )
    error("bad arg(s) to SubgroupGraphRep::SubgroupGraphRep");
#endif
  valence = whatValence;
  numberOfVertices = 0;
  table = NULL;
  resize(howManyVertices);
  numberOfVertices = 1;
  maximalSubtree = NULL;
  basisEdges = NULL;
  isReading = STOP;
}


SubgroupGraphRep::SubgroupGraphRep(const SubgroupGraphRep& SGR)
{
  numberOfVertices = 0;
  valence = SGR.valence;
  table = NULL;
  resize(SGR.numberOfVertices);
  numberOfVertices = SGR.numberOfVertices;

  long size = numberOfVertices * valence;
  while ( size-- )
    table[size] = SGR.table[size];

  //@rn Should really copy them, but I'm tired.
  maximalSubtree = NULL;
  basisEdges = NULL;
  isReading = STOP;
}


SubgroupGraphRep::SubgroupGraphRep(int ambientRank, const SetOf<Word>& S)
{
  // Initialize data members with enough room for a bouquet of circles,
  // one circle per word in S, with no folding. This may be more space
  // than we need.

  VertexType initialVertexCount = 1;
  SetIterator<Word> I(S);
  while ( !I.done() ) {
    initialVertexCount += I.value().length() - 1;
    I.next();
  }

  valence = 2 * ambientRank;
  numberOfVertices = 0;
  table = NULL;
  resize(initialVertexCount);
  numberOfVertices = 1;

  I.reset();
  while ( !I.done() ) {
    adjoinWord( I.value(), baseVertex );
    I.next();
  }

  resize(numberOfVertices);

  maximalSubtree = NULL;
  basisEdges = NULL;

#ifdef DEBUG_SubgroupGraph
  cerr << "\nAfter construction of SubgroupGraphRep\n";
  debugPrint(cerr);
#endif
  isReading = STOP;
}

SubgroupGraphRep::SubgroupGraphRep(int ambientRank, const VectorOf<Word>& V)
{
  // Initialize data members with enough room for a bouquet of circles,
  // one circle per word in V, with no folding. This may be more space
  // than we need.

  int len = V.length();
  VertexType initialVertexCount = 1;

  for ( int i = 0; i < len; i++ ) 
    initialVertexCount += ((Word)V[i]).length() - 1;

  valence = 2 * ambientRank;
  numberOfVertices = 0;
  table = NULL;
  resize(initialVertexCount);
  numberOfVertices = 1;

  for ( int i = 0; i < len; i++ )
    adjoinWord( (Word)V[i], baseVertex );

  resize(numberOfVertices);

  maximalSubtree = NULL;
  basisEdges = NULL;

  #ifdef DEBUG_SubgroupGraph
    cerr << "\nAfter construction of SubgroupGraphRep\n";
    debugPrint(cerr);
  #endif
  isReading = STOP;
}

SubgroupGraphRep::~SubgroupGraphRep( )
{
  delete [] table;
  delete [] maximalSubtree;
  delete [] basisEdges;
}


long SubgroupGraphRep::numberOfEdges( ) const
{
  long size = numberOfVertices * valence;
  long result = 0;
  while ( size-- )
    if ( table[size] != -1 ) ++result;
  return result >> 1;
}


void SubgroupGraphRep::resize(long howManyVertices)
{
  if ( howManyVertices == spaceForVertices &&
       howManyVertices == numberOfVertices ) return;

  numberOfVertices = min(numberOfVertices, howManyVertices);
  spaceForVertices = howManyVertices;
  long size = numberOfVertices * valence;
  long newSize = howManyVertices * valence;
  VertexType* newTable = new VertexType[newSize];
  if ( newTable == NULL )
    error("out of space in SubgroupGraphRep::resize");

  for( long i = 0; i < size; i++ )
    newTable[i] = table[i];
  for( long i = size; i < newSize; i++ )
    newTable[i] = -1;
  delete [] table;
  table = newTable;  
}


void SubgroupGraphRep::adjoinWord(const Word& w, VertexType atVertex)
  // We want to adjoin to this graph a loop at the vertex atVertex where
  // the label on the loop is the Word w. So the loop is a circle subdivided
  // into w.length() edges labeled according to the Word w.
  // At the start this graph is reduced, and when we finish we
  // must have a reduced graph.
{
  int w_length = w.length();
  if ( w_length == 0 ) return;
  int i = 0;
  int j = w_length;
  VertexType tmpVertex;
  VertexType leftVertex = atVertex;
  VertexType rightVertex = atVertex;
  
  // Follow the path from atVertex labelled by the Word w as far as
  // possible.
  
  while ( (i < j) &&
	  ((tmpVertex = targetOfGenerator(leftVertex, ord(w[i]))) != -1) ) {
    leftVertex = tmpVertex;
    ++i;
  }
  
  if ( ( i == w_length ) && ( leftVertex == atVertex ) ) return;
  // There is a closed loop labelled by w already at atVertex so exit.

  // Now follow w backward from the right - note inverse of label.

  while ( (i < j) &&
	  ((tmpVertex = targetOfGenerator(rightVertex, -ord(w[j-1]))) != -1)) {
    rightVertex = tmpVertex;
    --j;
  }
  
  if ( i < j ) {
    // w is not completely in the graph already so add new edges.
    if ( leftVertex != rightVertex )
      addWordArc(w, leftVertex, rightVertex, i, j - 1);
    else
      addWordLoop(w, leftVertex, i, j - 1);
    return;
  }
  
  // To get here i == j so the two vertices must be identical.

  if ( leftVertex != rightVertex )  
    identifyVertices(leftVertex, rightVertex);
}


void SubgroupGraphRep::addWordArc(const Word& w,
				  VertexType leftVertex, VertexType rightVertex,
				  int i, int j)
  // Add an arc with distinct endpoints labelled by w[i,j], starting at
  // leftVertex and ending at rightVertex.
  // We assume that w[i,j] cannot be followed at all in this graph,
  // so that the resulting graph will be reduced.
{
#ifdef DEBUG_SubgroupGraph
  cerr << "\nBefore addWordArc\n";
  cerr << w << " [" << i << ',' << j << "] at "
       << leftVertex << ',' << rightVertex << endl;
  debugPrint(cerr);
#endif

  for( int k = i; k < j; k++ ) {
    leftVertex = defineEdges(leftVertex, ord(w[k]));
  }
  defineEdges(leftVertex, ord(w[j]), rightVertex);

#ifdef DEBUG_SubgroupGraph
  cerr << "\nAfter addWordArc\n";
  debugPrint(cerr);
#endif
}


void SubgroupGraphRep::addWordLoop(const Word& w,
				   VertexType atVertex,
				   int i, int j)
  // The Word w[i,j] has the form v^u where u has length conjLen >= 0 and v
  // is cyclically reduced. Find this form. Then add to this graph an arc
  // labelled u starting at atVertex. At the end of u add a loop labeled v.
  // We assume that w[i,j] cannot be followed at all in this graph, so that
  // the resulting graph will be reduced.
{
#ifdef DEBUG_SubgroupGraph
  cerr << "\nBefore addWordLoop\n";
  w.debugPrint(cout);
  w.debugPrint(cerr);
  cerr << w;
  cerr << " [" << i << ',' << j << "] at "
       << atVertex << endl;
  //@rn
  cerr.flush();
  debugPrint(cerr);
#endif

  int lim = (j - i) >> 1;
  int conjLen = 0;
  while ( conjLen < lim && w[i + conjLen] == inv(w[j - conjLen]) ) ++conjLen;

  for( int k = 0; k < conjLen; k++ ) {
    atVertex = defineEdges(atVertex, ord(w[i + k]));
  }

  VertexType loopVertex = atVertex;

  lim = j - conjLen;
  for( int k = i + conjLen; k < lim; k++ ) {
    loopVertex = defineEdges(loopVertex, ord(w[k]));
  }
  defineEdges(loopVertex, ord(w[lim]), atVertex);

#ifdef DEBUG_SubgroupGraph
  cerr << "\nAfter addWordLoop\n";
  debugPrint(cerr);
#endif
}


void SubgroupGraphRep::identifyVertices(VertexType v1, VertexType v2)
  // Identify two vertices and reduce resulting graph.
{
#ifdef DEBUG_SubgroupGraph
  cerr << "\nIn identifyVertices(" << v1 << ',' << v2 << ")\n";
  debugPrint(cerr);
#endif

  if ( v1 == v2 ) return;

  VertexType* minEquivVertex = new VertexType[numberOfVertices];
  if ( minEquivVertex == NULL )
    error("out of space in SubgroupGraphRep::identifyVertices");

  for( VertexType i = 0; i < numberOfVertices; i++ ) minEquivVertex[i] = i;
  if ( v1 < v2 ) minEquivVertex[v2] = v1;
  else minEquivVertex[v1] = v2;
  
  reduceGraph(minEquivVertex);
  delete [] minEquivVertex;

#ifdef DEBUG_SubgroupGraph
  cerr << "\nAfter identifyVertices\n";
  debugPrint(cerr);
#endif
}


// This is a simplified version of the one I got by translating Chuck's
// code (below). There is no mysterious twiddling of edges; it first
// simply makes all vertex identifications, then builds a new edge table
// from the surviving vertices.

// One speed test was inconclusive: the H. Neumann conjecture code makes
// large random subgroups with long generators. As far as /bin/time is
// concerned, both versions take exactly the same amount of time.  There
// may be subtleties in Chuck's scheme that speed it up in some bad
// cases, but it seems that the usefulness of any edge updating is
// reflected in updating minEquivVertex often enough.

void SubgroupGraphRep::reduceGraph(VertexType* minEquivVertex)

  // minEquivVertex is owned by the caller, but this alters its entries.
  // Vertices i < j are identified by making minEquivVertex[j] == i.
  // Because i might be later identified with k < i, this lookup must be
  // iterated in general to get the true minimum equivalent vertex.

  // This can be optimized a bit more: when doing lookups in minEquivVertex,
  // each entry along the way could be updated with the current minimum,
  // to minimize later lookups.

{
  VertexType numberOfLiveVertices = numberOfVertices;
  VertexType* liveVertices = new VertexType[numberOfLiveVertices];
  if ( liveVertices == NULL )
    error("out of space in SubgroupGraphRep::reduceGraph");
  for( VertexType l = 0; l < numberOfLiveVertices; l++ )
    liveVertices[l] = l;
  // Holds a list of vertices which have not been killed by folding.
  // It is `shrunk' by moving the last entry onto the one to be
  // killed, and decrementing numberOfLiveVertices.

  VertexType v, min_v, target_v, target_min_v;
  LabelType label;
  Bool moreFolding = TRUE;
  VertexType liveVertexIndex;

  // First we make all vertex identifications, by making as many
  // passes as necessary over the live vertices.
  // We don't need to change any edges for this.
  
  while ( moreFolding ) {
    moreFolding = FALSE;
	 
    liveVertexIndex = 0;
    while ( liveVertexIndex < numberOfLiveVertices ) {
      v = liveVertices[liveVertexIndex];

      min_v = minEquivVertex[v];
      while ( minEquivVertex[min_v] < min_v )
	min_v = minEquivVertex[min_v];

      if ( min_v < v ) {             // Remove v from live vertices

#ifdef DEBUG_IDENTIFY
	cerr << "\nidentifying vertices " << v << " and " << min_v;
#endif
	liveVertices[liveVertexIndex] = liveVertices[--numberOfLiveVertices];
			 
	moreFolding = TRUE;          // There may be further consequences

	minEquivVertex[v] = min_v;   // May as well record this

	// If v and min_v are to be the same vertex, then any target
	// of the same label from v and min_v must be the same too.
	// Check all possibilities.

	for( label = 0; label < valence; label++ )
	  if ((target_v = targetOfLabel(v, label)) >= 0 )
	    if ((target_min_v = targetOfLabel(min_v, label)) == -1 ) {
	      table[ min_v*valence+label ] = target_v;
	      table[ target_v*valence+inverseLabel(label) ] = min_v;
	    }
	    else {
	      while ( minEquivVertex[target_v] < target_v )
		target_v = minEquivVertex[target_v];

	      while ( minEquivVertex[target_min_v] < target_min_v )
		target_min_v = minEquivVertex[target_min_v];

	      if ( target_v < target_min_v )
		minEquivVertex[target_min_v] = target_v;
	      else
		minEquivVertex[target_v] = target_min_v;
	    }
      } else ++liveVertexIndex; // Didn't kill v, so advance.
    }
  }

  // Prepare to rebuild the table.

  // Make one more pass through minEquivVertex so that the actual
  // minimum equivalent vertex can be gotten with one lookup.

  for( v = 0; v < numberOfVertices; ++v ) {
    min_v = minEquivVertex[v];
    while ( minEquivVertex[min_v] < min_v )
      min_v = minEquivVertex[min_v];
    minEquivVertex[v] = min_v;
  }

  // The vertex numbers in liveVertices are all mixed up, so we
  // renumber them consecutively.

  VertexType* newVertexNum = new VertexType[numberOfVertices];
  if ( newVertexNum == NULL )
    error("out of space in SubgroupGraphRep::reduceGraph");

  liveVertexIndex = 0;
  while ( liveVertexIndex < numberOfLiveVertices )
    newVertexNum[liveVertices[liveVertexIndex]] = liveVertexIndex++;
  // Vertex 0 is always live, so it gets reassigned to 0.

  // Now we can fill in the new table.
  // We take direct advantage of the structure of the table.

  VertexType* newTable = new VertexType[numberOfLiveVertices * valence];
  if ( newTable == NULL )
    error("out of space in SubgroupGraphRep::reduceGraph");
  liveVertexIndex = 0;
  VertexType* ptr = newTable;
  while ( liveVertexIndex < numberOfLiveVertices ) {
    v = liveVertices[liveVertexIndex++];
    for( label = 0; label < valence; label++ )
      if ( targetOfLabel(v, label) == -1 ) 
	*ptr++ = -1;
      else
	*ptr++ = newVertexNum[minEquivVertex[targetOfLabel(v, label)]];
  }

  numberOfVertices = spaceForVertices = numberOfLiveVertices;

  delete [] table;
  table = newTable;
  delete [] newVertexNum;
  delete [] liveVertices;
}

VectorOf<Word> SubgroupGraphRep::normalizer( )
{
  VectorOf<Word> nielsen = nielsenBasis();
  int theRank = nielsen.length();
  SetOf<Word> temp;

  int thatIsIt;

  for ( VertexType v = 1; v < numberOfVertices; v++ ) {
    thatIsIt = 1;
    for ( int j = 0; j < theRank; j++ )
      if ( !loopSearch((Word)nielsen[j],v) ) {
	thatIsIt = 0;
	break;
      }
    if ( thatIsIt ) temp.adjoinElement( getLabel(v) );
  }
  
  VectorOf<Word> result(theRank+temp.cardinality());

  int i;
  for ( i = 0; i < theRank; i++ ) result[i] = (Word)nielsen[i];
  SetIterator<Word> I(temp);
  while ( !I.done() ) {
    result[i++] = I.value();
    I.next();
  }
  return result;
}

VectorOf<Word> SubgroupGraphRep::nielsenBasis( )
  // Some tests:
  // { a^b, a^(b^2) }
  // { [a^2,b], [a^3,b] }
  // { [a^2,b], [a^3,b^2] }
  // { [a,b]^a, [a,b]^(a b) }
{
  int theRank = rank();
  VectorOf<Word> result( theRank );
  int i = 0;

  if ( result.length() == 0 ) return result;

  if ( !maximalSubtree ) makeMaxlSubtree( );

  // Each element of nielsen basis corresponds to an element of basisEdges.
  // basisEdges is sorted ( lexicographical order on pairs (iVertex,label).
  // To get index of an element of nielsen basis, find the index of the
  // element of basisEdges it contains.
  while ( i < theRank ) {

    VertexType source = basisEdges[i].v;
    LabelType label = basisEdges[i].l;
    VertexType target = targetOfLabel(source, label);

    int half_len = distanceFromOrigin(source);
    int index = half_len;
	 
    VectorOf<Generator> temp(half_len + distanceFromOrigin(target) + 1);

    temp[index] = labelToGenerator(label);
    while ( source ) {
      label = maximalSubtree[source];
      temp[--index] = labelToGenerator(inverseLabel(label));
      source = targetOfLabel(source, label);
    }
    index = half_len;
    while ( target ) {
      label = maximalSubtree[target];
      temp[++index] = labelToGenerator(label);
      target = targetOfLabel(target, label);
    }
    result[i++] = temp;
  }

  return result;
}

Word SubgroupGraphRep::nielsenWord(int i)
{
  if ( !maximalSubtree ) makeMaxlSubtree( );

  VertexType source = basisEdges[i].v;
  LabelType label = basisEdges[i].l;
  VertexType target = targetOfLabel(source, label);

  int half_len = distanceFromOrigin(source);
  int index = half_len;

  VectorOf<Generator> temp(half_len + distanceFromOrigin(target) + 1);

  temp[index] = labelToGenerator(label);
  while ( source ) {
    label = maximalSubtree[source];
    temp[--index] = labelToGenerator(inverseLabel(label));
    source = targetOfLabel(source, label);
  }
  index = half_len;
  while ( target ) {
    label = maximalSubtree[target];
    temp[++index] = labelToGenerator(label);
    target = targetOfLabel(target, label);
  }
  
  return (Word)temp;

}

Word SubgroupGraphRep::inNielsenWords(const Word& w)
{
  if ( w.length() == 0 ) return w;

  VertexType source = baseVertex;
  LabelType label;
  VertexType target;

  int w_len = w.length();
  int index;

  if ( !maximalSubtree ) makeMaxlSubtree( );

  VectorOf<Generator> temp(w_len);
  int num = 0;

  for ( int i = 0; i < w_len; i++ ) {
    label = generatorToLabel( ord(w[i]) );
    target = targetOfLabel(source, label);
    
    // passed basis edge?
    if ( (maximalSubtree[target] != inverseLabel(label)) &&
	 (maximalSubtree[source] != label)    ) {
      index = findBasisEdgeIndex(source,label);
      temp[num++] = index;
    }
    source = target;
  }
  
  if ( target != baseVertex ) 
    error("*** In SubgroupGraphRep::inNielsenWords: word is not in the subgroup");
  
  temp.shrink(num);
  return (Word)temp;
}

int SubgroupGraphRep::findIndex()
{
  for ( VertexType i = 0; i < numberOfVertices; i++ )
    if ( valenceAt(i) != valence ) return INFINITE;

  return numberOfVertices;
}

VectorOf<Word> SubgroupGraphRep::findWhiteheadBasis()
{
  VectorOf<Word> result = nielsenBasis();
  int theRank = result.length();
  VectorOf<Word> temp(theRank);
  int genLen;
  int tempGenLen;
  int done = 0;
  int i,j,k;

  while ( !done ) {
    genLen = 0;
    for ( i = 0; i < theRank; i++ ) genLen += ((Word)result[i]).length();

    done = 1;
    for ( i = valence/2; i >= 1; i-- ) {
      for ( j = valence/2; j >= 1; j-- ) {
	tempGenLen = genLen;
	if ( i != j ) {
	  Generator g(i);
	  Generator h(j);

	  Word w = g*h;
	  tempGenLen = 0;
	  for ( k = 0; k < theRank; k++ ) {
	    Word wrd = ((Word)result[k]).replaceGeneratorWithWord(g,w);
	    wrd = wrd.freelyReduce();
	    tempGenLen += wrd.length();
	    temp[k] = wrd;
	  }
	  if ( tempGenLen < genLen ) {
	    result = temp;
	    break;
	  }
	  w = h*g;
	  tempGenLen = 0;
	  for ( k = 0; k < theRank; k++ ) {
	    Word wrd = ((Word)result[k]).replaceGeneratorWithWord(g,w);
	    wrd = wrd.freelyReduce();
	    tempGenLen += wrd.length();
	    temp[k] = wrd;
	  }
	  if ( tempGenLen < genLen ) {
	    result = temp;
	    break;
	  }
	  w = g*inv(h);
	  tempGenLen = 0;
	  for ( k = 0; k < theRank; k++ ) {
	    Word wrd = ((Word)result[k]).replaceGeneratorWithWord(g,w);
	    wrd = wrd.freelyReduce();
	    tempGenLen += wrd.length();
	    temp[k] = wrd;
	  }
	  if ( tempGenLen < genLen ) {
	    result = temp;
	    break;
	  }
	  w = inv(h)*g;
	  tempGenLen = 0;
	  for ( k = 0; k < theRank; k++ ) {
	    Word wrd = ((Word)result[k]).replaceGeneratorWithWord(g,w);
	    wrd = wrd.freelyReduce();
	    tempGenLen += wrd.length();
	    temp[k] = wrd;
	  }
	  if ( tempGenLen < genLen ) {
	    result = temp;
	    break;
	  }
	}
	if ( tempGenLen < genLen ) break;
      }
      if ( tempGenLen < genLen ) break;
    }
    if ( tempGenLen < genLen ) done = 0;
  }
  return result;
}

Bool SubgroupGraphRep::isAFreeFactor( ) {
  VectorOf<Word> whitehead = findWhiteheadBasis( );
  
  for ( int i = whitehead.length()-1; i >= 0; i-- )
    if ( ((Word)whitehead[i]).length() != 1 ) return FALSE;

  return TRUE;
}

Word SubgroupGraphRep::rightSchreierRepresentative(const Word& w)
{
  // Suppose w = uv, where u is the longest prefix which we can trace in
  // the graph. Suppose p is the path, in the maximal subtree, from where u
  // leaves us, back to the origin. Then up is in the subgroup, so p^-1v is
  // a right representative of w.
  
  if ( w.length() == 0 ) return w;

  if ( !maximalSubtree ) makeMaxlSubtree( );

  VertexType source;
  VertexType target = 0;
  int i = 0;
  int len = w.length();
  do {
    source = target;
    target = targetOfGenerator(source, ord(w[i++]));
  } while ( ( target != -1 ) && ( i < len ) );
  if ( target != -1 ) {
    source = target;
  }
  else i--;
  int p_len = distanceFromOrigin(source);
  int j = p_len;
  VectorOf<Generator> temp(len - i + p_len);

  LabelType label;
  while ( source ) {
    label = maximalSubtree[source];
    temp[--j] = labelToGenerator(inverseLabel(label));
    source = targetOfLabel(source, label);
  }
  while ( i < len ) temp[p_len++] = w[i++];

  return Word(temp);
}


void SubgroupGraphRep::makeMaxlSubtree( )
{
  delete [] maximalSubtree;
  delete [] basisEdges;

  maximalSubtree = new LabelType[numberOfVertices];
  if ( maximalSubtree == NULL )
    error("out of space in SubgroupGraphRep::makeMaxlSubtree");
  for( VertexType i = 0; i < numberOfVertices; i++ ) maximalSubtree[i] = -1;
  // The -1 indicates that vertex i is not in the tree yet.

  basisEdges = new Edge[ rank() ];
  // There will be exactly rank() basis edges.

  if ( basisEdges == NULL )
    error("out of space in SubgroupGraphRep::makeMaxlSubtree");

  // An auxiliary vector for this computation:

  VertexType* pathHeads = new VertexType[numberOfVertices];
  pathHeads[0] = 0;
  // This stores the vertices which are the `heads' of the paths in
  // the maximal subtree under construction.
  // We usually don't need so much space, but it's unpredictable, so
  // what the hey.
  
  int numberOfPathHeads = 1;
  // The number of path heads we've `allocated' in pathHeads.

  long basisEdgeCount = 0;
  int theRank = rank();

  // Try to extend each path by one edge. We may hit the partially
  // constructed maximal subtree, in which case we store the basis
  // edge.
	 
  VertexType source, target, badLabel;
  for( VertexType i = 0; i < numberOfPathHeads; i++ ) {
    source = pathHeads[i];
    badLabel = maximalSubtree[source]; // -1 when source == 0
		
    // Check each label except badLabel to see whether
    // an edge so labeled from source exists.
    for( LabelType label = 0; label < valence; label++ ) {
      if ( (label != badLabel) &&
	   ((target = targetOfLabel(source, label)) > -1) ) {
	// OK, we have an edge; does it close a loop?
	if ( maximalSubtree[target] == -1 && target != 0 ) {
				// No loop; we can extend along this edge.
	  maximalSubtree[target] = inverseLabel(label);
	  pathHeads[numberOfPathHeads++] = target;
				
#ifdef DEBUG_SubgroupGraph
	  cerr << "adding " << source << " -- " << labelToGenerator(label)
	       << " --> " << target << " to max'l subtree\n";
#endif
	} else {
				// We've closed a loop. Record basis edge.
				// Choose one with smaller initial vertex.
				// They may be equal; then store one with
				// smaller label.

			        // If there is a label from the base vertex to itself
			        // we should take either the label or its inverse as
			        // a candidate to basisEdges. NOT BOTH OF THEM
	  if ( source < target ) {
	    basisEdges[basisEdgeCount].v = source;
	    basisEdges[basisEdgeCount++].l = label;
#ifdef DEBUG_SubgroupGraph
	    cerr << "closed loop between " << source << ',' << target << endl;
#endif
	  } else if ( source == target && label%2 == 0 ) {
	    basisEdges[basisEdgeCount].v = target;
	    basisEdges[basisEdgeCount++].l = label;
#ifdef DEBUG_SubgroupGraph
	    cerr << "closed loop between " << source << ',' << target << endl;
#endif
	  }
	}
      }
    } // end label loop

    // what's the ... ?????
    // $sl pathHeads[i] = pathHeads[--numberOfPathHeads];
    // pathHeads[i] is now stale; re-use its space.

  } // end loop over path heads
  
  delete [] pathHeads;
#ifdef DEBUG_SubgroupGraph
  if ( basisEdgeCount != theRank )
    cerr << "in SubgroupGraphRep::makeMaxlSubtree\n theRank == " << theRank << "; basisEdgeCount == " << basisEdgeCount;
#endif

  // make basisEdges ordered; lexicographical order on pairs (iVertex,label)
  VertexType i = 0;
  while ( i < theRank-1 ) {
    if ( basisEdges[i] > basisEdges[i+1] ) {
      int j = i;
      Edge tempEdge = basisEdges[j+1];

      basisEdges[j+1] = basisEdges[j];
      j--;
      while ( basisEdges[j] > tempEdge && j>=0 ) {
	basisEdges[j+1] = basisEdges[j];
	j--;
      }
      basisEdges[j+1] = tempEdge;
    }
    i++;
  }
}


int SubgroupGraphRep::distanceFromOrigin(VertexType v) const
{
  int result = 0;
  while ( v ) {
    v = targetOfLabel(v, maximalSubtree[v]);
    ++result;
  }
  return result;
}



SubgroupGraphRep* SubgroupGraphRep::join(const SubgroupGraphRep& SGR) const
{
  SubgroupGraphRep* result = disjointUnion(SGR);
  result->identifyVertices(0, numberOfVertices);
  return result;
}


SubgroupGraphRep* SubgroupGraphRep::intersection(const SubgroupGraphRep& SGR) const
  // Form graph for intersection of two sugroups (=graphs) which is the
  // component of (0,0) in the pull-back. This is constructed directly
  // from 0=(0,0) in what follows. Make a new vertex if there is an edge
  // leaving the current vertex in both A and B graphs. It takes a bit of
  // theory think to see that this actually works, but it does. This is
  // much more efficient than forming the full pull-back then finding
  // component of (0,0). The result is automatically reduced since the
  // input graphs are reduced (more think).
{
  VertexType maxCount = numberOfVertices + SGR.numberOfVertices;
  // A first guess at how many vertices we'll need.

  SubgroupGraphRep* result
    = new SubgroupGraphRep(min(valence, SGR.valence), maxCount);

  // Allowing different valences seems a little dangerous, but...

  VertexType* ACoord = new VertexType[maxCount];
  VertexType* BCoord = new VertexType[maxCount];
  ACoord[0] = 0;
  BCoord[0] = 0;

  Bool* satisfied = new Bool[maxCount];
  for( VertexType j = 0; j < maxCount; j++ )
    satisfied[j] = FALSE;
  
  Bool moreToDo = TRUE;
  
  while ( moreToDo ) {
    moreToDo = FALSE;
    for( VertexType ii = 0; ii < result->numberOfVertices; ii++ )
      if ( !satisfied[ii] ) {
	VertexType targetA, targetB;
	for( LabelType label = 0; label < result->valence; label++ ) {
	  targetA = targetOfLabel(ACoord[ii], label);
	  targetB = SGR.targetOfLabel(BCoord[ii], label);
	  if ( (targetA < 0) || (targetB < 0) ) continue;
	  VertexType target = -1;
	  for( VertexType j = 0; j < result->numberOfVertices; j++ )
	    if ( (ACoord[j] == targetA) && (BCoord[j] == targetB) ) {
	      target = j;
	      break;
	    }
	  if ( target < 0 ) {  // Have to make a new vertex.
	    if ( result->numberOfVertices >= maxCount ) {
	      // Expand available space.
	      VertexType newMaxCount = maxCount + max( maxCount / 5, 1000 );
	      result->resize(newMaxCount);
	      VertexType* newCoord = new VertexType[newMaxCount];
	      if ( newCoord == NULL )
		error("out of space in SubgroupGraphRep::intersection");
	      newCoord[0] = 0; //@rn Why?
	      for( VertexType k = 1; k < maxCount; k++ )
		newCoord[k] = ACoord[k];
	      delete [] ACoord;
	      ACoord = newCoord;
	      newCoord = new VertexType[newMaxCount];
	      if ( newCoord == NULL )
		error("out of space in SubgroupGraphRep::intersection");
	      newCoord[0] = 0; //@rn Why?
	      for( VertexType k = 1; k < maxCount; k++ )
		newCoord[k] = BCoord[k];
	      delete [] BCoord;
	      BCoord = newCoord;
	      Bool* newSatisfied = new Bool[newMaxCount];
	      if ( newSatisfied == NULL )
		error("out of space in SubgroupGraphRep::intersection");
	      newSatisfied[0] = FALSE; //@rn Why?
	      for( VertexType k = 1; k < maxCount; k++ )
		newSatisfied[k] = satisfied[k];
	      delete [] satisfied;
	      satisfied = newSatisfied;
	      for( VertexType k = maxCount; k < newMaxCount; k++ )
		satisfied[k] = FALSE;
	      maxCount = newMaxCount;
	    }
	    target = result->numberOfVertices++;
	    ACoord[target] = targetA;
	    BCoord[target] = targetB;
	    moreToDo = TRUE;
	  }
	  result->defineEdge(ii, label, target);
	  result->defineEdge(target, inverseLabel(label), ii);
	}
	satisfied[ii] = TRUE;
      }
  }
  
  delete [] ACoord;
  delete [] BCoord;
  delete [] satisfied;
  result->resize(result->numberOfVertices);
  
#ifdef DEBUG_SubgroupGraph
  cerr << "\nThe intersection:\n";
  result->debugPrint(cerr);
#endif
  
  return result;
}

Bool SubgroupGraphRep::loopSearch(const Word& w, VertexType v) const
{
  VertexType source = v;
  int len = w.length();
  for( int j = 0; j < len; j++ ) {
    source = targetOfGenerator(source, ord(w[j]));
    if ( source == -1 ) return FALSE;
  }
  return source == v;
}

Bool SubgroupGraphRep::contains(const SetOf<Word>& S) const
{
  SetIterator<Word> I(S);

  while ( !I.done() ) {
    if ( !contains(I.value()) ) return FALSE;
    I.next();
  }
  return TRUE;
}

Bool SubgroupGraphRep::contains(const VectorOf<Word>& V) const
{
  for ( int j = 0; j < V.length(); j++ ) {
    if ( !contains(V[j]) ) return FALSE;
  }
  return TRUE;
}

// Maybe, this can be optimized minimizing temporary data.
Bool SubgroupGraphRep::contains(SubgroupGraphRep& SGR) const
{
  VectorOf<Word> V = SGR.nielsenBasis();
  return contains(V);
}

Bool SubgroupGraphRep::equalTo(const SetOf<Word>& S)
{
  if ( !contains(S) ) return FALSE;
  SubgroupGraphRep temp(valence/2,S);
  if ( temp.contains(*this) ) return TRUE;
  return FALSE;
}

Bool SubgroupGraphRep::equalTo(SubgroupGraphRep& SGR)
{
  if ( contains(SGR) && SGR.contains(*this) ) return TRUE;
  return FALSE;
}

long SubgroupGraphRep::valenceAt(VertexType v) const
{
  long result = 0;

  for ( LabelType j = 0; j < valence; j++ )
    if ( targetOfLabel(v,j) != -1 ) result++;

  return result;
}

Word SubgroupGraphRep::getLabel(VertexType v) const
{
  ListOf<Generator> temp;
  VertexType i = 0, source = v;
  
  while ( source != baseVertex ) {
    LabelType tempLabel = maximalSubtree[source];
    temp.prepend( -labelToGenerator(tempLabel) );
    source = targetOfLabel(source,tempLabel);
  }
  return Word(temp);
}

Word SubgroupGraphRep::getInverseLabel(VertexType v) const
{
  ListOf<Generator> temp;
  VertexType i = 0, source = v;
  
  while ( source != baseVertex ) {
    LabelType tempLabel = maximalSubtree[source];
    temp.append( labelToGenerator(tempLabel) );
    source = targetOfLabel(source,tempLabel);
  }
  return Word(temp);
}

int SubgroupGraphRep::findBasisEdgeIndex(VertexType source,LabelType label)
{
  VertexType target = targetOfLabel(source,label);
  LabelType invLabel = inverseLabel(label);
  int theRank = rank();

  // Either edge or its inverse should be in basisEdges
  for ( int j = 0; j < theRank; j++ ) {
    if ( basisEdges[j].v == source && basisEdges[j].l == label ) return j+1;
    if ( basisEdges[j].v == target && basisEdges[j].l == invLabel ) return -(j+1);
  }


  // there is no such basis edge!?
  error("*** In SubgroupGraphRep::findBasisEdgeIndex");

  return 0;
}

Bool SubgroupGraphRep::conjugateInSubgroup(const Word& w, Word& conjugator) const
{
  if ( w.length() == 0 ) {
    conjugator = Word();
    return TRUE;
  }
  ListOf<Generator> temp;
  int initial = 0;
  int terminal = w.length()-1;

  while ( ord(w[initial]) == -ord(w[terminal]) ) {
    temp.append(ord(w[initial]));
    initial++;
    terminal--;
  }

  Word tempWord = w.cyclicallyReduce();
	 
  for ( VertexType j = 0; j < numberOfVertices; j++ ) {
    if ( loopSearch(tempWord,j) ) {
      int k = j;
      while ( k != baseVertex ) {
	LabelType tempLabel = maximalSubtree[k];
	temp.append( labelToGenerator(tempLabel) );
	k = targetOfLabel(k,tempLabel);
      }
      conjugator = ((Word)temp).freelyReduce();
      return TRUE;
    }
  }
  return FALSE;
}

// We couldn't find anything better than using Proposition 2.22 ( Lyndon & Schupp ).
// We try to use all words with length less than the maximum of lengths of generators
// of both subgroups as a conjugator.
Bool SubgroupGraphRep::conjugateInSubgroup(const SetOf<Word>& S, Word& conjugator)
{
  if ( contains(S) ) {
    conjugator = Word();
    return TRUE;
  }
  Word testWord;
  SubgroupGraphRep temp(valence,S);
  int maxLen = 0;

  VectorOf<Word> V = temp.nielsenBasis();
  for ( int j = V.length()-1; j >= 0; j-- )
    maxLen = max( maxLen, ((Word)V[j]).length() );
  V = nielsenBasis();
  for ( int j = V.length()-1; j >= 0; j-- )
    maxLen = max( maxLen, ((Word)V[j]).length() );
  
  testWord = testWord.nextInShortLex(valence/2);
  while ( testWord.length() <= maxLen ) {
    SetOf<Word> SN;
    SetIterator<Word> I(S);

    while ( !I.done() ) {
      SN.adjoinElement(((Word)I.value().conjugateBy(testWord)).freelyReduce());
      I.next();
    }
    if ( contains(SN) ) {
      conjugator = testWord;
      return TRUE;
    }
    testWord = testWord.nextInShortLex(valence/2);
  }
  return FALSE;
}


bool SubgroupGraphRep::conjugateTo(const SetOf<Word>& S)
{
  Word conjugator;
  if( !conjugateInSubgroup(S, conjugator) )
    return false;
  SubgroupGraphRep temp(valence/2,S);
  //@db I cannot use "conversions.h" here because of gcc templates bugs.
  VectorOf<Word> V = this->nielsenBasis();
  SetOf<Word> SS;
  for( int j = 0; j < V.length(); ++j )
    SS |= V[j];
  return( temp.conjugateInSubgroup( SS, conjugator ) );
}


long SubgroupGraphRep::powerInSubgroup( const Word& w ) const 
{
  VertexType curVertex = baseVertex;
  VertexType termVertex = baseVertex;
  long power = 0;
  int w_length = w.length();
  int initial = 0;
  int terminal = w_length-1;

  // aWord is trivial
  if ( w_length == 0 ) return 1;

  while ( ord(w[initial]) == -ord(w[terminal]) ) {
    curVertex = targetOfGenerator( curVertex, ord(w[initial]) );
    termVertex = targetOfGenerator( termVertex, -ord(w[terminal]) );
               
    // if there is no such edge:
    if ( curVertex == -1 || termVertex == -1 ) return 0;

    initial++;
    terminal--;
  }

  // This cycle is not infinite:
  // Observe, that there is only finite number of states (j,i), where
  // we have to go along the label ord(w[i]) from the vertex j.
  // Suppose, the state (k,m) is the first state occured twice, then
  // it's easy to see that the state 
  // (targetOfGenerator(k,-ord(w[m-1]),m-1) occured twice before (k,m)
  // ( provided the graph is reduced. )
  do {
    for ( int j = initial; j <= terminal; j++ ) {
      curVertex = targetOfGenerator( curVertex, ord(w[j]) );
      // if there is no such edge:
      if ( curVertex == -1 ) return 0;
    }
    power++;
  } while ( curVertex != termVertex );

  return power;
}

void SubgroupGraphRep::MHallComplete()
  // Complete graph to a covering space which is then a subgroup of finite
  // index. The original subgroup is a free factor of this subgroup since
  // only edges ouside a maximal tree are added. This implements a theorem
  // of Marshall Hall that such a completion is possible. Note that every
  // generator and its inverse necessarily have an equal number of -1 entries
  // in their columns making such a completion possible (elementary graph theory).
  // The index of the result is the number of its vertices.
{
  for( LabelType label = 0; label < valence; label += 2 ) {
    LabelType invLabel = label + 1;
    //@rn Coding of labels used explicitly here...

    VertexType curpos = 0;
    VertexType curneg = 0;
    while ( curpos < numberOfVertices )
      if ( targetOfLabel(curpos, label) == -1 ) {
	while ( (curneg < numberOfVertices) &&
		(targetOfLabel(curneg, invLabel) != -1) )
	  ++curneg;
	if ( curneg < numberOfVertices ) {
	  defineEdge(curpos, label, curneg);
	  defineEdge(curneg++, invLabel, curpos++);
	} else
	  error("unbalanced edges found in SubgroupGraphRep::MHallComplete()");
      }
      else ++curpos;
  }
}


SubgroupGraphRep* SubgroupGraphRep::disjointUnion(const SubgroupGraphRep& SGR) const
{
#if SAFETY > 0
  if ( valence != SGR.valence )
    error("SubgroupGraphRep::disjointUnion got arg of different valence");
#endif

  SubgroupGraphRep* result
    = new SubgroupGraphRep(valence, numberOfVertices + SGR.numberOfVertices);

  result->numberOfVertices = numberOfVertices + SGR.numberOfVertices;

  // Fill in vertices from this graph.
  long size = numberOfVertices * valence;
  for( long j = 0; j < size; j++ )
    result->table[j] = table[j];

  // Now fill in vertices from SGR, but adjust target vertex numbers.
  long other_size = SGR.numberOfVertices * valence;
  for( long j = 0; j < other_size; j++ )
    if ( SGR.table[j] == -1 )
      result->table[size + j] = -1;
    else
      result->table[size + j] = SGR.table[j] + numberOfVertices;

  return result;
}


// This is a miserably inefficient application of these neat graphs,
// but since we get it almost for free we may as well have it.

void SubgroupGraphRep::joinConjugate(int generator)
  // Join the conjugate of this graph by `generator' to this graph, i.e.
  // make this automaton accept also any conjugate by `generator' of any
  // word it now accepts.
{
  resize(2 * numberOfVertices);  // Make room for conjugate.

  // Copy lower half into upper half, adjusting target vertex numbers.

  long size = numberOfVertices * valence;
  for( long j = 0; j < size; j++ )
    if ( table[j] == -1 )
      table[size + j] = -1;
    else
      table[size + j] = table[j] + numberOfVertices;

  // Fix data members

  VertexType oldNumberOfVertices = numberOfVertices;
  numberOfVertices *= 2;
  delete [] maximalSubtree;
  maximalSubtree = NULL;
  delete [] basisEdges;
  basisEdges = NULL;

  // Sew the two halves together

  LabelType label = generatorToLabel(generator);
  LabelType invLabel = inverseLabel(label);
  VertexType genTarget = targetOfLabel(0, label);
  VertexType invGenTarget = targetOfLabel(0, invLabel);

  if ( (genTarget < 0) && (invGenTarget < 0) ) {
    // No edge labelled by generator or inverse exists at 0 
    // in this graph, so just join by edge.
    defineEdge(0, label, oldNumberOfVertices);
    defineEdge(oldNumberOfVertices, invLabel, 0);
  } else if ( genTarget >= 0 ) {
    identifyVertices(genTarget, oldNumberOfVertices);
    // Positive label exists.
  }
  else {
    identifyVertices(0, oldNumberOfVertices + invGenTarget);
    // Negative label exists.
  }
}



#ifdef DEBUG

void SubgroupGraphRep::debugPrint(ostream& ostr) const
{
  ostr << "SubgroupGraphRep with\n";
  ostr << numberOfEdges() << " edges,\n";
  ostr << numberOfVertices << " vertices,\n";
  ostr << "and valence " << valence << " :\n";

  Bool printAll = !consistentData();

  for( VertexType k = 0; k < numberOfVertices; k++ )
    for( int j = 0; j < valence; j++ )
      if ( targetOfLabel(k, j) != -1 )
	if ( printAll || !( j & 1) ) {
	  ostr << k;
	  ostr << " -- ";
	  ostr << labelToGenerator(j);
	  ostr << " --> ";
	  ostr << targetOfLabel(k, j);
	  ostr << endl;
	}
  ostr << endl << endl;

  for( VertexType k = 0; k < numberOfVertices; k++ ) {
    for( int j = 0; j < valence; j++ ) 
      cout << " " << table[k*valence+j];
    cout << endl;
  }
}


Bool SubgroupGraphRep::consistentData( ) const
{
  // This just checks target numbers and whether every edge has an inverse.
  // There is no good way to check numberOfVertices or valence.

  Bool result = TRUE;
  long i = numberOfVertices * valence;

  for( VertexType v = 0; v < numberOfVertices; v++ )
    for( LabelType l = 0; l < valence; l++ ) {
      VertexType target = targetOfLabel(v, l);
      if ( (target >= numberOfVertices) || (target < -1) ) {
	cerr << "\n*** Bad vertex number: " << target << endl;
	return FALSE;
      }
      if ( target != -1 )
	if ( v != targetOfLabel(target, inverseLabel(l)) ) {
	  cerr << "\n*** edge " << v << " -- " << labelToGenerator(l)
	       << " --> " << target << " lacks an inverse\n";
	  return FALSE;
	}
    }

  return result;
}

#endif


void SubgroupGraphRep::write( ostream& ostr ) const
{
  ostr 
    < Chars("SubgroupGraphRep<") < ' ' 
    < numberOfVertices < spaceForVertices < valence;

  int tableSize = numberOfVertices * valence;
  ostr < tableSize;
  for( int i1 = 0; i1 < tableSize; ++i1 )
    ostr < table[i1];

  ostr < ( maximalSubtree ? 1 : 0 );
  if( maximalSubtree ) 
    for( int i1 = 0; i1 < numberOfVertices; ++i1 )
      ostr < maximalSubtree[i1];

  ostr < ( basisEdges ? 1 : 0 );
  if( basisEdges ) {
    int Rank = rank();
    for( int i1 = 0; i1 < Rank; ++i1 )
      ostr < basisEdges[i1];
  }

  ostr < ' ' < Chars(">SubgroupGraphRep");
}


void SubgroupGraphRep::read( istream& istr )
{
  delete [] table;
  delete [] maximalSubtree;
  delete [] basisEdges;

  Chars header;
  istr > header;
#if SAFETY > 0
  if( header != Chars("SubgroupGraphRep<") ) {
    error("Cannot read SubgroupGraphRep -- bad header of a data section.");
  }
#endif

  istr > numberOfVertices > spaceForVertices > valence;

  int tableSize;
  istr > tableSize;
  table = new VertexType[tableSize];
  for( int in = 0; in < tableSize; ++in )
    istr > table[in];

  int j;
  istr > j;
  if( j ) {
    maximalSubtree = new LabelType[numberOfVertices];
    for( int in = 0; in < numberOfVertices; ++in )
      istr > maximalSubtree[in];
  }
  else 
    basisEdges = 0;
  
  istr > j;
  if( j ) {
    int Rank = rank();
    basisEdges = new Edge[Rank];
    for( int in = 0; in < Rank; ++in )
      istr > basisEdges[in];  
  }
  else
    basisEdges = 0;

  Chars footer;
  istr > footer;
#if SAFETY > 0
  if( footer != Chars(">SubgroupGraphRep") ) {
    error("Cannot read SubgroupGraphRep -- bad footer of a data section.");
  }
#endif
}


bool SubgroupGraphRep::readPiece( istream& istr, const Timer& timer )
{
  bool read = true;

  switch( isReading ) {
  
  case STOP:

    delete [] table;
    delete [] maximalSubtree;
    delete [] basisEdges;
    
    //@dp    // Don't want to read those two.
    //@dp will read these two later.
    maximalSubtree = 0;
    basisEdges = 0;

    {
      Chars header;
      istr > header;
#if SAFETY > 0
      if( header != Chars("SubgroupGraphRep<") ) {
	error("Cannot read SubgroupGraphRep -- bad header of a data section.");
      }
#endif
    }

    istr > numberOfVertices > spaceForVertices > valence;

    istr > tableSize;

#ifdef DEBUG_READ_PIECE
    cerr << endl 
	 << "numberOfVertices=" << numberOfVertices << ", " 
	 << "spaceForVertices=" << spaceForVertices << ", " 
	 << "valence=" << valence << ", "
	 << "tableSize=" << tableSize;
#endif

    table = new VertexType[tableSize];
    isReading = TABLE;
    n = 0;
  
  case TABLE:
    while( !timer.expired() && read ) {
      istr > table[n];
      ++n;
      read = n < tableSize;
    }
    if( read )
      break;

    isReading = SUBTREE;
    int j;
    istr > j;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "table = ";
    for( int q = 0; q < tableSize; ++q ) {
      cerr << table[q] << ' ';
    }
    cerr << endl << "isMaxSubtree =" << j;
#endif

    if( j )
      maximalSubtree = new LabelType[numberOfVertices];
    else 
      maximalSubtree = 0;
    n = 0;
    read = true;
    
  case SUBTREE:
    if( maximalSubtree ) {
      while( !timer.expired() && read ) {
	istr > maximalSubtree[n];
	++n;
	read = n < numberOfVertices;
      }
      if( read )
	break;
    }

    read = true;
    n = 0;
    isReading = BASIS_EDGES;
    int k;
    istr > k;

#ifdef DEBUG_READ_PIECE
    cerr << endl << "maximalSubtree = ";
    if( !maximalSubtree )
      cerr << '0';
    else {
      for( int m = 0; m < numberOfVertices; ++m ) {
	cerr << maximalSubtree[m] << ' ';
      }
    }
    cerr << endl << "isBasisEdges = " << k;
#endif

    if( k ) {
      int Rank = rank();
      basisEdges = new Edge[ Rank ];
    }
    else
      basisEdges = 0;

  case BASIS_EDGES:
    if( basisEdges ) {
      int Rank = rank();
      while( !timer.expired() && read ) {
	istr > basisEdges[n];  
	++n;
	read = n < Rank;
      }
      if( read )
	break;
    }

#ifdef DEBUG_READ_PIECE
    cerr << endl << "basisEdges = ";
    if( !basisEdges )
      cerr << '0';
    else {
      int Rank = rank();
      for( int b = 0; b < Rank; ++b ) {
	cerr < basisEdges[b];
	cerr << ' ';
      }
    }
    cerr << endl;
#endif

    {
      Chars footer;
      istr > footer;
#if SAFETY > 0
      if( footer != Chars(">SubgroupGraphRep") ) {
	error("Cannot read SubgroupGraphRep -- bad footer of a data section.");
      }
#endif
    }

    // finished reading !
    isReading = STOP;
    return true;
  }

  return false;

  //  return !(isReading = (n < tableSize));
}
