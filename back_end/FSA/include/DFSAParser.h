// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of DFSA parser class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _DFSA_PARSER_H_
#define _DFSA_PARSER_H_

#include <iostream.h>
#include <iomanip.h>
//#include <stdiostream.h>
#include "global.h"
#include "DFSARep.h"


enum { SIMPLE=0, WORDS=1, LABELED=2}; // types for states

class DFSAParser {
public:

  DFSAParser(istream  &istr,int stype,Bool useInv) : 
         name(""),
         str(istr),
	 parses(1),
	 table(0),
	 alphabet(0),
         baseAlphabet(0),
         stateAlphabet(0),
         symbolOrder(1), 
         symbolForColumn(1),
	 stateWordLabels(0),
	 stateCharsLabels(0),
// use the default constructor for stateLabelsIndex. Initialising on 0 puts
// the integer 0 as first entry of the list
	 numStates(0),
	 numSymbols(0),
         numTransits(0),
	 numStrings(0),
         minimized(NO),
         stateType(stype),
         useInverses(useInv)
  { symbolOrder[0]=0; symbolForColumn[0]=0;}

  ~DFSAParser() {
	 delete [] table;
  }

  void parseError(char * errormessage);
  void setDFSA(DFSARep * M);
  void setDFSA(GroupDFSARep * M);
  ListOf<int> getStateLabelsIndex() const {return stateLabelsIndex;};
  VectorOf<Chars> getStateCharsLabels() const {return stateCharsLabels;};
  VectorOf<Word> getStateWordLabels() const {return stateWordLabels;};
  void setAlphabet(DFSARep * M);
  void setAlphabet(GroupDFSARep * M);
  void setFlags(DFSARep * M);
  void setData(DFSARep * M);
  Bool parseDFSA(DFSARep * M);
  void parseAlphabetRec();
  void matchAlphabet(DFSARep * M);
  void matchAlphabet(GroupDFSARep * M);
  void parseStatesRec();
  void parseFlags();
  void parseAccepting();
  void parseTableRec();
  void setStateAlphabet(DFSARep * M);
  void parseStateLabelsRec();
  void parseStateWordLabels();

  
private:
  typedef DFSARep::State State;
  typedef DFSARep::GeneratorIndex GeneratorIndex;

// The basis of a DFSAParser is its table, which is simply an int **.
// The states are consecutive integers 1,2,3,.. numstates, with 0 as the
// fail state.  The states index the columns and the alphabet symbols the
// rows of the array.  The 0-th row is used to store the categories of
// the states.

  int ** table; 
  Chars name;
  VectorOf<Chars> alphabet;
  VectorOf<Chars> baseAlphabet;
  VectorOf<Chars> stateAlphabet;
  VectorOf<int> symbolOrder;
  VectorOf<int> symbolForColumn;
  Chars paddingSymbol;
  int stateType; // equal to SIMPLE, WORDS or LABELED
  VectorOf<Word> stateWordLabels;
  VectorOf<Chars> stateCharsLabels;
  ListOf<int> stateLabelsIndex;
  int numStates;  // Not including the fail state 0.
  int numSymbols; // The alphabet size.
  int numTransits; // number of transitions (essentially the square of the 
    // alphabet size for a 2 string automaton )
  int numStrings;
  Bool minimized;
  Bool useInverses;
  istream& str;
  Bool getInt(int &);
  Bool readToToken();
  Bool getToken(int &);
  Bool getToken(Chars &);
  Chars getToken();
  Bool checkToken(const char * token);
  Bool checkToken(const char ch);
  void parseTable(Bool compressed);
  Bool parses;
};

#endif
