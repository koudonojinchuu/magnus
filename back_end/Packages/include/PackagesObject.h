#include <fstream.h>
#include "Chars.h"
#include "Menu.h"
#include "FEData.h"
#include "Type.h"
#include "OID.h"

#ifndef _PackagesObject_h_
#define _PackagesObject_h_


// -------------------------- PObject ----------------------- //

class PObject
{
 public:
  PObject(  ) { }

  virtual Chars getDBName() const = 0;
  
  virtual void printObjectName( ostream& )const  = 0;
  virtual Chars getObjectName(  ) const = 0;
  virtual Type getObjectType( )const = 0;
  virtual void writeInitString(ostream& )const = 0;
  virtual void addPackagesToMenu(  ) { };
 private:
  //  PObject( const PObject& );
  PObject& operator = ( const PObject& );
};

// -------------------------- PGroup ----------------------- //

class PGroup : public PObject, protected FEData
{
public:
  // constructors
  PGroup() { }
  PGroup( const FPGroup& g, OID o, int nc) : 
    theGroup( g ), 
    oid(o), 
    nilpotClass( nc ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("Group");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( ) const { return Chars(MagnusTmp::magnusTmp()) + Chars("/group"); }
  void writeInitString(ostream& o)const{
    if (nilpotClass > 0)
      o << nilpotClass << "   ";
    o << theGroup << "  (" << oid << ")" << endl << flush;
  }

  // accessors
  FPGroup getFPGroup()const { return theGroup; }

  OID getOID() const { return oid; }
  
private:
  FPGroup theGroup;
  OID oid;
  int nilpotClass;
};

// -------------------------- PWord ----------------------- //

class PWord : public PObject, protected FEData
{
public:
  // constructors
  PWord() { }
  PWord( const PGroup& g, const Word& w,  OID ow) : 
    theGroup( g ),
    theWord( w ),
    oidW( ow ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("Word");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( ) const { return Chars(MagnusTmp::magnusTmp()) + Chars("/word"); }
  void writeInitString(ostream& o)const;

private:
  PGroup theGroup;
  Word theWord;

  OID oidW;
};



// -------------------------- PSubgroup ----------------------- //

class PSubgroup : public PObject, protected FEData
{
public:
  // constructors
  PSubgroup() { }
  PSubgroup( const PGroup& g, const VectorOf<Word>& v, OID os) : 
    theGroup( g ),
    theSubgroup( v ),
    oidS( os ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("Subgroup");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( ) const { return Chars(MagnusTmp::magnusTmp()) + Chars("/subgroup"); }
  void writeInitString(ostream& o)const;

private:
  PGroup theGroup;
  VectorOf<Word> theSubgroup;
  OID oidS;

};

// -------------------------- PMap ----------------------- //

class PMap : public PObject, protected FEData
{
public:
  // constructors
  PMap() { }
  PMap( const Map& m, const PGroup& d, const PGroup& r, OID om) : 
    theMap( m ),
    oidM( om ),
    theDomain( d ),
    theRange( r ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("Map");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( )const { return Chars(MagnusTmp::magnusTmp()) + Chars("/map"); }
  void writeInitString(ostream& o)const;

protected:
  Map theMap;
  PGroup theDomain;
  PGroup theRange;
  OID oidM;
};

// -------------------------- PHomo ----------------------- //

class PHomo : public PMap //, protected FEData
{
public:
  // constructors
  PHomo(): PMap() { }
  PHomo( const Map& m, const PGroup& d, const PGroup& r, OID om) : 
    PMap( m, d,  r,  om) { }


  // overwrites PackagesObject
  Chars getDBName() const { return Chars("Homomorphism");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( )const { return Chars(MagnusTmp::magnusTmp()) + Chars("/homomorphism"); }
  //  void writeInitString(ostream& o)const;  - inhereted from PMap

};

// -------------------------- PWordWord ----------------------- //

class PWordWord : public PObject, protected FEData
{
public:
  // constructors
  PWordWord() { }
  PWordWord( const PGroup& G, const Word& w1, const Word& w2,
	     OID ow1, OID ow2) : 
    theGroup( G ),
    theWord1( w1 ),
    theWord2( w2 ),
    oidW1( ow1 ),
    oidW2( ow2 ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("WordWord");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( )const { return Chars(MagnusTmp::magnusTmp()) + Chars("/word2"); }
  void writeInitString(ostream& o)const;

private:
  PGroup theGroup;
  Word theWord1;
  Word theWord2;
  OID oidW1;
  OID oidW2;
};

// -------------------------- PSubgroupSubgroup ----------------------- //

class PSubgroupSubgroup : public PObject, protected FEData
{
public:
  // constructors
  PSubgroupSubgroup() { }
  PSubgroupSubgroup( const PGroup& G, const VectorOf<Word>& s1, const VectorOf<Word>& s2, OID os1, OID os2) : 
    theGroup( G ),
    theSubgroup1( s1 ),
    theSubgroup2( s2 ),
    oidS1( os1 ),
    oidS2( os2 ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("SubgroupSubgroup");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( ) const { return Chars(MagnusTmp::magnusTmp()) + Chars("/subgroup2"); }
  void writeInitString(ostream& o)const;

private:
  PGroup theGroup;
  VectorOf<Word> theSubgroup1;
  VectorOf<Word> theSubgroup2;
  OID oidS1;
  OID oidS2;
};


// -------------------------- PSubgroupWord  ----------------------- //

class PSubgroupWord : public PObject, protected FEData
{
public:
  // constructors
  PSubgroupWord() { }
  PSubgroupWord( const PGroup& G, const VectorOf<Word>& s,  const Word& w,
		 OID os, OID ow) : 
    theGroup( G ),
    theSubgroup( s ),
    theWord( w ),
    oidS( os ),
    oidW( ow ){ }

  // overwrites PackagesObject
  Chars getDBName() const { return Chars("SubgroupWord");}
  void printObjectName( ostream& o ) const;
  Chars getObjectName(  ) const;
  void addPackagesToMenu(  );
  
  Type getObjectType( )const {  return thePGroupType; }
  static const Type thePGroupType;
  static Type type( ) { return thePGroupType; }
  Chars ipcFileName( ) const{ return Chars(MagnusTmp::magnusTmp()) + Chars("/subgroupword"); }
  void writeInitString(ostream& o)const;

private:
  PGroup theGroup;
  VectorOf<Word> theSubgroup;
  Word theWord;
  OID oidS;
  OID oidW;
};

// ------------ getPObjectFromType -----------------//

PObject* getPObjectFromType( Type t);
PObject* getPObjectFromInt( int i);

#endif
