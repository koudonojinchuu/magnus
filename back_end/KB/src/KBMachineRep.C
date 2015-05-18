// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of KBMachineRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// 12/07/94 @rn Rewrote KBMachineRep::sizeLanguage.


#include <iostream.h>
#include <string.h>
#include "Word.h"
#include "KBMachineRep.h"


void KBMachineRep::printOn(ostream &str) const
{
  GroupDFSARep::printOn(str);
  for (int i=1;i<=numRules;i++) {
    GroupDFSARep::printWord(str,lhs[i]);
    str << "->";
    GroupDFSARep::printWord(str,rhs[i]);
    str << endl;
  }
}

void KBMachineRep::oldFormatPrintOn(ostream &str) const
{
  int numOfStates = getNumStates();
  int numOfSymbols = getNumSymbols();
  VectorOf<State> renumber(numOfStates+1);
  renumber[0]=0;
  State s;
  int i;
  int numOfAcceptStates = 0;
  int numOfFailStates = 0;
  for (s=1;s<=numOfStates;s++){
    for (i=1;i<=numOfSymbols;i++) 
      if (targetInt(s,i)!=0) break;
    if (i>numOfSymbols) renumber[s] = -(++numOfFailStates);
    else renumber[s] = ++numOfAcceptStates;
  }
  if (numRules != numOfFailStates){
    cerr << "Error in old format print function for KBMachineRep."<< endl;
    cerr << "Number of rules does not equal number of fail states."<< endl;
  }
    
  str << " fsa {"<< endl;
  str << "\t states "<<numOfAcceptStates<< endl;
  str << "\t symbols "<<numOfSymbols<< endl;
  str << "\t variables 1 "<< endl;
  str << "\t no_eos"<< endl;
  str << "\t alphabet {"<< endl;
  for (i=1;i<=numOfSymbols;i++){
    str << i << " = "<<getSymbolName(i)<<" ";
    if (i%8==0) str << endl;
  }
  str << "}"<<endl;
  str << "\t start { 1 }"<< endl;
  str << endl;
  str << "\%"<< endl;
  str << "atable"<< endl;
  for (s=1;s<=numOfStates;s++){
    if (renumber[s]>0) {
      str << "\t" << renumber[s] <<" A " ;
      for (i=1;i<=numOfSymbols;i++)
        str << " "<<renumber[targetInt(s,i)];
      str << ";"<< endl;
    }
  }
  str << "}"<< endl<< endl;
  str << "rels {"<< endl;
  for (i=1;i<=numRules;i++) {
    if (lhs[i].length()>0)
      GroupDFSARep::printWord(str,lhs[i]);
    else str << "epsilon";
    str << " > ";
    if (rhs[i].length()>0)
      GroupDFSARep::printWord(str,rhs[i]);
    else str << "epsilon";
    str << endl;
  }
  str << "}"<< endl;
}

void KBMachineRep::setNumStates(int numOfStates)
{
  int oldNumStates = getNumStates();
  if (numOfStates>=oldNumStates){
    for (int i=oldNumStates+1;i<=numOfStates;i++){
      length.append(0);
      ruleIndex.append(0);
    }
  }
  else {
    length.shrink(0,numOfStates+1);
    ruleIndex.shrink(0,numOfStates+1);
  }
  DFSARep::setNumStates(numOfStates);
}

  
Bool KBMachineRep::rewrite(Word & w) const
{
  Bool ans=FALSE;
  int historyLen= w.length()+1;
  State * history = new State [historyLen];
  // if the rewrite rules are not length reducing, the length of history 
  // may need to be extended from time to time

  history[0] = startState();
  State s;
  Word w1;
  Word w2 = w;
  int len1, len2, ind;
  int i; 

  while ((len2=w2.length())>0){
    Generator x = w2[0];
    w2 = w2.terminalSegment(len2-1);
    len1 = w1.length();
    s = target(history[len1],x);
    if (category(s)==1){
      if (len1+1 == historyLen) {
        State * oldHistory = history;
        history = new State[2*historyLen];
        for (i=0;i<=len1;i++) history[i] = oldHistory[i];
        delete oldHistory;
        historyLen *= 2;
      }
      history[len1+1] = s;
      w1 *= x;
    }
    else  {
      ans = TRUE;
      ind = getRuleIndex(s);
      w1 = w1.initialSegment(len1+1 - lhs[ind].length());
      // w1 is the prefix of w1*x formed by deleting the terminal subword
      // equal to lhs[ind].
      w2 = rhs[ind]*w2;
    }
  }
  delete history; // @rn
  w = w1;
  return ans;
}

