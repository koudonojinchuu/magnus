// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of classes NilpotentRelator, NilpotentPresentation,
//           PresentationForFNG
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
//  1. The abstract class NilpotentPresentation presents certain nilpotent group
//  or a subgroup of a nilpotent group. There are two main components of the 
//  structure: theGenerators and theRelators. The former can be just one-letter
//  words or words in terms of the group's initial basis. The latter are words
//  int terms of presentation generators.
//
//  The relator looks like
//     [hi, hj] = h1^p1 h2^p2 ...., 
//  or:  
//     h1^p1 h2^p2 ... = 1.
//
//
// Special Notes:
//
//
// Revision History:
//

#ifndef _PRESENTATION_H_
#define _PRESENTATION_H_

#include "MalcevSet.h"
#include <iostream>

//=======================================================================
//====================== NilpotentRelator ===============================
//=======================================================================

// This is a structure to store relators.
//
// The relator looks like   [left1, left2] = right.
//
// If left1 == left2 == 0 then the relator is:   right = 1.

struct NilpotentRelator {

  void setLeft( int s1, int s2 ) {
    left1 = s1;
    left2 = s2;
  }
  // sets the left side of relator

  bool isCommutation() const {
    return left1 != 0 && left2 != 0;
  }
  // returns true iff this is a commutation relator


  // Data

  int left1, left2;
  PolyWord right;
};

inline ostream& operator < ( ostream& s, const NilpotentRelator& rel ) {
  return s < rel.left1 < rel.left2 < rel.right;
}
// IPC output

inline istream& operator > ( istream& s, NilpotentRelator& rel ) {
  return s > rel.left1 > rel.left2 > rel.right;
}
// IPC input

ostream& operator << ( ostream& s, const NilpotentRelator& rel );

inline bool operator == (const NilpotentRelator& r1, const NilpotentRelator& r2) {
  return r1.left1 == r2.left1 && r1.left2 == r2.left2 
  && (Word)r1.right == (Word)r2.right;
}



//=======================================================================
//====================== NilpotentPresentation ==========================
//=======================================================================

class NilpotentPresentation {

public:

  //---------------------------------------------------------------------
  //  Constructors
  //---------------------------------------------------------------------

  NilpotentPresentation() : built( false ), theCollector(1,1) { }
  // default constructor creates empty presentation.
  // The only purpose of this constructor is to allocate space for the
  // object. The object must be initialized by assignment.

  NilpotentPresentation(const NGCollector& coll,
			const VectorOf<Word>& rels)
    : built(false), theGroupRelators(rels), theCollector(coll)
  { }
 

  // copy constructor, destructor and assignment operator provided by compiler


  //---------------------------------------------------------------------
  //  User interface
  //---------------------------------------------------------------------

  bool isBuilt() const { return built; }
  // returns true iff the presentation is built

  void build();
  // to build the presentation completely

  void print( ostream& s ) const;
  // output presentation to the stream s

  class FPGroup makeGroup() const;
  // produces a group using the presentation


  //---------------------------------------------------------------------
  //   IPC tools
  //---------------------------------------------------------------------

  virtual ostream& write(ostream& s) const;

  virtual istream& read(istream& s);
 

  //---------------------------------------------------------------------
  //  Helper methods to form and use the presentation
  //---------------------------------------------------------------------

protected:

  SetOf<Word> convertRelators() const;
  // converts the presentation relators to Words

  Chars printRelator(int i) const;
  // outputs the ith relator in terms of theGenerators

  void buildKernelRelators();
  // builds relators di = 1.

  void improvePresentation();
  // extract identity generators from presentation

  //---------------------------------------------------------------------
  //  Virtual helper methods specific for each kind of presentation
  //---------------------------------------------------------------------

  virtual PolyWord decompose( const PolyWord& pw ) const = 0;
  // decomposes the word given in terms of the group generators
  // the result is the word in terms of theGenerators.

  virtual void printGenerators( ostream& s ) const = 0;
  // outputs the generators as a word s in terms of the group's generators

  //---------------------------------------------------------------------
  //  Data
  //---------------------------------------------------------------------

protected:

  VectorOf<PolyWord> theGenerators;
  //theGenerators are words in terms of the group generators
  //They are set within constructor

  VectorOf<Chars> theNames;
  // the names for the words from theGenerators
  //They are set within constructor

  VectorOf<NilpotentRelator> theRelators;
  // The nilpotent relators form presentation.
  //They are set within build() method.

  bool built;
  // Indicates whether the presentation is built.

  VectorOf<Word> theGroupRelators;
  // The relators of the nilpotent group
  
  NGCollector theCollector;
  //the group collector
};


//=======================================================================
//====================== PresentationForNG ==============================
//=======================================================================

// The presentation for nilpotent group
// theGenerators are just 1,2, ....
// theNames are c1,c2, ....
// theRelators are all commutation relators plus
//     di = 1 where di are words of Malcev basis for the relation subgroup
//     (in FPNilpotentGroup case)




class PresentationForNG : public NilpotentPresentation {

public:

  //---------------------------------------------------------------------
  //  Constructors
  //---------------------------------------------------------------------

  PresentationForNG(const NGCollector& coll, const VectorOf<Word>& rels);

protected:

  virtual PolyWord decompose( const PolyWord& pw ) const;
  // decomposes the word given in terms of the group generators
  // the result is the word in terms of theGenerators.

  void printGenerators( ostream& s ) const { }
  // nothing to print (now)

};



//=======================================================================
//====================== PresentationForSNG =============================
//=======================================================================

// The presentation for subgroup of nilpotent group
// theGenerators are the words of Malcev basis for the full preimage
// theRelators are all commutation relators plus
//     di = 1 where di are words of Malcev basis for the relation subgroup
//     (in SGOfFPNilpotentGroup case)


class PresentationForSNG : public NilpotentPresentation {

public:

  //---------------------------------------------------------------------
  //  Constructors
  //---------------------------------------------------------------------

  PresentationForSNG(const MalcevSet& preimage, const VectorOf<Word>& rels,
		     const NGCollector& coll);
  
protected:

  virtual PolyWord decompose( const PolyWord& pw ) const;
  // decomposes the word given in terms of the group generators
  // the result is the word in terms of theGenerators.

  virtual void printGenerators( ostream& s ) const;

private:

  MalcevSet thePreimageBasis;
};


//=======================================================================
//====================== global functions ===============================
//=======================================================================


inline ostream& operator < ( ostream& s, const NilpotentPresentation& pres ) {
  return pres.write(s);
}
// IPC output

inline istream& operator > ( istream& s, NilpotentPresentation& pres ) {
  return pres.read(s);
}
// IPC input


#endif
