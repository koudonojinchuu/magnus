// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of GenMultRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#include <iostream.h>
#include <string.h>
#include "List.h"
#include "GenMultRep.h"
#include "DFSAParser.h"

void GenMultRep::readFrom(istream &str)
{
  DFSAParser P(str,WORDS,YES);
  P.parseDFSA(this);
  P.setDFSA((GroupDFSARep *)this); // need to make sure we use the 
                              // GroupDFSARep version of the function
  ListOf<int> index= P.getStateLabelsIndex();
  VectorOf<Chars> labels = P.getStateCharsLabels();
  int N = getNumGenerators();
  int NN=getNumSymbols();
  if (labels.length() != NN+2){ // 0 position is empty
    cerr << "Error reading multiplier."<< endl;
    cerr << "Wrong number of distinct state labels."<< endl;
    return;
  }

  int * genIndex = new int [NN+2];
  Chars lab;
  Chars sym;
  int i,j;
  for (i=1;i<=NN+1;i++){
    lab = labels[i]; 
    sym = getPaddingSymbol();
    if (lab==sym){ 
      genIndex[i]=0;
      break;
    }
    for (j=1;j<=N;j++){
      Generator g(j);
      sym = getSymbolName(g);
      if (lab==sym){
        genIndex[i]=j;
        break;
      }
      else {
        sym += "^-1";
        if (lab==sym){
          genIndex[i]= -j;
          break;
        }
      }
    }
    if (j>N) cerr << "Unrecognised state label."<< endl;
  }

// initially all categories have been set to zero
  int M = getNumStates();
  for (i=0;i<=M;i++) mults.append(0);

  ListIterator< ListOf <int> > I(index);
  while (!I.done()){
    i=I.value();
    I.next();
    if (I.done()){
      cerr << "List should have an even number of entries,"<< endl;
      cerr << "Couldn't set multipliers in GenMult::readFrom."<< endl;
    }
    else {
      setCategory(i,1); 
      mults[i]=genIndex[I.value()];
      I.next();
    }
  }
  delete genIndex;
}

void GenMultRep::printAccepting(ostream &str) const 
{
  str << "       accepting := []";
}

void GenMultRep::printStates(ostream &str) const
{
  int numOfStates = getNumStates();
  int numOfGens = getNumGenerators();
  int i,j;
  str << "          states := rec("<< endl;
  str << "                type := \"labeled\","<< endl;
  str << "                size := "<<numOfStates<<","<< endl;
  str << "                labels := rec("<< endl;
  str << "                      type := \"identifiers\","<< endl;
  str << "                      size := "<< 2*numOfGens+1<< "," << endl;
  str << "                      format := \"dense\","<< endl;
  str << "                      names := [";
  for (i=1;i<=numOfGens;i++) {
    Generator g(i); 
    str << getSymbolName(g)<<","<<getSymbolName(inv(g))<<",";
  }
  str << getPaddingSymbol() << "]";
  str << endl<<"               ),"<<endl;
  str << "          format := \"sparse\","<<endl;
  str << "     setToLabels := ["<<endl;
  Bool seenfirst=NO;
  for (i=1;i<=numOfStates;i++) {
    if (category(i)){
      if (j=mults[i]){
        if (seenfirst) str <<","<<endl;
        str << "\t["<<i<<","<< (j!=0? getPosition((Generator)j):0)<<"]";
      }
      else{
        if (seenfirst) str <<","<<endl;
        str << "\t["<<i<<","<< 2*numOfGens+1<<"]";
      }
      seenfirst=YES;
    }
  }
  str << endl << "              ]"<< endl;
  str << "               )";
}

void GenMultRep::setNumStates(int numOfStates)
{
  int oldNumStates = getNumStates();
  if (oldNumStates<=numOfStates){
    for (int i=oldNumStates+1;i<=numOfStates;i++){
      mults.append(0); // default value, meaningless if
                       // category is 0
    }
  }
  else mults.shrink(0,numOfStates+1);
  DFSARep::setNumStates(numOfStates);
}


