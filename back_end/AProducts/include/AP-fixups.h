// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// A temporary collection of various standalone procedures.
// Many of them should better be methods of corresponding classes,
// but we have to discuss the changes.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: draft
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// Discussion:
//
// Bugs:
//
//
// Special Notes:
//
//

#ifndef _FIXUPS_H_
#define _FIXUPS_H_

#include "SGofFreeGroup.h"
#include "Associations.h"
#include "File.h"

struct RelatorConjugate 
{

  RelatorConjugate( ) : relator(), conjugator() { }

  RelatorConjugate( const Word& r, const Word& c )
    : relator(r), conjugator(c) { }

  // Copy constructor, operator= and destructor are supplied by compiler.
  bool operator==( const RelatorConjugate& t) const 
  {
    return relator == t.relator && conjugator == t.conjugator;
  }

  // Data members:
  Word relator, conjugator;
};

// Standard I/O routines.
inline ostream& operator<<( ostream& ostr, const RelatorConjugate& rc )
{ // It's a stub. Do nothing. 
  ostr << rc.relator << " ^ " << rc.conjugator;
  return ostr;
}
inline istream& operator>>( istream& istr, RelatorConjugate& rc )
{ // It's a stub. Do nothing. 
  return istr;
}

// IPC support.
inline ostream& operator<( ostream& ostr, const RelatorConjugate& rc )
{
  ostr < rc.relator < rc.conjugator;
  return ostr;
}
inline istream& operator>( istream& istr, RelatorConjugate& rc )
{
  istr > rc.relator > rc.conjugator;
  return istr;
}


typedef VectorOf<RelatorConjugate> ProductOfRelatorConjugates;

ProductOfRelatorConjugates conjugateBy( 
  const ProductOfRelatorConjugates& product, const Word& conjugator );


template <class T>
inline void swap( T& a, T& b ) 
{
  T temp = a;
  a = b;
  b = temp;
}

// Here go two dummy functions for SGofFreeGroup class. They are absent
// in the class, but are required by VectorOf.

inline ostream& operator << (ostream& o, const SGofFreeGroup& sg)
{
  //error("Dummy function operator<<(ostream&, SGofFreeGroup&) is called.");
  warn(" dumb operator<<(ostream&, SGofFreeGroup&) is called. ");
  o << " subgroup is ";
  sg.parentGroup().printVectorOfWords(o, sg.generators());
  o << " of ";
  o << sg.parentGroup();
  return o;
}

/*
inline bool operator == (const SGofFreeGroup& a,  const SGofFreeGroup& b)
{
  error("Dummy function operator==(SGofFreeGroup&, SGofFreeGroup) is called.");
  return true;
}
*/

// here go two local functions dealing with vectors
//@ep I think they'd better be methods of the VectorOf class


// exclude i-th element from vector, shifting the remainder left

template <class T>
inline void excludeFrom(VectorOf<T>& v, int i)
{
  int len = v.length();
  for(int j=i+1; j<len; j++)
    v[j-1] = v[j];
  v.shrink(len-1);
}

// Cyclically permutes vector elements; v[j] becomes v[0].
// Thus j > 0 means `left-shift' j elements.
// Negative j's yield a `right-shift'.
// j's of magnitude exceeding length of *this wrap around.

template< class T > VectorOf<T> cyclicallyPermute(const VectorOf<T>& v, int j)
{
  int len = v.length();
  int i = j;
  while ( i >= len ) i = i - len;
  while ( i < 0 ) i = i + len;
  if ( i == 0 ) return v;
  
  VectorOf<T> res(len);

  int t=0;

  for(int k = i; k < len; k++, t++)
    res[t] = v[k];

  for(int k = 0; k < i; k++, t++)
    res[t] = v[k];

  return res;
}



// These should be included into FreeGroup class:

// finds maximal root of given word `w'.
void maximalRootInFreeGroup(const Word& w, Word& root, int& power);

// Solves conjugacy problem in free group. If yes, computes conjugator.
Trichotomy conjugacyProblem(const FreeGroup& G, const Word& u,
			    const Word& v, Word& conjugator );


// There are candidates for Word class:

// counts exponent sum of all generators which are encountered in the word.
VectorOf<int> exponentSum(const Word& w);

// Returns cyclically reduced form of w such that w = answer^conjugator.
// For better performance, one should rewrite Word::cyclicallyReduce
Word cyclicallyReduce(const Word& w, Word& conjugator);

// This one is standalone but very general procedure -- should it be
// in /General, or /Global, or somewhere else?

Word compose(const VectorOf<Word>& v);
// Performs the multiplication of the components of given vector
// and returns the product.

inline int tailAgreementLength( const Word& u, const Word& v )
{
  return u.inverse().agreementLength( v.inverse() );
}
// It returns agreement length of terminal segments of the words.

int GCD2(int a, int b);
// calculates general common divider.

Word cyclicallyShortenByRelators( 
  const Word& u, const SetOf<Word>& givenRelators,
  Word& conjugator, ProductOfRelatorConjugates& productOfRelatorConjugates );
// u = w^conjugator * r1^t1 * .. * rk^tk
// Where u is the given word, w -- returned value, 
// ri are some relators or their negatives,
// ti are some words.


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                                                                        //
//  Helper class:  DetailedReport                                         //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class DetailedReport
{
public:

  // Constructors:

  DetailedReport( const FPGroup& group, const VectorOf<Word>& subgroup, 
		  const Chars fileName = File().getFileName() );

  DetailedReport( const FPGroup& group, 
		  const Chars fileName = File().getFileName() );

  // Accessors:

  ostream& file() { return theFile; }

  bool haveDetails( ) const;

  Chars getFileName( ) const;

  void printDehnTransformationDetails( const Word& w );

  void printTrivialWordDetails( const Word& w, 
				const ProductOfRelatorConjugates& deco, 
				const bool header );

  void printNonTrivialWordDetails( const Word& w, 
				   const Chars& expl, 
				   const bool header );

  void printTrivialGeneratorDetails( const Generator& g, 	
				     const ProductOfRelatorConjugates& deco, 
				     const bool header );

  void printNonTrivialGeneratorDetails( const Generator& g, 	
					const Chars& explanation, 
					const bool header );

  void printTrivialCommutatorDetails( const Chars& comm, 	
				      const ProductOfRelatorConjugates& deco, 
				      const bool header );

  void printNonTrivialCommutatorDetails( const Chars& comm,
					 const Chars& explanation, 
					 const bool header );
  
  void printSubgroupElement( const Word& w, 
			     const ProductOfRelatorConjugates& deco, 
			     const bool header );

private:

  // function members:

  void printTrivialWordDecomposition( const ProductOfRelatorConjugates& deco );

  void printHeader( const bool header );

  void buildRelators( );

  // data members:

  bool bHaveDetails;
  File theFile;
  FPGroup G;
  VectorOf<Word> H; // subgroup generators
  bool builtRelators;
  AssociationsOf<Word, int> theRelators;
  AssociationsOf<Word, int> theSGenerators;
};


inline
bool DetailedReport::haveDetails( ) const
{
  return bHaveDetails;
}

inline 
Chars DetailedReport::getFileName( ) const
{
  return theFile.getFileName();
}

#endif

