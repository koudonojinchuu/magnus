// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of mixin class FEData
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _FEDATA_H_
#define _FEDATA_H_


#include <iostream.h>
#include "Chars.h"
#include "OID.h"
#include "ARCSlotID.h"


//   Introduction:
//   ------------
// 
//   The following discussion is addressed to maintainers; users of class
// FEData may wish to skip to the `Usage' section.
// 
//   Overview of Requirements:
//   ------------------------
// 
//   The Magnus Front End (FE) must be able to reflect the state of the
// Session Manager (SM). For modularity, the FE should know as little as
// possible about the SM data it manipulates.
// 
//   Therefore, we predefine a small number of events, in response to
// which the FE is to take some action which reflects the SM's state. The
// SM describes to the FE what to do in each event, via a simple message
// protocol.  The descriptions may refer to elements of a `blind'
// database, which is stored by the FE and kept up to date by the SM.
// 
//   The FE must know nothing about the database except how to store and
// extract data. Programmers in the SM layer should know nothing about
// the database except its semantics in a given event.
// 
//   The classes wrapped in class FEData form the programmer's interface
// between the SM and the database.
// 
//   Terminology:
//   -----------
// 
//   The database is an associative array, where the keys and data can be
// arbitrary strings. Exactly what strings are used is, of course,
// hidden by the FEData classes.
// 
//   A `Key' may be constant or variable. When constant, it typically
// consists of an object id supplied by the programmer, and a key
// descriptor which is hidden from the programmer. When variable, it
// typically consists of the index of a formal variable supplied by the
// programmer, and the key descriptor. The latter allows database
// references to be specified for events in advance, and later evaluated
// in a context in which the formal variables have values. This is
// tricky; see the discussion of menus, below.
// 
//   A `Datum' is the value returned by the database for a given key. It
// is always constant, and is typically a boolean value, an object id, or
// a string such as an object's name.  A Datum is represented only
// conceptually by FEData (i.e., not as a class), since it exists only in
// the FE. The programmer specifies a Datum via a Key.
// 
//   An `Expression' is built up from Keys via operators like `==', `&&',
// and `!'. Thus an Expression may be variable or constant.
// 
//   A `Name' is an Expression which contains another Expression, where
// the latter evaluates to an object id.
// 
//   Events:
//   ------
// 
//   These are the events for which the FE must take action which depends
// on the database:
// 
//   1) There is new data to be stored.
// 
//      The SM must supply a constant Key, and a constant Expression
// (which may be a simple value) to become the Datum. For example, the SM
// may announce that `the parent group of word <object-id> is
// <object-id>'.  The `parent group' part is resolved as a key
// descriptor, the first object id becomes part of the Key, and the
// second object id is the Datum.
// 
//   2) The SM wishes to display textual data.
// 
//      In this case, the SM knows exactly which objects it wants to talk
// about, so it can supply their object id's. SM programmers must be able
// to embed constant Names in text.
// 
//   3) A new workspace object has been created.
// 
//      The view description for the object may contain text which refers
// to various objects by name. These objects are known when the new one
// is created, so the situation is as in 2).
// 
//   4) The workspace selection has changed.
// 
//      <To Appear>
//
//
//   Usage:
//   -----
// 
//      <To Appear>
//


//@rn  This stuff was too quickly hacked, and needs more thought. In
//@rn  particular, the ! operator does not work properly; e.g., the
//@rn  expression ( !  IsAuto(...) ) is spuriously false when there
//@rn  is no `auto' key stored at all in the FE.



//---------------------------------------------------------------------------//
//----------------------------- FEData --------------------------------------//
//---------------------------------------------------------------------------//

class FEData
{
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Rep Classes:                                                        //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
	 
  struct ExpressionRep
  {
	 ExpressionRep( ) : refs( 1 ) { }

	 ExpressionRep* gimme( ) {
		++refs;
		return this;
	 }

	 void getHence( ) {
		if ( ! --refs ) { delete this; }
	 }

	 virtual ~ExpressionRep( ) { }
	 
	 virtual void printOn(ostream& ostr) const = 0;

	 // Data Members:

	 int refs;
  };


  struct KeyRep : public ExpressionRep
  {
	 KeyRep(int i);  // Stupid special case

	 KeyRep(int i, const char* t);

	 KeyRep(OID oid, const char* t);

	 KeyRep(ExpressionRep* se, const char* t);

	 ~KeyRep( ) { if ( subExpression ) subExpression->getHence(); }

	 void printOn(ostream& ostr) const;        // overrides ExpressionRep

	 // Data Members:

	 const int index;
	 const char* text;
	 bool isConstant;
	 ExpressionRep* subExpression;
  };

  
  struct JoinRep : public ExpressionRep
  {
	 JoinRep(ExpressionRep* a1, ExpressionRep* a2, const char* j);

	 ~JoinRep( );
	 
	 void printOn(ostream& ostr) const;         // overrides ExpressionRep

	 // Data Members:

	 ExpressionRep* lhs;
	 ExpressionRep* rhs;
	 const char* junctor;  // Always static data
  };


  struct NotRep : public ExpressionRep
  {
	 NotRep(ExpressionRep* a);

	 ~NotRep( );

	 void printOn(ostream& ostr) const;         // overrides ExpressionRep

	 // Data Members:

	 ExpressionRep* expr;
  };


  struct NameRep : public ExpressionRep
  {
	 NameRep(ExpressionRep* a);

	 ~NameRep( );

	 void printOn(ostream& ostr) const;         // overrides ExpressionRep

	 // Data Members:

	 ExpressionRep* expr;
  };

//@njz
//protected:
public:
//

  //-------------------------------------------------------------------------//
  //---------------------------- Expression ---------------------------------//
  //-------------------------------------------------------------------------//
  
  class Expression
  {
  public:
	 
	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Constructors:                                                       //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////
	 
	 Expression(int i) : theRep( new KeyRep( i ) ) { }
	 // The hackery is getting rather thick; this is a quicky to allow storage
	 // of, e.g., enum values as data.
	 
	 Expression( const Expression& E ) {
		theRep = E.theRep->gimme();
	 }
	 
	 ~Expression( ) { theRep->getHence(); }
 
	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Operators:                                                          //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////
	 
	 Expression operator == ( const Expression& E ) const {
		return join( E, "==" );
	 }

	 Expression operator != ( const Expression& E ) const {
		return join( E, "!=" );
	 }
	 
	 Expression operator && ( const Expression& E ) const {
		return join( E, "&&" );
	 }

	 Expression operator || ( const Expression& E ) const {
		return join( E, "||" );
	 }

	 Expression operator ! ( ) const {
		return Expression( new NotRep( theRep->gimme() ) );
	 }

	 Expression operator > ( const Expression& E ) const {
		return join( E, ">" );
	 }

	 Expression operator >= ( const Expression& E ) const {
		return join( E, ">=" );
	 }

	 Expression operator < ( const Expression& E ) const {
		return join( E, "<" );
	 }

	 Expression operator <= ( const Expression& E ) const {
		return join( E, "<=" );
	 }
	 
	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Output Methods:                                                     //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////
	 
    friend ostream& operator << ( ostream& ostr, const Expression& E);

	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Protected Methods:                                                  //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////
	 
	 Expression( ExpressionRep* rep ) : theRep( rep ) { }
	 // Call this ONLY when rep.refs already has the right value!

	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Data Members:                                                       //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////

	 ExpressionRep* theRep;  // Public on purpose...

	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Private Methods:                                                    //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////

  private:
	 
	 Expression join( const Expression& E, const char* junctor ) const;
	 
  };


  //-------------------------------------------------------------------------//
  //------------------------------- Key -------------------------------------//
  //-------------------------------------------------------------------------//

  class Key : public Expression
  {
  public:

	 /////////////////////////////////////////////////////////////////////////
	 //                                                                     //
	 // Constructors:                                                       //
	 //                                                                     //
	 /////////////////////////////////////////////////////////////////////////

	 Key( const Key& K ) : Expression( K.theRep->gimme() ) { }

	 // Expression's dtor does the job

  protected:

	 Key(int i) : Expression( new KeyRep( i ) ) { }  // Stupid special case

	 Key(int i, const char* t) : Expression( new KeyRep( i, t ) ) { }

	 Key(OID oid, const char* t) : Expression( new KeyRep( oid, t ) ) { }

	 Key(const Key& K, const char* t)
	   : Expression( new KeyRep( K.theRep->gimme(), t ) )
	 { }

  };


  //-------------------------------------------------------------------------//
  //---------------------------- DataPair -----------------------------------//
  //-------------------------------------------------------------------------//

  struct DataPair
  {
	 DataPair( const Key& key, const Expression& datum )
      : theKey( key ), theDatum( datum )
	 { }
	 
	 // These are largely unneeded, but ListOf requires them:

	 bool operator == (const DataPair& dp) const { return false; }
	 DataPair& operator = (const DataPair& dp);
    friend ostream& operator << ( ostream& ostr, const DataPair& dp);

  private:

	 Key theKey;
	 Expression theDatum;
  };


  //-------------------------------------------------------------------------//
  //------------------------------ Text -------------------------------------//
  //-------------------------------------------------------------------------//

  struct Text
  {
         Text(const char* t = "");
         Text(const Expression& E);
    
         //@njz: moved from private to be public
	 Text(const Text&);
         //

	 operator Chars( ) const;
	 Text& operator + (const Text& t);
	 Text& operator + (const Expression& E);
    friend ostream& operator << (ostream& ostr, const Text& t);
  private:

    //@njz: moved to public
    //	 Text(const Text&);
	 // A Text is to be passed only by reference, so this is hidden and
	 // not to be implemented. Also, unnecessary parenthesizing results
	 // in a `passing reference to temp' warning which must not be
	 // ignored.
    //

    //@njz
    //	 ostrstream ostrstr;
         std::ostrstream ostrstr;
    //
  };


  //-------------------------------------------------------------------------//
  //------------------------- Key Derivatives -------------------------------//
  //-------------------------------------------------------------------------//
  
  struct True : public Key
  {
	 True( ) : Key( 1 ) { }
  };


  struct False : public Key
  {
	 False( ) : Key( 0 ) { }
  };


  struct Object : public Key
  {
	 Object(int i) : Key( i, 0 ) { }
	 Object(OID o) : Key( o, 0 ) { }
	 // Semantic pact with KeyRep: text == 0 => not a real key
  };


  struct CheckinType : public Key
  {
	 CheckinType(int i) : Key( i, "checkin_type" ) { }
	 CheckinType(OID o) : Key( o, "checkin_type" ) { }
	 CheckinType(const Key& K) : Key( K, "checkin_type" ) { }
  };
  // Many menus and ARC slots are conditional, based purely on how the
  // selected objects were checked in (e.g., a group checked in as an
  // abelian group, not an FP group which is discovered to be
  // abelian).  Thus, this allows for associating, e.g., an enum
  // (type) value with an object, when and only when it is created,
  // and for subsequently using the value to choose the right menu item.


  // However, some menu items cannot make sense unless some piece of
  // information is known. For example, `Extend <a free group> by <a map>'
  // must not be posted unless the map is known to be an automorphism of
  // the group. Thus, in as few cases as we can get away with (to
  // stave off utter insanity), we dynamically update the FE database
  // with such crucial information when it is discovered. The following
  // provide the interface:
  
  
  struct IsHomo : public Key
  {
	 IsHomo(int i) : Key( i, "homo" ) { }
	 IsHomo(OID o) : Key( o, "homo" ) { }
  };
  
  
  struct IsIso : public Key
  {
	 IsIso(int i) : Key( i, "iso" ) { }
	 IsIso(OID o) : Key( o, "iso" ) { }
  };
  
  
  struct IsAuto : public Key
  {
	 IsAuto(int i) : Key( i, "auto" ) { }
	 IsAuto(OID o) : Key( o, "auto" ) { }
  };


  // The following are for static properties of algebraic objects which
  // are needed to determine whether a menu item, etc., should be posted.

  struct Parent : public Key
  {
	 Parent(int i) : Key( i, "parent" ) { }
	 Parent(OID o) : Key( o, "parent" ) { }
  };

  struct ParentGroup : public Key
  {
	 ParentGroup(int i) : Key( Parent(i), "parent" ) { }
	 ParentGroup(OID o) : Key( Parent(o), "parent" ) { }
  };
  
  struct Domain : public Key
  {
	 Domain(int i) : Key( i, "domain" ) { }
	 Domain(OID o) : Key( o, "domain" ) { }
  };

  
  struct Range : public Key
  {
	 Range(int i) : Key( i, "range" ) { }
	 Range(OID o) : Key( o, "range" ) { }
  };
  

  struct Oid : public Key
  {
	 Oid(int i) : Key( i, "oid" ) { }
	 Oid(OID o) : Key( o, "oid" ) { }
  };


  //-------------------------------------------------------------------------//
  //------------------------------ Name -------------------------------------//
  //-------------------------------------------------------------------------//

  struct Name : public Expression
  {
	 Name(int i);
	 Name(OID o);
	 Name(const Expression& E);
  };


  // ----------------------------- Link ------------------------------------ //

  struct Link : public Text
  {
    Link( const Chars& text, const Chars& problemName,
	  const Chars& fileName, bool isDynamic = false );
  };


  // ------------------------ SubProblemName ------------------------------- //

  struct SubProblemName
  {
    SubProblemName( OID oid, ARCSlotID asi ) 
      : theOid( oid.unwrap() ), theAsi( asi.unwrap() ) 
    { }
    
    friend ostream& operator << ( ostream& ostr, const SubProblemName& n ) {
      ostr << "[get_subProblemName " << n.theOid << " " << n.theAsi << "]";
      return ostr;
    }
 
  private:
    
    int theOid;
    int theAsi;
  };

  // ---------------------------- Banner ----------------------------------- //

  struct Banner
  {
    Banner( OID oid ) 
      : theOid( oid.unwrap() )
    { }
    
    friend ostream& operator << ( ostream& ostr, const Banner& b ) {
      ostr << "[get_viewBanner " << b.theOid << " " << "]";
      return ostr;
    }
 
  private:
    
    int theOid;
  };

};

#endif
