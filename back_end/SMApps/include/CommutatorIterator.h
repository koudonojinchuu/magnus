// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class CommutatorIterator
//
// Principal Author: Dmitry Bormotov
//
// Status: useable
//
// Revision History:
//


#ifndef _CommutatorIterator_h_
#define _CommutatorIterator_h_


// ------------------------ CommutatorIterator ----------------------------- //


class CommutatorIterator {

public:


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor.

  // Copy constructor, operator=, and destructor supplied by compiler.
  
  CommutatorIterator( int numberOfGenerators, int length ) : 
    theLength( length ), numbers( length ), bDone( false ), 
    theWords( numberOfGenerators )
  { 
  #if SAFETY > 0
    if( numberOfGenerators <= 0 || theLength < 2 )
      error("CommutatorIterator( int numberOfGenerators, int length ) : "
	    "bad parameters.");
  #endif

    for( int i = 0; i < numberOfGenerators; ++i )
      theWords[i] = Word(Generator(i+1));
    reset();
  }

  CommutatorIterator( const VectorOf<Word>& V, int length ) : 
    theLength( length ), numbers( length ), bDone( false ), theWords( V )
  { 
  #if SAFETY > 0
    if(  V.length() <= 0 || theLength < 2 )
      error("CommutatorIterator(  const VectorOf<Word>& V, int length ) : "
	    "bad parameters.");
  #endif

    reset();
  }
  

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word value( ) const {
  #if SAFETY > 0
    if( done() )
      error("tried to retrieve value from SymmetricRelatorsIterator "
	    "which is done");
  #endif
    return current;
  }
  // Returns the current commutator.
  //Calling this is a fatal error if done().

  
  bool next( ) {

    if( bDone ) return false;
    
    for(int i = theLength-1; i >= 0; --i ) {

      numbers[i] = int(numbers[i]) + 1;
      
      if( numbers[i] <= theWords.length() ) {
	current = makeCommutator();
	return bDone;
      }
      
      numbers[i] = 1;
    }
    
    return (bDone = true);
  }
  // Advances this iterator.
  // Returns value of done().

  
  bool done( ) const { return bDone; }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.
  
  void reset( )
  {
    for(int i = 0; i < theLength; ++i )
      numbers[i] = 1;
    bDone = false;
    current = makeCommutator();
  }
  // Resets this iterator to the state it was in after construction.

  VectorOf<int> components() { return numbers; }
  // Returns all components of the current commutator.  F.e. for the
  // commutator [a,b,c] it returns {a,b,c}.
  

private:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private functons:                                //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word makeCommutator( )
  {
    Word result = theWords[int(numbers[0])-1];
    for( int i = 1; i < theLength; ++i )
      result = result.inverse() * Word(theWords[int(numbers[i])-1]).inverse()
	* result * theWords[int(numbers[i])-1];
    return result.freelyReduce();
  }

  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  bool bDone;
  int theLength;
  Word current; // current commutator
  VectorOf<int> numbers;
  VectorOf<Word> theWords;
};

#endif
