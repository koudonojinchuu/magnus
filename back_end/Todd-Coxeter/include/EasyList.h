// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of EasyList and EasyCell
// template classes.
//
// Principal Authors: Alexei Myasnikov
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//

#ifndef _EYASY_LIST_H_
#define _EYASY_LIST_H_

#include "Cell.h"


template <class T> class EasyList;


//---------------------- class EasyCell ----------------------------------

template <class T> class EasyCell {
public:
  // Constructors :
  EasyCell(const T& t): theContent(t),theNext(NULL){}
  EasyCell(const T& t, EasyCell* c): theContent(t),theNext(c){}
  
  EasyCell* next() const {return theNext;}
  // Returns next element, or NULL.
  const T& content()const {return theContent;}
  //Returns the value of content
  void setContent(const T& t) { theContent = t;}
  //Sets the value of content
  void setNext(EasyCell* n) { theNext = n;}
  // Sets pointer to next cell. 
private:
  EasyCell(const EasyCell&);
  // Copy constructor is prohibited

  friend class EasyList<T>;
  T theContent;
  T& ref() {return theContent;}
  EasyCell* theNext;
};

//---------------------- class EasyList ----------------------------------

template <class T> class EasyList {

public:
  //Constructors:
  EasyList( ) : root(NULL), last(NULL), current(NULL),end_was_red(false) { }

  EasyList( const T& t ) {
    current = root = last = new EasyCell<T>(t);
  }

  // copy constructor 
  EasyList( const EasyList& lr );
  
  ~EasyList( ) {
    EasyCell<T>* r;
    while ( root ) {
      r = root->next();
      delete root;
      root = r;
    }
  }
  //--------------------------------------------------
  // Fast methods :
  //--------------------------------------------------
  bool isFirst() const { 
    if (!root) return false;
    return root == current;
  }
  // True if current points to the first element.
  // false, if empty list

  bool isLast() const { 
    if (!last) return false;
    return last == current;
  }
  // True if current points to the last element.
  // false, if empty list
  
  bool gotEnd() const { 
    return end_was_red || current == NULL;
  }
  // True if list is empty, or if last element was red by 
  // getCurrent() or value() methods.
  // After changing of current position gotEnd() will be reseted to false

  EasyCell<T>* getLast() const { 
    if (!last)
      error ("List is empty.");
    return last;
  }
  // Returns a pointer on the last cell in the list

  EasyCell<T>* getFirst() const { 
    if (!root)
      error ("List is empty.");
    return root;
  }
  // Returns a pointer on the first cell in the list
  
  EasyCell<T>* getCurrent()const;
  // Returns a pointer on the current cell in the list
  // Pseudo constant. Actualy it can change the return value of 
  // gotEnd() method

  const T& value()const { return getCurrent()->content();}
  // Returns a value of the contents of current cell  

  bool setCurrent(EasyCell<T>* setpoint);
  // Returns true and sets current position in the list, pointing
  // to the setpoint. False if empty list  

  void setContent(T setcont){ getCurrent()->ref() = setcont; }
  // Sets value ov content of the current cell
  
  bool setToFirst() {return setCurrent(root);}
  // False if empty list, if not
  // sets current to the first cell

  bool setToLast() {return setCurrent(last);}
  // False if empty list, if not
  // sets current to the last cell
  
  bool isEmpty() const {return !root;}
  // True if empty list

  bool next();
  // false, if at the end of list. Moves current to the next cell.

  //--------------------------------------------------
  // Methods, adding elements
  //--------------------------------------------------
  void prepend(const T&);
  // add a new element at the begining of the list
  void append(const T&);
  // add a new element to the end of the list
   
  void insertAfter(const T&);
  // Inserts element after current, if list is emty
  // just adds this element
  void insertBefore(const T&);
  // Inserts element before current, if list is emty
  // just adds this element
  
  //-----------------------------------------------
  // Methods deleting elements
  //-----------------------------------------------
  bool deleteFirst();
  // Deletes first element of the list
  bool deleteCurrent();
  // Deletes current element of the list
  // If current == last, then it will be time consuming
  bool deleteAfter();
  // Deletes after current

  //---------------------------------------
  // Time consuming methods
  //---------------------------------------
  bool setCurrentToIndex(int i);
  // set current to the ith cell in the list
  // If i > then set to the end, and returns false.

  bool search(const T&,EasyCell<T>** ) const;
  // Set the second argument to the pointer on a cell with data equal to
  // first argument. NULL if can`t find.
  bool deleteLast();
  // Delete the last cell in the list

  //---------------------------------------
  // operators
  //---------------------------------------
  bool operator == ( const EasyList& lr ) const;

  friend ostream& operator << ( ostream& ostr, const EasyList& ls ) 
  {
    ls.printOn(ostr);
    return ostr;
  }

  friend ostream& operator < ( ostream& ostr, const EasyList& cr )
  {
    error("IPC tools are not emplemented for EasyList");
    return ostr;
  } 

  friend istream& operator > ( istream& istr, EasyList& cr )
  {
    error("IPC tools are not emplemented for EasyList");
    return istr;
  } 
  
private:
  void printOn(ostream& ostr=cout)const;
  // Data members:

  EasyCell<T>* root;
  // pointer to the first cell
  EasyCell<T>* last;
  // pointer to the last cell
  EasyCell<T>* current;
  // pointer to the current cell
  bool end_was_red;

};

#endif
