// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of KBmagPackage class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//


#include "KBmagPackage.h"
#include "BlackBox.h"
#include "Generator.h"
#include "MagnusHome.h"
#include "WordParser.h"


const Chars kbflags = " -mt 5 -hf 100 -t 20 -me 200 -ms 1000 -wd ";
const Chars kbflagsa = " -mt 20 -hf 100 -cn 0 -wd ";
const Chars kbflagsb = " -me 262144 -t 500 ";



KBmagPackage::KBmagPackage(const VectorOf<Chars>& genNames,
                           const SetOf<Word>& rels,
                           const WordOrder & ord,int tidyint) : 
  generatorNames(genNames),
  relators(rels),
  order(ord),
  tidyInterval(tidyint),
  error(NO),
  provedAutomatic(NO),
  provedConfluent(NO),
  problemName(tempnam(MagnusTmp::magnusTmp(),"KBmag"))
{
  cdbin =
    Chars("cd ")
    + MagnusHome::magnusHome()
    + "/back_end/black_boxes/kbmag/bin; ";
}

KBmagPackage::KBmagPackage(const VectorOf<Chars>& genNames,
                           const SetOf<Word>& rels,
                           const WordOrder & ord) : 
  generatorNames(genNames),
  relators(rels),
  order(ord),
  tidyInterval(20),
  error(NO),
  provedAutomatic(NO),
  provedConfluent(NO),
  problemName(tempnam(MagnusTmp::magnusTmp(),"KBmag"))
{
  cdbin =
    Chars("cd ")
    + MagnusHome::magnusHome()
    + "/back_end/black_boxes/kbmag/bin; ";
}


KBmagPackage::KBmagPackage(const VectorOf<Chars>& genNames,
                           const SetOf<Word>& rels) :
  generatorNames(genNames),
  relators(rels),
  order("ShortLex",2*genNames.length()),
  tidyInterval(20),
  error(NO),
  provedAutomatic(NO),
  provedConfluent(NO),
  problemName(tempnam(MagnusTmp::magnusTmp(),"KBmag"))
{ 
  cdbin =
    Chars("cd ")
    + MagnusHome::magnusHome()
    + "/back_end/black_boxes/kbmag/bin; ";
}



KBmagPackage::~KBmagPackage()
{
  /* @db we need those files
  Chars command = (Chars)"/bin/rm -f "+problemName + "*";
  system(command);
  */
}


Bool
KBmagPackage::findInputFile(const Chars & fname)
{
  Bool ans = NO;
  ifstream str(fname);
  Chars buf;
  if (str.good()) 
    ans=YES;
  str.close();
  return ans;
}

void 
KBmagPackage::createRWSFile()
{
  ofstream RWS(problemName);
  if ( !(RWS.good()) ){
    error = YES;
    cerr << "Error in KBmagPackage: couldn't open " << problemName << endl; 
    return;
  }
  RWS << problemName << " := rec("<< endl;
  RWS << "  isRWS := true,"<<endl;
  Chars otype = order.getOrderType();
  if (otype==Chars("ShortLex"))
    RWS << "  ordering := \"shortlex\","<<endl;
  else if (otype==Chars("WtLex")||otype==Chars("WtShortLex"))
// for certain functions it is OK if the order isn't accurately represented to
// kbmag
    RWS << "  ordering := \"wtlex\","<<endl;
  else {
    error = YES;
    cerr << 
     "Most of KBmag can currently only be used for shortlex, wtshortlex and wtlex orders." << endl; 
    return;
  }
  RWS << "  tidyint := "<<tidyInterval<<","<<endl;
  RWS << "  generatorOrder := [";
  int i;
  for (i=1;i<=order.getNumSymbols();i++){
    if (i>1) RWS << ",";
    RWS << getGeneratorName(order.getSymbol(i));
  }
  RWS << "],"<< endl;
  RWS << "  inverses := [";
  for (i=1;i<=order.getNumSymbols();i++){
    if (i>1) RWS << ",";
    Generator g=order.getSymbol(i);
    if (order.selfInverse(g))
      // g's an involution, and should be set to be its own inverse
      RWS << getGeneratorName(g);
    else
      RWS << getGeneratorName(inv(g));
  }
  RWS << "],"<< endl;
  if (otype==Chars("WtLex")||otype==Chars("WtShortLex")){
    RWS << "  weight := [";
    int i;
    for (i=1;i<=order.getNumSymbols();i++){
      if (i>1) RWS << ",";
      RWS << order.getWeight(i);
    }
    RWS << "],"<< endl;
  }
  RWS << "  equations := ["<< endl;
  Word id;
  SetIterator<Word> I(relators);
  Bool first=yes;
  while ( !I.done() ) {
    Word w = I.value();
    I.next();
    Word w1,w2;
    order.balancedEquationFromRelator(w,w1,w2);
    if (w1 != w2){
// w1 and w2 could be the same if w involves an involutary generator; 
// if g is involutary and its inverse symbol is not used, then
// the fact that g is involutary is recorded in the inverses
// section rather than in the rewrite rules 
      if (first==0) RWS << ","<<endl;
      first=no;
      RWS << "[";
      printWord(RWS,w1);
      RWS << ",";
      printWord(RWS,w2);
      RWS << "]";
    }
  }
  RWS << "  ]"<< endl;
  RWS << ");"<< endl;

}

Word
KBmagPackage::readWord(istream& istr)
{
  char ch;
  Chars errmesg;
  Word w;

  do {
    ch = istr.peek();
    if (ch!=EOF && isspace(ch)) ch = istr.get();
  } while (ch!=EOF && isspace(ch)); 
  if (ch==EOF) { 
    error=YES;  
    cerr << "Unexpected EOF from KBmagPackage when word was expected." << endl;
    return w;
  }

  Chars ww="";
  do {
    ch = istr.peek();
    if (ch!=EOF && isspace(ch)) ch = istr.get();
  } while (ch!=EOF && isspace(ch)); 
  while (ch!=EOF && !isspace(ch) && ch!=',' && ch!= ']'){
    ch=istr.get();
    ww += ch; 
    ch=istr.peek();
  }
  if (ch==EOF) { 
    error=YES;  
    cerr << "Unexpected EOF from KBmagPackage when word was expected." << endl;
  }
  else if (ww!=Chars("IdWord")){
    ww += ',';
    //@njz
    //    istrstream wwstr(ww);
    std::istrstream wwstr(ww);
    //
    WordParser P(wwstr);
    Chars errmesg;
    w= P.parseWord(generatorNames,errmesg);
  }
  return w;
}

void
KBmagPackage::printWord(ostream& ostr,const Word& w)
{
  int l = w.length();
  // The point of printing out the word a generator at a time is to
  // make sure that asterisks appear as separators .
  // With the FGGroup::printWord function (at the moment) spaces are being used
  // as separators instead.
  // Also we don't want to use Group functions in this file
  //@rn We MUST fix this.
  if ( l == 0 ) ostr << "IdWord";
  else {
    ostr << getGeneratorName(w[0]) ;
    for ( int i = 1; i < l; i++ ) 
      ostr << "*" << getGeneratorName(w[i]) ;
  }
}

Trichotomy
KBmagPackage::autgroup()
{
  BlackBox theAutGroup(cdbin+" ./autgroup "+problemName);

  Trichotomy ans=dontknow;
  if (!findInputFile(problemName)) createRWSFile();

  if (error) { 
    warn("Couldn't create input files.");
    return dontknow;
  }
  if (theAutGroup.start("#Running Knuth-Bendix Program:")==0) {
    warn("Unexpected output from autgroup, bailing from function");
    error = YES;
    return dontknow;
  } 
  Chars buf;
  while ( theAutGroup.fromCommand() >> buf ){
    if (buf==Chars("succeeded.")){  provedAutomatic=YES; ans=yes;}
    else if (buf==Chars("failed.")) ans = no;
  }
  return ans;

}

Trichotomy
KBmagPackage::gpaxioms()
{
  Chars command;
  Chars otype = order.getOrderType();
  if (otype==Chars("ShortLex"))
    command = cdbin + " ./gpaxioms " + problemName;
  else if (otype==Chars("WtLex")||otype==Chars("WtShortLex"))
    command = cdbin + " ./gpaxioms -wtlex " + problemName;
  else {
    warn
("Axiom checking can only be done for the shortlex, wtshortlex and wtlex orders.");
    return dontknow;
  }
  BlackBox theGpAxioms(command);
  Trichotomy ans=dontknow;
  if (!(findInputFile(problemName))) createRWSFile();
  if (!(findInputFile(problemName) && findInputFile(problemName+".diff2")
      && findInputFile(problemName+".wa") && findInputFile(problemName+".gm"))){
    cerr << "Cannot check axioms until word differences,"<<endl;
    cerr << "word acceptor and multipliers have been computed."<<endl;
    return dontknow;
  }
  theGpAxioms.start();
  Chars buf;
  while ( theGpAxioms.fromCommand() >> buf ){
    if (buf==Chars("succeeded.")){ provedAutomatic = YES; ans = yes;}
    else if (buf==Chars("#Relation") && 
      ( theGpAxioms.fromCommand() >> buf ) && buf==Chars("check") &&
      ( theGpAxioms.fromCommand() >> buf ) && buf==Chars("fails")) ans=no;
  }
  return ans;

}

Trichotomy
KBmagPackage::gpcheckmult()
{
  Chars command;
  Chars otype = order.getOrderType();
  if (otype==Chars("ShortLex"))
    command = cdbin + " ./gpcheckmult " + problemName;
  else if (otype==Chars("WtLex") || otype==Chars("WtShortLex"))
    command = cdbin + " ./gpcheckmult -ow " + problemName;
  BlackBox theGpCheckMult(command);
  Trichotomy ans=dontknow;
  if (!findInputFile(problemName+".gm")){
    cerr << "Cannot find general multiplier."<<endl;
    return dontknow;
  }
  theGpCheckMult.start();
  Chars buf;
  while ( theGpCheckMult.fromCommand() >> buf ){
    if (buf==Chars("succeeded.")) ans = yes; 
    else if (buf==Chars("incorrect")) ans = no; 
  }
  if (ans==no && (otype==Chars("WtLex")||otype==Chars("WtShortLex"))){
  // we shall recompute the difference machine manually 
    DiffMachine D=differenceMachine(2);
    ifstream str(problemName+".wg");
    if (str.good()){ 
      char ch;
      while ((ch=str.get())!=EOF && ch != '['); 
      Word w1, g, w2;
      do {
        while ((ch=str.get())!=EOF && isspace(ch)); 
        if (ch != '[') cerr << "Read "<<ch<<" where [ was expected."<< endl;
        w1=readWord(str);
        while ((ch=str.get())!=EOF && isspace(ch)); 
        if (ch != ',') cerr << "Read "<<ch<<" where comma was expected."<< endl;
        g=readWord(str);
        while ((ch=str.get())!=EOF && isspace(ch)); 
        if (ch != ']') cerr << "Read "<<ch<<" where ] was expected."<< endl;
        w2 = w1*g;
        D.rewrite(w2,order);
        cerr << "Correction with "<< w1 << "," << g <<","<< w2 << endl;
        D.addDifferencesEqn(w1*g,w2,order);
        D.addDifferencesEqn(w2,w1*g,order);
        while ((ch=str.get())!=EOF && isspace(ch)); 
      } while (ch==',');
      str.close();
    } 
    else {
      cerr << "Cannot find file of corrections for difference machine."<< endl;
      return dontknow;
    }
    setDifferenceMachine(D,2);
  }
  return ans;
}

Trichotomy
KBmagPackage::gpgenmult(Bool eqcheck)
{
  Chars command;
  if (eqcheck) command = cdbin + " ./gpgenmult " +problemName;
  else command = cdbin + " ./gpgenmult -ns " +problemName;
  BlackBox theGpGenMult(command);
  Trichotomy ans=dontknow;
  if (!(findInputFile(problemName+".diff2") &&
              findInputFile(problemName+".wa") )){
    cerr << "Cannot construct general multiplier"<<endl;
    cerr << "until word differences and word acceptor have been computed."<<endl;
    return dontknow;
  }
  theGpGenMult.start();
  Chars buf;
  while ( theGpGenMult.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
    else if (buf==Chars("#Equation")) ans = no; 
  }
  return ans;
}

Trichotomy
KBmagPackage::gpmakefsa(Bool eqcheck)
{
  Chars command;
  if (eqcheck) command = cdbin + " ./gpmakefsa " +problemName;
  else command = cdbin + " ./gpmakefsa -ns " +problemName;
  BlackBox theGpMakeFSA(command);
  Trichotomy ans=dontknow;
  if (!findInputFile(problemName+".diff2")){
    cerr << "Cannot make FSA's until word differences have been computed."<<endl;
    return dontknow;
  }
  theGpMakeFSA.start();
  Chars buf;
  while ( theGpMakeFSA.fromCommand() >> buf ){
    if (buf==Chars("succeeded.")) ans = yes;
  }
  return ans;

}

Trichotomy
KBmagPackage::gpwa()
{
  BlackBox theGpWA(cdbin + " ./gpwa " +problemName);
  Trichotomy ans=dontknow;
  if (!findInputFile(problemName+".diff2")){
    cerr << "Cannot make word acceptor until word differences have been computed."<<endl;
    return dontknow;
  }
  theGpWA.start();
  Chars buf;
  while ( theGpWA.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans=yes; 
  }
  return ans;
}

Trichotomy
KBmagPackage::kbprog(int haltingfactor,int tidyint,int maxeqns,
                                           int maxstates)
{
  char * flags = new char[100];
  if (maxstates && maxeqns)
    sprintf(flags," -mt 5 -hf %d -t %d -me %d -ms %d -wd ",
                 haltingfactor,tidyint,maxeqns,maxstates);
  else if (maxstates)
    sprintf(flags," -mt 5 -hf %d -t %d -ms %d -wd ",
                 haltingfactor,tidyint,maxstates);
  else if (maxeqns)
    sprintf(flags," -mt 5 -hf %d -t %d -me %d -wd ",
                 haltingfactor,tidyint);
  Chars Flags = flags; 
  BlackBox theKBProg(cdbin + 
    " ./kbprog "+flags +problemName);
  Trichotomy ans=dontknow;
  if (!findInputFile(problemName)) createRWSFile();
  Chars otype = order.getOrderType();
  if (otype!=Chars("ShortLex") && otype !=Chars("WtLex")){
    error = yes;
    warn("kbprog cannot yet be run for orders other than shortlex and wtlex.");
    return dontknow; 
  }

  if (error) { 
    warn("Couldn't create input files.");
    return dontknow;
  }
  theKBProg.start();
  Chars buf;
  int count=0;
  while ( theKBProg.fromCommand() >> buf ){
    if (buf==Chars("confluent.")) provedConfluent=yes; 
    else if (buf==Chars("computed.")) count++; 
    else if (buf==Chars("#Warning:")) ans=no;
// We want to return no if the following message is output. 
//#Warning: the inverse of a non-trivial word-difference is trivial.
//#Try running kbprog for longer.
  }
  if (ans==dontknow && count==2) ans=yes;
  return ans;
}

GroupDFSA 
KBmagPackage::wordAcceptor()
{
  GroupDFSA WA("",generatorNames,order);
  ifstream istr(problemName+".wa");
  if ( !(istr.good()) ){
    cerr << "Unable to find word acceptor. Has one been calculated?"
         <<endl;
  }
  else WA.readFrom(istr);
  return WA;
}

GroupDFSARep 
KBmagPackage::wordAcceptorRep()
{
  GroupDFSARep WARep("",generatorNames,order);
  ifstream istr(problemName+".wa");
  if ( !(istr.good()) ){
    cerr << "Unable to find word acceptor. Has one been calculated?"
         <<endl;
  }
  else WARep.readFrom(istr);
  return WARep;
}

void 
KBmagPackage::setWordAcceptor(const GroupDFSA & WA)
{
  ofstream ostr(problemName+".wa");
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for WA."<<endl;
  else WA.printOn(ostr);
}

void 
KBmagPackage::setWordAcceptor(const GroupDFSARep & WARep)
{
  ofstream ostr(problemName+".wa");
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for WA."<<endl;
  else WARep.printOn(ostr);
}

GenMult 
KBmagPackage::generalMultiplier()
{
  GenMult GM(generatorNames,order);
  ifstream istr(problemName+".gm");
  if ( !(istr.good()) ){
    cerr << "Unable to find general multiplier. Has one been calculated?"
         <<endl;
  }
  else GM.readFrom(istr);
  return GM;
}

GenMultRep 
KBmagPackage::generalMultiplierRep()
{
  GenMultRep GMRep(generatorNames,order);
  ifstream istr(problemName+".gm");
  if ( !(istr.good()) ){
    cerr << "Unable to find general multiplier. Has one been calculated?"
         <<endl;
  }
  else GMRep.readFrom(istr);
  return GMRep;
}

void 
KBmagPackage::setGeneralMultiplier(const GenMult & GM)
{
  ofstream ostr(problemName+".gm");
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for GM."<<endl;
  else GM.printOn(ostr);
}

void 
KBmagPackage::setGeneralMultiplier(const GenMultRep & GMRep)
{
  ofstream ostr(problemName+".gm");
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for GM."<<endl;
  else GMRep.printOn(ostr);
}

DiffMachine 
KBmagPackage::differenceMachine(int i)
{
  DiffMachine Diff(generatorNames,order);
  Chars fileName;
  if (i==1)
    fileName=problemName+".diff1";
  else if (i==2)
    fileName=problemName+".diff2";
  else cerr << "There are only two difference machines! Set i=1 or 2"<< endl;

  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Unable to find the difference machine. Has one been calculated?"
         <<endl;
  }
  else Diff.readFrom(istr);
  return Diff;
}

DiffMachineRep 
KBmagPackage::differenceMachineRep(int i)
{
  DiffMachineRep DiffRep(generatorNames,order);
  Chars fileName;
  if (i==1)
    fileName=problemName+".diff1";
  else if (i==2)
    fileName=problemName+".diff2";
  else cerr << "There are only two difference machines! Set i=1 or 2"<< endl;

  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Unable to find the difference machine. Has one been calculated?"
         <<endl;
  }
  else DiffRep.readFrom(istr);
  return DiffRep;
}

void 
KBmagPackage::setDifferenceMachine(const DiffMachine & Diff,int i)
{
  Chars fileName;
  if (i==1)
    fileName=problemName+".diff1";
  else if (i==2)
    fileName=problemName+".diff2";
  else cerr << "There are only two difference machines! Set i=1 or 2"<< endl;
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for difference machine."<<endl;
  else Diff.printOn(ostr);
}

void 
KBmagPackage::setDifferenceMachine(const DiffMachineRep & DiffRep,int i)
{
  Chars fileName;
  if (i==1)
    fileName=problemName+".diff1";
  else if (i==2)
    fileName=problemName+".diff2";
  else cerr << "There are only two difference machines! Set i=1 or 2"<< endl;
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file for difference machine."<<endl;
  else DiffRep.printOn(ostr);
}

// these three functions have the same code, but since the arguments
// are not directly related, I don't currently see how else to do it.
// Of course the FSA hierarchy should really have a minimization function
Bool KBmagPackage::minimize(DFSA & D)
{
  Bool ans = NO;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
  D.printOn(ostr);
  BlackBox theFsaMin(cdbin+" ./fsamin "+fileName);
  theFsaMin.start();
  Chars buf;
  while ( theFsaMin.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }

  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0){
    system(command);
    return NO; // minimization has failed for some reason
  }
  fileName += ".min";
  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
    system(command);
    return NO;
  }
  D.readFrom(istr);
  system(command);
  return YES;
}

Bool KBmagPackage::minimize(GroupDFSA & D)
{
  Bool ans = 0;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
  D.printOn(ostr);
  BlackBox theFsaMin(cdbin+" ./fsamin "+fileName);
  theFsaMin.start();
  Chars buf;
  while ( theFsaMin.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }
  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0) {
    system(command);
    return NO; // minimization has failed for some reason
  }
  fileName += ".min";
  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
    system(command);
    return NO;
  }
  D.readFrom(istr);
  system(command);
  return YES;
}

Bool KBmagPackage::minimize(DFSARep & D)
{
  Bool ans = NO;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
  D.printOn(ostr);
  BlackBox theFsaMin(cdbin+" ./fsamin "+fileName);
  theFsaMin.start();
  Chars buf;
  while ( theFsaMin.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }
  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0){
     system(command);
     return NO; // minimization has failed for some reason
  }
  fileName += ".min";
  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
    system(command);
    return NO;
  }
  D.readFrom(istr);
  system(command);
  return YES;
}

Bool KBmagPackage::minimize(GroupDFSARep & D)
{
  Bool ans = NO;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  ofstream ostr(fileName);
  if ( !(ostr.good()) )
    cerr << "Cannot open output file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
  D.printOn(ostr);
  BlackBox theFsaMin(cdbin+" ./fsamin "+fileName);
  theFsaMin.start();
  Chars buf;
  while ( theFsaMin.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }
  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0){
    system(command);
    return NO; // minimization has failed for some reason
  }
  fileName += ".min";
  ifstream istr(fileName);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName<<
                 " in function KBmagPackage::minimize."<<endl;
    system(command);
    return NO;
  }
  D.readFrom(istr);
  system(command);
  return YES;
}

Bool KBmagPackage::gpcomp(GroupDFSA & D1,GroupDFSA & D2,GroupDFSA & D3)
{
  Bool ans = NO;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  Chars fileName1 = fileName + "1";
  Chars fileName2 = fileName + "2";
  Chars fileName3 = fileName + "3";
  ofstream ostr1(fileName1);
  if ( !(ostr1.good()) )
    cerr << "Cannot open output file "<< fileName1<<
                 " in function KBmagPackage::gpcomp."<<endl;
  D1.printOn(ostr1);
  ofstream ostr2(fileName2);
  if ( !(ostr2.good()) )
    cerr << "Cannot open output file "<< fileName2<<
                 " in function KBmagPackage::gpcomp."<<endl;
  D2.printOn(ostr2);
  BlackBox theGpComp(cdbin+" ./gpcomp "+fileName1 +" "+fileName2+" "+ fileName3);
  theGpComp.start();
  Chars buf;
  while ( theGpComp.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }
  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0){
    system(command);
    return NO; // minimization has failed for some reason
  }
  ifstream istr(fileName3);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName3<<
                 " in function KBmagPackage::gpcomp."<<endl;
    system(command);
    return NO;
  }
  D3.readFrom(istr);
  system(command);
  return YES;
}

Bool KBmagPackage::gpcomp(GroupDFSARep & D1,GroupDFSARep & D2,GroupDFSARep & D3)
{
  Bool ans = NO;
  Chars fileName(tempnam(MagnusTmp::magnusTmp(),"KBmag"));
  Chars fileName1 = fileName + "1";
  Chars fileName2 = fileName + "2";
  Chars fileName3 = fileName + "3";
  ofstream ostr1(fileName1);
  if ( !(ostr1.good()) )
    cerr << "Cannot open output file "<< fileName1<<
                 " in function KBmagPackage::gpcomp."<<endl;
  D1.printOn(ostr1);
  ofstream ostr2(fileName2);
  if ( !(ostr2.good()) )
    cerr << "Cannot open output file "<< fileName2<<
                 " in function KBmagPackage::gpcomp."<<endl;
  D2.printOn(ostr2);
  BlackBox theGpComp(cdbin+" ./gpcomp "+fileName1 +" "+fileName2+" "+ fileName3);
  theGpComp.start();
  Chars buf;
  while ( theGpComp.fromCommand() >> buf ){
    if (buf==Chars("computed.")) ans = yes; 
  }
  Chars command = (Chars)"/bin/rm -f " + fileName + "*";
  /* this is the command to read the files created in this function,
     but we don't execute it just yet */

  if (ans == 0){
    system(command);
    return NO; // minimization has failed for some reason
  }
  ifstream istr(fileName3);
  if ( !(istr.good()) ){
    cerr << "Cannot open input file "<< fileName3<<
                 " in function KBmagPackage::gpcomp."<<endl;
    system(command);
    return NO;
  }
  D3.readFrom(istr);
  system(command);
  return YES;
}

