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

#include "Property.h"

//@njz: removed
//int SetData< QuickAssociation<PropertyType, Chars> >::hashElement(
//  const QuickAssociation<PropertyType, Chars>& t) const;
//
//int SetData< QuickAssociation<PropertyType, GenericProperty*> >::hashElement(
//  const QuickAssociation<PropertyType, GenericProperty*>& t) const;
//
//int SetData<QuickAssociation<Chars, PropertyType> >::hashElement(
// const QuickAssociation<Chars, PropertyType>& t) const;
template <> int SetData< QuickAssociation<PropertyType, Chars> >::hashElement(
  const QuickAssociation<PropertyType, Chars>& t) const;

template <> int SetData< QuickAssociation<PropertyType, GenericProperty*> >::hashElement(
  const QuickAssociation<PropertyType, GenericProperty*>& t) const;

template <> int SetData<QuickAssociation<Chars, PropertyType> >::hashElement(
  const QuickAssociation<Chars, PropertyType>& t) const;
//

//============================================================
//
//          Utilities
//
//============================================================

typedef bool (*ReadCriteria)(char c);

static 
bool isSpace(char c)
{
  static char spaces[] = " \n\t\r\f";
  return strchr(spaces,c);
}

static 
bool isLetter(char c) 
{
  return  c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

static 
Chars readLongLine( istream& istr, ReadCriteria reading, ReadCriteria skip )
{
  char c;
  while( skip( c = istr.peek()) ) {
    istr.get();
  }

  int buf_size = 16;
  char *buf = new char[buf_size];
  int pos = 0;
  
  while( reading( c = istr.peek()) ) {
    if( pos == buf_size-1 ) {
      const int new_size = 2 * buf_size;
      char*     new_buf  = new char[new_size];
      memcpy(new_buf, buf, pos);
      delete buf;
      buf      = new_buf;
      buf_size = new_size;
    }
    buf[pos++] = istr.get(); 
  }
  buf[pos] = 0;
  Chars result(buf);
  delete buf;
  return result;
}

static 
Chars IPC_safe_chars_read( istream& istr )
{
  int len;
  istr >> len;
  char dummy = istr.get(); // skip one blank symbol.
  char *buf = new char[len+1];
  istr.read(buf,len);
  buf[len] = '\0';
  Chars result = buf;
  delete buf;
  return result;
}

static 
void IPC_safe_chars_write( ostream& ostr, const Chars& s )
{
  ostr << s.length() << ' ' << s;
}


//============================================================
//
//          Properties Manager (Singleton)
//
//============================================================

PropertiesManager* PropertiesManager::instance()
{
  if( !theInstance ) {
    theInstance = new PropertiesManager();
  }
  return theInstance;
}

void PropertiesManager::registerProperty(const GenericProperty& property)
{
  theRegisteredProperties.bind( property.actualType(), property.clone() );
  theRegisteredTypes.bind( property.propertyName(), property.actualType() );
}

void PropertiesManager::unregisterProperty(const GenericProperty& property)
{
  unregisterProperty( property.actualType() );
}

void PropertiesManager::unregisterProperty(const PropertyType ptype)
{
  if( isRegisteredProperty(ptype) ) {
    GenericProperty *property = theRegisteredProperties[ptype];
    theRegisteredTypes.unbind( property->propertyName() );
    theRegisteredProperties.unbind( ptype );
    delete property;
  }
}

Chars PropertiesManager::nameOf( const PropertyType& ptype ) const
{
  if( isRegisteredProperty(ptype) )
    return theRegisteredProperties[ptype] -> propertyName();
  return Chars();
}

PropertyType PropertiesManager::typeOf( const Chars& pname ) const
{
  if( theRegisteredTypes.bound(pname) )
    return theRegisteredTypes[pname];
  return unregisteredProperty;
}

bool PropertiesManager::isRegisteredProperty( const PropertyType ptype ) const
{
  return theRegisteredProperties.bound(ptype);
}

GenericProperty* PropertiesManager::readProperty(
  istream& istr, bool extraDetails)
{
  Chars pname = readLongLine( istr, isLetter, isSpace );
  return readProperty(istr, typeOf(pname), extraDetails);
}


GenericProperty* PropertiesManager::readProperty(
  istream& istr, const PropertyType ptype, bool extraDetails )
{
  if( !isRegisteredProperty(ptype) ) 
    return 0;
  GenericProperty *p = theRegisteredProperties[ptype];
  p->read(istr, extraDetails);
  return p->clone();
}

//============================================================
//
//          Properties Collection
//
//============================================================

Trichotomy PropertiesCollection::haveProperty( const PropertyType ptype ) const
{
  if( knownProperties.bound(ptype) )
    return yes;
  if( haveNotProperties.bound(ptype) )
    return no;
  return dontknow;
}

void PropertiesCollection::putProperty ( const GenericProperty& property,
					       Trichotomy       have )
{
  if( have == yes )
    setKnownProperty( property );
  else if( have == no )
    setHaveNotProperty( property.actualType(), property.getDescription() );
  else
    setUnknownProperty( property.actualType() );
}

Chars PropertiesCollection::getDescription( const PropertyType ptype ) const
{
  if( knownProperties.bound(ptype) ) {
    return getKnownProperty(ptype) -> getDescription();
  }

  if( haveNotProperties.bound(ptype) ){
    return haveNotProperties.valueOf(ptype); 
  }
  
  return Chars("The property is unknown.");
}

void PropertiesCollection::setUnknownProperty( const PropertyType ptype )
{
  if( knownProperties.bound(ptype) ) {
    GenericProperty *ptr = knownProperties[ptype];
    knownProperties.unbind(ptype);
    delete ptr;
  }

  if( haveNotProperties.bound(ptype) )
    haveNotProperties.unbind(ptype);
}

void PropertiesCollection::setHaveNotProperty( const PropertyType ptype,
					       const Chars& expl )
{
  haveNotProperties.bind(ptype, expl);
  knownProperties.unbind(ptype);
}

void PropertiesCollection::setKnownProperty( const GenericProperty& property )
{
  PropertyType ptype = property.actualType();

  if( haveNotProperties.bound(ptype) )
    haveNotProperties.unbind(ptype);

  if( knownProperties.bound(ptype) ) {
    // remove an old binding.
    GenericProperty *ptr = knownProperties[ptype];
    knownProperties.unbind(ptype);
    delete ptr;
  }

  knownProperties.bind( ptype, property.clone() );
}

void PropertiesCollection::getKnownProperty( GenericProperty& property ) const
{
  PropertyType ptype = property.actualType();

  if( !knownProperties.bound(ptype) ) {
    error( Chars("PropertiesCollection::getKnownProperty(GenericProperty&):"
		 "the collection has no the property '") 
	   + property.propertyName() + "'." );
  }

  property.putProperty( knownProperties[ptype] );
}


const GenericProperty* PropertiesCollection::getKnownProperty( 
  const PropertyType ptype ) const
{
  if( !knownProperties.bound(ptype) ) {
    error( Chars("GenericProperty* PropertiesCollection::getKnownProperty() "
		 "const: the collection has no the property '") 
	   + PropertiesManager::instance()->nameOf(ptype) + "'." );
  }

  return knownProperties[ptype];
}

GenericProperty* PropertiesCollection::getKnownProperty(const PropertyType pt) 
{
  if( !knownProperties.bound(pt) ) {
    error( Chars("GenericProperty* PropertiesCollection::getKnownProperty():"
		 "the collection has no the property '") 
	   + PropertiesManager::instance()->nameOf(pt) + "'." );
  }

  return knownProperties[pt];
}

void PropertiesCollection::read( istream& istr, bool extraDetails ) 
{
  PropertiesParser parser(istr);

  Chars errMesg = parser.parsePropertiesCollection( knownProperties, 
						    haveNotProperties, 
						    extraDetails);
  if( errMesg.length() > 0 )
    error(errMesg);
}

void PropertiesCollection::write( ostream& ostr, bool extraDetails )
{
  Chars secondaryFileName = File().getFileName();

  ostr << "{ { ";

  QuickAssociationsIterator<PropertyType,GenericProperty*> I(knownProperties);
  for( I.reset(); !I.done(); I.next() ) {
    GenericProperty *property = I.value();
    if( property->isStorable() ) {
      ostr << ' ' << property->propertyName() << ' ';
      property->write( ostr, extraDetails, secondaryFileName );
    }
  }

  ostr << " } {";

  QuickAssociationsIterator<PropertyType,Chars> J(haveNotProperties);
  for( ; !J.done(); J.next() ) {
    ostr << ' ' << PropertiesManager::instance() -> nameOf( J.key() )
	 << ' ' << encodeSpaces(J.value());
  }
  
  ostr << " } } ";
}

//============================================================
//
//          Properties Parser
//
//============================================================

static void skipWhiteSpaces( istream& istr ) 
{
  while( isSpace( istr.peek()) )
    istr.get();
}

bool skipWhiteSpacesAndGivenChar( istream& istr, char c )
{
  skipWhiteSpaces(istr);
  if( istr.peek() != c ) 
    return false;
  istr.get();
  return true;
}

bool PropertiesParser::skipWhiteSpaces()
{
  ::skipWhiteSpaces( istr );
}

bool PropertiesParser::skipWhiteSpacesAndGivenChar(char c)
{
  ::skipWhiteSpacesAndGivenChar( istr, c );
}

Chars PropertiesParser::parseRawWord(Word& w)
{
  ListOf<Generator> wrd;
  w = Word();

  if( !skipWhiteSpacesAndGivenChar('[') )
    return "error";
  
  while( 1 ) {
    skipWhiteSpaces();

    if( peekCh() == ']' ) {
      getCh();
      break;
    }

    int g;
    istr >> g;
    wrd.append( Generator(g) );
  }

  w = Word(wrd);
  return Chars();
}

Chars PropertiesParser::parsePropertiesCollection(
  QuickAssociationsOf<PropertyType,GenericProperty*>& known,
  QuickAssociationsOf<PropertyType,Chars>&            unknown, 
  bool extraDetails )
{
  known   = QuickAssociationsOf<PropertyType, GenericProperty*>();
  unknown = QuickAssociationsOf<PropertyType, Chars>();

  if( !skipWhiteSpacesAndGivenChar('{') ) {
    return "PropertiesParser::parsePropertiesCollection(): "
           "no a left curly brace found.";
  }

  Chars errMesg = parseKnownProperties(known, extraDetails);
  if( errMesg.length() > 0 )
    return errMesg;

  errMesg = parseUnknownProperties(unknown);
  if( errMesg.length() > 0 )
    return errMesg;

  if( !skipWhiteSpacesAndGivenChar('}') ) {
    return "PropertiesParser::parsePropertiesCollection(): "
           "no a rightcurly brace found.";
  }

}

Chars PropertiesParser::parseKnownProperties(
  QuickAssociationsOf<PropertyType,GenericProperty*>& known, 
  bool extraDetails )
{
  known = QuickAssociationsOf<PropertyType,GenericProperty*>();

  
  if( !skipWhiteSpacesAndGivenChar('{') ) { 
    return "error in PropertiesParser::parseKnownProperties()";
  }

  Chars errMesg;

  while( 1 ) {
    skipWhiteSpaces();

    if( peekCh() == '}' ) {
      getCh();
      break;
    }

    GenericProperty* ptr = 
      PropertiesManager::instance() -> readProperty(istr, extraDetails);

    if( ptr ) {
      known.bind( ptr->actualType(), ptr );
    }
    else {
      errMesg = 
	"Unregistered property is detected in "
	"void PropertiesParser::parseKnownProperties"
	"( QuickAssociationsOf<PropertyType,GenericProperty*>& known )";
    }
  }

  return errMesg;
}

Chars PropertiesParser::parseUnknownProperties( 
  QuickAssociationsOf<PropertyType,Chars>& unknown )
{
  unknown = QuickAssociationsOf<PropertyType, Chars>();

  if( !skipWhiteSpacesAndGivenChar('{') ) {
    return "error in PropertiesParser::parseUnknownProperties()";
  }

  Chars errMesg;

  while( 1 ) {
    skipWhiteSpaces();

    if( peekCh() == '}' ) {
      getCh();
      break;
    }
    
    Chars pname, descr;
    istr >> pname >> descr;
 
    PropertyType ptype = PropertiesManager::instance() -> typeOf(pname);
    
    if( ptype != unregisteredProperty ) {
      descr = decodeSpaces(descr);
      unknown.bind( ptype, descr );
    } 
    else {
      errMesg = 
	"Unregistered property is detected in "
	"void PropertiesParser::parseUnknownProperties(SetOf<PropertyType>&)";
    }
  }

  return errMesg;
}

//============================================================
//
//          Properties I/O routines
//
//============================================================


void GenericProperty::readFrom(istream& istr)
{
  Chars descr;
  istr >> descr;
  theDescription = decodeSpaces(descr);
}

void GenericProperty::writeTo(ostream& ostr) const
{
  ostr << encodeSpaces( theDescription );
}

PropertyType GenericProperty::unique()
{ 
  static int typeCounter = 0;

  return ++typeCounter; 
}

void GenericSimpleProperty::read(istream& istr, bool extraDetails) 
{ 
  int dummyDetails;
  skipWhiteSpacesAndGivenChar(istr, '{');
  GenericProperty::readFrom(istr);
  istr >> dummyDetails;
  readFrom(istr); 
  skipWhiteSpacesAndGivenChar(istr, '}');
}

void GenericSimpleProperty::write( ostream& ostr, bool extraDetails, 
				   const Chars& /*unused*/ ) const 
{
  ostr << "{ ";
  GenericProperty::writeTo(ostr);
  ostr << " 0 "; 
  writeTo(ostr);
  ostr << " }";
}

void GenericComplexProperty::read(istream& istr, bool extraDetails)
{
  PropertiesParser parser(istr);
  parser.skipWhiteSpacesAndGivenChar( '{' );
  GenericProperty::readFrom(istr);
  
  int writtenDetails;
  istr >> writtenDetails;

  if( writtenDetails ) {
    parser.skipWhiteSpaces();

    Chars fileName;
    int start, length;
    istr >> fileName >> start >> length;

    if( extraDetails ) {
      
      ifstream file(fileName);

      if( file ) {
	char *buf = new char[length];
	file.seekg( start );
	file.read(buf, length);
	//@njz
	//	istrstream i(buf, length);
	std::istrstream i(buf, length);
	//

	Chars property_name;
	i >> property_name;

#if SAFETY > 0
	if( propertyName() != property_name ) {
	  error(Chars(" Cannot load complex property ") + propertyName() 
		+ ": wrong data type in secondary database file." );
	}
#endif

	readFrom(i);

	delete buf;
      }
    }
  }
  parser.skipWhiteSpacesAndGivenChar( '}' );
}

void GenericComplexProperty::write( ostream& ostr, bool extraDetails, 
				    const Chars& extraDetailsFileName ) const
{
  int start, length;

  if( extraDetails ) {    
    ofstream file(extraDetailsFileName, ios::app);
    start = file.tellp();
    file << propertyName() << ' ';
    writeTo(file);
    //@njz
    //    length = file.tellp() - start;     
    length = (int)file.tellp() - start;     
    //
  }

  ostr << "{ ";
  GenericProperty::writeTo(ostr);
  ostr << ' ' << extraDetails;
  if( extraDetails )
    ostr << ' ' << extraDetailsFileName << ' ' << start << ' ' << length;
  ostr << " }";
}

/*
void GenericComplexProperty::completeHeader( 
  char *header, const char *title, const char *name, int start, int length )
{
  sprintf( header, "%s: name = %s start = %d length = %d", 
	   title, name, start, length
	   );
  const int len = strlen(header);
  if( len > header_size ) 
    error("Header too long in GenericComplexProperty::completeHeader");

  for( int i = len; i < header_size-1; ++i )
    header[i] = ' ';
  header[header_size-1] = '\n';
}
*/

//============================================================
//
// Static data 
//
//============================================================

const PropertyType unregisteredProperty = -1;

PropertiesManager*                  PropertiesManager::theInstance = 0;
PropertiesManager::SingletonKiller  PropertiesManager::theKiller;

//PropertyType GenericProperty::theTypeCounter  =  0;
const int GenericComplexProperty::header_size = 80;

/*
const PropertyType Property::theTypeID = GenericProperty::unique();
*/

template <> int SetData<PropertyType>::hashElement(PropertyType const &t) const
{
  return abs(t.unwrap());
}

template <> int SetData< QuickAssociation<PropertyType, Chars> >::hashElement(
  const QuickAssociation<PropertyType, Chars>& t) const
{
  return t.hash();
}

template <> int SetData< QuickAssociation<PropertyType, GenericProperty*> >::hashElement(
  const QuickAssociation<PropertyType, GenericProperty*>& t) const
{
  return t.hash();
}

template <> int SetData<QuickAssociation<Chars, PropertyType> >::hashElement(
  const QuickAssociation<Chars, PropertyType>& t) const
{ 
  return t.hash();
}
