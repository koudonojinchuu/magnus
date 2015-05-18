// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of DoubleCosetGraph class.
//
// Principal Authors: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


//#define debug_DoubleCosetGraph

#include "DoubleCosetGraph.h" 


// --------------------------- DoubleCosetGraph -----------------------------//


DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph& h, const Word& u,
				    const SubgroupGraph& k,
				    int hWidth, int kWidth ) :
    H( h ), U( u ), K( k ), HWidth(hWidth), KWidth(kWidth)
{
  int HVertexCount = H.vertexCount();
  int KVertexCount = K.vertexCount();
  
  //@njz
  //  subgroupArrows = new (DCGVertexType*)[HVertexCount];
  subgroupArrows = new DCGVertexType*[HVertexCount];
  //
  for( DCGVertexType k = 0; k < HVertexCount; ++k ) {
    subgroupArrows[k] = new DCGVertexType[2];
    for( int j = 0; j < 2; ++j )
      subgroupArrows[k][j] = emptyTarget;
  }

  //@njz
  //  backSubgroupArrows = new (DCGVertexType*)[KVertexCount];
  backSubgroupArrows = new DCGVertexType*[KVertexCount];
  //
  for( DCGVertexType k = 0; k < KVertexCount; ++k ) {
    backSubgroupArrows[k] = new DCGVertexType[2];
    for( int j = 0; j < 2; ++j )
      backSubgroupArrows[k][j] = emptyTarget;
  }
    
  int ULen = U.length();
  if( ULen == 0 ) {
    subgroupArrows[baseVertex][1] = baseVertex;
    backSubgroupArrows[baseVertex][1] = baseVertex;
    joinPieces(baseVertex, baseVertex);
    return;
  }

  wordArrows = new DCGVertexType[ULen];
  for( int i = 0; i < ULen; ++i )
    wordArrows[i] = emptyTarget;

  backWordArrows = new DCGVertexType[ULen];
  for( int i = 0; i < ULen; ++i )
    backWordArrows[i] = emptyTarget;

  // connect all right edges

  DCGVertexType vertex = baseVertex;
  for( int i = ULen-1; i >= 0; --i ) {
    if( ( vertex = K.targetOfGenerator(vertex, -ord(U[i])) ) == emptyTarget )
      break;
    else {

     
      if( i == 0 ) {
/*	
      #if DEBUG > 0
	if( subgroupArrows[i][1] != emptyTarget )
	  error("DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph&,"
		"const Word&, const SubgroupGraph& ) : "
		"There is a loss of information in subgroupArrows.");
      #endif
*/
	subgroupArrows[i][1] = vertex;
	joinPieces(i, vertex);
      }
      else {
/*	
      #if DEBUG > 0
	if( wordArrows[i] != emptyTarget )
	  error("DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph&,"
		"const Word&, const SubgroupGraph& ) : "
		"There is a loss of information in wordArrows.");
      #endif
*/
	wordArrows[i] = vertex;
      }
    }
  }

  // connect all left edges

  vertex = baseVertex;
  for( int i = 0; i < ULen; ++i )
    if( ( vertex = H.targetOfGenerator( vertex, ord(U[i]) ) ) == emptyTarget )
      break;
    else {
      
      if( i+1 == ULen ) {
/*
      #if DEBUG > 0
	if( subgroupArrows[vertex][1] != emptyTarget &&
	    subgroupArrows[vertex][1] != 0 )
	  error("DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph&,"
		"const Word&, const SubgroupGraph& ) : "
		"There is a loss of information in subgroupArrows.");
      #endif
*/	
	subgroupArrows[vertex][1] = 0;
	joinPieces(vertex, 0);
      }
      else {
/*	
      #if DEBUG > 0
	if( subgroupArrows[vertex][0] != emptyTarget &&
	    subgroupArrows[vertex][0] != i+1 )
	  error("DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph&,"
		"const Word&, const SubgroupGraph& ) : "
		"There is a loss of information in subgroupArrows.");
      #endif
*/
	subgroupArrows[vertex][0] = i+1;
	DCGVertexType KVertex;
	if( ( KVertex = wordArrows[i+1]) != emptyTarget ) {
/*	  
      #if DEBUG > 0
	  if( subgroupArrows[vertex][1] != emptyTarget &&
	      subgroupArrows[vertex][1] != KVertex )
	    error("DoubleCosetGraph::DoubleCosetGraph( const SubgroupGraph&,"
		  "const Word&, const SubgroupGraph& ) : "
		  "There is a loss of information in subgroupArrows.");
      #endif
*/
	  subgroupArrows[vertex][1] = KVertex;
	  
	  // join all reducable pieces started from this vertex
	  
	  joinPieces(vertex, KVertex);
	}
      }
    }

  // make back trivial edges.

  for( DCGVertexType k = 0; k < KVertexCount; ++k ) {
    vertex = subgroupArrows[k][1];
    if( vertex != emptyTarget )
      backSubgroupArrows[vertex][1] = k;
    vertex = subgroupArrows[k][0];
    if( vertex != emptyTarget )
      backWordArrows[vertex] = k;
  }

  for( int i = 0; i < ULen; ++i ) {
    vertex = wordArrows[i];
    if( vertex != emptyTarget )
      backSubgroupArrows[vertex][0] = i;
  }
}


DoubleCosetGraph::~DoubleCosetGraph() 
{
  if( U.length() != 0 ) {
    delete [] wordArrows;
    delete [] backWordArrows;
  }

  for( DCGVertexType i = 0; i < H.vertexCount(); ++i ) {
    delete [] subgroupArrows[i];
    delete [] backSubgroupArrows[i];
  }

  delete [] subgroupArrows;
  delete [] backSubgroupArrows;
}


void DoubleCosetGraph::joinPieces( DCGVertexType HVertex,
				   DCGVertexType KVertex )
{
  DCGLabelType *way = new DCGLabelType[HWidth];
  DCGVertexType curHVertex = HVertex;
  DCGVertexType curKVertex = KVertex;
  int current = 0, origin = 0;
  DCGVertexType newHVertex, newKVertex;

  while( true ) {
    for( DCGLabelType i = origin; i < H.getValence(); ++i ) 
      if( ( newHVertex = H.targetOfLabel(curHVertex, i) )
	  != emptyTarget )
	if( ( newKVertex = K.targetOfLabel(curKVertex, i) ) != emptyTarget ) {
	  
	  if( current > 0 )
	    if( newHVertex ==
		H.targetOfLabel(curHVertex, H.inverseLabel(way[current-1])))
	      continue;
		
	  if( subgroupArrows[newHVertex][1] != emptyTarget )
	    continue;
/*	  
        #if DEBUG > 0
	  DCGVertexType tmpVertex = subgroupArrows[newHVertex][1];
	  if( tmpVertex != emptyTarget && tmpVertex != newKVertex )
	    error("DoubleCosetGraph::joinPieces( DCGVertexType,
	                                         DCGVertexType ) : "
		  "There is a loss of information in subgroupArrows.");
        #endif
*/	  
	  subgroupArrows[newHVertex][1] = newKVertex;
	  way[current++] = i;
	  origin = 0;
	  curHVertex = newHVertex;
	  curKVertex = newKVertex;
	  i = 0;
	  if( curHVertex == baseVertex )
	    break;
	  else
	    continue;
	}
    if( current == 0 )
      break;
    else {
      origin = way[--current];
      curHVertex = H.targetOfLabel(curHVertex, H.inverseLabel(origin));
      curKVertex = K.targetOfLabel(curKVertex, K.inverseLabel(origin));
      ++origin;
    }
  }
}


bool DoubleCosetGraph::canAdvancedGo( const DCGState& originState,
     DCGState& newState, DCGLabelType label ) const
{
  DCGState state;
  if( canGo(originState, newState, label ) )
    return true;

  if( canGo(originState, state, -2 ) )
    if( canGo(state, newState, label ) )
      return true;

  if( canGo(originState, state, -1 ) )
    if( canGo(state, newState, label ) )
      return true;

  return false;
}


bool DoubleCosetGraph::canAdvancedBack( const DCGState& originState,
     DCGState& newState, DCGLabelType label ) const
{
  DCGState state;
  if( canBack(originState, newState, label ) )
    return true;

  if( canBack(originState, state, -2 ) )
    if( canBack(state, newState, label ) )
      return true;

  if( canBack(originState, state, -1 ) )
    if( canBack(state, newState, label ) )
      return true;

  return false;
}


bool DoubleCosetGraph::canGo( const DCGState& originState, DCGState& newState,
			      DCGLabelType label ) const
{
  DCGVertexType vertex;
  int ULen = U.length();

  switch( originState.currentPlace ) {
  case inH : 
    if( label < 0 )
      if( label == -2 )
	if( ( vertex = subgroupArrows[originState.currentVertex][0] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inU, vertex);
	  return true;
	}
      else
	if( ( vertex = subgroupArrows[originState.currentVertex][1] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inK, vertex);
	  return true;
	}
    else
      if( ( vertex = H.targetOfLabel(originState.currentVertex, label) ) ==
	  emptyTarget)
	if( originState.currentVertex == baseVertex && ULen > 0
	    && U[0] == H.labelToGenerator(label) ) {

	  if( ULen == 1 )
	    newState.assign(inK, baseVertex);
	  else
	    newState.assign(inU, 1);

	  return true;
	}
	else
	  return false;
      else {
	newState.assign(inH, vertex);
	return true;
      }
    
  case inU :
    if( label < 0 )
      if( label == -2 )
	return false;
      else
	if( ( vertex = wordArrows[originState.UCurrent] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inK, vertex);
	  return true;
	}
    else
      if( ( U[originState.UCurrent] ) == H.labelToGenerator(label) )
	if( originState.UCurrent == ULen - 1 ) {
	  newState.assign(inK, baseVertex);
	  return true;
	}
	else {
	  newState.assign(inU, originState.UCurrent + 1);
	  return true;
	}
      else
	return false;
    
  case inK :
    if( label < 0 )
      return false;
    else
      if( ( vertex = K.targetOfLabel(originState.currentVertex, label) ) ==
	  emptyTarget)
	return false;
      else {
	newState.assign(inK, vertex);
	return true;
      }
  }
  
}


bool DoubleCosetGraph::canBack( const DCGState& originState,
				DCGState& newState,
				DCGLabelType label ) const
{
  DCGVertexType vertex;
  int ULen = U.length();

  switch( originState.currentPlace ) {
  case inK : 
    if( label < 0 )
      if( label == -2 )
	if( ( vertex = backSubgroupArrows[originState.currentVertex][0] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inU, vertex);
	  return true;
	}
      else
	if( ( vertex = backSubgroupArrows[originState.currentVertex][1] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inH, vertex);
	  return true;
	}
    else
      if( ( vertex = K.targetOfLabel(originState.currentVertex, label) ) ==
	  emptyTarget)
	if( originState.currentVertex == baseVertex && ULen > 0
	    && U[ULen-1] == K.labelToGenerator(label) ) {

	  if( ULen == 1)
	    newState.assign(inH, baseVertex);
	  else 
	    newState.assign(inU, ULen-1);

	  return true;
	}
	else
	  return false;
      else {
	newState.assign(inK, vertex);
	return true;
      }
    
  case inU :
    if( label < 0 )
      if( label == -2 )
	return false;
      else
	if( ( vertex = backWordArrows[originState.UCurrent] ) ==
	    emptyTarget)
	  return false;
	else {
	  newState.assign(inH, vertex);
	  return true;
	}
    else
      if( ( U[originState.UCurrent - 1] ) == -K.labelToGenerator(label) )
	if( originState.UCurrent == 1 ) {
	  newState.assign(inH, baseVertex);
	  return true;
	}
	else {
	  newState.assign(inU, originState.UCurrent - 1);
	  return true;
	}
      else
	return false;
    
  case inH :
    if( label < 0 )
      return false;
    else
      if( ( vertex = H.targetOfLabel(originState.currentVertex, label) ) ==
	  emptyTarget)
	return false;
      else {
	newState.assign(inH, vertex);
	return true;
      }
  }
  
}


bool DoubleCosetGraph::goThroughWord( const Word& W,
     const DCGState& originState, DCGState& finishState, int& WCurrent ) const
{
  int WLen = W.length();

  if( WLen == 0 ) {
    finishState = originState;
    return true;
  }

  // W is a non trivial word

  DCGState state = originState;
  DCGState nextState, nextNextState;
  
  for( int i = 0; i < WLen; ++i ) {

    int label = H.generatorToLabel( ord(W[i]) );

    if( canGo(state, nextState, label) ) {
      state = nextState;
      continue;
    }

    if( canGo(state, nextState, -2) )
      if( canGo( nextState, nextNextState, label ) ) {
	state = nextNextState;
	continue;
      }

    if( canGo(state, nextState, -1) )
      if( canGo( nextState, nextNextState, label ) ) {
	state = nextNextState;
	continue;
      }
	    
    WCurrent = i;
    return false;
  }

  finishState = state;
  return true;
}


bool DoubleCosetGraph::contains( const Word& W ) const
{
  DCGState originState, finishState, nextState;
  if( !findWord(W, originState, finishState) )
    return false;

  if( finishState.currentPlace == inU )
    return false;

  if( finishState.currentPlace == inK )
    if( finishState.currentVertex == baseVertex )
      return true;
    else
      return false;
  
  if( !canGo(finishState, nextState, -1) )
    return false;

  if( nextState.currentVertex != baseVertex )
    return false;

  return true;
}


void DoubleCosetGraph::debugPrint()
{
#ifdef debug_DoubleCosetGraph

  H.debugPrint(cout);
  
  cout << "subgroupArrows: " << endl;
  for( DCGVertexType i = 0; i < H.vertexCount(); ++i ) {
    for( int j = 0; j < 2; ++j )
      cout << " " << subgroupArrows[i][j];
    cout << endl;
  }

  cout << "wordArrows: " << endl;
  int ULen = U.length();
  for( int j = 0; j < ULen; ++j )
    cout << " " << wordArrows[j];
  cout << endl;

  cout << "backSubgroupArrows: " << endl;
  for( i = 0; i < K.vertexCount(); ++i ) {
    for( int j = 0; j < 2; ++j )
      cout << " " << backSubgroupArrows[i][j];
    cout << endl;
  }

  cout << "backWordArrows: " << endl;
  for( j = 0; j < ULen; ++j )
    cout << " " << backWordArrows[j];
  cout << endl;

#endif
}



//--------------------------- DCGVertexIterator -----------------------------//


void DCGVertexIterator::reset( )
{
  bDone = false;
  state = DCGState(inH, DCG.baseVertex );
}


bool DCGVertexIterator::next( )
{
#if SAFETY > 0
  if( bDone )
    error("bool DCGVertexIterator::next( ) : "
	  "Tried to call next() when iterator is done.");
#endif

  switch( state.currentPlace ) {
    
  case inH:
    if( state.currentVertex == DCG.H.vertexCount() - 1 ) 
      if( DCG.U.length() > 1 ) {
	state.assign(inU, 1);
	return true;
      }
      else {
	state.assign(inK, DCG.baseVertex);
	return true;
      }
    else {
      ++state.currentVertex;
      return true;
    }

  case inU:
    if( state.UCurrent == DCG.U.length()-1 ) {
      state.assign(inK, DCG.baseVertex);
      return true;
    }
    else {
      ++state.UCurrent;
      return true;
    }   
    
  case inK:  
    if( state.currentVertex == DCG.K.vertexCount() - 1 ) {
      bDone = true;
      return false;
    }
    else {
      ++state.currentVertex;
      return true;
    }   
  }
}



