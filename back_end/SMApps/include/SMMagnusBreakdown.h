// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMMagnusBreakdown
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Description: 
//
//  SMMagnusBreakdown's object is responsible to send to front-end
//  some extra parameters on Magnus breakdown of one-relator group.
//  Then it wait for reply from FE with selected generator to be
//  stable letter. After that, it creates fast CM and destroys itself
//  by calling TheObjects::remove().
//

#ifndef _SM_MAGNUS_BREAKDOWN_H_
#define _SM_MAGNUS_BREAKDOWN_H_


#include "SMFPGroup.h"


//---------------------------------------------------------------------------//
//----------------------- SMMagnusBreakdown----------------------------------//
//---------------------------------------------------------------------------//

class SMMagnusBreakdown : public SMObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMMagnusBreakdown( const SMFPGroup& G );

  //  SMFPGroup( ) : AlgebraicObject( "" ), gic( oid() ), theGCM( 0 ) { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMMagnusBreakdown"; }

  const IconID iconID( ) const { return IconID::do_not_display; }  // overrides SMObject

  const char* typeID( ) const { return type(); }          // overrides SMObject

  void viewStructure(ostream& ostr) const {}              // overrides SMObject

  void printProperties(ostream& ostr) const {}            // overrides SMObject

  void printDefinition(ostream& ostr) const {}            // overrides SMObject

  bool displayInFE( ) const { return false; }             // overrides SMObject


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  enum TagMessage { ACCEPT };

  void readMessage(istream&);                          // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

private:

  const SMFPGroup& theGroup;
};

#endif
