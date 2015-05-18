//   Copyright (C) 1998 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of base properties.
//
// Principal Author: Dmitry Pechkin
//
// Status: In progress.
//
// Revision History:
//
// Special Notes:
//
// Further implementation steps:
//
// Bugs:
//
//

#ifndef _BASE_PROPERTIES_H_
#define _BASE_PROPERTIES_H_

#include "Property.h"
 
#include "Associations.h"
#include "QuickAssociations.h"
#include "Chars.h"
#include "APofFreeGroups.h"
#include "FreeByCyclic.h"
#include "MSCGroup.h"
#include "Automorphism.h"
#include "HNNExtOfFreeGroup.h"
#include "NilpotentGroup.h"
#include "CosetEnumerator.h"
#include "DFSA.h"
#include "DiffMachine.h"
#include "KBMachine.h"
#include "DecomposeInSubgroup.h"
#include "AbelianSGPresentation.h"
#include "SGOfNilpotentGroup.h"
#include "PolyWord.h"

//============================================================
//
//          Simple properties:
//
//============================================================

class NoData {};
//  I/O stubs
inline istream& operator>>( const istream&, const NoData& ) {}
inline ostream& operator<<( const ostream&, const NoData& ) {}
 

/*
class $A$Property : public SimpleProperty< $S$ > {
public:
  $S$Property( const $S$& data, const Chars& descr = Chars() ) 
    : SimpleProperty< $S$ >(data, descr) {}
};
*/


class NoDataProperty : public SimpleProperty<NoData> {
public:
  NoDataProperty( const Chars& descr = Chars() ) 
    : SimpleProperty<NoData>(NoData(), descr) {}
};

class BoolProperty : public SimpleProperty<bool> {
public:
  BoolProperty( const bool& data, const Chars& descr = Chars() ) 
    : SimpleProperty<bool>(data, descr) {}
};

class IntProperty : public SimpleProperty<int> {
public:
  IntProperty( const int& data, const Chars& descr = Chars() ) 
    : SimpleProperty<int>(data, descr) {}
};


//class CharsProperty : public SimpleProperty<Chars> {
//public:
//  CharsProperty(const Chars& str) : SimpleProperty<>(str) {}
//  void readFrom(istream& istr); 
//  void writeTo(ostream& ostr) const;
//};

inline istream& operator>>( istream& istr, Word& w ) {
  error("Called a stub for: istream& operator>>(istream&,Word&)");
}           
//@dp just a stub. we don't use it.
 
class WordProperty : public SimpleProperty<Word> {
public:
  WordProperty( const Word& data, const Chars& descr = Chars() ) 
    : SimpleProperty<Word>(data,descr) {}
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};


inline istream& operator>>( istream& istr, PolyWord& w ) {
  error("Called a stub for: istream& operator>>(istream&,Word&)");
}
//@dp just a stub. we don't use it.

class PolyWordProperty : public SimpleProperty<PolyWord> {
public:
  PolyWordProperty( const PolyWord& data, const Chars& descr = Chars() ) 
    : SimpleProperty<PolyWord>(data,descr) {}
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class IntegerProperty : public SimpleProperty<Integer> {
public:
  IntegerProperty( const Integer& data, const Chars& descr = Chars() ) 
    : SimpleProperty<Integer>(data, descr) {}
};


class SMFPCheckinTypeProperty : public SimpleProperty<int> {
public:
  SMFPCheckinTypeProperty( const int& data, const Chars& descr = Chars() ) 
    : SimpleProperty<int>(data, descr) {}
};

class FPGroupProperty : public SimpleProperty<FPGroup> {
public:
  FPGroupProperty( const FPGroup& data, const Chars& descr = Chars() ) 
    : SimpleProperty<FPGroup>(data, descr) {} 
};

class FreeGroupProperty : public SimpleProperty<FreeGroup> {
public:
  FreeGroupProperty( const FreeGroup& data, const Chars& descr = Chars() ) 
    : SimpleProperty<FreeGroup>(data, descr) {}
};


class MSCGroupPtr 
{
public:

  MSCGroupPtr( const MSCGroupPtr& G );
  ~MSCGroupPtr();

  const MSCGroup& getMSCG() const;

private:

  friend class SimpleProperty<MSCGroupPtr>;
  friend class MSCGroupProperty;
  //friend class GIC;

  MSCGroupPtr( const FPGroup& G, int lmbda );
  MSCGroupPtr& operator= ( const MSCGroupPtr& G );

  friend istream& operator>>(istream& istr, MSCGroupPtr& G);
  friend ostream& operator<<(ostream& ostr, const MSCGroupPtr& G );
  // Data members:
  FPGroup fp;
  int lambda;
  MSCGroup *msc;
private:
  MSCGroupPtr();
};

class MSCGroupProperty : public SimpleProperty<MSCGroupPtr> {
public:
  MSCGroupProperty(const FPGroup& G, int lambda, const Chars& descr = Chars());
};

class FreeByCyclicProperty : public SimpleProperty<FreeByCyclic> {
public:
  FreeByCyclicProperty(const FreeByCyclic& data, const Chars& descr = Chars()) 
    : SimpleProperty<FreeByCyclic>(data, descr) {}
};

class AmalgProductOfFreeGroupsProperty :
  public SimpleProperty<AmalgProductOfFreeGroups> {
public:
  AmalgProductOfFreeGroupsProperty(const AmalgProductOfFreeGroups& data, 
				   const Chars& descr = Chars() ) 
    : SimpleProperty<AmalgProductOfFreeGroups>(data, descr) {}
};

class HNNExtOfFreeGroupProperty : public SimpleProperty<HNNExtOfFreeGroup> {
public:
  HNNExtOfFreeGroupProperty( const HNNExtOfFreeGroup& data, 
			     const Chars& descr = Chars() ) 
    : SimpleProperty<HNNExtOfFreeGroup>(data, descr) {}
};

class MapProperty : public SimpleProperty<Map> {
public:
  MapProperty( const Map& data, const Chars& descr = Chars() ) 
    : SimpleProperty<Map>(data, descr) {}
};

//============================================================
//
//          Complex properties:
//
//============================================================

/*
 *  // Template for complex base property:
 *

class $C$Property : public ComplexProperty< $C$ > {
public:
  $C$Property( const $C$& data, const Chars& descr = Chars() ) 
    : ComplexProperty< $C$ >(data, descr) {}
protected:
  void readFrom(istream& istr)       { istr > theData; }
  void writeTo (ostream& ostr) const { ostr < theData; }
};

 *
 */

//inline istream& operator>>( istream& istr, AbelianGroup& G ){}   //@dp imp!
//  error("Called a stub for: istream& operator>>(istream&,Word&)");
//}
//@dp just a stub. we don't use it.
// dummy stub.

class AbelianGroupProperty : public ComplexProperty< AbelianGroup > {
public:
  AbelianGroupProperty(const AbelianGroup& data, const Chars& descr = Chars()) 
    : ComplexProperty<AbelianGroup>(data, descr) {}
protected:
  void readFrom(istream& istr)       { istr > theData; }
  void writeTo (ostream& ostr) const { ostr < theData; }
};

class NilpotentGroupProperty : public ComplexProperty<NilpotentGroup> {
public:
  NilpotentGroupProperty( const NilpotentGroup& data, 
			  const Chars& descr = Chars() ) 
    : ComplexProperty<NilpotentGroup>(data, descr) {}
protected:
  void readFrom(istream& istr);
  void writeTo (ostream& ostr);
};

class NilpGroupAssocProperty : 
  public ComplexProperty< AssociationsOf<int,NilpotentGroup*> > 
{
public:
  NilpGroupAssocProperty( const AssociationsOf<int,NilpotentGroup*>& data, 
			  const Chars& descr = Chars() ) 
    : ComplexProperty< AssociationsOf<int,NilpotentGroup*> >(data, descr) {}

  NilpGroupAssocProperty( const NilpGroupAssocProperty& P );
  const NilpGroupAssocProperty& operator=( const NilpGroupAssocProperty& P );
  ~NilpGroupAssocProperty();

protected:
  void unbindAll();
  void copyAll( const NilpGroupAssocProperty& P );
  
  void readFrom(istream& istr);
  void writeTo(ostream& ostr) const ;
};


class SubgroupGraphProperty : public ComplexProperty<SubgroupGraph> {
public:
  SubgroupGraphProperty( const SubgroupGraph& data, 
			 const Chars& descr = Chars() ) 
    : ComplexProperty<SubgroupGraph>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class PermutationRepresentationProperty : 
  public ComplexProperty<PermutationRepresentation> {
public:
  PermutationRepresentationProperty( const PermutationRepresentation& data, 
				     const Chars& descr = Chars() ) 
    : ComplexProperty<PermutationRepresentation>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class GroupDFSAProperty : public ComplexProperty<GroupDFSA> {
public:
  GroupDFSAProperty( const GroupDFSA& data, const Chars& descr = Chars() ) 
    : ComplexProperty<GroupDFSA>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class DiffMachineProperty : public ComplexProperty<DiffMachine> {
public:
  DiffMachineProperty( const DiffMachine& data, const Chars& descr = Chars() )
    : ComplexProperty<DiffMachine>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

//@dp This property should be insvestigated more about data saved to and restored from
//    a disk storage. Now it stores itself in a wrong way.
///   And we don't want to store this property on the disk at all now.

class KBMachineProperty : public UnstorableProperty<KBMachine> {
public:
  KBMachineProperty( const KBMachine& data, const Chars& descr = Chars() )
    : UnstorableProperty<KBMachine>( data, descr ) {}
};

/*@dp The future implementation of KBMachineProperty may look like this:

class KBMachineProperty : public ComplexProperty<KBMachine> {
public:
  KBMachineProperty( const KBMachine& data, const Chars& descr = Chars() )
    : ComplexProperty<KBMachine>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};
*/

class DecomposeInSubgroupOfFPGroupProperty : 
  public ComplexProperty<DecomposeInSubgroupOfFPGroup> 
{
public:
  DecomposeInSubgroupOfFPGroupProperty( 
    const DecomposeInSubgroupOfFPGroup& data, const Chars& descr = Chars() 
  ) : ComplexProperty<DecomposeInSubgroupOfFPGroup>(data, descr) {}

protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class ListOfEndomorphismProperty : 
  public ComplexProperty< ListOf<Endomorphism> > 
{
public:
  ListOfEndomorphismProperty( const ListOf<Endomorphism> & data, 
			      const Chars& descr = Chars() ) 
    : ComplexProperty< ListOf<Endomorphism> >(data, descr) {}
protected:
  void readFrom(istream& istr);
  void writeTo (ostream& ostr) const;
};

class ListOfAutomorphismProperty : 
  public ComplexProperty< ListOf<Automorphism> > {
public:
  ListOfAutomorphismProperty( const ListOf<Automorphism> & data, 
			      const Chars& descr = Chars() ) 
    : ComplexProperty< ListOf<Automorphism> >(data, descr) {}
protected:
    void readFrom(istream& istr);
    void writeTo (ostream& ostr) const;
};

class AbelianSGPresentationProperty : 
  public ComplexProperty<AbelianSGPresentation> {
public:
  AbelianSGPresentationProperty( const AbelianSGPresentation& data, 
				 const Chars& descr = Chars() ) 
    : ComplexProperty<AbelianSGPresentation>(data, descr) {}
protected:
  void readFrom(istream& istr)      { istr > theData; }
  void writeTo(ostream& ostr) const { ostr < theData; }
};

class SGOfNilpotentGroupProperty : public ComplexProperty<SGOfNilpotentGroup> {
public:
  SGOfNilpotentGroupProperty( const SGOfNilpotentGroup& data, 
			      const Chars& descr = Chars() ) 
    : ComplexProperty<SGOfNilpotentGroup>(data, descr) {}

protected:
  void readFrom(istream& istr);
  void writeTo(ostream& ostr) const;
};

class SGNilpGroupAssocProperty : 
  public ComplexProperty< AssociationsOf<int,SGOfNilpotentGroup*> > 
{
public:
  SGNilpGroupAssocProperty(const AssociationsOf<int,SGOfNilpotentGroup*>& data,
			   const Chars& descr = Chars() ) 
    : ComplexProperty< AssociationsOf<int,SGOfNilpotentGroup*> >(data, descr){}

  SGNilpGroupAssocProperty( const SGNilpGroupAssocProperty& P );
  const SGNilpGroupAssocProperty& operator=( const SGNilpGroupAssocProperty& P );
  ~SGNilpGroupAssocProperty();

protected:
  void unbindAll();
  void copyAll( const SGNilpGroupAssocProperty& P );
  
  void readFrom(istream& istr);
  void writeTo(ostream& ostr) const ;
};

//============================================================
//
//          User defined properties:
//
//============================================================

/*
class <D>Property : public <B>Property {
public:
  <D>Property(const <B>& data, const Chars& descr) 
    : <B>Property(data, descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "<D>"; }
protected:
  GenericProperty* clone() const { return new <D>Property(*this); }
private:
  static const PropertyType theTypeID;
};
*/

#endif
