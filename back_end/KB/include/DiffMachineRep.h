// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of DiffMachineRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//

#ifndef _DiffMachineRep_H_
#define _DiffMachineRep_H_

#include "Word.h"
#include "Set.h"
#include "Vector.h"
#include "DFSARep.h"
#include "DFSA.h"
#include "DiffHistory.h"
#include "WordOrder.h"


class DiffMachineRep : public GroupDFSARep {
public:
  //@rn Kludge!!?
  DiffMachineRep() : GroupDFSARep() {}

  DiffMachineRep(const VectorOf<Chars> & genNames) : 
    GroupDFSARep("",genNames),
    diffs(1,1) { 
     Word w; diffs[0]=w; // position 0 isn't actually used
     setNumStrings(2);
    }

  DiffMachineRep(const VectorOf<Chars> & genNames,const WordOrder & word_order) : 
    GroupDFSARep("",genNames,word_order),
    diffs(1,1) { 
     Word w; diffs[0]=w; // position 0 isn't actually used
     setNumStrings(2);
    }

  DiffMachineRep( const DiffMachineRep& D ) :  // Copy constructor does deep copy.
    GroupDFSARep(D), diffs(D.diffs) { }

  DiffMachineRep & operator = ( const DiffMachineRep & D ) 
  {
    GroupDFSARep& temp = *this;
    temp = (GroupDFSARep)D;
    (DFSARep)*this = (DFSARep)D;
    diffs = D.diffs;
    return *this;
  }
  FSARep *clone() const { return new DiffMachineRep(*this); }

  Bool operator == ( const DiffMachineRep& D ) const { 
    if (equalDFSA(D)==0) return 0;
    else for (State s=1;s<=getNumStates();s++){
      if (category(s)==1 && diffs[s] != (D.diffs)[s]) 
        return 0;
    }
    return 1;
  } 


//  Bool nextAcceptedWord(Word w, int*& history) const {}; //@rn bad arg
  void minimize() {};
  
  void readFrom(istream &str = cin);

  void printAccepting(ostream &str = cout) const ;
  void printStates(ostream &str = cout) const ;

 // Set/reset the number of states in the automaton to numOfStates 
   
  void setNumStates(int numOfStates);
    
  Word getDifference(State s) const { 
      //cerr << "Got difference  number" << s << ": "<< diffs[s] << endl;
                                          return diffs[s];}

  void setDifference(State s,Word w) { diffs[s] = w;}

  void addDifferencesEqn
       (const Word & w1,const Word & w2,const WordOrder & word_order);
  void closeUnderSubstrings(int mode,const WordOrder & word_order);
  void closeUnderInverses(const WordOrder & word_order);
  Bool rewrite(Word & w) const;
  Bool rewrite(Word & w,const WordOrder & word_order) const;
  GroupDFSA wordAcceptor(const WordOrder & word_order) const;
  void DiffMachineRep::buildDifferenceMachine(DiffMachineRep & D,
       const SetOf<Word> & differences,const WordOrder & word_order) const;
  void DiffMachineRep::rebuildDifferenceMachine(const WordOrder & word_order);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void write( ostream& ostr ) const
  {
    GroupDFSARep::write(ostr);
    ostr < diffs;
  }

  void read( istream& istr )
  {
    GroupDFSARep::read(istr);
    istr > diffs;
  }

private:

  VectorOf<Word> diffs;
  void buildWordAcceptor(GroupDFSARep & Wp,const WordOrder & word_order) const;
  GroupDFSA convertToGroupDFSA(GroupDFSARep * Wp) const;
  Bool reductionAhead(State d,const AheadInfo & ai,const WordOrder & word_order) const;
};

#endif
