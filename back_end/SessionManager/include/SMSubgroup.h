// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of classes SMSubgroup, SIC, SCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMSubgroup_H_
#define _SMSubgroup_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "SGofFreeGroup.h"
#include "NGSubgroupProblems.h"
#include "AbelianSGPresentation.h"
#include "AbelianProblems.h"
#include "ToddCoxeter.h"
#include "HToddCoxeter.h"
#include "SGNilpotentQuotients.h"


class CyclicDecompositionOfFactorProperty : public AbelianGroupProperty {
public:
  CyclicDecompositionOfFactorProperty( const AbelianGroup& data )
    : AbelianGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "CyclicDecompositionOfFactor"; }
protected:
  GenericProperty* clone() const { 
    return new CyclicDecompositionOfFactorProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class AbelianSubgroupPresentationProperty 
  : public AbelianSGPresentationProperty {
public:
  AbelianSubgroupPresentationProperty(const AbelianSGPresentation& data)
    : AbelianSGPresentationProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "AbelianSubgroupPresentation"; }
protected:
  GenericProperty* clone() const { 
    return new AbelianSubgroupPresentationProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class IsPureSubgroupProperty : public NoDataProperty {
public:
  IsPureSubgroupProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsPureSubgroup"; }
protected:
  GenericProperty* clone() const { return new IsPureSubgroupProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class IsCentralSubgroupProperty : public NoDataProperty {
public:
  IsCentralSubgroupProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsCentralSubgroup"; }
protected:
  GenericProperty* clone() const { 
    return new IsCentralSubgroupProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class IsNormalSubgroupProperty : public NoDataProperty {
public:
  IsNormalSubgroupProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsNormalSubgroup"; }
protected:
  GenericProperty* clone() const { 
    return new IsNormalSubgroupProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class IsAbelianSubgroupProperty : public NoDataProperty {
public:
  IsAbelianSubgroupProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsAbelianSubgroup"; }
protected:
  GenericProperty* clone() const { return new IsAbelianSubgroupProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class IsTrivialSubgroupProperty : public NoDataProperty {
public:
  IsTrivialSubgroupProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsTrivialSubgroup"; }
protected:
  GenericProperty* clone() const { 
    return new IsTrivialSubgroupProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class IndexOfSubgroupProperty : public IntProperty {
public:
  IndexOfSubgroupProperty(const int& data) : IntProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IndexOfSubgroup"; }
protected:
  GenericProperty* clone() const { return new IndexOfSubgroupProperty(*this);}
private:
  static const PropertyType theTypeID;
};

class SGNilpotentQuotientsProperty : public SGNilpGroupAssocProperty {
public:
  SGNilpotentQuotientsProperty(
    const AssociationsOf<int,SGOfNilpotentGroup*>& data
    ) : SGNilpGroupAssocProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "SGNilpotentQuotients"; }
protected:
  GenericProperty* clone() const { 
    return new SGNilpotentQuotientsProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class SubgroupOfNilpotentGroupProperty : public SGOfNilpotentGroupProperty {
public:
  SubgroupOfNilpotentGroupProperty( const SGOfNilpotentGroup& data )
    : SGOfNilpotentGroupProperty( data ) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "SubgroupOfNilpotentGroup"; }
protected:
  GenericProperty* clone() const { 
    return new SubgroupOfNilpotentGroupProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};


// ------------------------------ SIC ------------------------------------- //


class SIC : public InformationCenter, protected FEData
{  
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SIC( OID subgroup_oid );

  ~SIC( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Abelianization of the factor group                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void putHaveCyclicDecompositionOfFactor( const AbelianGroup& );

  bool haveCyclicDecompositionOfFactor( ) const;

  const AbelianGroup& getCyclicDecompositionOfFactor( ) const;

  void putHaveCyclicDecomposition( const AbelianSGPresentation& );

  void putHavePrimaryDecomposition( const AbelianSGPresentation& );

  bool haveCyclicDecomposition( ) const;

  bool havePrimaryDecomposition( ) const;

  const AbelianSGPresentation& getSGPresentation( ) const;

  // Methods for nilpotent subgroups
  void putHaveSGOfNGPreimageInitialized( const SGOfNilpotentGroup& );

  bool haveSGOfNGPreimageInitialized( ) const;

  void putHaveSGOfNGinitialized( const SGOfNilpotentGroup& );

  bool haveSGOfNGinitialized( ) const;

  const SGOfNilpotentGroup& getSGOfNilpotentGroup( ) const;

  // Methods for subgroup nilpotent quotients

  void putHaveSGNilpotentQuotInited( const SGOfNilpotentGroup& theQuot, 
				     int theClass );

  bool haveSGNilpotentQuotInited(int theClass) const;

  const SGOfNilpotentGroup& getSGNilpotentQuotInited(int theClass) const;



  void putHaveCompleteCayleyGraph( const SubgroupGraph& S );

  bool haveCompleteCayleyGraph( ) const;

  const SubgroupGraph& getCompleteCayleyGraph( ) const;

  // Methods for word decomposer.

  void putHaveWordDecomposer( const DecomposeInSubgroupOfFPGroup& D );

  bool haveWordDecomposer( ) const;

  const DecomposeInSubgroupOfFPGroup& getWordDecomposer( ) const;

  //////////////////////////////////////////////////////////
  //
  // Schreier Transversal
  //
  /////////////////////////////////////////////////////////

  void putHaveSchreierTransvl(const PermutationRepresentation&);
  
  bool haveSchreierTransvl()const;
  
  const PermutationRepresentation& getSchreierTransvl()const;
  
  Trichotomy isPure() const;
  void putIsPure(bool ispure);

  Trichotomy isCentral() const;
  void putIsCentral( bool iscentral);

  Trichotomy isNormal() const;
  void putIsNormal( bool isnormal);

  Trichotomy isAbelian() const;
  void putIsAbelian( bool isabelian);

  Trichotomy isTrivial() const;
  void putIsTrivial( bool istrivial);

  int index() const;
  bool haveIndex() const;
  void putIndex(const int& index);
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
private:
  OID subgroupOID;
  // Need this for composing messages about what has been found.

  AssociationsOf<int,SGOfNilpotentGroup*>& getSGNilpotentQuotients() const;

  //AbelianGroup* theAbelianGroup;
  //AbelianSGPresentation* theAbelianSGPresentation;
  //SGOfNilpotentGroup* theSGOfNilpotentGroup;
  //Trichotomy isPureSubgroup;
  //Trichotomy isCentralSubgroup;
  //Trichotomy isNormalSubgroup;
  //Trichotomy isAbelianSubgroup;
  //Trichotomy isTrivialSubgroup;
  //int theIndex;
  //PermutationRepresentation* permutationRepresentation;

  //AssociationsOf<int, SGOfNilpotentGroup*> theSGNilpotentQuotients;
  //SubgroupGraph *theCompleteCayleyGraph;
  //DecomposeInSubgroupOfFPGroup *theWordDecomposer;
};


// ------------------------------- SCM ------------------------------------- //


class SCM : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SCM( class SMSubgroup& S );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& getSubgroup( ) const { return theSubgroup; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<SCM,AbelianInvariantsOfFactor> abelianInvariantsOfFactor;
  Subordinate<SCM,AbelianSGInvariants> abelianSGInvariants;
  Subordinate<SCM,SGOfNGinitializeProblem> initializeSGOfNG;
  Subordinate<SCM,SGOfNGinitPreimageProblem> initPreimageSGOfNG;
  Subordinate<SCM,SGIndexToddCoxeter> sgToddCoxeter;
  Subordinate<SCM,SGIndexToddCoxeter> sgGHToddCoxeter;

  Subordinate<SCM,SGNilpotentQuotients> sgNilpotentQuotients;
  Subordinate<SCM,NormalClosure> normalClosure;
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );        // overrides ComputationManager


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMSubgroup& theSubgroup;

  bool didFastChecks;
  // The takeControl method does some sneaky checks of obvious properties
  // (this should be formalized, since they are not always that fast).
  // This records the first call, so that they are not done over and over.

};


// ----------------------------- SMSubgroup -------------------------------- //

class SMSubgroup : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup(SMFPGroup& G, const SGofFreeGroup& S, const Chars heritage,
	    Chars def = Chars());

  SMSubgroup(SMFPGroup& G)
    : AlgebraicObject(""), theGroup(G), theSubgroup( *((SGofFreeGroup*)0) ),
      sic( oid() ), theSCM( 0 )
  { }
  //Bogus ctor; see `But no' comment in Menu.h

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGofFreeGroup getSubgroup( ) const { return theSubgroup; }

  SMFPGroup& getParent( ) const { return theGroup; }

  SCM& scm() const { return *theSCM; }


        InformationCenter* infoCenter()       { return &sic; }
  const InformationCenter* infoCenter() const { return &sic; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMSubgroup"; }

  const char* typeID( ) const { return type(); }         // overrides SMObject

  const IconID iconID( ) const { return IconID::subgroup; }
  // overrides SMObject

  void viewStructure(ostream& ostr) const;               // overrides SMObject

  void printProperties(ostream& ostr) const;             // overrides SMObject

  void printDefinition(ostream& ostr) const;             // overrides SMObject


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { }                         // overrides SMObject


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

public:

  SIC sic;

private:

  const SGofFreeGroup theSubgroup;

  SMFPGroup& theGroup;

  SCM* theSCM;  // TheObjects removes scm when this is removed.

  Chars definition;
  // This is a definition of the subgroup. If the string is empty, then
  // printDefinition outputs some "standard" definition:
  //          gp(<list-of-generators>)
  //
  // The alternative definition can be put in the constructor.
};

#endif
