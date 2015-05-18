// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of DFSA parser class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 8/96 Dmitry B. made porting to gcc 2.7.2.
//

#include <iostream.h>
#include <string.h>
#include "DFSAParser.h"
#include "Chars.h"
#include "WordParser.h"


void DFSAParser::parseError(char * message)
{
  cerr << message << endl;
  parses = 0;
}

Bool DFSAParser::getInt(int & i)
{
  int ch;
  while (((ch = str.peek()) != EOF) && ch!=']'){
    if (ch == '#'){
      while (((ch = str.get()) != EOF) && ch != '\n'){
        if (ch == EOF){
          parseError("End of file reached unexpectedly.");
          return 0;
        }
      }
    }
    else if (ch=='-' || isdigit(ch)){  str >> i ; return 1;}
    else ch = str.get();
  }
  if (ch == EOF) parseError("End of file reached unexpectedly.");
  return 0;
}

Bool DFSAParser::readToToken()
{
  int ch;
  while ((ch = str.peek()) != EOF){
    if (ch == '#'){
      while (((ch = str.get()) != EOF) && ch != '\n'){
        if (ch == EOF){
          parseError("End of file reached unexpectedly.");
          return 0;
        }
      }
    }
    else if (!isspace(ch)) return 1; 
    else ch = str.get(); 
  }
  if (ch == EOF) parseError("End of file reached unexpectedly.");
  return 0;
}

Bool DFSAParser::getToken(int & ch)
{
  if (readToToken()){ ch = str.get(); return 1;}
  else return 0;
}

Bool DFSAParser::getToken(Chars & buf)
{
  int len = 0;
  int ch;
  Bool quotes=0;
  buf="";
  
  if (readToToken()){
    
    while (((ch = str.peek()) != EOF) && parses){
      if (len>0 && 

         (ch == '#'  // comment symbol   

        || ch==','|| ch==':'|| ch=='['||ch==']' || ch=='(' || ch==')'
        // each of the above marks the beginning of a new token

        || (quotes==0 && isspace(ch))))
       // a space marks the end of a token if not in quotes

           break;

      else {
        ch=str.get();
        if (ch=='\\'){ 
          while ((ch = str.get()) != EOF && ch!='\n'); 
          continue;
      // a backslash masks a return, so we read to the next line
        }
        if (ch=='\"') quotes = 1-quotes;
        buf += (char)ch;
        len++;

        if (ch==','|| ch=='('|| ch==')'|| ch=='['||ch==']' ||ch=='=' ) 

        // each of the above marks the end of a token

          break;
      }
    }
  }
  if (ch == EOF){
    parseError("End of file reached in middle of reading token.");
    buf="";
    return 0;
  }
  else {
    return 1;
  }
}

Chars DFSAParser::getToken()
{
  Chars buf;
  getToken(buf);
  return buf;
}
  

Bool DFSAParser::checkToken(const char * token)
{
  int i=0;
  int ch,cch;
  
  if (readToToken()){
    while ((cch=token[i++])!='\0' && parses){ 
      ch= str.get();
      if (ch == EOF){
        parseError("End of file reached in middle of reading token.");
      }
      else if (ch!=cch){
        parseError("Token not as expected.");
        cerr << "Read "<<(char)ch << " not "<< (char)cch << " as expected from token "<<token<< endl;
      } 
    }
  }
  if (parses) return 1;
  else return 0; 
}

Bool DFSAParser::checkToken(const char token)
{
  int ch;
  if (getToken(ch) && ch==(int)token) return 1;
  else return 0;
}

void DFSAParser::setDFSA(DFSARep * M)
{
  setAlphabet(M);
  if (parses) {
    M->resetTransitTable();
    setFlags(M);
    setData(M);
  }
}

void DFSAParser::setDFSA(GroupDFSARep * M)
{
  setAlphabet(M);
  if (parses) {
    M->resetTransitTable();
    setFlags(M);
    setData(M);
  }
}

void
DFSAParser::setAlphabet(DFSARep * M)
{
  if (M->getNumSymbols()==0){
    // No alphabet has been specified so far.
    M->setGeneratorNames(alphabet);
    for (int i=1;i<=numSymbols;i++) symbolOrder.append(i);
  }
  else matchAlphabet(M);
  M->setPaddingSymbol(paddingSymbol);
}

void
DFSAParser::setAlphabet(GroupDFSARep * M)
{
  if (M->getNumSymbols()==0){
    // No alphabet has been specified so far.
    M->setGeneratorNames(baseAlphabet,symbolForColumn);
    for (int i=1;i<=numSymbols;i++) symbolOrder.append(i);
  }
  else matchAlphabet(M);
  M->setPaddingSymbol(paddingSymbol);
}

void
DFSAParser::matchAlphabet(DFSARep * M)
{
  int numOfSymbols = M->getNumSymbols();
  if (numSymbols != numOfSymbols)
    parseError(
      "Input alphabet has different size from that originally specified.");
  else for (int i=1;i<=numSymbols;i++){
    int j;
    for ( j=1;j<=numOfSymbols;j++){
      if (alphabet[i-1]==M->getSymbolName(j)){ // alphabet is indexed from 0
        symbolOrder.append(j);
        break;
      }
    }
    if (j>numOfSymbols) parseError("Alphabet symbols don't match.");
  }
}

void
DFSAParser::matchAlphabet(GroupDFSARep * M)
{
  int numOfSymbols = M->getNumSymbols();
  if (numSymbols != numOfSymbols)
    parseError(
      "Input alphabet has different size from that originally specified.");
  else for (int i=1;i<=numSymbols;i++){
    int j;
    for ( j=1;j<=numOfSymbols;j++){
      if (alphabet[i-1]==M->getSymbolName(j)){ // alphabet is indexed from 0
        symbolOrder.append(j);
        break;
      }
    }
    if (j>numOfSymbols) parseError("Alphabet symbols don't match.");
  }
}

void DFSAParser::setFlags(DFSARep * M)
{ 
  M->setName(name);
  M->setNumStrings(numStrings);
  M->setNumStates(numStates);
  M->setMinimized(minimized);
}

void DFSAParser::setData(DFSARep * M)
{
  State s;
  int i,j;
  int k,l;

  for (s = 1;s <= numStates;s++) M->setCategory(s,table[s][0]);

  if (numStrings==1)
    for (s = 1;s <= numStates;s++){
      for (i = 1;i<=numSymbols;i++){
        M->setTargetInt(s,symbolOrder[i],table[s][i]);
      }
    }
  else {
    for (s = 1;s <= numStates;s++) { 
      for (i = 1;i<=numSymbols;i++){
        k = symbolOrder[i]; 
        for (j = 1;j<=numSymbols;j++)
          M->setTargetInt(s,k,symbolOrder[j],table[s][(i-1)*(numSymbols+1)+j]);
        M->setTargetIntOn1stString(s,k,table[s][(i-1)*(numSymbols+1)+j]);
      }
      for (j = 1;j<=numSymbols;j++)
        M->setTargetIntOn2ndString
              (s,symbolOrder[j],table[s][(i-1)*(numSymbols+1)+j]);
    }
  }
}

Bool DFSAParser::parseDFSA(DFSARep * M)
{
  Chars buf;
  int i;
  
  name = getToken();
  if (parses && checkToken(":=") && checkToken("rec") && checkToken('(')
   && checkToken("isFSA") && checkToken(":=") && checkToken("true")
   && checkToken(',')){
    while (getToken(buf) && parses == 1 && buf!=Chars(")")){
      if (buf==Chars(",")) continue; // "," separates record entries
      else if (buf==Chars("alphabet")){
        parseAlphabetRec();
      }
      else if (buf==Chars("states")){
        setStateAlphabet(M);
        parseStatesRec();
      }
      else if (buf==Chars("flags"))
        parseFlags();
      else if (buf==Chars("initial")){
        if ((checkToken(":=")&& checkToken('[') && getInt(i) && i==1
              && checkToken(']') && checkToken(','))==0)
          parseError("Cannot read this FSA. Initial state is not set to 1");
      }
      else if (buf==Chars("accepting"))
        parseAccepting();
      else if (buf==Chars("table"))
        parseTableRec();
    }
    checkToken(';');
  }
  return parses;
}

void 
DFSAParser::parseAlphabetRec()
{
  Chars buf;
  int i,j;
  if (checkToken(":=") && checkToken("rec") && checkToken('(')){
    while (parses && getToken(buf) && buf!=Chars(")")){
      if (buf==Chars(",")) continue; // "," separates record entries
      else if (buf==Chars("type")){ 
        if (checkToken(":=") && getToken(buf)){
          if (buf==Chars("\"identifiers\"")) numStrings=1; 
          else if (buf==Chars("\"product\"")){ 
          }
          else 
            parseError("Alphabet should be of type \"identifiers\" or \"product\"");
        }
      }
      else if (buf==Chars("size")){
        if ((checkToken(":=") && getInt(numTransits))==0)
          parseError("Format error after \"size\" keyword.");
      }
      else if (buf==Chars("names")){
        checkToken(":=");
        numSymbols= numTransits;
        checkToken('[');
        for (i=1;i<=numTransits;i++){
          alphabet.append(getToken());
          if (i<numTransits) checkToken(',');
        } 
        checkToken(']');
      }
      else if (buf==Chars("arity")){
        if (checkToken(":=") && getInt(numStrings)){
          if (numStrings!=2) parseError("only 1 or 2 strings allowed."); 
        }
        else parseError("Format error after \"arity\" keyword.");
      }
      else if (buf==Chars("format")){
        if (checkToken(":=")==0 || checkToken("\"dense\"")==0) 
          parseError("Format error after \"format\" keyword.");
      }
      else if (buf==Chars("padding")){
        if (checkToken(":=")) paddingSymbol = getToken();
        else parseError("Format error after \"padding\" keyword.");
      }
      else if (buf==Chars("base")){
        if (checkToken(":=") && checkToken("rec") && checkToken('(')){
          while (parses && getToken(buf) && buf!=Chars(")")){
            if (buf==Chars("type")){ 
              if (checkToken(":=")==0 ||getToken(buf)==0
                || buf!=Chars("\"identifiers\""))
             parseError("Base should be of type \"identifiers\".");
            }
            else if (buf==Chars("size")){ 
              if (checkToken(":=") && getInt(numSymbols)){
                checkToken(',');
                if (numTransits != numSymbols*(numSymbols+2))
                  parseError("Inconsistent entry for size");
              }
              else parseError("Format error after \"size\" keyword.");
            }
            else if (buf==Chars("format")){
              if (checkToken(":=") && checkToken("\"dense\"")) 
                checkToken(',');
              else parseError("Format error after \"format\" keyword.");
            }
            else if (buf==Chars("names")){
              checkToken(":=");
              checkToken('[');
              for (i=1;i<=numSymbols;i++){
                alphabet.append(getToken());
                if (i<numSymbols) checkToken(',');
              } 
              checkToken(']');
              if (parses==0)
                parseError("Format error after \"names\"keyword.");
            }
          }
        }
        else parseError("Parse error after \"base\" keyword.");
      }
    }
  }
  if (useInverses) { 
    for (i=0;i<numSymbols;i++){
      Chars ss = alphabet[i];
      int len = ss.length();
      const char * tt=ss;
      if (len>2 && tt[len-3]=='^' && tt[len-2] =='-' && tt[len-1]=='1')
        continue;
      else baseAlphabet.append(ss);
    }

    int numGens=baseAlphabet.length();
    for (i=0;i<numSymbols;i++){
      Chars ss = alphabet[i];
      int len = ss.length();
      const char * tt=ss;
      if (len>2 && tt[len-3]=='^' && tt[len-2] =='-' && tt[len-1]=='1'){
         char * uu = new char[len+1];
         strcpy(uu,tt);
         uu[len-3]='\0';
         for (j=0;j<numGens;j++){
            if (baseAlphabet[j]==Chars(uu)){
               symbolForColumn.append(-j-1);
               break;
            }
            if (j>numGens) cerr << "Programming error"<< endl;
         }
      }
      else {
         for (j=0;j<numGens;j++){
            if (baseAlphabet[j]==ss){
               symbolForColumn.append(j+1);
               break;
            }
            if (j>numGens) cerr << "Programming error"<< endl;
         }
      }
    }
  }
}

void
DFSAParser::setStateAlphabet(DFSARep * M)
{
  if (M->getNumSymbols()==0){
    // No alphabet has been specified so far.
    if (useInverses) stateAlphabet=baseAlphabet;
    else stateAlphabet=alphabet;
  }
  else stateAlphabet=M->getGeneratorNames();
/*
  for (int j=0;j<stateAlphabet.length();j++) cerr << stateAlphabet[j]<<" ";
  cerr << endl;
*/
}

void 
DFSAParser::parseStatesRec()
{
  Chars buf;
  int ch;
  int i,j;
  if (checkToken(":=") && checkToken("rec") && checkToken('(')){
    while (parses && getToken(buf) && buf!=Chars(")")){
      if (buf==Chars(",")) continue; // "," separates record entries
      else if (buf==Chars("type")){ 
        if (checkToken(":=") && getToken(buf)){
          if (buf==Chars("\"simple\"")) stateType = SIMPLE; 
          else if (buf==Chars("\"words\"")) stateType = WORDS; 
          else if (buf==Chars("\"labeled\"")) stateType = LABELED; 
          else
            parseError("States should be of type \"simple\" or \"words\" or \"labeled\"");
        }
      }
      else if (buf==Chars("size")){
        if (checkToken(":=")) getInt(numStates);
        else parseError("Format error after \"size\" keyword.");
      }
      else if (buf==Chars("alphabet")){
        if (alphabet==0) parseError("Must set FSA alphabet before state labels");
        if (checkToken(":=") && checkToken('[')){
          for (i=0;i<numSymbols;i++){
            getToken(buf);
            if (buf!=alphabet[i]){
              parseError("State alphabet doesn't match FSA alphabet.");
            }
            if (i<numSymbols-1) checkToken(',');
          }
          checkToken(']');
        }
        else parseError("Format error after \"alphabet\" keyword.");
      }
      else if (buf==Chars("labels")) parseStateLabelsRec();
      else if (buf==Chars("names")) parseStateWordLabels();
      else if (buf==Chars("format")){
        if (checkToken(":=") && getToken(buf)){
          if (buf==Chars("\"dense\"")){ 
            parseError("For the time being can only read sparse format");
          }
          else if (buf==Chars("\"sparse\"")){ 
          }
          else 
            parseError("format should be of type \"dense\" or \"sparse\"");
        }
        else parseError("Format error after \"format\" keyword.");
      }
      else if (buf==Chars("setToLabels")){
        if (checkToken(":=") && checkToken('[')){
          getToken(ch);
          if (ch=='[') do{
            getInt(i);
            stateLabelsIndex.append(i);
            checkToken(',');
            getInt(j);
            stateLabelsIndex.append(j);
            checkToken(']');
            getToken(ch);
            if (ch!=',') break;
          } while (parses && checkToken('['));
          if (ch!=']') parseError("List must be terminated with a ]");
        }
      }
    }
  }
}

void DFSAParser::parseStateLabelsRec()
{
  Chars buf;
  int i,numLabels;
  if (checkToken(":=") && checkToken("rec") && checkToken('(')){
    while (parses && getToken(buf) && buf!=Chars(")")){
      if (buf==Chars(",")) continue; // "," separates record entries
      else if (buf==Chars("type")){ 
        if (checkToken(":=")==0 || checkToken("\"identifiers\"")==0)
          parseError("Unexpected input after \"type\" keyword.");
      }
      else if (buf==Chars("size")){
        if (checkToken(":=")==0 || getInt(numLabels)==0 )
          parseError("Unexpected input after \"size\" keyword.");
      }
      else if (buf==Chars("names")){
        if (checkToken(":=") && checkToken('[')){
          VectorOf<Chars> L(numLabels+1);
          for (i=1;i<=numLabels;i++){
            L[i] = getToken();
            getToken(buf);
            if (buf==Chars("]")) break;
            else if (buf!=Chars(",")) 
	      parseError("Comma not found as expected.");
            if (parses==0) break;
          }
          if (parses) stateCharsLabels = L;
        }
      }
    }
  }
}

void DFSAParser::parseStateWordLabels()
{
  int ch;
  if (stateType!=WORDS) 
    parseError
      ("\"names\" keyword should only occur for states of type \"words\"");
  VectorOf<Word> L(numStates+1);
  if (checkToken(":=") && checkToken('[')){
    int i;
    Word w;
    getToken(ch);
    if (ch=='[') do{
      getInt(i);
      checkToken(',');
      Chars ww = getToken();
      if (ww!=Chars("IdWord")){
        ww += ',';
	std::istrstream wwstr(ww);
        WordParser P(wwstr);
        Chars errmesg;
        w= P.parseWord(stateAlphabet,errmesg);
/*
        cerr << i << ":"<<w << endl;
*/
        //@rn Maybe check for error
      }
      L[i]=w;
      checkToken(']');
      getToken(ch);
      if (ch!=',') break;
    } while (parses && checkToken('['));
    if (ch!=']') parseError("List must be terminated with a ]");
  }
  if (parses) stateWordLabels = L;
}

void 
DFSAParser::parseFlags()
{
  Bool DFA=0;
  Chars buf;
  if (checkToken(":=") && checkToken('[')){
    while (parses && getToken(buf) && buf!=Chars("]")){
      if (buf==Chars(",")) continue; // "," separates entries
      else if (buf==Chars("\"DFA\"")) DFA=1;
      else if (buf==Chars("\"minimized\"")) minimized=1;
    }
  }
  if (DFA==0)
    parseError("Cannot read non-deterministic FSA");
}

void
DFSAParser::parseAccepting()
{
  Bool interval=0;
  Bool empty=1;
  int i,j=0;
  int ch;

  if (table==0){
    table = new int * [numStates+1];
    for (i = 0;i <= numStates;i++){
      table[i] = new int[numTransits + 1];
      for  (j=0;j<=numTransits;j++) table[i][j]=0;
    }
  }

  if (parses && checkToken(":=")) checkToken('[');
  while (parses && getInt(i) && 1 <= i && i <=numStates){
    empty=0;
    if (interval && j<i) while (j<i) table[++j][0]=1;
    if (interval && j>i) while (j>i) table[--j][0]=1;
    else table[i][0]=1;
    if (getToken(ch)){
      if (ch==']') break;
      else if (ch==',') interval=0; 
      else if (ch=='.'){ 
        checkToken('.'); // there are two .'s
        interval=1;
        j=i;
      }
    }
  }
  if (empty) checkToken(']');
}

void DFSAParser::parseTableRec()
{
  Bool sparse =1;
  int i,j;
  Chars buf;
  int numTransitions; // this might be useful if we had a sparse way of
                      // storing FSAs 

  if (table==0){
    table = new int * [numStates+1];
    for (i = 0;i <= numStates;i++){
      table[i] = new int[numTransits + 1];
      for  (j=0;j<=numTransits;j++) table[i][j]=0;
    }
  }

  if (checkToken(":=") && checkToken("rec") && checkToken('(')){
    while (parses && getToken(buf) && buf!=Chars(")")){
      if (buf==Chars(",")) continue; // "," separates record entries
      else if (buf==Chars("numTransitions")){ 
        if (checkToken(":=")) getInt(numTransitions);
      }
      else if (buf==Chars("format")){
        if (checkToken(":=") && getToken(buf)){
          if (buf==Chars("\"dense deterministic\""))
            sparse=0;
          else if (buf==Chars("\"sparse\""))
            sparse=1;
          else 
            parseError("format should be of type \"dense deterministic\" or \"sparse\"");
        }
        else parseError("Format error after \"format\" keyword.");
      }
      else if (buf==Chars("transitions")){
        if (checkToken(":=") && checkToken('[')){
          for (i=1;i<=numStates && parses;i++){
            checkToken('[');
            if (sparse){
              int ch;
              getToken(ch);
              if (ch!=']') do{
                if (ch!='[') 
                  parseError("[ expected but not found in transition table");
                getInt(j);
                checkToken(',');
                if (getInt(table[i][j])==0)
                  parseError("Integer expected but not found.");
                checkToken(']');
                getToken(ch);
                if (ch==']') break;
                else if (ch!=',') parseError("Comma expected but not found.");
              } while (parses && getToken(ch));
            }
            else {
              for (j=1;j<=numTransits && parses;j++){
                if (getInt(table[i][j])==0)
                  parseError("Integer expected but not found.");
                if (j<numTransits) checkToken(",");
              }
              checkToken(']');
            }
            if (i<numStates) checkToken(",");
          }
          checkToken(']');
        }
      }
    }
  }
}
