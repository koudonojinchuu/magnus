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

#ifndef _SMENUMERATOR_H_
#define _SMENUMERATOR_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "Word.h"
#include "Associations.h"
#include "Subgroup.h"

template <class T> class EnumeratorProblem;


// ---------------------------- NoType -------------------------- //

class NoType {
 public:
  static Type type( ) { return theNoType; }
 private:
  static const Type theNoType;

};

// -----------------------------------------------------------------------//
// ------------------------ SMListData -----------------------------------//
// -----------------------------------------------------------------------//

class SMListData
{
public:

  // Default constructor
  SMListData( );

  SMListData( const SMListData& sd );

  void doHardCopy( const SMListData& );

  ~SMListData() {
    closeCurrentMode();
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //          :                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // operator for adding elements
  SMListData& operator << (const class WriteEnumeratorElement& e);

  // operator for all other information
  SMListData& operator << (const Chars& c);
  
  // appends a list to the end 
  void append( SMListData& sd );
  
  void setWriteMode();
  void setReadMode();
  void closeCurrentMode();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Access functions :                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  // returns i-th element as a character. 
  // Iterator must parse it to the corresponding data type
  Chars getElementOf( int i );
  
  // number of elements in the list
  int numberOfElements() const { return number_of_elements; }

  //  for front end only
  Chars getDataFileName() const { return dataFileName; }

  // returns current mode: ios::out, ios::in or -1 if 
  // no one of those was set
  int getMode() const { return mode; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // IPC tools needed to obtain the number of elements in the list
  friend ostream& operator < ( ostream& ostr, const SMListData& s )
  {
    ostr < s.dataFileName < s.refFileName;
    ostr < s.number_of_elements;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, SMListData& s )
  {
    Chars dF, rF;
    istr > dF > rF;

    if ( dF != s.dataFileName || rF != s.refFileName )
      error("friend istream& operator > ( istream& istr, SMListData& s ) :"
	    " data files are seem to be different");
    istr >  s.number_of_elements;
    return istr;
  }

private:
  
  void openFiles( int );
  
  // gives offset of the i-th element 
  int getBegPosOf( int i);

  // returns the length of an i-th element
  int getLengthOf( int i);

  ////////////////////////////////////////////////////////////////
  // Data members:                                              //
  ////////////////////////////////////////////////////////////////
  
  fstream dataFile;
  // file with elements

  fstream refFile;
  // file with offsets of lements in dataFile 
  
  Chars dataFileName;
  Chars refFileName;
  // file names

  int number_of_elements;
  // number of elements
  int mode;
  // current mode
};

//--------------------------------------------------------------------//
//------------------------ EnumeratorARCer ---------------------------//
//--------------------------------------------------------------------//



class EnumeratorARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  EnumeratorARCer( ComputationManager& boss, SMListData& d ) 
    : ARCer( boss ), theData( d ) { } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );
  
  void submitSignal( );

//  Chars getFileName() const { return file.getFileName(); }
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
  
 protected:

  virtual void enumerate() = 0;

  bool submitSignalRecieved()const;

  SMListData& theData;

 private:

 
};

//--------------------------------------------------------------------------//
//-------------------------- EnumeratorProblem -----------------------------//
//--------------------------------------------------------------------------//
template <class T> class EnumeratorProblem;

template <class T>  class EnProbType {
public:
  EnProbType() {
    //@njz
    //    ostrstream ostr;
    std::ostrstream ostr;
    //
    ostr << "EnumeratorProblem" << T::type() << '\0' << flush;
    strcpy(s, ostr.str());
    ostr.freeze(0);
  }
private:
  friend class EnumeratorProblem<T>;
  char s[100];
};

// ---------------------------------------------------------------------  //

template <class T> class EnumeratorProblem  : public Supervisor
{
public:

  EnumeratorProblem( AlgebraicObject& b ): 
    Supervisor(true),
    theParent(b) 
   {
      theData.setWriteMode();
    }
  
  ~EnumeratorProblem() {
   theData.closeCurrentMode();
  }
  
  AlgebraicObject& getParentObject() const { return theParent; }
  
  const IconID iconID( ) const {
    return IconID::enumerator_problem;
  }

  const char* typeID( ) const {
    return type( );
  }
  
  static const char* type( )  { return theType.s; }

  // Access functions
  
  SMListData& getData() { return theData;}

protected:
  SMListData theData;

  Chars theDataFile;
  static EnProbType<T> theType;
  AlgebraicObject& theParent;							    
};

#endif
