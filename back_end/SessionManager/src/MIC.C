// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class MIC
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//

#include "MIC.h"
#include "OutMessages.h"


//------------------------------ MIC ----------------------------------------//


MIC::MIC(OID map_oid) :
  mapOID( map_oid )//,
  //extendToHom( dontknow ),
  //homIsMono( dontknow ),
  //homIsEpi( dontknow )
{ }


MIC::~MIC( ) { }

/*
void MIC::putDoesExtendToHom( bool doesExtend, Chars explanation )
{
  if( extendToHom != dontknow ) return;
  
  extendToHom = doesExtend;
  extendToHomExplanation = explanation;

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //
  
  msg << Name( mapOID );
  if ( !doesExtend )
    msg << " doesn't extend";
  else
    msg << " extends";

  msg << " to an homomorphism";

  if( extendToHomExplanation.length() > 0 )
    msg << ": " << extendToHomExplanation;
  msg << "." << ends;
  
  extendToHomMessage = msg.str();
  
  LogMessage(mapOID, extendToHomMessage).send();
}
*/

/////////////

Trichotomy MIC::doesExtendToHom( ) const
{ 
  return haveProperty( ExtendToHomProperty::type() );
  //return extendToHom; 
}

void MIC::putDoesExtendToHom( bool doesExtend, Chars explanation )
{
  if( doesExtendToHom() != dontknow ) return;
  putProperty( ExtendToHomProperty(explanation), doesExtend );
  LogMessage(mapOID, getExtendToHomMessage() ).send();
}


Chars MIC::getExtendToHomExplanation( ) const 
{ 
  return getDescription( ExtendToHomProperty::type() );
  //return extendToHomExplanation; 
}

Chars MIC::getExtendToHomMessage( ) const 
{ 
  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //
  
  msg << Name( mapOID );
  if ( doesExtendToHom() != yes )
    msg << " doesn't extend";
  else
    msg << " extends";

  msg << " to an homomorphism";

  if( getExtendToHomExplanation().length() > 0 )
    msg << ": " << getExtendToHomExplanation();
  msg << "." << ends;
  
  Chars extendToHomMessage = msg.str();
  msg.freeze(0);

  return extendToHomMessage; 
}

void MIC::putIsMono(bool ismono)
{
  putProperty( HomIsMonoProperty(), ismono );
  //homIsMono = ismono;
}

void MIC::putIsEpi(bool isepi)
{
  putProperty( HomIsEpiProperty(), isepi );
  //homIsEpi = isepi;
}

Trichotomy MIC::isMono() const 
{
  return haveProperty( HomIsMonoProperty::type() );
  //return homIsMono;
}

Trichotomy MIC::isEpi() const 
{
  return haveProperty( HomIsEpiProperty::type() );
  //return homIsEpi;
}

const PropertyType HomIsMonoProperty  :: theTypeID = GenericProperty::unique();
const PropertyType HomIsEpiProperty   :: theTypeID = GenericProperty::unique();
const PropertyType ExtendToHomProperty:: theTypeID = GenericProperty::unique();
