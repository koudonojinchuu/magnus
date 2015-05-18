// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FreeGroup class.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// Usage: @rn move to manual discussion of IStreamPoll
//
// * I/O:
//   The normal high-level i/o is performed by the operators
//		ostr << G;
//    istr >> G;
//	 The former returns an ostream&, the latter returns an intermediate
//	 type IStreamPoll, which is automatically converted depending on how
//   you use it. It can become
//   - an int, to do
//           FreeGroup G[20]; int i = 0;
//           while ( i < 20 && cin >> G[i]) i++;
//   - a Chars, to poll an error message
//           FreeGroup F;
//           Chars errMesg = cin >> F;
//   - an istream, to concatenate in the usual manner
//           cin >> F >> G;
//
// Special Notes:
//
// * Tentatively, FreeGroup derives directly from FGGroup;
//   in other words a free group is not treated as a special case of a
//   FPGroup where the set of relators is empty, but as
//   an instance of a FGGroup with a known representation
//   for the elements, freely reduced words.
//
// Questions:
//
// * how much of the functions do we want to be (pseudo-) virtual?
//

#ifndef _FREE_GROUP_H_
#define _FREE_GROUP_H_

#include "FGGroup.h"
#include "FreeGroupRep.h"
#include "Map.h"
#include "File.h"

class Aut; // Doesn't exist yet

class FreeGroup : public DerivedObjectOf<FGGroup,FreeGroupRep> {
  
public:
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////
  
  // Copy constructor provided by compiler

  FreeGroup( int rank = 0 ) :
    DerivedObjectOf<FGGroup,FreeGroupRep>( new FreeGroupRep(rank) ) { }
  // To construct a group of given rank.
  // Default constructor gives trivial group with rank 0.
  
  FreeGroup( const VectorOf<Chars>& gennames ) :
    DerivedObjectOf<FGGroup,FreeGroupRep>( new FreeGroupRep(gennames) ) { }
  // To construct a group with named generators.
  
  // Destructor provided by compiler.
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Operators                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  // assignement operator provided by compiler
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  static Type type( ) { return FreeGroupRep::type(); }
  // dominates FGGroup::type();

  // Inherited from FGGroup: @rn update
  //  Type actualType() const; // pseudo-virtual
  //  int numberOfGenerators( ) const
  //  Chars nameOfGenerator(int i) const
  //  VectorOf<Chars> namesOfGenerators( ) const

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Group structure methods                          //
  //                                                   //
  ///////////////////////////////////////////////////////

  int rank( ) const { return numberOfGenerators(); }
  
  // members to manipulate homs :
  
  Aut randomAut( ) const; // @stc some thought has to be put into this and
                          // into manipulating distributions

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods dealing with group elements              //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup: @rn update
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
  // Elt eval( const Word& w ) const;
  // Trichotomy wordProblem( const Word& w ) const;
  // Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;

  Trichotomy conjugacyProblem( const Word& u, const Word& v ,Word& c) const {
       return look()->conjugacyProblem(u,v,c);
  }

  Bool inCommutatorSG(const Word& w) const { 
    return look()->inCommutatorSG(w);
  }
  // Returns TRUE iff `w' is in the commutator subgroup.

  Bool isCommutator(const Word& w, Word& u1, Word& u2) const {
    return look()->isCommutator(w,u1,u2);
  }
  // Returns TRUE iff `w' is a commutator. If TRUE, `u1' and `u2'
  // are set to words such that w = [u1,u2].

  Chars productOfCommutators( const Word& w , File& file )
    {
      return change()->productOfCommutators(w,file);
    }
  // If the word is in derived subgroup this procedure rewrites the 
  // word as a product of commutators.

  Chars productOfSquares( const Word& w , File& file )
    {
      return change()->productOfSquares(w,file);
    }
  // If the word is in the subgroup generated by all squares this procedure 
  // rewrites it as a product of commutators.

  Word getN_thElement( int n ) const
  {
    return look()->getN_thElement(n);
  }
  // Get the n-th element of a free group.

  int numberOfElement( const Word& w ) const
  {
    return look()->numberOfElement(w);
  }
  // Computes the number of the word in the free group.

  VectorOf<Word> nielsenBasis(const VectorOf<Word>& V) const {
    return look()->nielsenBasis(V);
  }
  // Returns a nielsen basis for the subgroup generated by `V'.

  typedef FreeGroupRep::NielsenBasis NielsenBasis;

  NielsenBasis nielsenBasis(const VectorOf<Word>& V, bool writeToFile ) const {
    return NielsenBasis(V, namesOfGenerators(), writeToFile );
  }
  // Returns a nielsen basis for the subgroup generated by `V'
  // and decomposition of the nielsen basis in V.

  Bool isAutomorphism(const VectorOf<Word>& V) const {
    return look()->isAutomorphism(V);
  }
  // Returns TRUE iff generating vector `V' defines an
  // automorphism of this group. The length of `V' should be
  // equal to the rank of this group.

  Bool isInnerAutomorphism(const VectorOf<Word>& V) const {
    return look()->isInnerAutomorphism(V);
  }
  // Returns TRUE iff generating vector `V' defines an inner
  // automorphism of this group. The length of `V' should be
  // equal to the rank of this group and `V' should define an
  // automorphism.

  bool isIAAutomorphism(const VectorOf<Word>& V) const {
    return look()->isIAAutomorphism(V);
  }
  // Returns `true' iff generating vector `V' defines an
  // IA-automorphism of this group. The length of `V' should be
  // equal to the rank of this group and `V' should define an
  // automorphism.

  VectorOf<Word> inverseAutomorphism(const VectorOf<Word>& V) const {
    return look()->inverseAutomorphism(V);
  }
  // Returns a generating vector for the inverse automorphism.
  // `V' should define an automorphism of this group.

  Map inverseAutomorphism( const Map& M) const 
  {
    return Map( *this, *this, inverseAutomorphism( M.generatingImages() ) );
  }
  // Returns an inverse automorphism.

  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods dealing with subgroups                   //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup: @rn update
  //  Subgroup randomSubgroup( )

  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup: @rn update
  //  Word readWord(istream& istr, Chars& errMesg) const
  //  void printGenerators( ostream& ostr ) const
  //  void printWord( ostream& ostr, const Word& w ) const
 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Representation access methods                    //
  //                                                   //
  ///////////////////////////////////////////////////////

/* 
private:
 
  typedef FreeGroupRep ThisRep;
  typedef FGGroup Base;
 
  // Shadow representation accessors to get representations of the
  // right type in the members of this class:
 
  const ThisRep* look( ) const {
    return (const ThisRep*)GenericObject<FreeGroupRep>::look();
  }
  ThisRep* enhance( ) const {
	 return (ThisRep*)GenericObject<FreeGroupRep>::enhance();
  }
  ThisRep* change( ) {
	 return (ThisRep*)GenericObject<FreeGroupRep>::change();
  }
*/
 
protected:
 
  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  FreeGroup( FreeGroupRep* newrep ) :
    DerivedObjectOf<FGGroup,FreeGroupRep>(newrep)
  { }


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

private:
  
#ifdef DEBUG
  //friend int main( );
  friend void debugPrint(ostream&, const FreeGroup& g);
#endif

};

#endif
