// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of utility class 
//           EnumeratorOfWordTuples.
//
// Principal Authors: Eugeny Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special remarks:
//
// * Class EnumeratorOfWordTuple is implemented for to allow enumerating
//   of tuple of word which may contain different number of generators.
//
//   It implicitly uses techniques of meta-enumerator: 
//   enumerator of enumerators. Low level enumerators should supply only
//   two operations: reset(restart) and get a next element.
//   @dp do we want meta-enumerator at all?
//
// * Efficiency of the class depends on ability of Word::nextFreelyReduced()
//   to effectively produce next freely reduced word.
//
// * Usage:
//
//   Semantics of using of class EnumeratorOfWordTuple is like iterator.
//   
//   Example of use:
//
//   FreeGroup F2(2);
//   FreeGroup F3(3);
//   EnumeratorOfWordTuple tuple(3, 2);
//
//   // Enumerating of maps F3 to F2:
//   while( 1 ) {
//      VectorOf<Word> images = tuple.value();
//      Map M(F3, F2, images);
//      cout << M << endl;
//      ewt.next();
//   }
//
//  More complex example:
//    Enumerating of maps from F3 into F2 and from F2 into F3 simultaneously.
//
//	VectorOf<int> gens(5);
//	gens[0]=gens[1]=gens[2]= 2; // 3 gens in the domain, 2 gens in the range
//	gens[3]=gens[4]= 3;         // 2 gens in the domain, 3 gens in the range
//	EnumeratorOfWordTuples tuples(gens);
//	for(i = 0; i<100; i++) {
//		VectorOf<Word> images = tuples.value();
//		Map M32(F3,F2);
//		for(int j = 0; j<3; j++)
//			M32.setGeneratingImages(j, images[j]);
//		cout << "F3->F2: " << M32 << "; ";
//		Map M23(F2, F3);
//		for(j = 0; j<2; j++)
//			M23.setGeneratingImages(j, images[j+3]);
//		cout << "F2->F3: " << M23 << endl;
//		tuples.next();
//	}
//
//

#ifndef _TUPLE_ENUMERATOR_H_
#define _TUPLE_ENUMERATOR_H_

#include "Vector.h"
#include "Word.h"

class EnumeratorOfWordTuples
{
public:

	///////////////////////////////////////////////////////////////////
	//                                                               //
	// Constructors                                                  //
	//                                                               //
	///////////////////////////////////////////////////////////////////

  // no default	constructor

	EnumeratorOfWordTuples(VectorOf<int> numOfGens) :
		tupleSize(numOfGens.length()), tuple(numOfGens.length()),
		maxWord(numOfGens.length()), numberOfGens(numOfGens),
		fixedWord(0) {}

	EnumeratorOfWordTuples(int size, int numOfGens) :
		tupleSize(size), tuple(size), maxWord(size),
		numberOfGens(size), fixedWord(0)
	{
		for(int i=0; i<size; i++)
			numberOfGens[i] = numOfGens;
	}

	// copy constructor, operator=, and destructor provided by compiler.

	///////////////////////////////////////////////////////////////////
	//                                                               //
	// Methods                                                       //
	//                                                               //
	///////////////////////////////////////////////////////////////////

	void reset() {
		for(int i=0; i<tupleSize; i++)
			tuple[i] = maxWord[i] = Word();
		fixedWord = tupleSize;
	}
  // Resets this enumerator to the state it was in after construction.

	void next();
	// Advances this enumerator.

	VectorOf<Word> value() const  { return tuple; }
  // Returns the current value.

private:

	// Data members:

	int tupleSize;

	VectorOf<Word> tuple;
	VectorOf<Word> maxWord;
	VectorOf<int>  numberOfGens;

	int fixedWord;

};

class CantorEnumeration {
public:
  static Integer encodePair( const Integer& x, const Integer& y);

  static void decodePair( const Integer& n, Integer& x, Integer& y );

  static Integer encodeTuple( const VectorOf<Integer>& tuple );

  static VectorOf<Integer> decodeTuple( const Integer& number );
};


#endif

