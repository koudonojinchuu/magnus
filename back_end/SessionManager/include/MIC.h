// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class MIC (Map Information Center)
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Usage:
//
// For a given piece of info, there are up to four kinds of names needed:
// 1) The data member
// 2) The member function through which it is reported
// 3) The member function via which its existence is queried
// 4) The member function via which it is accessed
// 
// In an attempt at coherence, we try to use these conventions for a
// Trichotomy (resp. bool) <I>
// e.g., I == abelian, (resp. I == solvedWordProblem):
// 
// 1) <I>
// 2) putIs<I>  resp. putHave<I>
// 3) is<I>     resp. have<I>
// 
// and in other cases (e.g., I == confluentKBMachine):
// 
// 1) <I>
// 2) putHave<I>
// 3) have<I>
// 4) get<I>
//
// Revision History:
//


#ifndef _MIC_H_
#define _MIC_H_

#include "Trichotomy.h"
#include "Chars.h"
#include "FEData.h"
#include "InformationCenter.h"

#include "BaseProperties.h"

// Types stored by the MIC:

class HomIsMonoProperty : public NoDataProperty {
public:
  HomIsMonoProperty( ) : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "HomIsMono"; }
protected:
  GenericProperty* clone() const { return new HomIsMonoProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class HomIsEpiProperty : public NoDataProperty {
public:
  HomIsEpiProperty( ) : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "HomIsEpi"; }
protected:
  GenericProperty* clone() const { return new HomIsEpiProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class ExtendToHomProperty : public NoDataProperty {
public:
  ExtendToHomProperty( const Chars& descr ) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "ExtendToHom"; }
protected:
  GenericProperty* clone() const { return new ExtendToHomProperty(*this); }
private:
  static const PropertyType theTypeID;
};

//-------------------------------- MIC --------------------------------------//


class MIC : public InformationCenter, protected FEData
{  
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  MIC( OID map_oid );

  ~MIC( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Extend to Homomorphism:                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void putDoesExtendToHom( bool doesExtend = true, Chars explanation = "" );
  
  Trichotomy doesExtendToHom( ) const;

  Chars getExtendToHomExplanation( ) const;

  Chars getExtendToHomMessage( ) const;

  void putIsMono(bool ismono);
  void putIsEpi(bool isepi);

  Trichotomy isMono() const;
  Trichotomy isEpi() const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID mapOID;
  // Need this for composing messages about what has been found.
  
  //Trichotomy extendToHom;
  //Chars extendToHomExplanation;
  //Chars extendToHomMessage;
private:
  //Trichotomy homIsMono;
  //Trichotomy homIsEpi;
};

#endif
