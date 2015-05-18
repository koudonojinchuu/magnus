// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMLIST_ITERATOR_H_
#define _SMLIST_ITERATOR_H_

#include "SMList.h"
#include "Word.h"
#include "SMEnumerator.h"

//-------------------------- SMListIterator -----------------------------//

template <class T> class SMListIterator {
public:
  SMListIterator(const SMList<T>& );
  
  ~SMListIterator() { 
    theData.closeCurrentMode();
    if ( currentData )
      delete currentData;
  }
  
  // Iteration procedures
       
  bool EOL() const { return eol_reached; }
  
  void reset(); 

  bool nextCell();
  
  const T& getData(); 

  int getNumberOfElements()const { return theData.numberOfElements(); }

private:

  void deleteCurrentData();

  void parseData();
  // Parser of an element from the list.
  // Unique for each type T.
			    
  Chars currentLine;

  T* currentData;
  // value of the current element from the list
  FPGroup theGroup;
  // group

  int numberOfCurrent;
  // number of the last parsed element( which is the currentData)
   
  SMListData theData;

  bool eol_reached;
};


// Classes below define standards for element presentation in
// enumerators and lists.  
 

// ---------------- WriteEnumeratorElement --------------------- //

class WriteEnumeratorElement
{
 public:
  WriteEnumeratorElement() {}

  //@njz: moved from private
  WriteEnumeratorElement(const WriteEnumeratorElement&);
  //

  virtual void print(ostream& ostr) const = 0;
  inline friend ostream& operator << ( ostream& o, 
				       const WriteEnumeratorElement& we ) {
//    o << we.position << ":" << "$D" << " " ;
    we.print( o );
    return o;
  }
 protected:

 private:
  //@njz: moved to be public
  //  WriteEnumeratorElement(const WriteEnumeratorElement&);
  //
};

// ------------------------ EnumWriteWord --------------------- //

class EnumWriteWord : public WriteEnumeratorElement
{
 public:
  EnumWriteWord( const Word& w, const FPGroup& g) 
    :  theW( w ), theG( g ) { }
  
  void print( ostream& o ) const;
 private:
  Word theW;
  FPGroup theG;

};

// ------------------------ EnumWriteMap --------------------- //

class EnumWriteMap : public WriteEnumeratorElement
{
 public:
  EnumWriteMap( const Map& m ) 
    : theM( m ) { }
  
  void print( ostream& o ) const;
 private:
  Map theM;
};
// ------------------------ EnumWriteVectorOfWords --------------------- //

class EnumWriteVectorOfWords : public WriteEnumeratorElement
{
 public:
  EnumWriteVectorOfWords( const VectorOf<Word>& v, const FPGroup& g ) 
    : theV( v ),
    theG( g )
    { }
  
  void print( ostream& o ) const;
 private:
  VectorOf<Word> theV;
  FPGroup theG;
};
	
// ------------------------ EnumWriteSetOfWords --------------------- //

class EnumWriteSetOfWords : public WriteEnumeratorElement
{
 public:
  EnumWriteSetOfWords( const SetOf<Word>& s, const FPGroup& g ) 
    : theS( s ),
    theG( g )
    { }
  
  void print( ostream& o ) const;
 private:
  SetOf<Word> theS;
  FPGroup theG;
};

// ------------------------ EnumWriteChars  --------------------- //

class EnumWriteChars : public WriteEnumeratorElement
{
 public:
  EnumWriteChars( const Chars& c ) 
    : theC( c )
    { }
  
  void print( ostream& o ) const;
 private:
  Chars theC;
};

#endif
