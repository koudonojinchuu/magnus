// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of DFSARep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//
// * 08/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "DFSARep.h"
#include "DFSAParser.h"

typedef DFSARep::State State;

VectorOf<State *> DFSARep::copyTransitTable() const
{
  VectorOf<State *> copy(numStates+1);
  for (State s=0;s<=numStates;s++){
    State * row = transitTable[s];
    if (row){
      State * copyrow = newRow();
      for (int i=1;i<=numTransits;i++)
        copyrow[i] = row[i];
      copy[s] = copyrow;
    }
    else copy[s] = row;
  }
  return copy;
}

void DFSARep::clearTransits(State s)
{
  State * row = transitTable[s];
  if (row){
    row++;
    delete row;
    transitTable[s]=(State *)0;
  }
}

void DFSARep::clearTransitTable()
{
  for (State s=0;s<=numStates;s++){
    State * row = transitTable[s];
    if (row){
      row++;
      delete row;
    }
  }
  if (transitTable.length()>1)
    transitTable.shrink(0,1);
  numStates = 0;
}

void DFSARep::removeDeadState(State s)
// A dead state is removed. There must be no transitions into it, or its
// deletion will cause trouble.
{
  State * row = transitTable[s];
  if (row){
    row++;
    delete row;
  }
  State t;
  for (t=s+1;t<=numStates;s++){
    transitTable[t-1] = transitTable[t];
    categoryList[t-1] = categoryList[t];
  }
  numStates--;
  for (t=1;t<=numStates;t++){
    row = transitTable[t];
    if (row){
      for (int i=1;i<=numTransits;i++)
        if (row[i]>s) row[i]--;
    }
  }
  transitTable.shrink(0,numStates+1);
  categoryList.shrink(0,numStates+1);
}

void
DFSARep::setNumStates(int numOfStates)
{
  if (numOfStates>=numStates) 
    for (State i=numStates+1;i<=numOfStates;i++){
      transitTable.append((State *)0);
      categoryList.append(0);
    }
  else {
    for (State s=numOfStates+1;s<=numStates;s++){
      int * row = transitTable[s];
      if (row){
        row++;
        delete row;
      }
    }
    transitTable.shrink(0,numOfStates+1);
    categoryList.shrink(0,numOfStates+1);
  }
  numStates = numOfStates;
}

 
State DFSARep::newState() 
{
  setNumStates(numStates+1);
  return numStates;
}


void DFSARep::printStates(ostream &str) const
{
  str << "          states := rec("<< endl;
  str << "                type := \"simple\","<< endl;
  str << "                size := "<< getNumStates() << endl;
  str << "               )";
}


void DFSARep::printFlags(ostream & str) const
{
  str << "           flags := [\"DFA\"";
  if (getMinimized())
    str << ",\"minimized\""; 
  str << "]"; 
}

void DFSARep::printAccepting(ostream & str) const
{
  str << "       accepting := [";
  int k=0;
 
  State s,t= -1; // records the last accept state
  int numOfStates = getNumStates();

  for (s=1;s<=numOfStates;s++){
    if (category(s)!=1) break;
  }
  if (s==numOfStates+1) str << 1<<".."<<numOfStates;
  else { 
    for (s=1;s<=numOfStates;s++){
      if (category(s)==1){
        if (k>0) { 
          str << ",";
          if (k%10==0) str << endl << "        ";
        }
        str << s;
        k++;
      }
    }
  }
  str << "]";
}

void DFSARep::printTableDensely(ostream &str) const
{
  int numOfStates = getNumStates();
  int numOfSymbols = getNumSymbols();
  int numOfTransits = getNumTransits();
  State s,t;
  int i,j;
  str << "           table := rec("<< endl;
  //@sr Unfortunately it seems that the total number of transitions needs
  // to be printed out. What a drag!
  int ct=0;
  for (s=1;s<=numOfStates;s++)
    for (i=1;i<=numOfTransits;i++) 
      if (targetInt(s,i)!=0) ct++;
  str << "      numTransitions := " << ct << "," << endl;
  str << "              format := \"dense deterministic\","<< endl;
  str << "         transitions := [";
  for (s=1;s<=numOfStates;s++){
    if (s>1) str <<"                         ";
    str << "[";

    if (getNumStrings()==1){

      for (i=1;i<=numOfSymbols;i++){
        if (i>1) str << ",";
        if (i%30==0) str <<endl<<"                          ";
        str << targetInt(s,i) ;
      }
    }

    else { // 2 strings

      for (i=1;i<=numOfSymbols;i++){
        for (j=1;j<=numOfSymbols;j++){
          if (i>1||j>1) str << ",";
          if (i%7==0) str <<endl<<"                          ";
          str << targetInt(s,i,j);
          str << ","<< targetIntOn1stString(s,i) ;
        }
      }
      for (j=1;j<=numOfSymbols;j++){
        str << ","<< targetIntOn2ndString(s,j) ;
      }
    }
    if (s==numOfStates) str << "]"<<endl;
    else str << "],"<<endl;
  }
  str << "                   ]"<<endl;
  str << "               )";
}

void DFSARep::printTableSparsely(ostream &str) const
{
  int numOfStates = getNumStates();
  int numOfTransits = getNumTransits();
  int numOfSymbols = getNumSymbols();
  State s,t;
  int i,j;
  str << "           table := rec("<< endl;
  //@sr Unfortunately it seems that the total number of transitions needs
  // to be printed out. What a drag!
  int ct=0;
  for (s=1;s<=numOfStates;s++)
    for (i=1;i<=numOfTransits;i++) 
      if (targetInt(s,i)!=0) ct++;
  str << "      numTransitions := " << ct << "," << endl;
  str << "              format := \"sparse\","<< endl;
  str << "         transitions := [";
  for (s=1;s<=numOfStates;s++){
    if (s>1) str <<"                         ";
    str << "[";
    int k=0;
    int count=0;

    if (getNumStrings()==1){

      for (i=1;i<=numOfSymbols;i++){
        k++;
        if (t=targetInt(s,i)!=0){
          if (count>0){ 
            str << ",";
            if (count%7==0) str <<endl<<"                          ";
          }
          count++;
          str <<  "[" << k << ","<<t<<"]";
        }
      }
    }

    else { // 2 strings

      for (i=1;i<=numOfSymbols;i++){
        for (j=1;j<=numOfSymbols;j++){
          k++;
          if ((t=targetInt(s,i,j))!=0){ 
            if (count>0) str << ",";
            count++;
            if (count%7==0) str <<endl<<"                          ";
            str <<  "[" << k << ","<<t<<"]";
          }
        }
        k++;
        if ((t=targetIntOn1stString(s,i))!=0){ 
          if (count>0) str << ",";
          count++;
          if (count%7==0) str <<endl<<"                          ";
          str <<  "[" << k << ","<<t<<"]";
        }
      }
      for (j=1;j<=numOfSymbols;j++){
        k++;
        if ((t=targetIntOn2ndString(s,j))!=0){ 
          if (count>0) str << ",";
          count++;
          if (count%7==0) str <<endl<<"                          ";
          str <<  "[" << k << ","<<t<<"]";
        }
      }

    }
    if (s==numOfStates) str << "]"<<endl;
    else str << "],"<<endl;
  }
  str << "               ]"<<endl;
  str << "               )";
}


void DFSARep::readFrom(istream &str) 
{  
    DFSAParser P(str,SIMPLE,NO);
    if (P.parseDFSA(this))  
      P.setDFSA(this);
    else cerr << "DFSA wouldn't parse."<< endl;
}

void GroupDFSARep::readFrom(istream &str) 
{  
    DFSAParser P(str,SIMPLE,YES);
    if (P.parseDFSA(this))  
      P.setDFSA(this);
    else cerr << "DFSA wouldn't parse."<< endl;
}

void DFSARep::printWord(ostream &str,const Word & w) const
{
  int len = w.length();
  if (len!=0){
    str << getSymbolName(w[0]);
    for (int i=1;i<len;i++){
      if (i%30==0) str << endl;
      str << '*';
      str << getSymbolName(w[i]);
    }
  }
  else str << "1";
  str << ' ';
}

void DFSARep::printOn(ostream &str) const
{

  int i,j, ne;
  State s,t;
  Chars name = getName();
  if (name==Chars("")) name = "unnamed";
  
  int numOfStates = getNumStates();

  str << name << " := rec(" << endl;
  str << "           isFSA := true," << endl;
  printAlphabet(str);
  str << ","<< endl;
  printStates(str); 
  str << ","<< endl;
  printFlags(str);  
  str << ","<< endl;
  str << "         initial := [1],"<< endl;
  printAccepting(str); 
  str << ","<< endl;
  if (getNumStrings()==1) printTableDensely(str);
  else printTableSparsely(str);
  str << endl <<");"<<endl;
}

void DFSARep::printAlphabet(ostream &str) const
{
  int numOfSymbols = getNumSymbols();
  int numOfTransits = getNumTransits();
  str << "        alphabet := rec(" << endl;
  if (getNumStrings()==1){
    str << "                type := \"identifiers\","<< endl;
    str << "                size := "<< numOfSymbols << "," << endl;
    str << "                  format := \"dense\","<< endl;
//@sr - don't know what dense means - should check.
    str << "                   names := [";
    for (int i=1;i<=numOfSymbols;i++){
      str << getSymbolName(i);
      if (i<numOfSymbols) str << ",";
      else str << "]" << endl;
    } 
  }
  else {
    str << "                type := \"product\","<< endl;
    str << "                size := "<< numOfTransits << "," << endl;
    str << "               arity := "<< "2," << endl;
    str << "             padding := "<<  getPaddingSymbol() << "," << endl;
    str << "                base := rec(" << endl;
    str << "                    type := \"identifiers\","<< endl;
    str << "                    size := "<< numOfSymbols <<","<< endl;
    str << "                  format := \"dense\","<< endl;
//@sr - don't know what dense means - should check.
    str << "                   names := [";
    for (int i=1;i<=numOfSymbols;i++){
      str << getSymbolName(i);
      if (i<numOfSymbols) str << ",";
      else str << "]" << endl;
    } 
    str << "                   )"<< endl;
  }
  str << "               )";
}

void DFSARep::GAPprintWord(ostream &str,const Word & w) const
{
  int len = w.length();
  Generator g;
  if (len!=0){
    g=w[0];
    str << getSymbolName(g);
    for (int i=1;i<len;i++){
      str << '*';
      if (i%30==0) str<< "\\" << endl;
      g=w[i];
      str << getSymbolName(g);
    }
  }
  else str << "IdWord";
}


Integer DFSARep::sizeLanguage() const
{
// This both determines whether the language is infinite, and counts the
// size of the language when it is finite.
// The counting is done by the obvious depth-first traversal, storing
// local counts for re-use. See the comment for `counts', below.
// To detect infiniteness at the same time, we use the graph acyclicity
// test of Aho, Hopcroft, Ullman, ``Data Structures and Algorithms'', pp.
// 217--221. The supporting data structues are `ids' and `desc'.


  int numberOfStates = getNumStates();
  int numOfTransits = getNumTransits();
  if (numOfTransits==0) return category(startState());  
  // If numOfTransits = 0 at most the emtpy string can be accepted.
  // The code below assumes that numTransits is non-zero

  int* ids = new int[numberOfStates + 1];
  if ( ids == NULL ) error("out of memory in DFSARep::sizeLanguage");
  for( int i = 1; i <= numberOfStates; ++i ) ids[i] = 0;
  ids[startState()] = 1;
  // We number the vertices as we do a depth-first traversal. A `back-edge'
  // is one which goes from s to t, where s has higher number than t, and
  // and s is a descendant of t in the depth-first spanning tree (see below).
  // Since all states are accept or fail states, the language is infinite
  // iff we encounter a back-edge in the traversal.
  // This does double-duty: ids[s] == 0 => state s is unvisited.

  int id_count = 1;

  int* desc = new int[numberOfStates + 1];
  if ( desc == NULL ) error("out of memory in DFSARep::sizeLanguage");
  for( int i = 1; i <= numberOfStates; ++i ) desc[i] = 0;
  // This vector stores the number of descendants of each state in the
  // depth-first spanning tree.

  Integer* counts = new Integer[numberOfStates + 1];
  if ( counts == NULL ) error("out of memory in DFSARep::sizeLanguage");
  for( int i = 1; i <= numberOfStates; ++i ) counts[i] = 1;
  // At each state, we recursivly count the number of paths from that state
  // during our traversal. Then we record that number here, so that if we
  // visit this state again, we don't have to recount! This saves oodles
  // of time. We start with 1, since all states are accept states.

  // We're doing recursion by iteration (tricky -- it's not tail recursion),
  // so we need an explicit activation stack:

  struct ActivationRecord {
    State s, t;
    int g;
  };

  ActivationRecord* stack = new ActivationRecord[numberOfStates + 1];
  if ( stack == NULL ) error("out of memory in DFSARep::sizeLanguage");
  int stackCounter = 0;

  State s, t;
  // Some scratch variables

  // Push the first `call' on the stack:

  stack[0].s = startState();
  stack[0].g = 1;

  State result_s = 0;
  // Each pass through the loop represents a recursive function call, with
  // the top of the stack holding the argument s and local variables t, g,
  // or the return from a call, in which case result_s != 0, and is the
  // index in counts of the `return value'.

  // The first depth-first traversal merely sets the values of desc.
  // @rn I don't see how to do this except by a separate pass.

  do {
    s = stack[stackCounter].s;

    if ( result_s ) {
      result_s = 0;
      desc[s] += 1 + desc[stack[stackCounter].t];

    } else {
      
      t = targetInt(s, stack[stackCounter].g);
      if ( category(t) == 1 ) {
        if ( !ids[t] ) {
          ids[t] = ++id_count;
          // Set up for `recursive call'
          stack[stackCounter].t = t;
          stack[++stackCounter].s = t;
          stack[stackCounter].g = 1;
          continue;                           // Make the call
        }
      }
    }
    int& g = stack[stackCounter].g;
    ++g;
    if ( g > numOfTransits ) {
      // Now we `return' from this call.
      result_s = s;
      --stackCounter;
    }
  } while ( stackCounter >= 0 );

  // Now we make the real depth-first traversal.

  stackCounter = 0;
  stack[0].s = startState();
  stack[0].g = 1;
  result_s = 0;
  for( int i = 1; i <= numberOfStates; ++i ) ids[i] = 0;
  // We need to reset these since they indicate `visited' status.

  do {
    s = stack[stackCounter].s;

    if ( result_s ) {
      counts[s] += counts[result_s];
      result_s = 0;

    } else {
      
      t = targetInt(s, stack[stackCounter].g);
      if ( category(t) == 1 ) {
        if ( ids[t] ) {
          // Page 218 of text:
          if ( (ids[s] >= ids[t]) && (ids[s] <= ids[t] + desc[t]) ) {
            counts[startState()] = -1;
            break;
          }
          counts[s] += counts[t];
        } else {
          ids[t] = ++id_count;
          // Set up for `recursive call'
          stack[stackCounter].t = t;
          stack[++stackCounter].s = t;
          stack[stackCounter].g = 1;
          continue;                           // Make the call
        }
      }
    }
    int& g = stack[stackCounter].g;
    ++g;
    if ( g > numOfTransits ) {
      // Now we `return' from this call.
      result_s = s;
      --stackCounter;
    }
  } while ( stackCounter >= 0 );

  Integer result = counts[startState()];
  delete [] ids;
  delete [] desc;
  delete [] counts;
  delete [] stack;

  return result;
}


void DFSARep::write( ostream& ostr ) const
{
  ostr < generatorNames < numSymbols < numTransits < numStrings < name 
       < paddingSymbol < categoryList < numStates < minimized;

  ostr < "A";

  for( int i = 0; i <= numStates; ++i )
    if( transitTable[i] )
      ostr < (int)1;
    else
      ostr < (int)0;

  ostr < "B";
  
  for( int i = 0; i <= numStates; ++i ) {
    State *row = transitTable[i];
    if( row )
      for( int j = 1; j <= numTransits; j++ )
	ostr < (State)(row[j]);
  }

}


void DFSARep::read( istream& istr ) 
{ 
//  clearTransitTable();

  istr > generatorNames > numSymbols > numTransits > numStrings > name 
       > paddingSymbol > categoryList > numStates > minimized;

  char c;
  istr > c;
  
  VectorOf<int> T(numStates+1);
  for( int i = 0; i <= numStates ; ++i ) {
    int k;
    istr > k;
    T[i] = k;
  }

  istr > c;

  int len = transitTable.length();
  
  for( int i = 0; i < len; ++i ) {
    State *s = transitTable[i];
    if( s ) {
      s++;
      delete s;
    }
  }

  transitTable = VectorOf<State*>(numStates+1,true);

  for( int i = 0; i <= numStates; ++i )
    if( T[i] ) {
      State *newrow = newRow();
      for( int j = 1; j <= numTransits; ++j ) {
	State s;
	istr > s;
	newrow[j] = s;
      }
      transitTable[i] = newrow;
    }
    else
      transitTable[i] = (State*)0;
}
