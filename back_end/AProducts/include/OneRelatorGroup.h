// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author:
//
// Status:
//
// Revision History:
//

#ifndef _ONE_RELATOR_GROUP_H_
#define _ONE_RELATOR_GROUP_H_

#include "Word.h"
#include "Chars.h"
#include "Vector.h"
#include "FPGroup.h"
#include "AP-fixups.h"

class OneRelatorGroupRep : public FGGroupRep
{

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////
        
  OneRelatorGroupRep( int ngens, const Word& relator );
  // To construct a group of given number of unnamed generators and
  // a given relator.
  
  OneRelatorGroupRep( const VectorOf<Chars>& gennames, const Word& relator ); 
  // To construct a group with named generators, a given relator.  

  OneRelatorGroupRep( const FPGroup& G );
  
  // Copy ctor, operator= and dtor provided by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Representation methods:                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  PureRep* clone( ) const { return new OneRelatorGroupRep(*this); }
  // Overrides FGGroupRep::clone().

  static const Type theOneRelatorGroupType;

  static Type type( ) { return theOneRelatorGroupType; }
  // dominates FGGroupRep::type()

  Type actualType( ) const { return type(); }
  // overrides FGGroupRep::actualType();

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors / Modifiers                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  //FPGroup presentation( ) const { return theFPGroup; }

  // Word relator( ) const { return theRelator; }

  //  int numberOfGenerators( ) const { return theFPGroup.numberOfGenerators(); }

  // VectorOf<Chars> namesOfGenerators() const { return theFPGroup.namesOfGenerators(); }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods dealing with group structure             //
  //                                                   //
  ///////////////////////////////////////////////////////

  int order( ) const;

  Trichotomy isTrivial( ) const; 
  // override FGGroupRep::isTrivial().

  Trichotomy isFinite( ) const; 
  // override FGGroupRep::isFinite().

  Trichotomy isInfinite( ) const;
  // override FGGroupRep::isInfinite().

  Trichotomy isAbelian( ) const; 
  // override FGGroupRep::isAbelian().

  Trichotomy isFree( ) const;
  // override FGGroupRep::isFree().

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////

  Trichotomy wordProblem( const Word& w, bool keepDetails,
    ProductOfRelatorConjugates& productOfRelatorConjugates ) const;

  Trichotomy wordProblem( const Word& w ) const;

  Trichotomy areEqual( const Elt& e1, const Elt& e2 ) const {
    return wordProblem( e1 * e2.inverse() );
  }

  Elt eval( const Word& w ) const { return w.freelyReduce(); }
  // Overrides pure-virtual FGGroupRep::eval().

  Trichotomy conjugacyProblem( const Word& u, const Word& w ) const {
    return dontknow;
  }
  // Overrides pure-virtual FGGroupRep::eval().
  // This method is required by compiler to instantiate this class.
  // This problem is unsolvable in theory.
  

  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////

  GroupRep* readFrom( istream& istr, Chars& errMesg ) const;

  void printOn( ostream& ostr ) const;

  bool operator == ( const OneRelatorGroupRep& G ) const {
    if( theNumberOfGenerators == G.theNumberOfGenerators 
	&& theRelator == G.theRelator )
      return true;
    return false;
  }

  bool operator != ( const OneRelatorGroupRep& G ) const {
    return !( *this == G );
  }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  IPC tools:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  void write( ostream& ostr ) const;
  // overrides FGGroupRep::write().

  void read( istream& istr );
  // overrides FGGroupRep::read().

 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data:                                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word theRelator;  
};



class OneRelatorGroup 
  : public DerivedObjectOf< FGGroup, OneRelatorGroupRep >
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OneRelatorGroup( int ngens, const Word& relator ) 
    : DerivedObjectOf<FGGroup, OneRelatorGroupRep>( 
	new OneRelatorGroupRep( ngens, relator ) ) {} 
  // To construct a group of given number of unnamed generators and
  // a given relator.
  
  OneRelatorGroup( const VectorOf<Chars>& gennames, const Word& relator )
    : DerivedObjectOf<FGGroup, OneRelatorGroupRep>( 
	new OneRelatorGroupRep( gennames, relator ) ) {} 
  // To construct a group with named generators, a given relator.  

  OneRelatorGroup( const FPGroup& G )
    : DerivedObjectOf<FGGroup, OneRelatorGroupRep>( 
	new OneRelatorGroupRep( G ) ) {} 
  
  // Copy constructor, operator=, and destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word relator( ) const { return look()->theRelator; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy wordProblem( const Word& w ) const
  {
    ProductOfRelatorConjugates dummy;
    const bool dontKeepDetails = false;
    return look()->wordProblem( w, dontKeepDetails, dummy );
  }


  Trichotomy wordProblem( const Word& w, bool keepDetails, 
    ProductOfRelatorConjugates& productOfRelatorConjugates ) const
  {
    return look()->wordProblem( w, keepDetails, productOfRelatorConjugates );
  }

  // Trichotomy wordProblem( const Word& w ) const;
  // Overrides pseudo-virtual FGGroup::wordProblem().

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const OneRelatorGroup& G )
  {
    G.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, OneRelatorGroup& G )
  {
    G.change()->read(istr);
    return istr;
  }

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OneRelatorGroup( OneRelatorGroupRep *newrep ) 
    : DerivedObjectOf<FGGroup, OneRelatorGroupRep> ( newrep ) { }

};


/////////////////////////////////////////////////////////////////////////// 

class EnumeratorOfConsequences 
{
public:
  EnumeratorOfConsequences( const OneRelatorGroup& G );
  // Create enumerator of all relator's consequences for the given group.

  Word word() const;
  // It returns a consequence of the relator.

  ProductOfRelatorConjugates tuple() const;
  // It return a consequence is presented as decomposition of conjugates
  // of the given relator.

  void reset();
  // Resets the enumerator.

  bool done() const;
  // Returns false iff all consequences are enumerated.

  void advance();
  // It advances enumeration.
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const EnumeratorOfConsequences& ce )
  {
    ostr 
      < ce.theCurrentWordNumber 
      < ce.theLastComputedWordNumber
      < ce.theCurrentWord
      < ce.theCurrentProduct
      < ce.theRelator
      < ce.theGroup;
      
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, EnumeratorOfConsequences& ce )
  {
    
    istr 
      > ce.theCurrentWordNumber 
      > ce.theLastComputedWordNumber
      > ce.theCurrentWord
      > ce.theCurrentProduct
      > ce.theRelator
      > ce.theGroup;
  
  return istr;
  }

private:

  void generate( ) const;
  // Generate new consequence.

  Integer theCurrentWordNumber;
  Integer theLastComputedWordNumber;
  Word theCurrentWord;
  ProductOfRelatorConjugates theCurrentProduct;
  Word theRelator;
  FreeGroup theGroup;
};

#endif
