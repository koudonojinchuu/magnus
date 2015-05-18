// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMEquation
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMEQUATION_H_
#define _SMEQUATION_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "Word.h"
#include "QEqnSolutions.h"
#include "BaseProperties.h"
#include "InformationCenter.h"

class AllBasicSolutionsProperty : public NoDataProperty {
public:
  AllBasicSolutionsProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "AllBasicSolutions"; }
protected:
  GenericProperty* clone() const { 
    return new AllBasicSolutionsProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class AllRegStabGeneratorsProperty : public NoDataProperty {
public:
  AllRegStabGeneratorsProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "AllRegStabGenerators"; }
protected:
  GenericProperty* clone() const { 
    return new AllRegStabGeneratorsProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class BasicSolutionsProperty : public ListOfEndomorphismProperty {
public:
  BasicSolutionsProperty(const ListOf<Endomorphism>& data, 
			 const Chars& descr = "" )
    : ListOfEndomorphismProperty(data, descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "BasicSolutions"; }
protected:
  GenericProperty* clone() const { return new BasicSolutionsProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class RegStabGeneratorsProperty : public ListOfAutomorphismProperty {
public:
  RegStabGeneratorsProperty(const ListOf<Automorphism>& data, 
			    const Chars& descr = "" ) 
    : ListOfAutomorphismProperty(data, descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "RegStabGenerators"; }
protected:
  GenericProperty* clone() const { 
    return new RegStabGeneratorsProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

//---------------------------------------------------------------------------//
//------------------------------- EIC ---------------------------------------//
//---------------------------------------------------------------------------//

class EIC : public InformationCenter, protected FEData
{
public:
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  EIC(OID group_oid);

  ~EIC( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Reporting Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void putHaveAllBasicSolutions( bool found );

  void putHaveAllRegStabGenerators( bool found );

  void addBasicSolutions( const ListOf<Endomorphism>& basicSolutions );
  void addRegStabGenerators( const ListOf<Automorphism>& regStabGens );

  void putIsSolvedInAbelianization() {
    solvedInAbelian = yes;
  }
  
  void putHaveSolutionsInAbelianization( bool sol ) { 
    haveSolInAbelian = sol; 
  }  
  // void clearBasicSolutions( );
  // void clearRegStabGenerators( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessing Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isSolved() const { 
    return haveAllBasicSolutions() && haveAllRegStabGenerators(); 
  }

  bool haveAllBasicSolutions( ) const;

  bool haveAllRegStabGenerators( ) const;

  Trichotomy haveBasicSolutions() const;

  bool isSolvedInAbelianization() const { 
    return solvedInAbelian; 
  }
  
  bool haveSolutionsInAbelianization( ) const { 
    return haveSolInAbelian; 
  }

  ListOf<Endomorphism> getBasicSolutions( ) const;

  Trichotomy haveRegStabGenerators() const;
  
  ListOf<Automorphism> getRegStabGenerators( ) const;

private:

  EIC(const EIC&); // Disable copying

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID equationOID;
  // Need this for composing messages about what has been found.

  ListOf<Endomorphism> theBasicSolutions; // known basic solutions.
  ListOf<Automorphism> theRegStabGenerators; // known generators of RegStab.

  bool foundAllBasicSolutions;
  bool foundAllRegStabGenerators;
  
  bool solvedInAbelian;
  bool haveSolInAbelian;
};

//---------------------------------------------------------------------------//
//-------------------------- SMEquation -------------------------------------//
//---------------------------------------------------------------------------//

class SMEquation : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation( SMFPGroup& G, FreeGroup F, const Word w, int nvar, 
	      const Chars heritage);

  SMEquation(SMFPGroup& G) : AlgebraicObject(""), theGroup(G), eic( oid() ) { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getWord( ) const { return theWord; };

  SMFPGroup& getParent( ) const { return theGroup; };

  const FreeGroup& getFreeGroup( ) const { return theFreeGroup; };

  int numberOfVariables( ) const { return numOfVar; };

        InformationCenter* infoCenter()       { return &eic; }
  const InformationCenter* infoCenter() const { return &eic; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMEquation"; };

  const char* typeID( ) const { return type(); };        // overrides SMObject

  const IconID iconID( ) const { return IconID::equation; };
  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject


public:

  EIC eic; 
  // equation information center.

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { };                     // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  // The group that this equation is over.

  FreeGroup theFreeGroup;
  // The free group generated by theGroup's generators, along with the
  // variables.

  Word theWord;
  // A Word in theFreeGroup, represents the equation.

  int numOfVar;
  // a number of variables

};


//@db temporary class SMEquation2


//---------------------------------------------------------------------------//
//------------------------- SMEquation2 -------------------------------------//
//---------------------------------------------------------------------------//

class SMEquation2 : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation2( SMFPGroup& G, FreeGroup F, const Word w, int nvar, 
	       const Chars heritage);

  SMEquation2(SMFPGroup& G) : AlgebraicObject(""), theGroup(G), eic( oid() ) { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getWord( ) const { return theWord; };

  SMFPGroup& getParent( ) const { return theGroup; };

  const FreeGroup& getFreeGroup( ) const { return theFreeGroup; };

  int numberOfVariables( ) const { return numOfVar; };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMEquation2"; };

  const char* typeID( ) const { return type(); };        // overrides SMObject

  const IconID iconID( ) const { return IconID::equation; };
  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject


public:

  EIC eic; 
  // equation information center.

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { };                     // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  // The group that this equation is over.

  FreeGroup theFreeGroup;
  // The free group generated by theGroup's generators, along with the
  // variables.

  Word theWord;
  // A Word in theFreeGroup, represents the equation.

  int numOfVar;
  // a number of variables

};


#endif
