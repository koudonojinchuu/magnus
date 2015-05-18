// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of ElementaryWhiteheadAuto and 
//           WhiteheadAuto classes.
//           
//
// Principal Authors: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special remarks:
//


#ifndef _WHITEHEAD_H_
#define _WHITEHEAD_H_

#include "Set.h"
#include "List.h"
#include "Word.h"
#include "Automorphism.h"


//////////////////////////////////////////////////////////////
//                                                          //
//   Class ElementaryWhiteheadAuto                          //
//                                                          //
//////////////////////////////////////////////////////////////


typedef enum { WA_INVERSE, WA_PERMUTATION, WA_LEFT_MULT, WA_RIGHT_MULT } 
        WhiteheadAutoType;

class ElementaryWhiteheadAuto 
{
public:

  // No default constructor.

  ElementaryWhiteheadAuto( const Generator& x );
  // Construct an elementary Whitehead automorphism:
  //   x -> x^-1 (WA_INVERSE)
  
  ElementaryWhiteheadAuto( const Generator& x, const Generator& y, 
			   const WhiteheadAutoType& wat );
  // Construct an elementary Whitehead automorphism:
  //  x <-> y     (WA_PERMUTATION), generators must have same order (+1/-1).
  //  x -> y * x  (WA_LEFT_MULT)
  //  x -> x * y  (WA_RIGHT_MULT)

  // Copy constructor, operator= and destructor supplied by compiler.

  Generator x( ) const;
  // Returns first generator of Whitehead auto.

  Generator y( ) const;
  // Returns second generator of Whitehead auto.
  // If the last one is inverse then the call of the method is prohibited.

  Word imageOf( const Word& w ) const;
  // Returns an image of the given word under the Whitehead automorphism.

  ElementaryWhiteheadAuto inverse( ) const;
  // Return the inverse of the automorphism.
 
  WhiteheadAutoType type( ) const { return theType; }
  // Returns a type of the Whitehead automorphism.

  void printOn( ostream& ostr ) const;
  // Prints the automorphism on stream with standard names of generators
  // (i.e. x1, x2, etc.)
  
  void printOn( ostream& ostr, const VectorOf<Chars>& names ) const;
  // Prints the automorphism on stream using given names of generators.

private:

  void printGenerator( ostream& ostr, const Generator& g ) const;

  void printGenerator( ostream& ostr, const Generator& g,
		       const VectorOf<Chars>& names ) const;

  WhiteheadAutoType theType;

  Generator g1, g2;
};

inline Generator ElementaryWhiteheadAuto::x( ) const { return g1; }

inline Generator ElementaryWhiteheadAuto::y( ) const
{
#if SAFETY > 0
  if( theType == WA_INVERSE ) 
    error("Retrieving second generator of an ElementaryWhiteheadAuto "
	  "while the type of the auto is WA_INVERSE.");
#endif

  return g2;
}

inline ostream& operator << ( ostream& ostr, const ElementaryWhiteheadAuto& aut )
{
  aut.printOn( ostr );
  return ostr;
}

inline bool operator == ( const ElementaryWhiteheadAuto& u, 
			  const ElementaryWhiteheadAuto& w )
{
  return ( u.type() == w.type() && u.x() == w.x() && u.y() == w.y() );
}

inline bool operator != ( const ElementaryWhiteheadAuto& u, 
			  const ElementaryWhiteheadAuto& w )
{
  return !( u == w );
}

//////////////////////////////////////////////////////////////
//                                                          //
//   Class WhiteheadAuto                                    //
//                                                          //
//////////////////////////////////////////////////////////////

class WhiteheadAuto 
{
public:

  WhiteheadAuto() {} // is treated as identity automorphism.
  WhiteheadAuto(const ElementaryWhiteheadAuto& a) : autoList(a) {}
  WhiteheadAuto(const ListOf<ElementaryWhiteheadAuto>& list) : autoList(list) {}

  // copy constructor, operator= and destructor supplied by compiler.

  // Accessors:

  ListOf<ElementaryWhiteheadAuto> getAutoList() const { return autoList; }

  // Methods:

  WhiteheadAuto inverse() const;

  Word imageOf(const Word& w) const;
  // Returns an image of the given word under the Whitehead automorphisms.

  Automorphism makeAutomorphism(const FGGroup& G) const;
  // Converts the given WhiteheadAuto into Automorphism acting on group G

  void printOn( ostream& ostr ) const;

private:

  // data members

  ListOf<ElementaryWhiteheadAuto> autoList;
};


inline bool operator==(const WhiteheadAuto& u, const WhiteheadAuto& w) 
{
  return ( u.getAutoList() == w.getAutoList() );
}
// the comparison is unsatisfactory because we don't reduce
// inverse pairs. We didn't need this method...

inline Word operator | (Word& w, ElementaryWhiteheadAuto& ewa) 
{
  return ewa.imageOf(w);
}

inline Word operator | (const Word& w, const WhiteheadAuto& wa) 
{
  return wa.imageOf(w);
}

inline WhiteheadAuto operator | (const WhiteheadAuto& wa1, const WhiteheadAuto& wa2)
{	
  return WhiteheadAuto(concatenate(wa1.getAutoList(),wa2.getAutoList())); 
}

inline WhiteheadAuto& operator |= ( WhiteheadAuto& wa1, const WhiteheadAuto& wa2)
{	
  wa1 = WhiteheadAuto(concatenate(wa1.getAutoList(),wa2.getAutoList())); 
  return wa1;
}

inline ostream& operator << ( ostream& ostr, const WhiteheadAuto& wa ) 
{
  wa.printOn( ostr );
  return ostr;
}


WhiteheadAuto whiteheadDecomposition( const VectorOf<Word>& vec );

#endif



