//   Copyright (C) 1998 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of an object property template 
//           and collection of properties.
//
// Principal Author: Dmitry Pechkin
//
// Status: In progress.
//
// Revision History:
//
//
// Special Notes:
//
//  See examples of use at the bottom of the file.
//
// Further implementation steps:
//
//
// Bugs:
//
//

#ifndef _PROPERTY_H_
#define _PROPERTY_H_
 
#include "QuickAssociations.h"
#include "Chars.h"
#include "APofFreeGroups.h"
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

class PropertyType {
public:
  PropertyType( int t ) : type(t) {}
  int unwrap() const { return type; }
  int operator++() { return ++type; }

  //bool operator ==( const PropertyType& p ) { return type == p.type; }
  //bool operator !=( const PropertyType& p ) { return type != p.type; }
  
  friend ostream& operator<<(ostream& ostr, const PropertyType& p) { 
    ostr << p.type; 
  }
  friend istream& operator>>(istream& istr, PropertyType& p) { 
    istr >> p.type; 
  }

private:
  int type;
};

inline bool operator==( const PropertyType a, const PropertyType b ) {
  return a.unwrap() == b.unwrap();
}

inline bool operator!=( const PropertyType a, const PropertyType b ) {
  return a.unwrap() != b.unwrap();
}

class GenericProperty;

//============================================================
//
//          Properties Manager
//
//============================================================

class PropertiesManager {
  // The class implements a `Singleton' design pattern (see the G4's book)
public:
  static PropertiesManager* instance();
  
  PropertyType typeOf( const Chars& propertyName ) const;
  Chars nameOf( const PropertyType& ptype ) const;

  const GenericProperty* property( const PropertyType& ptype ) const;

  bool isRegisteredProperty( const PropertyType ptype ) const;
  void registerProperty( const GenericProperty& property );
  void unregisterProperty( const GenericProperty& property );
  void unregisterProperty( const PropertyType ptype );

  GenericProperty* readProperty(istream& istr, bool extraDetails);
  GenericProperty* readProperty(istream& istr, const PropertyType ptype,
				bool extraDetails);

private:
  PropertiesManager() {}

  // Disable the copying of the instance -- 
  // these two functions are not to be implemented.
  PropertiesManager( const PropertiesManager& );
  PropertiesManager& operator=( const PropertiesManager& );

  class SingletonKiller {
  public:
    ~SingletonKiller() { delete PropertiesManager::theInstance; }
  };
  friend class SingletonKiller;

  static PropertiesManager *theInstance;
  static SingletonKiller    theKiller;

  // Instance's data members:
  QuickAssociationsOf<PropertyType, GenericProperty*> theRegisteredProperties;
  QuickAssociationsOf<Chars, PropertyType>            theRegisteredTypes;
};

void registerProperties();

extern const PropertyType unregisteredProperty;

//============================================================
//
//          Properties Collection
//
//============================================================

class PropertiesCollection
{
public:
  PropertiesCollection( ) { }

  Trichotomy haveProperty  ( const PropertyType ptype  ) const;
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

  QuickAssociationsOf<PropertyType, GenericProperty*> knownProperties;
  QuickAssociationsOf<PropertyType, Chars>            haveNotProperties;
};

//============================================================
//
//          Properties Parser
//
//============================================================

class PropertiesParser //: protected PresentationParser
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PropertiesParser(istream &i) : istr(i) {} 
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Parsing Methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars parsePropertiesCollection(
    QuickAssociationsOf<PropertyType, GenericProperty*>& known,
    QuickAssociationsOf<PropertyType, Chars>&            unknown,
    bool extraDetails );

  Chars parseRawWord(Word& w);

  bool skipWhiteSpaces();
  bool skipWhiteSpacesAndGivenChar(char c);

private:
  Chars parseKnownProperties(
    QuickAssociationsOf<PropertyType,GenericProperty*>& known,
    bool extraDetails );

  Chars parseUnknownProperties( 
    QuickAssociationsOf<PropertyType,Chars>& unknown );

  char peekCh() { return istr.peek(); }
  char getCh()  { return istr.get();  }

  istream& istr;
};

//============================================================
//
//          Generic Properties
//
//============================================================

class GenericProperty {
public:
  GenericProperty( const Chars& descr ) : theDescription( descr ) {}

  virtual ~GenericProperty() {}
  virtual PropertyType actualType  () const = 0;
  virtual Chars        propertyName() const = 0;
  virtual void         putProperty( GenericProperty* property ) = 0;

  Chars getDescription() const { return theDescription; }

  virtual bool isStorable() const { return true; }

  // I/O routines
  friend  istream& operator>>( istream& istr, GenericProperty& p ) {
    p.read(istr);
    return istr;
  }

  friend ostream& operator<<( ostream& ostr, const GenericProperty& p ) {
    Chars noExtraFile;
    p.write(ostr, false, noExtraFile);
    return ostr;
  }

  virtual void read (istream& istr, bool extraDetails = false)  = 0;
  virtual void write(ostream& ostr, bool extraDetails, 
		     const Chars& extrasDetailsFileName ) const = 0;

protected:
  friend class PropertiesCollection;
  friend class PropertiesManager;
  virtual GenericProperty* clone() const = 0;

protected:
  virtual void readFrom(istream& istr);
  virtual void writeTo (ostream& ostr) const;

  static PropertyType unique();
private:
  static PropertyType theTypeCounter;

  Chars theDescription;
};

class GenericUnstorableProperty : public GenericProperty {
public:
  GenericUnstorableProperty( const Chars& descr ) : GenericProperty(descr) {}

  bool isStorable() const { return false; }
protected:
  void read (istream& istr, bool extraDetails = false) {
    error( "Cannot load an unstorable propery.");
  }
  void write(ostream& ostr, bool extraDetails, 
	     const Chars& extrasDetailsFileName) const {
    // Do not save anything.
  }
};

class GenericSimpleProperty : public GenericProperty {
public:
  GenericSimpleProperty( const Chars& descr ) : GenericProperty(descr) {}
protected:
  void read (istream& istr, bool extraDetails = false);
  void write(ostream& ostr, bool extraDetails, 
	     const Chars& extrasDetailsFileName) const;
};

class GenericComplexProperty : public GenericProperty {
public:
  GenericComplexProperty( const Chars& descr ) : GenericProperty(descr) {}

protected:
  void read (istream& istr, bool extraDetails = false);
  void write(ostream& ostr, bool extraDetails, 
	     const Chars& extrasDetailsFileName) const;

private:
  static void completeHeader( char *header, const char *title, 
			      const char *name, int start, int length );
  static const int header_size;
};

template <class T>
class UnstorableProperty : public GenericUnstorableProperty {
public:

  UnstorableProperty(const T& data, const Chars descr) 
    : GenericUnstorableProperty(descr), theData(data) { }

  const T& value() const { return theData; }
        T& ref  ()       { return theData; }

  void putProperty( GenericProperty* property ) {
    if( property->actualType() != actualType() ) {
      error( "UnstorableProperty<T>::putProperty(): "
	     "wrong an actual type of the argument" );
      return;
    }
    theData = ((UnstorableProperty<T> *)property)->theData;
  }

  void readFrom(istream& istr)       {  }
  void writeTo (ostream& ostr) const {  }

protected:
  T theData;
};

template <class T>
class SimpleProperty : public GenericSimpleProperty {
public:

  SimpleProperty(const T& data, const Chars descr) 
    : GenericSimpleProperty(descr), theData(data) { }

  const T& value() const { return theData; }
        T& ref  ()       { return theData; }

  void putProperty( GenericProperty* property ) {
    if( property->actualType() != actualType() ) {
      error( "SimpleProperty<T>::putProperty(): "
	     "wrong an actual type of the argument" );
      return;
    }
    theData = ((SimpleProperty<T> *)property)->theData;
  }

  void readFrom(istream& istr)       { istr >> theData; }
  void writeTo (ostream& ostr) const { ostr << theData; }

protected:
  T theData;
};

template <class T>
class ComplexProperty : public GenericComplexProperty {
public:

  ComplexProperty(const T& data, const Chars descr) 
    : GenericComplexProperty(descr), theData(data) { }

  const T& value() const { return theData; }
        T& ref  ()       { return theData; }

  void putProperty( GenericProperty* property ) {
    if( property->actualType() != actualType() ) {
      error( "ComplexProperty<T>::putProperty(): "
	     "wrong an actual type of the argument" );
      return;
    }
    theData = ((ComplexProperty<T> *)property)->theData;
  }

  //PropertyType actualType()   const { return unregisteredProperty; } 
  //Chars        propertyName() const { return "unregisteredProperty"; }

protected:
  //GenericProperty* clone() const { return new ComplexProperty(*this); }

protected:
  T theData;
};

//
//  EXAMPLES OF USING
//
// 1) Define a property class.
//
// class GroupHasOrder : public Property<int> 
// {
// public:
//   GroupHasOrder( ) : Property<int>() { }
//   GroupHasOrder( int order ) : Property<int>( new int(order) ) { } 
//   static PropertyType type( ) { return theGroupHasOrder; }
//   PropertyType actualType( ) const { return type(); }
//   static const PropertyType theGroupHasOrder;
// };
//
// // Static data members of GroupHasOrder class.
// const PropertyType GroupHasOrder::theGroupHasOrder = 
//   GenericProperty::unique();
//
//
// 2) Work with the property like this:
//
//   PropertyCollection C; // Declare a properties collection
//
//   C.addProperty( new GroupHasOrder( 5 ) );
//
//   // Search and retreive the first property from the collection.
//
//   if( C.hasProperty( GroupHasOrder::type() ) != dontknow ) {
//     GroupHasOrder P;
//     C.getProperty( &P );
//     int order = P.value();
//     cout << "property = " << GroupHasOrder::type() 
//	    << ", value = " << order << endl;
//   }
// 
//   // Store and restore a collection from a file
//
//      To appear.
//
//
//  That's all for now.


#endif
