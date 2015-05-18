// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMEnumerator
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMLIST_H_
#define _SMLIST_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "Word.h"
#include "Associations.h"
#include "SMEnumerator.h"

template <class T> class SMList;


//--------------------------------------------------------------------------//
//---------------------------- SMListType ----------------------------------//
//--------------------------------------------------------------------------//


template <class T> class SMList;

template <class T>  class SMListType {
public:
  SMListType() {
    //@njz
    //    ostrstream ostr;
    std::ostrstream ostr;
    //
    ostr << "SMList" << T::type() << '\0' << flush;
    strcpy(s, ostr.str());
  }
private:
  friend class SMList<T>;
  char s[100];
};

// ------------------------------- LIC -------------------------------- //


class LIC : protected FEData
{  
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  LIC( OID, int );

  ~LIC( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Trivality information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy getTrivialStatus( int, GIC::AlgorithmID&, Chars& ) const;
  Trichotomy getTrivialStatus( int i ) const {
    GIC::AlgorithmID al;
    Chars s;
    return getTrivialStatus( i, al, s );
  }

  void setTrivialStatus( int i, Trichotomy is_trivial, 
			 GIC::AlgorithmID al = GIC::NONE  );
  
  bool IsTrivialChecked() const;

  bool hasTrivialStatusBuffer() const;

  void putIsTrivialFiles( const Chars& is_trivial,
			const Chars& is_not_trivial ){
    isTrivialFName = is_trivial;
    isNonTrivialFName = is_not_trivial;
  }


  Chars getListOfTrivial() const { return isTrivialFName; }
  Chars getListOfNonTrivial() const { return isNonTrivialFName; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // ''Is abelian'' information:                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy getAbelianStatus( int, GIC::AlgorithmID&, Chars& ) const;

  Trichotomy getAbelianStatus( int i ) const {
    GIC::AlgorithmID al;
    Chars s;
    return getAbelianStatus( i, al, s );
  }

  void setAbelianStatus( int i, Trichotomy is_abelian, 
			 GIC::AlgorithmID al = GIC::NONE  );
  
  bool IsAbelianChecked() const;

  bool hasAbelianStatusBuffer( ) const;

  void putAbelianFiles( const Chars& is_abelian,
			const Chars& is_not_abelian ){
    isAbelianFName = is_abelian;
    isNonAbelianFName = is_not_abelian;
  }


  Chars getListOfAbelian() const { return isAbelianFName; }
  Chars getListOfNonAbelian() const { return isNonAbelianFName; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // ''Is central'' information:                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy getIsCentralStatus( int, GIC::AlgorithmID&, Chars& ) const;

  Trichotomy getIsCentralStatus( int i ) const {
    GIC::AlgorithmID al;
    Chars s;
    return getIsCentralStatus( i, al, s );
  }

  void setIsCentralStatus( int i, Trichotomy is_abelian, 
			 GIC::AlgorithmID al = GIC::NONE  );
  
  bool IsCentralChecked() const;

  bool hasIsCentralStatusBuffer( ) const;

  void putIsCentralFiles( const Chars& is_central,
			  const Chars& is_not_central ){
    isCentralFName = is_central;
    isNonCentralFName = is_not_central;
  }


  Chars getListOfCentral() const { return isCentralFName; }
  Chars getListOfNonCentral() const { return isNonCentralFName; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // General accessors:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int numberOfElements() const { return number_of_elements; }

private:

  struct status_type {
    status_type( int );
    ~status_type();

    Trichotomy getStatus( int, GIC::AlgorithmID& ) const;
    void setStatus( int i, Trichotomy is_abelian, 
		    GIC::AlgorithmID al = GIC::NONE  );
  
    bool isChecked() const;

  private:
    char* status_buffer;
    int status_file;    
    int size;
  };
  
  Chars explanation( GIC::AlgorithmID ) const;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID listOID;
  // Need this for composing messages about what has been found.
			       
  int number_of_elements;

  status_type* abStatus;
  status_type* wpStatus;
  status_type* centStatus;
  
  Chars isAbelianFName;
  Chars isNonAbelianFName;

  Chars isTrivialFName;
  Chars isNonTrivialFName;

  Chars isCentralFName;
  Chars isNonCentralFName;
};

//--------------------------------------------------------------------------//
//-------------------------- SMList ----------------------------------------//
//--------------------------------------------------------------------------//

template <class T> class SMList : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList( class EnumeratorProblem< T >& P, const Chars heritage);

  SMList( const SMFPGroup& ao, const SMListData& d, const Chars heritage );
  
//  SMList( EnumeratorProblem< T >& P) : AlgebraicObject(""), 
//    theParent(P.getBoss().getParentObject())
//    theGroup(){ }
  
  //@Rn Bogus ctor; see But no  comment in Menu.h.
  

  ~SMList();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  const SMListData& getData() const { return theData; }

  const SMFPGroup& getGroup() const;

  const AlgebraicObject& getParent()const { return theParent; }

  LIC& lic() { return *theLic; }
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  static const char* type( )  { return theType.s; } 
  
  const char* typeID( ) const { return type(); };        // overrides SMObject
  
  const IconID iconID( ) const { return IconID::list_object; };
  // overrides SMObject
  
  void printProperties(ostream& ostr) const;
  
  void viewStructure(ostream& ostr) const;
  
  void printDefinition(ostream& ostr) const;            // overrides SMObject
  

public:
 
protected:

  void readMessage(istream&) { };                     // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  const AlgebraicObject& theParent;							    
  static SMListType<T> theType;
  
  SMListData theData;
  LIC* theLic;

};


#endif
