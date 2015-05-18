// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: definition of MapRep and Map classes
//
// * provisional, one class implementation of the notion of map and
//   homomorphism.
//
// Principal Author: Stephane Collart
//
// Status: under trial
//
// Restrictions:
//
// * the map may only be defined in terms of formal images in the
//   generators of the range group
//
// * images can only be computed of formal words in the generators of
//   the domain group
//
// * in order for formal words to have meaningful interpretations in the
//   domain and range groups, both must be finitely generated
//
// * in keeping with the restriction to formal word images, the default
//   is to evaluate an image as a formal word; to compute an evaluated
//   image, use evalImage() and postEvalImage().
//
// Discussion:
//
// * Is it convenient for a map to be default initialised to the
//   trivial map (everything mapped to the identity?) Is there
//   and alternative? Should in cases where there are enough generators
//   in the range the identity be preferred? And then all the necessary
//   checks be made?
//
// * When moving to a more general map scheme, permitting e.g.
//   additionally + images defined via actual elements + arbitrary domain
//   and map groups, + etc, more machinery must be provided to
//   distinguish and deal with the various cases in which formal or
//   actual elements are encountered.
//
// Revision History:
//
// *@rn: dp, ep should indicate their changes here.
//
//  07/96 Alexey M. implemented IPC tools
//
//  01/99 Dmitry B. changed operator == for MapRep (see comments below)
//


#ifndef _MAP_H_
#define _MAP_H_

#include <iostream.h>

#include "GenericObject.h"
#include "DerivedObjectOf.h"
#include "Vector.h"
#include "Set.h"
#include "Elt.h"
#include "Group.h"
#include "FGGroup.h"
#include "FPGroup.h"
//#include "FreeGroup.h"


//------------------------- MapRep ---------------------------//

struct MapRep : GenericRep {

  // constructors:

  // no default constructor

  // copy constructor suppled by compiler (does logical deep-copy)

  MapRep( const FGGroup& domain,
	  const FGGroup& range ) :
    theDomain(domain), theRange(range), 
    theGeneratingImages(domain.numberOfGenerators())  { }

  MapRep(const FGGroup& domain,
	 const FGGroup& range,
	 const VectorOf<Word>& generatingImages ) :
    theDomain(domain),
    theRange(range),
    theGeneratingImages(generatingImages) { }

  // destructor supplied by compiler (members have own destructors)

  // representation members:

  PureRep* clone( ) const { return new MapRep(*this); }

  // standard operators

  /* @db we should probably just check the number of generators,
         not to compare the whole groups?

  bool operator == (const MapRep& m) const {
    return (theDomain == m.theDomain && theRange == m.theRange &&
	    theGeneratingImages == m.theGeneratingImages);
  }
  */
  bool operator == (const MapRep& m) const {
    return 
      ( theDomain.numberOfGenerators() == m.theDomain.numberOfGenerators() 
	&& theRange.numberOfGenerators() == m.theRange.numberOfGenerators() 
	&& theGeneratingImages == m.theGeneratingImages
	);
  }

  // I/O

  MapRep* readFrom(istream& istr, Chars& errMesg) const;


  //  virtual void checkMapProperties() const;
  // Checks whether the map is a monomorphism and/or epimorphism.
  // Sets corresponding flags.
  // This can be done only for homomorphisms (isHom is true) and
  // only for free groups.


  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Type Stuff and Access:                                           //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  static const Type theMapType;

  static Type type() { return theMapType; }

  Type actualType() const { return type(); } // overrides

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void  write( ostream& ostr ) const {
      ostr < theDomain;
      ostr < theRange;
      ostr < theGeneratingImages;
      ostr < theFlags.isHom <  theFlags.isMono <  theFlags.isEpi;
  }
  void  read( istream& istr ){
      istr > theDomain;
      istr > theRange;
      istr > theGeneratingImages;
      istr >  theFlags.isHom >  theFlags.isMono > theFlags.isEpi;
  }

  // data members:

  // real data:
  FGGroup theDomain;
  FGGroup theRange;
  VectorOf<Word> theGeneratingImages;

  // flags:
  struct MapProperties {
    Trichotomy isHom;
    Trichotomy isMono;
    Trichotomy isEpi;
    MapProperties( ) :
      isHom(dontknow),
      isMono(dontknow),
      isEpi(dontknow)
    { }
  };
  
  MapProperties theFlags;

private:

  MapRep& operator = ( const MapRep& );
  // disable assignment

};


//-------------------------- Map ---------------------------//


class Map : public DerivedObjectOf<GenericObject,MapRep> {

public:

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Constructors                                                   //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  // @stc note the complete lack of consistency checks in the
  // constructors

  // no default constructor: domain and range groups must be given

  Map( ) :
    DerivedObjectOf<GenericObject,MapRep>(
      new MapRep(FPGroup(),FPGroup(), VectorOf<Word>())
														)
  { }
  // copy constructor supplied by compiler

  Map( const FGGroup& domain,
       const FGGroup& range ) :
    DerivedObjectOf<GenericObject,MapRep>(
					  new MapRep(domain, range, VectorOf<Word>(domain.numberOfGenerators()))
					  )
  { }
  // to construct a map with unspecified images;
  // by default, becomes the trivial map, i.e.
  // the map mapping everything to the identity;
  // Word's default constructor takes care of initialising
  // the generating images to the identity.

  Map( const FGGroup& domain,
       const FGGroup& range,
       const VectorOf<Word>& generatingImages ) :
    DerivedObjectOf<GenericObject,MapRep>(
					  new MapRep(domain,range,generatingImages)
					  ) {

#if SAFETY > 0 // temporary restriction
      if (generatingImages.length() != domain.numberOfGenerators())
	error("wrong number of generating images in"
	      " Map::Map(domain,range,generatingImages)");
#endif
  }
  // to construct a map, given a domain and range group, and images
  // for the generators of the domain
  // @stc this should do some fancy stuff to deal with cases when
  // too few or too many generating images are given (both could be
  // legal with appropriate semantics)

  // destructor supplied by compiler

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Standard Operators                                             //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  bool operator == ( const Map& m ) const { 
    return look() == m.look() || *look() == *m.look(); 
  }

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Accessors / Modifiers                                          //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  static Type type( ) { return MapRep::type(); }

  // @stc it is assumed that domain, range and generatingImages
  // in their entirety are modified infrequently, hence seperate
  // members are provided to (re)set them
  // individual generatingImages might be modified more often;
  // hence, for convenience, an intermediate assignable result is
  // constructed when taking the image of a generator

  const FGGroup& domain( ) const { return look()->theDomain; }
  // to poll the domain of definition of the map
  // not for modification

  const FGGroup& range( ) const { return look()->theRange; }
  // to poll the range of definition of the map
  // not for modification

  void setDomain( const FGGroup& g );
  // to modify domain group; if the number of generators of g is less
  // than the number of stored generating images, the extra ones are
  // thrown away; if it is greater, the additional generating images
  // are uninitialised
  // @stc more fancy facilities should be provided to specify how one
  // wants to reuse the old images.

  void setRange( const FGGroup& g ) { change()->theRange = g; }
  // to set of modify range group
  // @stc this should check the images for validity

  const VectorOf<Word>& generatingImages( ) const
  { return look()->theGeneratingImages; }
  // to poll the defining images of the map
  // not for modification

  Word generatingImages( int i ) const
  { return look()->theGeneratingImages[i]; }
  // to poll the i'th defining image (0-based indexing)
  // not for modification
  // @stc this cannot be made to return a const ref, as
  // theGeneratingImages[i] is not an lvalue and this would be a
  // a dangling reference
  // when changing the MapRep this could change

  Word generatingImages( const Generator& g ) const {
    int Ord = ord(g)-1;
    if( Ord >= 0 ) 
      return generatingImages(Ord);
    else 
      return generatingImages(-Ord).inverse();
  }

  void setGeneratingImages( const VectorOf<Word> gi ) {

#if SAFETY > 0
    if (gi.length() != domain().numberOfGenerators())
      error("wrong number of generating images in"
	    " Map::setGeneratingImages( const VectorOf<Word> )");
#endif

    change()->theGeneratingImages = gi;
  }
  // to set or modify generating images

  void setGeneratingImages( int i, const Word& e  ) {

#if SAFETY > 0
    if (i < 0 || i >= domain().numberOfGenerators())
      error("generating image index out of range in"
	    " Map::setGeneratingImages(int, const Word&)");
#endif
    change()->theGeneratingImages[i] = e;
  }
  // to assign to the i-th (0-based) generating image

  void setGeneratingImages( const Generator& g, const Word& w ) {
    int Ord = ord(g);
    if( Ord > 0 ) setGeneratingImages(Ord-1, w);
    else setGeneratingImages(-Ord-1, w.inverse());
  }
	
  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Mapping methods                                                //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  // computing images:
  // formally:

  Word imageOf( const Word& w ) const
  { return w.replaceGenerators( look()->theGeneratingImages ); }
  // takes a formal word in the generators of domain and evaluates its
  // `image' in range; if the images are defined via formal words,
  // then the result is also a formal word.
  // @stc this bears discussion -- should a seperate image evaluater
  // also be provided?

  Word imageOf( const Generator& g ) const;
  // image of generator g; not an lvalue

  // computing images:
  // evaluated:

  Elt evalImage( const Word& w ) const;
  // to compute the image, using the range's operations

  Elt postEvalImage( const Word& w ) const {
    return range().eval( imageOf(w) );
  }
  // to compute an image formally, the evaluate it in the range

  // operations on maps:

  friend Map composition( const Map& secondMap,
			  const Map& firstMap );
  // map-theoretic composition

  Map& composeAfter( const Map& firstMap );
  // equivalent to as *this = composition(*this,firstMap);
  // @stc tricky one to name: not to confuse the order of composition

  Map operator | ( const Map& secondMap )
  { return composition(secondMap,*this); }
  // @stc this is a tentative operator abbreviation for
  // composition -- its intuitive iterpretation should be a pipe,
  // hence composition is left to right; its advantage is to permit
  // syntactically light concatenation: fcomp = f1 | f2 | f3;
  // In the same line of thought an Word | Map is provided below
  // @stc not a global to avoid most kinds of unintended conversions

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Mapping properties                                             //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  // @stc values cast to Trichotomy before returning to keep warnings
  // silent

  Trichotomy extendsToHom( ) const {
    return look()->theFlags.isHom;
  }

  void putIsMono( ) const { enhance()->theFlags.isMono = yes; }

  Trichotomy isMono( ) const
  { return Trichotomy(look()->theFlags.isMono); }

  void putIsEpi( ) const { enhance()->theFlags.isEpi = yes; }
 
  Trichotomy isEpi( ) const
  { return Trichotomy(look()->theFlags.isEpi); }
 
  Trichotomy isIso( ) const { 

    if ( isMono() == dontknow || isEpi() == dontknow) return dontknow;
    else return ( isMono() == yes && isEpi() == yes );
  }

  Trichotomy isTrivialMap( ) const; // @stc impl. tmp.

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // I/O                                                            //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  void printOn(ostream& ostr) const;
 
  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Debugging stuff                                                //
  //                                                                //
  ////////////////////////////////////////////////////////////////////
 
#ifdef DEBUG

  //friend int main( );

  bool consistent( ) {
    // the map object is assumed to be consistent if it has no defining
    // images (uninitialised) or the correct number of defining images

    if (look()->theGeneratingImages.length() !=
	look()->theDomain.numberOfGenerators() &&
	look()->theGeneratingImages.length() != 0)
      return false;
    return true;
    // @stc this should also check the defining images themselves
    // for consistency
  }

#endif

  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // I/O                                                            //
  //                                                                //
  ////////////////////////////////////////////////////////////////////

  friend IStreamPoll operator >> ( istream&, Map& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Map& M )
  {
    M.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Map& M)
  {
    M.change()->read(istr);
    return istr;
  }


  ////////////////////////////////////////////////////////////////////
  //                                                                //
  // Representation access methods                                  //
  //                                                                //
  ////////////////////////////////////////////////////////////////////
 
protected:
 
  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  Map( MapRep* newrep ) : DerivedObjectOf<GenericObject,MapRep>(newrep) { }

};


//------------------ Related global functions --------------------//

ostream& operator << ( ostream&, const Map& );


#endif
