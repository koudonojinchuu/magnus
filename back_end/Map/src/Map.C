// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of classes Map & MapRep
//
// Principal Author: Stephane Collart
//
// Status: under trial
//
// Revision History:
//
// * 11/01/95 Roger made sure that generating images are always freely reduced.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "Map.h"
#include "MapParser.h"


//------------------------------------------------------------------//
//---------------------------- MapRep ------------------------------//
//------------------------------------------------------------------//

MapRep* MapRep::readFrom(istream& istr, Chars& errMesg) const
{
  // @dp force linker to bind static member of MapRep class
  //  Type dummy( theMapType );

  MapParser P(istr);
  return P.parseMap( theDomain, theRange, errMesg );
}





//------------------------------------------------------------------//
//----------------------------- Map --------------------------------//
//------------------------------------------------------------------//


void Map::setDomain( const FGGroup& g ) {

	change()->theDomain = g;
	if (generatingImages().length() != g.numberOfGenerators())
		setGeneratingImages(
			VectorOf<Word>( g.numberOfGenerators(), generatingImages() )
		);
}

Word Map::imageOf( const Generator& g ) const {

	if (ord(g) < 0)
		return generatingImages(-ord(g) - 1).inverse();
	else
		return generatingImages(ord(g) - 1);
}

Elt Map::evalImage( const Word& w ) const {

	if (w.length() == 0) return range().eval(w);
	// don't bother to compute the image of the trivial word

	Elt res = range().eval( imageOf(w[0]) );
	for (int i = 1; i < w.length(); i++)
		res = range().multiply( res, range().eval( imageOf(w[i]) ) );
	return res;
}

Map& Map::composeAfter( const Map& firstMap ) {

	return *this = composition(*this,firstMap);
}


Trichotomy Map::isTrivialMap( ) const {
// @stc temporary hack; this is not even correct, since it does not
// check whether the generators of the domain are trivial.
  
  int i = generatingImages().length();
  Trichotomy answer = TRUE;
  
  while (answer != NO && i-- > 0) { // loop if answer != NO
	 Trichotomy check = range().isTrivialElt(generatingImages(i));
	 if ( check == dontknow ) {
		answer = DONTKNOW;
		break;
	 } else if ( check == no ) {
		answer = NO;
		break;
	 }
  }
  
  return answer;
}


void Map::printOn(ostream& ostr) const
{
  // Due to front end requirements, this can not use newlines, which
  // is a pain.

//@db  ostr << "{ ";
  int len = generatingImages().length();
  for( int i = 0; i < len; ++i ) {	 
	 if ( i > 0 ) ostr << ", ";
	 domain().printGenerator(ostr, i + 1);
	 ostr << " -> ";
	 range().printWord(ostr, generatingImages(i));
  }
//@db  ostr << " }";
}

	
//---------------------------- global -----------------------------//


Map composition( const Map& secondMap, const Map& firstMap )
{
  
  int i;
  
#if SAFETY > 1
  // @stc semantic errors: not at level 1?
  if (secondMap.domain() != firstMap.range())
    error("tried to compose differing domain and range in"
	  " ::composition(const Map&, const Map&)");
  for (i = 0; i<firstMap.generatingImages().length(); i++)
    if (firstMap.generatingImages(i).actualType() != Word::type())
      error("first map image(s) not given as formal words in"
	    " ::composition(const Map&, const Map&)");
  // @stc if the words themselves are out of bounds, that is an
  // inconsistency of the map itself (which replaceGenerators() will detect)
#endif
  
  VectorOf<Word> images(firstMap.generatingImages().length());
  for (i=0; i < images.length(); i++) {
    Word w = firstMap.generatingImages(i);
    images[i] = ( w.replaceGenerators(secondMap.generatingImages()))
      .freelyReduce();
  }
  return Map(firstMap.domain(),secondMap.range(),images);
}


ostream& operator << ( ostream& o, const Map& m )
{
  m.printOn(o);
  return o;
}


IStreamPoll operator >> ( istream& istr, Map& m )
{
  Chars errMesg;
  MapRep* rep = m.look()->readFrom(istr, errMesg);
  if ( rep ) m = Map(rep);
  // readFrom returns NULL on error.
  return IStreamPoll( istr, errMesg );
}



//------------------------------------------------------------------//
//---------------------------- MapRep ------------------------------//
//------------------------- static members -------------------------//


const Type MapRep::theMapType = Type(Type::unique());
