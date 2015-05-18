// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FreeGroup class.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// * The class is primarily designed and used for solving
//   quadratic equations (in free groups).
//
// * The class methods need advanced consistency checking and
//   more manipulation with Map properties.
//

#ifndef _ENDOMORPHISM_H_
#define _ENDOMORPHISM_H_

#include "Map.h"

//---------------------- Endomorphism ---------------------------//


class Endomorphism : public Map {

public:

////////////////////////////////////////////////////////////////////
//                                                                //
// Constructors                                                   //
//                                                                //
////////////////////////////////////////////////////////////////////

	// no default constructor: a domain group must be given

	Endomorphism( const FGGroup& dom) : Map(dom, dom) {}
	// make trivial endomorphism on given group

	Endomorphism( const FGGroup& dom, const VectorOf<Word>& generatingImages ) 
		: Map(dom, dom, generatingImages) {}
	// an Endomorphism with explicit generatingImages

	Endomorphism( const Map& m ) : Map(m) {}
  // cast construtor: to cast an arbitrary map as an endomorphism.
  // NB. This rewraps the unchanged representation, hence is in general
  // only useful for casting a map known to be an actual endomorphism.

	// copy constructor, operator=, and destructor supplied by compiler

////////////////////////////////////////////////////////////////////
//                                                                //
// Accessors / Modifiers                                          //
//                                                                //
////////////////////////////////////////////////////////////////////

	// inherited from the base class

	// const FGGroup& domain( ) const;
	// const FGGroup& range( ) const;
	// const VectorOf<Word>& generatingImages( ) const;
	// Word generatingImages( int i ) const;
	// void setGeneratingImages( const VectorOf<Word>& gi );
	// void setGeneratingImages( int i, const Word& e  );

  void makeIdentity()
	{
		for(int i=0; i<domain().numberOfGenerators(); i++)
			setGeneratingImages(i, Generator(i+1));
	}
	// turns the endomorphism into the identity automorphism. 

  void reduceGenImages() 
	{
		for(int i = 0; i<generatingImages().length(); i++)
			setGeneratingImages(i, generatingImages(i).freelyReduce());
	}
	// freely reduce generating images to prevent its overgrowth 
	// in composition of maps. 
	//@ep Should be this done automatically?

////////////////////////////////////////////////////////////////////
//                                                                //
// Standard operations                                            //
//                                                                //
////////////////////////////////////////////////////////////////////

  bool operator ==(const Endomorphism& e) const {
    return ( look() == e.look() || *look() == *e.look() ); 
  }
	//@dp this operator should be defined in Map only. But now
	//    Map class has too simple of this one.

////////////////////////////////////////////////////////////////////
//                                                                //
// Mapping methods                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

	// inherited from the base class

//    friend CondParamLvalue<Map,Word,Generator>::operator Word ( ) const;
//    friend Word& CondParamLvalue<Map,Word,Generator>::operator =
//        ( const Word& ) const;
//    Word imageOf( const Word& w ) const;
//    Word imageOf( const Generator& g ) const;
//    CondParamLvalue<Map,Word,Generator> imageOf( const Generator& g );
//    Word operator()( const Word& w ) const;
//    Word operator()( const Generator& g ) const;
//    CondParamLvalue<Map,Word,Generator> operator()( const Generator& g )
//        { return imageOf(g); }

// computing images:

//    Elt evalImage( const Word& w ) const;
//    Elt postEvalImage( const Word& w ) const;


////////////////////////////////////////////////////////////////////
//                                                                //
// Mapping properties                                             //
//                                                                //
////////////////////////////////////////////////////////////////////

//    Trichotomy extendsToHom( ) const;

//    Trichotomy isMono( ) const;
//    Trichotomy isEpi( ) const;
//    Trichotomy isIso( ) const;
//    Trichotomy isTrivialMap( ) const; // @stc impl. tmp.

////////////////////////////////////////////////////////////////////
//                                                                //
// Debugging stuff                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

#ifdef DEBUG

//	friend int main( );

	bool consistent( ) {
		return Map::consistent() && domain() == range();
	}

#endif

};

#endif

