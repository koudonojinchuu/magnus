// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the DoubleCosetGraph class.
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


#ifndef _DOUBLE_COSET_GRAPH_H_
#define _DOUBLE_COSET_GRAPH_H_

#include "SubgroupGraph.h"

typedef SubgroupGraphRep::LabelType DCGLabelType;
typedef SubgroupGraphRep::VertexType DCGVertexType;

enum DCGPlace { inH, inU, inK };

struct DCGState {
  
  DCGState( DCGPlace place = inH , 
	DCGVertexType vertex = SubgroupGraphRep::baseVertex )
    : currentPlace( place ), currentVertex( vertex ) { }
  
  DCGState( DCGPlace place, int uCurrent ) :
    currentPlace( place ), UCurrent( uCurrent ) { }

  DCGState& operator = ( const DCGState& state )
  {
    if( (currentPlace = state.currentPlace) == inU )
      UCurrent = state.UCurrent;
    else
      currentVertex = state.currentVertex;
  }

  bool operator == ( const DCGState& state ) const
  {
    if( currentPlace != state.currentPlace )
      return false;
    if( currentPlace == inU ) {
      if( UCurrent != state.UCurrent )
	return false;
    }
    else
      if( currentVertex != state.currentVertex )
	return false;
    
    return true;
  }

  bool operator != ( const DCGState& state ) const
  {
    return !(*this == state);
  }

  friend ostream& operator << ( ostream& ostr, const DCGState& state )
  {
    ostr << "(" << state.currentPlace << ",";
    if( state.currentPlace == inU ) 
      ostr << state.UCurrent;
    else
      ostr << state.currentVertex;
    ostr << ")";
     
    return ostr;
  }

  void assign( DCGPlace place, DCGVertexType vertex)
  {
    currentPlace = place;
    currentVertex = vertex;
  }
    
  void assign( DCGPlace place, int uCurrent )
  {
    currentPlace = place;
    UCurrent = uCurrent;
  }
  
  
  DCGPlace currentPlace;
  union {
    DCGVertexType currentVertex;
    int UCurrent;
  };

};


// ------------------------ GraphConjugacyProblem ---------------------------//


class DoubleCosetGraph 
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  DoubleCosetGraph( const SubgroupGraph& h, const Word& u, 
		    const SubgroupGraph& k, int hWidth, int kWidth);

  ~DoubleCosetGraph();


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool contains( const Word& ) const;

  bool canGo( const DCGState& originState, DCGState& newState, 
	      DCGLabelType label ) const;

  bool canBack( const DCGState& originState, DCGState& newState, 
	        DCGLabelType label ) const;

  bool canAdvancedGo( const DCGState& originState,
		      DCGState& newState, DCGLabelType label ) const;

  bool canAdvancedBack( const DCGState& originState,
		        DCGState& newState, DCGLabelType label ) const;
  
  bool goThroughWord( const Word& W, const DCGState& originState, 
		      DCGState& finishState, int& WCurrent ) const;

  bool findWord( const Word& W, const DCGState& originState, 
		 DCGState& finishState ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  static const DCGVertexType emptyTarget = SubgroupGraphRep::emptyTarget;
  static const DCGVertexType baseVertex = SubgroupGraphRep::baseVertex;
  
  const SubgroupGraph H;
  const SubgroupGraph K;
  const Word U;

private:

  DCGVertexType **subgroupArrows;
  DCGVertexType *wordArrows;
  DCGVertexType **backSubgroupArrows;
  DCGVertexType *backWordArrows;
 
  int HWidth;
  int KWidth;

  DCGVertexType enumOriginVertex;
  int enumWordLength;


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  DoubleCosetGraph( const DoubleCosetGraph& );

  DoubleCosetGraph& operator = ( const DoubleCosetGraph& );

  void joinPieces( DCGVertexType HVertex, DCGVertexType KVertex );
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

public:

  void debugPrint( );

#ifdef DEBUG
  
  friend int main(int, char**);
  
#endif

};


inline bool DoubleCosetGraph::findWord( const Word& W,
	    const DCGState& originState, DCGState& finishState ) const
{
  int WCurrent;
  if( goThroughWord(W, originState, finishState, WCurrent) )
    return true;
  else
    return false;
}


//--------------------------- DCGVertexIterator -----------------------------//


class DCGVertexIterator 
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  DCGVertexIterator( const DoubleCosetGraph& dcg ) : DCG( dcg ) { reset( ); }

  // No default constructor
  // Copy constructor provided by compiler (does logical deep copy).


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) { return bDone; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void reset( );

  DCGState value( ) 
  { 
  #if SAFETY > 0
    if( bDone )
      error("DCGState DCGVertexIterator<K>::value( ) : "
	    "Tried to retrieve value from iterator which is done.");
  #endif  
    
    return state; 
  }

  bool next( );


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const DoubleCosetGraph& DCG;
  DCGState state;
  bool bDone;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
};

#endif
