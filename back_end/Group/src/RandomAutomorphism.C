// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes RandomAutomorphism
//
// Principal Author: Sergei Lioutikov
//
// Status: in progress
//
// Revision History:
//


#include "RandomAutomorphism.h"


//---------------------------------------------------------------------------//
//------------------------- RandomAutomorphism ------------------------------//
//---------------------------------------------------------------------------//


RandomAutomorphism::RandomAutomorphism( const FreeGroup& F, int seed )
  :
  theGroup( F ),
  numberOfGroupGens( F.numberOfGenerators() ),
  numGensPicker(seed),
  typeGenPicker(seed)
{ }


RandomAutomorphism::~RandomAutomorphism( )
{

}


VectorOf<Word> RandomAutomorphism::getGeneratingVector( int avgNumGens )
{

  // Start with identity automorphism:
  VectorOf<Word> images(numberOfGroupGens);
  for( int k = 0; k < numberOfGroupGens; ++k )
	 images[k] = Word(Generator(k+1));

  int numGens;
  do {
	 numGens = numGensPicker.rand(avgNumGens, avgNumGens >> 1);
  } while ( numGens < 1 );
  // Number of Whitehead generators to use.
  //@rn Hard-wire standard deviation for now.

  while ( numGens-- ) {
	 // Randomly choose a Whitehead generator and apply it to images.
	 // First switch on kind of generator:
	 // 0: Xi --> Xi Xj, i != j
	 // 1: Xi --> Xj Xi, i != j
	 // 2: Xi <-> Xj, i != j
	 // 3: Xi --> Xi^-1


	 int kind = typeGenPicker.rand(0, 3);
	 if ( kind != 3 ) {

		// OK, which i and j?

		int j, i = typeGenPicker.rand(1, numberOfGroupGens);
		do {
		  j = typeGenPicker.rand(1, numberOfGroupGens);
		} while ( i == j );

		if ( kind == 0 ) {	 // 0: Xi --> Xi Xj, i != j

		  Generator g(i);
		  Word w = g * Generator(j);
		  for( k = 0; k < numberOfGroupGens; ++k )
			 images[k] =
				(((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();

		} else if ( kind == 1 ) {	 // 1: Xi --> Xj Xi, i != j

		  Generator g(i);
		  Word w = Generator(j) * g;
		  for( k = 0; k < numberOfGroupGens; ++k )
			 images[k] =
				(((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();

		} else {	 // 2: Xi <-> Xj, i != j

		  // Pretty kludgy:

		    VectorOf<Word> transp(numberOfGroupGens);
			 for( k = 0; k < numberOfGroupGens; ++k )
				transp[k] = Word(Generator(k+1));
			 transp[i-1] = Word(Generator(j));
			 transp[j-1] = Word(Generator(i));
			 for( k = 0; k < numberOfGroupGens; ++k )
				images[k] =
				  (((Word)images[k]).replaceGenerators( transp )).freelyReduce();

		}

	 } else {	 // 3: Xi --> Xi^-1

		int i = typeGenPicker.rand(1, numberOfGroupGens);
		Generator g(i);
		Word w( inv(g) );
		for( k = 0; k < numberOfGroupGens; ++k )
		  images[k] =
			 (((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();

	 }
  }   // while

  // Now make Map from images and output it:
  // Map m(theGroup.getFreePreimage(), theGroup.getFreePreimage(), images);

  return images;
}

Map RandomAutomorphism::getAutomorphism( int avgNumGens )
{
  return Map(theGroup, theGroup, getGeneratingVector( avgNumGens ));
}
