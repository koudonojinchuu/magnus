// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of RKBPackage class.
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
// Next implementation steps:
//
// * Deal with the many @rn, @rn:@sr, @sr comments here and in RKBPackage.C


#ifndef _RKBPackage_H_
#define _RKBPackage_H_

#include <iomanip.h>

#include "BlackBox.h"
#include "Vector.h"
#include "Chars.h"
#include "Set.h"
#include "Word.h"
#include "DiffMachine.h"
#include "KBMachine.h"
#include "MagnusHome.h"
#include "WordOrder.h"


// RKBPackage is a class associated with a run of the RKBPackage program.
// The constructor creates the 3 necessary files and starts the
// program up. The destructor halts the program and deletes the 3
// files. The program may also be halted and restarted at an intermediate
// stage. 
// Initially the group relations are turned into rewrite rules.
// The function runKB runs a KnuthBendix process on the rules, according
// to certain parameters.
// At any stage, using the rules so far, any word in the group generators
// can be rewritten, or a random process approximates the growth rate of
// the group.
// A KnuthBendixMachine or a WordDifferenceMachine can be initialised on the 
// process, at any stage.
// The first allows word reduction in the group independently of the RKBPackage,
// the second is the first step in the construction of the automata of an 
// automatic group. The word differences themselves can also be computed.


class RKBPackage {
public:

  //@rn:@sr Explain what these do.


  RKBPackage(const VectorOf<Chars>& genNames, const SetOf<Word>& relators) 
  : theRKBPackage(Chars("cd ")
		  + MagnusHome::magnusHome()
		  + "/back_end/black_boxes/rkbp/bin; ./rkbp"
		  ),
    index(1+genNames.length()),index_inv(1+genNames.length()),
    genNumber(2*genNames.length())
    {
      VectorOf<int> order(2*genNames.length());
      for (int i=1;i<=genNames.length();i++){ order[2*i-2]=i; order[2*i-1]= -i;}
      WordOrder wsl("ShortLex",order);
      initialize(genNames, relators,wsl);
    }
  // starts up the program rkbp, with the generators and relators of
  // a group as input.
  // An initial rewrite system is created from the group relations,
  // using the shortlex ordering.
  // No calculation is yet done with the program.

  RKBPackage(const VectorOf<Chars>& genNames, const SetOf<Word>& relators,
    const WordOrder & word_order)
  : theRKBPackage(Chars("cd ")
						+ MagnusHome::magnusHome()
						+ "/back_end/black_boxes/rkbp/bin; ./rkbp"
						),
     index(1+genNames.length()),index_inv(1+genNames.length()),
     genNumber(2*genNames.length())
  {
    initialize(genNames, relators,word_order);
  }
  ~RKBPackage();
  // Takes care of shutting down rkbp, and unlinking temp files/pipes.

  Chars getName() const { Chars s = problemName; return s;}

  void runKB(int MaxLen, int numIterations, int saveLHSLen, int saveRHSLen);

  // The rewrite system currently stored is modified by running the Knuth
  // Bendix process with the 4 specified parameters,
  // MaxLen, numIterations, saveLHSLen, saveRHSLen.
  // The (parameterised) Knuth Bendix process procedes as follows.
  // Since it is parameterised it will terminate.
  
  // All left hand sides of existing rules are compared for
  // overlaps. (An overlap is a string of the form abc where
  // a,b,c are strings and ab and bc are left hand sides,
  // The length of the overlap is defined to be the sum of the lengths
  // of the strings a,b, and c.)
  // Any overlap of length at most maxLen is reduced in two different ways,
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
  // the set of rules by the Knuth Bendix process. 
  // If it is set to -1, the process runs until no
  // more overlaps are found of length less than maxLen.


  float* growthRate(int maxLen, int numTrials, int seed);
  // The growth rate of the group is estimated using a random
  // procedure. The output is an array of floating point numbers;
  // the i-th entry is the estimated number of elements of
  // length i in the group. Since this is a random (mean) estimate
  // it need not be an integer.
  // The estimate is made over a number (numTrials) of trials.
  // An integer seed needs to be set.

  void saveToFiles();
  // the rewrite system currently stored in the process is saved to files
  // so that it may be read by magnus functions and not only accessible
  // to the program rkbp.
  // The name of the files are predetermined, and are not settable.

  void printRules(ostream& str = cout);
  // The rules currently held within the rewrite system are printed
  // in a user-readable form.

  void readRule(istream& str, Word& left, Word& right);
  // The rewrite rule left->right is read from the stream str.
  // This function (together with saveToFiles) is used to transfer the
  // rewrite rules from the program rkbp to other magnus functions.
  // The rules can only be read from the file (with name rulesFilename())
  // to which they have been written by the program rkbp.
  // Hence ifstream must have been initialised on the file with that name.


  void oneOffRewrite(Word& w);
  // Using the rkbp itself to reduce a word is possible, but it requires
  // telling rkbp to change modes. This is equivalent to the statement:
  // { enterRewriteMode(); rewrite(Word& w); quitRewriteMode(); }

  void enterRewriteMode();
  // Tells the rkbp to enter rewrite mode. This is preparation for reducing
  // more than a few words at once.

  void quitRewriteMode();
  // Tells the rkbp to leave rewrite mode.

  void rewrite(Word& w);
  // Use this if there are many words to be rewritten.
  // Bracket calls to rewrite with enterRewriteMode and quitRewriteMode.
  // Do not call other functions from this class between calls to rewrite,
  // because the rkbp is in the wrong mode.
  // @rn:@sr Check the mode flag in the other functions, and report error.

  SetOf<Word> wordDifferences();
  // The set of "word differences" associated with the current rewrite system
  // is computed and returned.

  // If u->v is a rewrite rule, then where u(i) is the maximal prefix
  // of u of length at most i and v(i) is the maximal prefix of v of length at
  // most i, the reudction (according to the rewrite system) of the
  // word u(i)^-1*v(i) is defined to be a word difference.
  // The set of word differences associated with u->v is defined to be
  // the set of all word differences as above for all integers i.
  // The set of all word differences associated with the rewrite system is
  // the union of all the sets of word differences associated with the rules
  // in the system.
  
  // The set of word differences is relevant to the automatic group algorithms.

  DiffMachine differenceMachine(const SetOf<Word> & differences);

  DiffMachine differenceMachine() {return differenceMachine(wordDifferences());}

  KBMachine KnuthBendixMachine();

  VectorOf<Chars> getGeneratorNames() const { return generatorNames; }

  const char* rulesFilename() const { return rulesFileName; }
  // Returns the name of the file in which the rewrite rules discovered by
  // the rkbp are stored.
  // @rn:@sr Probs with simultaneous access? Encapsulation violation.

  int currentNumOfRules() const { return numRules; }
  // Returns the number of rules in the rewrite system currently
  // stored by the rkbp.

  Bool isProvedConfluent() const { return provedConfluent;}
  // Returns true if the rewrite system currently stored  by the rkbp
  // is proved to be confluent. (However it could be confluent without us
  // knowing it.)

  Bool rulesReduced(Word * lhs,Word * rhs,int numOfrules); 

  Bool sanityCheck() const { return !error; }
  // Returns TRUE iff this RKBPackage is functioning properly.

  Chars getGeneratorName(Generator g) const {
     int i = ord(g);
     if (i>0) return generatorNames[i-1];
     else return(generatorNames[-i-1]+"^-1");
  }



private:

  //@rn:@sr Explain what the rest of these are used for.

  // Data members:

  //  state flags:

  Bool error;
  // Set TRUE when any error in the rkbp is detected.

  Bool runningProcess;
  // Set TRUE when the rkbp is generating rules. @rn:@sr right? Wrong.
  // Actually, it reports whether or not the rkbp is actually running or not
  // (it might have been aborted). It could be the need for this is removed
  // by the existence of sanityCheck - will check this out @sr

  Bool rewriteMode;
  // Set TRUE when the rkbp is in rewrite mode.

  Bool filesOutOfDate;
  // Some functions read the rules from files, and so need to know if
  // the files don't contain the current rules.
  // If so they update the files.
  Bool provedConfluent;
  // Returns true if the rewrite system currently stored  by the rkbp
  // is proved to be confluent. (However it could be confluent without us
  // knowing it.)


  int numRules;
  int lenLeftMin;
  int lenLeftMax;
  int lenLeftTotal;
  int lenRightMin;
  int lenRightMax;
  int lenRightTotal;

  char problemName[64];     // Problem name supplied to rkbp
//  char* problemName;     // Problem name supplied to rkbp
  char inFileName[64];      // Name of pipe bound to stdin of rkbp
  char outFileName[64];     // Name of pipe bound to stdout of rkbp
  char rulesFileName[64];   // Name of file to which rkbp writes rules, 
                            //and from which they can be read
  char subsysFileName[64];  // Name of file to which rkbp saves subsystem info,
                            // and from which that can be read
  char sysFileName[64];     // Name of file to which rkbp saves system info
                            // and from which that can be read

  VectorOf<Chars> generatorNames;
  WordOrder order;
  VectorOf<int> index; // index[i] gives the integer rkbp uses to label the 
   // i-th generator (an integer in the range 
   // 0..2*generatorNames.length()-1). index[0] is meaninless.
  VectorOf<int> index_inv; // index_inv[i] gives the integer rkbp uses to 
  //label the inverse of the i-th generator (an integer in the 
  //range 0..2*generatorNames.length()-1). index_inv[0] is meaningless.
  VectorOf<int> genNumber; // genNumber[i] gives the number (as returned by the
 // Generator function ord)  attached to the generator which rkbp labels with i.
  BlackBox theRKBPackage;


  // Methods:

  void initialize(const VectorOf<Chars>& genNames, const SetOf<Word>& relators,
                  const WordOrder & word_order);
    // Initialize the process on a specified set of generator names
    // and relations, with a specified word order.
  void restartProcess();
  void haltProcess();
  void createSystemFile();
  void setWeightsAndLevels(ofstream & rkbp_system);
  void createSubsystemFile();
  void createRulesFile(const SetOf<Word>& relators);
  void skipToNextPrompt();
  void skipToImmediatePrompt();
  void printWord(ostream& str,const Word& w);
  Word readWord(istream& str);
  void readSummary();
  float* readProbe(int maxLen);
  DiffMachine convertToDiffMachine(DiffMachineRep * R)const ;
  KBMachine convertToKBMachine(KBMachineRep * R) const;
  void buildDifferenceMachine(DiffMachineRep & D, 
                         const SetOf<Word> & differences);
  void buildKnuthBendixMachine(KBMachineRep & K); 
};

#endif  
