// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of GenMultRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 11/97 Dmitry B. implemented IPC tools.
//

#ifndef _GenMultRep_H_
#define _GenMultRep_H_

#include "Word.h"
#include "Set.h"
#include "Vector.h"
#include "DFSARep.h"

class GenMultRep : public GroupDFSARep {
public:
  //@rn Kludge!!?
  GenMultRep() : GroupDFSARep() {}

  GenMultRep(const VectorOf<Chars> & genNames) : 
    GroupDFSARep("",genNames),
    mults(1,YES) { 
     setNumStrings(2);
    }

  GenMultRep(const VectorOf<Chars> & genNames,const WordOrder & word_order) : 
    GroupDFSARep("",genNames,word_order),
    mults(1,YES) { 
     setNumStrings(2);
    }


  GenMultRep( const GenMultRep& GM ) :  // Copy constructor does deep copy.
    GroupDFSARep(GM), mults(GM.mults) { }

  GenMultRep & operator = ( const GenMultRep & GM )
  {
    GroupDFSARep& temp = *this;
    temp = (GroupDFSARep)GM;
    mults = GM.mults;
    return *this;  
  }

  FSARep *clone() const { return new GenMultRep(*this); }

  Bool operator == ( const DFSARep& ) const { return 0; } //@rn implement!

//  Bool nextAcceptedWord(Word w, int*& history) const {}; //@rn bad arg
  void minimize() {};
  
  void readFrom(istream &str = cin);

/*
  void printOn(ostream &str = cout) const ;
*/
  void printAccepting(ostream &str = cout) const ;
  void printStates(ostream &str = cout) const ;

 // Set/reset the number of states in the automaton to numOfStates 
   
  void setNumStates(int numOfStates);

// i below is ord(g) where g is the generator associated with s,
// except that i=0 where s is associated with the identity
  void setMultiplier(State s,int i) { setCategory(s,1); mults[s]=i; }
  int getMultiplier(State s) { return mults[s];}


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void write( ostream& ostr ) const
  {
    GroupDFSARep::write(ostr);
    ostr < mults;
  }

  void read( istream& istr )
  {
    GroupDFSARep::read(istr);
    istr > mults;
  }
    
private:

  VectorOf<int> mults;
};
#endif
