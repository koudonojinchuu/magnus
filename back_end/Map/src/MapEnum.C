// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes MapEnum, IntTuples
//
// Principal Author: Roger Needham
//
// Status: In development
//
// Further Implementation Steps:
//
// Revision History:
//


#include "MapEnum.h"



//------------------------------------------------------------------------//
//------------------------------ MapEnum ---------------------------------//
//------------------------------------------------------------------------//


MapEnum::MapEnum(const FGGroup& domain, const FGGroup& range, int radius) :
  domainRank(domain.numberOfGenerators()),
  rangeRank(range.numberOfGenerators()),
  theDomain(domain),
  theRange(range),
  theTuples(domainRank, radius)
{

}


Map MapEnum::nextMap(int jump)
{
  const int* tuple;
  while ( jump-- ) tuple = theTuples.nextTuple();

  VectorOf<Word> Images(domainRank);
  for( int i = 0; i < domainRank; ++i )
	 Images[i] = kthWord( *tuple++, rangeRank );

  return Map(theDomain, theRange, Images);
}


Word MapEnum::kthWord(int k, int n)
{
  if ( k == 0 ) return Word();
  
  // There are n (n - 1)^(l - 1) freely reduced words of length
  // l on n generators.
  // First we find the length. We could take logs, but I don't
  // trust floating point (even if we never port to a pentium).

  int length = 1;
  int m = n;
  int n_1 = n - 1;

  while ( k > m ) {
	 ++length;
	 k -= m;
	 m *= n_1;
  }

  VectorOf<Generator> result(length);  // For building the word
  int i = 0;                           // Index into result

  --k;  // Want 0-based index into words of length length.

  // Determine the first letter. Of these m words, the first m / n
  // start with letter 1, the next m / n with letter -1, then 2, -2,
  // and so on.

  m /= n;
  int prev = k / m;
  result[i++] = ( prev & 1 ? ( -1 - prev ) >> 1 : ( prev + 2 ) >> 1 );

  // Uh, that did the mapping of ints to Generator ordinals:
  // 0  1  2  3  4  5 . . .
  // 1 -1  2 -2  3 -3 . . .

  // Now get the rest. By convention, we use the same letter in a given
  // spot as long as possible. Thus the letters appear in consecutive
  // runs of m / n_1.

  int temp = prev;

  while ( i < length ) {

	 k -= temp * m;          // Get index into correct subrun
	 m /= n_1;               // The subruns get smaller
	 temp = k / m;
	 prev = ( temp >= ( prev ^ 1 ) ? temp + 1 : temp );
	 result[i++] = ( prev & 1 ? ( -1 - prev ) >> 1 : ( prev + 2 ) >> 1 );
  }

  return Word(result);
}



//------------------------------------------------------------------------//
//----------------------------- IntTuples --------------------------------//
//---------------------------- helper class ------------------------------//


IntTuples::IntTuples(const int tupleLength, const int startRadius)
{
  tuple = new int[tupleLength];
  for( int i = 0; i < tupleLength; ++i ) tuple[i] = 0;
  end = tuple + tupleLength - 1;
  sp = end;
  r = startRadius - 1;  // To bootstrap the first call to nextTuple().
}


const int* IntTuples::nextTuple( )
{
  if ( sp != end ) {

	 *sp++ -= 1;
	 *sp += 1;

  } else {

	 while ( --sp >= tuple && *sp == 0 );

	 if ( sp < tuple ) {  // Next higher radius
		sp = tuple;
		*sp = ++r;
	 } else {
		*sp++ -= 1;
		*sp = *end + 1;
	 }

	 if ( sp != end ) *end = 0;

  }

  return tuple;
}
