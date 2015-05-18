// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of classes SMHomomorphism, SMHomomorphism2 
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMHOMOMORPHISM_H_
#define _SMHOMOMORPHISM_H_


#include "SMMap.h"


//--------------------------- SMHomomorphism --------------------------------//


class SMHomomorphism : public SMMap
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMHomomorphism( const class SMMap& map, const Chars& heritage ) : 
    SMMap( map.getDomain(), map.getRange(), map.getMap(), heritage ) { }

  SMHomomorphism( SMFPGroup& G, Map m, const Chars heritage) :
    SMMap( G, m, heritage ) { }

  SMHomomorphism( SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage) :
    SMMap( G, H, m, heritage ) { }

/*
  SMMap(SMFPGroup& G, Map m, const Chars heritage);

  SMMap(SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage);

  SMMap(SMFPGroup& G)
  : AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( G ) 
    { }
  SMMap(SMFPGroup& G, SMFPGroup& H)
  : AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( H ) 
    { }
  //@rn Bogus ctors; see `But no' comment in Menu.h
*/
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
/*
  Map getMap( ) const { return theMap; }

  SMFPGroup& getDomain( ) const { return theDomain; }

  SMFPGroup& getRange( ) const { return theRange; }
*/
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMHomomorphism"; }

  const char* typeID( ) const { return type(); }        // overrides SMObject

  const IconID iconID( ) const { return IconID::homomorphism; }  
  // overrides SMObject
/*
  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject
*/

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
/*
  const Map theMap;

  SMFPGroup& theDomain;

  SMFPGroup& theRange;
*/
};


//--------------------------- SMHomomorphism2 -------------------------------//


class SMHomomorphism2 : public SMHomomorphism
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMHomomorphism2( const class SMMap& map, const Chars& heritage ) : 
    SMHomomorphism( map, heritage ) { }

  SMHomomorphism2( SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage) :
    SMHomomorphism( G, H, m, heritage ) { }

/*
  SMMap(SMFPGroup& G, Map m, const Chars heritage);

  SMMap(SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage);

  SMMap(SMFPGroup& G)
  : AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( G ) 
    { }
  SMMap(SMFPGroup& G, SMFPGroup& H)
  : AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( H ) 
    { }
  //@rn Bogus ctors; see `But no' comment in Menu.h
*/
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
/*
  Map getMap( ) const { return theMap; }

  SMFPGroup& getDomain( ) const { return theDomain; }

  SMFPGroup& getRange( ) const { return theRange; }
*/
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMHomomorphism2"; }

  const char* typeID( ) const { return type(); }        // overrides SMObject
/*
  const IconID iconID( ) const { return IconID::homomorphism2; }  
  // overrides SMObject
*/
/*
  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject
*/

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

//  void readMessage(istream&) { }                        // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
/*
  const Map theMap;

  SMFPGroup& theDomain;

  SMFPGroup& theRange;
*/
};

#endif
