// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FGGroupRep class.
//           The name is abbreviated to fit in a library.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//

#ifndef _FIN_GEN_GROUP_REP_H_
#define _FIN_GEN_GROUP_REP_H_

#include "Chars.h"
#include "Vector.h"
#include "Word.h"
#include "GroupRep.h"


struct FGGroupRep : GroupRep {
  
public:

// Constructors:

  // No default constructor
  // Copy constructor provided by compiler (does deep copy).
  
  FGGroupRep( int ngens ) :
    theNumberOfGenerators(ngens),
    theNamesOfGenerators(0) { }
  // to construct a group with ngens nameless generators
  
  FGGroupRep( const VectorOf<Chars>& gennames ) :
    theNumberOfGenerators(gennames.length()),
    theNamesOfGenerators(gennames) { }
  // to construct a group with named generators
  
  // Destructor provided by compiler.

// operators:

  // disable assignment:
  private: FGGroupRep& operator = ( const FGGroupRep& );
  public:
  
// representation methods:

  // Inherited from GroupRep:
  // virtual PureRep* clone( );
  // No need for clone( ) in derived abstract representation.

  static const Type theFGGroupType;

  static Type type( ) { return theFGGroupType; }

  Type actualType( ) const { return type(); }
  // overrides GroupRep::actualType();

// Methods dealing with group structure:

  // Inherited from GroupRep:
  // virtual int order( ) const = 0;
  // virtual Trichotomy isTrivial( ) const = 0;
  // virtual Trichotomy isFinite( ) const = 0;
  // virtual Trichotomy isInfinite( ) const = 0;
  // virtual Trichotomy isAbelian( ) const = 0;

  bool compare( const GroupRep* G ) const;

// Methods dealing with group elements:
  
  Elt makeIdentity( ) const { return Word(); }
  // overrides GroupRep::...

  Bool isSyntacticIdentity( const Elt& e) const;
  // overrides GroupRep::...

  // Inherited from GroupRep:
  // virtual Trichotomy isTrivialElt( const Elt& e ) const;
  // virtual Trichotomy areEqual(const Elt&, const Elt&) const = 0;

  Elt firstElt( ) const { return Word(); }
  // overrides GroupRep::...

  Elt nextElt( const Elt& e ) const;
  // overrides GroupRep::...

  Elt multiply( const Elt& e1, const Elt& e2 ) const;
  // overrides GroupRep::...

  Elt inverseOf( const Elt& e ) const;
  // overrides GroupRep::...

  // Inherited from GroupRep:
  // virtual Elt raiseToPower(const Elt&, int) const;
  // virtual Elt conjugateBy(const Elt&, const Elt&) const;
  // virtual Elt commutator(const Elt&, const Elt&) const;

  virtual Elt eval( const Word& w ) const = 0;

  virtual Trichotomy wordProblem( const Word& w ) const = 0;

  virtual Trichotomy conjugacyProblem( const Word& u, const Word& v ) const = 0;

// I/O:

  // Inherited from GroupRep:
  // virtual void printOn(ostream&) const = 0;
  // virtual GroupRep* readFrom(istream&, Chars&) const = 0;

  virtual void printGenerator( ostream& ostr, int n ) const;

  virtual void printGenerators( ostream& ostr ) const;

  virtual void printWord( ostream& ostr, const Word& w ) const;

  virtual void printSetOfWords( ostream& ostr, const SetOf<Word>& S ) const;

  virtual void printVectorOfWords( ostream& ostr, const VectorOf<Word>& V,
	            char* leftBracket = "{", char* rightBracket = "}" ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const ; 
 
  void read( istream& istr ) ;


//@rn private:

// Data members:
 
  // Inherited from GroupRep:
  // int theOrder;
  
  int theNumberOfGenerators;
  
  VectorOf<Chars> theNamesOfGenerators;
  
// Debugging stuff:

//#ifdef DEBUG

  bool consistent( ) const {
    return theNumberOfGenerators == theNamesOfGenerators.length();
  } // overrides bool GroupRep::consistent() const;

//#endif

};

#endif
