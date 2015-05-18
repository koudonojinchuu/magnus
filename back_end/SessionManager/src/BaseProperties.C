// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//           
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//
// Special Notes:
//
// Revision History:
//

#include "BaseProperties.h"

MSCGroupPtr::MSCGroupPtr( const FPGroup& G, int lmbda ) 
  : fp(G), lambda(lmbda), msc(0)
{
}


MSCGroupPtr::MSCGroupPtr()
  : fp(), lambda(0), msc(0)
{
}


MSCGroupPtr::MSCGroupPtr( const MSCGroupPtr& G )
  : fp(G.fp), lambda(G.lambda), msc(0)
{
}

MSCGroupPtr::~MSCGroupPtr()
{
  delete msc;
}

MSCGroupPtr& MSCGroupPtr::operator= ( const MSCGroupPtr& G )
{
  if( &G != this ) {
    fp = G.fp;
    lambda = G.lambda;
    delete msc;
    msc = 0;
  }
  return *this;
}

const MSCGroup& MSCGroupPtr::getMSCG() const
{
  if( !msc ) {
    MSCGroupPtr *This = const_cast<MSCGroupPtr*>(this);
    This->msc = new MSCGroup( fp, lambda );
  }
  return *msc;
}

istream& operator>>(istream& istr, MSCGroupPtr& G)
{
  istr >> G.lambda;
  istr >> G.fp;

  delete G.msc;
  G.msc = 0;

  return istr;
}

ostream& operator<<(ostream& ostr, const MSCGroupPtr& G )
{
  FPGroup FP( G.fp.namesOfGenerators(), G.fp.getRelators() );
  ostr << G.lambda << ' ' << FP;
  return ostr;
}


MSCGroupProperty::MSCGroupProperty(
  const FPGroup& G, int lambda, const Chars& descr )
  : SimpleProperty<MSCGroupPtr>( MSCGroupPtr(G,lambda), descr) 
{
}

void NilpotentGroupProperty::readFrom( istream& istr )
{
  int relatorsCount;
  istr > relatorsCount;

  if( relatorsCount ) {
    theData = NilpotentGroup( 2, 0, VectorOf<Word>(1) );
  }
  else {
    theData = NilpotentGroup( 2, 0 );
  }

  istr > theData;
}


NilpGroupAssocProperty::NilpGroupAssocProperty( const NilpGroupAssocProperty& P )
  : ComplexProperty< AssociationsOf<int,NilpotentGroup*> > ( P )
{
  AssociationsIterator< int, NilpotentGroup* > I(theData);
  for( ; !I.done(); I.next() ) {
    theData.unbind( I.key() );
  }
  copyAll( P );
}

const NilpGroupAssocProperty& 
NilpGroupAssocProperty::operator=( const NilpGroupAssocProperty& P )
{
  if( &P != this ) {
    unbindAll();
    copyAll( P );
  }
}

NilpGroupAssocProperty::~NilpGroupAssocProperty()
{
  unbindAll();
}

void NilpGroupAssocProperty::unbindAll() 
{
  AssociationsIterator< int, NilpotentGroup* > I(theData);
  for( ; !I.done(); I.next() ) {
    delete I.value();
    theData.unbind( I.key() );
  }
}

void NilpGroupAssocProperty::copyAll( const NilpGroupAssocProperty& P )
{
  AssociationsIterator< int, NilpotentGroup* > I(P.theData);
  for( ; !I.done(); I.next() ) {
    theData.bind( I.key(), new NilpotentGroup(*I.value()) );
  }
}

void NilpotentGroupProperty::writeTo( ostream& ostr )
{
  int relatorsCount = theData.relators().length();
  ostr < relatorsCount < theData;
}

void NilpGroupAssocProperty::readFrom( istream& istr )
{
  unbindAll();

  int count, relatorsCount;
  istr > count > relatorsCount;

  NilpotentGroup theGroup( 0, 2 );
  if( relatorsCount > 0 ) {
    theGroup = NilpotentGroup( 0, 2, VectorOf<Word>(1) );
  }

  for( int i = 0; i < count; ++i ) {
    int theClass;
    istr > theClass > theGroup;
    theData.bind( theClass, new NilpotentGroup(theGroup) );
  }
}

void NilpGroupAssocProperty::writeTo( ostream& ostr ) const 
{
  AssociationsIterator< int, NilpotentGroup* > I(theData);
  
  ostr < theData.cardinality();

  if( theData.cardinality() == 0 ) {
    ostr < 0;
  } 
  else {
    ostr < I.value()->relators().length();
  }

  for( ; !I.done(); I.next() ) {
    ostr < I.key() < *I.value();
  }
}

void ListOfEndomorphismProperty::readFrom(istream& istr)
{
  theData = ListOf<Endomorphism>();

  int len;

  istr > len;

  FreeGroup F;
  Map map(F, F);
  for( int j = 0; j < len; ++j ) {
    istr > map;
    Endomorphism endo( map.domain(), map.generatingImages() );
    theData.append( map );
  }
}

void ListOfEndomorphismProperty::writeTo (ostream& ostr) const
{
  ostr < theData.length();
  ListIterator< ListOf<Endomorphism> > I( theData );
  for( ; !I.done(); I.next() ) {
    ostr < ' ' < I.value();
  }
}


void ListOfAutomorphismProperty::readFrom(istream& istr)
{
  theData = ListOf<Automorphism>();

  int len;

  istr > len;

  FreeGroup F;
  Map map(F, F);
  for( int j = 0; j < len; ++j ) {
    istr > map;
    Automorphism aut( map.domain(), map.generatingImages() );
    theData.append( aut );
  }
}

void ListOfAutomorphismProperty::writeTo (ostream& ostr) const
{
  ostr < theData.length();
  ListIterator< ListOf<Automorphism> > I( theData );
  for( ; !I.done(); I.next() ) {
    ostr < ' ' < I.value();
  }
}

void SGOfNilpotentGroupProperty::readFrom( istream& istr )
{
  int relatorsCount;
  istr > relatorsCount;

  NilpotentGroup theGroup( 2, 0 );
  if( relatorsCount ) {
    theGroup = NilpotentGroup( 2, 0, VectorOf<Word>(1) );
  }

  theData = SGOfNilpotentGroup( theGroup, VectorOf<Word>(0) );

  istr > theData;
}


void SGOfNilpotentGroupProperty::writeTo( ostream& ostr ) const
{
  int relatorsCount = theData.parentGroup().relators().length();
  ostr < relatorsCount < theData;
}

SGNilpGroupAssocProperty::SGNilpGroupAssocProperty( const SGNilpGroupAssocProperty& P )
  : ComplexProperty< AssociationsOf<int,SGOfNilpotentGroup*> > ( P )
{
  AssociationsIterator< int, SGOfNilpotentGroup* > I(theData);
  for( ; !I.done(); I.next() ) {
    theData.unbind( I.key() );
  }
  copyAll( P );
}

const SGNilpGroupAssocProperty& 
SGNilpGroupAssocProperty::operator=( const SGNilpGroupAssocProperty& P )
{
  if( &P != this ) {
    unbindAll();
    copyAll( P );
  }
}

SGNilpGroupAssocProperty::~SGNilpGroupAssocProperty()
{
  unbindAll();
}

void SGNilpGroupAssocProperty::unbindAll() 
{
  AssociationsIterator< int, SGOfNilpotentGroup* > I(theData);
  for( ; !I.done(); I.next() ) {
    delete I.value();
    theData.unbind( I.key() );
  }
}

void SGNilpGroupAssocProperty::copyAll( const SGNilpGroupAssocProperty& P )
{
  AssociationsIterator< int, SGOfNilpotentGroup* > I(P.theData);
  for( ; !I.done(); I.next() ) {
    theData.bind( I.key(), new SGOfNilpotentGroup(*I.value()) );
  }
}

void SGNilpGroupAssocProperty::readFrom(istream& istr)
{
  AssociationsIterator< int, SGOfNilpotentGroup* > I(theData);
  for( ; !I.done(); I.next() ) {
    delete I.value();
    theData.unbind( I.key() );
  }

  int count, relatorsCount;
  istr > count > relatorsCount;

  NilpotentGroup theGroup( 0, 2 );
  if( relatorsCount > 0 ) {
    theGroup = NilpotentGroup( 0, 2, VectorOf<Word>(1) );
  }
  
  SGOfNilpotentGroup theSubgroup( theGroup, VectorOf<Word>(0) );
  for( int i = 0; i < count; ++i ) {
    int theClass;
    istr > theClass > theSubgroup;

    theData.bind( theClass, new SGOfNilpotentGroup(theSubgroup) );
  }
}

void SGNilpGroupAssocProperty::writeTo(ostream& ostr) const 
{
  AssociationsIterator< int, SGOfNilpotentGroup* > I(theData);

  ostr < theData.cardinality();

  if( theData.cardinality() == 0 ) {
    ostr < 0;
  } 
  else {
    ostr < I.value()->parentGroup().relators().length();
  }

  for( ; !I.done(); I.next() ) {
    ostr < I.key() < *I.value();
  }
}

