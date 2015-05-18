// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class RandomAutoInFree
//
// Principal Author: Alexey Myasnikov
//
// Status: in progress
//
// Revision History:
//


#include "RandomAutoInFree.h"


//---------------------------------------------------------------------------//
//--------------------------- RandomAutoInFree ------------------------------//
//---------------------------------------------------------------------------//


RandomAutoInFree::RandomAutoInFree(const FreeGroup& F, int avgNumGens):
	 theGroup( F ),
	 numberOfGroupGens( F.numberOfGenerators() ),
	 numGensPicker(2112),                         //@rn Hard-wire seed for now.
	 typeGenPicker(1812),                         //@rn Hard-wire seed for now.
	 avgNumGens( avgNumGens )
{ }

Map RandomAutoInFree::getAutomorphism( )
{
  // If no  generators there is 1->1 map  only:
  if (numberOfGroupGens == 0)
    return Map();
  
  // If only 1 generator then there are only two automorphisms:
  // g->g and g->g^-1
  if (numberOfGroupGens == 1){
    int firstOne = typeGenPicker.rand(0, 1);
    VectorOf<Word> images(numberOfGroupGens);
    if (firstOne){
      images[0] = Word(Generator(1));
      return Map(theGroup,theGroup,images);
    }else{
      images[0] = Word(inv(Generator(1)));
      return Map(theGroup,theGroup,images);
    }
  }

  // We do it by choosing a normally distributed number of Whitehead
  // generators to compose, then use uniform deviates to choose the
  // generators.

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

		if ( kind == 0 || kind == 1)
		  if (isTooLong(images))
		    break;

		if ( kind == 0 ) {	 // 0: Xi --> Xi Xj, i != j

		  Generator g(i);
		  Word w = g * Generator(j);
		  for( int k = 0; k < numberOfGroupGens; ++k )
			 images[k] =
				(((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();

		} else if ( kind == 1 ) {	 // 1: Xi --> Xj Xi, i != j

		  Generator g(i);
		  Word w = Generator(j) * g;
		  for( int k = 0; k < numberOfGroupGens; ++k )
			 images[k] =
				(((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();

		} else {	 // 2: Xi <-> Xj, i != j

		  // Pretty kludgy:

		    VectorOf<Word> transp(numberOfGroupGens);
			 for( int k = 0; k < numberOfGroupGens; ++k )
				transp[k] = Word(Generator(k+1));
			 transp[i-1] = Word(Generator(j));
			 transp[j-1] = Word(Generator(i));
			 for( int k = 0; k < numberOfGroupGens; ++k )
				images[k] =
				  (((Word)images[k]).replaceGenerators( transp )).freelyReduce();

		}

	 } else {	 // 3: Xi --> Xi^-1

		int i = typeGenPicker.rand(1, numberOfGroupGens);
		Generator g(i);
		Word w( inv(g) );
		for( int k = 0; k < numberOfGroupGens; ++k )
		  images[k] =
			 (((Word)images[k]).replaceGeneratorWithWord( g, w )).freelyReduce();
		
	 }
  }   // while
  
  // Now make Map from images and return  it:

  return   Map(theGroup, theGroup, images);
}

Map RandomAutoInFree::getFiniteAutomorphism(int& order )
{
  Map m;
  int theOrder;
  do{ 
    m = getAutomorphism();
  } while (isFiniteAutomorphism(m,theOrder) != yes);
  order = theOrder;
  return m; 
}
Trichotomy RandomAutoInFree::isFiniteAutomorphism(const Map& m,int& order)const
{
  bool got_one = false;
  bool panic = false;
  Trichotomy answer(dontknow);
    // Give up now if m is not of prime order p, where it must be
    // the case that p <= rank + 1.
    
    bool isEnoughSpace = true;   
    int maxLengthOfImage = 0;
    for (int i=0;i<numberOfGroupGens;i++)
      if (maxLengthOfImage<m.generatingImages(i).length())
	maxLengthOfImage = m.generatingImages(i).length();
 
    if(sumOfImagesLens(m.generatingImages())*maxLengthOfImage > MAXLENGTH){
      isEnoughSpace = false;
      panic = true;
    }
    int count = numberOfGroupGens*numberOfGroupGens;
    int maxCount = count;    
    int theOrder = 1;

    if (isEnoughSpace){
      //      Map result = composition(m, m);
      Map result = m;
      got_one = true;

      panic = false;
      
      do {
      got_one = true;
	
	//@rn kludgy trap explosion of generating images:
	for( int j = 0; j < numberOfGroupGens; ++j ) {
	  if ( result.generatingImages(j).length() > 300 ) {
	    got_one = false;
	    panic = true;
	    break;
	  }
	}
	
	if ( ! panic ) {
	  for( int i = 0; i < numberOfGroupGens; ++i ) {
	    if ( result.generatingImages(i).freelyReduce() !=
		 Word(Generator( i + 1 )) ) {
	      got_one = false;
	      theOrder++;
	      break;
	    }
	  }
	  if(sumOfImagesLens(result.generatingImages())*maxLengthOfImage > MAXLENGTH){
	    got_one = false;
	    panic = true;
	  }
	  else  
	    result = composition(m, result);
	}
	
      } while ( --count && ! got_one && ! panic );
    }
    if (got_one) {
      answer = yes;
      order = theOrder;
    }
    if (!got_one && !panic ) answer = no;
    return answer;  
}

bool RandomAutoInFree::isTooLong(const VectorOf<Word>& images )const{
  int maxLength = 0;
  for (int i=0;i<numberOfGroupGens;i++)
    if (images[i].length() > maxLength)
      maxLength = images[i].length();
  if (maxLength > MAXLENGTH / 2)
    return true;
  else
    return false;
}
int RandomAutoInFree::sumOfImagesLens(const VectorOf<Word>& images)const
{
  int retSum = 0;
  for (int i=0;i<numberOfGroupGens;i++)
    retSum+=images[i].length();
  return retSum;
}
