// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the AbelianWord class.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//


#include "AbelianWord.h"


// --------------------------- AbelianWordRep ------------------------------ //


AbelianWordRep::AbelianWordRep( int numOfGens, const Word& w )
  : thePowers( numOfGens )
{
  int wLen = w.length();

  for( int i = 0; i < numOfGens; ++i )
    thePowers[i] = 0;

  for( int i = 0; i < wLen; ++i ) {

    int gen;

    if( ( gen = ord(w[i]) ) > 0 )
      thePowers[gen-1] += Integer(1);
    else
      thePowers[-gen-1] -= Integer(1);
  }
}


Word AbelianWordRep::getWord( ) const
{
  int powLen = thePowers.length();
  VectorOf<Generator> w( fullLength().as_long() );
  int k = 0;
  Integer power;
  
  for( int i = 0; i < powLen; ++i )
    if( sign( power = thePowers[i] ) >= 0 )
      for( int j = 0; j < power; ++j )
	w[k++] = Generator(i+1);
    else
      for( int j = 0; j < -power; ++j )
	w[k++] = Generator(-(i+1));

  return Word(w);
}


Integer AbelianWordRep::fullLength( ) const
{
  Integer result;
  int powLen = thePowers.length();

  for( int i = 0; i < powLen; ++i )
    result += abs( thePowers[i] );

  return result;
}


AbelianWordRep AbelianWordRep::inverse( ) const
{
  int powLen = thePowers.length();
  AbelianWordRep rep( *this );
  
  for( int i = 0; i < powLen; ++i )
    rep.thePowers[i].negate();

  return rep;
}


bool AbelianWordRep::isTrivial( ) const
{
  int powLen = thePowers.length();
  for( int i = 0; i < powLen; ++i )
    if( sign(thePowers[i]) != 0 )
      return false;
  return true;
}


Integer& AbelianWordRep::operator [] ( int i )
{ 
#if SAFETY > 0
  if ( i >= thePowers.length() )
    error( "Integer& AbelianWordRep::operator [] ( int i ) : "
	   "unknown generator"
	   );
#endif
  
  return thePowers[i];
}

Integer AbelianWordRep::operator [] ( int i ) const
{ 
#if SAFETY > 0
  if ( i >= thePowers.length() )
    error( "Integer AbelianWordRep::operator [] ( int i ) const : "
	   "unknown generator"
	   );
#endif
  
  return thePowers[i];
}


bool AbelianWordRep::operator == ( const AbelianWordRep& w ) const
{
  return ( thePowers == w.thePowers );
}

AbelianWordRep* AbelianWordRep::operator * ( const AbelianWordRep& aw ) const
 {
  #if SAFETY > 0
   if ( aw.thePowers.length() != thePowers.length() )
    error( "Integer AbelianWordRep::operator * ( const AbelianWordRep& aw ) const : "
	   "different number of generators"
	   );
  #endif  
  int numOfGens = thePowers.length();
  VectorOf<Integer> newPowers(numOfGens);
  for (int i=0;i<numOfGens;i++) 
      newPowers[i] = thePowers[i]+aw.thePowers[i];
  return new AbelianWordRep(newPowers);
}

void AbelianWordRep::write( ostream& ostr ) const
{
  ostr < thePowers;
}


void AbelianWordRep::read( istream& istr ) 
{
  istr > thePowers;
}


void AbelianWordRep::printOn( ostream& ostr ) const
{
  ostr << "[";
  for( int i = 0; i < numberOfGenerators(); ++i )
    ostr << " " << i+1 << "^" << thePowers[i];
  ostr << " ]";
}


