// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of Type class
//
// * Type objects represent the type of a class, and thus permit
//   objects to explicitely identify themselves by returning such
//   an object. This is useful for run-time type enquiries and
//   explicit polymorphism. Type objects can only be created, copied,
//   and compared.
//
// Principal Author: Stephane Collart
//
// Status: in use
//
// Usage:
//
// * A class MyClass must store a
//      static const Type MyClass::theMyClassType
//   and initialise this in MyClass.C with
//      const Type MyClass::theMyClassType = Type( unique() );
//   at a suitable place. Access to the type is conveniently done for
//   instance from a function
//      virtual Type MyClass::actualType( ) const { return theMyClassType; }
//   It is guaranteed that the Types of different classes are different
//   amongst each other and different from the Type constructed by
//   Type().
//
// * In the case of an object-level class or hierarchy with multiple
//   representations, the type ur-object will be in the representation,
//   e.g.
//      static const Type MyClassRep::theMyClassType;
//   the object level class will have e.g.
//      static Type MyClass::type( ) { return MyClassRep::type(); }
//      Type MyClass::actualType( ) const { return look()->actualType(); }
//   and in its represenation one will find e.g.
//      static Type MyClassRep::type( ) { return theMyClassType; }
//      virtual Type MyClassRep::actualType const { return theMyClassType; }
//   the latter to be overridden in derived representations.
//   
// Extensions:
//
// * If useful, concrete information can be added to the type objects,
//   from instance by deriving from Type.
//
// Revision History:
//
// Implementation Notes :
//
// * Alternative Implementation:
//
//   class TypeDatum {
//   private :
//   	// disable all operations with TypeDatum
//   	TypeDatum( const TypeDatum& ) { }
//   	TypeDatum operator=( const TypeDatum& );
//   };
//   // TypeDatums are dummy objects which just exist; they can only be
//   // created or deleted -- their only purpose is to be pointed to
//   // by Type objects representing the same type; for every type, there
//   // is exactly one TypeDatum object, in that class as a static
//   // const member.
//   
//   class Type {
//   private :
//   	TypeDatum* typeflag;
//   public :
//   	bool operator == ( const Type& t ) { return typeflag == t.typeflag; }
//   	boll operator!= ( const Type& t) { return !(*this == t); }
//   };
//   // Type objects identify themselves by pointing to the TypeDatum
//   // of the class whose type they represent.
//

#ifndef _TYPE_H_
#define _TYPE_H_


#include <iostream.h>
 
class Type {
public :

// constructors:

	class notype { }; // pure `marker' helper class
	class unique { }; // pure `marker' helper class

    Type( const notype& ) : typeNumber(0) { }
	// default constructor yields Type object guaranteed not to
    // represent type of any class

    Type( const unique& ) : typeNumber(++uniqueTypeNumber) { }
	// special constructor for initialisation of the client class static
	// Type member

	// copy constructor provided by compiler

	// destructor provided by compiler

// operators:

	// operator = provided by compiler

// comparators:

    int operator== ( const Type& t) { return typeNumber == t.typeNumber; }
    int operator!= ( const Type& t) { return !(*this == t); }

// I/O:

	void writeTo( ostream& o ) const { o << typeNumber; }

	void readMeFrom( istream& i ) { i >> typeNumber; }
 
private :

// data members:

    int typeNumber;
	// the class Types have a unique identification number each

// shared data:

	static int uniqueTypeNumber;
	// central register of used unique Type numbers; always set to the
	// last used number. Numbering of types starts at 1; 0 reserved for
	// `no type'.

// debuggging stuff:

	#ifdef DEBUG
	//friend int main( );
	#endif

};


// Operator Synonyms for I/O members:

inline ostream& operator << ( ostream& o, const Type& t )
	{ t.writeTo(o); return o; }

inline istream& operator >> ( istream& i, Type& t )
	{ t.readMeFrom(i); return i; }


#endif
 

