// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of GraphConjugacyProblem class.
//
// Principal Authors: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


//#define debug_GraphConjugacyProblem

#include "GraphConjugacyProblem.h" 


// ------------------------ GraphConjugacyProblem ---------------------------//

GraphConjugacyProblem::GraphConjugacyProblem( const FPGroup& G,
    const Word& u, const Word& v)
    :
    U( u.freelyReduce() ), V( v.freelyReduce() ),
    numberOfGenerators( G.numberOfGenerators() ), theConjugator(),
    theGroup(G), UConjugator(), VConjugator(), I(NULL),
    bStart( false ), bDone( false )
{ 
  maxGeneratorLength = 0;
  SetIterator<Word> I(G.getRelators());
  for( ; !I.done(); I.next() ) {
    int len = I.value().length();
    if( len > maxGeneratorLength )
      maxGeneratorLength = len; 
  }
}


bool GraphConjugacyProblem::theConjugatorIsFound( const DCGState& state1,
						  const DCGState& state2) const
{
  DCGState newState;
  if( state1 != DCGState(inH, DCG->baseVertex) )
    if( DCG->canBack(state1, newState, -1) ) {
      if( newState != DCGState(inH, DCG->baseVertex) )
	return false;
    }
    else
      return false;
    
  if( state2 != DCGState(inK, DCG->baseVertex) )
    if( DCG->canGo(state2, newState, -1) ) {
      if( newState != DCGState(inK, DCG->baseVertex) )
	return false;
    }
    else
      return false;

  return true;
}


void GraphConjugacyProblem::mark(
     int *leftMarks, const DCGState& leftState,
     int *rightMarks, const DCGState& rightState, int markValue )
{
  int numOfVertices = DCG->H.vertexCount();

  switch( leftState.currentPlace )
  {
  case inH:
    leftMarks[leftState.currentVertex] = markValue;
    break;
    
  case inU:
    leftMarks[2 * numOfVertices + leftState.UCurrent] = markValue;
    break;
    
  case inK:
    leftMarks[numOfVertices + leftState.currentVertex] = markValue;
  }

  switch( rightState.currentPlace )
  {
  case inH:
    rightMarks[rightState.currentVertex] = markValue;
    break;
    
  case inU:
    rightMarks[2 * numOfVertices + rightState.UCurrent] = markValue;
    break;
    
  case inK:
    rightMarks[numOfVertices + rightState.currentVertex] = markValue;
  }
}


void GraphConjugacyProblem::clear(
     int *leftMarks, const DCGState& leftState,
     int *rightMarks, const DCGState& rightState )
{
  mark(leftMarks, leftState, rightMarks, rightState, 0);
}


void GraphConjugacyProblem::getMarks(
     const int *leftMarks, const DCGState& leftState,
     const int *rightMarks, const DCGState& rightState,
     int& leftValue, int& rightValue ) const
{
  int numOfVertices = DCG->H.vertexCount();

  switch( leftState.currentPlace )
  {
  case inH:
    leftValue = leftMarks[leftState.currentVertex];
    break;
    
  case inU:
    leftValue = leftMarks[2 * numOfVertices + leftState.UCurrent];
    break;
    
  case inK:
    leftValue = leftMarks[numOfVertices + leftState.currentVertex];
  }

  switch( rightState.currentPlace )
  {
  case inH:
    rightValue = rightMarks[rightState.currentVertex];
    break;
    
  case inU:
    rightValue = rightMarks[2 * numOfVertices + rightState.UCurrent];
    break;
    
  case inK:
    rightValue = rightMarks[numOfVertices + rightState.currentVertex];
  }

}


bool GraphConjugacyProblem::weHaveTheSameCycles(
     int *leftMarks, const DCGState& leftState,
     int *rightMarks, const DCGState& rightState ) const
{
  int leftValue, rightValue;
  getMarks(leftMarks, leftState, rightMarks, rightState,
	   leftValue, rightValue);
  return( leftValue != 0 && leftValue == rightValue );
}


void GraphConjugacyProblem::finishInterruption()
{
  delete [] leftMarks;
  delete [] rightMarks;
  delete [] way;
  isInterrupted = false;
}


bool GraphConjugacyProblem::findConjugator( const DCGState& state1,
     DCGLabelType label1, const DCGState& state2, DCGLabelType label2,
     int length, Word& conjugator )
{
  int wayIndex, label;
  DCGState leftState, rightState, nextLeftState, nextRightState;

  DCGLabelType valence = DCG->H.getValence();

  if( isInterrupted ) {
    wayIndex = saveWayIndex;
    leftState = saveLeftState;
    rightState = saveRightState;
    label = saveLabel;
  }
  else {
    
    if( theConjugatorIsFound(state1, state2) ) {
      conjugator = Word();
      return true;
    }
    
    way = new DoubleWayElt[length];
    wayIndex = 0;

    leftState = state1;
    rightState = state2;
    label = 0;

    DCGVertexType size = DCG->H.vertexCount() * 2 + DCG->U.length();
    leftMarks = new int[size];
    rightMarks = new int[size];
    memset(leftMarks, 0, size);
    memset(rightMarks, 0, size);
    mark(leftMarks, state1, rightMarks, state2, wayIndex + 1);
  }
  
  while(true) {

    if( timer->expired() ) {
      saveWayIndex = wayIndex;
      saveLeftState = leftState;
      saveRightState = rightState;
      saveLabel = label;
      isInterrupted = true;
      return false;
    }
 
    if( DCG->canAdvancedBack( leftState, nextLeftState, label) )
      if( leftState != state1 || label != label1 )
	if( DCG->canAdvancedGo( rightState, nextRightState, label ) )
	  if( rightState != state2 || label != label2 ) {

	    if( wayIndex > 0 &&
		DCG->H.inverseLabel(way[wayIndex-1].currentLabel) == label )
		;
	    else {
	      if( theConjugatorIsFound(nextLeftState, nextRightState) ) {

		//calculate the conjugator
		
		way[wayIndex++].assign(leftState, rightState, label); 
		VectorOf<Generator> V(wayIndex);
		for( int i = 0; i < wayIndex; ++i )
		  V[i] = DCG->H.labelToGenerator(way[i].currentLabel);
		conjugator = Word(V);
		
		finishInterruption();
		return true;
	      }
	      if( wayIndex != length - 1 &&
		  !weHaveTheSameCycles(leftMarks, nextLeftState,
				       rightMarks, nextRightState) )
	      {
		way[wayIndex++].assign(leftState, rightState, label);
		leftState = nextLeftState;
		rightState = nextRightState;
		label = 0;
		mark(leftMarks, leftState, rightMarks, rightState, wayIndex+1);
		continue;
	      }
	    }
	  }
	    
    ++label;
    while( label >= valence )
      if( wayIndex == 0) {
	finishInterruption();
	return false;
      }
      else {
	clear(leftMarks, leftState, rightMarks, rightState);
	way[--wayIndex].take(leftState, rightState, label);
	++label;
      }
  }
}


void GraphConjugacyProblem::continueComputation(
     const SubgroupGraph& theGraph )
{
#if SAFETY > 0
  if ( !bStart )
    error("void GraphConjugacyProblem::continueComputation( const "
	  "SubgroupGraph& ) : "
	  "tried to continue computation before it's started");
  if ( I != NULL )
    error("void GraphConjugacyProblem::continueComputation( const "
	  "SubgroupGraph& ) : "
	  "tried to use the new graph when the old one was existing.");
#endif

  if ( bDone )
    return;
  
  DCG = new DoubleCosetGraph (theGraph, U, theGraph,
			      maxGeneratorLength, maxGeneratorLength);
  DCG->debugPrint();

  DCGState state, state1;
  
  if( V.length() == 0 ) {
    if( DCG->canGo(state, state1, -1) )
      if( state1.currentVertex == DCG->baseVertex ) {
	finishComputation();
	delete DCG;
	return;
      }
    maxGeneratorLength += 2;
    delete DCG;
    return;
  }

  
  I = new DCGVertexIterator(*DCG);
  timer = new Timer(0);
  continueComputation();
}


void GraphConjugacyProblem::continueComputation( )
{
#if SAFETY > 0
  if ( !bStart )
    error("void GraphConjugacyProblem::continueComputation( ) : "
	  "tried to continue computation before it's started");
  if ( I == NULL )
    error("void GraphConjugacyProblem::continueComputation( ) : "
	  "tried to continue computation without any existed graph.");
#endif

  if ( bDone )
    return;
  
  DCGState state, state1, state2;
  int VLen = V.length();
  int length = maxGeneratorLength + V.length();
  Word conjugator;
  int VCurrent;
  timer->reset(timerValue);
  
  for( ; !I->done(); I->next() ) {
//    if( timer->expired() )
//      break;
    state = I->value();
    if( DCG->canGo( state, state1, DCG->H.generatorToLabel(ord(V[0])) ) )
      if( DCG->goThroughWord(V, state, state1, VCurrent) ) {
	if( findConjugator(state, DCG->H.generatorToLabel(ord(V[0])),
			   state1, DCG->H.generatorToLabel(-ord(V[VLen-1])),
			   length, theConjugator) )
	{
	  finishComputation();
	  return;
	}
	else
	  if( isInterrupted )
	    return;
      }
      else {
	Word w = (V.terminalSegment(VCurrent)).inverse();
	if( DCG->findWord(w, state, state2) )
	  if( findConjugator( state2,
		  DCG->H.generatorToLabel(ord(V[VCurrent])),
		  state1, DCG->H.generatorToLabel(-ord(V[VCurrent-1])),
		  length, conjugator) )
	  {
	    theConjugator = w * conjugator;
	    finishComputation();
	    return;
	  }
	  else
 	    if( isInterrupted )
	      return;
     }
    else
      if( DCG->canGo( state, state1,
		      DCG->H.generatorToLabel(-ord(V[VLen-1])) ) )
	if( !DCG->goThroughWord(V.inverse(), state, state1, VCurrent) ) {
	  VCurrent = VLen - VCurrent;
	  Word w = V.initialSegment(VCurrent + 1);
	  if( DCG->findWord(w, state, state2) )
	    if( findConjugator(state1,
		    DCG->H.generatorToLabel(ord(V[VCurrent])),
		    state2, DCG->H.generatorToLabel(-ord(V[VCurrent-1])),
		    length, conjugator) )
	    {
	      theConjugator = w * conjugator;
	      finishComputation();
	      return;
	    }
	    else
	      if( isInterrupted )
		return;
	}
  }	

  if( I->done() ) {
    delete timer;
    delete I;
    I = NULL;
    delete DCG;
    maxGeneratorLength += 2;
  }

}


