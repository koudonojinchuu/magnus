// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the Group class
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//

#include "Group.h"


// Methods and operators which deal with the group:


int Group::order( ) const {
  if ( look()->theOrder != -1 ) return look()->theOrder;
  else return enhance()->order();
}
//@rn hide theOrder


ostream& operator << ( ostream& ostr, const Group& G )
{
  G.look()->printOn(ostr);
  return ostr;
}


IStreamPoll operator >> ( istream& istr, Group& G )
{
  Chars errMesg;
  GroupRep* rep = G.look()->readFrom(istr, errMesg);
  if ( rep ) G = Group(rep);
  // readFrom returns NULL on error.
  return IStreamPoll( istr, errMesg );
}


/* @stc don't want this in this form anymore: should delegate printing
   of representation information to base

#ifdef DEBUG
  void debugPrint(ostream& s, const Group& g ) {

	 s << "Group:: this : " << &g;
	 s << "; rep : " << g.look();
	 s << "; nxrefs : " << g.look()->nxrefs();
  }
#endif
*/


// Methods which deal with group elements:


// Methods which deal with sets of group elements:


SetOf<Elt> Group::setMultiply(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I1(S1);
  SetIterator<Elt> I2(S2);
  while ( !I1.done() ) {
	 while ( !I2.done() ) {
		result |= multiply( I1.value(), I2.value() );
		I2.next();
	 }
	 I2.reset();
	 I1.next();
  }
  return result;
}


SetOf<Elt> Group::setMultiply(const Elt& e, const SetOf<Elt>& S) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I(S);
  while ( !I.done() ) {
	 result |= multiply( e, I.value() );
	 I.next();
  }
  return result;
}


SetOf<Elt> Group::setMultiply(const SetOf<Elt>& S, const Elt& e) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I(S);
  while ( !I.done() ) {
	 result |= multiply( I.value(), e );
	 I.next();
  }
  return result;
}


SetOf<Elt> Group::conjugateBy(const SetOf<Elt>& S1, const SetOf<Elt>& S2) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I1(S1);
  SetIterator<Elt> I2(S2);
  while ( !I1.done() ) {
	 while ( !I2.done() ) {
		result |= conjugateBy( I1.value(), I2.value() );
		I2.next();
	 }
	 I2.reset();
	 I1.next();
  }
  return result;
}


SetOf<Elt> Group::conjugateBy(const Elt& e, const SetOf<Elt>& S) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I(S);
  while ( !I.done() ) {
	 result |= conjugateBy( e, I.value() );
	 I.next();
  }
  return result;
}


SetOf<Elt> Group::conjugateBy(const SetOf<Elt>& S, const Elt& e) const
{
  SetOf<Elt> result;
  SetIterator<Elt> I(S);
  while ( !I.done() ) {
	 result |= conjugateBy( I.value(), e );
	 I.next();
  }
  return result;
}


void Group::closeUnderInverses(SetOf<Elt>& S) const
{
  SetIterator<Elt> I(S);
  while ( !I.done() ) {
	 S |= inverseOf( I.value() );
	 I.next();
  }
}


//------------------- top level i/o function -------------------------//


//-------------------------- INITIALISATIONS -----------------------//
//-------------------------- mind the order! -----------------------//


//--------------------------- GroupRep -----------------------------//
//------------------------- Static members -------------------------//

const Type GroupRep::theGroupType = Type( Type::unique() );


//----------------------------- Group ------------------------------//
//------------------------- Static members -------------------------//


