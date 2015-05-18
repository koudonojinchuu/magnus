// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the Group class.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// Special Remarks:
//
// * As a general principle, the (conditional) error-checking takes
//   place as close to the root of the class hierarchy and at the
//   location where the construction actually takes place;
//   for instance checking for a negative number of generators is done
//   if FGGroup but not higher. This has the slight
//   inconvenience that the user does not immediately see where the
//   origin of the error is, but saves placing error traps.
//
// * Tentatively, operator == and != are not delegated to virtual functions.
//   Instead, every group will be compared according to the type by which
//   it is referenced, and these operators must be defined in every class
//   of the hierarchy which adds representation information. (@rn huh?)
//   The meaning of comparison is thus equality of representation as
//   referenced.
//
// Bugs:
//
// * A bug in g++ 2.5.8 and less forces to derive Group publicly from
//   ObjectOf<GroupRep> although there is no reason to do this.
//   The difficulty may be due to a confusion in the assignment of
//   objects of derived classes
//
// * 07/96 Alexey M. implemented IPC tools.
//

#ifndef _GROUP_H_
#define _GROUP_H_


#include "Type.h"
#include "IStreamPoll.h"
#include "GroupRep.h"


class Group : public GenericObject {
  
public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor for pseudo-abstract class.
  // Copy constructor and destructor supplied by compiler.
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Operators                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Operator= supplied by compiler.

  int operator == ( const Group& G ) const 
  { 
    if( actualType() != G.actualType() )
      return false;
    if( look() == G.look() ) 
      return true; 
    return look()->compare( G.look() );
  }
  // @rn:stc Can we do better than this?
  // @dp made some enhancements.
  
  int operator != ( const Group& G ) const { return !(*this == G); }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  static Type type( ) { return GroupRep::type(); }

  Type actualType( ) const { return look()->actualType(); }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Group structure methods                          //
  //                                                   //
  ///////////////////////////////////////////////////////

  int order( ) const; // pseudo-virtual
  // Returns the order of this group, or 0 if this is infinite,
  // or -1 if no answer can be determined.

  Trichotomy isTrivial( ) const { return enhance()->isTrivial(); }
  // pseudo-virtual

  Trichotomy isFinite( ) const { return enhance()->isFinite(); }
  // pseudo-virtual

  Trichotomy isInfinite( ) const { return enhance()->isInfinite(); }
  // pseudo-virtual

  Trichotomy isAbelian( ) const { return enhance()->isAbelian(); }
  // pseudo-virtual


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////


  Elt makeIdentity( ) const { return look()->makeIdentity(); }
  // Returns an object which this group treats as a
  // syntactic representation of its identity element.

  Bool isSyntacticIdentity(const Elt& e) const {
	 return look()->isSyntacticIdentity(e);
  }
  // Says whether this group considers e to be a syntactic representation
  // of its identity element. This does not try to solve the word problem,
  // or apply such a solution if it exists.

  Trichotomy isTrivialElt( const Elt& e ) const
	{ return look()->isTrivialElt(e); }
  // Attempts to determine whether e is or represents the identity in
  // this group

  Trichotomy areEqual(const Elt& e1, const Elt& e2) const {
	 return enhance()->areEqual(e1, e2);
  }
  // Tries to decide whether e1 and e2 represent the same element
  // of this group.

  Elt firstElt( ) const { return look()->firstElt(); }
  // Returns the first Elt in a fixed, pre-determined sequence of Elts
  // which maps onto, but not necessarily 1-1, the set of this group's
  // elements.

  Elt nextElt(const Elt& e) const { return look()->nextElt(e); }
  // Returns the element which comes after e in the sequence described
  // above. Guaranteed defined only on Elts returned by firstElt or
  // nextElt, since the sequence may not contain all Elts.

  Elt multiply(const Elt& e1, const Elt& e2) const {
	 return look()->multiply(e1, e2);
  }
  // Returns a representation of the product of e1 and e2 in this group.

  Elt inverseOf(const Elt& e) const { return look()->inverseOf(e); }
  // Returns a representation of the inverse of e in this group.

  Elt raiseToPower(const Elt& e, int n) const {
	 return look()->raiseToPower(e, n);
  }
  // Returns a representation of e^n in this group, for any int n.

  Elt conjugateBy(const Elt& e1, const Elt& e2) const {
	 return look()->conjugateBy(e1, e2);
  }
  // Returns a representation of e2^-1 e1 e2 in this group.

  Elt commutator(const Elt& e1, const Elt& e2) const {
	 return look()->commutator(e1, e2);
  }
  // Returns a representation of e1^-1 e2^-1 e1 e2 in this group.


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with sets of group elements   //
  //                                                   //
  ///////////////////////////////////////////////////////


  SetOf<Elt> setMultiply(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const;
  SetOf<Elt> setMultiply(const Elt& e, const SetOf<Elt>& S) const;
  SetOf<Elt> setMultiply(const SetOf<Elt>& S, const Elt& e) const;
  // These return elementwise products, multiplying in the order of the args.

  SetOf<Elt> conjugateBy(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const;
  SetOf<Elt> conjugateBy(const Elt& e, const SetOf<Elt>& S) const;
  SetOf<Elt> conjugateBy(const SetOf<Elt>& S, const Elt& e) const;
  // By convention, conjugateBy(S1, S2) = { s2^-1 s1 s2 : s_i in S_i }.

  void closeUnderInverses(SetOf<Elt>& S) const;
  // Alters S so that it is closed under (group element) inverses.
   

  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////


  // @stc outdated: (valid only for ostream & istream)
  // The paradigm for I/O:
  //
  // The Rep of the root of a hierarchy H defines
  //
  // virtual void printOn(ostream&) const;
  // virtual Rep* readFrom(istream&, Chars&) const;
  //
  // The root defines
  //
  // friend ostream& operator << ( ostream&, const H& );
  // friend IStreamPoll operator >> ( istream&, H& );
  //
  // which delegate to printOn and readFrom, resp.
  // Subclasses of Rep override printOn/readFrom.
  // readFrom returns NULL when there is a parse error;
  // in this case, >> does not change H.

  // the old i/o way :

  friend ostream& operator << ( ostream&, const Group& );
  
  friend IStreamPoll operator >> ( istream&, Group& );
 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Group& G )
  {
    G.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Group& G )
  {
    G.change()->read(istr);
    return istr;
  }



  ///////////////////////////////////////////////////////
  //                                                   //
  //  Representation access methods                    //
  //                                                   //
  ///////////////////////////////////////////////////////


protected:

  // Shadow representation accessors to get representations of the
  // right type in the members of this class:

  const GroupRep* look( ) const {
	 return (const GroupRep*)GenericObject::look();
  }
  GroupRep* enhance( ) const {
	 return (GroupRep*)GenericObject::enhance();
  }
  GroupRep* change( ) {
	 return (GroupRep*)GenericObject::change();
  }

  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:

  Group( GroupRep* newrep ) : GenericObject(newrep) { }


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

#ifdef DEBUG

private:
  
  //friend int main( );
  friend void debugPrint(ostream&, const Group& g);

public:

  bool consistent( ) const { return look()->consistent(); } // pseudo-virtual

#endif
  
};


//--------------------------- Group -----------------------------------//
//------------------- top level i/o functions -------------------------//
 

/*@rn
// @stc Ostream& operator << ( Ostream&, const Group& );
// @stc provisional bypass:
inline Ostream& operator << ( Ostream& o, const Group& g ) {
	ostream(o) << g;
	return o;
}
*/

#endif
