// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of the GeneralWhitehead class.
//
// Principal Authors: Dmitry Bormotov
//
// Status: 
//
// Revision History:
//
// Special Remarks:
//


#ifndef _GENERAL_WHITEHEAD_H_
#define _GENERAL_WHITEHEAD_H_


#include "Map.h"
#include "FreeGroup.h"
#include "Timer.h"
#include "File.h"


//#define debug_GeneralWhitehead


// -------------------------- GeneralWhitehead ---------------------------- //


class GeneralWhitehead 
{

public:

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GeneralWhitehead( const FreeGroup& F, bool keepDetails = false );

  ~GeneralWhitehead( );
      

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void startComputation( const VectorOf<Word>& );
  // Start the computation. 
  // You shouldn't call this more than ones.

  void startComputation( const Word& );
  // The same as previsious one, but for one word only.
             
  bool continueComputation( );
  // Advance the computation. 
  // You have to call startComputation() before.
  // This function returns true if the computation is done.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) const { return bDone; }
  // true only when the computation is finished.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // You can call all these functions iff the computation is finished
  // ( when the done() functions returns true ).

  bool extendsToFreeBasis( ) 
  {
  #if SAFETY > 0
    if ( !bDone )
      error("bool WhiteheadAutomorphisms::extendToFreeBasis( ) : "
	    "tried to get result before the computation is finished.");
  #endif

    return doesExtend;
  }

  Map getAutomorphism( )
  {
  #if SAFETY > 0
    if( !extendsToFreeBasis() )
      error("Map WhiteheadAutomorphisms::getAutomorphism( ) : "
	    "the automorphism doesn't exist.");
  #endif

    return automorphism;
  }

  Chars getFileName( )
  {
   #if SAFETY > 0
    if( !file )
      error("Chars GeneralWhitehead::getFileName( ) : "
	    "The file has not been created.");
  #endif

    return file->getFileName();
  }
  // Returns name of the file with computation details
  

private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const int timerValue = 1000;

  bool bDone;
  bool bStart;
  bool doesExtend;
  bool itIsInterrupted;
  Timer timer;

  const FreeGroup theGroup;
  int numberOfGenerators;
  VectorOf<Word> images;
  int *numOfAuto;
  Map automorphism;
  VectorOf<Word> theVector;
  int theVectorLen;
  int theVectorCommonLength;
  int saveN;

  File* file;

  bool hasOneWordOnly;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GeneralWhitehead( const GeneralWhitehead& );
  // It is hidden, not implemented.

  GeneralWhitehead& operator = ( const GeneralWhitehead& );
  // It is hidden, not implemented.

  Word makeWhiteheadAutomorpism( Generator, int, Generator ) const;

  void prepareAutomorphism( Map& automorphism ) const;

  void setResult( bool DoesExtend );

  void finishComputation( bool DoesExtend );

  int commonLength( const VectorOf<Word>& );
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Debugging stuff:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef debug_GeneralWhitehead

  int numOfShortenings;

  friend int main(int, char** );

#endif

};


inline void GeneralWhitehead::finishComputation( bool DoesExtend )
{
  delete [] numOfAuto;
  numOfAuto = 0;
  setResult( DoesExtend );
}

#endif

