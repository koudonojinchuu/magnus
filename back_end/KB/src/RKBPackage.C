// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of RKBPackage class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// XI/94: RN: Repaired violations of encapsulation.
//            Replaced direct linking of named pipes with BlackBox.
//
// * 8/96 Dmitry B. made porting to gcc 2.7.2.
//
//
// Next implementation steps:
//
// * @rn:@sr 
//   Sarah, I may have completely subverted your intentions with
//   restartProcess() and haltProcess(), as well as the various status flags.
//   I also couldn't decipher the error reporting protocol, so I didn't do
//   anything with it.
//
// * @sr With a reference to setw this won't currently compile in ncl,
//   (@rn:@sr compiler version?)
//   so I've temporarily replaced such lines, which may lead to array overflow.


#include "RKBPackage.h"
#include "MagnusHome.h"
#include "WordParser.h"
#include "DiffMachineRep.h"

void
RKBPackage::initialize
    (const VectorOf<Chars>& genNames, const SetOf<Word>& relators,
                                         const WordOrder & word_order)
{
  error      = 0;

  generatorNames = genNames;
  order = word_order;

  char* tmpStr = tempnam(MagnusTmp::magnusTmp(),"");
  strcpy(problemName,tmpStr);
  free(tmpStr);

  //@rn Without making a file named problemName, these may not be unique:

  sprintf(sysFileName,"%s.sys", problemName);
  sprintf(rulesFileName,"%s.rl1", problemName);
  sprintf(subsysFileName,"%s.sb1", problemName);

  createSystemFile(); 
  createSubsystemFile(); 
  createRulesFile(relators); 
  if (error) { 
    warn("Couldn't create input files; bailing from RKBPackage constructor");
    return;
  }

  if (theRKBPackage.start("Rutgers Knuth-Bendix Package, Version 1.25")==0) {
    warn("Couldn't find rkbp 1.25 banner; bailing from RKBPackage constructor");
    error = 1;
    return;
  }
  theRKBPackage.toCommand() << "input " << problemName << endl;
  // Puts rkbp into input mode.

  provedConfluent = 0;
  skipToNextPrompt();
  skipToImmediatePrompt();
  runningProcess = 1;
  rewriteMode = 1;
  filesOutOfDate = 0;
  // The program only knows what it has just read from the files.
}


RKBPackage::~RKBPackage()
{
  char* command = new char[strlen(problemName)+ 20];
  haltProcess();
  sprintf(command,"/bin/rm -f %s", sysFileName);
  system(command);
  sprintf(command,"/bin/rm -f %s", rulesFileName);
  system(command);
  sprintf(command,"/bin/rm -f %s", subsysFileName);
  system(command);
  delete command;
}


void 
RKBPackage::createSystemFile()
{
  int i,j;
  ofstream rkbp_system(sysFileName);
  if ( !(rkbp_system.good()) ){
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << sysFileName << endl; 
    return;
  }
  rkbp_system << "system" << endl;
  rkbp_system << "word_type:  group" << endl;
  rkbp_system << "ngens:  " << generatorNames.length() <<  endl;
  rkbp_system << "naming_convention:  none" << endl;
  rkbp_system << "numbering_convention:  none" << endl;
  rkbp_system << "inverse_convention:  none" << endl;
  rkbp_system << "generators:" << endl;

  index[0]=index_inv[0]=0;
  // Output our group generators as semigroup generators:
  for ( i = 0; i < order.getNumSymbols(); i++ ){
    j=order.getSymbolIndex(i+1);
    genNumber[i]=j;
    if (j>0) index[j]=i;
    else index_inv[-j]=i;
    Generator g(j);
    rkbp_system << getGeneratorName(g) << endl;
  }
// at the end, list the inverses of involutary generators, or rkbp isn't happy
  int k=order.getNumSymbols();
  if (k<2*generatorNames.length()-1) 
  for ( i = 0; i < order.getNumSymbols(); i++ ){
    j=order.getSymbolIndex(i+1);
    Generator g(j);
    if (j>0 && order.selfInverse(g)){
      genNumber[k]= -j;
      index_inv[j]= k;
      rkbp_system << getGeneratorName(g)<<"^-1" << endl;
      k++;
    }
  }
  rkbp_system << "inverse_pairs:" << endl;
  for ( i = 1; i <= generatorNames.length(); i++ ){
    Generator g(i);
    rkbp_system <<  getGeneratorName(g) << "  ";
    rkbp_system << getGeneratorName(g)<<"^-1" << endl;
  }
  setWeightsAndLevels(rkbp_system);
  rkbp_system << "nsub:  1" << endl;
  rkbp_system << "subsystems:" << endl << subsysFileName << endl;;
}

void
RKBPackage::setWeightsAndLevels (ofstream & rkbp_system)
{
  Chars oType = order.getOrderType();
  int i;
  if (oType==Chars("ShortLex")){
    rkbp_system << "weight_convention:  constant" << endl;
    rkbp_system << "level_convention:  constant" << endl;
  }
  else if (oType==Chars("WtShortLex")){
    rkbp_system << "weight_convention:  none" << endl;
    rkbp_system << "weights:"<<endl;
    for (i = 1; i <= order.getNumSymbols(); i++ ){
      rkbp_system << order.getWeight(i) << endl;
    }
// To keep rkbp happy also list weights for the inverses of involutions
    for (i = 1; i <= order.getNumSymbols(); i++ ){
      if (order.selfInverse((order.getSymbol(i))))
        rkbp_system << order.getWeight(i) << endl;
    }
    rkbp_system << "level_convention:  constant" << endl;
  }
  else if (oType==Chars("InvPairWreath")){
    rkbp_system << "weight_convention:  constant" << endl;
    rkbp_system << "levels:"<<endl;
    for (i = 1; i <= generatorNames.length(); i++ )
      rkbp_system <<  i << endl << i << endl;
  }
  else 
    cerr << 
  "Can deal only with word orders of types Shortlex, WtShortLex, InvPairWreath"
         << endl;
}


void 
RKBPackage::createSubsystemFile()
{
  ofstream rkbp_subsystem(subsysFileName);
  if ( !(rkbp_subsystem.good()) ){
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << sysFileName << endl; 
    return;
  }
  rkbp_subsystem << "subsystem" << endl;
  rkbp_subsystem << "rule_type:  subword" << endl;
  rkbp_subsystem << "index_type:  automaton" << endl;
  rkbp_subsystem << "external:  len_numbers" << endl;
  rkbp_subsystem << "compressed:  no" << endl;
  rkbp_subsystem << "rule_file:  " << rulesFileName  << endl;
}


void 
RKBPackage::createRulesFile(const SetOf<Word>& relators)
{
  Word w;
  Word w1,w2;
  Generator g;
  int i, l;
  ofstream rkbp_rules(rulesFileName);
  if ( !(rkbp_rules.good()) ){
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << sysFileName << endl; 
    return;
  }
  SetIterator<Word> I(relators);
  while ( !I.done() ) {
    w = I.value();
    order.balancedEquationFromRelator(w,w1,w2);
    if (w1 != w2) {
// w1=w2 can happen if w contains an involutary generator
      printWord(rkbp_rules, w1);
      rkbp_rules << " ";
      printWord(rkbp_rules, w2);
      rkbp_rules << endl;
    }
    I.next();
  }
// if g is an involutary generator whose inverse is not in the alphabet of
// the word order then all the rewrite rules exclude that symbol.
// Then we need to explicitly add the rule g^-1 -> g
  for (i=1;i<=order.getNumSymbols();i++){
    g=order.getSymbol(i);
    if (order.selfInverse(g)){
      printWord(rkbp_rules,(Word)inv(g));
      rkbp_rules << " ";
      printWord(rkbp_rules,(Word)g);
      rkbp_rules << endl;
    }
  }
}


void
RKBPackage::printWord(ostream& ostr,const Word& w)
// We need a function in this class to print words because none of
// the notations acceptable to rkbp is exactly comparably to a magnus notation.
// We communicate with rkbp now using the len_numbers notation, since this seems
// to be less likely to conflict with magnus assumptions (such as what is 
// allowable as a generator) than the len_names notation (which was used in
// earlier versions of this function).
{
  int l = w.length();
  ostr << l << " ";
  for (int i=0;i<l;i++)
    ostr << " "<< (ord(w[i])>0? index[ord(w[i])]:index_inv[-ord(w[i])]) ;
}


Word
RKBPackage::readWord(istream& istr)
{
  int len;
  int i,j;
  char ch;
  Chars woid;
  Chars errmesg;
  Word w;
  Generator g;

  do {
    ch = istr.peek();
    if (ch!=EOF && isspace(ch)) ch = istr.get();
  } while (ch!=EOF && isspace(ch)); 
  if (ch==EOF) { 
    error=1;  
    cerr << "Unexpected EOF from RKBPackage when word was expected." << endl;
    cerr << "Process aborted." << endl;
    haltProcess();
    return w;
  }
  istr >> len;
  if (len==0) return w;

  VectorOf<Generator> v(len);
  
  for (i=0;i<len;i++){
    do {
      ch = istr.peek();
      if (ch!=EOF && isspace(ch)) ch = istr.get();
    } while (ch!=EOF && isspace(ch)); 
    if (ch==EOF) { 
      error=1;  
      cerr << "Unexpected EOF from RKBPackage when word was expected." << endl;
      cerr << "Process aborted." << endl;
      haltProcess();
      return w;
    }
    istr >> j;
    g = (Generator)(genNumber[j]);
    if (ord(g)<0 && order.selfInverse(inv(g))) g=inv(g);
    v[i] = g;
  }
  w = v;
  return w;
}


void
RKBPackage::readRule(istream& istr, Word& left, Word& right)
{
  left = readWord(istr);
  if ( error ) return;
  right = readWord(istr);
}


void
RKBPackage::restartProcess()
{
  if ( error ) { 
    cerr << "Error in RKBPackage. Cannot restart process." << endl; 
    return; 
  }
  if ( runningProcess ) return;

  // Look to see if the input files are there.

  ifstream sysstr(sysFileName);
  if ( sysstr.good() ) sysstr.close(); 
  else {
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << sysFileName << endl; 
    return;
  }

  ifstream subsysstr(subsysFileName); 
  if ( subsysstr.good() ) subsysstr.close(); 
  else {
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << subsysFileName << endl; 
    return;
  }

  ifstream rulesstr(rulesFileName);
  if ( rulesstr.good() ) rulesstr.close();
  else {
    error = 1;
    cerr << "Error in RKBPackage: couldn't open " << rulesFileName << endl; 
    return;
  }


  if (theRKBPackage.restart("Rutgers Knuth-Bendix Package, Version 1.25")==0) {
    warn("Couldn't find rkbp 1.25 banner; bailing from RKBPackage constructor");
    error = 1;
    return;
  }
  theRKBPackage.toCommand() << "input " << problemName << endl;
  // Puts rkbp into input mode.

  provedConfluent = 0;
  skipToNextPrompt();
  skipToImmediatePrompt();
  runningProcess = 1;
  filesOutOfDate = 0;
  // The program only knows what it has just read from the files.
}


void
RKBPackage::haltProcess()
{
  if ( runningProcess == 0 ) return;

  theRKBPackage.toCommand() << "quit" << endl;
  runningProcess = 0;

  //@rn Should this be a pedestrian "bye, now" to rkbp, or should more
  //    drastic action like kill -9 be an option?
  //    That may be too heavy-handed, since it seems that closing the i/o
  //    pipes to/from rkbp makes it go away.
  //    Maybe BlackBox should have a method for that, aside from its
  //    destructor.
}


void
RKBPackage::skipToNextPrompt()
{
  const int bufsize = 128;
  char buf[bufsize];

// @sr The next line has been temporarily replaced by the one following it.
//  while ((theRKBPackage.fromCommand() >> setw(bufsize) >> buf) && strcmp(buf,">:")!=0) ;

  while ( (theRKBPackage.fromCommand() >> buf) && strcmp(buf,">:") != 0 );
  if ( strcmp(buf,">:") != 0 ) {
    // The output stream from RKBPackage must have broken, since the RKBPackage
    // always eventually returns a prompt after any input.
    error = 1;
    cerr << "RKBPackage has crashed. Process aborted." << endl;
    haltProcess();
  }
}


void
RKBPackage::skipToImmediatePrompt()
{
  //@dp const int bufsize = 128;
  //@dp char buf[bufsize];
  Chars buf;
  // @sr The next line has been temporarily replaced by the one following it.
  //  theRKBPackage.fromCommand() >> setw(bufsize)>> buf;
  theRKBPackage.fromCommand() >> buf;
  //if ( strcmp(buf, ">:") != 0 ) {
  if ( buf != Chars(">:") ) {
    error = 1;
    cerr << "Unexpected output from RKBPackage." << endl;
    cerr << buf << " read when prompt was expected."<< endl;
    cerr << "Process aborted." << endl;
    haltProcess();
  }
}


void 
RKBPackage::readSummary()
{
  const int bufsize = 128;
  char buf[bufsize];
  char ch;
  Bool beginning=1;
  // @sr The next line has been temporarily replaced by the one following it.
  // while (theRKBPackage.fromCommand() >> setw(bufsize) >> buf){
  while ( theRKBPackage.fromCommand() >> buf ) {
    if ( strcmp(buf, ">:") == 0 ){
      if (beginning==1) continue;
// this is to deal with excess prompts which may not have been read yet.
      else break;
    }
    else if (strcmp(buf,"nrules")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> numRules;
    }
    else if (strcmp(buf,"llmin")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenLeftMin;
    }
    else if (strcmp(buf,"llmax")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenLeftMax;
    }
    else if (strcmp(buf,"lltotal")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenLeftTotal;
    }
    else if (strcmp(buf,"lrmin")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenRightMin;
    }
    else if (strcmp(buf,"lrmax")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenRightMax;
    }
    else if (strcmp(buf,"lrtotal")==0) { 
      theRKBPackage.fromCommand() >> ch;  // read '='
      theRKBPackage.fromCommand() >> lenRightTotal;
    }
    beginning = 0;
  }
}


float* RKBPackage::readProbe(int maxLen)
{
  const int bufsize = 128;
  char buf[bufsize];
  char ch;
  int i, k;
  Bool trialsPrompt = 0;
  Bool lengthPrompt = 0;
  Bool seedPrompt = 0;
  Bool beginning=1;
  float* A = new float[maxLen + 1];
  // @sr The next line has been temporarily replaced by the one following it.
  // while (theRKBPackage.fromCommand() >> setw(bufsize) >> buf){
  while ( theRKBPackage.fromCommand() >> buf ) {
    if ( strcmp(buf, ">:") == 0 ) {
      if (beginning==1) continue; // this is to deal with spare prompts which
            // might have been left lying around unread after a previous command 
            // (the rkbp seems not to be totally consistent about the
            // number of prompts it prints out)
      else if ( trialsPrompt == 0 || lengthPrompt == 0 ||
           seedPrompt == 0 ) {
        error = 1;
        cerr << "Unexpected output from RKBPackage after probe command was input." << endl;
        cerr << "Expected prompts trials: length: and seed: not found." 
             << endl;
        cerr << "Process aborted." << endl;
        haltProcess();
      }
      break;
    }
    else if ( strcmp(buf, "trials:") == 0 ) { 
      trialsPrompt = 1;
    }
    else if ( strcmp(buf, "length:") == 0 ) { 
      lengthPrompt=1;
    }
    else if ( strcmp(buf, "seed:") == 0 ) { 
      seedPrompt = 1;
      break;
    }
    beginning=0;
  }
  if ( error ) return 0;
  for ( i = 0; i <= maxLen; i++ ){
    theRKBPackage.fromCommand() >> k;
    if ( k != i ) {
      error = 1;
      cerr << "Unexpected output from RKBPackage after probe command was input." << endl;
      cerr << "Expected integer "<< i << "but read integer "<< k << endl;
      cerr << "Process aborted." << endl;
      haltProcess();
      return 0;
    }
    else theRKBPackage.fromCommand() >> A[i];
  }
  skipToNextPrompt();
  return A;
}


void
RKBPackage::saveToFiles()
{
  if ( error ) { cerr << "Error in RKBPackage" << endl; return; }
  if ( filesOutOfDate == 0 ) return;
  // No point in saving to files
  // if the info hasn't changed - this could take time.

  restartProcess();
  theRKBPackage.toCommand() << "output " << problemName << endl;
  const int bufsize = 128;
  char buf[bufsize];
  Bool beginning=1;

  Bool formatPrompt = 0;

  // @sr The next line has been temporarily replaced by the one following it.
  // while (theRKBPackage.fromCommand() >> setw(bufsize) >> buf){

  while ( theRKBPackage.fromCommand() >> buf ) {
    if ( strcmp(buf, ">:") == 0 ){
      if (beginning==1) continue; // this is to deal with spare prompts which
            // might have been left lying around unread after a previous command 
            // (the rkbp seems not to be totally consistent about the
            // number of prompts it prints out)
      else error = 1;
      cerr << "Unexpected output from RKBPackage." << endl;
      cerr << 
    "Prompt read unexpectedly after input of command output."<<endl;
      cerr << "Process aborted." << endl;
      haltProcess();
      break;
    }
    else if ( strcmp(buf, "format:") == 0 ){
      formatPrompt = 1;
      theRKBPackage.toCommand() << "len_numbers" << endl;
      break;
    }
    beginning = 0; // We're past the stage of picking up excess >:'s.
  }

  skipToNextPrompt();
  filesOutOfDate = 0;
}

void
RKBPackage::printRules(ostream & ostr)
{
  if ( error ) { cerr << "Error in RKBPackage" << endl; return; }
  if ( filesOutOfDate) saveToFiles();
  ifstream rkbp_rules(rulesFileName);
  Word left, right;

  ostr << "RKBP reduction rules:-" << endl;
  for (int i = 1; i <= numRules; i++ ) {
    readRule(rkbp_rules, left, right);
    printWord(ostr,left);
    ostr << "->";
    printWord(ostr,right);
    ostr << endl;
  }
  rkbp_rules.close();
}


void RKBPackage::runKB(int maxLen, int numIterations, int saveLHSLen,
                         int saveRHSLen)

// This function runs the Knuth Bendix process on the current system 
// of rules, according to the 4 specified parameters.

// All left hand sides of existing rules are compared for
// overlaps. An overlap is a string of the form abc where
// a,b,c are strings and ab and bc are left hand sides,  
// The length of the overlap is defined to be the sum of the lengths
// of the strings a,b, and c.
// Any overlap of length at most maxLen is reduced in two different,
// first using the rule with left hand side ab, and then so
// far as possible using the remaining rules,
// second using the rule with left hand side bc, and then so 
// far as possible using the remaining rules.
// If the results are different a new rule is found.
// That rule is saved and existing rules which are direct
// consequences of it are deleted provided the lengths of its
// left and right hand sides are less than the bounds
// saveLHSLen and saveRHSLen.

// The parameter numIterations determines how many passes are made through
// the set of rules. If it is set to -1, the process runs until no
// more overlaps are found of length less than maxLen.

// A summary of the results is obtained from the process.
// If the maximum length of a left hand size is at most one half
// of maxLen + 1 then the system of rules must be confluent.

{
  if ( error ) { cerr << "Error in RKBPackage. Cannot run KB." << endl; return; }
  restartProcess();
  theRKBPackage.toCommand() << "kb " << maxLen << " " << numIterations << " "
                      << saveLHSLen << " " << saveRHSLen << endl;   

  skipToImmediatePrompt();
  theRKBPackage.toCommand() << "summary" << endl;
  readSummary();
  if (2*lenLeftMax-1 <=maxLen && numIterations== -1)
    provedConfluent = 1;
  filesOutOfDate = 1;
}


float* RKBPackage::growthRate(int maxLen, int numTrials, int seed)
{
  if ( error ) { 
    cerr << "Error in RKBPackage. Cannot compute growth rate." << endl; 
    return 0; 
  }
  restartProcess();
  theRKBPackage.toCommand() << "probe" << endl;
  theRKBPackage.toCommand() << numTrials << endl;
  theRKBPackage.toCommand() << maxLen << endl;
  theRKBPackage.toCommand() << seed << endl;
  return readProbe(maxLen);
}


void RKBPackage::enterRewriteMode()
{
  if ( error ) { 
    cerr << "Error in RKBPackage. Cannot enter rewrite mode." << endl; 
    return; 
  }
  restartProcess();
  theRKBPackage.toCommand() << "rewrite_words" << endl;

  Bool inputPrompt = 0;
  Bool formatPrompt = 0;
  Bool typePrompt = 0;
  const int bufsize = 128;
  char buf[bufsize];
  Bool beginning=1;

  // @sr The next line has been temporarily replaced by the one following it.
  // while (theRKBPackage.fromCommand() >> setw(bufsize) >> buf){

  while ( theRKBPackage.fromCommand() >> buf ) {
    if ( strcmp(buf, ">:") == 0 ){
      if (beginning==1) continue; // this is to deal with spare prompts which
            // might have been left lying around unread after a previous command 
            // (the rkbp seems not to be totally consistent about the
            // number of prompts it prints out)
      error = 1;
      cerr << "Unexpected output from RKBPackage." << endl;
      cerr << 
    "Prompt read unexpectedly after input of command rewrite_words."<<endl;
      cerr << "Process aborted." << endl;
      haltProcess();
      break;
    }
    else if ( strcmp(buf, "input") == 0 ){
      inputPrompt = 1; 
      theRKBPackage.toCommand() << "none" << endl;
     // No input file for words, reading from stdin.
    }
    else if ( strcmp(buf, "format") == 0 ){
      formatPrompt = 1;
      theRKBPackage.toCommand() << "len_numbers" << endl;
    }
    else if ( strcmp(buf, "type") == 0 ){
      typePrompt = 1;
      theRKBPackage.toCommand() << "subword" << endl;   
    }
    else if ( strcmp(buf, "word") == 0 ) {
      if ( inputPrompt == 0 || formatPrompt == 0 ||
           typePrompt == 0 ) {
        error = 1;
        cerr << 
    "Unexpected output from RKBPackage after input of command rewrite_words." << endl;
        cerr << 
    "Expected output strings input format and word not found." << endl;
        cerr << "Process aborted." << endl;
        haltProcess();
      }
      break;
    }
    beginning = 0; // We're past the stage of picking up excess >:'s.
  }
  rewriteMode = 1;
}


void RKBPackage::quitRewriteMode()
{
  Word triv;
  printWord(theRKBPackage.toCommand(), triv);
  theRKBPackage.toCommand() << endl;
  skipToImmediatePrompt();
  rewriteMode = 0;
}


void RKBPackage::oneOffRewrite(Word& w)
{
  enterRewriteMode();
  rewrite(w);
  quitRewriteMode();
}


void RKBPackage::rewrite(Word& w)
{
  if ( error ) { 
    cerr << "Error in RKBPackage. Cannot rewrite." << endl; return; }
  if (w.length()==0) return; 
// @sr The rkbp doesn't accept the empty word as input for its rewrite function,
// but instead uses it to terminate a list of words to be rewritten.
// So it seems impossible to deal with a rewrite system where the empty
// word is not reduced. Could we be interested in such a system?
  if ( rewriteMode == 0 ) { oneOffRewrite(w); return; }
  printWord(theRKBPackage.toCommand(), w);
  theRKBPackage.toCommand() << endl;
  w = readWord(theRKBPackage.fromCommand());
}

Bool RKBPackage::rulesReduced(Word * lhs,Word * rhs,int numOfRules) 
{


  int i,j,len;
  Word w,ww;
  enterRewriteMode();

  for (i=1;i<=numOfRules;i++){

    len = lhs[i].length();
    
    if (len==0) {
      cerr << "Left hand side of length zero. Can't handle this."<< endl;
// @sr I may be wrong about this, but it seems to me that the rkbp cannot
// handle a rules system where the empty word is not reduced.
      error=1;
      haltProcess();
      return 0;
    }

    ww = lhs[i].initialSegment(len-1); 
    w=ww; 
    rewrite(w); 
    if (w!=ww){  
      cerr << "Prefix of left hand side of rule number "
                                       <<i<<" not reduced."<< endl;
      quitRewriteMode();
      return 0; 
    }    
   
    ww = lhs[i].terminalSegment(len-1);
    w=ww;
    rewrite(w);
    if (w!=ww){    
      cerr << "Suffix of left hand side of rule number "
                                       <<i<<" not reduced."<< endl;
      quitRewriteMode();
      return 0;
    }
 
    w = rhs[i];
    if (w.length()>0){
      rewrite(w);
      if (w!=rhs[i]){   
        cerr << "Right hand side of rule number "
                                     <<i<<" not reduced."<< endl;
        quitRewriteMode();
        return 0;
      }
    }
    for (j=i+1;j<=numOfRules;j++){
      if (lhs[i]==lhs[j]){
        cerr << "Rules number "<< i<<" and "<<j<<
                   " have same left hand side."<< endl;
        quitRewriteMode();
        return 0;
      }
    }
  }
  quitRewriteMode();
}


SetOf<Word> RKBPackage::wordDifferences()
{
  SetOf<Word> differences;
  if ( error ) { 
    cerr << "Error in RKBPackage. Cannot compute word differences."<< endl; 
    return differences; }
  restartProcess();
  saveToFiles();   // Make sure the current rules are in the rules file.

  enterRewriteMode();

  Word trivial;
  Word w;

  int len1, len2, len;
  int i, j, k;
  char ch;

  differences.adjoinElement(trivial);
  // Putting the identity in at the beginning saves computing the
  // last word difference for every rule.

  ifstream rkbp_rules(rulesFileName);
  Word left, right;

  for ( i = 1; i <= numRules; i++ ) {

    readRule(rkbp_rules, left, right);
    if ( error ) break;
 
    len1 = left.length();
    len2 = right.length();
    if ( len1 >= len2 ) len = len1;
    else len = len2;
 
    w = trivial;
    for ( j = 0; j < len - 1; j++ ) {
      if ( j < len1 && j < len2 ) w = inv(left[j]) * w * right[j];
      else if ( j < len1 ) w = inv(left[j]) * w;
      else if ( j < len2 ) w = w * right[j];

      rewrite(w);
      if ( error ) break;
      differences.adjoinElement(w);
    }
    if ( error ) break;

    w = trivial;
    for ( j = 0; j < len - 1; j++ ) {
      if ( j < len1 && j < len2 ) w = inv(right[j]) * w * left[j];
      else if ( j < len2 ) w = inv(right[j]) * w;
      else if ( j < len1 ) w = w * left[j];

      rewrite(w);
      if ( error ) break;
      differences.adjoinElement(w);
    }
    if ( error ) break;
  }
  rkbp_rules.close();
  quitRewriteMode();
  if ( error ) differences.removeAllElements();

  return differences;
}

typedef DFSARep::State State;

DiffMachine
RKBPackage::convertToDiffMachine(DiffMachineRep * R) const
{
  DiffMachine D(R);
  return D;
}


DiffMachine
RKBPackage::differenceMachine(const SetOf<Word> & differences)
{
  DiffMachineRep *  Dp = new DiffMachineRep(generatorNames,order);
  buildDifferenceMachine(*Dp,differences);
  DiffMachine DD= convertToDiffMachine(Dp);
  return DD;
}


void 
RKBPackage::buildDifferenceMachine(DiffMachineRep & D,const SetOf<Word> & differences)
{
  State s;
  int i,j,k;
  int numGens = generatorNames.length();
  int numDiffs = differences.cardinality();
  D.setNumStates(numDiffs);
  SetIterator<Word> I(differences);
  SetOf<Word> reducedDifferences; 
    // in case the set differences contains some non-reduced words we make a 
    // reduced copy. It should make the search for differences quicker to 
    // have them in a set.
  Word w,ww;
  s=2;
// the identity must be the first word difference, 
// so has already been dealt with 
  while (!I.done()){
    w = I.value();
    rewrite(w);
    if (w.length()>0 && reducedDifferences.contains(w)==0){
      reducedDifferences.adjoinElement(w);
      D.setDifference(s,w);
      s++;
    }
    I.next();
  }
  numDiffs= s-1;
  D.setNumStates(numDiffs);
  enterRewriteMode(); if (error){
    cerr << "Error in RKBPackage. Cannot build difference machine."; 
    return;
  }
  for (s=1;s<=numDiffs;s++){
    D.setCategory(s,0); // irrelevant, but maybe it's better to have something set
    w = D.getDifference(s);
    for (i=1;i<=numGens;i++){
      Generator x(i); 
      Generator xx= (order.selfInverse(x)? x : inv(x));
       // xx is set to the inverse of x unless x is an involution, when it is 
       // set to be x

      for (j=1;j<=numGens;j++){
        Generator y(j); 
        Generator yy= (order.selfInverse(y) ? y: inv(y));
       // yy is set to the inverse of y unless y is an involution, when it is 
       // set to be y

        ww = xx*w*y;
        rewrite(ww); if (error) break;
        if (ww.length()==0) k=1; // the identity is automatically in the set
        else if (reducedDifferences.contains(ww))
          for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
        else k=0;
        D.setTarget(s,x,y,k); 

        if (yy!=y){
          ww = xx*w*yy;
          rewrite(ww); if (error) break;
          k=0;
          if (ww.length()==0) k=1;
          else if (reducedDifferences.contains(ww))
            for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
          else k=0;
          D.setTarget(s,x,yy,k);
        }

        if (xx==x) continue;

        ww = x*w*y;
        rewrite(ww); if (error) break;
        if (ww.length()==0) k=1;
        else if (reducedDifferences.contains(ww))
          for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
        else k=0;
        D.setTarget(s,xx,y,k);
        
        if (yy!=y){
          ww = x*w*yy;
          rewrite(ww); if (error) break;
          if (ww.length()==0) k=1;
          else if (reducedDifferences.contains(ww))
            for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
          else k=0;
          D.setTarget(s,xx,yy,k);
        }
      }

      ww = xx*w;
      rewrite(ww); if (error) break;
      if (ww.length()==0) k=1;
      else if (reducedDifferences.contains(ww))
        for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
      else k=0;
      D.setTargetOn1stString(s,x,k);

      if (xx==x) continue;

      ww = x*w;
      rewrite(ww); if (error) break;
      if (ww.length()==0) k=1;
      else if (reducedDifferences.contains(ww))
        for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
      else k=0;
      D.setTargetOn1stString(s,xx,k);
    }
    for (j=1;j<=numGens;j++){
      Generator y(j); 
      Generator yy=(order.selfInverse(y)? y : inv(y));

      ww = w*y;
      rewrite(ww); if (error) break;
      if (ww.length()==0) k=1;
      else if (reducedDifferences.contains(ww))
        for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
      else k=0;
      D.setTargetOn2ndString(s,y,k);

      if (yy==y) continue;

      ww = w*yy;
      rewrite(ww); if (error) break;
      if (ww.length()==0) k=1;
      else if (reducedDifferences.contains(ww))
        for (k=2;k<=numDiffs;k++){ if (D.getDifference(k)==ww) break;}
      else k=0;
      D.setTargetOn2ndString(s,yy,k);
    }
  }
  quitRewriteMode();
}

KBMachine
RKBPackage::convertToKBMachine(KBMachineRep * R) const
{
  KBMachine K(R);
  return K;
}

KBMachine
RKBPackage::KnuthBendixMachine()
{
  KBMachineRep *  Kp = new KBMachineRep(generatorNames,order,numRules);
  buildKnuthBendixMachine(*Kp);
  KBMachine KK= convertToKBMachine(Kp);
  return KK;
}

//@rn This wants a reduced rewrite system, doesn't it?

void RKBPackage::buildKnuthBendixMachine(KBMachineRep & K)

// The algorithm comes from Sims: Computation with Finitely Presented Groups,
// p 118.
// The states correspond to all prefixes of the left hand sides of rules.
// If s corresponds to a left hand side, all arrows from s go to s itself.
// If s is only a proper prefix U of a left hand side, the arrow from s on x
// goes to the state t which corresponds to the maximal suffix of Ux which is a
// prefix of a left hand side. 
// Sims identifies three subsets of the set of edges:
// E1 the set of edges from a state which corresponds to a lhs to itself,
// E2 the set of all edges from states which correspond only to prefixes 
// of lhs's
// E3 the set of edges in E2 from a state s to a state t, where s corresponds
// to a prefix U of a lhs and t to Ux, also a prefix of a lhs.
//
// This implementation differs from Sims' description is two ways
// (a) The categories are set so that states which correspond to left hand 
// sides have category 0. This is opposite to Sims, who accepts words which can
// be rewritten.
// (b) For efficiency, those states which correspond to lhs's have been set
// to be fail states. That means no transitions from them are defined, 
// thus the E1 edges of Sims correspond to non-edges here.

{
  int numGens = K.getNumGenerators();
  int numSyms = K.getNumSymbols();
  int len;
  int maxlen = 0;
  int i, j;
  State s, t, u;
  Generator x;
  Word left, right;

  // Before we do anything else we read in the rules from the rkbp.

  // @rn:@sr The functionality of this block might be an RKBPackage method.
  //         Then a KBMachine might be initialized only with some sort of
  //          `rewrite system' (a helper class?), and there would be less
  //         cross-talk between RKBPackage and this class.
  {
    saveToFiles(); // Make sure the current rules are in the rules file.
    
    ifstream rkbp_rules(rulesFilename());

    // First read the rules from the file
    Word left, right;
    
    for( i = 1; i <= K.getNumRules(); ++i ) {

      readRule(rkbp_rules, left, right);
      K.setLHS(i,left);
      K.setRHS(i,right);
      len = (K.getLHS(i)).length();
      if ( len > maxlen ) maxlen = len;


      // We check for an error after each rule has been read, in case,
      // for instance, there aren't as many rules in the file as there should be.
      // @rn:@sr Haven't the rules already been written to the file you're
      //         reading? What does that file now have to do with
      //         sanityCheck(), which reflects communication with rkbp?
      if ( !sanityCheck() ) {
        cerr << "Error in reading "<< 
                i << "-th rule." << endl;
        cerr << "Cannot build Knuth Bendix reduction machine." << endl;
        rkbp_rules.close();
// for debugging
        char command[100];
        sprintf(command,"cp %s rules",rulesFilename()); 
        system(command);
        K.setNumRules(0);
        return;
      }
    }
    rkbp_rules.close();
  }

/*
  if (rulesReduced(lhs,rhs,numRules)==FALSE){
    cerr << "Rules not reduced. Cannot build index automaton." << endl;
    return;
  }
  else cerr << "Have checked that rules are reduced."<< endl;
*/
  
  // Now we actually build the automaton.
  // First create the start state.
  K.setNumStates(1);
  K.setLength(K.startState(), 0);
  K.setCategory(K.startState(), 1);
  
  // First the edges in E1 and E3.
  for( i = 1; i <= numRules; ++i ) {
    s = K.startState(); 
    Word w = K.getLHS(i);

    len = w.length();
    for( j = 0; j < len; ++j ) {
      x = w[j];
      if ( (t = K.target(s, x)) == 0 ) { 
        t=K.newState();
        K.setTarget(s, x, t);  // An edge in E3
        K.setLength(t, j + 1);
        K.setRuleIndex(t, i);
        if ( j == len - 1 ) K.setCategory(t,0);
        else {
          K.setCategory(t, 1);
        }
      }
      s = t;
    }
    
    // s is the left hand side of a rule. 
    // In Sims' algorithm we would now define an E1 edge from s to s 
    // on each symbol. Instead we define no transitions from s.
    // So that part of Sims' algorithm is not seen here.
    // This has implications below.
    // For Sims' algorithm assumes that all arrows from a fail state
    // lead to the fail state.

  }

  // Now we add the edges in E2 \ E3.
  s = K.startState();
  for( j = 1; j <= numSyms; ++j ) {
    if ( K.targetInt(s, j) == 0 ) K.setTargetInt(s, j, s); 
  }

  // @rn Why does s start at 2? I assume because getLength(1) == 0, so
  //     there is nothing to do.

// This loop may look inefficient but we MUST pass through the states in increasing
// order of length.

  for( len = 1; len <= maxlen; ++len ) {
    for( s = 2; s <= K.getNumStates(); ++s ) {
      if ( K.getLength(s) != len ) continue;
      if (K.category(s)==0) continue;
      Word w = K.getLHS(K.getRuleIndex(s));

      // t is set equal to the last actual state on a path from 
      // the start labelled by w.
      // Then the target of s under y is t, if t is a fail state,
      // or the target of t under y otherwise.

      t = K.startState();
      for (j=1;j<len;++j){
        if ((u=K.target(t,w[j]))!=0) t=u;
        else break;
      } 

      for( j = 1; j <= numSyms; ++j) {
        if ( K.targetInt(s, j) == 0){
          if (u=K.targetInt(t,j)) K.setTargetInt(s, j, u);
          else K.setTargetInt(s, j, t);
        }
      }
    }
  }

}

