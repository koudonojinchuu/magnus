// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the GraphConjugacyProblem class.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// Next implementation steps:
//


#ifndef _GRAPH_CONJUGACY_PROBLEM_H_
#define _GRAPH_CONJUGACY_PROBLEM_H_

#include "DoubleCosetGraph.h"
#include "FPGroup.h"
#include "Timer.h"


struct DoubleWayElt {
  
/*
  DoubleWayElt( const DCGState& state1, const DCGState& state2,
		DCGLabelType label = 0 ) 
  {
    assign(state1, state2, label);
  }
*/

  void assign( const DCGState& state1, const DCGState& state2,
	       DCGLabelType label ) 
  {
    leftState = state1;
    rightState = state2;
    currentLabel = label;
  }

  void take( DCGState& state1, DCGState& state2, DCGLabelType& label) 
  {
    state1 = leftState;
    state2 = rightState;
    label = currentLabel;
  }
  
//private:

  DCGState leftState, rightState;
  DCGLabelType currentLabel;
};


// ------------------------ GraphConjugacyProblem ---------------------------//


class GraphConjugacyProblem 
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GraphConjugacyProblem( const FPGroup&, const Word& u, const Word& v );

  ~GraphConjugacyProblem( ) 
  {
    if( !done() )
      finishComputation();
  }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void startComputation( )
  {
  #if SAFETY > 0   
    if ( bStart )
      error("void GraphConjugacyProblem::startComputation( ) : "
	    "the computation has been already started.");
  #endif

    Word t = U.cyclicallyReduce();
    UConjugator = U.terminalSegment( (U.length() - t.length()) / 2 );
    U = t;

    t = V.cyclicallyReduce();
    VConjugator = V.terminalSegment( (V.length() - t.length()) / 2 );
    V = t;

    bStart = true;
    bDone = false;
    isInterrupted = false;
  }  
  // Start the problem: are u and v conjugate ?      
  // You shouldn't call this more than one time.
         
  void continueComputation( const SubgroupGraph& theGraph );
  // Advance the computation of conjugacy problem.

  void continueComputation( );
  // Advance the computation of conjugacy problem.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) const { return bDone; }
  // Is the conjugacy problem done ?
  // If it is done then u and v are conjugate.

  bool theNewGraphIsNeeded( ) const { return I == NULL; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // You can call all these functions iff the computation is finished
  // ( when the done() functions returns true ).


  Word getConjugator( ) 
  {
  #if SAFETY > 0
    if ( !bDone )
      error("Word GraphConjugacyProblem::getConjugator( ) : "
	    "tried to get result before the computation is finished.");
  #endif

    return theConjugator;
  }


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  int numberOfGenerators;
  int maxGeneratorLength;
  Word theConjugator;
  const FPGroup theGroup;
  Word UConjugator;
  Word VConjugator;

  Word U;                  // fisrt argument for conjugacy problem
  Word V;                  // second argument for conjugacy problem
  bool bStart;             // TRUE if the computation is started
  bool bDone;              // TRUE if the computation is finished

  DoubleCosetGraph *DCG;
  DCGVertexIterator *I;
  Timer *timer;
  static const int timerValue = 1000;

  
  // The data which is necessary to divide findConjugator() on arcs.

  bool isInterrupted; 
  DoubleWayElt *way;
  int saveWayIndex;
  int saveLabel;
  DCGState saveLeftState;
  DCGState saveRightState;
  int *leftMarks;
  int *rightMarks;

 
  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  GraphConjugacyProblem( const GraphConjugacyProblem& );
  // It is hidden, not implemented.

  GraphConjugacyProblem& operator = ( const GraphConjugacyProblem& );
  // It is hidden, not implemented.

  void finishComputation( )
  {
    bDone = true;
    // bStart = false;
    theConjugator = theGroup.shortenByRelators(theConjugator);
    theConjugator = VConjugator.inverse() * theConjugator * UConjugator;
    theConjugator = theConjugator.inverse();
    if( isInterrupted )
      finishInterruption();
  }

  bool findConjugator( const DCGState& state1, DCGLabelType label1,
		       const DCGState& state2, DCGLabelType label2, 
		       int length, Word& conjugator);

  bool theConjugatorIsFound( const DCGState& state1, 
                             const DCGState& state2 ) const;

  void mark( int *, const DCGState&, int *, const DCGState&, int );

  void clear( int *, const DCGState&, int *, const DCGState& );

  void getMarks( const int *, const DCGState&, const int *, const DCGState&,
		int&, int& ) const;

  bool weHaveTheSameCycles( int *, const DCGState&, 
			    int *, const DCGState& ) const;

  void finishInterruption( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  friend int main(int, char**);

#endif

};

#endif
