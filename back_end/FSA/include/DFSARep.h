// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the DFSARep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//


#ifndef _DFSA_REP_H_
#define _DFSA_REP_H_

#include <Integer.h>
#include "global.h"
#include "Vector.h"
#include "Generator.h"
#include "Word.h"
#include "Vector.h"
#include "Chars.h"
#include "FSARep.h"
#include "WordOrder.h"

// Use the class DFSARep for a basic DFSA whose language is seen only
// as a semigroup, and the class GroupDFSARep if the language is seen
// as a group.


class DFSARep : public FSARep {

// This is the class at the base of the DFSA hierarchy, and contains all
// the data for a DFSA.
// The exact interpretation of the
// alphabet varies according to whether the language of the DFSA
// is connected with a semigroup or a group, so the functions
// which depend on the connection with the alphabet are overwritten for
// a GroupDFSA in the GroupDFSARep class.

public:
// Typedefs
// These will always be integral types, but it's possible 
// we might change them to short, or char.
// Since categories are stored in the transit table, 
// the Category type mustn't be
// more space consuming that the State type.

typedef int State;
typedef int Category; 
typedef int GeneratorIndex; // the type returned by the ord function


// constructors

//default constructor
  DFSARep() : 
    name(""), paddingSymbol("_"), // don't want empty strings for these
    generatorNames(0),
    numSymbols(0), numTransits(0),
    numStates(0), numStrings(1),
    transitTable(1,YES),
    categoryList(1,YES),
    minimized(NO){
      transitTable[0] = (State *)0;
      categoryList[0]=0;
    } 

  DFSARep (const Chars & Name,const VectorOf<Chars> & genNames) :
    name(Name), paddingSymbol("_"),
    generatorNames(genNames),
    numSymbols(genNames.length()), 
    numStates(0), numStrings(1),
    numTransits(genNames.length()),
    transitTable(1,YES),
    categoryList(1,YES),
    minimized(NO){ 
      transitTable[0] = (State *)0;
      categoryList[0]=0;
    }

  DFSARep (const Chars & Name, const VectorOf<Chars> & genNames, 
                               int numOfStates, int numOfStrings) :
    name(name), paddingSymbol("_"),
    generatorNames(genNames),
    numSymbols(genNames.length()), 
    numStates(numOfStates), numStrings(numOfStrings),
    numTransits(numOfStrings==1? genNames.length() : genNames.length()*(genNames.length()+2)),
    transitTable(numOfStates+1,YES),
    categoryList(numOfStates+1,YES),
    minimized(NO) { 
      for (State i=0;i<=numOfStates;i++){
        transitTable[i] = (State *)0;
        categoryList[0]=0;
      }
    }

  DFSARep( const DFSARep& D ) :  // Copy constructor does deep copy.
    name(D.name), paddingSymbol(D.paddingSymbol),
    generatorNames(D.generatorNames),
    numStates(D.numStates), numStrings(D.numStrings),
    numSymbols(D.numSymbols), numTransits(D.numTransits),
    categoryList(D.categoryList),
    minimized(D.minimized) { 
      transitTable = D.copyTransitTable();
// since the transitTable is a vectorOf pointers we need to specifically do
// a copy here - the VectorOf copy constructor copies only the pointers,
// and not their values
    }

// destructor

  ~DFSARep() {
    clearTransitTable();
  }

  DFSARep & operator = ( const DFSARep & D ) 
  {
    name=D.name; paddingSymbol=D.paddingSymbol;
    generatorNames=D.generatorNames;
    numStates=D.numStates; numStrings=D.numStrings;
    numSymbols=D.numSymbols; numTransits=D.numTransits;
    transitTable = D.copyTransitTable();
// since the transitTable is a vectorOf pointers we need to specifically do
// a copy here - the VectorOf copy constructor copies only the pointers,
// and not their values
    categoryList=D.categoryList;
    minimized=D.minimized;
    return *this;
  }

  inline friend ostream& operator << 
        ( ostream& ostr, const DFSARep& D ) {
        ostr << "This is just a dummy operator"<< endl;
    return ostr;
  }

  FSARep *clone() const { return new DFSARep(*this); }

  Bool equalDFSA(const FSARep & D) const {

// We declare two DFSA's to be equal if their alphabets are the same,
// the numbers of strings, and the transitions and categories match.
// We don't care about the name of a DFSA or the symbol used for padding.
// We check the numbers of states before the table, because it's easy to do
// that, and if they don't match the DFSA's cannot be equal.
    const DFSARep & DD = (DFSARep &)D;
    if (generatorNames!=DD.generatorNames)
      return no;
    else if (numSymbols!=DD.numSymbols)
      return no;
    else if (numStates!=DD.numStates)
      return no;
    else if (numStrings != DD.numStrings)
      return no;
    else for (State s=1;s<=numStates;s++){
      if (categoryList[s]!=DD.categoryList[s]) return no;
      int * row1=transitTable[s];
      int * row2=DD.transitTable[s];
      int i;
      if (row1==0 && row2!=0){
        for (i=1;i<=numTransits;i++)
           if (row2[i]) return no;
      }
      else if (row1!=0 && row2==0){
        for (i=1;i<=numTransits;i++)
           if (row1[i]) return no;
      }
      else if (row1 && row2){
        for (i=1;i<=numTransits;i++)
           if (row1[i]!=row2[i]) return no;
      }
    }
    return yes;
  };

  Bool operator == ( const FSARep& D ) const { 
    return equalDFSA(D);
 };

  void minimize() {};
  Integer sizeLanguage() const ;
  Bool finiteLanguage() const { return (sizeLanguage()!= -1);} ;
  virtual void printAccepting(ostream &str=cout) const ;
  void printFlags(ostream &str=cout) const ;
  virtual void printStates(ostream &str=cout) const ;
  void printTableDensely(ostream &str=cout) const ;
  void printTableSparsely(ostream &str=cout) const ;

  //Bool acceptedWord(Word w) const { return category(target(1,w));}
  Bool accepts(Word w) const { return category(target(1,w));};
  Bool rejectsInState(Word w, int& state) const {};
  Bool nextAcceptedWord(Word& w) const {};
//  Bool nextAcceptedWord(Word w, int*& history) const {}; //@rn bad arg
  void readFrom(istream &str = cin);
  void printOn(ostream &str = cout) const;
  void GAPprintWord(ostream &str,const Word & w) const;
  void printAlphabet(ostream &str=cout) const ;
  void printWord(ostream &str,const Word & w) const ;

// First we list the functions which do not depend on the alphabet.

// Accessors (methods for accessing data values)

  Chars getName() const { return name;}

  State startState() const { return 1;}  // the start state of the automaton
  State failure() const { return 0;}  // undefined targets are represented by 0   

// Category of state s, usually ACCEPT or NONACCEPT.

  Category category(State s) const { return categoryList[s]; }

  Bool getMinimized() const { return minimized;} // yes if known to be minimised

// The number of states in the automaton
  State getNumStates() const { return numStates; }

// The number of variables/strings- currently always 1 or 2.

  int getNumStrings() const { return numStrings;} 
     // not to be set it once transitions have been defined

// number of alphabet symbols
  int getNumSymbols() const { return numSymbols;} // no write access

// number of possible transitions from a state (equal to the number
// of symbols squared -1 for a 2 string automaton
  int getNumTransits() const { return numTransits;} // no write access

  Chars getPaddingSymbol() const { return paddingSymbol; }



// Target functions.
// These return the state which is the target of an arrow 
// (or a path of arrows) from a given state.
// The automaton may have one or two strings, and so
// labels on arrows are either single symbols or pairs of such.
// An alphabet symbol may be identified by its corresponding
// integer in the range 1..numSymbols or as a Generator.
// A string of symbols labelling a path of arrows
//  is represented by a word.

// For a two string automaton, either (but not both) of the two symbols
// might be  a padding symbol. As an integer, this is represented by 0.
// Hence one but not both of the integer parameters for targetInt could be 0.

// Target functions for one string automata.

  State targetInt( State s,int i) const {
    State * T; return ((s!=0 && (T=transitTable[s])!=0)? T[i] :  0);
  }

// Target functions for a two string automaton

  State targetInt( State s,int i, int j) const {
    State * T; return ((s!=0 && (T=transitTable[s])!=0)? 
               T[i*(numSymbols+1)+j] :  0); }

// Target with padding symbol on 2nd string.

  State targetIntOn1stString(State s,int i) const {
    State * T;
    return ((s!=0 && (T=transitTable[s])!=0)? T[i*(numSymbols+1)] :  0);
  }

// Target with padding symbol on 1st string.

  State targetIntOn2ndString(State s,int j) const {
    State * T; return ((s!=0 && (T=transitTable[s])!=0)? T[j] :  0); }



// Now all the accessors which relate to the alphabet and the way in which
// it is tied to the DFSA. Most are virtual, because they are overwritten
// in a GroupDFSA, where each of the generators also has an inverse.

// The user name for an alphabet symbol.

// the array of symbols

  VectorOf<Chars> getGeneratorNames() const {
    return generatorNames;
  }

// the name of a symbol

  int getNumGenerators() const { return generatorNames.length();}

  virtual Chars getSymbolName(Generator g) const { 
    return (generatorNames[ord(g)-1]);
  }

  virtual Chars getSymbolName(int i) const { 
    return (generatorNames[i-1]);
  }

  virtual Generator getSymbol(int i) const {
    Generator g(i);
    return g;
  }

  virtual int getPosition(Generator a) const {
    return ord(a);
  }


// Target functions for one string automata.

  virtual State target( State s,Generator a) const {
    return targetInt(s,ord(a));
  }

  virtual State target( State s,Word w) const {
    State t=s;
    for (int k=0;k<w.length();k++){
      t = targetInt(t,ord(w[k]));
      if (t==0) break;
    }
    return t;
  }



// Target functions for a two string automaton

  virtual State target( State s,Generator a, Generator b) const {
    return targetInt(s,ord(a),ord(b));
  }

  virtual State targetOn1stString(State s,Generator a) const {
    return targetIntOn1stString(s,ord(a));
  }

  virtual State targetOn2ndString(State s,Generator b) const {
    return targetIntOn2ndString(s,ord(b));
  }

  bool noTransits(State s) const {
    return (transitTable[s]==0);
  }


// methods for setting or resetting  data values

  void setName(const Chars & Name) { name = Name;}

// Reset the number of strings (the default is 1). 
// Warning: if the number of strings is reset, any transition information
// already stored will certainly be lost.
// Such a change is after all a fundamental change to the automaton.
// The only sensible time to change the number of strings of an automaton
// is before it is built.

  void setNumStrings(int numOfStrings) {
    if (numStrings!=numOfStrings){
      clearTransitTable();
      numStrings = numOfStrings;
      numTransits = (numStrings==1? numSymbols : numSymbols * (numSymbols + 2));
      for (State i=0;i<=numStates;i++)
        transitTable.append((State *)0);
// @sr, surely it would be better to first reallocate space for the transitTable,
// and then set all the entries, rather than append entries one by one,
// but that doesn't seem to be possible
    }
  }

// Set or reset the actual number of states in an automaton.
  virtual void setNumStates(int numOfStates);

// Define and return a single new state, and so increase the 
// number of states by one. 

  State newState();

// Functions to set the target of an arrow from a given state.
// The automaton may have one or two strings, and so
// labels on arrows are either single symbols or pairs of such.
// A symbol may be identified directly (in which case the 
// argument is of type Generator) or by its (integer) index.

// For a two string automaton, either (but not both) of the two symbols
// might be padding symbols. 

// Set a target in a 1 string automaton.

  void setTargetInt( State source,int i, State target ) {
    State * T= (transitTable[source]==0 ? 
      (transitTable[source] = newRow()) : transitTable[source]);
    T[i] = target;
  }


// Set a target in a 2 string automaton.

  void setTargetInt
      ( State source,int i, int j, State target ){
    State * T= (transitTable[source]==0 ? 
      (transitTable[source] = newRow()) : transitTable[source]);
    T[i*(numSymbols+1)+j] = target; 
  }

// Set a target in a 2 string automaton, where the padding symbol is read 
// on the second string.

  void setTargetIntOn1stString
       ( State source,int i, State target ){
    State * T= (transitTable[source]==0 ? 
      (transitTable[source] = newRow()) : transitTable[source]);
    T[i*(numSymbols+1)] = target; 
  }

// Set a target in a 2 string automaton, where the padding symbol is read 
// on the first string.

  void setTargetIntOn2ndString
       ( State source, int j, State target ){
    State * T= (transitTable[source]==0 ? 
      (transitTable[source] = newRow()) : transitTable[source]);
    T[j] = target; 
  }

// Set the category of a state.

  void setCategory(State s, Category cat) {  categoryList[s]=cat;}

  void setPaddingSymbol(const Chars & s) {
    paddingSymbol = s;
  }


// Set the boolean flag that indicates an automaton is minimized. 

  void setMinimized(Bool m) { minimized = m;}

// Now all the functions for setting data which are dependent on
// the way the alphabet is interpreted.
// Many of these are virtual because they are overwritten in GroupDFSA

  virtual void setGeneratorNames(const VectorOf<Chars> & names) {
    if (generatorNames!=0)
      cerr <<"Cannot set generator names because they're already set."<<endl;
    generatorNames = names;
    numSymbols = names.length();
    numTransits = (numStrings==1? numSymbols : numSymbols*(numSymbols+2));
  }

// Set a target in a 1 string automaton.

  virtual void setTarget( State source,Generator a, State target ) {
    setTargetInt(source,ord(a),target);
  }

// Set a target in a 2 string automaton.

  virtual void setTarget
          ( State source,Generator a, Generator b, State target ){
    setTargetInt(source,ord(a),ord(b),target);
  }

  virtual void setTargetOn1stString( State source,Generator a,State target ) {
    setTargetIntOn1stString(source,ord(a),target);
  }

  virtual void setTargetOn2ndString
       ( State source, Generator b, State target ) {
    setTargetIntOn2ndString(source,ord(b),target);
  }
  void resetTransitTable() { clearTransitTable(); }
  void clearTransits(State s); 
  void removeDeadState(State s);
  void removeDeadStates(const VectorOf<State> & deadStates);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const;

  void read( istream& istr );


protected:
// The following are protected so that they are accessible to the GroupDFSARep
// class.
  VectorOf<Chars> generatorNames;
  int numSymbols; // number of alphabet symbols 
                  // (excludes padding symbol)
  int numTransits; // equal to numSymbols for a 1 string automaton,
                   // and to numSymbols*(numSymbols+2) for a 2string automaton
  int numStrings; // 1 or 2

private:

  Chars name; // a name for the machine, sometimes useful for printing purposes
  Chars paddingSymbol;
  VectorOf<State *> transitTable; // used for storing transitions
                   // Each row has numTransits entries, indexed from 
                   // 1 to numTransits (NOT from 0),
  VectorOf<Category> categoryList; // used for storing categories.
  int  numStates;    // not including the fail state 0 
  Bool minimized; // true if the automaton is minimized
  State * newRow() const { 
    State * row =  new State[numTransits];
    row--;
    for (int i=1;i<=numTransits;i++) row[i]=0;
    return row; 
  }
  VectorOf<State *> copyTransitTable() const;
  void clearTransitTable();

};

class GroupDFSARep  : public DFSARep {

public:

// constructors

//default constructor
  GroupDFSARep() : DFSARep(),
    symbolForColumn(1), columnForSymbol(1), columnForInvSymbol(1) {
    symbolForColumn[0]=0; columnForSymbol[0]=0; columnForInvSymbol[0]=0;
  }

  GroupDFSARep (const Chars & Name, const VectorOf<Chars>& genNames) :
    DFSARep(Name,0), 
    symbolForColumn(1), columnForSymbol(1), columnForInvSymbol(1) {
    symbolForColumn[0]=0; columnForSymbol[0]=0; columnForInvSymbol[0]=0;
    setGeneratorNames(genNames);}

  GroupDFSARep (const Chars & Name, const VectorOf<Chars>& genNames,
                const WordOrder & word_order) :
    DFSARep(Name,0),
    symbolForColumn(1), columnForSymbol(1), columnForInvSymbol(1) {
    symbolForColumn[0]=0; columnForSymbol[0]=0; columnForInvSymbol[0]=0;
    VectorOf<int> symForCol(1); symForCol[0]=0;
    for (int i=1;i<=word_order.getNumSymbols();i++)
      symForCol.append(word_order.getSymbolIndex(i));
    setGeneratorNames(genNames,symForCol);}


  GroupDFSARep (const Chars & Name, const VectorOf<Chars>& genNames, 
                               int numOfStates, int numOfStrings) :
    DFSARep(Name,0,numOfStates,numOfStrings),
    symbolForColumn(1), columnForSymbol(1), columnForInvSymbol(1) {
    symbolForColumn[0]=0; columnForSymbol[0]=0; columnForInvSymbol[0]=0;
    setGeneratorNames(genNames); }

  GroupDFSARep (const Chars & Name, const VectorOf<Chars>& genNames, 
     const WordOrder & word_order, int numOfStates, int numOfStrings) :
    DFSARep(Name,0,numOfStates,numOfStrings),
    symbolForColumn(1), columnForSymbol(1), columnForInvSymbol(1) {
    symbolForColumn[0]=0; columnForSymbol[0]=0; columnForInvSymbol[0]=0;
    VectorOf<int> symForCol(1);
    symForCol[0]=0;
    for (int i=1;i<=word_order.getNumSymbols();i++)
      symForCol.append(word_order.getSymbolIndex(i));
    setGeneratorNames(genNames,symForCol); }

  GroupDFSARep & operator = (const GroupDFSARep & D) {
    DFSARep& temp = *this;
    temp = (DFSARep)D;
    symbolForColumn=D.symbolForColumn;
    columnForSymbol=D.columnForSymbol;
    columnForInvSymbol=D.columnForInvSymbol;
    return *this;
  }
  FSARep *clone() const { return new GroupDFSARep(*this); }

  Bool operator == ( const GroupDFSARep& D ) const { 
    return (equalDFSA(D) && symbolForColumn==D.symbolForColumn);
  }

// Target functions -see comment above target functions for DFSARep 
// Target functions need to be redefined for a GroupDFSA

// Target functions for one string automata.

  State target( State s,Generator a) const {
    return targetInt(s,(ord(a)>0? columnForSymbol[ord(a)]: 
                                   columnForInvSymbol[-ord(a)]));
  }

  State target( State s,Word w) const {
    State t=s;
    State * T;
    for (int k=0;k<w.length();k++){
      if (t==0) break;
      int i=ord(w[k]);
      t = targetInt(t,(i>0? columnForSymbol[i]: 
                                   columnForInvSymbol[-i]));
    }
    return t;
  }

// Target functions for a two string automaton

  State target( State s,Generator a, Generator b) const {
   return targetInt(s, 
           (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]),
           (ord(b)>0? columnForSymbol[ord(b)]: columnForInvSymbol[-ord(b)]));
  }

// Target with padding symbol on 2nd string.

  State targetOn1stString(State s,Generator a) const {
    return targetIntOn1stString(s,
           (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]));
  }

// Target with padding symbol on 1st string.

  State targetOn2ndString(State s,Generator b) const {
    return targetIntOn2ndString(s,
           (ord(b)>0? columnForSymbol[ord(b)]: columnForInvSymbol[-ord(b)]));
  }

// The user name for an alphabet symbol.

// the name of a symbol

  Chars getSymbolName(Generator g) const { 
    GeneratorIndex i=ord(g);
    if (i>0) return generatorNames[i-1];
    else {
      Chars s = generatorNames[-i-1];
      s += "^-1";
      return s;
    }
  }

  Chars getSymbolName(int i) const { 
    int j=symbolForColumn[i];
    if (j>0) return generatorNames[j-1];
    else {
      Chars s=generatorNames[-j-1];
      s += "^-1";
      return s;
    }
  }

  Generator getSymbol(int i) const { 
    Generator g = symbolForColumn[i];
    return g;
  }

  int getPosition(Generator a) const {
    return
      (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]);
  }

// Returns yes if the inverse generator is not a symbol.
// It doesn't make sense to call this unless ord(g) is positive
// but this is checked to avoid crashes.
  Bool selfInverse(Generator a) const {
    return ( ord(a)>0 && columnForInvSymbol[ord(a)]==0);
  }

// Set a target in a 1 string automaton.

  void setTarget( State source,Generator a, State target ) {
    setTargetInt(source,
       (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]),target);
  }

// Set a target in a 2 string automaton.

  void setTarget
          ( State source,Generator a, Generator b, State target ){
    setTargetInt(source,
       (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]),
       (ord(b)>0? columnForSymbol[ord(b)]: columnForInvSymbol[-ord(b)]), target);
  }

// Set a target in a 2 string automaton, where the padding symbol is read 
// on the second string.

  void setTargetOn1stString( State source,Generator a,State target ) {
    setTargetIntOn1stString(source,
       (ord(a)>0? columnForSymbol[ord(a)]: columnForInvSymbol[-ord(a)]), target);
  }

// Set a target in a 2 string automaton, where the padding symbol is read 
// on the first string.

  void setTargetOn2ndString
       ( State source, Generator b, State target ) {
    setTargetIntOn2ndString(source,
       (ord(b)>0? columnForSymbol[ord(b)]: columnForInvSymbol[-ord(b)]), target);
  }

  void setGeneratorNames(const VectorOf<Chars> & names) {
    if (generatorNames!=0)
      cerr <<"Cannot set generator names because they're already set."<<endl;
    generatorNames = names;
    numSymbols = 2*names.length();
    numTransits = (numStrings==1? numSymbols : numSymbols*(numSymbols+2));
    int j;
    for (j=1;j<=names.length();j++){
       columnForSymbol.append(2*j-1);
       symbolForColumn.append(j);
       columnForInvSymbol.append(2*j);
       symbolForColumn.append(-j);
    }
  }


  void setGeneratorNames(const VectorOf<Chars> & names,
                              const VectorOf<int> & symForCol) {
    if (generatorNames!=0)
      cerr <<"Cannot set generator names because they're already set."<<endl;
    int i,j;

    generatorNames = names;
    numSymbols = symForCol.length()-1;
    numTransits = (numStrings==1? numSymbols : numSymbols*(numSymbols+2));

    for (j=1;j<=numSymbols;j++)
      symbolForColumn.append(symForCol[j]);

    // initialise at zero. Some entries of columnForInvSymbols may remain
    // at zero.

    for (j=1;j<=names.length();j++){
      columnForSymbol.append(0);
      columnForInvSymbol.append(0);
    }

    for (j=1;j<=numSymbols;j++)
       if ((i=symbolForColumn[j])>0) columnForSymbol[i] = j;
       else columnForInvSymbol[-i]=j;

  }


  //Bool acceptedWord(Word w) const { return category(target(1,w));}
  Bool accepts(Word w) const { return category(target(1,w));};
  Bool rejectsInState(Word w, int& state) const {};
  Bool nextAcceptedWord(Word& w) const {};
//  Bool nextAcceptedWord(Word w, int*& history) const {}; //@rn bad arg
  void readFrom(istream &str = cin);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void write( ostream& ostr ) const
  {
    DFSARep::write(ostr);
    ostr < symbolForColumn < columnForSymbol < columnForInvSymbol;
  }

  void read( istream& istr )
  {
    DFSARep::read(istr);
    istr > symbolForColumn > columnForSymbol > columnForInvSymbol;
  }


private:
  // The arrays symbolForColumn, columnForSymbol, columnForInvSymbol
  // describe the relationship between the columns of
  // the transition table and symbols in the alphabet
  // symbolForColumn[i]=j if column no. i corresponds to the generator indexed by j. 
  // If j>0, columnForSymbol[j]=i, and if j<0 columnForInvSymbol[-j]=i.
  // If the generator indexed by j is an involution, and its inverse symbol is
  // not actually in the alphabet, then symbolForInvSymbol[-j] is set to 0.

  VectorOf<int> symbolForColumn; 
  VectorOf<int> columnForSymbol;  
  VectorOf<int> columnForInvSymbol;

};

#endif
