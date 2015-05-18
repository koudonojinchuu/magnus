// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the Automorphism and AutomorphismRep class.
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

#ifndef _AUTOMORPHISM_H_
#define _AUTOMORPHISM_H_

#include "Endomorphism.h"
#include "FreeGroup.h"

//---------------------- Automorphism ---------------------------//


class Automorphism : public Endomorphism {

public:

////////////////////////////////////////////////////////////////////
//                                                                //
// Constructors                                                   //
//                                                                //
////////////////////////////////////////////////////////////////////

  Automorphism( const FGGroup& dom) : Endomorphism(dom) { makeIdentity(); }
	// make trivial Automorphism on given group

	Automorphism( const FGGroup& dom,	const VectorOf<Word>& generatingImages )
		: Endomorphism(dom,generatingImages) { }
	// an Automorphism with explicit generatingImages

	Automorphism(FGGroup& dom, Generator x, Word& w) : Endomorphism(dom)
	{
		makeIdentity();
		setGeneratingImages(x,w);
	}
	// maps x to w, the others to themselves

	Automorphism(FGGroup& dom, Word& w) : Endomorphism(dom)
	{
		for(int i=0; i< dom.numberOfGenerators(); i++) {
			Word x = Generator(i+1);
			setGeneratingImages(i, x.conjugateBy(w));
		}
	}
	// An `inner' endomorphism, i.e., x -> x^w for all generators x.

	Automorphism( const Map& m ) : Endomorphism(m) {}
  // cast construtor: to cast an arbitrary map as an automorphism.
  // NB. This rewraps the unchanged representation, hence is in general
  // only useful for casting a map known to be an actual automorphism.

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
	// void setDomain( const FGGroup& g ); 
	// void setRange( const FGGroup& g );
	// void makeIdentity()
	// void reduceGenImages();

////////////////////////////////////////////////////////////////////
//                                                                //
// Mapping methods                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

	// inherited from the base class

	// friend CondParamLvalue<Map,Word,Generator>::operator Word ( ) const;
	// friend Word& CondParamLvalue<Map,Word,Generator>::operator =( const Word& ) const;
	// Word imageOf( const Word& w ) const;
	// Word imageOf( const Generator& g ) const;
	// CondParamLvalue<Map,Word,Generator> imageOf( const Generator& g );
	// Word operator()( const Word& w ) const;
	// Word operator()( const Generator& g ) const;
	// CondParamLvalue<Map,Word,Generator> operator()( const Generator& g );
	// computing images:
	// Elt evalImage( const Word& w ) const;
	// Elt postEvalImage( const Word& w ) const;

// operations on maps:

	// Map& composeAfter( const Map& firstMap );
	// Map operator | ( const Map& secondEndo )

	Automorphism inverse() const {
		#if SAFETY > 0
		if( domain().actualType() != FreeGroup::type() )
			error("Automorphism::inverse() supported for "
			      "automorphism of a free group only");
		#endif
		const VectorOf<Word>& images = look()->theGeneratingImages;
		return Automorphism(domain(), ((const FreeGroup&)domain()).inverseAutomorphism(images) );
	}

////////////////////////////////////////////////////////////////////
//                                                                //
// Mapping properties                                             //
//                                                                //
////////////////////////////////////////////////////////////////////

	// inherited from the base class:

	// Trichotomy extendsToHom( ) const;
	// Trichotomy isMono( ) const;
	// Trichotomy isEpi( ) const;
	// Trichotomy isIso( ) const;
	// Trichotomy isTrivialMap( ) const; // @stc impl. tmp.

////////////////////////////////////////////////////////////////////
//                                                                //
// Debugging stuff                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

#ifdef DEBUG

	//friend int main( );

	bool consistent( ) {
		const VectorOf<Word>& images = look()->theGeneratingImages;
		return Endomorphism::consistent() &&
		       ((const FreeGroup&)domain()).isAutomorphism(images);
	}

#endif

};

#endif

