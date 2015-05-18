// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FGGroup class.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// Questions:
//
// * methods which may enhance a group, such as wordProblem(), are const,
//   to permit application to const objects;
//
// 07/96 Alexey M. implemented IPC tools

#ifndef _FINITELY_GENERATED_GROUP_H_
#define _FINITELY_GENERATED_GROUP_H_


#include "DerivedObjectOf.h"
#include "Group.h"
#include "FGGroupRep.h"


class Subgroup; // Doesn't exist yet.


class FGGroup : public DerivedObjectOf<Group,FGGroupRep> {
  
public:
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor for pseudo-abstract class.
  // Copy constructor, operator=, and destructor supplied by compiler.

  FGGroup( const Group& g ) : DerivedObjectOf<Group,FGGroupRep>( g ) { }
  // cast constructor: to cast an arbitrary group as an FG group
  // NB. This rewraps the unchanged representation, hence is in general
  // only useful for casting a group known to be an actual FG group.
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  static Type type( ) { return FGGroupRep::type(); }
  // dominates Group::type();

  // Inherited from Group:
  //  Type actualType() const; // pseudo-virtual

  int numberOfGenerators( ) const { return look()->theNumberOfGenerators; }
  
  Chars nameOfGenerator(int i) const;
  // Returns print name of (i + 1)th generator. Zero-based indexing.
  // No negative i's; does not return print names of formal inverses.
  // @rn Take a Generator instead?

  Chars nameOfGenerator(Generator g) const; //@rn

  VectorOf<Chars> namesOfGenerators( ) const {
	 return look()->theNamesOfGenerators;
  }
  // Returns vector of print names of all generators, in the order of
  // their ordinals.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Group structure methods                          //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from Group:
  // int order( ) const; // pseudo-virtual
  // Trichotomy isTrivial( ) const; // pseudo-virtual
  // Trichotomy isFinite( ) const; // pseudo-virtual
  // Trichotomy isInfinite( ) const; // pseudo-virtual
  // Trichotomy isAbelian( ) const; // pseudo-virtual

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from Group:
  // Elt makeIdentity( ) const; // pseudo-virtual
  // Bool isSyntacticIdentity(const Elt& e) const; // pseudo-virtual
  // Trichotomy isTrivialElt( const Elt& e ) const; // pseudo-virtual
  // Trichotomy areEqual(const Elt& e1, const Elt& e2) const; // pseudo-virtual
  // Elt firstElt( ) const; // pseudo-virtual
  // Elt nextElt(const Elt& e) const; // pseudo-virtual
  // Elt multiply(const Elt& e1, const Elt& e2) const; // pseudo-virtual
  // Elt inverseOf(const Elt& e) const; // pseudo-virtual
  // Elt raiseToPower(const Elt& e, int n) const; // pseudo-virtual
  // Elt conjugateBy(const Elt& e1, const Elt& e2) const; // pseudo-virtual
  // Elt commutator(const Elt& e1, const Elt& e2) const; // pseudo-virtual
  
  Elt eval( const Word& w ) const { return look()->eval(w); }
  // Takes a word and evaluates it as a formal word in the generators
  // of the group. Returns a (possibly canonic) representative of the
  // element. This method is pseudo-virtual.

  Trichotomy wordProblem( const Word& w ) const {
	 return enhance()->wordProblem(w);
  }
  // Returns YES if w represents the identity, NO if not, and
  // DONTKNOW if no answer can be determined.

  Trichotomy conjugacyProblem( const Word& u, const Word& v ) const {
	 return enhance()->conjugacyProblem(u, v);
  }
  // Returns YES if u and v represent conjugate elements, NO if not, and
  // DONTKNOW if no answer can be determined.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with sets of group elements   //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from Group:
  // SetOf<Elt> setMultiply(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const;
  // SetOf<Elt> setMultiply(const Elt& e, const SetOf<Elt>& S) const;
  // SetOf<Elt> setMultiply(const SetOf<Elt>& S, const Elt& e) const;
  // SetOf<Elt> conjugateBy(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const;
  // SetOf<Elt> conjugateBy(const Elt& e, const SetOf<Elt>& S) const;
  // SetOf<Elt> conjugateBy(const SetOf<Elt>& S, const Elt& e) const;
  // void closeUnderInverses(SetOf<Elt>& S) const;

  void closeUnderCyclicPermutations(SetOf<Word>& S) const;
  // Alters S so that if w is in S, then any cyclic permutation of w is too.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word readWord(istream& istr, Chars& errMesg) const;
  // Tries to parse and return a Word from istr. At present, the word
  // in istr must end in ',', so the parser knows when to stop.
  // If there is a parse error, a message is returned in errMesg,
  // and the empty Word is returned.

  SetOf<Word> readSetOfWords(istream& istr, Chars& errMesg) const;
  // Tries to parse and return a set of Words, delimited by '{', '}' and
  // comma-separated, from istr.
  // If there is a parse error, a message is returned in errMesg,
  // and the empty set is returned.

  VectorOf<Word> readVectorOfWords(istream& istr, Chars& errMesg) const;
  // Tries to parse and return a set of Words, delimited by '{', '}' and
  // comma-separated, from istr.
  // If there is a parse error, a message is returned in errMesg,
  // and the empty set is returned.

  void printGenerator( ostream& ostr, const Generator& g ) const {
	 look()->printGenerator( ostr, ord(g) );
  }
  // Outputs the print name of g on ostr, followed by "^-1" if ord(g) < 0.

  void printGenerators( ostream& ostr ) const {
	 look()->printGenerators( ostr );
  }
  // Calls printGenerator for each generator of this group, separating
  // them with commas.

  void printWord( ostream& ostr, const Word& w ) const {
	 look()->printWord( ostr, w );
  }
  // Outputs w to ostr, in a form that can input by readWord.

  void printSetOfWords( ostream& ostr, const SetOf<Word>& S ) const {
 	 look()->printSetOfWords( ostr, S );
  }
  // Outputs S to ostr, in a form that can input by readWord.

  void printVectorOfWords( ostream& ostr, const VectorOf<Word>& V,
	    char* leftBracket = "{", char* rightBracket = "}" ) const 
  {
    look()->printVectorOfWords( ostr, V, leftBracket, rightBracket );
  }
  // Outputs V to ostr, in a form that can input by readWord.



  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with subgroups                //
  //                                                   //
  ///////////////////////////////////////////////////////

  
  Subgroup randomSubgroup( ) const; // @stc this has to be set up with
                              // handles to manipulate distributions


 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Representation access methods                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  // operators inhereted from Group  

 
/* @rn inherited
private:
 
  typedef FGGroupRep ThisRep;
  typedef Group Base;
 
  // Shadow representation accessors to get representations of the
  // right type in the members of this class:
 
  const FGGroupRep* look( ) const {
    return (const ThisRep*)DerivedObjectOf<Group,FGGroupRep>::look();
  }
  FGGroupRep* enhance( ) const {
	 return (ThisRep*)DerivedObjectOf<Group,FGGroupRep>::enhance();
  }
  FGGroupRep* change( ) {
	 return (ThisRep*)DerivedObjectOf<Group,FGGroupRep>::change();
  }
*/
 
protected:

  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  FGGroup( FGGroupRep* newrep ) : DerivedObjectOf<Group,FGGroupRep>(newrep) { }
 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////
 
#ifdef DEBUG

  // Inherited from Group:
  // bool consistent( ) const;

#endif

};

#endif
