// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of regular (x -> xy)
//           and singular (x -> 1) Nielsen transformations (classes
//           ElementaryRegularAuto, RegularAuto, ElementarySingularEndo, 
//           SingularEndo)
//
// Principal Authors: Eugeny Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special remarks:
//
// * We implemented this for our internal needs only. To achieve
//   better reusability, many improvements must be done. For example,
//   when we multiply regular automorphisms, we only concatenate
//   lists, not trying to reduce inverse neighbors.
//
// * Again, we didn't need transformations x -> x^-1, so we didn't
//   consider them. So we call an automorphism regular iff it consists
//   of elementary transformations of type x -> xy.
//


#ifndef _NIELSEN_TRANSFORMATIONS_H_
#define _NIELSEN_TRANSFORMATIONS_H_

#include "Set.h"
#include "List.h"
#include "Word.h"
#include "Automorphism.h"

////////////////// ElementaryRegularAuto /////////////////////////////////

class ElementaryRegularAuto
{
  // data members

  Generator g, g1, g2;

	// We store the transformation in form g -> g1*g2, when g is
	// always of "positive order" (acoording to its digital value).
	// Instead, we could represent it with only two generators,
	// meaning g1 -> g1*g2. The current representation was chosen
	// mainly due to imperfection of Word::replaceGeneratorWithWord
	// (it works only when generator is "positive"). It can be easily
	// changed.

public:

  // constructors

  ElementaryRegularAuto(const Generator& x, const Generator& y)
    : g(x), g1(x), g2(y) 
  {
#if SAFETY > 0
    if( x == y || x == ::inv(y) )
      error("Bad parameters in calling "
	    "ElementaryRegularAuto::ElementaryRegularAuto(x,y) "
	    " - not an automorphism");
#endif

    if( ord(x) < 0 ) {
      g  = ::inv(x);
      g1 = ::inv(y);
      g2 = g;
    }
  }
  // x -> xy

  // copy constructor, operator=, and destructor supplied by compiler


  // accessors

  Generator x() const { return (g == g1? g  : ::inv(g)); }
  Generator y() const { return (g == g1? g2 : ::inv(g1)); }

  // methods

  inline friend bool operator==(const ElementaryRegularAuto& u, 
				const ElementaryRegularAuto& w) 
  {	return ( u.g == w.g && u.g1 == w.g1 && u.g2 == w.g2);	}

  ElementaryRegularAuto inv() const {
    if( g == g1 )
      return ElementaryRegularAuto(g, ::inv(g2));
    else
      return ElementaryRegularAuto(::inv(g), g1);
  }

  Word imageOf(const Word& w) const 
  {	return w.replaceGeneratorWithWord(g, g1*g2); }

  Word operator()(const Word& w) const
  { return imageOf(w); }
  // operator synonym of imageOf.

};

inline bool operator!=(const ElementaryRegularAuto& u, const ElementaryRegularAuto& w) {
  return !(u == w);
}

///////////////// RegularAuto ////////////////////////////////////////

class RegularAuto
{
  // data members

  ListOf<ElementaryRegularAuto> autoList;

public:

  // constructors

  RegularAuto() {} // is treated as identity automorphism
  RegularAuto(const ElementaryRegularAuto& a) : autoList(a) {}
  RegularAuto(const ListOf<ElementaryRegularAuto>& list) : autoList(list) {}

  // copy constructor and destructor supplied by compiler


  // accessors

  ListOf<ElementaryRegularAuto> getAutoList() const { return autoList; }

  // methods

  RegularAuto inv() const {
    ListOf<ElementaryRegularAuto> invAutoList;
    ListIterator< ListOf<ElementaryRegularAuto> > I(autoList);
    while( !I.done() ) {
      invAutoList.prepend( I.value().inv() );
      I.next();
    }
    return RegularAuto(invAutoList);
  }

  Word imageOf(const Word& w) const {
    Word u = w;
    ListIterator< ListOf<ElementaryRegularAuto> > I(autoList);
    while( !I.done() ) {
      u = I.value().imageOf(u).freelyReduce();
      // the word is freely reduced to avoid
      // its overgrowth (exponential!) when
      // the autoList is long enough
      I.next();
    }
    return u;
  }

  inline friend bool operator==(const RegularAuto& u, const RegularAuto& w) {
    return (u.autoList == w.autoList);
  }
  // the comparison is unsatisfactory because we don't reduce
  // inverse pairs. We didn't need this method...

  Automorphism makeAutomorphism(const FGGroup& G) const {
    Automorphism M(G);

    int numOfGens = G.numberOfGenerators();
    for(int i=0; i<numOfGens; i++)
      M.setGeneratingImages( i, imageOf(Generator(i+1)) );
    return M;
  }
  // converts the given RegularAuto into Automorphism acting on
  // group G
};

//////////////////////////////////////////////////////////////////////

// Functions related to RegularAuto

inline Word operator | (Word& w, ElementaryRegularAuto& era) {
  return era.imageOf(w);
}

inline Word operator | (const Word& w, const RegularAuto& ra) {
  return ra.imageOf(w);
}

inline RegularAuto operator | (const RegularAuto& ra1, const RegularAuto& ra2)
{	return RegularAuto(concatenate(ra1.getAutoList(),ra2.getAutoList())); }


////////////// ElementarySingularEndo //////////////////////////////////

class ElementarySingularEndo
{
public:
  // data members

  Generator g;

  // constructors

  ElementarySingularEndo(const Generator& G) : g(abs(ord(G))) {}
  // g -> 1

  // accessors:

  Generator gen() const { return g; }

  // methods:

  Word imageOf(const Word& w) const {
    return w.replaceGeneratorWithWord(g, Word());
  }      // the word IS NOT freely reduced!

  inline friend bool operator==(const ElementarySingularEndo& u, 
				const ElementarySingularEndo& w) 
  { return (u.g == w.g); }

};

inline Word operator | (const Word& w, const ElementarySingularEndo& ese) 
{ return ese.imageOf(w); }


///////////////// SingularEndo //////////////////////////////////////

class SingularEndo
{
  // data members

  SetOf<Generator> genSet;

public:

  // constructors

  SingularEndo() {}  // empty set is treated as identity

  SingularEndo(const ElementarySingularEndo& ese) : genSet(ese.gen()) {}

  SingularEndo(const SetOf<Generator>& set) : genSet(set) {}

  // accessors

  SetOf<Generator> generators() const { return genSet; }

  // methods

  bool isSpecializationOf(const SingularEndo& second) const 
  { return genSet.contains(second.genSet); }
  // true iff the endo is a specialization of the second one

  friend SingularEndo operator | (const SingularEndo& se1, 
				  const SingularEndo& se2) 
  { return SingularEndo( se1.generators() | se2.generators() ); }

  Word imageOf(const Word& w) const {
    Word emptyWord;
    VectorOf<Word> images(ord(w.maxOccurringGenerator()));
    for(int i=0; i<images.length(); i++) {
      if( genSet.contains(Generator(i+1)) ) images[i] = emptyWord;
      else images[i] = Word(Generator(i+1));
    }
    return w.replaceGenerators(images);
  }

  Endomorphism makeEndomorphism(const FGGroup& G) const {
    Word emptyWord;
    VectorOf<Word> images(G.numberOfGenerators());
    for(int i=0; i<images.length(); i++) {
      if( genSet.contains(Generator(i+1)) ) images[i] = emptyWord;
      else images[i] = Word(Generator(i+1));
    }
    return Endomorphism(G, images);
  }

  inline friend bool operator==(const SingularEndo& u, const SingularEndo& w) {
    return (u.genSet == w.genSet);
  }
};


inline Word operator | (const Word& w, const SingularEndo& se) {
  return se.imageOf(w);
}

// ostream routines

inline ostream& operator <<(ostream& o, const ElementaryRegularAuto& era) {
  o << era.x() << " -> " << era.x() << " " << era.y();
  return o;
}

inline ostream& operator <<(ostream& o, const RegularAuto& ra) {
  o << ra.getAutoList() << endl;
  return o;
}

inline ostream& operator <<(ostream& o, const ElementarySingularEndo& era) {
  o << era.gen() << " -> 1 ";
  return o;
}

inline ostream& operator <<(ostream& o, const SingularEndo& ra) 
{
  SetIterator<Generator> I(ra.generators());
  Generator maxGen = 1;
  while( !I.done() ) {
    if( I.value() > maxGen ) maxGen = I.value();
    I.next();
  }
  o << " { ";
  for(int gen = 1; gen <= maxGen; gen++)
    if( ra.generators().contains(Generator(gen)) ) o << Generator(gen);
  o << " } -> 1";
  return o;
}

#endif
