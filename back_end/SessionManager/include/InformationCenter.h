// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class InformationCenter
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

#ifndef _INFORMATION_CENTER_H_
#define _INFORMATION_CENTER_H_

#include "Property.h"

class InformationCenter {
public:
  InformationCenter() {}

  Trichotomy haveProperty ( const PropertyType ptype  ) const;
  void  putProperty       ( const GenericProperty& property, Trichotomy have );
  Chars getDescription    ( const PropertyType ptype  ) const;
  void  setUnknownProperty( const PropertyType ptype  );
  void  setHaveNotProperty( const PropertyType ptype, const Chars& expl );
  void  setKnownProperty  ( const GenericProperty& property );
  void  getKnownProperty  ( GenericProperty& property ) const;
  const GenericProperty* getKnownProperty( const PropertyType ptype ) const;
        GenericProperty* getKnownProperty( const PropertyType ptype );
  
  void read ( istream& istr, bool extraDetails );
  void write( ostream& ostr, bool extraDetails );

private:
  // Disable copying of Information Center.
  InformationCenter( const InformationCenter& );
  InformationCenter& operator=( const InformationCenter& );

  // Data members:
  PropertiesCollection theProperties;
};

inline 
Trichotomy InformationCenter::haveProperty ( const PropertyType ptype ) const
{
  return theProperties.haveProperty( ptype );
}

inline 
void InformationCenter::putProperty ( const GenericProperty& property,
					    Trichotomy       have )
{
  theProperties.putProperty( property, have );
}

inline
Chars InformationCenter::getDescription ( const PropertyType ptype ) const
{
  return theProperties.getDescription( ptype );
}

inline 
void InformationCenter::setUnknownProperty( const PropertyType ptype )
{
  theProperties.setUnknownProperty( ptype );
}

inline 
void InformationCenter::setHaveNotProperty( const PropertyType ptype, 
					    const Chars& expl )
{
  theProperties.setHaveNotProperty( ptype, expl );
}

inline
void InformationCenter::setKnownProperty  ( const GenericProperty& property )
{
  theProperties.setKnownProperty( property );
}

inline 
void InformationCenter::getKnownProperty  ( GenericProperty& property) const
{
  theProperties.getKnownProperty( property );
}

inline 
const GenericProperty* InformationCenter::getKnownProperty(
   const PropertyType ptype  ) const
{
  return theProperties.getKnownProperty(ptype);
}

inline 
GenericProperty* InformationCenter::getKnownProperty(const PropertyType ptype) 
{
  return theProperties.getKnownProperty(ptype);
}

inline 
void InformationCenter::read ( istream& istr, bool extraDetails )
{
  theProperties.read( istr, extraDetails );
}

inline 
void InformationCenter::write( ostream& ostr, bool extraDetails )
{
  theProperties.write( ostr, extraDetails );
}

#endif
