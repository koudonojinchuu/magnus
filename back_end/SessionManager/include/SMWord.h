// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMWord
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMWORD_H_
#define _SMWORD_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "Word.h"

#include "InformationCenter.h"
#include "BaseProperties.h"

//
// New properties of Word's Information Center (WIC)
// 

class IsTrivialProperty : public NoDataProperty {
public:
  IsTrivialProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsTrivial"; }
protected:
  GenericProperty* clone() const { return new IsTrivialProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class WordOrderProperty : public IntegerProperty {
public:
  WordOrderProperty(const Integer& data) : IntegerProperty(data) {}
  PropertyType actualType  () const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "WordOrder";   }
protected:
  GenericProperty* clone() const { return new WordOrderProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class MaximalRootProperty : public WordProperty {
public:
  MaximalRootProperty(const Word& data) : WordProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "MaximalRoot"; }
protected:
  GenericProperty* clone() const { return new MaximalRootProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class MaximalPowerProperty : public IntegerProperty {
public:
  MaximalPowerProperty(const Integer& data) : IntegerProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "MaximalPower"; }
protected:
  GenericProperty* clone() const { return new MaximalPowerProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class CollectedFormProperty : public PolyWordProperty {
public:
  CollectedFormProperty(const PolyWord& data) : PolyWordProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "CollectedForm"; }
protected:
  GenericProperty* clone() const { return new CollectedFormProperty(*this); }
private:
  static const PropertyType theTypeID;
};


// ------------------------------- WIC ------------------------------------- //


class WIC : public InformationCenter, protected FEData 
{  
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  WIC( OID word_oid );
  ~WIC( );

  /*
   *   Adapter for the IsTrivial property:
   */

  Trichotomy isTrivial() const;
  Trichotomy isTrivial(Chars& expl);
  // Yes if word is trivial, no if it is not.

  void putIsTrivial(bool istrivial, const Chars& expl);
  // Yes if word is trivial, no if it is not.

  /*
   *   Adapter for the CollectedForm property:
   */

  bool haveCollectedForm() const;
  // True if have cannonical decomposition in nilpotent

  void putHaveCollectedForm( const PolyWord& p );

  const PolyWord& getCollectedForm() const;

  /*
   *   Adapter for the WordOrder property:
   */

  bool haveOrder( ) const;
  void putHaveOrder (const Integer& o);
  Integer getOrder( ) const;

  /*
   *   Adapter for the MaximalRoot property:
   */

  bool haveMaximalRoot() const;
  // True if word has the maximal root

  void putHaveMaximalRoot(const Word& ,const Integer&);
  // Puts the solution of maximal root problem

  void getMaximalRoot(Word& , Integer& ) const;

  Trichotomy isPower() const;
  // True if maximal root problem, was solved

  Integer getMaximalExponent() const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID wordOID;
  // Need this for composing messages about what has been found.
};

// ------------------------------- WCM ------------------------------------- //


class WCM : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WCM( class SMWord& W );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMWord& getWord( )  { return theWord; }


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

  Subordinate<WCM,NGdecomposeWord> ngDecomposeWord;

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

  class SMWord& theWord;

  bool didFastChecks;
  // The takeControl method does some sneaky checks of obvious properties
  // (this should be formalized, since they are not always that fast).
  // This records the first call, so that they are not done over and over.

};

//---------------------------------------------------------------------------//
//---------------------------- SMWord ---------------------------------------//
//---------------------------------------------------------------------------//

class SMWord : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMWord(SMFPGroup& G, const Word w, const Chars heritage);

  SMWord(SMFPGroup& G) : AlgebraicObject(""), theGroup(G), 
    wic( oid() ),theWCM ( 0 )
    { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getWord( ) const { return theWord; }

  SMFPGroup& getParent( ) const { return theGroup; }

  WCM& wcm() const { return *theWCM; }

        InformationCenter* infoCenter()       { return &wic; }
  const InformationCenter* infoCenter() const { return &wic; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMWord"; }

  const char* typeID( ) const { return type(); }        // overrides SMObject

  const IconID iconID( ) const { return IconID::elt; }  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject

  WIC wic;

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { }                        // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;

  const Word theWord;
 
  WCM* theWCM;
};

#endif
